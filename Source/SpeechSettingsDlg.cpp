// SpeechSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wordaid2.h"
#include "SpeechSettingsDlg.h"
#include "settingindex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpeechSettingsDlg dialog


CSpeechSettingsDlg::CSpeechSettingsDlg(CWnd* pParent /*=NULL*/)
	: CTreePage(CSpeechSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpeechSettingsDlg)
	m_nPitch = 0;
	m_nSpeed = 0;
	m_nVolume = 0;
	//}}AFX_DATA_INIT
	m_bModified = FALSE;
	m_bNoSAPI = FALSE;
}


void CSpeechSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpeechSettingsDlg)
	DDX_Control(pDX, IDC_SLIDER_VOL, m_ctrlVolume);
	DDX_Control(pDX, IDC_SLIDER_RATE, m_ctrlSpeed);
	DDX_Control(pDX, IDC_SLIDER_PITCH, m_ctrlPitch);
	DDX_Text(pDX, IDC_EDIT_PITCH, m_nPitch);
	DDV_MinMaxInt(pDX, m_nPitch, 100, 200);
	DDX_Text(pDX, IDC_EDIT_RATE, m_nSpeed);
	DDV_MinMaxInt(pDX, m_nSpeed, 50, 300);
	DDX_Text(pDX, IDC_EDIT_VOL, m_nVolume);
	DDV_MinMaxInt(pDX, m_nVolume, 1, 99);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpeechSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CSpeechSettingsDlg)
	ON_BN_CLICKED(IDC_BTN_TEST, OnBtnTest)
	ON_CBN_SELCHANGE(IDC_COMBO_VOICE, OnSelchangeComboVoice)
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_EN_KILLFOCUS(IDC_EDIT_PITCH, OnKillfocusEditPitch)
	ON_EN_KILLFOCUS(IDC_EDIT_RATE, OnKillfocusEditRate)
	ON_EN_KILLFOCUS(IDC_EDIT_VOL, OnKillfocusEditVol)
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpeechSettingsDlg message handlers

BOOL CSpeechSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ctrlPitch.SetRange(100, 200, TRUE);
	m_ctrlPitch.SetTicFreq(10);
	m_ctrlSpeed.SetRange(50, 300, TRUE);
	m_ctrlSpeed.SetTicFreq(25);
	m_ctrlVolume.SetRange(1, 99, TRUE);
	m_ctrlVolume.SetTicFreq(10);

	if(!m_ctrlSpeaker.Create("Speaker",WS_CHILD,CRect(0,0,1,1),this,2))
	{
		m_bNoSAPI = TRUE;

		DisablePage();	
	}
	else
	{
		InitialisePage();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpeechSettingsDlg::DisablePage()
{
	m_ctrlVolume.EnableWindow(FALSE);
	m_ctrlSpeed.EnableWindow(FALSE);
	m_ctrlPitch.EnableWindow(FALSE);
}

void CSpeechSettingsDlg::DialogSelected()
{
	if(!m_bNoSAPI)
	{
		InitialisePage();
	}
	else
	{
		DisableControls();

		CString strError = _T("");

		strError.LoadString(IDS_NO_SAPI);

		AfxMessageBox(strError, MB_OK|MB_ICONEXCLAMATION);
	}
}




void CSpeechSettingsDlg::InitialisePage()
{
	if(!m_bNoSAPI)
	{
		m_bSpeechOn = atoi((const char*)m_Settings->GetSetting(USE_SPEECH));
		m_bSpeakAll = atoi((const char*)m_Settings->GetSetting(SPEAK_ALL));
		m_nPitch = atoi((const char*)m_Settings->GetSetting(SPEECH_PITCH));
		m_nSpeed = atoi((const char*)m_Settings->GetSetting(SPEECH_SPEED));
		m_nVolume = atoi((const char*)m_Settings->GetSetting(SPEECH_VOLUME));

		m_ctrlPitch.SetPos(m_nPitch);
		m_ctrlSpeed.SetPos(m_nSpeed);
		m_ctrlVolume.SetPos(m_nVolume);
		
		if (!m_bSpeechOn)	//speech not selected so disable everything else
		{
			DisableControls();
		}

		if (m_ctrlSpeaker)
		{
			int no_of_voices = m_ctrlSpeaker.GetVoiceCount();
			m_ctrlVoice = (CComboBox*)GetDlgItem(IDC_COMBO_VOICE);

			//WA0027 clear list first
//			if (m_ctrlVoice->GetCount() > 0)
//				m_ctrlVoice->ResetContent();
			// List the names of the voices in the list box
			for (int i = 0;  i < no_of_voices;  i++)
			{
				// Get each voice from the TextSpeaker. 
				CVoice voice;
				voice.AttachDispatch(m_ctrlSpeaker.GetVoice(i));
//WA0027 start				m_ctrlVoice->AddString(voice.GetVoiceName());
				CString name = voice.GetManufacturerName();
				if (name == "IBM")
					;	//do nothing
				else
					m_ctrlVoice->AddString(voice.GetVoiceName());
//WA0027 end
				voice.ReleaseDispatch();
			}
			// select the current voice 
			int pos = m_ctrlVoice->FindString(-1,m_Settings->GetSetting(SPEECH_VOICE));
			if (pos != -1 ) // voice found
			{
				m_ctrlVoice->SetCurSel(pos);

				// create this voice
				IDispatch* voice = m_ctrlSpeaker.GetVoice(pos);
				if (!m_ctrlSpeaker.SelectVoice(voice))
					TRACE("can't select voice");
				voice->Release();
			}
		}
	}
	else
	{
		DisableControls();
	}

	UpdateData(FALSE);

}
void CSpeechSettingsDlg::OnChkSpeechOn() 
{
	// TODO: Add your control notification handler code here
	if (m_bSpeechOn || m_bNoSAPI)
	{
		m_bSpeechOn = FALSE;
		DisableControls();
	}
	else
	{
		m_bSpeechOn = TRUE;
		if (!m_ctrlSpeaker)
		{
			if (!m_ctrlSpeaker.Create("Speaker",WS_CHILD,CRect(0,0,1,1),this,2))
				m_bNoSAPI = TRUE;	//WA0027

			int no_of_voices = m_ctrlSpeaker.GetVoiceCount();
			m_ctrlVoice = (CComboBox*)GetDlgItem(IDC_COMBO_VOICE);

			//WA0027 clear list first
//			if (m_ctrlVoice->GetCount() > 0)
//				m_ctrlVoice->ResetContent();

			// List the names of the voices in the list box
			for (int i = 0;  i < no_of_voices;  i++)
			{
				// Get each voice from the TextSpeaker. 
				CVoice voice;
				voice.AttachDispatch(m_ctrlSpeaker.GetVoice(i));
//WA0027 start				m_ctrlVoice->AddString(voice.GetVoiceName());
				CString name = voice.GetManufacturerName();
				if (name == "IBM")
					;
				else
					m_ctrlVoice->AddString(voice.GetVoiceName());
//WA0027 stop
				voice.ReleaseDispatch();
			}
			// select the current voice 
			int pos = m_ctrlVoice->FindString(-1,m_Settings->GetSetting(SPEECH_VOICE));
			m_ctrlVoice->SetCurSel(pos);
			// create this voice
			IDispatch* voice = m_ctrlSpeaker.GetVoice(pos);
			if (!m_ctrlSpeaker.SelectVoice(voice))
				TRACE("can't select voice");

			voice->Release();

		}
		EnableControls();
	}
	SetModified();	
}

void CSpeechSettingsDlg::OnBtnTest() 
{
	// TODO: Add your control notification handler code here
	CString str = _T("");
	str.LoadString(IDS_TEST_STRING);
	if (m_ctrlSpeaker)
		m_ctrlSpeaker.Speak(str);
}

void CSpeechSettingsDlg::OnSelchangeComboVoice() 
{
	// TODO: Add your control notification handler code here
	int pos = m_ctrlVoice->GetCurSel();

	IDispatch* voice = m_ctrlSpeaker.GetVoice(pos);
	if (!m_ctrlSpeaker.SelectVoice(voice))
	{
		TRACE("can't select voice");
		voice->Release();
		return;
	}
	voice->Release();
				
	SetModified();	
}


void CSpeechSettingsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nSBCode)
	{
	case TB_PAGEDOWN:
	case TB_PAGEUP:
	case TB_THUMBTRACK:
	{
		if (pScrollBar == (CScrollBar*)&m_ctrlVolume)
		{
			m_ctrlSpeaker.SetVolume(m_ctrlVolume.GetPos());
			m_nVolume = m_ctrlVolume.GetPos();
		}
		if (pScrollBar == (CScrollBar*)&m_ctrlSpeed)
		{
			m_ctrlSpeaker.SetSpeed(m_ctrlSpeed.GetPos());
			m_nSpeed = m_ctrlSpeed.GetPos();
		}
		if (pScrollBar == (CScrollBar*)&m_ctrlPitch)
		{
			m_ctrlSpeaker.SetPitch(m_ctrlPitch.GetPos());
			m_nPitch = m_ctrlPitch.GetPos();
		}
	}
	default:
		;
	}
	UpdateData(FALSE);
	SetModified();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSpeechSettingsDlg::DisableControls()
{
	m_ctrlSpeed.EnableWindow(FALSE);
	m_ctrlVolume.EnableWindow(FALSE);
	m_ctrlPitch.EnableWindow(FALSE);
	m_ctrlVoice = (CComboBox*)GetDlgItem(IDC_COMBO_VOICE);
	m_ctrlVoice->EnableWindow(FALSE);
	CEdit* pitch = (CEdit*)GetDlgItem(IDC_EDIT_PITCH);
	pitch->EnableWindow(FALSE);
	CEdit* volume = (CEdit*)GetDlgItem(IDC_EDIT_VOL);
	volume->EnableWindow(FALSE);
	CEdit* speed = (CEdit*)GetDlgItem(IDC_EDIT_RATE);
	speed->EnableWindow(FALSE);
	CButton* test = (CButton*)GetDlgItem(IDC_BTN_TEST);
	test->EnableWindow(FALSE);
//	CButton* speakall = (CButton*)GetDlgItem(IDC_CHK_SPEAKALL);
//	speakall->EnableWindow(FALSE);
}

void CSpeechSettingsDlg::EnableControls()
{
	m_ctrlSpeed.EnableWindow();
	m_ctrlVolume.EnableWindow();
	m_ctrlPitch.EnableWindow();
	m_ctrlVoice = (CComboBox*)GetDlgItem(IDC_COMBO_VOICE);
	m_ctrlVoice->EnableWindow();
	CEdit* pitch = (CEdit*)GetDlgItem(IDC_EDIT_PITCH);
	pitch->EnableWindow();
	CEdit* volume = (CEdit*)GetDlgItem(IDC_EDIT_VOL);
	volume->EnableWindow();
	CEdit* speed = (CEdit*)GetDlgItem(IDC_EDIT_RATE);
	speed->EnableWindow();
	CButton* test = (CButton*)GetDlgItem(IDC_BTN_TEST);
	test->EnableWindow();
//	CButton* speakall = (CButton*)GetDlgItem(IDC_CHK_SPEAKALL);
//	speakall->EnableWindow();

}

void CSpeechSettingsDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_ctrlSpeaker)
	{
		m_ctrlSpeaker.DestroyWindow();
	}

	CDialog::OnClose();
}


void CSpeechSettingsDlg::OnKillfocusEditPitch() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_ctrlPitch.SetPos(m_nPitch);
	SetModified();
	
}

void CSpeechSettingsDlg::OnKillfocusEditRate() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_ctrlSpeed.SetPos(m_nSpeed);
	m_ctrlSpeaker.SetSpeed(m_nSpeed);
	SetModified();	
}

void CSpeechSettingsDlg::OnKillfocusEditVol() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_ctrlVolume.SetPos(m_nVolume);
	m_ctrlSpeaker.SetVolume(m_ctrlVolume.GetPos());

	SetModified();
}

void CSpeechSettingsDlg::OnOK()
{// update registry settings for modified elements
	if (m_bModified)
	{
		UpdateData();
	
		if (m_ctrlSpeaker)
			m_ctrlSpeaker.SaveVoice("WORDAID");

		CString szSetting = _T("");
		m_ctrlVoice->GetWindowText(szSetting);
		m_Settings->SetSetting(SPEECH_VOICE,szSetting);

		szSetting = _T("");
		szSetting.Format(_T("%d"),m_nPitch);
		m_Settings->SetSetting(SPEECH_PITCH,szSetting);

		szSetting = _T("");
		szSetting.Format(_T("%d"),m_nVolume);
		m_Settings->SetSetting(SPEECH_VOLUME,szSetting);

		szSetting = _T("");
		szSetting.Format(_T("%d"),m_nSpeed);
		m_Settings->SetSetting(SPEECH_SPEED,szSetting);
	}

	SetModified(FALSE);
}

void CSpeechSettingsDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CTreePage::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if (bShow)
	{
		m_bSpeechOn = atoi((const char*)m_Settings->GetSetting(USE_SPEECH));
		m_bSpeakAll = atoi((const char*)m_Settings->GetSetting(SPEAK_ALL));
		m_nPitch = atoi((const char*)m_Settings->GetSetting(SPEECH_PITCH));
		m_nSpeed = atoi((const char*)m_Settings->GetSetting(SPEECH_SPEED));
		m_nVolume = atoi((const char*)m_Settings->GetSetting(SPEECH_VOLUME));

		m_ctrlPitch.SetPos(m_nPitch);
		m_ctrlSpeed.SetPos(m_nSpeed);
		m_ctrlVolume.SetPos(m_nVolume);
		
		if (!m_bSpeechOn)	//speech not selected so disable everything else
			DisableControls();
		else if (!m_ctrlSpeaker)
		{
			if ( !m_ctrlSpeaker.Create("Speaker",WS_CHILD,CRect(0,0,1,1),this,2))
				m_bNoSAPI = TRUE;	//WA0027
		}

		if (m_bSpeechOn && !m_bNoSAPI)
			EnableControls();

		if (m_ctrlSpeaker && m_bSpeechOn && !m_bNoSAPI)
		{
			int no_of_voices = m_ctrlSpeaker.GetVoiceCount();
			m_ctrlVoice = (CComboBox*)GetDlgItem(IDC_COMBO_VOICE);

			//WA0027 clear the list first
			if (m_ctrlVoice->GetCount() > 0)
				m_ctrlVoice->ResetContent();

			// List the names of the voices in the list box
			for (int i = 0;  i < no_of_voices;  i++)
			{
				// Get each voice from the TextSpeaker. 
				CVoice voice;
				voice.AttachDispatch(m_ctrlSpeaker.GetVoice(i));
//WA0027 start				m_ctrlVoice->AddString(voice.GetVoiceName());
				CString name = voice.GetManufacturerName();
				if (name == "IBM")
					;
				else
					m_ctrlVoice->AddString(voice.GetVoiceName());
//WA0027 end
				voice.ReleaseDispatch();
			}
			// select the current voice 
			int pos = m_ctrlVoice->FindString(-1,m_Settings->GetSetting(SPEECH_VOICE));
			if (pos != -1 ) // voice found
			{
				m_ctrlVoice->SetCurSel(pos);

				// create this voice
				IDispatch* voice = m_ctrlSpeaker.GetVoice(pos);
				if (!m_ctrlSpeaker.SelectVoice(voice))
					TRACE("can't select voice");
				voice->Release();
			}
		}
		UpdateData(FALSE);
	}
	
}
