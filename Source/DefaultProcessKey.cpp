//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0014 - solve problem with using numbers as abbreviations capitalising the whole	//
//			of expansion																//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0015 - solve problem with punctuation not clearing wordlists						//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0017 - add speech indication when there are no more words above or below the		//
//			current list																//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0019 - make sure abbreviations are spoken whatever form of selection is used		//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0026 - stop auto space working when popup window hidden							//	
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
	
#include "stdafx.h"
#include "defaultprocesskey.h"
#include "ChildWnd.h"

#include "VKConversion.h"

// SEMI AUTOMATIC Hotkey
const CString SEMI_SPACE		= _T(" ");
const CString SEMI_FULLSTOP		= _T(".");
const CString SEMI_COMMA		= _T(",");
const CString SEMI_QUESTION		= _T("?");
const CString SEMI_EXCLAMATION	= _T("!");
const CString SEMI_COLON		= _T(":");
const CString SEMI_SEMI_COLON	= _T(";");


const UINT TOPIC_BUT	=1;
const UINT ALPHA_BUT	=2;

// indicators for which buttons, if any, should be enabled in dlg
static BOOL g_bAlpha = TRUE;
static BOOL g_bTopic = TRUE;

// WA0039 - LPARAM for InsertDeleteWordDlgProc
struct INSERT_DELETE_LPARAM
{
	CString strWord;
	BOOL	bInsert;
};// WA0039

INT_PTR CALLBACK InsertDeleteWordDlgProc (HWND hDlg, UINT nMessage, WPARAM wParam,LPARAM lParam)
{
static  short nIndex;
//add code from CAddDelDlg into this switch statement
    switch (nMessage)
    {
	    case WM_INITDIALOG:
	    {
			CString szTitle = _T("");

			// WA0039 - Cast LPARM to Strcuture
			INSERT_DELETE_LPARAM* pInsertDelete = (INSERT_DELETE_LPARAM*)lParam;

			// WA0039
			//if (LOWORD(lParam))
			if(pInsertDelete->bInsert)
			{
				szTitle.Format(IDS_INSERT_WORD, pInsertDelete->strWord);		// WA0039
				//szTitle.LoadString(IDS_INSERT_WORD);
			}
			else
			{
				szTitle.Format(IDS_DELETE_WORD, pInsertDelete->strWord);		// WA0039
				//szTitle.LoadString(IDS_DELETE_WORD);
			}

			SetDlgItemText(hDlg,IDC_STATIC_5,szTitle);
			// disable buttons
			if (!g_bAlpha)
				EnableWindow(GetDlgItem(hDlg,IDC_CHK_ALPHA),FALSE);
			if (!g_bTopic)
				EnableWindow(GetDlgItem(hDlg,IDC_CHK_TOPIC),FALSE);

		    break;
		}

	    case WM_COMMAND:
	        switch (wParam)
	        {
		        case IDOK:

					return ( TRUE );
	            break;
		
		        case IDC_CHK_TOPIC:
		        	if ( IsWindowEnabled( GetDlgItem( hDlg, IDC_CHK_TOPIC ) ) )
		            {
			            EndDialog (hDlg, TOPIC_BUT);

					}
		            return ( TRUE );
	                break;
		        case IDC_CHK_ALPHA:
		        	if ( IsWindowEnabled( GetDlgItem( hDlg, IDC_CHK_ALPHA ) ) )
		            {
			            EndDialog (hDlg, ALPHA_BUT);

					}
		            return ( TRUE );
	                break;
		
		        case IDCANCEL:
		            EndDialog (hDlg, FALSE);

					return ( TRUE );
	                break;
		
		        default:
		            return ( TRUE );
	        }
	        break;
	
		default:
	        return (FALSE);
	}
    return (TRUE);
}

////////////////////////////////////////////////////////////////////////////////

CDefaultProcessKey::CDefaultProcessKey(CSetting* settings /*= NULL*/, 
									   CChildWnd* pPopupWnd /*= NULL*/)
{
	m_pSettings			= settings;	
	m_pWndPopup			= pPopupWnd;
	m_strStore			= _T("");	
	m_strTyped			= _T("");	
	m_bExpanded			= FALSE;	
	m_nScrolled			= 0;		
	m_strText			= _T("");	
	m_nListDisplayed	= 0;
	m_bLastCharPunc		= FALSE;
//	m_AbbrevList		= NULL;			// WA0047
	m_bWordInserted		= FALSE;
	m_bWindowHidden		= FALSE;		// WA0034
}

////////////////////////////////////////////////////////////////////////////////

CDefaultProcessKey::~CDefaultProcessKey(void)
{
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: This function should handle all of the hotkeys related to 
//				: Abbrev.
// 
// PARAMETERS	: nChar - Character to check to see if it is a hotkey.
//
// RETURNS		: TRUE if key is a hotkey.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::ProcessAbbrevHotkey(UINT nChar)
{
	BOOL bReturn = FALSE;		// Set to TRUE if key is a Hotkey.

	if(m_pSettings)
	{
		// TODO : Set Keys to Expand to
		if(m_pSettings->GetSetting(ABBREV_EXPANSION_METHOD) == MANUAL_HOTKEY && IsManualHotkey(nChar))
		{
			m_bExpanded = ExpandAbbrev();
			bReturn = m_bExpanded;
		}
		else
		{
			if(nChar == (UINT)atoi(m_pSettings->GetSetting(ABBREV_FULL_EXPAN_HOTKEY)))		// Un-expand (Full)
			{	
				UnExpandFull();
			
				bReturn = TRUE;
			}
			else if(nChar == (UINT)atoi(m_pSettings->GetSetting(ABBREV_PART_EXPAN_HOTKEY)))	// Un-expand (Partial)
			{
				UnExpandPartial();
			
				bReturn = TRUE;
			}
			else if(nChar == (UINT)atoi(m_pSettings->GetSetting(PGUP_KEY)) && m_nListDisplayed == ABBREV)
			{
				if(PrevPage())	// Only redisplay if page moved
				{
					m_pWndPopup->DisplayAbbrevList(m_AbbrevList);
				}
				else if (m_pWndPopup->m_ctrlSpeaker)					//WA0017
				{														//WA0017
					CString strNoneAbove = _T("");						//WA0017
					strNoneAbove.LoadString(IDS_NONE_ABOVE);			//WA0017	
					m_pWndPopup->m_ctrlSpeaker.Speak(strNoneAbove);		//WA0017
				}														//WA0017
			}
			else if(nChar == (UINT)atoi(m_pSettings->GetSetting(PGDWN_KEY)) && m_nListDisplayed == ABBREV)
			{
				if(NextPage(m_lstDisplay))		// Only redisplay if page moved
				{
					m_pWndPopup->DisplayAbbrevList(m_lstDisplay);
				}
				else if (m_pWndPopup->m_ctrlSpeaker)					//WA0017
				{														//WA0017
					CString strNoneBelow = _T("");						//WA0017
					strNoneBelow.LoadString(IDS_NONE_BELOW);			//WA0017	
					m_pWndPopup->m_ctrlSpeaker.Speak(strNoneBelow);		//WA0017
				}														//WA0017

			}
			else if (nChar == (UINT)atoi(m_pSettings->GetSetting(SAY_KEY)) && m_nListDisplayed == ABBREV)
			{
				m_pWndPopup->SayCurrentSelection();
				
				bReturn = TRUE;
			}
			else if(ProcessShowPopupKey(nChar))
			{
				bReturn = TRUE;
			}
			else if(ProcessInsertKey(nChar))
			{
				bReturn = TRUE;
			}
			else if (nChar == /*CVKConversion::VKToChar(*/(UINT)atoi(m_pSettings->GetSetting(ABBREV_BTN_HOTKEY))//)
//				&& m_pWndPopup->m_bAbbrevButton)		//WA0021
				&& (atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) || m_pWndPopup->m_bAbbrevButton))
			{
				ProcessAbbrevBtnHotkey(nChar);		//WA0021
				bReturn = TRUE;					//WA0021
			}
		}
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Process the semi-automatic key if pressed and the expansion
//				: method is semi-automatic.
//
// PARAMETERS	: nChar - The virtual key code pressed
//
// RETURNS		: TRUE if Semi-Automatic is the expansion method and the code is
//				: one of the hotkeys.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::ProcessAbbrevKeyDown(UINT nChar)
{
	BOOL bReturn = FALSE;		// Set to TRUE if key is a Hotkey.

	if(m_pSettings)
	{
		// WA0034
		if(!m_bWindowHidden && m_pSettings->GetSetting(ABBREV_EXPANSION_METHOD) == SEMI_AUTOMATIC && IsSemiAutomaticKey(nChar) &&
			!SpecialAbbrevCase(nChar) )
		//if(m_pSettings->GetSetting(ABBREV_EXPANSION_METHOD) == SEMI_AUTOMATIC && IsSemiAutomaticKey(nChar) &&
		//	!SpecialAbbrevCase(nChar) )
		{
			m_bExpanded = ExpandAbbrev(nChar);
			bReturn = m_bExpanded;
		}
		// WA0034
		else if(!m_bWindowHidden && m_pSettings->GetSetting(ABBREV_EXPANSION_METHOD) == AUTOMATIC_EXPANSION &&
			m_AbbrevList.IsAbbrev(m_strTyped+CVKConversion::VKToChar(nChar)) != _T(""))		// AUTO EXPANSION
		//else if(m_pSettings->GetSetting(ABBREV_EXPANSION_METHOD) == AUTOMATIC_EXPANSION &&
		//	m_AbbrevList->IsAbbrev(m_strTyped+CVKConversion::VKToChar(nChar)) != _T(""))		// AUTO EXPANSION
		{
			m_pWndPopup->SetAbbrevSelectionTimer();		// Set timer going
		}
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: This function checks to see if the abbreviations expansion
//				: is a special case and should not be expanded.  Currently the
//				: only special case is when the expansion contains a number
//				: it cannot be expanded with a semi-automatic value that requires
//				: the shift key to be down.  This is because the number becomes
//				: its upper character.
//
// PARAMETERS	: nVK - virtual key code
//
// RETURNS		: TRUE if the expansion is a special case
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::SpecialAbbrevCase(UINT nVK)
{
	BOOL bReturn = FALSE;

	

//	if(m_AbbrevList)		WA0047
	{
		// Get the expansion
		CString strExpan = m_AbbrevList.IsAbbrev(m_strTyped);

		if(!strExpan.IsEmpty())
		{
			if(HasNumber(strExpan))
			{
				CString strChar = CVKConversion::VKToChar(nVK);

				if(strChar == SEMI_EXCLAMATION || strChar == SEMI_QUESTION || 
					strChar == SEMI_COLON)
				{
					bReturn = TRUE;
				}
			}
		}
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Checks a string to see it contains a number.
//
// PARAMETERS	: str - the string to be checked
//
// RETURNS		: TRUE if the string contains a number
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::HasNumber(CString str)
{
	BOOL bReturn = FALSE;

	if(str.Find(_T("1")) > -1 || str.Find(_T("2")) > -1 || str.Find(_T("3")) > -1||
		str.Find(_T("4")) > -1 || str.Find(_T("5")) > -1 || str.Find(_T("6")) > -1 ||
		str.Find(_T("7")) > -1 || str.Find(_T("8")) > -1 || str.Find(_T("9")) > -1 ||
		str.Find(_T("0")) > -1)
	{
		bReturn = TRUE;
	}

	return bReturn;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::ProcessWordAidHotkey(UINT nChar)
{
	BOOL bReturn = FALSE;

	if(ProcessWordAidSelectionKeys(nChar) || ProcessShowPopupKey(nChar) ||
		ProcessUndoKey(nChar) || ProcessInsertKey(nChar) || ProcessSayWordKey(nChar) ||
		ProcessAddDeleteWordKey(nChar))
	{
		bReturn = TRUE;
	}
	else if(nChar == (UINT)atoi(m_pSettings->GetSetting(PGUP_KEY)) && (m_nListDisplayed == ALPHA || m_nListDisplayed == TOPIC))
	{
		bReturn = TRUE;
		if(PrevPage())	// Only redisplay if page moved
		{
			m_pWndPopup->DisplayList(m_lstWordDisplay);
		}
		else if (m_pWndPopup->m_ctrlSpeaker)					//WA0017
		{														//WA0017
			CString strNoneAbove = _T("");						//WA0017
			strNoneAbove.LoadString(IDS_NONE_ABOVE);			//WA0017	
			m_pWndPopup->m_ctrlSpeaker.Speak(strNoneAbove);		//WA0017
		}														//WA0017
	}
	else if(nChar == (UINT)atoi(m_pSettings->GetSetting(PGDWN_KEY)) && (m_nListDisplayed == ALPHA || m_nListDisplayed == TOPIC))
	{
		bReturn = TRUE;
		if(NextPage(m_lstWordDisplay))		// Only redisplay if page moved
		{
			m_pWndPopup->DisplayList(m_lstWordDisplay);
		}
		else if (m_pWndPopup->m_ctrlSpeaker)					//WA0017
		{														//WA0017
			CString strNoneBelow  = _T("");						//WA0017
			strNoneBelow.LoadString(IDS_NONE_BELOW);			//WA0017
			m_pWndPopup->m_ctrlSpeaker.Speak(strNoneBelow);		//WA0017
		}														//WA0017
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Displays or shows the popup window if the virtual key code
//				: is the ShowPopup Setting hotkey.
//
// PARAMETERS	: nVK - Virtual key to check.
//
// RETURNS		: TRUE if the key is the ShowPopup Hotkey.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::ProcessShowPopupKey(UINT nVK)
{
	BOOL bReturn = FALSE;
	// TODO : Make so that other keys can be selected
	if(atoi(m_pSettings->GetSetting(SHOW_LIST)))
	{
		if (nVK == (UINT)atoi(m_pSettings->GetSetting(SHOW_LIST_HOTKEY))) //default show window hotkey
		{
			bReturn = TRUE;
			//toggle bShow in CChildWnd class
			if (m_pWndPopup->IsVisible() == 0)
			{
				m_pWndPopup->ShowWindow(SW_HIDE);
				m_pWndPopup->SetVisible(TRUE);
				m_pWndPopup->SetMonitorsHiddenWnd();
				m_bWindowHidden = TRUE;		// WA0034
			}
			else
			{
				// WA0046 - Set to TRUE so Voice is not set
				m_pWndPopup->m_bHotkeyShow = TRUE;		
				m_pWndPopup->ShowWindow(SW_SHOWNA);
				m_pWndPopup->m_bHotkeyShow = FALSE;	// WA0046 - Set Back
				m_pWndPopup->SetVisible(FALSE);
				m_pWndPopup->SetMonitorKeys();
				m_bWindowHidden = FALSE;	// WA0034
			}
		}
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: If virtual key code is the Undo hotkey and any special keys 
//				: are pressed then undo last output and put back typed 
//				: characters.
//
// PARAMETERS	: nVK - Virtual key pressed
//
// RETURNS		: TRUE if Undo hotkey.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::ProcessUndoKey(UINT nVK)
{
	BOOL bReturn = FALSE;

	// TODO : Make it so other keys can be used
	if (nVK== (UINT)atoi(m_pSettings->GetSetting(UNDO_HOTKEY)))	// UNDO Key
	{
		bReturn = Undo();
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to undo last insert.  This removes the text entered
//				: and inserts back the text that was entered before.
//
// RETURNS		: TRUE if the UNDO was successfull.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::Undo()
{
	BOOL bReturn = FALSE;

	{
		bReturn = TRUE;

		CString backspace = (char)_T(VK_BACK);

		if(HIBYTE(GetKeyState(VK_CONTROL)))
		{
			// Simulate a key release
			keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_KEYUP, 0);
		}

		if(HIBYTE(GetKeyState(VK_MENU)))
		{
			// Simulate a key release
			keybd_event(VK_MENU, MapVirtualKey(VK_MENU,0), KEYEVENTF_KEYUP, 0);
		}

		// remove letters already output		
		for (int i =0; i<m_strText.GetLength();i++)
		{
			m_pWndPopup->SendString(backspace);
		}

 		m_pWndPopup->SendString(m_strStore);	// Add Back Text
		
		m_strTyped = m_strStore;							// Store is now typed
		m_strStore = _T("");
		m_strText = _T("");
		DisplayList(m_strTyped);
		//m_pWndPopup->DisplayList(m_AlphaList.Search(m_strTyped));		// Display Alpha List
	}

	return bReturn;
}
////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Inserts the currently selected (highlighted) word in the 
//				: popup window.
//
// PARAMETERS	: nVK - Virtual key to check
//
// RETURNS		: TRUE if Insert hotkey
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::ProcessInsertKey(UINT nVK)
{
	BOOL bReturn = FALSE;

	// TODO : Make changeable
	// handle Insert key if list window showing - output current word to document
	if (nVK == (UINT)atoi(m_pSettings->GetSetting(INS_WORD_KEY)) && !m_pWndPopup->IsVisible())	//	WA0026 changed - changed back to make HOME key work again
	{
		bReturn = TRUE;

		if(atoi(m_pSettings->GetSetting(WORDAID_ON)) &&
			atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
			!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
		{
			if(IsAbbrevDisplayed())
			{
				SelectAbbrev(m_pWndPopup->GetCurSel());
			}
			else
			{
				SelectWord(m_pWndPopup->GetCurSel());
			}
		}
		else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
		{
			SelectWord(m_pWndPopup->GetCurSel());
		}
		else if(atoi(m_pSettings->GetSetting(ABBREV_ON)))
		{
			SelectAbbrev(m_pWndPopup->GetCurSel());
		}
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: If Say Word hotkey then Speak the word currently selected
//				: (highlighted)
//
// PARPAMETERS	: Virual key to check
//
// RETURNS		: TRUE if the virtual key is the say word hotkey.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::ProcessSayWordKey(UINT nVK)
{
	BOOL bReturn = FALSE;

	// handle End to say currently highlighted word
	if (nVK == (UINT)atoi(m_pSettings->GetSetting(SAY_KEY)) && !m_pWndPopup->IsVisible())	//	WA0026 changed
	{
		if (m_pWndPopup->CanSpeak())
		{
			bReturn = TRUE;

			CString szNoMatch = _T("");
			szNoMatch.LoadString(IDS_NO_MATCH);

			if (m_pWndPopup->GetCurrentSelectText() != szNoMatch)
			{
				m_pWndPopup->SayText(m_pWndPopup->GetCurrentSelectText());
			}
			else
			{
				szNoMatch.Remove('[');
				szNoMatch.Remove(']');
				m_pWndPopup->SayText(szNoMatch);
			}
		}
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to check to see if the hotkey pressed is the hotkey
//				: Insert a word into the wordlist.
//
// PARAMETERS	: nVK - Virtual key code to check
//
// RETURNS		: TRUE if the code was the hotkey.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::ProcessAddDeleteWordKey(UINT nVK)
{
	BOOL bReturn = FALSE;

	if (nVK == (UINT)atoi(m_pSettings->GetSetting(ADD_WORD)))
	{
		bReturn = TRUE;

		m_pWndPopup->DisableKeyhook();

		if (CheckAdd())
		{
			// WA0039 - Create LPARAM structure
			INSERT_DELETE_LPARAM lParam;

			lParam.bInsert = TRUE;
			lParam.strWord = (m_strTyped.IsEmpty()) ? m_strStore : m_strTyped;

			int nRes = DialogBoxParam(AfxGetInstanceHandle(),
						MAKEINTRESOURCE(IDD_ADD_DELETE_DLG),
						GetForegroundWindow(),
						(DLGPROC)InsertDeleteWordDlgProc,
						//MAKELPARAM(TRUE,0));
						(LONG)&lParam);				//WA0039

			if ( nRes == TOPIC_BUT)		
				InsertTopic();

			if (nRes == ALPHA_BUT)
				InsertAlpha();
		}

		m_pWndPopup->EnableKeyhook();
	}
	else if (nVK == (UINT)atoi(m_pSettings->GetSetting(DEL_WORD_KEY)))
	{
		bReturn = TRUE;
		m_pWndPopup->DisableKeyhook();	
				
		if (CheckDelete())
		{
			// WA0039 - Create LPARAM structure
			INSERT_DELETE_LPARAM lParam;

			lParam.bInsert = FALSE;
			lParam.strWord = (m_strTyped.IsEmpty()) ? m_strStore : m_strTyped;

			int nRes = DialogBoxParam(AfxGetInstanceHandle(),
						MAKEINTRESOURCE(IDD_ADD_DELETE_DLG),
						GetForegroundWindow(),
						(DLGPROC)InsertDeleteWordDlgProc,
						//MAKELPARAM(FALSE,0));
						(LONG)&lParam);				// WA0039

			if (nRes == TOPIC_BUT)
			{
				DeleteTopic();
			}
			
			if (nRes == ALPHA_BUT)
			{
				DeleteAlpha();
			}
		}

		m_pWndPopup->EnableKeyhook();
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Check to see if the word does not already exist in the list.
//
// RETURNS		: TRUE if the the word does not exist; otherwise FALSE.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::CheckAdd()
{
	BOOL bRetVal = TRUE;

	if (m_TopicList.SearchWord(m_strTyped) != -1)
		g_bTopic = FALSE;
	else
		g_bTopic = TRUE;

	if (m_AlphaList.SearchWord(m_strTyped) != -1)
		g_bAlpha = FALSE;
	else
		g_bAlpha = TRUE;

	if (g_bAlpha == FALSE && g_bTopic == FALSE)
		bRetVal = FALSE;

	return bRetVal;
}

BOOL CDefaultProcessKey::CheckDelete()
{
	BOOL bRetVal = TRUE;

	if (m_TopicList.SearchWord(m_strTyped) == -1)
		g_bTopic = FALSE;
	else
		g_bTopic = TRUE;

	if (m_AlphaList.SearchWord(m_strTyped) == -1)
		g_bAlpha = FALSE;
	else
		g_bAlpha = TRUE;

	if (g_bAlpha == FALSE && g_bTopic == FALSE)
		bRetVal = FALSE;

	return bRetVal;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Inserts the word into the alphabetic list.
//
// RETURNS		: TRUE if inserted.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::InsertAlpha()
{
	if (m_strTyped.IsEmpty())
	{
		m_AlphaList.InsertAtEnd(m_strStore);
	}
	else
	{
		m_AlphaList.InsertAtEnd(m_strTyped);
	}

	// WA0037 - Save the wordlist after change
	m_AlphaList.SaveList(m_pSettings->GetSetting(ALPHA_FILENAME));		
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Inserts the word into the topic list.
// 
// RETURNS		: TRUE if inserted.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::InsertTopic()
{
	if (m_strTyped.IsEmpty())
	{
		m_TopicList.InsertAtEnd(m_strStore);
	}
	else
	{
		m_TopicList.InsertAtEnd(m_strTyped);
	}

	// WA0037 - Save the wordlist after change
	m_TopicList.SaveList(m_pSettings->GetSetting(TOPIC_FILENAME));

	DisplayList();

	TRACE("inserting topic word\n");
	return TRUE;
}

BOOL CDefaultProcessKey::DeleteAlpha()
{
	if (m_strTyped.IsEmpty())
	{
		m_AlphaList.Delete(m_strStore);
	}
	else
	{
		m_AlphaList.Delete(m_strTyped);
	}

	DisplayList();

	return TRUE;
}

BOOL CDefaultProcessKey::DeleteTopic()
{
	if (m_strTyped.IsEmpty())
	{
		m_TopicList.Delete(m_strStore);
	}
	else
	{
		m_TopicList.Delete(m_strTyped);
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Checks to see if the Virtual Key is a Semi - Automatic Hotkey
//				
// PARAMETERS	: nWk - Virtual key
//
// RETURNS		: TRUE if the virtual key is a SPACE, FULLSTOP, COMMA.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::bAbbrebHotkey(UINT nChar)
{
	BOOL bReturn = FALSE;

	if(IsSemiAutomaticKey(nChar))//|| IsManualHotkey(nChar))
	{
		bReturn = TRUE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::bWordAidHotkey(UINT nChar)
{
	BOOL bReturn = FALSE;

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Checks to see if the keys pressed is a selection key.  The
//				: selection key can either be the Function keys + a special key,
//				: number keys (1 - 0) + a special key or the number pad keys +
//				: a special key.
//
// PARAMETERS	: nVK - Virtual key presssed
//
// RETURNS		: TRUE if the virtual key code is a selection key and if a
//				: special key(s) is required then these are pressed as well.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::ProcessWordAidSelectionKeys( UINT nVK)
{
	BOOL bReturn = FALSE;
	CString strNumKeys = _T("");
	CString strFuncKeys = _T("");
	CString strNumPadKeys = _T("");

	strNumKeys.LoadString(IDS_NUM_KEYS);
	strFuncKeys.LoadString(IDS_FUNC_KEYS);
	strNumPadKeys.LoadString(IDS_NUMPAD_KEYS);

	if(m_pSettings->GetSetting(SELECT_WORD_KEYS) == strFuncKeys)
	{
		bReturn = SelectFunctionKeys(nVK);
	}
	else if(m_pSettings->GetSetting(SELECT_WORD_KEYS) == strNumKeys)
	{
		bReturn = SelectNumberKeys(nVK);
	}
	else if(m_pSettings->GetSetting(SELECT_WORD_KEYS) == strNumPadKeys)
	{
		bReturn = SelectNumPadKeys(nVK);
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: If the Virtual key code is a function key then the word is
//				: selected and inserted into the window with keyboard focus.
//
// PARAMETERS	: nVK - Virtual key code to check to see if it is a Function key
//
// RETURNS		: TRUE if the virtual key is a Function key.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::SelectFunctionKeys(UINT nVK)
{
	BOOL bReturn = TRUE;

	switch(nVK)
	{
	case VK_F1:
		SelectWord(0);
		break;
	case VK_F2:
		SelectWord(1);
		break;
	case VK_F3:
		SelectWord(2);
		break;
	case VK_F4:
		SelectWord(3);
		break;
	case VK_F5:
		SelectWord(4);
		break;
	case VK_F6:
		SelectWord(5);
		break;
	case VK_F7:
		SelectWord(6);
		break;
	case VK_F8:
		SelectWord(7);
		break;
	case VK_F9:
		SelectWord(8);
		break;
	case VK_F10:
		SelectWord(9);
		break;
	default:
		bReturn = FALSE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: If the Virtual key code is a number key then the word is
//				: selected and inserted into the window with keyboard focus.
//
// PARAMETERS	: nVK - Virtual key code to check to see if it is a number key
//
// RETURNS		: TRUE if the virtual key is a number key.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::SelectNumberKeys(UINT nVK)
{
	BOOL bReturn = TRUE;

	switch(nVK)
	{
	case 48:
		SelectWord(9);
		break;
	case 49:
		SelectWord(0);
		break;
	case 50:
		SelectWord(1);
		break;
	case 51:
		SelectWord(2);
		break;
	case 52:
		SelectWord(3);
		break;
	case 53:
		SelectWord(4);
		break;
	case 54:
		SelectWord(5);
		break;
	case 55:
		SelectWord(6);
		break;
	case 56:
		SelectWord(7);
		break;
	case 57:
		SelectWord(8);
		break;
	default:
		bReturn = FALSE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: If the Virtual key code is a number pad key then the word is
//				: selected and inserted into the window with keyboard focus.
//
// PARAMETERS	: nVK - Virtual key code to check to see if it is a number pad
//				: key
//
// RETURNS		: TRUE if the virtual key is a number pad key.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::SelectNumPadKeys(UINT nVK)
{
	BOOL bReturn = TRUE;

	switch(nVK)
	{
	case VK_NUMPAD0:
		SelectWord(0);
		break;
	case VK_NUMPAD1:
		SelectWord(1);
		break;
	case VK_NUMPAD2:
		SelectWord(2);
		break;
	case VK_NUMPAD3:
		SelectWord(3);
		break;
	case VK_NUMPAD4:
		SelectWord(4);
		break;
	case VK_NUMPAD5:
		SelectWord(5);
		break;
	case VK_NUMPAD6:
		SelectWord(6);
		break;
	case VK_NUMPAD7:
		SelectWord(7);
		break;
	case VK_NUMPAD8:
		SelectWord(8);
		break;
	case VK_NUMPAD9:
		SelectWord(9);
		break;
	default:
		bReturn = FALSE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Selects a word at a particualr index in the topic or alpha
//				: list.  The topic list is select from if the string typed is 
//				: empty and there is a topic list and the alpha list is selected
//				: from if the string typed is not empty and there is an alpha
//				: list.
//
// PARAMETERS	: nIndex - Index to select from.
//				: bDDE - TRUE if being sent from a DDE action
////////////////////////////////////////////////////////////////////////////////

void CDefaultProcessKey::SelectWord(int nIndex, BOOL bDDE /*= FALSE*/)
{
	if(bDDE && this->m_lstWordDisplay.GetSize() > nIndex)
	{
		m_strText = m_lstWordDisplay.GetAt(nIndex);
	}
	else if(m_pWndPopup->GetSize() > nIndex)
	{
		m_strText = m_pWndPopup->GetCurrentSelectText(nIndex);	// Store the word inserted
	}
	else
	{
		return;
	}

	CString szNoMatch = _T("");
	szNoMatch.LoadString(IDS_NO_MATCH);

/*	// Rremove brackets
	szNoMatch.Remove('[');
	szNoMatch.Remove(']');
*/
	if(szNoMatch != m_strText)
	{
		if(HIBYTE(GetKeyState(VK_CONTROL)))
		{
			// Simulate a key release
			keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_KEYUP, 0);
		}

		if(HIBYTE(GetKeyState(VK_MENU)))
		{
			// Simulate a key release
			keybd_event(VK_MENU, MapVirtualKey(VK_MENU,0), KEYEVENTF_KEYUP, 0);
		}

		m_pWndPopup->OutputString(m_strText, m_strTyped.GetLength(), bDDE);	

		m_bWordInserted = TRUE;		// Word has just been inserted

		// If Autospace is on then add space 
		if (atoi((const char*)(m_pSettings->GetSetting(AUTO_SPACE_WORD))))
		{
			m_strText += _T(" ");
		}

		m_strStore = m_strTyped;		// Store typed letters
		m_strTyped = _T("");			// Clear typed characters
		m_bLastCharPunc = FALSE;

		m_pWndPopup->DisplayAbbrevButton(FALSE);	// Hide Abbrev button

		ResetScroll();

		DisplayList(m_strTyped);
	}
	else
	{
		m_strText = _T("");
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Checks a string to see if all its characters are capitals.
//
// PARAMETERS	: strText - Text to check.
//
// RETURNS		: TRUE if all Capitals
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::IsCapital(CString strText)
{
	BOOL bReturn = TRUE;
	int count	= 0;			//WA0014
	CString strTemp = strText;

	if(strText.GetLength() == 0)	
	{
		bReturn = FALSE;	// Empty string return FALSE.
	}

	strTemp.MakeUpper();

	for(int i = 0; i < strText.GetLength() && bReturn; i++)
	{
		//WA0014 if it's a number keep count but ignore it
		if (IsNumber(strText.GetAt(i)))	//WA0014
			count ++;					//WA0014

		else if(strText.GetAt(i) != strTemp.GetAt(i))
		{
			bReturn = FALSE;
		}
	}
	//WA0014 Is it a numeric string
	if (count <= strText.GetLength())	//WA0014
		bReturn = FALSE;				//WA0014

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Expands the Abbreviation if the typed text matches an 
//				: abbreviation in the list.  If a character is set as the nChar
//				: parameter then this is added to the end of the expansion.
//
// PARAMETERS	: nChar - Character to tag to end of expansion. Optional if not
//				: set then nothing is tagged.
//
// RETURNS		: TRUE if the Expansion is inserted into window with keyboard
//				: focus.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::ExpandAbbrev(UINT nChar /*= 0*/)
{
	BOOL bReturn = TRUE;

//	if(m_AbbrevList)		WA0047
	{
		CString strExpansion = m_AbbrevList.IsAbbrev(m_strTyped);

		// Is there an expansion 
		if(strExpansion != _T(""))
		{
			if(IsCapital(m_strTyped))
			{
				strExpansion.MakeUpper();
			}
			else if(IsCapital(m_strTyped.GetAt(0)))	// First character Capital
			{
				// Capitalise first character of expansion
				CString strFirst = strExpansion.Left(1);

				strFirst.MakeUpper();

				strExpansion = strFirst + strExpansion.Right(strExpansion.GetLength() - 1);
			}
			ASSERT(m_pWndPopup);		// Make sure that there is one

/*			if(HIBYTE(GetKeyState(VK_CONTROL)))
			{
				// Simulate a key release
				keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_KEYUP, 0);
			}

			if(HIBYTE(GetKeyState(VK_SHIFT)))
			{
			//	keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT,0), KEYEVENTF_KEYUP, 0);
			}

			if(HIBYTE(GetKeyState(VK_MENU)))
			{
				// Simulate a key release
				keybd_event(VK_MENU, MapVirtualKey(VK_MENU,0), KEYEVENTF_KEYUP, 0);
			}*/
			// Remove Abbreviation
			// Semi automatic must remove the typed character and then add it to the end
			if(m_pSettings->GetSetting(ABBREV_EXPANSION_METHOD) == SEMI_AUTOMATIC)
			{
				for(int i = 0; i <= m_strTyped.GetLength(); i++)
				{		
					m_pWndPopup->SendString((char)VK_BACK);
				}
			}
			else
			{
				for(int i = 0; i < m_strTyped.GetLength(); i++)
				{		
					m_pWndPopup->SendString((char)VK_BACK);
				}
			}

			strExpansion.TrimLeft();	// Remove All Spaces
			strExpansion.TrimRight();

			// Tag character if sent
			if(nChar != 0)
			{
				strExpansion += CVKConversion::VKToChar(nChar);
			}

			// If auto-sapce after insertion on and the expansion method is not a space
			if(atoi(m_pSettings->GetSetting(AUTO_SPACE_WORD)) && nChar != SEMI_SPACE)
			{
				strExpansion += SEMI_SPACE;		// Add Space
			}

			CAbbrevList::FormatMultiLineOutput(strExpansion);	// Format the string

			if(HIBYTE(GetKeyState(VK_SHIFT)))
			{
				SendShiftString(strExpansion);
			}
			else
			{
				m_pWndPopup->SendString(strExpansion);				// Add Expansion
			}

			// Set speak on insert timer if required
			if ( atoi((const char*)m_pSettings->GetSetting(SPEAK_SELECTED)) )
			{
				int val = atoi((const char*)m_pSettings->GetSetting(DWELL_TIME_SELECTED));

				if (val > 0)
				{
					m_pWndPopup->SetTimer(TIMER_OUTPUT,val*100,NULL);
				}
				else
					m_pWndPopup->OnTimer(TIMER_OUTPUT);
			}

			m_strText = strExpansion;		// Store the Expansion for Un-Expand

			// Store typed characters
			m_strStore = m_strTyped;
			m_strTyped = _T("");
			DisplayList();

		//	return bReturn;		// Exit form method so that another space is not inserted if turned on
		}
		else
		{
			// This has been removed as this should be cleared in the Process key
	//		m_strTyped = _T("");
			m_bExpanded = FALSE;
			bReturn = FALSE;
		}

		// If the expansion method is semi automatic and the auto space after 
		// punctuation is turned on then add a space after expansion.
		if(m_pSettings->GetSetting(ABBREV_EXPANSION_METHOD) == SEMI_AUTOMATIC)
		{
			SpaceAfterPunctuation(nChar);
		}
	}
 	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: When the shift key is down then to get the correct format
/////////////////////////////////////////////////////////////////////////////////

void CDefaultProcessKey::SendShiftString(CString strText)
{
	if(IsCapital(strText))	// All Capitals
	{
		m_pWndPopup->SendString(strText);
	}
	else if(IsCapital(strText.Left(1))) // First letter capital only
	{
		m_pWndPopup->SendString(strText.Left(1));

		SimulateKeyPress(VK_CAPITAL);

		m_pWndPopup->SendString(strText.Right(strText.GetLength() - 1));

		SimulateKeyPress(VK_CAPITAL);
	}
	else	// All lower case
	{
		SimulateKeyPress(VK_CAPITAL);

		m_pWndPopup->SendString(strText);

		SimulateKeyPress(VK_CAPITAL);
	}

	// TODO : REMOVE
/*	if(atoi(m_pSettings->GetSetting(AUTO_SPACE)) ||
		atoi(m_pSettings->GetSetting(AUTO_SPACE_WORD)))
	{
		m_pWndPopup->SendString(_T(" "));
	}*/
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Simulates the pressing and releasing of a key.
//
// PARAMETERS	: nVK - Virtual key code to press
/////////////////////////////////////////////////////////////////////////////////

void CDefaultProcessKey::SimulateKeyPress(UINT nVK)
{
	// Simulate a key press
	keybd_event(nVK,
				0x45,
				KEYEVENTF_EXTENDEDKEY | 0,
				0 );

	// Simulate a key release
	keybd_event(nVK,
				0x45,
				KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,			
				0);
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::IsManualHotkey(UINT nVK)
{
	BOOL bReturn = FALSE;

	if(nVK == (UINT)atoi(m_pSettings->GetSetting(ABBREV_MANUAL_HOTKEY)))
	{
		bReturn = TRUE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Checks to see if the Virtual Key is a Semi - Automatic Hotkey
//				
// PARAMETERS	: nWk - Virtual key
//
// RETURNS		: TRUE if the virtual key is a SPACE, FULLSTOP, COMMA.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::IsSemiAutomaticKey(UINT nVK)
{
	BOOL bReturn = FALSE;

	CString strChar = CVKConversion::VKToChar(nVK);

 	if(strChar == SEMI_SPACE || strChar == SEMI_FULLSTOP || strChar == SEMI_COMMA ||
		strChar == SEMI_QUESTION || strChar == SEMI_EXCLAMATION || 
		strChar == SEMI_COLON || strChar == SEMI_SEMI_COLON || nVK == VK_RETURN ||
		nVK == VK_TAB)
	{
		if(IsCharPunctuation(nVK) && strChar != SEMI_COMMA)		// Punctuation character
		{
			m_bLastCharPunc = TRUE;
		}

		bReturn = TRUE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to remove the expanded abbreviation for the window
//				: with the keyboard focus and then add back the full 
//				: abbreviaiton.
//
// RETURNS		: TRUE if the Expansion was removed.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::UnExpandFull()
{
	if(m_bExpanded)
	{
		if(HIBYTE(GetKeyState(VK_CONTROL)))
		{
			keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_KEYUP, 0);
		}

		if(HIBYTE(GetKeyState(VK_SHIFT)))
		{
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT,0), KEYEVENTF_KEYUP, 0);
		}

		if(HIBYTE(GetKeyState(VK_MENU)))
		{
			keybd_event(VK_MENU, MapVirtualKey(VK_MENU,0), KEYEVENTF_KEYUP, 0);
		}

		// Remove Expanded Word
		for(int i = 0; i < m_strText.GetLength(); i++)
		{
			m_pWndPopup->SendString((char)VK_BACK);
		}

		// Add Back Abbreviation
		m_pWndPopup->SendString(m_strStore);

		m_strTyped = m_strStore;		// Set back abbreviation typed
		m_strStore = _T("");			// Clear Store
		m_strText = _T("");				// Clear expanded text

		m_bExpanded = FALSE;

		DisplayList(m_strTyped);	// Display the abbreviaiton list

		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to remove the expanded abbreviation for the window
//				: with the keyboard focus and then add back the abbreviaiton
//				: less its last character.
//
// RETURNS		: TRUE if the Expansion was removed.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::UnExpandPartial()
{
	if(m_bExpanded)
	{
		if(HIBYTE(GetKeyState(VK_CONTROL)))
		{
			keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_KEYUP, 0);
		}

		if(HIBYTE(GetKeyState(VK_SHIFT)))
		{
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT,0), KEYEVENTF_KEYUP, 0);
		}

		if(HIBYTE(GetKeyState(VK_MENU)))
		{
			keybd_event(VK_MENU, MapVirtualKey(VK_MENU,0), KEYEVENTF_KEYUP, 0);
		}

		// Remove Expanded Word
		for(int i = 0; i < m_strText.GetLength(); i++)
		{
			m_pWndPopup->SendString((char)VK_BACK);
		}

		// Add Back Abbreviation less last character
		m_pWndPopup->SendString(m_strStore.Left(m_strStore.GetLength() -1));
		
		m_strTyped = m_strStore.Left(m_strStore.GetLength() - 1);	// Set back abbreviation typed
		m_strStore = _T("");						// Clear Store
		m_strText = _T("");

		m_bExpanded = FALSE;

		DisplayList(m_strTyped);	// Display the abbreviaiton list

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Move to the next page in the list.  This function is used
//				: for the popup window to diplay the next items in the list if
//				: there are too many to display at one time.  It checks to 
//				: make sure that there is something to display in the next
//				: page.
//
// PARAMETERS	: list - Abbreviation list to move to next page to
//
// RETURNS		: TRUE if the page was moved.
////////////////////////////////////////////////////////////////////////////////

//WA0047
//BOOL CDefaultProcessKey::NextPage(CAbbrevList* list)
BOOL CDefaultProcessKey::NextPage(CAbbrevList list)
{										
	if((m_nScrolled+1)*m_pWndPopup->GetMaxItemsDisplayed() < list.GetSize())
	{
		m_nScrolled++;
		return TRUE;
	}
	else
		return FALSE;
}	

///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Move to the next page in the list.  This function is used
//				: for the popup window to diplay the next items in the list if
//				: there are too many to display at one time.  It checks to 
//				: make sure that there is something to display in the next
//				: page.
//
// PARAMETERS	: list - Abbreviation list to move to next page to
//
// RETURNS		: TRUE if the page was moved.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::NextPage(CWordlist list)
{										
	if((m_nScrolled+1)*m_pWndPopup->GetMaxItemsDisplayed() < list.GetSize())
	{
		m_nScrolled++;
		return TRUE;
	}
	else
		return FALSE;
}	

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Move to the previous page in the list.  This function is used
//				: for the popup window to diplay the previous items in the list if
//				: there are too many to display at one time.  It checks to 
//				: make sure that there is something to display in the previous
//				: page.
//
// RETURNS		: TRUE if the page was moved.
/////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::PrevPage() 
{
	if(m_nScrolled != 0) 
	{
		m_nScrolled--;
		return TRUE;
	}
	else
		return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Checks to make sure that a key is not a key that should not 
//				: be processed.
//
// PARAMETERS	: nVK - Virtual key code to check.
//
// RETURNS		: TRUE if the key is not a key that should be processed.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::NonKey(UINT nVK)
{
	BOOL bReturn = FALSE;

	if(nVK == VK_SHIFT || nVK == VK_CONTROL || nVK == VK_MENU)
	{
		bReturn = TRUE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: This function formats an abbreviation list depending upon
//				: a string that repesents the abbreviation.  The rules for the
//				: formating are if the string that represents the abbreviation 
//				: is all in capitals and the abbrevaition matches the text then
//				: the whole expansion is made upper case.  If the first character
//				: of the text is uppercase then the first letter of the first
//				: word is captialised.
//
// PARAMETERS	: list - Abbreviation list to format
//				: strTyped - string to format from.
////////////////////////////////////////////////////////////////////////////////

//WA0047
//void CDefaultProcessKey::FormatExpansion(CAbbrevList* list,
void CDefaultProcessKey::FormatExpansion(CAbbrevList list,
												 CString strTyped)
{
	CString strExpan = _T("");		// Stores the expansion from the list
	CString strAbbrev = _T("");		// Stores the abbreviation for the expansion 

	if(strTyped.IsEmpty())	// Empty do nothing
	{
		return;
	}

	for(int i = 0; i < list.GetSize(); i++)
	{
		strAbbrev = list.GetAbbreviationAt(i);
		strExpan = list.GetExpansionAt(i);
		
		strAbbrev.TrimLeft();
		strAbbrev.TrimRight();

		if(strAbbrev.CompareNoCase(strTyped) == 0 && IsCapital(strTyped))
		{
			strExpan.MakeUpper();		// Make upper case

			list.SetExpansionAt(strExpan, i);	// Replace expansion
		}
		else if(IsCapital(strTyped.Left(1)) && strExpan.Left(1).CompareNoCase(strTyped.Left(1)) == 0)
		{
			// Capitalise first character of expansion
			CString strFirst = strExpan.Left(1);

			strFirst.MakeUpper();

			strExpan = strFirst + strExpan.Right(strExpan.GetLength() - 1);

			list.SetExpansionAt(strExpan, i);		// Replace the expansion in the list
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: This method checks to see if the Auto-space after punctuation
//				: is turned on.  If it is then a space is inserted if the key
//				: is a puncuation mark.
//
// PARAETERS	: nVK - Virtual key code to check to see if it is a puncuation
//
// RETURNS		: TRUE if Space inserted.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::SpaceAfterPunctuation(UINT nVK)
{
	BOOL bReturn = FALSE;
	
 	if(IsCharPunctuation(nVK))
	{
		if(atoi(m_pSettings->GetSetting(AUTO_SPACE)) && !m_pWndPopup->IsVisible())	//	WA0026 changed
		{
			bReturn = TRUE;

			// Insert space
			m_pWndPopup->SendString(SEMI_SPACE);

			if(IsCharPunctuation(nVK) && CVKConversion::VKToChar(nVK) != SEMI_COMMA)	// Punctuation character
			{
				// Toggle last char puntuation flag
				m_bLastCharPunc = TRUE;
			}
		}
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when the WM_KEYDOWN message is sent to the process 
//				: key class.  The function handles any general functionality
//				: that has to be carried out for both WordAid and Abbrev and
//				: passes the code onto the virtual function keydown for the class
//				: that is to process the key.
//
// RETURNS		: TRUE if the key was processed.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL bReturn = FALSE;

  	if(KeyDown(nChar))
	{
		bReturn = TRUE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCEIPTION	: This function should be called to see if the character typed
//				: should be captialised.
//
// PARAMETERS	: nChar - ASCII Character code
//
// RETURNS		: the character captialised if automatic capitalise after 
//				: punctuation is on and the last character was a punctuation
//				: character.
////////////////////////////////////////////////////////////////////////////////

CString CDefaultProcessKey::CapitaliseAfterPunc(UINT nChar)
{
	CString strReturn = nChar;
	int nVK = CVKConversion::CharToVK(nChar);

	// FOR Win 9x.  When the character is recirved in OnChar the Shift key is 
	// not taken into account (it does on Unicode based systems.)  When the key
	// gets to the OnChar as far as the application is concerened the Shift key
	// is not down.  We have stored the state of the shift key in the KeyDown and
	// KeyUp method in the ChildWnd.  If the shift key is down the character
	// should be switched to its upper case value.
	if(m_pWndPopup->IsSHIFTDown())
	{
		CVKConversion::SwitchCharacter(strReturn);
	}

	// Process sticky Keys
	if(m_pWndPopup->IsStickyKeysOn())
	{
		if(m_pWndPopup->IsStickySHIFT())
		{
			CVKConversion::SwitchCharacter(strReturn);
		}
	}
//WA0015	if(m_bLastCharPunc && atoi(m_pSettings->GetSetting(AUTO_CAPS))
	if(m_bLastCharPunc && atoi(m_pSettings->GetSetting(AUTO_CAPS)) && !IsCharPunctuation(nVK)
		&& !m_pWndPopup->IsVisible())	//WA001
	{
		CString str = (char)VK_BACK;

		m_pWndPopup->SendString(str);
		strReturn.MakeUpper();
		m_pWndPopup->SendString(strReturn);

		m_bLastCharPunc = FALSE;
	}
	else if(IsCharPunctuation(nVK) && strReturn != SEMI_COMMA)	// Punctuation character
	{
		m_bLastCharPunc = TRUE;
	}
	else							// Not a punctuation character
	{
		m_bLastCharPunc = FALSE;
	}

	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Checks to see if the virtual key code is a punctuiation 
//				: character.
//
// PARAMETERS	: nVK - Virtual key code to check
//
// RETURNS		: TRUE if the character is a punctuation character
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::IsCharPunctuation(UINT nVK)
{
	BOOL bReturn = FALSE;
	CString strChar = CVKConversion::VKToChar(nVK);		// Get the character

	if(strChar == SEMI_FULLSTOP || strChar == SEMI_COMMA || strChar == SEMI_QUESTION ||
		strChar == SEMI_EXCLAMATION || strChar == SEMI_COLON || strChar == SEMI_SEMI_COLON)
	{
		bReturn = TRUE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Formats an abbreviation and expansion so that it is spoken
//				: correctly by the speech engine.  The abbreviation has spaces 
//				: added between characters and the <CR> are removed and a space
//				: replaces this.
//
// PARAMETERS	: strSay - the string that is to be formated to be spoken;
////////////////////////////////////////////////////////////////////////////////

void CDefaultProcessKey::AbbrevSayFormat(CString& strSay)
{
	CString strTemp = _T("");

	// Get the abbreviation
	int nPos = strSay.Find(SEPERATOR);

//	ASSERT((nPos != -1));	// Make sure the string is a abbreviaiton and expansion pair
	if (nPos == -1)
		return;

	CString strAbbrev = strSay.Left(nPos);
	strAbbrev.TrimLeft();

	// Add a space between each character in the abbreviation
	for(int i = 0; i < strAbbrev.GetLength(); i++)
	{
		strTemp += strAbbrev.GetAt(i);
		strTemp += _T(" ");
	}

	// Remove the abbreviation from the string
	strSay = strSay.Right(strSay.GetLength() - SEPERATOR.GetLength());
	strSay.TrimLeft();

	// Addd the abbreviation with spaces back to the expansion
	strSay = strTemp + strSay;

	// Remove <CR> from string
	strSay.Replace(XML_CARRIAGE_RETURN, _T(" "));
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to insert an abbreviations expansion into the winodw
//				: with keyboard foucs.  The abbreviation to be expanded is got
//				: from the index that is sent to the method.
//
// PARAMETERS	: nIndex - index of abbreviation to expand in the displayed 
//				: list.
//
// RETURNS		: TRUE if the abbreviation was expanded; Otherwise false if
//				: the index was invalid or the abbreviation could not be
//				: expanded.
////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::SelectAbbrev(int nIndex)
{
	// Make sure that the index is valid
	ASSERT(m_lstDisplay.GetSize() >= nIndex && nIndex >= 0);

	// make sure list isn't scrolled
	if (GetNoScrolled() > 0)
		nIndex += m_pWndPopup->m_nMaxNumberOfItems;

	// Get the abbreviation and expansion pair
	CString strText = m_lstDisplay.GetAt(nIndex);

	// Find the abbreviation and expansion seperator
	int nPos = strText.Find(SEPERATOR);

	// Get the expansion that is going to be inserted
//	strText = m_AbbrevList->IsAbbrev(strText.Left(nPos));
	strText = strText.Right(strText.GetLength() - (nPos+2));

	// Check to see if whole abbreviation is typed.  This is so that 
	// if all the abbreviations characters have been typed the expansion
	// can be formatted correctly.
	CString strExpansion = m_AbbrevList.IsAbbrev(m_strTyped);

	// Any characters typed then format expansion
	if(!m_strTyped.IsEmpty())
	{
		// Make all upper if the typed string is all in capitals and
		// the whole abbreviation has been typed.  If only the first
		// character is typed in capitals and the Abbreviation has not
		// been fully typed then only capitalise the first letter of
		// the expansion.
		if(IsCapital(m_strTyped) && strText == strExpansion)
		{
			strText.MakeUpper();
		}
		else if(IsCapital(m_strTyped.GetAt(0)))	// First character Capital
		{
			// Capitalise first character of expansion
			CString strFirst = strText.Left(1);

			strFirst.MakeUpper();

			strText = strFirst + strText.Right(strText.GetLength() - 1);
		}
	}

	ASSERT(m_pWndPopup);		// Make sure that there is one

	if(HIBYTE(GetKeyState(VK_CONTROL)))
	{
		// Simulate a key release
		keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_KEYUP, 0);
	}

	if(HIBYTE(GetKeyState(VK_SHIFT)))
	{
	//	keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT,0), KEYEVENTF_KEYUP, 0);
	}

	if(HIBYTE(GetKeyState(VK_MENU)))
	{
		// Simulate a key release
		keybd_event(VK_MENU, MapVirtualKey(VK_MENU,0), KEYEVENTF_KEYUP, 0);
	}

	// Remove any typed characters
	if(!m_strTyped.IsEmpty())
	{
		for(int i = 0; i < m_strTyped.GetLength(); i++)
		{		
			m_pWndPopup->SendString((char)VK_BACK); 	}
	}

	strText.TrimLeft();	// Remove All Spaces
	strText.TrimRight();

	// If auto-sapce after insertion on
	if(atoi(m_pSettings->GetSetting(AUTO_SPACE_WORD)))
	{
		strText += SEMI_SPACE;		// Add Space
	}

	CAbbrevList::FormatMultiLineOutput(strText);	// Format the string

	//WA0019 Set speak on insert timer if required
	if ( atoi((const char*)m_pSettings->GetSetting(SPEAK_SELECTED)) )				//WA0019
	{
		int val = atoi((const char*)m_pSettings->GetSetting(DWELL_TIME_SELECTED));	//WA0019
		if (val > 0)																//WA0019
		{
			m_pWndPopup->SetTimer(TIMER_OUTPUT,val*100,NULL);						//WA0019
		}
		else																		//WA0019
			m_pWndPopup->OnTimer(TIMER_OUTPUT);										//WA0019
	}


	if(HIBYTE(GetKeyState(VK_SHIFT)))
	{
		SendShiftString(strText);
	}
	else
	{
		m_pWndPopup->SendString(strText);				// Add Expansion
	}

	m_strText = strText;		// Store the Expansion for Un-Expand

	// Store typed characters
	m_strStore = m_strTyped;
	m_strTyped = _T("");

	m_bExpanded = TRUE;
	ResetScroll();	// make sure scroll is reset after inserting abbreviation
	m_pWndPopup->IsShowingAbbrev(FALSE);	//WA0018
	DisplayList();

	return TRUE;
/*	// Format the expansion in case of multi-line output
	CAbbrevList::FormatMultiLineOutput(strText);	

	m_pWndPopup->SendString(strText);	// Insert the expansion

	m_strText = strText;		// Store the Expansion for Un-Expand

	m_strStore = m_strTyped;	// Store typed characters
	m_strTyped = _T("");

	DisplayList();				// Display the correct list in the popup window*/
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to insert the expansion from the popup window when the 
//				: mouse is clicked on one.  The function strips the abbreviation
//				: from the expansion removes any characters typed and then 
//				: insertes the formatted expansion into the window with keyboard
//				: focus.
/////////////////////////////////////////////////////////////////////////////////

void CDefaultProcessKey::MouseSelectAbbrev(UINT index)
{
//	if(m_AbbrevList)		WA0047
	{
//		SelectAbbrev(m_pWndPopup->GetCurSel());
		SelectAbbrev(index);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Removes the space after an insert, the insert can be of a word
//				: or of an abbreviations expansion, if the character is a 
//				: punctuation.  The space is only removed if space was inserted
//				: because the Automatic space after a word insertion is on.
//
// PARAMETERS	: nChar - Characyer to check to see if it is punctuation.
//
// RETURNS		: TRUE if the space was removed
/////////////////////////////////////////////////////////////////////////////////

BOOL CDefaultProcessKey::RemoveSpaceAfterInsert(UINT nChar)
{
	BOOL bReturn = FALSE;
	// If a Word or Expansion has been inserted and autospace is on and the character being
	// typed is a punctuation character then remove the space that has been
	// inserted after the inserted word.
	if((m_bWordInserted || m_bExpanded) && atoi(m_pSettings->GetSetting(AUTO_SPACE_WORD)) && 
		IsCharPunctuation(CVKConversion::CharToVK(nChar)))
	{
		CString remove = (char)_T(VK_BACK); 
				
		m_pWndPopup->DeleteOutput(remove);
		m_pWndPopup->DeleteOutput(remove);
		remove = nChar;
		m_pWndPopup->DeleteOutput(remove);

		bReturn = TRUE;
	}

	return bReturn;
}

//WA0014 function added to process numbers in abbreviations
BOOL CDefaultProcessKey::IsNumber(char c)
{
	BOOL bReturn = FALSE;

	if (c == '0' || c == '1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' 
		|| c=='7' || c=='8' || c=='9')
	{
		bReturn = TRUE;
	}
	return bReturn;
}

//! Loads the orginal topic list.  This is so that any changes that we have made can be wiped out.
//! The changes that we are likely to make are, making the initial letter of all the words capital.
/*!
	\return TRUE if the Topic Wordlist was successfully opened
*/
// WA0056

BOOL CDefaultProcessKey::LoadOriginalTopicList()
{
	return m_TopicList.LoadWordlist(m_pSettings->GetSetting(TOPIC_FILENAME),TOPIC);
}

//! WA0057 - Called to make the current topic or alphabetic wordlist have the first letter uppercase and get
//! this sent to the DDE client

void CDefaultProcessKey::SendDDEFirstUpperCase()
{
	if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
	{
		if(IsTopicDisplayed())
		{
			m_TopicList.InitialCapital();

			m_pWndPopup->SendDDEMessage(_T("topic"), m_TopicList.GetDDEString());
		}
		else if(IsAlphaDisplayed())
		{
			m_AlphaList.InitialCapital();

			m_pWndPopup->SendDDEMessage(_T("alpha"), m_AlphaList.GetDDEString());
		}
	}
}

//! WA0057 - Called to make the current topic or alphabetic wordlist have all its letters made uppercase and
//! get it sent to the DDE client

void CDefaultProcessKey::SendDDEAllUpperCase()
{
	if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
	{
		if(IsTopicDisplayed())
		{
			m_TopicList.ToUpper();

			m_pWndPopup->SendDDEMessage(_T("topic"), m_TopicList.GetDDEString());
		}
		else if(IsAlphaDisplayed())
		{
			m_AlphaList.ToUpper();

			m_pWndPopup->SendDDEMessage(_T("alpha"), m_AlphaList.GetDDEString());
		}
	}
}

//! WA0057 - Called to make the current topic or alphabetic wordlist have all its letters made lowercase
//! and get it sent to the DDE client

void CDefaultProcessKey::SendDDEAllLowerCase()
{	
	if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
	{
		if(IsTopicDisplayed())
		{
			m_TopicList.ToLower();

			m_pWndPopup->SendDDEMessage(_T("topic"), m_TopicList.GetDDEString());
		}
		else if(IsAlphaDisplayed())
		{
			m_AlphaList.ToLower();

			m_pWndPopup->SendDDEMessage(_T("alpha"), m_AlphaList.GetDDEString());
		}
	}
}