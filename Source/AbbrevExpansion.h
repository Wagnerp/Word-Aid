	#pragma once
#include "afxwin.h"
#include "TreeDialog.h"		// CTreePage class
#include "Setting.h"		// Setting class
#include "KeyEdit.h"		// CKeyEdit class for Expansion hotkey
#include "WordAidTreeDialog.h"	// The settings dialog

// CAbbrevExpansion dialog

class CAbbrevExpansion : public CTreePage
{
	//DECLARE_DYNAMIC(CAbbrevExpansion)

public:
	CAbbrevExpansion(CWnd* pParent = NULL, CWordAidTreeDialog* pWordAidTree = NULL);   // standard constructor
	virtual ~CAbbrevExpansion();

// Dialog Data
	enum { IDD = IDD_ABBREV_EXPANSION };

	virtual void OnOK();
	virtual void DialogSelected() {InitialiseSettings();}
	virtual void LeavingDialog() {OnOK();}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// Dialog Controls
	CEdit m_edt_auto_after;
	CKeyEdit m_edt_manual_hotkey;
	CButton m_chk_listed_expansion;
	CButton m_chk_word_bank;
	CButton m_rad_automatic_after;
	CButton m_rad_semi_automatic;
	CButton m_rad_manual_hotkey;

	// DialogEvents
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadManual();
	afx_msg void OnBnClickedRadSemiAuto();
	afx_msg void OnBnClickedRadAutoAfter();
	afx_msg void OnEnChangeEdtManualHotkey();

	CSetting* m_Settings;		// Store the settings

private:
	void InitialiseSettings();		// Initialise the settings from registry
	void SetControlsState();		// Sets the controls state depending upon the 
									// expansion method.	
	void SaveSettings();			// Store settings in registry class

	CWordAidTreeDialog* m_pWordAidTreeDlg;
};
