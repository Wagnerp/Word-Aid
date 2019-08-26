#pragma once
#include "afxwin.h"

#include "Setting.h"		// Settings class
#include "WordAidTreeDialog.h"	// CTreePage class
#include "KeyEdit.h"

// CKeySettingsPage dialog

class CKeySettingsPage : public CTreePage
{

public:
	CKeySettingsPage(CWnd* pParent = NULL, CWordAidTreeDialog* pTreeDlg = NULL);   // standard constructor
	virtual ~CKeySettingsPage();

	CSetting* m_Settings;
// Dialog Data
	enum { IDD = IDD_KEY_SETTINGS };

	virtual void DialogSelected() {InitialiseDialog();}
	virtual void LeavingDialog() {OnOK();}

protected:
	// Methods to Initialise Dialog
	void SetupCombos();
	void InitialiseDialog();
	void SetSpecCheckBox(CButton& chkCtrl, CButton& chkAlt, CString szSpecKeys);
	void ClearShowListControls();

	// Methods to Store Settings
	virtual void OnOK();
	void StoreSettings();
	CString GetSpecCheckBox(CButton& chkCtrl, CButton& chkAlt);

	void SetModified(BOOL bModified =  TRUE) {m_bModified = bModified;}
	BOOL m_bModified;		// TRUE if a settings has changed

	CWordAidTreeDialog* m_pWordAidTreeDlg;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cmb_select_word;
	CButton m_chk_select_word_ctrl;
	CButton m_chk_select_word_alt;
	CButton m_chk_show_list;
	CKeyEdit m_edtUndoKey;
	CKeyEdit m_edtShowListKey;
	CKeyEdit m_edtUnExpandPartKey;
	CKeyEdit m_edtUnExpandFullKey;
	afx_msg void OnBnClickedChkShowListHotkey();
	afx_msg void OnBnClickedChkSelectWordCtrl();
	afx_msg void OnBnClickedChkSelectWordAlt();
	afx_msg void OnCbnSelchangeCmbSelectWord();
	afx_msg void OnEnChangeEditUnexpandFullHotkey();
	afx_msg void OnEnChangeEditUnexpandPartHotkey();
	afx_msg void OnEnChangeEditUndoKey();
	afx_msg void OnEnChangeEditShowListKey();
	afx_msg void OnBnClickedDefaultKey();
	afx_msg void OnBnClickedAdvanced();
};
