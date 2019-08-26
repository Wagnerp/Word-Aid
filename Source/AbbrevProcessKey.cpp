#include "stdafx.h"
#include "abbrevprocesskey.h"
#include "ChildWnd.h"		// Popup Window Class
#include "VKConversion.h"

////////////////////////////////////////////////////////////////////////////////

CAbbrevProcessKey::CAbbrevProcessKey(CSetting* settings /*= NULL*/,
									 CChildWnd* pPopupWnd /*= NULL*/)
	: CDefaultProcessKey(settings, pPopupWnd)
{
}

////////////////////////////////////////////////////////////////////////////////

CAbbrevProcessKey::~CAbbrevProcessKey(void)
{
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when a WM_KEYDOWN message is recieved in the popup 
//				: window.  Handles how the key should be processed.
//
// PARAMETERS	: Standard OnKeyDown Parameters.  See MSDN OnKeyDown for 
//				: description.
//
// RETURNS		: TRUE
////////////////////////////////////////////////////////////////////////////////

BOOL CAbbrevProcessKey::KeyDown(UINT nVK)
{
	BOOL bReturn = TRUE;

	// Must first check to see that it is not semi automatic so that method to 
	// add space after punctuation is only called if the expansion method is not
	// semi automatic
	if(m_pSettings->GetSetting(ABBREV_EXPANSION_METHOD) != SEMI_AUTOMATIC &&
 		SpaceAfterPunctuation(nVK))
	{
		bReturn = TRUE;
	}
	// REMOVED 11/07/02
   	/*else if(!ProcessAbbrevKeyDown(nVK))		// Check to see if character is a hotkey
	{
		m_lstDisplay.RemoveAll();

		int nChar;
		ProcessKey(nVK, nChar);	// Get Abbrev List
	}*/

	return bReturn;
}

BOOL CAbbrevProcessKey::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nVK = CVKConversion::CharToVK((char)nChar);

	// Check to see if a word has been inserted and whether the next character
	// is a punctuation character.
	RemoveSpaceAfterInsert(nChar);

	if(!ProcessAbbrevKeyDown(nVK))		// Check to see if character is a hotkey
	{
		// WA0058 - Stop the CONTROL + ALT when pressed from procssing
		if(HIBYTE(GetKeyState(VK_CONTROL)) || HIBYTE(GetKeyState(VK_MENU)))
		{
			return FALSE;
		}
		m_lstDisplay.RemoveAll();

		ProcessKey(nVK, nChar);	// Get Abbrev List
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to store the character and to search the abbreviation 
//				: to get the list that should be displayed.  If Word Bank and 
//				: Listed Expansion are not on then an empty list is returned.
//
// PARAMETERS	: nChar - Virtual key that is pressed
//
// RETURNS		: A llist of found abbreviations that match the stored string
//				: if listed expansion is on.  If Word Bank is on the whole 
//				: abbreviation list is returned; Otherwise a blank list is
//				: returned.
////////////////////////////////////////////////////////////////////////////////

BOOL CAbbrevProcessKey::ProcessKey(UINT nVK, UINT nChar)
{
	CAbbrevList list;

	// Process Sticky keys
	if(m_pWndPopup->IsStickyKeysOn())
	{
		// Control or ALT then do no further processing
		if(m_pWndPopup->IsStickyCTRL())
		{
			return FALSE;
		}
		else if(m_pWndPopup->IsStickyALT())
		{
			return FALSE;
		}
	}

	if(NonKey(nVK))		// Do not process these keys
	{
		return FALSE;
	}

	// Store the Character if not a Hotkey
	if(!bAbbrebHotkey(nVK))
	{
		m_bExpanded = FALSE;		// Set to FALSE as something has been typed
		ResetScroll();				// Reset the scroll page

		if(nVK == VK_BACK)		// If Backspace remove character
		{
			if(!m_strTyped.IsEmpty())
			{
				m_strTyped = m_strTyped.Left(m_strTyped.GetLength() - 1);
			}
		}
		else
		{
			m_strTyped += CapitaliseAfterPunc(nChar);
		}
	}
	else 	// Reset Typed String
	{
		//TODO : Will not be like this always need to change
		m_strTyped = _T("");
		m_bExpanded = FALSE;	// Set to FALSE as something has been typed
	}

	DisplayList(m_strTyped);	// Display the correct abbreviation list in the
								// popup window

	/*if(atoi(m_pSettings->GetSetting(ABBREV_LISTED_EXPANSION)))
	{
		m_lstDisplay = m_AbbrevList->Search(m_strTyped);
	}

	// If WordBank is on and there is nothing in the list (meaning either 'listed
	// exapansion' is not on or did not find anything then display all the abbrev
	// list.
	if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) &&  m_lstDisplay.GetSize() == 0)
	{
		m_lstDisplay = m_AbbrevList->GetAll();
	}

	// Display the list in Popup Window if Word Bank or Listed Expansion on
	if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) ||
		atoi(m_pSettings->GetSetting(ABBREV_LISTED_EXPANSION)))
	{
		m_pWndPopup->DisplayAbbrevList(&m_lstDisplay);
	}
	else
	{
		m_pWndPopup->ShowWindow(SW_HIDE);
	}*/

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to Display the list in the poup window
//
// PARAMETERS	: Any characters that have been typed
/////////////////////////////////////////////////////////////////////////////////

void CAbbrevProcessKey::DisplayList(CString strText /*= _T("")*/)
{
	if(atoi(m_pSettings->GetSetting(ABBREV_LISTED_EXPANSION)) || atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
	{
//		if(m_AbbrevList)		//WA0047
		{
			// Search list for typed string
			m_lstDisplay = m_AbbrevList.Search(strText);
		}
//		m_pWndPopup->DisplayAbbrevList(&m_lstDisplay);
	}
	
	// Display the whole abbreviation list if WordBank is on and nothing was
	// found in the search of the list if it was carried out.
	if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) && m_lstDisplay.GetSize() == 0)
	{
	//	if(m_AbbrevList)		//WA0047
		{
			//m_pWndPopup->DisplayAbbrevList(m_AbbrevList);
			m_lstDisplay = m_AbbrevList.GetAll();
		}
	}

	// Display the list in Popup Window if Word Bank or Listed Expansion on
	if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) ||
		atoi(m_pSettings->GetSetting(ABBREV_LISTED_EXPANSION)))
	{
		m_pWndPopup->DisplayAbbrevList(m_lstDisplay);
	}
	else
	{
		m_pWndPopup->SetVisible(FALSE);
		//m_pWndPopup->ShowWindow(SW_HIDE);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when the Down button is pressed to move down a list.
//				: This function moves down the abbrev list
/////////////////////////////////////////////////////////////////////////////////

void CAbbrevProcessKey::OnButtonDown()
{

	if(NextPage(m_lstDisplay))
	{
		m_pWndPopup->DisplayAbbrevList(m_lstDisplay);
		m_pWndPopup->EnableUpButton();
	}
	else
	{
		m_pWndPopup->EnableDownButton(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when the Up button is pressed to move down a list.
//				: This function moves Up the abbrev list
/////////////////////////////////////////////////////////////////////////////////

void CAbbrevProcessKey::OnButtonUp()
{
	if(PrevPage())
	{
		m_pWndPopup->DisplayAbbrevList(m_lstDisplay);
		m_pWndPopup->EnableDownButton();
	}
	else
	{
		m_pWndPopup->EnableUpButton(FALSE);
	}
}