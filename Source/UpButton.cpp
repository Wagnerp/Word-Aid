// UpButton.cpp: implementation of the CUpButton class.
//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0022 - make up button display constant size of triangle regardless of the size of	//
//			 the button																	//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//

#include "stdafx.h"
#include "UpButton.h"
#include <afxtempl.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUpButton::CUpButton(CRect rect /*= CRect(0,0,0,0)*/, CString szName /*= _T("")*/,
						 COLORREF back /*= RGB(192,192,192)*/,
						 COLORREF fore /*= RGB(0, 0, 0)*/, int nSize /*= 12*/,
						 CString szFace /*= _T("Arial")*/, UINT nImageID /*= 0*/)
			  : CWAButton(rect, szName, back, fore, nSize, szFace, nImageID)
{

}

CUpButton::~CUpButton()
{

}

void CUpButton::DrawButtonFace(CDC* pDC)
{
	CRgn	rgnArrow;
	CArray<CPoint, CPoint&> array;			// Contains the points for the arrow region
	CPoint	ptCP = m_rectOuter.CenterPoint();

	// Create the Region for Down Arrow
	CPoint pt1 = CPoint(ptCP.x, ptCP.y - ((m_rectOuter.Height() / 2) - 5));
//WA0022	CPoint pt2 = CPoint(ptCP.x - (m_rectOuter.Width() / 5), 
//WA0022						ptCP.y + ((m_rectOuter.Height() / 2) - 5));
//WA0022	CPoint pt3 = CPoint(ptCP.x + (m_rectOuter.Width() / 5),
//WA0022						ptCP.y + ((m_rectOuter.Height() / 2) - 5));
	CPoint pt2 = CPoint(ptCP.x - ((m_rectOuter.Height() / 2)+5), 
						ptCP.y + ((m_rectOuter.Height() / 2) - 5));	//WA0022
	CPoint pt3 = CPoint(ptCP.x + ((m_rectOuter.Height() / 2)+5),
						ptCP.y + ((m_rectOuter.Height() / 2) - 5));	//WA0022

	array.Add(pt1);
	array.Add(pt2);
	array.Add(pt3);

	VERIFY(rgnArrow.CreatePolygonRgn(&array[0], 3, ALTERNATE));

	// Create Brush to fill Down Arrow
	CBrush brush;

	try
	{
		brush.CreateSolidBrush(m_rgbForeGround);

		VERIFY(pDC->FillRgn(&rgnArrow, &brush));
	}
	catch(CResourceException* e)
	{
		e->ReportError();
		e->Delete();
		brush.DeleteObject();		//WA0045
		rgnArrow.DeleteObject();	//WA0045
	}

	brush.DeleteObject();		//WA0045
	rgnArrow.DeleteObject();	//WA0045
}