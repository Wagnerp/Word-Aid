#if !defined(AFX_SPEECHSETUPDLG_H__9BCCAD08_A2E9_4EC0_8998_3143EF5048EA__INCLUDED_)
#define AFX_SPEECHSETUPDLG_H__9BCCAD08_A2E9_4EC0_8998_3143EF5048EA__INCLUDED_

#include "TreeDialog.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpeechSetupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpeechSetupDlg dialog

class CSpeechSetupDlg : public CTreePage
{
// Construction
public:
	CSetting* m_Settings;
	BOOL m_bModified;
	CSpeechSetupDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpeechSetupDlg)
	enum { IDD = IDD_SPEECH_SETUP };
	CButton	m_chkFirstAuto;	//WA0027
	CEdit	m_edtDwellSelected;
	CEdit	m_edtDwellTime;
	CButton	m_chkSpeakOn;
	CButton	m_chkSpeakAll;
	CButton	m_chkSelection;
	BOOL	m_bSpeakDwell;
	BOOL	m_bSpeechOn;
	BOOL	m_bSpeakSelected;
	int		m_nDwellTimeAllWords;
	UINT	m_nDwellTimeSelectedWord;
	BOOL	m_bSpeakFirst;	//WA0027
	//}}AFX_DATA

	virtual void DialogSelected();
	virtual void LeavingDialog() {OnOK();}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpeechSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DisablePage();

	//!indicate whether settings have changed
	void SetModified(BOOL bmodified = TRUE) {m_bModified = bmodified;}
	void EnableSpeakDwell(BOOL bVal = TRUE);
	void EnableSpeakDwellTime(BOOL bVal = TRUE);
	void EnableSpeakSelected(BOOL bVal = TRUE);
	void EnableSpeakSelectedTime(BOOL bVal = TRUE);
	void EnableSpeakFirstAuto(BOOL bVal = TRUE);	//WA0027
	void InitialisePage();

	BOOL m_bNoSAPI;			// TRUE if there is no SAPI support

	// Generated message map functions
	//{{AFX_MSG(CSpeechSetupDlg)
	virtual void OnOK();
	afx_msg void OnKillfocusDwellSelected();
	afx_msg void OnKillfocusDwellAll();
	afx_msg void OnChkSpeakall();
	afx_msg void OnChkSpeechon();
	afx_msg void OnChkSpeakSelected();
	virtual BOOL OnInitDialog();
	afx_msg void OnChkFirstauto();	//WA0027
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPEECHSETUPDLG_H__9BCCAD08_A2E9_4EC0_8998_3143EF5048EA__INCLUDED_)
