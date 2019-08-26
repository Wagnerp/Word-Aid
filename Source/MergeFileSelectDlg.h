#if !defined(AFX_MERGEFILESELECTDLG_H__9401E7FF_F98F_4B20_A93A_52AF135591D1__INCLUDED_)
#define AFX_MERGEFILESELECTDLG_H__9401E7FF_F98F_4B20_A93A_52AF135591D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergeFileSelectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMergeFileSelectDlg dialog

class CMergeFileSelectDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CMergeFileSelectDlg)

// Construction
public:
	CMergeFileSelectDlg();
	~CMergeFileSelectDlg();

// Dialog Data
	//{{AFX_DATA(CMergeFileSelectDlg)
	enum { IDD = IDD_DLG_FILE_SELECT };
	CString	m_strOutputFilename;
	CString	m_strMainFilename;
	CString	m_strMergeFilename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMergeFileSelectDlg)
	public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMergeFileSelectDlg)
	afx_msg void OnBtnBrowseFile1();
	afx_msg void OnBtnBrowseFile2();
	afx_msg void OnBtnBrowseOutput();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEFILESELECTDLG_H__9401E7FF_F98F_4B20_A93A_52AF135591D1__INCLUDED_)
