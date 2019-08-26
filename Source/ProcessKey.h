// ProcessKey.h: interface for the CProcessKey class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_PROCESSKEY_H__ACD00669_4C7A_42C1_B048_DB97D92923C9__INCLUDED_)
#define AFX_PROCESSKEY_H__ACD00669_4C7A_42C1_B048_DB97D92923C9__INCLUDED_

#include "Wordlist.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum key {FUNC,NUM,NUMPAD,NOKEY} KEYS;

class CProcessKey  
{
private:
	BOOL ProcessNumber(UINT nChar, CString* str);
	BOOL ProcessFuncKey(UINT nChar, CString* str);
	BOOL ProcessNumberPad(UINT nChar, CString* str);

	BOOL DeleteAlpha();
	BOOL DeleteTopic();
	BOOL InsertAlpha();
	BOOL InsertTopic();
	BOOL CheckDelete();
	BOOL CheckInsert();

	BOOL m_bTopicListDisplayed;		// Set to TRUE if topic list is being displayed
public:
	BOOL m_bStickyKeys;
	CWordlist m_listTopic;	//holds the current topic list
	BOOL m_bMatched;	//has the string been matched?
	UINT m_nMaxNumWords;
	CWordlist OnUpButton();
	CWordlist OnDownButton();
	BOOL SelectWord(UINT index, CString* word);

	BOOL m_bCapitalNeeded;
	BOOL m_bAutoCaps;
	BOOL m_bPunctuationMark;
	KEYS m_bFuncKeys; // are function keys(TRUE)/numbers(FALSE) used to select word
	
	int m_nScrolled;		// indicates if up/down buttons have been used on wordlist
	CString m_strOldString;	// holds the old search string for undo function
	CWordlist m_listAlpha;	// holds the whole wordlist
	CString m_strChars;		// holds the string currently being used for searching

	void SetSorted(BOOL sorted) { m_listAlpha.m_bSorted = sorted;}

	CWordlist OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	CString OnKeyDown(UINT nChar, BOOL* remove, int* count);
	CWordlist OnKeyUp(UINT nChar);

	void ClearSearchString();


	CProcessKey();
	virtual ~CProcessKey();

protected:
};

#endif // !defined(AFX_PROCESSKEY_H__ACD00669_4C7A_42C1_B048_DB97D92923C9__INCLUDED_)
