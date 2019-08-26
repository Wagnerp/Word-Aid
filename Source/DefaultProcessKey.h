#pragma once

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: This class is an abstract class that contains the default
//				: implementation for the processing of keys in the child window.
//				: All class that define the beahviour of the keys in the Popupwindow
//				: should be inherit from this.  The class was created becuase
//				: there were a number of repitions in the code between the differnt
//				: classes that handle the Popup Widnows key processing.
////////////////////////////////////////////////////////////////////////////////
//
//	WA0021 - solve problem with abbrev button hotkey special key not being stored
//
////////////////////////////////////////////////////////////////////////////////
//

#include "Setting.h"	// Setting class
#include "Wordlist.h"	// Wordlist and Abbrev list classes

class CChildWnd;

class CDefaultProcessKey
{
public:
	virtual void ProcessAbbrevBtnHotkey(UINT nChar) {}		//WA0021
	CDefaultProcessKey(CSetting* settings = NULL, CChildWnd* pPopupWnd = NULL);
	~CDefaultProcessKey();

	// General Methods
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) = 0;
	virtual BOOL KeyDown(UINT nVK) = 0;
	virtual BOOL KeyUp(UINT nVK) = 0;
	virtual void OnButtonDown() =0;
	virtual void OnButtonUp() = 0;
	
	void SetSettings(CSetting* settings) {m_pSettings = settings;}
	void SetPopupWindow(CChildWnd* pWnd) {m_pWndPopup = pWnd;}
	
	int GetNoScrolled() {return m_nScrolled;};

	BOOL LoadOriginalTopicList();		// WA0056

	//WA0047
	//BOOL NextPage(CAbbrevList* list);	// Move to next page of in abbrev list					
	BOOL NextPage(CAbbrevList list);	// Move to next page of in abbrev list					
	BOOL NextPage(CWordlist list);		// Move to next page in a wordlist
	BOOL PrevPage();	 				// Moave to prev page in abbrev list
	void ResetScroll() {m_nScrolled = 0;}	// Resets the scroll back to 0

	BOOL GotChars() {return !m_strTyped.IsEmpty();}
	CString GetCharsTyped() {return m_strTyped;}
	virtual void ClearTyped() {m_strTyped = _T("");DisplayList(m_strTyped);}

	void AbbrevListDisplayed() {m_nListDisplayed = ABBREV;}
	void AlphaListDisplayed() {m_nListDisplayed = ALPHA;}
	void TopicListDisplayed() {m_nListDisplayed = TOPIC;}
	BOOL IsAbbrevDisplayed()	{return (m_nListDisplayed == ABBREV);}
	BOOL IsAlphaDisplayed()	{return (m_nListDisplayed == ALPHA);}
	BOOL IsTopicDisplayed() {return (m_nListDisplayed == TOPIC);}

	// Abbrev Methods
	BOOL ExpandAbbrev(UINT nChar = 0);		// Expands an abbreviation
	BOOL ProcessAbbrevHotkey(UINT nChar);	// Is Abbrev hotkey and was it processed	
	void SetAbbrevOnly()	{ m_nListDisplayed = ABBREV;}

	// Formats the Expansion depending upon typed abbreviation
	// WA0047
	//void FormatExpansion(CAbbrevList* list, CString strTyped);	
	void FormatExpansion(CAbbrevList list, CString strTyped);	

	// Set whether the abbreviation has been expanded
	void SetExpanded(BOOL bExpanded = TRUE) {m_bExpanded = bExpanded;}	

	void AbbrevSayFormat(CString& strSay);
	BOOL UnExpandFull();
	BOOL UnExpandPartial();
	BOOL SelectAbbrev(int nIndex);			// Selects an Abbreviation from the displayed list
	void MouseSelectAbbrev(UINT index);				// Expands an abbreviation when selected via the mouse

	// WordAid Methods
	void SelectWord(int nIndex, BOOL bDDE = FALSE);			// Selects a word at a particular index
	BOOL ProcessWordAidHotkey(UINT nChar);	// Is WordAid hotkey and was it processed	
	BOOL InsertAlpha();						// Insert into alpha list
	BOOL InsertTopic();						// Insert into topic list
	BOOL DeleteAlpha();						// Delets a word from the list
	BOOL DeleteTopic();						// Deletes a word from the list

	// For DDE
	BOOL Undo();							// Undo last insert
	void SendDDEFirstUpperCase();	// WA0057
	void SendDDEAllUpperCase();		// WA0057
	void SendDDEAllLowerCase();		// WA0057

	// Public Attributes
	//WA0047
	//CAbbrevList* m_AbbrevList;	// Lists
	CAbbrevList m_AbbrevList;	// Lists
	CWordlist m_AlphaList;
	CWordlist m_TopicList;
	CAbbrevList m_lstDisplay;		// The list of Abbreviations that are being displayed
	CWordlist m_lstWordDisplay;		// The Wordlist that is being displayed
	CString m_strText;				// The Text that has been inserted.

	virtual void DisplayList(CString strText = _T("")) {} // Display the lists in popup window

protected:
	BOOL IsNumber(char c);					//WA0014 function added
	// Abbrev Methods
	//BOOL ProcessAbbrevHotkey(UINT nChar);	// Is Abbrev hotkey and was it processed	
	BOOL ProcessAbbrevKeyDown(UINT nChar);	// Is the Semi-Automatic key pressed
	BOOL IsSemiAutomaticKey(UINT nVK);
	BOOL IsManualHotkey(UINT nVK);
	BOOL bAbbrebHotkey(UINT nChar);			// Is key an Abbrev Hotkey	
	void SendShiftString(CString strText);	// Used to output string when SHIFT key is down
	BOOL SpecialAbbrevCase(UINT nVK);		// Checks to see if the abbreivation should be expanded
											// See method for more details

	// WordAid Methods
	BOOL bWordAidHotkey(UINT nChar);		// Is key an WordAid Hotkey		
	BOOL ProcessWordAidSelectionKeys(UINT nVK);
	BOOL ProcessShowPopupKey(UINT nVK);		// Show PopupWindow?
	BOOL ProcessUndoKey(UINT nVK);			// Undo Hotkey?
	BOOL ProcessInsertKey(UINT nVK);		// Insert hotkey?
	BOOL ProcessSayWordKey(UINT nVK);		// Say Word hotkey?
	BOOL ProcessAddDeleteWordKey(UINT nVK);	// Add Word to wordlist
	BOOL CheckAdd();
	BOOL CheckDelete();
	BOOL SelectFunctionKeys(UINT nVK);
	BOOL SelectNumberKeys(UINT nVK);
	BOOL SelectNumPadKeys(UINT nVK);

	// General Methods
	BOOL IsCapital(CString strText);		// Whether the string conatins all capitals
	BOOL NonKey(UINT nVK);					// A key that should be processed.
	BOOL SpaceAfterPunctuation(UINT nVK);	// Checks to see if a space should be inserted after punctuation
	BOOL IsCharPunctuation(UINT nVK);		// TRUE if the code is a punctuation character
	CString CapitaliseAfterPunc(UINT nVK);	// Checks to see if the character should be captialised
	void SimulateKeyPress(UINT nVK);		// Simulates a virtual keys extended key press
	BOOL HasNumber(CString str);			// Checks to see if a string contains a number
	BOOL RemoveSpaceAfterInsert(UINT nChar);// Remove the space after the insert and places back the character

	virtual BOOL ProcessKey(UINT nVK, UINT nChar) =0;	// Process key and display required list.

	// Attributes
	CSetting* m_pSettings;			// THe settings for user
	CChildWnd* m_pWndPopup;			// Pointer to the Popup Window
	CString m_strStore;				// Lasted typed words used for Un-expand
	CString m_strTyped;				// Stores the currently typed character used for searching abbrev list
	BOOL m_bExpanded;				// Whether an expansion has taken place
	BOOL m_bWordInserted;			// Whether a word has just been inserted
	int m_nScrolled;				// Number of times paged down
//	CString m_strText;				// The Text that has been inserted.
	UINT m_nListDisplayed;			// Set to which list is being used in the processing.  This is set to one
									// of the values in Wordlist.h
	BOOL m_bLastCharPunc;			// TRUE if last character was a punctuation mark
	//! TRUE if the Window has been hidden using the F12 key.
	BOOL m_bWindowHidden;	//WA0034
};
