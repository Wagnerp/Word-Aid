//! Definition of the window class, CChildWnd, that displays the current wordlist
/*! This class makes use of settings in CSetting and also loads the keyhook dll.
	The size of the window is determined by the number of words being displayed and
	the font size selected for the window text. The window acts like an owner drawn
	list box.
*/
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0017 - add speech indication when there are no more words above or below the		//
//			current list and display stop button rather than up or down as appropriate	//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0018 - solve problem with window not wide enough when return from displaying		//
//			abbreviation window															//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//

#if !defined(AFX_CHILDWND_H__1FE33626_C698_4364_9D00_7FD1126C8D57__INCLUDED_)
#define AFX_CHILDWND_H__1FE33626_C698_4364_9D00_7FD1126C8D57__INCLUDED_

//#include "ProcessKey.h"	// Added by ClassView
#include "Wordlist.h"	// Added by ClassView

#include <afxtempl.h>	// For CArray

#include "MyServ.h"	// DDE Server

#include "WAButton.h"					// For Abbrev Button
#include "DownButton.h"					// For Down Button
#include "UpButton.h"					// For Up Button
#include "stopbutton.h"					//WA0017 for stop button
#include "setting.h"
#include "textspeaker.h"				// Added by ClassView
#include "AbbrevProcessKey.h"			// Class to Process Abbrev Keys
#include "WorAidAbbrevProccessKey.h"	// Class to process WordAId and Abbrev keys
#include "WordAidProcessKey.h"			// Class to process WordAid keys

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TIMER IDS
#define TIMER_SCROLL		1
#define TIMER_ABBREV		2
#define TIMER_OUTPUT		3

/////////////////////////////////////////////////////////////////////////////
//! typedefs for keyhook dll functions

typedef BOOL (WINAPI* ENABLE) (HWND hWnd);
typedef BOOL (WINAPI* ISENABLED) ();
typedef BOOL (WINAPI* DISABLE) ();
typedef BOOL (WINAPI* CLEAR_MONITORS) ();
typedef BOOL (WINAPI* MONITOR) (WORD nVK,int nKeyState,BOOL bCapture);
typedef BOOL (WINAPI* SET_MESSAGES) (WORD nKeyDownMsg, WORD nKeyUpMsg, WORD nKeyMatchedMsg);
typedef BOOL (WINAPI* OUTPUT_STRING) (LPCSTR lpsz);
typedef BOOL (WINAPI* GET_CAPS_LOCK) ();
typedef void (WINAPI* SET_CAPS_LOCK) (BOOL bState);
typedef BOOL (WINAPI* GET_SHIFT) ();
//! struct used to define position and content of each word in the list window
 struct TEXT_AREA
	{	//! the word to be displayed
		CString szText;			
		//! the rectangle to display it in
		CRect   rectBounding;	
	};

class CChildWnd : public CWnd
{
	friend class CMainFrame;
	friend class CDefaultProcessKey;		//WA0017
// Construction
public:
	CChildWnd();
	virtual ~CChildWnd();
	
	// WordAid Methods
	void SetWordlistA(CWordlist alphalist);
	void SetWordlistT(CWordlist topiclist);
	CWordlist GetWordlistA();
	CWordlist GetWordlistT();
	void DisplayList(CWordlist list);				// Display the Alpha or Topic list

	// Abbrev Methods
	// WA0047
	//void SetAbbrevList(CAbbrevList* abbrevList);
	void SetAbbrevList(CAbbrevList abbrevList);
	//CAbbrevList* GetAbbrevList();		// WA0047
	CAbbrevList GetAbbrevList();
	// WA0047
	//void DisplayAbbrevList(CAbbrevList* list);		// Display Abbreviation list
	void DisplayAbbrevList(CAbbrevList list);		// Display Abbreviation list
	BOOL SetAbbrevSelectionTimer();					// Sets the timer for the Abbreviation insert

	// Set the Abbrev button displayed or hidden
	void DisplayAbbrevButton(BOOL bDisplayed = TRUE) {m_bAbbrevButton = bDisplayed;}	

	// Popup Methods
	BOOL MoveDownList();
	BOOL MoveUpList();
	BOOL MovePopupLeft();
	BOOL MovePopupRight();
	virtual BOOL Create(HWND hWnd);
	int GetNewWindowPos(BOOL forward);
	void SetSorted(BOOL sorted = TRUE);
	void ResizeWindow();					//! calculate size of window to display and redraw it
	BOOL IsVisible()	{return m_bShow;}
	void SetVisible(BOOL bVisible = TRUE)	{m_bShow = bVisible;}
	int GetMaxItemsDisplayed() {return m_nMaxNumberOfItems;}	// Returns the maximum number of items
																// displayed in the popup window
	void EnableDownButton(BOOL bEnable = TRUE) {m_pBtnDown->EnableButton(bEnable);}
	void EnableUpButton(BOOL bEnable = TRUE) {m_pBtnUp->EnableButton(bEnable);}
	void HideWindow() {MoveWindow(0,0,0,0,TRUE);}	// Hides the window by making it 0 size

	// Keyhook methods
	BOOL KeyhookLoaded();
	void EnableKeyhook();
	void DisableKeyhook();
	void OutputString(CString str, UINT count, BOOL bDDE = FALSE);		//! output the selected string to application
	void SetMonitorsHiddenWnd();
	void SetMonitorKeys();
	BOOL SendString(CString strSend);				// Sends a string to output window.
	void DeleteOutput(CString str);

	// Highlighted word functions
	int GetCurSel() {return m_nWordlistPos;}		// Rerturns the currently selected index
	int GetSize() {return m_TextList.GetSize();}	// Return number of items displayed
	CString GetCurrentSelectText() {return m_TextList[m_nWordlistPos].szText;}
	CString GetCurrentSelectText(int nIndex) 
		{ASSERT(nIndex <= m_TextList.GetUpperBound()); return m_TextList[nIndex].szText;}

	// Speech Functions
	BOOL CanSpeak() {return (m_ctrlSpeaker) ? TRUE : FALSE;}
	void SayText(CString szText) {m_ctrlSpeaker.Speak(szText);}
	void SayCurrentSelection();

	// DDE Methods
	BOOL OpenAlpha(CString strFilename);
	BOOL OpenTopic(CString strFilename);
	BOOL OpenAbbrev(CString strFilename);
	BOOL UndoFull();
	BOOL UndoPart();
	void ResetDDEString();

	// WordAid Attributes

	// Abbrev Attributes
	CAbbrevProcessKey m_procAbbrevKey;					// Process keys for Abbrev
	CWorAidAbbrevProccessKey m_procWordAidAbbrevKey;	// Process Keys for Abbrev and WordAid
	CWordAidProcessKey m_procWordAidKey;				// Process Keys for WordAid

	// Popup Attirbutes
	CSetting* m_pSettings;		//! pointer to class containing settings for this user
	int m_nHighlighted;			//! index into textarea array giving current highlighted word (using mouse)
	//CProcessKey m_prockKey;		//! link to class responsible for processing input
	CRect m_rectButSize;		//! calculated size of buttons
	CWAButton*  m_pBtnUp;		//! owner drawn up button
	CWAButton* m_pBtnDown;		//! owner drawn down button
	CWAButton* m_pBtnAbbrev;	//! Owner drawn Abbrev Button
	CWAButton* m_pBtnStop;		//WA0017 owner drawn Stop button
	CString m_strAppPathName;	// path to .ext location

	// Sticky Keys methods
	BOOL IsStickyKeysOn()	{return m_bStickyKeysOn;}
	BOOL IsStickySHIFT()	{return m_bStickySHIFT;}
	BOOL IsStickyCTRL()		{return m_bSitckyCTRL;}
	BOOL IsStickyALT()		{return m_bStickyALT;}

	// Keys
	BOOL IsSHIFTDown()		{ return m_bShiftDown;}

	// DDE
	//! WA0057 -Sends a DDE Message to the client
	/*!
		\param strType - the type of message for ie alpha, topic or abbrev
		\param strMessage - the wordlist to send
	*/
	void SendDDEMessage(CString strType, CString strMessage)
				{m_pDDEServer.SendDDEMessage(strType, _T("wordlist"), strMessage);}
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildWnd)
	public:
	//}}AFX_VIRTUAL
private:

	// Popup Methods
	void CheckStickyKeys();
	void UpdateFont();
	void CalculateWndGrid(int w, int h);
	// WA0032
	//void HighlightWord(CRect rect, CString word);	
	void HighlightWord(int nSel);
	// WA0032
	//void Unhighlight(CRect rect, CString word);		
	void UnHighlight(int nSel);		// WA0032
	BOOL LoadLibraryDll(CString);	//! load keyhook dll

	// Functions to handle key press for WordAid and Abbrev
	BOOL WordAidKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnWordAidKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	// Popup Attibutes
	BOOL m_bSpaceOutput;
	int m_nSizeLastOutput;
	BOOL m_bShow;					//! TRUE = window hidden, FALSE = window showing
	HINSTANCE m_hLibraryInstance;	//! pointer to keyhook dll
	
	//! pointers to keyhook functions	
	ENABLE			lpfnEnable;
	ISENABLED		lpfnIsEnabled;
	DISABLE			lpfnDisable;
	CLEAR_MONITORS	lpfnClearMonitor;
	MONITOR			lpfnMonitor;
	OUTPUT_STRING	lpfnOutputString;
	SET_MESSAGES	lpfnSetMessages;
	GET_CAPS_LOCK	lpfnGetCapsLock;
	SET_CAPS_LOCK	lpfnSetCapsLock;
	GET_SHIFT		lpfnGetShift;
	

	HWND m_hActiveWnd;			// Stores the active window

protected:
	BOOL m_bFirstWord;			//WA0027 - TRUE if this is first word in list
	// Attributes
	BOOL m_bShowingAbbrev;		//! TRUE if Abbrev list is being displayed.  Used so that the key is not appended to it.
	BOOL m_bSpeech;				//! TRUE = use speech, FALSE = speech off
	CTextSpeaker m_ctrlSpeaker;	//! ActiveX speech control
	int m_nWordlistPos;			//! index into m_TextList array for current cursor position in wordlist (arrow keys)
	CFont m_fontList;			//! font used for list window
	int m_nWindowPos;			//! index into m_rectArrayWndPositions for current position
	CPoint m_rectArrayWndPositions[15];	//! array of possible window positions
	BOOL m_bButtonsOn;			//! Whether the buttons are on or off
	int m_nLargestWordSize;		//! Stores the longest word set when adding string to window
	CArray<TEXT_AREA, TEXT_AREA&> m_TextList;	//! word list and relevant screen positions
	int m_nMaxNumberOfItems;	//! maximum number of words that should be displayed
	CMyServer m_pDDEServer;	//! pointer to DDE server for wordaid
	bool m_bAltDown;			// Has alt keydown been received?
	bool m_bCtrlDown;			// Has ctrl keydown been received?
	bool m_bShiftDown;			// Has shift keydown been received?
	int m_nHideWindowHotkey;	// integer value of hide window hotkey
	int m_nHideWindowSpecial;	// hide window special key 
	int m_nUndoHotkeyVal;		// integer value of undo hotkey
	int m_nUndoSpecial;			// undo special key
	BOOL m_bWordInserted;		// TRUE if a Word has been inserted.  Used to stop a word from being said
								// after just being inserted.

	// Sitcky Keys attibutes
	BOOL m_bStickyKeysOn;		// TRUE if Sticky keys are on
	BOOL m_bStickySHIFT;		// TRUE if the Shift keys has been pressed and sticky keys is on
	BOOL m_bSitckyCTRL;			// TRUE if the CTRL keys has been pressed and sticky keys is on
	BOOL m_bStickyALT;			// TRUE if the ALT keys has been pressed and sticky keys is on

	//! Set to TRUE when the Window is shown by the user pressing the Hotkey, this is to stop the
	//! voice from being reset that seems to use memory - WA0046
	BOOL m_bHotkeyShow;	

	// Methods
	void ResetContent();									//! empty the wordlist
	int AddString(CString szText, BOOL bAddKey = TRUE);		//! add string to list window	
	CRect CalculateDownButtonBoundingRect();				//! calculates size of down button
	CRect CalculateUpButtonBoundingRect();					//! calculates size of up button
	CRect CalculateAbbrevButtonBoundingRect();				//! calculate size of the abbrev Button
	CString AddKeyToString(const CString szText, int nNum);	// Adds key to word
	CString GetClassName();									//! Methods to Setup popup
	int ConvertToFuncKey(CString key);			// Convert a function key registry setting to it's virtual key code
	int ConvertToSpecialKey(CString str);		// convert special key string to virtual key value
	bool CheckSpecialKey(int key);				// check whether this special key has been pressed
	int GetKeyTypes(CString strKeys);			// Converts a set of character to the special keys for keyhook
	BYTE CharToVK(CString strChar);				// Converts a char to its virtual key
	BOOL DefaultPoppupKeys(UINT nChar);			// Called to process the default keys to move around popup

	void CreateButtons();

	void SetAbbrevButtonText();		//WA0033

	void MoveCursorOffPopup();		// WA0051 - This moves the cursor off the popup if it is on it

	//{{AFX_MSG(CChildWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nchar,UINT nRepCnt,UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg LRESULT  OnUserKeyMatched(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage( MSG* pMsg );
public:
	void IsShowingAbbrev(BOOL val) { m_bShowingAbbrev = val; }	//WA0018
	afx_msg void OnTimer(UINT nIDEvent);
	BOOL m_bAbbrevButton;		//! TRUE if the Abbrev Button should be displayed in popup window

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDWND_H__1FE33626_C698_4364_9D00_7FD1126C8D57__INCLUDED_)
