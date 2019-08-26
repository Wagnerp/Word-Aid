#if !defined(AFX_MERGESELECTWORDDLG_H__84BBBC6F_8890_41D9_8525_C2E3C8B39A67__INCLUDED_)
#define AFX_MERGESELECTWORDDLG_H__84BBBC6F_8890_41D9_8525_C2E3C8B39A67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergeSelectWordDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMergeSelectWordDlg dialog

class CMergeSelectWordDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CMergeSelectWordDlg)

// Construction
public:
	CStringArray m_strArrayOutput;
	CMergeSelectWordDlg();
	~CMergeSelectWordDlg();

// Dialog Data
	//{{AFX_DATA(CMergeSelectWordDlg)
	enum { IDD = IDD_DLG_WORD_SELECT };
	CListBox	m_listInput;
	CListBox	m_listOutput;
	CString	m_strFileInput;
	CString	m_strFileOutput;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMergeSelectWordDlg)
	public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int FindString(CString str);
	// Generated message map functions
	//{{AFX_MSG(CMergeSelectWordDlg)
	afx_msg void OnBtnAllIn();
	afx_msg void OnBtnAllOut();
	afx_msg void OnBtnWordIn();
	afx_msg void OnBtnWordOut();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGESELECTWORDDLG_H__84BBBC6F_8890_41D9_8525_C2E3C8B39A67__INCLUDED_)
