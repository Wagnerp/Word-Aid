#pragma once


// CDlgToolBar

class CDlgToolBar : public CToolBar
{
// Construction
public:
	CDlgToolBar();

// Implementation
public:
	virtual ~CDlgToolBar();

protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgToolBar)
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


