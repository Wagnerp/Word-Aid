#if !defined(AFX_WORDAIDSETTINGSDLG_H__1947543C_FA6C_43D0_973F_3FDB1A264CB4__INCLUDED_)
#define AFX_WORDAIDSETTINGSDLG_H__1947543C_FA6C_43D0_973F_3FDB1A264CB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WordAidSettingsDlg.h : header file
//

#include "TreeDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CWordAidSettingsDlg dialog

class CWordAidSettingsDlg : public CTreePage
{
// Construction
public:
	CWordAidSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWordAidSettingsDlg)
	enum { IDD = IDD_WORDAID };
	int		m_nMaxWordLength;
	int		m_nMinWordLength;
	//}}AFX_DATA

	CSetting* m_Settings;		// Store the settings

	virtual void OnOK();
	virtual void DialogSelected() {InitialiseSettings();}
	virtual void LeavingDialog() {OnOK();}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWordAidSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void InitialiseSettings();
	void SaveSettings();

	// Generated message map functions
	//{{AFX_MSG(CWordAidSettingsDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	afx_msg void OnChangeEditMaxword();//added 17th July
	afx_msg void OnChangeEditMinword();//added 17th July

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORDAIDSETTINGSDLG_H__1947543C_FA6C_43D0_973F_3FDB1A264CB4__INCLUDED_)
