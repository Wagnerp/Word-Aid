#if !defined(AFX_SEARCHLISTBOX_H__A78ADC16_C121_4CE0_80D2_666346C85627__INCLUDED_)
#define AFX_SEARCHLISTBOX_H__A78ADC16_C121_4CE0_80D2_666346C85627__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchListBox.h : header file
//
// added for WA0010 to make list box searchable
/////////////////////////////////////////////////////////////////////////////
// CSearchListBox window

class CSearchListBox : public CListBox
{
// Construction
public:
	CSearchListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSearchListBox();

	// Generated message map functions
protected:
	CString m_strTyped;
	//{{AFX_MSG(CSearchListBox)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnSelChange();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHLISTBOX_H__A78ADC16_C121_4CE0_80D2_666346C85627__INCLUDED_)
