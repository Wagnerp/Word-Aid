#if !defined(AFX_ADDDELDLG_H__0B226578_2E5B_419A_A3B9_3DB9564A34EC__INCLUDED_)
#define AFX_ADDDELDLG_H__0B226578_2E5B_419A_A3B9_3DB9564A34EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddDelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddDelDlg dialog

class CAddDelDlg : public CDialog
{
// Construction
public:
	BOOL m_bAdd;
	CAddDelDlg(CWnd* pParent = NULL);   // standard constructor
	int GetType();
// Dialog Data
	//{{AFX_DATA(CAddDelDlg)
	enum { IDD = IDD_ADD_DELETE_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddDelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nType;
	// Generated message map functions
	//{{AFX_MSG(CAddDelDlg)
	afx_msg void OnChkTopic();
	afx_msg void OnChkAlpha();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDDELDLG_H__0B226578_2E5B_419A_A3B9_3DB9564A34EC__INCLUDED_)
