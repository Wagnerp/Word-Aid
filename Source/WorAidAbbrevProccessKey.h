#pragma once

#include "DefaultProcessKey.h"

class CWorAidAbbrevProccessKey : public CDefaultProcessKey
{
public:
	void ProcessAbbrevBtnHotkey(UINT nChar);
	CWorAidAbbrevProccessKey(CSetting* settings = NULL, CChildWnd* pPopupWnd = NULL);
	~CWorAidAbbrevProccessKey(void);

	BOOL KeyDown(UINT nVk);
	BOOL KeyUp(UINT nVK);

	CAbbrevList GetStoredAbbrevList() {return m_AbbrevStoreList;}
	//WA0047
//	void SetAbbrevList(CAbbrevList* list);
	void SetAbbrevList(CAbbrevList list);

	virtual BOOL OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnButtonDown();
	void OnButtonUp();
	void OnAbbrevButtonDown();

	void DisplayList(CString strText = _T(""));	// Display the list

	UINT GetAlphaOrTopic();		// WA0033

	// Attributes
	CAbbrevList m_AbbrevStoreList;		// Stores the list that has been found so that it can
											// be displayed after topic or alpha list.
private:
	// Methods
	BOOL ProcessKey(UINT nVK, UINT nChar);				// Process key and display required list.
	

};
