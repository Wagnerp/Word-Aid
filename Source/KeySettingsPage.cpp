// KeySettingsPage.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////////////////////
//																					//
//	WA0005 - changes to enforce a maximum of 10 words returned when selection keys	//
//			are being used.															//	
//																					//
//////////////////////////////////////////////////////////////////////////////////////
//																					//

#include "stdafx.h"
#include "WordAid2.h"
#include "KeySettingsPage.h"
#include "SettingIndex.h"
#include "DefaultKeys.h"
#include "AdvancedKeyDlg.h"		// Advanced Keyboard setup dialog

// CKeySettingsPage dialog

////////////////////////////////////////////////////////////////////////////////

CKeySettingsPage::CKeySettingsPage(CWnd* pParent /*=NULL*/, CWordAidTreeDialog* pTreeDlg /*= NULL*/)
	: CTreePage(CKeySettingsPage::IDD, pParent)
{
	m_pWordAidTreeDlg = pTreeDlg;
	m_bModified = FALSE;
}

////////////////////////////////////////////////////////////////////////////////

CKeySettingsPage::~CKeySettingsPage()
{
}

////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_SELECT_WORD, m_cmb_select_word);
	DDX_Control(pDX, IDC_CHK_SELECT_WORD_CTRL, m_chk_select_word_ctrl);
	DDX_Control(pDX, IDC_CHK_SELECT_WORD_ALT, m_chk_select_word_alt);
	DDX_Control(pDX, IDC_CHK_SHOW_LIST_HOTKEY, m_chk_show_list);
	DDX_Control(pDX, IDC_EDT_FULL_EXPAN_HOTKEY1, m_edtUnExpandFullKey);
	DDX_Control(pDX, IDC_EDT_PART_EXPAN_HOTKEY1, m_edtUnExpandPartKey);
	DDX_Control(pDX, IDC_EDT_UNDO_HOTKEY, m_edtUndoKey);
	DDX_Control(pDX, IDC_EDT_SHOW_LIST_KEY, m_edtShowListKey);

}

////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CKeySettingsPage, CTreePage)
	ON_BN_CLICKED(IDC_CHK_SHOW_LIST_HOTKEY, OnBnClickedChkShowListHotkey)
	ON_BN_CLICKED(IDC_CHK_SELECT_WORD_CTRL, OnBnClickedChkSelectWordCtrl)
	ON_BN_CLICKED(IDC_CHK_SELECT_WORD_ALT, OnBnClickedChkSelectWordAlt)
	ON_CBN_SELCHANGE(IDC_CMB_SELECT_WORD, OnCbnSelchangeCmbSelectWord)
	ON_EN_CHANGE(IDC_EDT_FULL_EXPAN_HOTKEY, OnEnChangeEditUnexpandFullHotkey)
	ON_EN_CHANGE(IDC_EDT_PART_EXPAN_HOTKEY, OnEnChangeEditUnexpandPartHotkey)
	ON_EN_CHANGE(IDC_EDT_UNDO_HOTKEY, OnEnChangeEditUndoKey)
	ON_EN_CHANGE(IDC_EDT_SHOW_LIST_KEY, OnEnChangeEditShowListKey)
	ON_BN_CLICKED(IDC_BTN_DEFAULT, OnBnClickedDefaultKey)
	ON_BN_CLICKED(IDC_BTN_ADVANCED, OnBnClickedAdvanced)
END_MESSAGE_MAP()


////////////////////////////////////////////////////////////////////////////////
// CKeySettingsPage message handlers

BOOL CKeySettingsPage::OnInitDialog()
{
	CTreePage::OnInitDialog();

	m_edtUndoKey.SHIFTOn(FALSE);
	m_edtShowListKey.SHIFTOn(FALSE);
	m_edtUnExpandPartKey.SHIFTOn(FALSE);;
	m_edtUnExpandFullKey.SHIFTOn(FALSE);;

	SetupCombos();

	InitialiseDialog();

	return TRUE;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::OnBnClickedChkShowListHotkey()
{
	if(m_chk_show_list.GetCheck() == 0)
	{
		m_edtShowListKey.EnableWindow(FALSE);

		ClearShowListControls();
	}
	else
	{
		m_edtShowListKey.EnableWindow(TRUE);

	}
	SetModified();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::OnCbnSelchangeCmbSelectWord()
{
	SetModified();

	CString strSelectWord = _T("");

	m_cmb_select_word.GetLBText(m_cmb_select_word.GetCurSel(), strSelectWord);

	if(!m_pWordAidTreeDlg->SetSelectWordKeys(strSelectWord,
										 GetSpecCheckBox(m_chk_select_word_ctrl,
														 m_chk_select_word_alt)))
	{
		m_cmb_select_word.SetCurSel(-1);
		m_cmb_select_word.SetFocus();
	}
	//WA0005 check still valid number words to be returned
	if (strSelectWord != "None")													//WA0005
	{
		if (atoi(m_Settings->GetSetting(MAX_WORDS)) >10)							//WA0005
		{
			int ret = AfxMessageBox(IDS_ERR_MAX_WORD,MB_OKCANCEL|MB_ICONQUESTION);	//WA0005
			if (ret == IDOK)														//WA0005
			{
				//set max words to 10
				m_Settings->SetSetting(MAX_WORDS,"10");								//WA0005
			}
			else																	//WA0005
			{
				//set selection key to none
				CString str = _T("");												//WA0005
				str.LoadString(IDS_NO_KEYS);										//WA0005
				m_cmb_select_word.SelectString(0,str);								//WA0005
			}
		}
	}

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::OnBnClickedChkSelectWordCtrl()
{
	if (m_chk_select_word_ctrl.GetCheck() == 1)
	{
		m_chk_select_word_alt.SetCheck(FALSE);
	}
	SetModified();

	CString strSelectWord = _T("");

	m_cmb_select_word.GetLBText(m_cmb_select_word.GetCurSel(), strSelectWord);

	if(!m_pWordAidTreeDlg->SetSelectWordKeys(strSelectWord,
										 GetSpecCheckBox(m_chk_select_word_ctrl,
														 m_chk_select_word_alt)))
	{
		m_chk_select_word_ctrl.SetCheck(!m_chk_select_word_ctrl.GetCheck());
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::OnBnClickedChkSelectWordAlt()
{
	if (m_chk_select_word_alt.GetCheck() == 1)
	{
		m_chk_select_word_ctrl.SetCheck(FALSE);
	}
	SetModified();

	CString strSelectWord = _T("");

	m_cmb_select_word.GetLBText(m_cmb_select_word.GetCurSel(), strSelectWord);

	if(!m_pWordAidTreeDlg->SetSelectWordKeys(strSelectWord,
										 GetSpecCheckBox(m_chk_select_word_ctrl,
														 m_chk_select_word_alt)))
	{
		m_chk_select_word_alt.SetCheck(!m_chk_select_word_alt.GetCheck());
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::SetupCombos()
{
	CString strNumKeys = _T("");
	CString strFuncKeys = _T("");
	CString strNumPadKeys = _T("");
	CString strNoKeys = _T("");

	strNumKeys.LoadString(IDS_NUM_KEYS);
	strFuncKeys.LoadString(IDS_FUNC_KEYS);
	strNumPadKeys.LoadString(IDS_NUMPAD_KEYS);
	strNoKeys.LoadString(IDS_NO_KEYS);

	m_cmb_select_word.AddString(strNumKeys);
	m_cmb_select_word.AddString(strFuncKeys);
	m_cmb_select_word.AddString(strNumPadKeys);	
	m_cmb_select_word.AddString(strNoKeys);
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Initialise the dialog with the settings.					  //
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::InitialiseDialog()
{
	CString szSpecKey = _T("");

	// Keys to select word settings
	m_cmb_select_word.SelectString(0, m_Settings->GetSetting(SELECT_WORD_KEYS));
	
	szSpecKey = m_Settings->GetSetting(SELECT_WORD_SPEC);

	SetSpecCheckBox(m_chk_select_word_ctrl,
					m_chk_select_word_alt,
					szSpecKey);

	// Display the Full and partial expansion hotkeys
	m_edtUnExpandFullKey.SetVKPressed(atoi(m_Settings->GetSetting(ABBREV_FULL_EXPAN_HOTKEY)));
	m_edtUnExpandFullKey.SetSpecialKeysPressed(m_Settings->GetSetting(ABBREV_FULL_EXPAN_SPEC_KEY));
	m_edtUnExpandFullKey.DisplayShortCut();

	m_edtUnExpandPartKey.SetVKPressed(atoi(m_Settings->GetSetting(ABBREV_PART_EXPAN_HOTKEY)));
	m_edtUnExpandPartKey.SetSpecialKeysPressed(m_Settings->GetSetting(ABBREV_PART_EXPAN_SPEC_KEY));
	m_edtUnExpandPartKey.DisplayShortCut();

	m_edtUndoKey.SetVKPressed(atoi(m_Settings->GetSetting(UNDO_HOTKEY)));
	m_edtUndoKey.SetSpecialKeysPressed(m_Settings->GetSetting(UNDO_SPEC));
	m_edtUndoKey.DisplayShortCut();

	if(atoi(m_Settings->GetSetting(SHOW_LIST)))
	{
		m_edtShowListKey.SetVKPressed(atoi(m_Settings->GetSetting(SHOW_LIST_HOTKEY)));
		m_edtShowListKey.SetSpecialKeysPressed(m_Settings->GetSetting(SHOW_LIST_SPEC));
		m_edtShowListKey.DisplayShortCut();
	}

	m_chk_show_list.SetCheck(atoi(m_Settings->GetSetting(SHOW_LIST)));
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Sets up the shift ctrl and alt checkboxes.  The check is    //
//				: if s - shift is checked, c - control is checked and		  //
//				: a - alt is checked.										  //
//				:															  //
// PARAMETERS	: chkCtrl - the ctrl checkbox.								  //
//				: chkAlt - the alt checkbox									  //
//				: szSpecKeys - the string to set the check boxes.			  //
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::SetSpecCheckBox(CButton& chkCtrl,
									   CButton& chkAlt,
									   CString szSpecKeys)
{
	// Control
	if(szSpecKeys.Find(_T("c")) != -1)
	{
		chkCtrl.SetCheck(1);
	}

	// Alt
	if(szSpecKeys.Find(_T("a")) != -1)
	{
		chkAlt.SetCheck(1);
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Clears the show list controls								  //
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::ClearShowListControls()
{
	//m_edtShowListKey.ClearEditBox();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::OnOK()
{
		StoreSettings();
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Stores the settings to registry.							  //
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::StoreSettings()
{
	CString szSetting = _T("");

	// Store the Select word key
	m_cmb_select_word.GetLBText(m_cmb_select_word.GetCurSel(), szSetting);
	m_Settings->SetSetting(SELECT_WORD_KEYS, szSetting);						

	m_Settings->SetSetting(SELECT_WORD_SPEC, GetSpecCheckBox(m_chk_select_word_ctrl,
															 m_chk_select_word_alt));

	// Store Full and partial expansion hotkeys
	CString strVK = _T("");
	strVK.Format(_T("%d"), m_edtUnExpandFullKey.GetVKPressed());
	m_Settings->SetSetting(ABBREV_FULL_EXPAN_HOTKEY, strVK);
	m_Settings->SetSetting(ABBREV_FULL_EXPAN_SPEC_KEY, m_edtUnExpandFullKey.GetSpecialKeysPressed());

	strVK = _T("");
	strVK.Format(_T("%d"), m_edtUnExpandPartKey.GetVKPressed());
	m_Settings->SetSetting(ABBREV_PART_EXPAN_HOTKEY, strVK);
	m_Settings->SetSetting(ABBREV_PART_EXPAN_SPEC_KEY, m_edtUnExpandPartKey.GetSpecialKeysPressed());

	strVK = _T("");
	strVK.Format(_T("%d"), m_edtUndoKey.GetVKPressed());
	m_Settings->SetSetting(UNDO_HOTKEY, strVK);
	m_Settings->SetSetting(UNDO_SPEC, m_edtUndoKey.GetSpecialKeysPressed());

	// Store Show List hotey
	szSetting = _T("");

	szSetting.Format(_T("%d"), m_chk_show_list.GetCheck());
	m_Settings->SetSetting(SHOW_LIST, szSetting);
	if (m_chk_show_list.GetCheck()==1)
	{
		strVK = _T("");
		strVK.Format(_T("%d"), m_edtShowListKey.GetVKPressed());

		m_Settings->SetSetting(SHOW_LIST_HOTKEY, strVK);
		m_Settings->SetSetting(SHOW_LIST_SPEC, m_edtShowListKey.GetSpecialKeysPressed());
	}

	m_Settings->StoreSettings();
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Formats a string to store the special keys that have been   //
//				: selected in the checkboxes.								  //
//				:															  //
// PARAMETERS	: chkCtrl - control checkbox.								  //
//				: chkAlt - Alt checkbox.									  //
//				:															  //
// RETURNS		: A string formated with s if shift is checked, c if ctrl is  //
//				: checked and a if alt is checked.							  //
////////////////////////////////////////////////////////////////////////////////

CString CKeySettingsPage::GetSpecCheckBox(CButton& chkCtrl,
										  CButton& chkAlt)
{
	CString szReturn = _T("");

	// Control
	if(chkCtrl.GetCheck() == 1)
	{
		szReturn += _T("c");
	}

	// Alt
	if(chkAlt.GetCheck() == 1)
	{
		szReturn += _T("a");
	}

	return szReturn;

}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Check to make sure that the short cut is not used by any
//				: other action
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::OnEnChangeEditUnexpandFullHotkey()
{
	KEYS_USED keys;

	keys.nVK = m_edtUnExpandFullKey.GetVKPressed();
	keys.strSpecKey = m_edtUnExpandFullKey.GetSpecialKeysPressed();

	if(!m_pWordAidTreeDlg->SetUnExpandFullKey(keys))
	{
		m_edtUnExpandFullKey.ClearEditBox();
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Check to make sure that the short cut is not used by any
//				: other action
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::OnEnChangeEditUnexpandPartHotkey()
{
	KEYS_USED keys;

	keys.nVK = m_edtUnExpandPartKey.GetVKPressed();
	keys.strSpecKey = m_edtUnExpandPartKey.GetSpecialKeysPressed();

	if(!m_pWordAidTreeDlg->SetUnExpandPartKey(keys))
	{
		m_edtUnExpandPartKey.ClearEditBox();
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Check to make sure that the short cut is not used by any
//				: other action
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::OnEnChangeEditUndoKey()
{
	KEYS_USED keys;

	keys.nVK = m_edtUndoKey.GetVKPressed();
	keys.strSpecKey = m_edtUndoKey.GetSpecialKeysPressed();

	if(!m_pWordAidTreeDlg->SetUndoKey(keys))
	{
		m_edtUndoKey.ClearEditBox();
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Check to make sure that the short cut is not used by any
//				: other action
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::OnEnChangeEditShowListKey()
{
	KEYS_USED keys;

	keys.nVK = m_edtShowListKey.GetVKPressed();
	keys.strSpecKey = m_edtShowListKey.GetSpecialKeysPressed();

	if(!m_pWordAidTreeDlg->SetShowListKey(keys))
	{
		m_edtShowListKey.ClearEditBox();
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Set back the default short cuts.
////////////////////////////////////////////////////////////////////////////////
void CKeySettingsPage::OnBnClickedDefaultKey()
{
	// Set keys back to default values
	m_edtUnExpandFullKey.ClearEditBox();
	m_edtUnExpandFullKey.SetVKPressed(DEFAULT_FULL_EXPAN_KEY);
	m_edtUnExpandFullKey.SetSpecialKeysPressed(DEFAULT_FULL_EXPAN_SPEC_KEY);
	m_edtUnExpandFullKey.DisplayShortCut();

	m_edtUnExpandPartKey.ClearEditBox();
	m_edtUnExpandPartKey.SetVKPressed(DEFAULT_PART_EXPAN_KEY);
	m_edtUnExpandPartKey.SetSpecialKeysPressed(DEFAULT_PART_EXPAN_SPEC_KEY);
	m_edtUnExpandPartKey.DisplayShortCut();

	m_edtUndoKey.ClearEditBox();
	m_edtUndoKey.SetVKPressed(DEFAULT_UNDO_KEY);
	m_edtUndoKey.SetSpecialKeysPressed(DEFAULT_UNDO_SPEC_KEY);
	m_edtUndoKey.DisplayShortCut();

	m_edtShowListKey.ClearEditBox();
	m_edtShowListKey.SetVKPressed(DEFAULT_SHOWLIST_KEY);
	m_edtShowListKey.SetSpecialKeysPressed(DEFAULT_SHOWLIST_SPEC_KEY);
	m_edtShowListKey.DisplayShortCut();

	m_chk_show_list.SetCheck(atoi(DEFAULT_SHOWLIST_ON));
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Allow user to set all short cut keys
////////////////////////////////////////////////////////////////////////////////

void CKeySettingsPage::OnBnClickedAdvanced()
{
	// WA0042 - Fixex problem with main window getting enabled
	//CAdvancedKeyDlg aDlg();
	CAdvancedKeyDlg aDlg(this);

	aDlg.SetSettings(m_Settings);
	aDlg.m_pWordAidSetting = m_pWordAidTreeDlg;

	if(aDlg.DoModal() == IDOK)
	{
		m_Settings->StoreSettings();

		// Display any changes
		InitialiseDialog();
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////