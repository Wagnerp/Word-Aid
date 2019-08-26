#pragma once
#include "defaultprocesskey.h"

class CWordAidProcessKey :
	public CDefaultProcessKey
{
public:
	CWordAidProcessKey(void);
	~CWordAidProcessKey(void);

	virtual BOOL OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL KeyDown(UINT nVK);
	virtual BOOL KeyUp(UINT nVK);
	virtual void OnButtonDown();
	virtual void OnButtonUp();

	virtual void DisplayList(CString strText = _T("")); // Display the lists in popup window
protected:
	virtual BOOL ProcessKey(UINT nVK, UINT nChar);	// Process key and display required list.
	
};
