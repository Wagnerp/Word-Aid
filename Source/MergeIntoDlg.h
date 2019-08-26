#if !defined(AFX_MERGEINTODLG_H__D0BBAA96_2EA2_4D8C_9AE1_61337A3A654F__INCLUDED_)
#define AFX_MERGEINTODLG_H__D0BBAA96_2EA2_4D8C_9AE1_61337A3A654F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergeIntoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMergeIntoDlg dialog

class CMergeIntoDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CMergeIntoDlg)

// Construction
public:
	CMergeIntoDlg();
	~CMergeIntoDlg();

// Dialog Data
	//{{AFX_DATA(CMergeIntoDlg)
	enum { IDD = IDD_DLG_MERGE_INTRO };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMergeIntoDlg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMergeIntoDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEINTODLG_H__D0BBAA96_2EA2_4D8C_9AE1_61337A3A654F__INCLUDED_)
