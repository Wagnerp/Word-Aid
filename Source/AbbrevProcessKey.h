#pragma once

#include "DefaultProcessKey.h"


class CAbbrevProcessKey : public CDefaultProcessKey
{
public:
	CAbbrevProcessKey(CSetting* settings = NULL, CChildWnd* pPopupWnd = NULL);
	~CAbbrevProcessKey(void);

	virtual BOOL OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL KeyDown(UINT nVK);
	BOOL KeyUp(UINT nVK) {return TRUE;}
	virtual void OnButtonDown();
	virtual void OnButtonUp();

	// Methods
	void DisplayList(CString szText = _T(""));

private:
	// Methods
	BOOL ProcessKey(UINT nVK, UINT nChar);
	
};
