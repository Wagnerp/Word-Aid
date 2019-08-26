// AbbrevExpansion.cpp : implementation file
//

#include "stdafx.h"
#include "WordAid2.h"
#include "AbbrevExpansion.h"

#include "SettingIndex.h"		// Const for settings

////////////////////////////////////////////////////////////////////////////////
// CAbbrevExpansion dialog

//IMPLEMENT_DYNAMIC(CAbbrevExpansion, CTreePage)
CAbbrevExpansion::CAbbrevExpansion(CWnd* pParent /*=NULL*/,
								   CWordAidTreeDialog* pWordAidTree /*= NULL*/)
	: CTreePage(CAbbrevExpansion::IDD, pParent)
{
	m_pWordAidTreeDlg = pWordAidTree;
}

////////////////////////////////////////////////////////////////////////////////

CAbbrevExpansion::~CAbbrevExpansion()
{
}

////////////////////////////////////////////////////////////////////////////////

void CAbbrevExpansion::DoDataExchange(CDataExchange* pDX)
{
	CTreePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_AUTO_AFTER, m_edt_auto_after);
	DDX_Control(pDX, IDC_EDT_MANUAL_HOTKEY, m_edt_manual_hotkey);
	DDX_Control(pDX, IDC_CHK_LISTED_EXPAN, m_chk_listed_expansion);
	DDX_Control(pDX, IDC_CHK_WORD_BANK, m_chk_word_bank);
	DDX_Control(pDX, IDC_RAD_AUTO_AFTER, m_rad_automatic_after);
	DDX_Control(pDX, IDC_RAD_SEMI_AUTO, m_rad_semi_automatic);
	DDX_Control(pDX, IDC_RAD_MANUAL, m_rad_manual_hotkey);
}

////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAbbrevExpansion, CTreePage)
	ON_BN_CLICKED(IDC_RAD_MANUAL, OnBnClickedRadManual)
	ON_BN_CLICKED(IDC_RAD_SEMI_AUTO, OnBnClickedRadSemiAuto)
	ON_BN_CLICKED(IDC_RAD_AUTO_AFTER, OnBnClickedRadAutoAfter)
	ON_EN_CHANGE(IDC_EDT_MANUAL_HOTKEY, OnEnChangeEdtManualHotkey)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
// CAbbrevExpansion message handlers

BOOL CAbbrevExpansion::OnInitDialog() 
{
	CTreePage::OnInitDialog();

	m_edt_manual_hotkey.SHIFTOn(FALSE);

	InitialiseSettings();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////////////////////////////////////////////////////////////////////

void CAbbrevExpansion::OnBnClickedRadManual()
{
	SetControlsState();		// Enables or disable controls depending upon expansion
							// method
}

////////////////////////////////////////////////////////////////////////////////

void CAbbrevExpansion::OnBnClickedRadSemiAuto()
{
	SetControlsState();		// Enables or disable controls depending upon expansion
							// method
}

////////////////////////////////////////////////////////////////////////////////

void CAbbrevExpansion::OnBnClickedRadAutoAfter()
{
	SetControlsState();		// Enables or disable controls depending upon expansion
							// method
}

////////////////////////////////////////////////////////////////////////////////

void CAbbrevExpansion::OnEnChangeEdtManualHotkey()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CTreePage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	KEYS_USED keys;

	keys.nVK = m_edt_manual_hotkey.GetVKPressed();
	keys.strSpecKey = m_edt_manual_hotkey.GetSpecialKeysPressed();

	if(!m_pWordAidTreeDlg->SetAbbrevManualKey(keys))
	{
		m_edt_manual_hotkey.ClearEditBox();
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Initialise Settings from the registry and sets up the state
//				: of the controls.
////////////////////////////////////////////////////////////////////////////////

void CAbbrevExpansion::InitialiseSettings()
{
	CString szExpansionMethod = m_Settings->GetSetting(ABBREV_EXPANSION_METHOD);

	if(szExpansionMethod == AUTOMATIC_EXPANSION)
	{
		m_rad_automatic_after.SetCheck(1);
		m_edt_auto_after.SetWindowText(m_Settings->GetSetting(ABBREV_AUTOMATIC_DELAY));
	}
	else if(szExpansionMethod == SEMI_AUTOMATIC)
	{
		m_rad_semi_automatic.SetCheck(1);
	}
	else if(szExpansionMethod == MANUAL_HOTKEY)
	{
		m_rad_manual_hotkey.SetCheck(1);
	}

	SetControlsState();		// Enables or disable controls depending upon expansion
							// method

	// Display hotkey
	m_edt_manual_hotkey.SetVKPressed(atoi(m_Settings->GetSetting(ABBREV_MANUAL_HOTKEY)));
	m_edt_manual_hotkey.SetSpecialKeysPressed(m_Settings->GetSetting(ABBREV_MANUAL_SPEC_KEY));
	m_edt_manual_hotkey.DisplayShortCut();

	m_chk_word_bank.SetCheck(atoi(m_Settings->GetSetting(ABBREV_WORD_BANK)));
	m_chk_listed_expansion.SetCheck(atoi(m_Settings->GetSetting(ABBREV_LISTED_EXPANSION)));

}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Enables or disables the Manual Hotkey edit box depending
//				: upon whether the Manual radio button is set.
////////////////////////////////////////////////////////////////////////////////

void CAbbrevExpansion::SetControlsState()
{
	m_edt_manual_hotkey.EnableWindow(m_rad_manual_hotkey.GetCheck());

	// Enables or disables the delay for automatic expansion if set
	m_edt_auto_after.EnableWindow(m_rad_automatic_after.GetCheck());
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Save the settings in registry class
////////////////////////////////////////////////////////////////////////////////

void CAbbrevExpansion::SaveSettings()
{
	if(m_rad_automatic_after.GetCheck() == 1)
	{
		m_Settings->SetSetting(ABBREV_EXPANSION_METHOD, AUTOMATIC_EXPANSION);

		CString szDelay = _T("");

		// Store Automatic Delay
		m_edt_auto_after.GetWindowText(szDelay);
		m_Settings->SetSetting(ABBREV_AUTOMATIC_DELAY, szDelay);
	}
	else if(m_rad_semi_automatic.GetCheck() == 1)
	{
		m_Settings->SetSetting(ABBREV_EXPANSION_METHOD, SEMI_AUTOMATIC);
	}
	else if(m_rad_manual_hotkey.GetCheck() == 1)
	{
		CString strVK = _T("");
		m_Settings->SetSetting(ABBREV_EXPANSION_METHOD, MANUAL_HOTKEY);
		strVK.Format(_T("%d"), m_edt_manual_hotkey.GetVKPressed());
		m_Settings->SetSetting(ABBREV_MANUAL_HOTKEY, strVK);
		m_Settings->SetSetting(ABBREV_MANUAL_SPEC_KEY, m_edt_manual_hotkey.GetSpecialKeysPressed());
	}

	CString szSetting = _T("");

	szSetting.Format(_T("%d"), m_chk_word_bank.GetCheck());
	m_Settings->SetSetting(ABBREV_WORD_BANK, szSetting);

	szSetting = _T("");

	szSetting.Format(_T("%d"), m_chk_listed_expansion.GetCheck());
	m_Settings->SetSetting(ABBREV_LISTED_EXPANSION, szSetting);

	m_Settings->StoreSettings();
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: OK Clicked so save the settings
////////////////////////////////////////////////////////////////////////////////

void CAbbrevExpansion::OnOK()
{
	SaveSettings();
}