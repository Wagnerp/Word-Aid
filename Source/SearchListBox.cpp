// SearchListBox.cpp : implementation file
//

#include "stdafx.h"
#include "wordaid2.h"
#include "SearchListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchListBox

CSearchListBox::CSearchListBox()
{
	m_strTyped = _T("");
}

CSearchListBox::~CSearchListBox()
{
}


BEGIN_MESSAGE_MAP(CSearchListBox, CListBox)
	//{{AFX_MSG_MAP(CSearchListBox)
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelChange)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchListBox message handlers
// added for WA0010 to make list box searchable
void CSearchListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	m_strTyped += nChar;

	int nMatch = FindString(0,m_strTyped);
	if (nMatch != LB_ERR)
		SetCurSel(nMatch);

//	CListBox::OnChar(nChar, nRepCnt, nFlags);
}

void CSearchListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_strTyped = _T("");

	CListBox::OnLButtonDown(nFlags, point);
}

void CSearchListBox::OnSelChange()
{
	m_strTyped = _T("");
}
