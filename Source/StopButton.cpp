// StopButton.cpp: implementation of the CStopButton class.
//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0017 - add speech indication when there are no more words above or below the		//
//			current list and display stop button rather than up or down as appropriate	//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0022 - make stop button display a square regardless of the size of the button		//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//


#include "stdafx.h"
#include "wordaid2.h"
#include "StopButton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStopButton::CStopButton(CRect rect /*= CRect(0,0,0,0)*/, CString szName /*= _T("")*/,
						 COLORREF back /*= RGB(192,192,192)*/,
						 COLORREF fore /*= RGB(0, 0, 0)*/, int nSize /*= 12*/,
						 CString szFace /*= _T("Arial")*/, UINT nImageID /*= 0*/)
			  : CWAButton(rect, szName, back, fore, nSize, szFace, nImageID)
{

}

CStopButton::~CStopButton()
{

}

void CStopButton::DrawButtonFace(CDC* pDC)
{	// removed to improve look - md & ms
/*	CRgn	rgnSquare;
	CPoint	ptCP = m_rectOuter.CenterPoint();

	// Create the Region for Down Arrow
//WA0022	CPoint pt1 = CPoint(ptCP.x - (m_rectOuter.Width() / 5), ptCP.y - ((m_rectOuter.Height() / 2) - 5));
	CPoint pt1 = CPoint(ptCP.x - (m_rectOuter.Width() / 5), ptCP.y - ((m_rectOuter.Height() / 2) - 5));	//WA0022
	CPoint pt2 = CPoint(ptCP.x + (m_rectOuter.Width() / 5), 
						ptCP.y + ((m_rectOuter.Height() / 2) - 5));

	VERIFY(rgnSquare.CreateRectRgn(pt1.x,pt1.y,pt2.x,pt2.y));

	// Create Brush to fill Down Arrow
	CBrush brush;

	try
	{
		brush.CreateSolidBrush(m_rgbForeGround);

		VERIFY(pDC->FillRgn(&rgnSquare, &brush));
	}
	catch(CResourceException* e)
	{
		e->ReportError();
		e->Delete();
	}*/
}