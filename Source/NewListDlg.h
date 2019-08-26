#if !defined(AFX_NEWLISTDLG_H__78ACF2B8_6A7A_4948_BA6D_B5364FDFA3D3__INCLUDED_)
#define AFX_NEWLISTDLG_H__78ACF2B8_6A7A_4948_BA6D_B5364FDFA3D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewListDlg.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CNewListDlg dialog

class CNewListDlg : public CDialog
{
// Construction
public:
	CNewListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewListDlg)
	enum { IDD = IDD_NEWLISTDLG };
	CListBox	m_lstTypes;
	//}}AFX_DATA

	BOOL m_bAbbrevOn;
	BOOL m_bWordAidOn;

	int m_nListType;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewListDlg)
	afx_msg void OnDblclkListType();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWLISTDLG_H__78ACF2B8_6A7A_4948_BA6D_B5364FDFA3D3__INCLUDED_)
