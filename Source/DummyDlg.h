#if !defined(AFX_DUMMYDLG_H__0ED8A9C4_2BBA_11D6_92B8_AE25653F9771__INCLUDED_)
#define AFX_DUMMYDLG_H__0ED8A9C4_2BBA_11D6_92B8_AE25653F9771__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DummyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDummyDlg dialog

class CDummyDlg : public CDialog
{
// Construction
public:
	CDummyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDummyDlg)
	enum { IDD = IDD_DUMMY_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDummyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDummyDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUMMYDLG_H__0ED8A9C4_2BBA_11D6_92B8_AE25653F9771__INCLUDED_)
