//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//  WA0005 - apply capitalisation to topic lists as required                            //
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0015 - solve problem with punctuation not clearing wordlists						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
	
#include "stdafx.h"
#include "wordaidprocesskey.h"
#include "ChildWnd.h"
#include "VKConversion.h"
#include "wordaid2.h"	//added 17th July
//punctuation keys
const CString SEMI_SPACE		= _T(" ");
const CString SEMI_FULLSTOP		= _T(".");
const CString SEMI_COMMA		= _T(",");
const CString SEMI_QUESTION		= _T("?");
const CString SEMI_EXCLAMATION	= _T("!");
const CString SEMI_COLON		= _T(":");
const CString SEMI_SEMI_COLON	= _T(";");

CWordAidProcessKey::CWordAidProcessKey(void)
{
}

CWordAidProcessKey::~CWordAidProcessKey(void)
{
}

BOOL CWordAidProcessKey::KeyDown(UINT nVK)
{
	BOOL bReturn = TRUE;

	if(nVK == VK_SHIFT)
	{
		// handle shift key
		CWordlist temp(m_AlphaList.Search(m_strTyped));
		if (!m_strTyped.IsEmpty() && isupper(m_strTyped[0]))
		{
			temp.Capitalise();
			m_pWndPopup->DisplayList(temp);
		}
		else if(!m_strTyped.IsEmpty())
		{
			temp.InitialCapital();
			m_pWndPopup->DisplayList(temp);
		}
		else
		{
			// Display Topic list with intial letter capitalised
			CWordlist topic(m_TopicList);

			topic.InitialCapital();
			m_pWndPopup->DisplayList(topic);
		}
		return bReturn;
	}
	// REMOVED 11/07/02
	/*else if(SpaceAfterPunctuation(nVK))
	{
		return TRUE;
	}
	else
	{
		ProcessKey(nVK);
	}*/

	return bReturn;
}

BOOL CWordAidProcessKey::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
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

	// Convert Character to the 
	int nVK = CVKConversion::CharToVK((char)nChar);

	// Check to see if a word has been inserted and whether the next character
	// is a punctuation character.
	RemoveSpaceAfterInsert(nChar);

	if(SpaceAfterPunctuation(nVK))
	{
   		ProcessKey(nVK,nChar);		//WA0015
		return TRUE;
	}
	else
	{
		ProcessKey(nVK, nChar);
	}

	return TRUE;
}

BOOL CWordAidProcessKey::KeyUp(UINT nVK)
{
	if (nVK == VK_SHIFT)
	{
		CWordlist temp(m_AlphaList.Search(m_strTyped));
		if (!m_strTyped.IsEmpty())
		{
			m_pWndPopup->DisplayList(temp);
		}
		else
		{
			m_pWndPopup->DisplayList(m_TopicList);
		}
	}

	return TRUE;
}

BOOL CWordAidProcessKey::ProcessKey(UINT nVK, UINT nChar)
{
	CWordlist wordList;			// Used to store searh lists

	if(nVK == VK_ESCAPE)
	{
		return FALSE;
	}

	// Not and Abbrev or WordAid hotkey then add to store
	if(!bWordAidHotkey(nVK) && nVK != VK_SPACE && nVK != VK_RETURN)
	{
		m_bExpanded		= FALSE;		// Set to FALSE as something has been typed
		m_bWordInserted = FALSE;
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
			// if punctuation entered need to store current string and clear
			// search string
			if (m_bLastCharPunc || IsCharPunctuation(CVKConversion::CharToVK(nChar))) //added 17th July
			{
				m_strStore = m_strTyped.Left(m_strTyped.GetLength()-1);
				m_strTyped = _T("");
				if (atoi(m_pSettings->GetSetting(AUTO_SPACE_WORD)))
				{
					// WA0048 - Stops last character of word before punctuation from being deleted
				/*	CString remove = (char)_T(VK_BACK);
					CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;
					pWnd->DeleteOutput(remove);
					pWnd->DeleteOutput(remove);
					remove = nChar;
					pWnd->DeleteOutput(remove);*/
				}
			}//added 17th July

		}
	}
	else
	{
		m_strStore		= m_strTyped;
		m_strTyped		= _T("");
		m_bExpanded		= FALSE;	// Set to FALSE as something has been typed
		m_bWordInserted = FALSE;
		ResetScroll();
	}
	
	// If not empty search alpha list
	if(m_AlphaList.GetSize())
	{
		m_lstWordDisplay = m_AlphaList.Search(m_strTyped);
	}

	// If nothing is typed or there is no alpha list but there is a topic list then
	// display the topic list
	if((m_strTyped.IsEmpty() || !m_AlphaList.GetSize()) && m_TopicList.GetSize())
	{
		if (atoi(m_pSettings->GetSetting(AUTO_CAPS)) &&			// WA0054
			(nChar == SEMI_FULLSTOP || nChar == SEMI_QUESTION || nChar == SEMI_EXCLAMATION))	//WA0006
			m_TopicList.InitialCapital();													//WA0006
		else		//reload the original topic list										//WA0006
			m_TopicList.LoadWordlist(m_pSettings->GetSetting(TOPIC_FILENAME),TOPIC);		//WA0006

		m_pWndPopup->DisplayList(m_TopicList);	
		m_lstWordDisplay = m_TopicList;
		m_nListDisplayed = TOPIC;
	}
	// If something has been typed and there is a alpha list then display alpha list
	else
	{
		m_pWndPopup->DisplayList(m_lstWordDisplay);
		m_nListDisplayed = ALPHA;
	}

	return TRUE;

}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to Display the list in the poup window
//
// PARAMETERS	: Any characters that have been typed
/////////////////////////////////////////////////////////////////////////////////

void CWordAidProcessKey::DisplayList(CString strText /*= _T(""*/)
{
	if(strText.IsEmpty() && m_TopicList.GetSize() > 0)
	{
		m_pWndPopup->DisplayList(m_TopicList);
		m_lstWordDisplay = m_TopicList;
	}
	else if(!strText.IsEmpty() && m_AlphaList.GetSize() > 0)
	{
		m_pWndPopup->DisplayList(m_AlphaList.Search(strText));
		m_lstWordDisplay = m_AlphaList;
	}
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when the Down button is pressed to move down a list.
//				: This function moves down the list which is currently being 
//				: displayed in the popup window.
/////////////////////////////////////////////////////////////////////////////////

void CWordAidProcessKey::OnButtonDown()
{
	NextPage(m_lstWordDisplay);

	m_pWndPopup->DisplayList(m_lstWordDisplay);
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when the Up button is pressed to move down a list.
//				: This function moves Up the list which is currently being 
//				: displayed in the popup window.
/////////////////////////////////////////////////////////////////////////////////

void CWordAidProcessKey::OnButtonUp()
{
	PrevPage();

	m_pWndPopup->DisplayList(m_lstWordDisplay);
}
