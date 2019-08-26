// SpeechSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wordaid2.h"
#include "SpeechSetupDlg.h"
#include "settingindex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpeechSetupDlg dialog


CSpeechSetupDlg::CSpeechSetupDlg(CWnd* pParent /*=NULL*/)
	: CTreePage(CSpeechSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpeechSetupDlg)
	m_bSpeakDwell = FALSE;
	m_bSpeechOn = FALSE;
	m_bSpeakSelected = FALSE;
	m_nDwellTimeAllWords = 0;
	m_nDwellTimeSelectedWord = 0;
	m_bSpeakFirst = FALSE;	//WA0027
	//}}AFX_DATA_INIT

	m_bNoSAPI = FALSE;
}


void CSpeechSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpeechSetupDlg)
	DDX_Control(pDX, IDC_CHK_FIRSTAUTO, m_chkFirstAuto);	//WA0027
	DDX_Control(pDX, IDC_DWELL_SELECTED, m_edtDwellSelected);
	DDX_Control(pDX, IDC_DWELL_ALL, m_edtDwellTime);
	DDX_Control(pDX, IDC_CHK_SPEECHON, m_chkSpeakOn);
	DDX_Control(pDX, IDC_CHK_SPEAKALL, m_chkSpeakAll);
	DDX_Control(pDX, IDC_CHK_SELECTION, m_chkSelection);
	DDX_Check(pDX, IDC_CHK_SPEAKALL, m_bSpeakDwell);
	DDX_Check(pDX, IDC_CHK_SPEECHON, m_bSpeechOn);
	DDX_Check(pDX, IDC_CHK_SELECTION, m_bSpeakSelected);
	DDX_Text(pDX, IDC_DWELL_ALL, m_nDwellTimeAllWords);
	DDV_MinMaxInt(pDX, m_nDwellTimeAllWords, 0, 30);
	DDX_Text(pDX, IDC_DWELL_SELECTED, m_nDwellTimeSelectedWord);
	DDV_MinMaxUInt(pDX, m_nDwellTimeSelectedWord, 0, 30);
	DDX_Check(pDX, IDC_CHK_FIRSTAUTO, m_bSpeakFirst);	//WA0027
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpeechSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CSpeechSetupDlg)
	ON_EN_KILLFOCUS(IDC_DWELL_SELECTED, OnKillfocusDwellSelected)
	ON_EN_KILLFOCUS(IDC_DWELL_ALL, OnKillfocusDwellAll)
	ON_BN_CLICKED(IDC_CHK_SPEAKALL, OnChkSpeakall)
	ON_BN_CLICKED(IDC_CHK_SPEECHON, OnChkSpeechon)
	ON_BN_CLICKED(IDC_CHK_SELECTION, OnChkSpeakSelected)
	ON_BN_CLICKED(IDC_CHK_FIRSTAUTO, OnChkFirstauto)	//WA0027
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpeechSetupDlg message handlers

void CSpeechSetupDlg::OnKillfocusDwellSelected() 
{
	// TODO: Add your control notification handler code here
	SetModified();
	
}

void CSpeechSetupDlg::OnKillfocusDwellAll() 
{
	// TODO: Add your control notification handler code here
	SetModified();
	
}

void CSpeechSetupDlg::OnChkSpeakall() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (!m_bSpeakDwell)
	{
		EnableSpeakDwellTime(FALSE);
		EnableSpeakFirstAuto(FALSE);	//SAW0027
	}
	else
	{
		EnableSpeakDwellTime();
		EnableSpeakFirstAuto();	//SAW0027
	}

	SetModified();
	
}

void CSpeechSetupDlg::OnChkSpeechon() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if (!m_bSpeechOn)
	{
		EnableSpeakDwell(FALSE);
		EnableSpeakDwellTime(FALSE);
		EnableSpeakSelected(FALSE);
		EnableSpeakSelectedTime(FALSE);
		EnableSpeakFirstAuto(FALSE);	//SAW0027
	}
	else
	{
		EnableSpeakDwell();
		EnableSpeakDwellTime(m_bSpeakDwell);
		EnableSpeakSelected();
		EnableSpeakSelectedTime(m_bSpeakSelected);
		EnableSpeakFirstAuto(m_bSpeakFirst);	//SAW0027
	}

	SetModified();

}


BOOL CSpeechSetupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Create the text speaker to see if it is there.  All the controls
	// should be disabled if the text speaker is not there
	CTextSpeaker ctrlSpeaker;

	if(!ctrlSpeaker.Create("Speaker",WS_CHILD,CRect(0,0,1,1),this,2))
	{
		m_bNoSAPI = TRUE;

		DisablePage();

	}
	else
	{
		InitialisePage();
	}

	if (ctrlSpeaker)
	{
		ctrlSpeaker.DestroyWindow();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpeechSetupDlg::DialogSelected()
{
	if(m_bNoSAPI)
	{
		DisablePage();

		CString strError = _T("");

		strError.LoadString(IDS_NO_SAPI);

		AfxMessageBox(strError, MB_OK|MB_ICONEXCLAMATION);
	}
	else
	{
		InitialisePage();
	}
}

void CSpeechSetupDlg::DisablePage()
{
	m_edtDwellSelected.EnableWindow(FALSE);
	m_edtDwellTime.EnableWindow(FALSE);
	m_chkSpeakOn.EnableWindow(FALSE);
	m_chkSpeakAll.EnableWindow(FALSE);
	m_chkSelection.EnableWindow(FALSE);
	m_chkFirstAuto.EnableWindow(FALSE);	//SAW0027
}

void CSpeechSetupDlg::InitialisePage()
{
	m_bSpeechOn = atoi((const char*)m_Settings->GetSetting(USE_SPEECH));
	m_bSpeakDwell = atoi((const char*)m_Settings->GetSetting(SPEAK_ALL));
	m_nDwellTimeAllWords = atoi((const char*)m_Settings->GetSetting(DWELL_TIME_ALL));
	m_bSpeakSelected = atoi((const char*)m_Settings->GetSetting(SPEAK_SELECTED));
	m_nDwellTimeSelectedWord = atoi((const char*)m_Settings->GetSetting(DWELL_TIME_SELECTED));
	m_bSpeakFirst = atoi((const char*)m_Settings->GetSetting(SPEAK_FIRST_AUTO));	//SAW0027

	UpdateData(FALSE);

	OnChkSpeechon();
	OnChkSpeakall();
	OnChkSpeakSelected();

//	if (!m_bSpeechOn)
//		m_bNoSAPI = TRUE;

	SetModified(FALSE);
}

void CSpeechSetupDlg::OnOK()
{
	UpdateData();

	CString szSetting = _T("");
	szSetting.Format(_T("%d"), m_bSpeechOn);
	m_Settings->SetSetting(USE_SPEECH,szSetting);

	szSetting = _T("");
	szSetting.Format(_T("%d"),m_bSpeakDwell);
	m_Settings->SetSetting(SPEAK_ALL,szSetting);

	szSetting = _T("");
	szSetting.Format(_T("%d"),m_nDwellTimeAllWords);
	m_Settings->SetSetting(DWELL_TIME_ALL,szSetting);

	szSetting = _T("");
	szSetting.Format(_T("%d"),m_bSpeakSelected);
	m_Settings->SetSetting(SPEAK_SELECTED,szSetting);

	szSetting = _T("");
	szSetting.Format(_T("%d"),m_nDwellTimeSelectedWord);
	m_Settings->SetSetting(DWELL_TIME_SELECTED,szSetting);

	szSetting =_T("");									//WA0027
	szSetting.Format(_T("%d"),m_bSpeakFirst);			//WA0027
	m_Settings->SetSetting(SPEAK_FIRST_AUTO,szSetting);	//WA0027

	SetModified(FALSE);

}
void CSpeechSetupDlg::OnChkSpeakSelected(void)
{
	UpdateData(TRUE);
	if (!m_bSpeakSelected)
		EnableSpeakSelectedTime(FALSE);
	else
		EnableSpeakSelectedTime();

	SetModified();
}

void CSpeechSetupDlg::EnableSpeakDwell(BOOL bVal /* default = TRUE*/)
{
	CWnd* dlgItem = GetDlgItem(IDC_CHK_SPEAKALL);
	dlgItem->EnableWindow(bVal);
}

void CSpeechSetupDlg::EnableSpeakDwellTime(BOOL bVal /* default = TRUE*/)
{
	CWnd* dlgItem = GetDlgItem(IDC_DWELL_ALL);
	dlgItem->EnableWindow(bVal);
}

void CSpeechSetupDlg::EnableSpeakSelected(BOOL bVal /* default = TRUE*/)
{
	CWnd* dlgItem = GetDlgItem(IDC_CHK_SELECTION);
	dlgItem->EnableWindow(bVal);
}

void CSpeechSetupDlg::EnableSpeakSelectedTime(BOOL bVal /* default = TRUE*/)
{
	CWnd* dlgItem = GetDlgItem(IDC_DWELL_SELECTED);
	dlgItem->EnableWindow(bVal);
}

void CSpeechSetupDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CTreePage::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if (bShow && !m_bNoSAPI)
	{
		m_bSpeechOn = atoi((const char*)m_Settings->GetSetting(USE_SPEECH));
		m_bSpeakDwell = atoi((const char*)m_Settings->GetSetting(SPEAK_ALL));
		m_nDwellTimeAllWords = atoi((const char*)m_Settings->GetSetting(DWELL_TIME_ALL));
		m_bSpeakSelected = atoi((const char*)m_Settings->GetSetting(SPEAK_SELECTED));
		m_nDwellTimeSelectedWord = atoi((const char*)m_Settings->GetSetting(DWELL_TIME_SELECTED));
		m_bSpeakFirst = atoi((const char*)m_Settings->GetSetting(SPEAK_FIRST_AUTO));	//WA0027

		UpdateData(FALSE);

		OnChkSpeechon();
		OnChkSpeakall();
		OnChkSpeakSelected();
//		OnChkFirstauto();	//WA0027

		SetModified(FALSE);
	}
}
//WA0027 begin
void CSpeechSetupDlg::OnChkFirstauto() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

//	if (!m_bSpeakFirst)
//		EnableSpeakFirstAuto(FALSE);
//	else
//		EnableSpeakFirstAuto();

	SetModified();
	
}

void CSpeechSetupDlg::EnableSpeakFirstAuto(BOOL bVal /* default = TRUE*/)
{
	CWnd* dlgItem = GetDlgItem(IDC_CHK_FIRSTAUTO);
	dlgItem->EnableWindow(bVal);
}
//WA0027 end