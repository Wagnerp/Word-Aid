//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CWAButton
//
// Version 1.1.0.0
//
// Copyright 2001-2003 (C) - ACE Centre Advisory Trust
//
// Created	: 2001 by Jason Walsh
//
// Updated	: 14/05/03 by Jason Walsh
//
// WA0035	: Improved how the buttons are drawn so that background has
//			: a slight gradiant and the text uses a transparent mode
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// WAButton.h: interface for the CWAButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WABUTTON_H__5E535188_1095_4464_B98D_4F038F19745D__INCLUDED_)
#define AFX_WABUTTON_H__5E535188_1095_4464_B98D_4F038F19745D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWAButton  
{
friend class CChildWnd;

public:
	CWAButton(CRect rect = CRect(0,0,0,0), CString szName = _T(""), COLORREF back = RGB(192,192,192),
			  COLORREF fore = RGB(0, 0, 0), int nSize = 12, CString szFace = _T("Arial"), UINT nImageID = 0);
	virtual ~CWAButton();
	
	void Draw(CDC* pDC);

	int GetTop() {return m_rectOuter.top;}
	int GetLeft() {return m_rectOuter.left;}
	int GetBottom() {return m_rectOuter.bottom;}
	int GetRight() {return m_rectOuter.right;}
	int GetHeight() {return m_rectOuter.bottom - m_rectOuter.top;}
	int GetWidth() {return m_rectOuter.right - m_rectOuter.left;}
	void SetBoundingRect(CRect rect) {m_rectOuter = rect;}
	BOOL InRect(CPoint point){return m_rectOuter.PtInRect(point);}
	void EnableButton(BOOL bEnable = TRUE) {m_bEnabled = bEnable;}
	BOOL IsButtonEnabled() {return m_bEnabled;}
	void SetFontSize(int size) {m_fontSize = size;}
	void SetFontFace(CString face) {m_fontFace = face;}

	//! Sets the Text that is displayed in the Button
	void	SetText(CString strName)	{m_szName = strName;}		//WA0033
	//! Gets the text that is displayed in the button
	CString GetText()					{return m_szName;}			//WA0033
protected:

	BOOL DisplayBitmap(CDC* pDC);
	CBitmap* CreateCopyBitmap(CBitmap* pbmpSource, LPBITMAP lpBmap);
	virtual void DrawButtonFace(CDC* pDC);

	void DoGradientFill(CDC *pDC, CRect rect);

	CBrush m_brush;
	CPen m_penGrey;
	CPen m_penBlack;
	CPen m_penWhite;
	CFont m_fontText;
	CString m_szName;
	CRect m_rectOuter;
	CRect m_rectImage;
	COLORREF m_rgbBackGround;
	COLORREF m_rgbForeGround;
	BOOL m_bEnabled;
	int m_fontSize;
	CString m_fontFace;

	CBitmap m_bmpImage;
	int m_nImageID;

};

#endif // !defined(AFX_WABUTTON_H__5E535188_1095_4464_B98D_4F038F19745D__INCLUDED_)
