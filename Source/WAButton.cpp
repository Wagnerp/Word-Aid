// WAButton.cpp: implementation of the CWAButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "Wordaid32v2.h"
#include "WAButton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWAButton::CWAButton(CRect rect /*= CRect(0,0,0,0)*/, CString szName /*= _T("")*/,
					 COLORREF back /*= RGB(192,192,192)*/, COLORREF fore /*= RGB(0, 0, 0)*/,
					 int nSize /*= 12*/, CString szFace /*= _T("Arial")*/, UINT nImageID /*= 0*/)
{
	m_szName = szName;
	m_rectOuter = rect;
	m_rgbBackGround = back;
	m_rgbForeGround = fore;
	m_fontSize = nSize;
	m_fontFace = szFace;
//	m_fontText.CreateFont(m_fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, 0, ANSI_CHARSET, 
//					  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
//					  DEFAULT_PITCH, szFace);
	m_bEnabled = TRUE;
	m_penWhite.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	m_penBlack.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	m_penGrey.CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
//	m_brush.CreateSolidBrush(m_rgbBackGround);

	// If there is a bitmap then load it.
	if(nImageID > 0)
	{
		m_nImageID = nImageID;
		if(m_bmpImage.LoadBitmap(nImageID) == 0)
		{
			m_nImageID = 0;
		}
	}

}

CWAButton::~CWAButton()
{
	m_penWhite.DeleteObject();
	m_penBlack.DeleteObject();
	m_penGrey.DeleteObject();
	m_fontText.DeleteObject();
	m_brush.DeleteObject();
	m_bmpImage.DeleteObject();
}

void CWAButton::Draw(CDC* pDC)
{
//	CBrush brush;
	m_brush.DeleteObject();		//WA0031
	m_brush.CreateSolidBrush(m_rgbBackGround);
	m_fontText.DeleteObject();
	m_fontText.CreateFont(m_fontSize,0,0,0,FW_DONTCARE,FALSE, FALSE, 0, ANSI_CHARSET, 
					  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					  DEFAULT_PITCH, m_fontFace);

	CFont* pOldFont = (CFont*)pDC->SelectObject(m_fontText);

	DoGradientFill(pDC, m_rectOuter);
	//pDC->FillRect(m_rectOuter, &m_brush);

	// Create Shadow for Button
//	CPen whitePen, blackPen, greyPen;

//	whitePen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
//	blackPen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
//	greyPen.CreatePen(PS_SOLID, 1, RGB(128, 128, 128));

	CPen* pOldPen = (CPen*)pDC->SelectObject(&m_penWhite);
	pDC->MoveTo(m_rectOuter.left, m_rectOuter.bottom);
	pDC->LineTo(m_rectOuter.left, m_rectOuter.top);
	pDC->LineTo(m_rectOuter.right, m_rectOuter.top);

	pDC->SelectObject(&m_penBlack);
	pDC->MoveTo(m_rectOuter.right - 1, m_rectOuter.top+1);
	pDC->LineTo(m_rectOuter.right-1, m_rectOuter.bottom -1);
	pDC->LineTo(m_rectOuter.left-1, m_rectOuter.bottom- 1);

	pDC->SelectObject(&m_penGrey);
	pDC->MoveTo(m_rectOuter.left, m_rectOuter.bottom - 2);
	pDC->LineTo(m_rectOuter.right - 2, m_rectOuter.bottom - 2);
	pDC->LineTo(m_rectOuter.right - 2, m_rectOuter.top);


	pDC->SetTextColor(m_rgbForeGround);
	int nMode = pDC->SetBkMode(TRANSPARENT);

	DrawButtonFace(pDC);

	pDC->SetBkMode(nMode);
//	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldPen);
//	whitePen.DeleteObject();
//	blackPen.DeleteObject();
//	greyPen.DeleteObject();
//	brush.DeleteObject();

}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Virtual method that is called to display just text as the
//				: button face.  The text that is drawn is the text that is set
//				: as the button name.  To draw somthing else on the button 
//				: derive aclass from CWAButton and place your code to draw the
//				: button face in this virtual method.
//
// PARAMETERS	: pDC - pointer to the deveice context
////////////////////////////////////////////////////////////////////////////////

void CWAButton::DrawButtonFace(CDC* pDC)
{
	CRect rect = m_rectOuter;
	rect.InflateRect(0, +11, 0, 0);
	pDC->DrawText(m_szName, m_rectOuter, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
}

BOOL CWAButton::DisplayBitmap(CDC* pDC)
{ 
	// make a working copy of the source bitmap

    BITMAP Bmap;
    CBitmap* pbmpSource = CreateCopyBitmap(&m_bmpImage, (LPBITMAP)&Bmap);
    if(pbmpSource == NULL)
        return FALSE; 

	// create DC for source give it source bitmap and set
    // its background colour which will be white in the mask
 
    CDC dcSource;
    dcSource.CreateCompatibleDC(pDC);
    CBitmap* pbmpOldSource = dcSource.SelectObject(&m_bmpImage);
    dcSource.SetBkColor(RGB(255,25,25));

	// create DC for the mask, give it a monochrome bitmap
    // and create mask by bitblting across

    CDC dcMask;
    dcMask.CreateCompatibleDC(pDC);
    CBitmap bmpMask;
    if(!bmpMask.CreateCompatibleBitmap(&dcMask,
									   Bmap.bmWidth,
									   Bmap.bmHeight))
        return FALSE;
 
    CBitmap* pbmpOldMask = dcMask.SelectObject(&bmpMask);
    dcMask.BitBlt(0, 0, Bmap.bmWidth,
					    m_bmpImage.GetBitmapDimension().cy,
						&dcSource, 0, 0, SRCCOPY);

	// mask source

    dcSource.SetBkColor(RGB(66,255,55));
    dcSource.SetTextColor(m_rgbBackGround);
    dcSource.BitBlt(0, 0, Bmap.bmWidth,
					Bmap.bmHeight,
                    &dcMask, 0, 0, SRCAND);

	// Get Image rectangle

	CPoint ptCP = m_rectOuter.CenterPoint();

	m_rectImage = CRect(ptCP.x - (m_rectOuter.Width() / 3)/2, ptCP.y - (m_rectOuter.Height() - 12)/2,
						ptCP.x + (m_rectOuter.Width() / 3)/2, ptCP.y + (m_rectOuter.Height() - 12)/2);

	// mask destination

    dcSource.SetBkColor(RGB(44, 55, 255));
    dcSource.SetTextColor(RGB(44,33,255));
    pDC->StretchBlt(ptCP.x - (m_rectOuter.Width() / 3)/2,
                     ptCP.y - (m_rectOuter.Height() - 12)/2,
                     m_rectOuter.Width() / 3,
                     m_rectOuter.Height() - 12,
                     &dcMask,
                     0, 0,
                     Bmap.bmWidth,
					 Bmap.bmHeight,
                     SRCAND);

	// combine the two

    pDC->StretchBlt(ptCP.x - (m_rectOuter.Width() / 3)/2,
                     ptCP.y - (m_rectOuter.Height() - 12)/2,
                     (m_rectOuter.Width() / 3),
                     m_rectOuter.Height() - 12,
                     &dcSource,
                     0, 0,
                     Bmap.bmWidth,
					 Bmap.bmHeight,
                     SRCPAINT);

	 // release and delete extra bitmaps

    dcMask.SelectObject(pbmpOldMask);
    bmpMask.DeleteObject();

    dcSource.SelectObject(pbmpOldSource);
    pbmpSource->DeleteObject();

    delete pbmpSource;
  
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////

CBitmap* CWAButton::CreateCopyBitmap(CBitmap* pbmpSource, LPBITMAP lpBmap)
{
 // Make a copy of a bitmap
 // Return information in the struct pointed to by lpBmap

    pbmpSource->GetObject(sizeof(BITMAP), (LPSTR)lpBmap);
    DWORD dwBytes = (DWORD)lpBmap->bmHeight *
                     (DWORD)lpBmap->bmWidthBytes *
                     (DWORD)lpBmap->bmPlanes;
    GLOBALHANDLE hBits = GlobalAlloc(GMEM_MOVEABLE, dwBytes);
    if(hBits == NULL)
        return NULL;

    LPSTR lpBits = (LPSTR)GlobalLock(hBits);
    pbmpSource-> GetBitmapBits(dwBytes, lpBits);
    CBitmap* pbmpResult = new CBitmap;
    if(pbmpResult)
    {
        pbmpResult->CreateBitmapIndirect(lpBmap);
        pbmpResult->SetBitmapBits(dwBytes, lpBits);
    }
    GlobalUnlock(hBits);
    GlobalFree(hBits);

    return pbmpResult;
}

//! Creates a gradiant effect for the buttons
/*!
	\param  pDC - DC to paint the gradiant fill
	\param  rect - the rectangle to fill
*/

void CWAButton::DoGradientFill(CDC *pDC, CRect rect)
{
	CBrush pBrush[64];
	int nWidth = (rect.right) - (rect.left);
    int nHeight = (rect.bottom) - (rect.top);
    CRect rct;
    for (int i=0; i<64; i++)
	{
//		if(m_bOverControl)
		{
//			if(Focus)
			{
			//	pBrush[i].CreateSolidBrush(RGB(55-(i/4), 255-(i/4), 255-(i/3)));
			}
//			else
//			{
//				pBrush[i].CreateSolidBrush(RGB(255-(i/4), 255-(i/4), 255-(i/5)));			
//			}
//		}
//		else
//		{
//			if(Focus)
			{
//				pBrush[i].CreateSolidBrush(RGB(255-(i/3), 255-(i/3), 255-(i/4)));
			}
//			else
			{
				int nRed = GetRValue(m_rgbBackGround)-(i/3);
				int nGreen = GetGValue(m_rgbBackGround)-(i/3);
				int nBlue = GetBValue(m_rgbBackGround)-(i/5);
				pBrush[i].CreateSolidBrush(RGB((nRed < 0) ? 0 : nRed, (nGreen < 0) ? 0 : nGreen, (nBlue < 0) ? 0 : nBlue));
			}
		}
	}
	
	
    for (i=0/*rect.top*/; i<nHeight+2; i++) 
	{
        rct.SetRect (rect.left, rect.top + i, rect.left + nWidth+2, rect.top + i + 1);
        pDC->FillRect (&rct, &pBrush[((i * 63) / nHeight)]);
    }
	
    for (i=0; i<64; i++) 
	{
		pBrush[i].DeleteObject();
	}
	
}