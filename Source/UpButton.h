// UpButton.h: interface for the CUpButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UPBUTTON_H__4F55704A_D49C_46E5_A53B_F32857F913A3__INCLUDED_)
#define AFX_UPBUTTON_H__4F55704A_D49C_46E5_A53B_F32857F913A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WAButton.h"

class CUpButton : public CWAButton  
{
public:
	CUpButton(CRect rect = CRect(0,0,0,0), CString szName = _T(""), COLORREF back = RGB(192,192,192),
			  COLORREF fore = RGB(0, 0, 0), int nSize = 12, CString szFace = _T("Arial"), UINT nImageID = 0);

	virtual ~CUpButton();

protected:

	virtual void DrawButtonFace(CDC* pDC);
};

#endif // !defined(AFX_UPBUTTON_H__4F55704A_D49C_46E5_A53B_F32857F913A3__INCLUDED_)
