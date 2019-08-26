#pragma once


#include "TreeDialog.h"
#include "Setting.h"
#include "afxwin.h"

// CGeneralSettings dialog

class CGeneralSettings : public CTreePage
{

public:
	CGeneralSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGeneralSettings();

// Dialog Data
	//{{AFX_DATA(CWordSettings)
	enum { IDD = IDD_GENERAL_SETTINGS };
	CEdit	m_cmb_max_words;
	//}}AFX_DATA


	CSetting* m_Settings;

	virtual void OnOK();
	virtual void DialogSelected() {InitialiseSettings();}
	virtual void LeavingDialog() {OnOK();}
protected:

	// Methods to initialise settings
	void InitialiseSettings();

	virtual BOOL OnInitDialog();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void SetModified(BOOL bModified =  TRUE) {m_bModified = bModified;}
	BOOL m_bModified;		// TRUE if a settings has changed

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChkAutoSpace();
	afx_msg void OnBnClickedChkAutoSpaceWord();
	afx_msg void OnBnClickedChkAutoCap();
	afx_msg void OnBnClickedChkMinimize();
	afx_msg void OnCbnSelchangeCmbMaxWords();
	CButton m_chk_auto_space;
	CButton m_chk_auto_space_word;
	CButton m_chk_auto_cap;
	CButton m_chk_startup_minimize;
	CButton m_chk_abbrev_on;
	CButton m_chk_wordaid_on;
};
