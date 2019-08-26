// GeneralSettings.cpp : implementation file
//																					//
//////////////////////////////////////////////////////////////////////////////////////
//																					//
//	WA0005 - changes to enforce a maximum of 10 words returned when selection keys	//
//			are being used.															//	
//																					//
//////////////////////////////////////////////////////////////////////////////////////
//																					//


#include "stdafx.h"
#include "WordAid2.h"
#include "GeneralSettings.h"
#include "SettingIndex.h"

////////////////////////////////////////////////////////////////////////////////
// CGeneralSettings dialog


CGeneralSettings::CGeneralSettings(CWnd* pParent /*=NULL*/)
	: CTreePage(CGeneralSettings::IDD, pParent)
{
	m_bModified = FALSE;

}

////////////////////////////////////////////////////////////////////////////////

CGeneralSettings::~CGeneralSettings()
{
}

////////////////////////////////////////////////////////////////////////////////

void CGeneralSettings::DoDataExchange(CDataExchange* pDX)
{
	CTreePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_AUTO_SPACE, m_chk_auto_space);
	DDX_Control(pDX, IDC_CHK_AUTO_SPACE_WORD, m_chk_auto_space_word);
	DDX_Control(pDX, IDC_CHK_AUTO_CAP, m_chk_auto_cap);
	DDX_Control(pDX, IDC_CHK_MINIMIZE, m_chk_startup_minimize);
	DDX_Control(pDX, IDC_COMBO_MAXWORDS, m_cmb_max_words);
	DDX_Control(pDX, IDC_CHK_ABBREV_ON, m_chk_abbrev_on);
	DDX_Control(pDX, IDC_CHK_WORDAID, m_chk_wordaid_on);
}

////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGeneralSettings, CTreePage)
	ON_BN_CLICKED(IDC_CHK_AUTO_SPACE, OnBnClickedChkAutoSpace)
	ON_BN_CLICKED(IDC_CHK_AUTO_SPACE_WORD, OnBnClickedChkAutoSpaceWord)
	ON_BN_CLICKED(IDC_CHK_AUTO_CAP, OnBnClickedChkAutoCap)
	ON_BN_CLICKED(IDC_CHK_MINIMIZE, OnBnClickedChkMinimize)
//	ON_CBN_SELCHANGE(IDC_COMBO_MAXWORDS, OnCbnSelchangeCmbMaxWords)
	ON_EN_CHANGE(IDC_COMBO_MAXWORDS,OnCbnSelchangeCmbMaxWords)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
// CGeneralSettings message handlers

BOOL CGeneralSettings::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	InitialiseSettings();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

void CGeneralSettings::OnBnClickedChkAutoSpace()
{
	// TODO: Add your control notification handler code here
	SetModified();
}

////////////////////////////////////////////////////////////////////////////////

void CGeneralSettings::OnBnClickedChkAutoSpaceWord()
{
	SetModified();

}
////////////////////////////////////////////////////////////////////////////////

void CGeneralSettings::OnBnClickedChkAutoCap()
{
	// TODO: Add your control notification handler code here
	SetModified();

}

////////////////////////////////////////////////////////////////////////////////

void CGeneralSettings::OnBnClickedChkMinimize()
{
	// TODO: Add your control notification handler code here
	SetModified();
}

///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

void CGeneralSettings::InitialiseSettings()
{
	m_chk_auto_space.SetCheck(atoi(m_Settings->GetSetting(AUTO_SPACE)));
	m_chk_auto_space_word.SetCheck(atoi(m_Settings->GetSetting(AUTO_SPACE_WORD)));
	m_chk_auto_cap.SetCheck(atoi(m_Settings->GetSetting(AUTO_CAPS)));
	m_chk_startup_minimize.SetCheck(atoi(m_Settings->GetSetting(STARTUP_MINIMIZE)));
//	m_cmb_max_words.SelectString(-1, m_Settings->GetSetting(MAX_WORDS));
	
	m_cmb_max_words.SetWindowText(m_Settings->GetSetting(MAX_WORDS));
	//CEdit* box = (CEdit*)GetDlgItem(IDC_EDT_MAXWORDS);
	//box->SetWindowText(m_Settings->GetSetting(MAX_WORDS));
	m_chk_abbrev_on.SetCheck(atoi(m_Settings->GetSetting(ABBREV_ON)));
	m_chk_wordaid_on.SetCheck(atoi(m_Settings->GetSetting(WORDAID_ON)));
}

void CGeneralSettings::OnOK()
{
	if (m_bModified)
	{
/*		CString szSave = _T("");
		CString szWordAid = _T("");

		TRY
		{
			szSave.LoadString(IDS_SAVE_SETTING);
			szWordAid.LoadString(IDS_WORDAID2);
		}
		CATCH(CMemoryException, e)
		{
		}
		END_CATCH

		if(::MessageBox(this->m_hWnd, szSave, szWordAid, MB_YESNO) == IDYES)
		{
*/			CString szSetting = _T("");

			szSetting.Format(_T("%d"), m_chk_auto_space.GetCheck());
			m_Settings->SetSetting(AUTO_SPACE, szSetting);

			szSetting = _T("");

			szSetting.Format(_T("%d"), m_chk_auto_space_word.GetCheck());
			m_Settings->SetSetting(AUTO_SPACE_WORD, szSetting);

			szSetting = _T("");


			szSetting.Format(_T("%d"), m_chk_auto_cap.GetCheck());
			m_Settings->SetSetting(AUTO_CAPS, szSetting);

			szSetting = _T("");

			szSetting.Format(_T("%d"), m_chk_startup_minimize.GetCheck());
			m_Settings->SetSetting(STARTUP_MINIMIZE, szSetting);

			szSetting = _T("");

//			m_cmb_max_words.GetLBText(m_cmb_max_words.GetCurSel(), szSetting);
			CEdit* box = (CEdit*)GetDlgItem(IDC_COMBO_MAXWORDS);
			CString word = _T("");
			box->GetWindowText(word);
//			m_Settings->SetSetting(MAX_WORDS, szSetting);
			m_Settings->SetSetting(MAX_WORDS, word);

			szSetting = _T("");

			szSetting.Format(_T("%d"), m_chk_abbrev_on.GetCheck());
			m_Settings->SetSetting(ABBREV_ON, szSetting);

			szSetting = _T("");

			szSetting.Format(_T("%d"), m_chk_wordaid_on.GetCheck());
			m_Settings->SetSetting(WORDAID_ON, szSetting);

			SetModified(FALSE);
//		}
	}
}
void CGeneralSettings::OnCbnSelchangeCmbMaxWords()
{
	CEdit* box = (CEdit*)GetDlgItem(IDC_COMBO_MAXWORDS);				//WA0005
	CString word = _T("");												//WA0005
	box->GetWindowText(word);											//WA0005
	int num = atoi(word);												//WA0005
	if (m_Settings->GetSetting(SELECT_WORD_KEYS) != "None" && (num >10))//WA0005
	{
		AfxMessageBox(IDS_ERR_SELECT_KEYS,MB_OK|MB_ICONQUESTION);		//WA0005
		// reset max words to 10
		box->SetWindowText("10");										//WA0005
	}

	SetModified();
}

