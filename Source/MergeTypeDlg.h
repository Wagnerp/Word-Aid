#if !defined(AFX_MERGETYPEDLG_H__BBBD9E71_19E6_49ED_AD75_E388EAF0E0E7__INCLUDED_)
#define AFX_MERGETYPEDLG_H__BBBD9E71_19E6_49ED_AD75_E388EAF0E0E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergeTypeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMergeTypeDlg dialog

class CMergeTypeDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CMergeTypeDlg)

// Construction
public:
	BOOL m_bMergeSelected;
	BOOL m_bMergeAll;
	CButton m_radMergeSelected;
	CButton m_radMergeAll;
	CMergeTypeDlg();
	~CMergeTypeDlg();

// Dialog Data
	//{{AFX_DATA(CMergeTypeDlg)
	enum { IDD = IDD_DLG_MERGE_TYPE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMergeTypeDlg)
	public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMergeTypeDlg)
	afx_msg void OnRadioMergeAll();
	afx_msg void OnRadioMergeSelected();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGETYPEDLG_H__BBBD9E71_19E6_49ED_AD75_E388EAF0E0E7__INCLUDED_)
