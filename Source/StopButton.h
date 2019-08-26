// StopButton.h: interface for the CStopButton class.
//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0017 - add speech indication when there are no more words above or below the		//
//			current list and display stop button rather than up or down as appropriate	//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//

#if !defined(AFX_STOPBUTTON_H__B5CFA129_C388_4D32_A684_2FA3B56D8F6E__INCLUDED_)
#define AFX_STOPBUTTON_H__B5CFA129_C388_4D32_A684_2FA3B56D8F6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WAButton.h"

class CStopButton : public CWAButton  
{
public:
	CStopButton(CRect rect = CRect(0,0,0,0), CString szName = _T(""), COLORREF back = RGB(192,192,192),
			  COLORREF fore = RGB(0, 0, 0), int nSize = 12, CString szFace = _T("Arial"), UINT nImageID = 0);
	virtual ~CStopButton();

protected:

	virtual void DrawButtonFace(CDC* pDC);
};

#endif // !defined(AFX_STOPBUTTON_H__B5CFA129_C388_4D32_A684_2FA3B56D8F6E__INCLUDED_)
