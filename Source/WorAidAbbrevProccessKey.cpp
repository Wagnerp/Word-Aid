//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//  WA0005 - apply capitalisation to topic lists as required                            //
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0015 - solve problem with punctuation not clearing wordlists						//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0018 - solve problem with window not wide enough when return from displaying		//
//			abbreviation window															//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0026 - stop auto space working when popup window hidden							//	
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//

#include "stdafx.h"
#include "woraidabbrevproccesskey.h"
#include "ChildWnd.h"		// Popup Window Class
#include "wordaid2.h"	//added 17th July
#include "VKConversion.h"
// punctuation keys
const CString SEMI_SPACE		= _T(" ");
const CString SEMI_FULLSTOP		= _T(".");
const CString SEMI_COMMA		= _T(",");
const CString SEMI_QUESTION		= _T("?");
const CString SEMI_EXCLAMATION	= _T("!");
const CString SEMI_COLON		= _T(":");
const CString SEMI_SEMI_COLON	= _T(";");

////////////////////////////////////////////////////////////////////////////////

CWorAidAbbrevProccessKey::CWorAidAbbrevProccessKey(CSetting* settings /*= NULL*/,
												   CChildWnd* pPopupWnd /*= NULL*/)
	: CDefaultProcessKey(settings, pPopupWnd)
{
}

////////////////////////////////////////////////////////////////////////////////

CWorAidAbbrevProccessKey::~CWorAidAbbrevProccessKey(void)
{
}

////////////////////////////////////////////////////////////////////////////////
// DESCIPTION	: Called when the WM_KEYDOWN message is recieved in the Popup
//				: window.  This function handles the processing of the virtual
//				: key code.
////////////////////////////////////////////////////////////////////////////////

BOOL CWorAidAbbrevProccessKey::KeyDown(UINT nVK)
{

	BOOL bReturn = TRUE;

	if(nVK == VK_SHIFT)
	{
		if(m_nListDisplayed == ALPHA || m_nListDisplayed == TOPIC)
		{
			// handle shift key
			CWordlist temp(m_AlphaList.Search(m_strTyped));
			if (!m_strTyped.IsEmpty() && isupper(m_strTyped[0]))
			{
				temp.Capitalise();
				m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
				m_pWndPopup->DisplayList(temp);
			}
			else if(!m_strTyped.IsEmpty())
			{
				temp.InitialCapital();
				m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
				m_pWndPopup->DisplayList(temp);
			}
			else
			{
				// Display Topic list with intial letter capitalised
				CWordlist topic(m_TopicList);

				topic.InitialCapital();
				m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
				m_pWndPopup->DisplayList(topic);
			}
		}
		return bReturn;
	}

	return bReturn;
}

BOOL CWorAidAbbrevProccessKey::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL bReturn = TRUE;

	// WA0058 - Stop the CONTROL + ALT when pressed from procssing
	if(HIBYTE(GetKeyState(VK_CONTROL)) || HIBYTE(GetKeyState(VK_MENU)))
	{
		return FALSE;
	}

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

	// Check to see if a word has been inserted and whether the next character
	// is a punctuation character.
	RemoveSpaceAfterInsert(nChar);

	int nVK = CVKConversion::CharToVK((char)nChar);

	// Must first check to see that it is not semi automatic so that method to 
	// add space after punctuation is only called if the expansion method is not
	// semi automatic
	if(m_pSettings->GetSetting(ABBREV_EXPANSION_METHOD) != SEMI_AUTOMATIC &&
		SpaceAfterPunctuation(nVK) && !m_pWndPopup->IsVisible())	//	WA0026 added condition
	{
 		if (nChar == VK_SPACE)			//WA0015
			bReturn = TRUE;
		else							//WA0015
			ProcessKey(nVK,nChar);		//WA0015
	}
 	else if(/*!ProcessWordAidHotkey(nChar) &&*/ !ProcessAbbrevKeyDown(nVK))
	{
		ProcessKey(nVK, nChar);
	}

	return bReturn;
}

BOOL CWorAidAbbrevProccessKey::KeyUp(UINT nVK)
{
	if (nVK == VK_SHIFT)
	{
		if(m_nListDisplayed == ALPHA || m_nListDisplayed == TOPIC)
		{
			CWordlist temp(m_AlphaList.Search(m_strTyped));
			if (!m_strTyped.IsEmpty())
			{
				m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
				m_pWndPopup->DisplayList(temp);
			}
			else
			{
				m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
				m_pWndPopup->DisplayList(m_TopicList);
			}
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to process the virtual key code so that the correct
//				: list is displayed in the popup window.
//
// PARAMETERS	: nChar - virtual key code pressed in WM_KEYDOWN message.
////////////////////////////////////////////////////////////////////////////////

BOOL CWorAidAbbrevProccessKey::ProcessKey(UINT nVK, UINT nChar)
{
	CWordlist wordList;			// Used to store searh lists
	CAbbrevList abbrevList;

	if(NonKey(nChar))		// Do not process these keys
	{
		return FALSE;
	}

	// Not and Abbrev or WordAid hotkey then add to store
	if(!bAbbrebHotkey(nVK) && !bWordAidHotkey(nVK))
	{
		m_bExpanded		= FALSE;	// Set to FALSE as something has been typed
		m_bWordInserted = FALSE;	// FALSE as something now has been typed
		ResetScroll();				// Reset the scroll page

		if(nChar == VK_BACK)		// If Backspace remove character
		{
			if(!m_strTyped.IsEmpty())
			{
				m_strTyped = m_strTyped.Left(m_strTyped.GetLength() - 1);
			}
			else 
			{
				m_strTyped = m_strStore;
			}
		}
		else
		{
			m_strTyped += CapitaliseAfterPunc(nChar);
			// if punctuation entered need to store current string and clear
			// search string
			if (m_bLastCharPunc || IsCharPunctuation(CVKConversion::CharToVK(nChar))) //added 17th July
			{
				m_strStore = m_strTyped.Left(m_strTyped.GetLength()-1);
				m_strTyped = _T("");
				if (atoi(m_pSettings->GetSetting(AUTO_SPACE_WORD)))
				{
					CString remove = (char)_T(VK_BACK);
					CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;
					pWnd->DeleteOutput(remove);
					pWnd->DeleteOutput(remove);
					remove = nChar;
					pWnd->DeleteOutput(remove);
				}
			}//added 17th July

		}
	}
	else
	{
		m_strStore = m_strTyped;
//		m_strStore += m_strTyped;		// WA0050 - Store just keeps getting bigger and bigger

		m_strTyped = _T("");
		m_bExpanded = FALSE;	// Set to FALSE as something has been typed
		m_bWordInserted = FALSE;
		ResetScroll();
	}
	
	// If not empty search alpha list
	if(m_AlphaList.GetSize())
	{
		m_lstWordDisplay = m_AlphaList.Search(m_strTyped);
	}

	// Listed Expansion on then search abbrev list
	if(atoi(m_pSettings->GetSetting(ABBREV_LISTED_EXPANSION)) || atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
	{
//		if(m_AbbrevList)	WA0047
		{
			abbrevList = m_AbbrevList.Search(m_strTyped);
		}
	}

	// If WordBank is on and there is nothing in the list (meaning either 'listed
	// exapansion' is not on and did not find anything and nothing is typed then 
	//display all the abbrev list.
	if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) && abbrevList.GetSize() == 0 &&
		m_strTyped.IsEmpty())
	{
//		if(m_AbbrevList)		WA0047
		{
			abbrevList = m_AbbrevList.GetAll();
		}
	}

	// If there is an abbrev list and there is either a topic or alpha list then 
	// display the abbrev button
	if(abbrevList.GetSize() && (m_TopicList.GetSize() || m_AlphaList.GetSize()))
	{
		m_pWndPopup->DisplayAbbrevButton();
		m_AbbrevStoreList = abbrevList;		// Store list so that it can be displayed
	}
	// If there is no abbrev list hide abbrev button
	else if(!abbrevList.GetSize())
	{
		m_pWndPopup->DisplayAbbrevButton(FALSE);
		m_AbbrevStoreList.RemoveAll();
	}
	else	// There is only a abbrebv list so display this
	{
		m_pWndPopup->DisplayAbbrevList(abbrevList);
		m_nListDisplayed = ABBREV;
	}

	// If nothing is typed or there is no alpha list but there is a topic list then
	// display the topic list
	if((m_strTyped.IsEmpty() || !m_AlphaList.GetSize()) && 
		m_TopicList.GetSize() && !atoi(m_pSettings->GetSetting(TOPIC_LIST_HIDE)))
	{
		if (nChar == SEMI_FULLSTOP || nChar == SEMI_QUESTION || nChar == SEMI_EXCLAMATION)		//WA0006
			m_TopicList.InitialCapital();													//WA0006
		else	//reload the original topic list											//WA0006
			m_TopicList.LoadWordlist(m_pSettings->GetSetting(TOPIC_FILENAME),TOPIC);		//WA0006
		
		m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
		m_pWndPopup->DisplayList(m_TopicList);
		m_lstWordDisplay = m_TopicList;
		m_nListDisplayed = TOPIC;
	}
	// If something has been typed and there is a alpha list then display alpha list
	else if(!m_strTyped.IsEmpty() && m_lstWordDisplay.GetSize()  &&
			!atoi(m_pSettings->GetSetting(ALPHA_LIST_HIDE)))
	{
		m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
		m_pWndPopup->DisplayList(m_lstWordDisplay);
		m_nListDisplayed = ALPHA;
	}
	else if (!abbrevList.GetSize())//nothing to display so hide window
	{
		m_pWndPopup->HideWindow();
		m_pWndPopup->ResetDDEString();
	}
	else
	{
		m_lstDisplay = abbrevList;
		m_pWndPopup->DisplayAbbrevList(abbrevList);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to Display the list in the poup window
//
// PARAMETERS	: Any characters that have been typed
/////////////////////////////////////////////////////////////////////////////////

void CWorAidAbbrevProccessKey::DisplayList(CString strText /*= _T(""*/)
{
	// Turn off the abbrev button if Word Bank is not on
	if(!atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
	{
		m_pWndPopup->DisplayAbbrevButton(FALSE);
	}

	if(strText.IsEmpty() && m_TopicList.GetSize() > 0)
	{
		m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
		m_pWndPopup->DisplayList(m_TopicList);
		m_lstWordDisplay = m_TopicList;
		m_nListDisplayed = TOPIC;
	}
	else if(!strText.IsEmpty() && m_AlphaList.GetSize() > 0)
	{
		m_lstWordDisplay = m_AlphaList.Search(strText);
		m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
		m_pWndPopup->DisplayList(m_lstWordDisplay);
		m_nListDisplayed = ALPHA;
	}
	else if(m_AbbrevList.GetSize() > 0)
	{	
		CAbbrevList abbrevList;

		if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
		{
//			if(m_AbbrevList)		WA0047
			{
				abbrevList = m_AbbrevList.Search(m_strTyped);
			}
		}

		// Display whole Abbrev list if word bank is on
		if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) && abbrevList.GetSize() == 0)
		{
	//		if(m_AbbrevList)	WA0047
			{
				m_lstDisplay = m_AbbrevList.GetAll();
				m_pWndPopup->DisplayAbbrevList(m_AbbrevList);
				m_nListDisplayed = ABBREV;
			}
		}
		else
		{
			m_pWndPopup->DisplayAbbrevList(abbrevList);
			m_lstDisplay = abbrevList;
			m_nListDisplayed = ABBREV;
		}
	}

}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when the Down button is pressed to move down a list.
//				: This function moves down the list which is currently being 
//				: displayed in the popup window.
/////////////////////////////////////////////////////////////////////////////////

void CWorAidAbbrevProccessKey::OnButtonDown()
{
	if(m_nListDisplayed == ABBREV)
	{
//		if(m_AbbrevList)		WA0047
		{
			NextPage(m_AbbrevList);
		
			m_pWndPopup->DisplayAbbrevList(m_AbbrevList);
		}
	}
	else if(m_nListDisplayed == ALPHA || m_nListDisplayed == TOPIC)
	{
		NextPage(m_lstWordDisplay);
		m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
		m_pWndPopup->DisplayList(m_lstWordDisplay);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when the Up button is pressed to move down a list.
//				: This function moves Up the list which is currently being 
//				: displayed in the popup window.
/////////////////////////////////////////////////////////////////////////////////

void CWorAidAbbrevProccessKey::OnButtonUp()
{
	if(m_nListDisplayed == ABBREV)
	{
		PrevPage();

//			if(m_AbbrevList)		WA0047
		{
			m_pWndPopup->DisplayAbbrevList(m_AbbrevList);
		}
	}
	else if(m_nListDisplayed == ALPHA || m_nListDisplayed == TOPIC)
	{
		PrevPage();
		m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
		m_pWndPopup->DisplayList(m_lstWordDisplay);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when the abbreviation button is clicked to display 
//				: the abbreviation list or to go back to the alpha or topic list.
/////////////////////////////////////////////////////////////////////////////////

void CWorAidAbbrevProccessKey::OnAbbrevButtonDown()
{
	if(m_nListDisplayed != ABBREV)
	{
		ResetScroll();
		m_lstDisplay = GetStoredAbbrevList();
		//if there's no matched list and wordbank is on display whole list
		if (m_lstDisplay.GetSize() == 0 && atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
			m_lstDisplay = m_AbbrevList;

		m_pWndPopup->DisplayAbbrevList(m_lstDisplay);	
	}
	else
	{
		ResetScroll();
		if(m_strTyped.IsEmpty())
		{
			// Display Topic list
			if(!atoi(m_pSettings->GetSetting(TOPIC_LIST_HIDE)))
			{
				m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
				m_pWndPopup->DisplayList(m_TopicList);
				m_lstWordDisplay = m_TopicList;
				m_nListDisplayed = TOPIC;
			}
			else
				m_pWndPopup->HideWindow();
		}
		else
		{
			// Display alpha list
			if(!atoi(m_pSettings->GetSetting(ALPHA_LIST_HIDE)))
			{
				m_lstWordDisplay = m_AlphaList.Search(m_strTyped);
				m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
				m_pWndPopup->DisplayList(m_lstWordDisplay);
				m_nListDisplayed = ALPHA;
			}
			else if(!atoi(m_pSettings->GetSetting(TOPIC_LIST_HIDE)))
			{// Display Topic list
				m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
				m_pWndPopup->DisplayList(m_TopicList);
				m_lstWordDisplay = m_TopicList;
				m_nListDisplayed = TOPIC;
			}
			else
				m_pWndPopup->HideWindow();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to set the abbreviation list that is to be used.  It 
//				: sets the stored list to the whole abbreviation list so that if
//				: word bank is on then the list can be displayed.
//
// PARAMETERS	: list - the abbreviation list that is being used.
/////////////////////////////////////////////////////////////////////////////////

//WA0047
//void CWorAidAbbrevProccessKey::SetAbbrevList(CAbbrevList* list)
void CWorAidAbbrevProccessKey::SetAbbrevList(CAbbrevList list)
{
	m_AbbrevList = list;

//	if(m_AbbrevList)	WA0047
	{
		m_AbbrevStoreList = m_AbbrevList.GetAll();
	}
}

void CWorAidAbbrevProccessKey::ProcessAbbrevBtnHotkey(UINT nChar)
{
	m_bExpanded = FALSE;
	m_bWordInserted = FALSE;
	ResetScroll();
//	CString remove = (char)_T(VK_BACK);
//	CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;
//	pWnd->DeleteOutput(remove);
	OnAbbrevButtonDown();

}

//! Called to find out which list should be displayed.  This is mainly used when the Abbreviation list
//! is displayed so that you can find out whether when clicking on the Change button which list
//! will be displayed either the TOPIC or Alpha
/*!
	\return TOPIC or ALPHA depending on which list would be displayed this is depending on whether anything
			has been typed
*/

//WA0033
UINT CWorAidAbbrevProccessKey::GetAlphaOrTopic()
{
	if(m_strTyped.IsEmpty())
	{
		return TOPIC;
	}
	else
	{
		return ALPHA;
	}
}