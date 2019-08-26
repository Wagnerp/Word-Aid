// DownButton.h: interface for the CDownButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOWNBUTTON_H__509C532B_9F74_47D6_A440_68372F5BBD03__INCLUDED_)
#define AFX_DOWNBUTTON_H__509C532B_9F74_47D6_A440_68372F5BBD03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WAButton.h"

class CDownButton : public CWAButton  
{
public:
	CDownButton(CRect rect = CRect(0,0,0,0), CString szName = _T(""), COLORREF back = RGB(192,192,192),
			  COLORREF fore = RGB(0, 0, 0), int nSize = 12, CString szFace = _T("Arial"), UINT nImageID = 0);

	virtual ~CDownButton();

protected:

	virtual void DrawButtonFace(CDC* pDC);
};

#endif // !defined(AFX_DOWNBUTTON_H__509C532B_9F74_47D6_A440_68372F5BBD03__INCLUDED_)
