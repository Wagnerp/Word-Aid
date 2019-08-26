// ChildWnd.cpp : implementation file
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0017 - add speech indication when there are no more words above or below the		//
//			current list and display stop button rather than up or down as appropriate	//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0021 - solve problem with abbrev button hotkey special key not being stored		//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	WA0018 - solve problem with window not wide enough when return from displaying		//
//			other windows																//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////
//																						//
	
#include "stdafx.h"
#include "ChildWnd.h"
#include "worddlg.h"
#include "WordAid2.h"

#include "keyhook.h"
#include "SettingIndex.h"		// Has setting that should be got 
#include "VKConversion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//! define default values
//#define MAX_NUMBER_OF_WORDS_TO_DISPLAY		10
#define MIN_WINDOW_WIDTH		50
#define MIN_BUTTON_HEIGHT		30

#define SHIFTED 0x8000 //check for caps lock down

// hotkey definitions for undo and hide wordlist options
#define FUNC1	"F1"
#define FUNC2	"F2"
#define FUNC3	"F3"
#define FUNC4	"F4"
#define FUNC5	"F5"
#define FUNC6	"F6"
#define FUNC7	"F7"
#define FUNC8	"F8"
#define FUNC9	"F9"
#define FUNC10	"F10"
#define FUNC11	"F11"
#define FUNC12	"F12"
//special key definitions for undo and hide wordlist options
#define CTRL_KEY	"c"
#define ALT_KEY		"a"
#define SHIFT_KEY	"s"

#define VK_W 0x57

#define VK_0	0x30
#define VK_1	0x31	
#define VK_2	0x32
#define VK_3	0x33
#define VK_4	0x34
#define VK_5	0x35
#define VK_6	0x36
#define VK_7	0x37
#define VK_8	0x38
#define VK_9	0x39

const int MIN_ABBREV_BUTTON_WIDTH	= 100;

/////////////////////////////////////////////////////////////////////////////
// CChildWnd

CChildWnd::CChildWnd()
{
	m_TextList.RemoveAll();
	m_bButtonsOn			= TRUE;	//999 from settings
	m_nLargestWordSize		= MIN_WINDOW_WIDTH;
	m_nHighlighted			= 0;
	m_nWordlistPos			= 0;
	m_hActiveWnd			= NULL;
	m_nHideWindowHotkey		= VK_F12;
	m_nUndoHotkeyVal		= VK_F11;
	m_nHideWindowSpecial	= 0;
	m_nUndoSpecial			= 0;

	m_bAltDown				= FALSE;
	m_bCtrlDown				= FALSE;
	m_bShiftDown			= FALSE;
	m_bFirstWord			= FALSE;	//WA0027

	//store application directory so loadlibrary can start at right place
	unsigned long lSize = _MAX_PATH; 
	GetCurrentDirectory(lSize,m_strAppPathName.GetBuffer(_MAX_PATH));
	m_strAppPathName.ReleaseBuffer();

	m_hLibraryInstance		= NULL;
	m_nWindowPos			= 0;
	m_bShowingAbbrev		= FALSE;
	m_bAbbrevButton			= FALSE;
	m_bWordInserted			= FALSE;
	m_pBtnUp				= NULL;
	m_pBtnDown				= NULL;
	m_pBtnStop				= NULL;		//WA0017
	m_pBtnAbbrev			= NULL;
//	m_pDDEServer			= NULL;		//WA0031
	m_bHotkeyShow			= FALSE;	//WA0046
}

CChildWnd::~CChildWnd()
{
	if (m_hLibraryInstance)
		(lpfnDisable) ();
	
	FreeLibrary(m_hLibraryInstance);

	m_TextList.RemoveAll();
	m_fontList.DeleteObject();
	//delete m_pDDEServer;
	delete m_pBtnUp;
	delete m_pBtnDown;
	delete m_pBtnAbbrev;
	delete m_pBtnStop;		//WA0017

	if (m_ctrlSpeaker)
		m_ctrlSpeaker.DestroyWindow();
	
	DestroyWindow();

	m_bAbbrevButton = TRUE;
}


BEGIN_MESSAGE_MAP(CChildWnd, CWnd)
	//{{AFX_MSG_MAP(CChildWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_CHAR()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYUP()
	ON_WM_SYSKEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_KEYMATCHED, OnUserKeyMatched)
END_MESSAGE_MAP()

void CChildWnd::DisplayList(CWordlist list)
{
	CString szDDEText = _T("");
	CString prefix = _T("<");
	CString suffix = _T(">");
	ResetContent();
	
	// make sure number of words to display hasn't changed
	m_nMaxNumberOfItems = atoi((const char*)m_pSettings->GetSetting(MAX_WORDS));

	// calculate where in list to start displaying
	int pos = 0;

	if (m_procWordAidKey.GetNoScrolled() > 0 || m_procWordAidAbbrevKey.GetNoScrolled() > 0)	
	{
		m_pBtnUp->EnableButton();

		if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
			atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
			!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
		{
			pos = m_nMaxNumberOfItems * m_procWordAidAbbrevKey.GetNoScrolled();
		}
		else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
		{
			pos = m_nMaxNumberOfItems * m_procWordAidKey.GetNoScrolled();
		}

		if(list.GetSize() <= pos + m_nMaxNumberOfItems)
		{
			m_pBtnDown->EnableButton(FALSE);
		}
		else
		{
			m_pBtnDown->EnableButton();
		}
	}
	else
	{
		if(list.GetSize() <= pos + m_nMaxNumberOfItems)
		{
			m_pBtnDown->EnableButton(FALSE);
		}
		else
		{
			m_pBtnDown->EnableButton();
		}

		m_pBtnUp->EnableButton(FALSE);
	}
	UpdateFont();
	// display words to max defined
	if (list.GetSize() == 0)
	{
		CString szNoMatch = _T("");
		szNoMatch.LoadString(IDS_NO_MATCH);

		AddString(szNoMatch);
	}
	else
	{

		for (int i = pos;i<list.GetSize() && i<(pos+m_nMaxNumberOfItems);i++)
		{
			AddString(list.GetAt(i), TRUE);
		}
	}
//	if(m_pDDEServer)
	{
		for (int i=0;i<list.GetSize();i++)
		{
			szDDEText += list[i];
			szDDEText += (char)ETX;
		}

		if(list.m_nType == ALPHA)
		{
 			m_pDDEServer.SendDDEMessage(_T("alpha"), _T("wordlist"), szDDEText);
		}
		else if(list.m_nType == TOPIC)
		{
			m_pDDEServer.SendDDEMessage(_T("topic"), _T("wordlist"), szDDEText);
		}
		else
		{
			// ERROR the list is not set
			ASSERT(FALSE);
		}

	}

	// make textarea bounding rect size of window
	for (int i=0;i<m_TextList.GetSize();++i)
		m_TextList[i].rectBounding.right = m_nLargestWordSize+10;

	ResizeWindow();

	// Resize buttons to fit new window
	m_pBtnDown->SetBoundingRect(CalculateDownButtonBoundingRect());
	m_pBtnUp->SetBoundingRect(CalculateUpButtonBoundingRect());

	
//	if(m_bAbbrevButton)
	if (m_bAbbrevButton || atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
	{
		m_pBtnAbbrev->SetBoundingRect(CalculateAbbrevButtonBoundingRect());
	}
	else
	// highlight first word in new list
//	m_nHighlighted = m_nWordlistPos = 0;
//	HighlightWord(m_TextList[0].rectBounding,AddKeyToString(m_TextList[0].szText,0));
		m_bShowingAbbrev = FALSE;
}

BOOL CChildWnd::LoadLibraryDll(CString dll)
{
	// get the address of the library loaded by function call
	if (!m_hLibraryInstance)
	{	//ensure looking for dll in correct place
		SetCurrentDirectory(m_strAppPathName);
		m_hLibraryInstance = LoadLibrary(dll);
		if (!m_hLibraryInstance)
		{
			TRACE("%s not loaded",dll);
			return FALSE;
		}
	
	// now get function addresses and assign to function pointers that will
	// be used to call them in code
	lpfnEnable		 = (ENABLE)			GetProcAddress(m_hLibraryInstance,"Enable");
	lpfnIsEnabled	 = (ISENABLED)		GetProcAddress(m_hLibraryInstance,"IsEnabled");
	lpfnDisable		 = (DISABLE)		GetProcAddress(m_hLibraryInstance,"Disable");
	lpfnClearMonitor = (CLEAR_MONITORS)	GetProcAddress(m_hLibraryInstance,"ClearMonitors");
	lpfnMonitor		 = (MONITOR)		GetProcAddress(m_hLibraryInstance,"Monitor");
	lpfnOutputString = (OUTPUT_STRING)	GetProcAddress(m_hLibraryInstance,"OutputString");
	lpfnSetMessages	 = (SET_MESSAGES)	GetProcAddress(m_hLibraryInstance,"SetMessages");
	lpfnGetCapsLock  = (GET_CAPS_LOCK)	GetProcAddress(m_hLibraryInstance,"GetCapsLock");
	lpfnSetCapsLock  = (SET_CAPS_LOCK)	GetProcAddress(m_hLibraryInstance,"SetCapsLock");
	lpfnGetShift	 = (GET_SHIFT)		GetProcAddress(m_hLibraryInstance,"GetShift	");
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChildWnd message handlers

int CChildWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_nMaxNumberOfItems = atoi((const char*)m_pSettings->GetSetting(MAX_WORDS));	//999 from settings
	
	m_fontList.DeleteObject();
	m_fontList.CreateFont(atoi((const char*)(m_pSettings->GetSetting(LABEL_FONT_SIZE))), 
						0, 0, 0, FW_DONTCARE, FALSE, FALSE, 0, ANSI_CHARSET, 
					  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					  DEFAULT_PITCH, m_pSettings->GetSetting(LABEL_FONT_NAME));

	// Create the buttons if turned on
	if(m_bButtonsOn)
	{
		CreateButtons();

		 m_pBtnUp->EnableButton(FALSE);
	}

	// Create Abbrev Button
	CString strAbbrev = _T("");
	CRect abvRect;

	abvRect = CalculateAbbrevButtonBoundingRect();

	strAbbrev.LoadString(IDS_ABBREV_BUTTON);

	// WA0031 - Make sure memory is cleaned up
	if(m_pBtnAbbrev)
	{
		delete m_pBtnAbbrev;
	}

	m_pBtnAbbrev = new CWAButton(abvRect, strAbbrev, m_pSettings->GetColourSetting(BUTTON_BACKGROUND),
						m_pSettings->GetColourSetting(BUTTON_FOREGROUND),
						atoi((const char*)(m_pSettings->GetSetting(BUTTON_FONT_SIZE))),
						m_pSettings->GetSetting(BUTTON_FONT_NAME));


	// Create DDE Server for Word Aid
	/*m_pDDEServer = new CMyServer();

	ASSERT(m_pDDEServer);*/

	
    if (!m_pDDEServer.Create(_T("Wordaid"))) 
	{
        char buf[128];
        sprintf(buf,"Service failed to start. Error: %d", m_pDDEServer.GetLastError());
        AfxMessageBox(buf);
    }

	if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
	{
		// start by displaying the topic file
		if (m_procWordAidKey.m_TopicList.GetSize() > 0)
			DisplayList(m_procWordAidKey.m_TopicList);
	}

	return 0;
}

void CChildWnd::OnShowWindow( BOOL bShow, UINT nStatus )
{
	if(bShow)
	{
		CheckStickyKeys();

		//load registry settings
		m_pSettings->LoadSettings();

		m_bShow = atoi((const char*)m_pSettings->GetSetting(STARTUP_MINIMIZE));
		
		// get the word selection keys from the registry entry
		CString set = m_pSettings->GetSetting(SELECT_WORD_KEYS);		

		m_nUndoHotkeyVal = ConvertToFuncKey(m_pSettings->GetSetting(UNDO_HOTKEY));
		m_nUndoSpecial = ConvertToSpecialKey(m_pSettings->GetSetting(UNDO_SPEC));
		m_nHideWindowHotkey = ConvertToFuncKey(m_pSettings->GetSetting(SHOW_LIST_HOTKEY));
		m_nHideWindowSpecial = ConvertToSpecialKey(m_pSettings->GetSetting(SHOW_LIST_SPEC));

		// Display the correct buttons either Bitmap or text
		CreateButtons();

		// show window
		TRACE("called show window - words = %d\n",m_nMaxNumberOfItems);
		if (bShow && !m_hLibraryInstance)
		{//get settings from settings object here ***
			// load kyhook dll here
			if (!LoadLibraryDll(KEYHOOK_DLL))
				TRACE("Haven't loaded keyhook dll\n");
			// now enable it
			if (!(lpfnEnable) (GetSafeHwnd()))
				TRACE("Call to Enable didn't work\n");
			// setup which messages to monitor

			// WA0044 - Only set Hotkeys if the window is being shown
			if(!m_bShow)
			{
				SetMonitorKeys();	
			}
			else
			{
				SetMonitorsHiddenWnd();
			}
		}

		if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
			atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
			!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))		// WORDAID & ABBREV ON
		{
			m_bAbbrevButton = FALSE;

			// Set popup Window pointer and settings pointer
			m_procWordAidAbbrevKey.SetPopupWindow(this);
			m_procWordAidAbbrevKey.SetSettings(m_pSettings);

			m_procWordAidAbbrevKey.m_AlphaList.SetMinLength(atoi(m_pSettings->GetSetting(MIN_WORD_LENGTH)));
			m_procWordAidAbbrevKey.m_TopicList.SetMinLength(atoi(m_pSettings->GetSetting(MIN_WORD_LENGTH)));
			m_procWordAidAbbrevKey.m_AlphaList.SetMaxLength(atoi(m_pSettings->GetSetting(MAX_WORD_LENGTH)));
			m_procWordAidAbbrevKey.m_TopicList.SetMaxLength(atoi(m_pSettings->GetSetting(MAX_WORD_LENGTH)));

			// Character tpyed and an alpha list display alpha list
			if (!m_procWordAidAbbrevKey.GetCharsTyped().IsEmpty() &&
				m_procWordAidAbbrevKey.m_AlphaList.GetSize() > 0)	
			{
				if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
				{
					// Word Bank on display abbrev button
					m_bAbbrevButton = TRUE;
				}
				else
				{
					m_bAbbrevButton = FALSE;
				}

				m_procWordAidAbbrevKey.m_lstWordDisplay = m_procWordAidAbbrevKey.m_AlphaList.Search(m_procWordAidAbbrevKey.GetCharsTyped());
				IsShowingAbbrev(FALSE);			//WA0018
				DisplayList(m_procWordAidAbbrevKey.m_lstWordDisplay);
				m_procWordAidAbbrevKey.AlphaListDisplayed();
			}
			else if (m_procWordAidAbbrevKey.m_TopicList.GetSize() > 0)	// Topic list display
			{
				if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
				{
					// Word Bank on display abbrev button
					m_bAbbrevButton = TRUE;
				}
				else
				{
					m_bAbbrevButton = FALSE;
				}

				m_procWordAidAbbrevKey.m_lstWordDisplay = m_procWordAidAbbrevKey.m_TopicList;
				IsShowingAbbrev(FALSE);			//WA0018
				DisplayList(m_procWordAidAbbrevKey.m_TopicList);
				m_procWordAidAbbrevKey.TopicListDisplayed();
			}
			else // Display Abbrev List
			{
				m_procWordAidAbbrevKey.m_AbbrevStoreList = m_procAbbrevKey.m_AbbrevList.Search(m_procAbbrevKey.GetCharsTyped());
				DisplayAbbrevList(m_procWordAidAbbrevKey.m_AbbrevStoreList);
				m_procWordAidAbbrevKey.AbbrevListDisplayed();
			}
		}
		else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))			// WORD AID ONLY ON
		{
			m_bAbbrevButton = FALSE;

			m_procWordAidKey.SetPopupWindow(this);
			m_procWordAidKey.SetSettings(m_pSettings);

			m_procWordAidKey.m_AlphaList.SetMinLength(atoi(m_pSettings->GetSetting(MIN_WORD_LENGTH)));
			m_procWordAidKey.m_TopicList.SetMinLength(atoi(m_pSettings->GetSetting(MIN_WORD_LENGTH)));
			m_procWordAidKey.m_AlphaList.SetMaxLength(atoi(m_pSettings->GetSetting(MAX_WORD_LENGTH)));
			m_procWordAidKey.m_TopicList.SetMaxLength(atoi(m_pSettings->GetSetting(MAX_WORD_LENGTH)));

			if (!m_procWordAidKey.GetCharsTyped().IsEmpty())
			{
				m_procWordAidKey.m_lstWordDisplay = m_procWordAidKey.m_AlphaList.Search(m_procWordAidKey.GetCharsTyped());

				// If the size is greater than zero then display alpha list otherwise display topic
				if(m_procWordAidKey.m_lstWordDisplay.GetSize() > 0)
				{
					DisplayList(m_procWordAidKey.m_lstWordDisplay);
				}
				else
				{
					m_procWordAidKey.m_lstWordDisplay = m_procWordAidKey.m_TopicList;
					DisplayList(m_procWordAidKey.m_TopicList);
				}
			}
			else if (m_procWordAidKey.m_TopicList.GetSize() > 0)
			{
				m_procWordAidKey.m_lstWordDisplay = m_procWordAidKey.m_TopicList;
				DisplayList(m_procWordAidKey.m_TopicList);
			}
		}
		else if(atoi(m_pSettings->GetSetting(ABBREV_ON)))		// ABBREV ONLY ON
		{
			m_bAbbrevButton = FALSE;

			// Set Popup Window pointer and settings pointer.
			m_procAbbrevKey.SetPopupWindow(this);
			m_procAbbrevKey.SetSettings(m_pSettings);
			m_procAbbrevKey.SetAbbrevOnly();

			// Word Bank and Listed Expansion is on then
			if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) && atoi(m_pSettings->GetSetting(ABBREV_LISTED_EXPANSION)))
			{
				if(m_procAbbrevKey.GotChars())
				{
					m_procAbbrevKey.DisplayList(m_procAbbrevKey.GetCharsTyped());
					//DisplayAbbrevList(&m_procAbbrevKey.m_AbbrevList->Search(m_procAbbrevKey.GetCharsTyped()));
				}
				else
				{
					m_procAbbrevKey.DisplayList();
					//DisplayAbbrevList(m_procAbbrevKey.m_AbbrevList);
				}
			}
			else if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
			{
				m_procAbbrevKey.DisplayList();
				//DisplayAbbrevList(m_procAbbrevKey.m_AbbrevList);
			}
			else
			{
				m_procAbbrevKey.DisplayList();
				// TODO : Make the window not appear
				ShowWindow(SW_HIDE);
			}
		}
		
		// set up speech, if required
		m_bSpeech = atoi((const char*)m_pSettings->GetSetting(USE_SPEECH));

		if (!m_ctrlSpeaker && m_bSpeech)
		{
			m_ctrlSpeaker.Create("Speaker",WS_CHILD,CRect(0,0,1,1),this,1);
		}
		
		if(m_ctrlSpeaker)
		{
			// WA0046 - If the ShowWindow message has been sent because of the Hotkey being pressed then
			// the voice settings do not need setting.  As call to RestoreVoice uses memory
			if(!m_bHotkeyShow)
			{
				m_ctrlSpeaker.RestoreVoice("WORDAID");
				m_ctrlSpeaker.SetPitch(atoi((const char*)m_pSettings->GetSetting(SPEECH_PITCH)));	//WA0027
				m_ctrlSpeaker.SetSpeed(atoi((const char*)m_pSettings->GetSetting(SPEECH_SPEED)));	//WA0027
				m_ctrlSpeaker.SetVolume(atoi((const char*)m_pSettings->GetSetting(SPEECH_VOLUME)));	//WA0027
			}
		}
	}
}

void CChildWnd::OnClose() 
{
	(lpfnDisable) ();

	CWnd::OnClose();
}

void CChildWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))	
	{
		m_procWordAidAbbrevKey.OnChar(nChar, nRepCnt, nFlags);

		SetAbbrevButtonText();		//WA0033
	}
	else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))	// WORDAID ON ONLY
	{
		m_procWordAidKey.OnChar(nChar, nRepCnt, nFlags);
	}
	else if(atoi(m_pSettings->GetSetting(ABBREV_ON)))
	{
		m_procAbbrevKey.OnChar(nChar, nRepCnt, nFlags);
	}

	// Sticky keys on then turn off after character press
	if(IsStickyKeysOn())
	{
		m_bStickySHIFT	= FALSE;
		m_bSitckyCTRL	= FALSE;
		m_bStickyALT	= FALSE;
	}
}

void CChildWnd::OutputString(CString str, UINT count, BOOL bDDE /*= FALSE*/)
{
	CString szNoMatch = _T("");
	szNoMatch.LoadString(IDS_NO_MATCH);
	
	if(szNoMatch != str)
	{
		CString backspace = (char)_T(VK_BACK);
		CString space = (char)_T(VK_SPACE);
		if (!m_bShow || bDDE)
		{
	    		// remove letters already output		
			for (int i =0; i<(int)count;i++)
				(lpfnOutputString) (backspace);
// this is a desperate fix to ensure alt and ctrl keys work as special keys in 95/98/ME
// added 04/10/2002
			keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_KEYUP, 0);
			keybd_event(VK_MENU, MapVirtualKey(VK_MENU,0), KEYEVENTF_KEYUP, 0);

 			(lpfnOutputString) (str);
			m_nSizeLastOutput = str.GetLength();
		
			if (atoi((const char*)(m_pSettings->GetSetting(AUTO_SPACE_WORD))))
			{
				(lpfnOutputString) (space);
				m_bSpaceOutput = TRUE;
			}
		}

		// Set speak on insert timer if required
		if ( atoi((const char*)m_pSettings->GetSetting(SPEAK_SELECTED)) )
		{
			int val = atoi((const char*)m_pSettings->GetSetting(DWELL_TIME_SELECTED));

			if (val > 0)
			{
				SetTimer(TIMER_OUTPUT,val*100,NULL);
			}
			else
				OnTimer(TIMER_OUTPUT);
		}
	
		// Word jst inserted
		m_bWordInserted = TRUE;
		

		// now clear the wordlist window
  		ResetContent();
//		m_prockKey.m_nScrolled = 0;

	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))	
	{
		// WA0056 - Make sure the original Topic list is displayed
		m_procWordAidAbbrevKey.LoadOriginalTopicList();
		DisplayList(m_procWordAidAbbrevKey.m_TopicList);
	}
	else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))	// WORDAID ON ONLY
	{
		// WA0056 - Make sure the original Topic list is displayed
		m_procWordAidKey.LoadOriginalTopicList();
		DisplayList(m_procWordAidKey.m_TopicList);
	}
	else if(atoi(m_pSettings->GetSetting(ABBREV_ON)))
	{
		// WA0056 - Make sure the original Topic list is displayed
		m_procAbbrevKey.LoadOriginalTopicList();
		DisplayList(m_procAbbrevKey.m_TopicList);
	}
//		DisplayList(m_procWordAidKey.m_TopicList); 

		//m_procWordAidKey.ClearTyped();
	}
}

// handles any keydown messages relevant to list window
// messages that manipulate wordlists are handles by processkey class
void CChildWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	BOOL remove = FALSE;

	// If the shift is being called because it is still down then 
	// to stop the popup window re-drawing just exit function
	if(m_bShiftDown && nChar == VK_SHIFT)
	{
		return;
	}

	if(nChar == VK_SHIFT)	// SHIFT button down
	{
		if(IsStickyKeysOn())
		{
			m_bStickySHIFT = TRUE;
		}

		m_bShiftDown = TRUE;	
	}
	else if(nChar == VK_CONTROL)
	{
		if(IsStickyKeysOn())
		{
			m_bSitckyCTRL = TRUE;
		}
	}
	else if(nChar == VK_MENU)
	{
		if(IsStickyKeysOn())
		{
			m_bStickyALT = TRUE;
		}
	}

	// WA0043 - Up Down Left and Right caught in OnUserKeyMatched
	// handle up & down arrows to move thru word list if list window showing
/*	if (nChar == (UINT)atoi(m_pSettings->GetSetting(UPARR_KEY)) && !m_bShow)
	{
		MoveUpList();
	}
	else if (nChar == (UINT)atoi(m_pSettings->GetSetting(DWNARR_KEY)) && !m_bShow)
	{//! m_TextList is a 1 based index not 0
		MoveDownList();
	}
	else if (nChar == (UINT)atoi(m_pSettings->GetSetting(LFTARR_KEY)) && !m_bShow) // handle move window keys - left & right arrows - if list window showing
	{
		MovePopupLeft();
		remove = TRUE;
	}
	else if (nChar == (UINT)atoi(m_pSettings->GetSetting(RGHTARR_KEY)) && !m_bShow)
	{
		MovePopupRight();
		remove = TRUE;
	}
	else*/ if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{//wordaid and abbrev
		m_procWordAidAbbrevKey.OnKeyDown(nChar, nRepCnt, nFlags);
	}
	else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
	{//just wordaid
		m_procWordAidKey.OnKeyDown(nChar, nRepCnt, nFlags);
		//remove = WordAidKeyDown(nChar, nRepCnt, nFlags);
	}
	else if(atoi(m_pSettings->GetSetting(ABBREV_ON)))
	{//just abbrev
		m_procAbbrevKey.OnKeyDown(nChar, nRepCnt, nFlags);
	}
	
	// if not processed, pass back to queue
	if (!remove)
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CChildWnd::SetSorted(BOOL sorted)
{
//	m_prockKey.SetSorted(sorted);
}

int CChildWnd::AddString(CString szText, BOOL bAddKey /*=TRUE*/)
{
	TEXT_AREA lastArea;
	TEXT_AREA textArea;		// Used to create new text area

	// Make sure you cannot add more than the maximum number of words to list
	ASSERT(m_TextList.GetSize() <= m_nMaxNumberOfItems);

	// Store the text in struture
	textArea.szText = szText;

	CDC* pDC;
	pDC = GetDC();

	CFont* pOldFont = (CFont*)pDC->SelectObject(m_fontList);

	CSize fontSize;

	// Only add key to string if WordAid list
	if(bAddKey)
	{
		fontSize = pDC->GetTextExtent(AddKeyToString(szText, m_TextList.GetSize()));
	}
	else
	{
		fontSize = pDC->GetTextExtent(szText);
	}
	
	// See if longest
	if(fontSize.cx > m_nLargestWordSize)
	{
		m_nLargestWordSize = fontSize.cx+1;

	}

	// Calculate next bounding rectangle
	if(m_TextList.GetSize() > 0)
	{
		lastArea = m_TextList.GetAt(m_TextList.GetSize() -1);

		textArea.rectBounding.top = lastArea.rectBounding.bottom;
		textArea.rectBounding.left = lastArea.rectBounding.left;
		textArea.rectBounding.bottom = textArea.rectBounding.top + fontSize.cy;   // NEED TO BE CALCULATED
		if(m_nLargestWordSize > MIN_WINDOW_WIDTH)
		{
			textArea.rectBounding.right = m_nLargestWordSize;
		}
		else
		{
			textArea.rectBounding.right = MIN_WINDOW_WIDTH;
		}
	}
	else
	{
		if(m_bButtonsOn)
		{
			textArea.rectBounding.top = m_pBtnUp->GetBottom();
			textArea.rectBounding.left = m_pBtnUp->GetLeft();
			textArea.rectBounding.bottom = textArea.rectBounding.top + fontSize.cy; // NEED TO BE CHANGED 
			if(m_nLargestWordSize > MIN_WINDOW_WIDTH)
			{
				textArea.rectBounding.right = m_nLargestWordSize;
			}
			else
			{
				textArea.rectBounding.right = MIN_WINDOW_WIDTH;
			}
		}
		else
		{
			CRect rectWindow;

			// Get the rectangle of the child window, this is returned in Screen Co-ordinates
			GetWindowRect(rectWindow);

			// Convert to client co-ordinates
			ScreenToClient(rectWindow);

			textArea.rectBounding.top = rectWindow.top;
			textArea.rectBounding.left = rectWindow.left;
			textArea.rectBounding.bottom = textArea.rectBounding.top + fontSize.cy; // NEED TO BE CHANGED 
			if(m_nLargestWordSize > MIN_WINDOW_WIDTH)
			{
				textArea.rectBounding.right = m_nLargestWordSize;
			}
			else
			{
				textArea.rectBounding.right = MIN_WINDOW_WIDTH;
			}
		}
	}	

	ReleaseDC(pDC);
	return m_TextList.Add(textArea);
}

void CChildWnd::ResetContent()
{
	//Clear list
	m_TextList.RemoveAll();

	m_nLargestWordSize = MIN_WINDOW_WIDTH;			// Reset largest word
	m_nHighlighted = m_nWordlistPos = 0;	//reset position in wordlist

	// Redraw as blank
	Invalidate();
}

void CChildWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	TEXT_AREA textArea;
	CRect rectWindow;

	GetWindowRect(rectWindow);

	ScreenToClient(rectWindow);

	CBrush brush;

	UpdateFont();
	CFont* oldFont = (CFont*)dc.SelectObject(&m_fontList);
	// Create brush for Background Colour
	brush.CreateSolidBrush(m_pSettings->GetColourSetting(LABEL_BACKGROUND));

	dc.FillRect(rectWindow, &brush);

	// Need to set the font //999 from settings

	// Display Words in window
	for(int i = 0; i < m_TextList.GetSize(); i++)
	{
		textArea = m_TextList.GetAt(i);

		dc.SetBkColor(m_pSettings->GetColourSetting(LABEL_BACKGROUND));
		dc.SetTextColor(m_pSettings->GetColourSetting(LABEL_FOREGROUND));		


		dc.TextOut(textArea.rectBounding.left+4,
				   textArea.rectBounding.top,
				   AddKeyToString(textArea.szText, i));		// add 4 to start to move word away from edge
	}

	dc.SelectObject(oldFont);
	brush.DeleteObject();

	if (m_pBtnUp->IsButtonEnabled())											//WA0017
	{//set back/foreground colours for buttons then draw them			
		m_pBtnUp->m_rgbBackGround = m_pSettings->GetColourSetting(BUTTON_BACKGROUND);
		m_pBtnUp->m_rgbForeGround = m_pSettings->GetColourSetting(BUTTON_FOREGROUND);
		m_pBtnUp->SetFontFace(m_pSettings->GetSetting(BUTTON_FONT_NAME));
		m_pBtnUp->SetFontSize(atoi((const char*)(m_pSettings->GetSetting(BUTTON_FONT_SIZE))));
		m_pBtnUp->Draw(&dc);
	}
	else																			//WA0017
	{																				//WA0017
		TRACE("draw stop button");														//WA0017
		m_pBtnStop->m_rgbBackGround = m_pSettings->GetColourSetting(BUTTON_BACKGROUND);	//WA0017
		m_pBtnStop->m_rgbForeGround = m_pSettings->GetColourSetting(BUTTON_FOREGROUND);	//WA0017
		m_pBtnStop->SetFontFace(m_pSettings->GetSetting(BUTTON_FONT_NAME));				//WA0017	
		m_pBtnStop->SetFontSize(atoi((const char*)(m_pSettings->GetSetting(BUTTON_FONT_SIZE))));	//WA0017
		m_pBtnStop->SetBoundingRect(m_pBtnUp->m_rectOuter);								//WA0017
		m_pBtnStop->Draw(&dc);															//WA0017	
	}																					//WA0017
	if (m_pBtnDown->IsButtonEnabled())													//WA0017
	{
		m_pBtnDown->m_rgbBackGround = m_pSettings->GetColourSetting(BUTTON_BACKGROUND);
		m_pBtnDown->m_rgbForeGround = m_pSettings->GetColourSetting(BUTTON_FOREGROUND);
		m_pBtnDown->SetFontFace(m_pSettings->GetSetting(BUTTON_FONT_NAME));
		m_pBtnDown->SetFontSize(atoi((const char*)(m_pSettings->GetSetting(BUTTON_FONT_SIZE))));
		m_pBtnDown->Draw(&dc);
	}
	else																				//WA0017
	{																					//WA0017
		TRACE("draw stop button down");													//WA0017
		m_pBtnStop->m_rgbBackGround = m_pSettings->GetColourSetting(BUTTON_BACKGROUND);	//WA0017
		m_pBtnStop->m_rgbForeGround = m_pSettings->GetColourSetting(BUTTON_FOREGROUND);	//WA0017
		m_pBtnStop->SetFontFace(m_pSettings->GetSetting(BUTTON_FONT_NAME));				//WA0017
		m_pBtnStop->SetFontSize(atoi((const char*)(m_pSettings->GetSetting(BUTTON_FONT_SIZE))));	//WA0017
		m_pBtnStop->SetBoundingRect(m_pBtnDown->m_rectOuter);							//WA0017
		m_pBtnStop->Draw(&dc);															//WA0017
	}																					//WA0017

//	if(m_bAbbrevButton  && atoi(m_pSettings->GetSetting(WORDAID_ON)) &&
	if (m_bAbbrevButton || atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) && 
		atoi(m_pSettings->GetSetting(WORDAID_ON)) &&	
		atoi(m_pSettings->GetSetting(ABBREV_ON)) && 
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		m_pBtnAbbrev->m_rgbBackGround = m_pSettings->GetColourSetting(BUTTON_BACKGROUND);
		m_pBtnAbbrev->m_rgbForeGround = m_pSettings->GetColourSetting(BUTTON_FOREGROUND);
		m_pBtnAbbrev->SetFontFace(m_pSettings->GetSetting(BUTTON_FONT_NAME));
		m_pBtnAbbrev->SetFontSize(atoi((const char*)(m_pSettings->GetSetting(BUTTON_FONT_SIZE))));
		m_pBtnAbbrev->Draw(&dc);	
	}
	if( m_TextList.GetSize() != 0)
	{
		if (atoi((const char*)m_pSettings->GetSetting(SPEAK_FIRST_AUTO)))	//WA0027
			m_bFirstWord = FALSE;		//WA0027
		else	//WA0027
			m_bFirstWord = TRUE;	//WA0027

		//! highlight first item in list
		// WA0032
		//HighlightWord(m_TextList[m_nHighlighted].rectBounding,AddKeyToString(m_TextList[m_nHighlighted].szText, m_nHighlighted));
		HighlightWord(m_nHighlighted);
		m_bFirstWord = FALSE;		//WA0027
	}

	dc.SelectObject(&oldFont);
}

CString CChildWnd::AddKeyToString(const CString szText, int nNum)
{
	// Do not add key if Showing abbreviation list
	if(m_bShowingAbbrev)
	{
		return szText;		// Just return text to display
	}

	CString strTemp = _T("");
	CString prefix = _T("");
	CString suffix = _T("");

	// Don't add keys to word if no match
	CString szNoMatch = _T("");
	szNoMatch.LoadString(IDS_NO_MATCH);
	CString szNoKeys = _T("");
	szNoKeys.LoadString(IDS_NO_KEYS);

	if(szNoMatch != szText && (m_pSettings->GetSetting(SELECT_WORD_KEYS) != szNoKeys))//added 17th July
	{

		char c[3]; itoa(nNum,c,10);
		CString tag = _T("");

		CString strFuncKeys = _T("");
		CString strNumPadKeys = _T("");

		strFuncKeys.LoadString(IDS_FUNC_KEYS);
		strNumPadKeys.LoadString(IDS_NUMPAD_KEYS);

		if(m_pSettings->GetSetting(SELECT_WORD_KEYS) == strFuncKeys)
		{
			CString f = _T("F");
			itoa(nNum+1,c,10);
			tag += f + c;
		}
		else if(m_pSettings->GetSetting(SELECT_WORD_KEYS) == strNumPadKeys)
		{
			tag = c;
		}
		else
		{
			if (nNum <9)
				itoa(nNum+1,c,10);
			else
				itoa(0,c,10);
			tag = c;
		}

		strTemp = prefix ;
		strTemp+= tag + suffix;
		strTemp += " ";
		strTemp += szText;
	}
	else
	{
		strTemp = szText;
	}
	return strTemp;
}

void CChildWnd::ResizeWindow()
{
	// Calculate Height of window
	int nHeight = 0;

	CDC* pDC;
	pDC = GetDC();

	CFont* pOldFont = (CFont*)pDC->SelectObject(m_fontList);
	CSize fontSize = pDC->GetTextExtent(_T(" "));

	nHeight = fontSize.cy * m_TextList.GetSize();

	if(m_bButtonsOn)
	{
		nHeight += m_pBtnUp->GetHeight();
		nHeight += m_pBtnDown->GetHeight();
	}

	// Calculate Width
	int nWidth = m_nLargestWordSize + 10;

	// When the abbreviation button is being displayed make sure that 
	// the down and abbrev button are displayed correctly by setting
	// the width to no smaller than the minimum width
//	if(m_bAbbrevButton)
	if (m_bAbbrevButton || atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
	{
		if(nWidth < MIN_ABBREV_BUTTON_WIDTH)
		{
			nWidth = MIN_ABBREV_BUTTON_WIDTH;
		}
	}

	// Get Window rect for Top left position
	CRect rectWindow;

	GetWindowRect(rectWindow);

	ScreenToClient(rectWindow);	// Not required when not in a parent window

	// Resize window
	CalculateWndGrid(nWidth, nHeight);		
	MoveWindow(m_rectArrayWndPositions[m_nWindowPos].x, m_rectArrayWndPositions[m_nWindowPos].y, nWidth, nHeight+2, TRUE); 

	ReleaseDC(pDC);
}


CRect CChildWnd::CalculateUpButtonBoundingRect()
{
	CRect upRect, wndRect;
	GetWindowRect(wndRect);

	upRect.top = 0;
	upRect.left = 0;
	upRect.bottom = upRect.top + MIN_BUTTON_HEIGHT;
	upRect.right = upRect.left + wndRect.Width()-2;

	return upRect;
}

CRect CChildWnd::CalculateDownButtonBoundingRect()
{
	CRect downRect, windowRect;
	CDC* pDC;
	pDC = GetDC();
	CSize fontSize;
	CString szDown = _T("");

	fontSize = pDC->GetTextExtent(_T(" "));

	// Top poisiotn is found by looking at the maximum number of items then multiplying it
	// by the font height and adding the up buttons height
	downRect.top = (m_TextList.GetSize() * fontSize.cy) + MIN_BUTTON_HEIGHT;
	downRect.left = 0;
	CRect rectWindow;
	GetWindowRect(rectWindow);
	

	// WA0052 - Make sure the Down button is drawn across window if Abbrev is on and
	// Word Aid is off
	/*if (m_bAbbrevButton || atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)) &&
		(atoi(m_pSettings->GetSetting(ABBREV_ON))*/
	if (m_bAbbrevButton || atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)) &&
		(atoi(m_pSettings->GetSetting(ABBREV_ON)) && 
		(atoi(m_pSettings->GetSetting(WORDAID_ON)))))	// If abbrev button on make down shorter to show abbrev
	{
		downRect.right = downRect.left + (rectWindow.Width() - (3*fontSize.cy));
	}
	else
	{
		downRect.right = downRect.left + rectWindow.Width();
	}
	downRect.bottom = downRect.top + MIN_BUTTON_HEIGHT; 

	ReleaseDC(pDC);

	return downRect;
}

CRect CChildWnd::CalculateAbbrevButtonBoundingRect()
{
	CDC* pDC;
	pDC = GetDC();
	CSize fontSize;
	CString szDown = _T("");

	fontSize = pDC->GetTextExtent(_T(" "));

	// Top poisiotn is found by looking at the maximum number of items then multiplying it
	// by the font height and adding the up buttons height
	CRect abbrevRect;
	abbrevRect.top = (m_TextList.GetSize() * fontSize.cy) + MIN_BUTTON_HEIGHT;
	CRect rectWindow;
	GetWindowRect(rectWindow);
	abbrevRect.left = rectWindow.Width() - (3*fontSize.cy);
	abbrevRect.right = abbrevRect.left + (3*fontSize.cy)-2;
	abbrevRect.bottom = abbrevRect.top + MIN_BUTTON_HEIGHT; 
//	abbrevRect.left = m_pBtnDown->GetRight();
	ReleaseDC(pDC);
	return abbrevRect;
}

void CChildWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	BOOL bInTextBox = FALSE;

	for (int i =0;i<m_TextList.GetSize();i++)
	{

		if (m_TextList[i].rectBounding.PtInRect(point))
		{//999 from settings
			TRACE("Highlight %s\n",m_TextList[i].szText);

			if (m_nHighlighted < m_TextList.GetSize() && i != m_nHighlighted)
			{
				//WA0032
				//Unhighlight(m_TextList[m_nHighlighted].rectBounding,AddKeyToString(m_TextList[m_nHighlighted].szText, m_nHighlighted));
				UnHighlight(m_nHighlighted);
				//HighlightWord(m_TextList[i].rectBounding,AddKeyToString(m_TextList[i].szText, i));
				HighlightWord(i);
			}
			m_nWordlistPos =m_nHighlighted=i; // store which word is currently highlighted

			i = m_TextList.GetSize();

			bInTextBox = TRUE;
		}
	}

	if(!bInTextBox)
	{
		// WA0032 - If the user moves onto one of the buttons then do not speak any words
		KillTimer(TIMER_SCROLL);

		UnHighlight(m_nHighlighted);

		m_nWordlistPos = m_nHighlighted = -1;
	}

	CWnd::OnMouseMove(nFlags, point);
}

//! Unhighlights a sel within the WordAid ChildWnd.
/*!
	\param  nSel - the sel to highlight.  -1 means no sel
*/

//WA0032
//void CChildWnd::Unhighlight(CRect rect, CString word)
void CChildWnd::UnHighlight(int nSel)
{
	//WA0032
	ASSERT(nSel < m_TextList.GetSize());	// TOO LARGE

	if(nSel == -1)
	{
		return;
	}

	CRect	rect = m_TextList[nSel].rectBounding;
	CString word = AddKeyToString(m_TextList[nSel].szText, nSel);	//WA0032

	CDC* pdc;
	pdc = GetDC();
	CBrush brush;

	brush.CreateSolidBrush(m_pSettings->GetColourSetting(LABEL_BACKGROUND));

	pdc->FillRect(&rect,&brush);
	pdc->SetBkColor(m_pSettings->GetColourSetting(LABEL_BACKGROUND));
	pdc->SetTextColor(m_pSettings->GetColourSetting(LABEL_FOREGROUND));

	pdc->SelectObject(&m_fontList);
	pdc->TextOut(rect.left+4, rect.top, word);		// add 4 to start to move word away from edge
	brush.DeleteObject();

	ReleaseDC(pdc);
}

//! Highlights a word in the ChildWindow
/*!
	\param  nSel - the Sel within in the child window to highlight
*/

//WA0032
void CChildWnd::HighlightWord(int nSel)
//void CChildWnd::HighlightWord(CRect rect, CString word)
{	
	//WA0032
	ASSERT(nSel < m_TextList.GetSize());	// TOO LARGE

	if(nSel == -1)
	{
		return;
	}

	CRect rect = m_TextList[nSel].rectBounding;
	CString word = AddKeyToString(m_TextList[nSel].szText,nSel);

	CDC* pdc;
	pdc = GetDC();
	CBrush brush;

	pdc->SelectObject(&m_fontList);
	brush.CreateSolidBrush(m_pSettings->GetColourSetting(HIGHLIGHT_COLOUR));
	pdc->FillRect(&rect,&brush);

	pdc->SetBkColor(m_pSettings->GetColourSetting(HIGHLIGHT_COLOUR));
	pdc->SetTextColor(m_pSettings->GetColourSetting(LABEL_FOREGROUND));
	pdc->TextOut(rect.left+4, rect.top, word);		// add 4 to start to move word away from edge

	//set timer
	if (m_bSpeech)
	{
		int val = 0;
		if ( atoi((const char*)m_pSettings->GetSetting(SPEAK_ALL)) )
			val = atoi((const char*)m_pSettings->GetSetting(DWELL_TIME_ALL));
		if (atoi((const char*)m_pSettings->GetSetting(SPEAK_FIRST_AUTO)) && !m_bFirstWord)	//WA0027
			SetTimer(TIMER_SCROLL,val*100,NULL);	//WA0027
	//	if ( atoi((const char*)m_pSettings->GetSetting(SPEAK_SELECTED)))
	//		val = atoi((const char*)m_pSettings->GetSetting(DWELL_TIME_SELECTED));
	//	if (val > 0)		
		else if (val > 0 && !m_bFirstWord)	//WA0027 
			SetTimer(TIMER_SCROLL,val*100,NULL);
	//	else
	//		SetTimer(TIMER_SCROLL,100,NULL);
	}

	brush.DeleteObject();
	ReleaseDC(pdc);
}

void CChildWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{// add processing for arrow keys ***

	BOOL bRemove = FALSE;

	if(nChar == VK_SHIFT)	// Shift Key Up
	{
		m_bShiftDown = FALSE;
	}

	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		m_procWordAidAbbrevKey.KeyUp(nChar);
	}
	else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
	{
		m_procWordAidKey.KeyUp(nChar);
	}

	
	else	// if it's not selection key pass it on
		CWnd::OnKeyUp(nChar, nRepCnt, nFlags);

}

void CChildWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CString strNoneAbove = _T("");	//WA0017
	CString strNoneBelow = _T("");	//WA0017
	strNoneAbove.LoadString(IDS_NONE_ABOVE);	//WA0017
	strNoneBelow.LoadString(IDS_NONE_BELOW);	//WA0017

	// TODO: Set WordAid and Abbrev specific information.
	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		// Abbrev button clicked display abbrev
		if(m_pBtnAbbrev->IsButtonEnabled() && m_pBtnAbbrev->InRect(point))
		{
			m_procWordAidAbbrevKey.OnAbbrevButtonDown();

			SetAbbrevButtonText();		//WA0033
		}
		else if (m_pBtnDown->IsButtonEnabled() && m_pBtnDown->InRect(point))
		{
			m_procWordAidAbbrevKey.OnButtonDown();
		}
		else if (m_pBtnUp->IsButtonEnabled() && m_pBtnUp->InRect(point))
		{
			m_procWordAidAbbrevKey.OnButtonUp();
		}
		else if (!m_pBtnUp->IsButtonEnabled()&& m_pBtnUp->InRect(point))	//WA0017
		{																	//WA0017
			if (m_ctrlSpeaker)												//WA0017
				m_ctrlSpeaker.Speak(strNoneAbove);							//WA0017
		}																	//WA0017
		else if (!m_pBtnDown->IsButtonEnabled()&& m_pBtnDown->InRect(point))//WA0017
		{																	//WA0017
			if (m_ctrlSpeaker)												//WA0017
				m_ctrlSpeaker.Speak(strNoneBelow);							//WA0017
		}																	//WA0017
		else	// Select a word
		{
			for (int i =0;i<m_TextList.GetSize();i++)
			{
				if (m_TextList[i].rectBounding.PtInRect(point))
				{
					if(m_procWordAidAbbrevKey.IsAbbrevDisplayed())
					{	
						m_procWordAidAbbrevKey.MouseSelectAbbrev(i);
					}
					else
					{
						if (GetKeyState(VK_CAPITAL) == 1)
							m_TextList[i].szText.MakeUpper();

						m_procWordAidAbbrevKey.SelectWord(i);		
					}
				}
			}
			
		}
	}
	else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))	// WORD AID ON ONLY
	{
		if (m_pBtnDown->IsButtonEnabled() && m_pBtnDown->InRect(point))
		{
			TRACE("Down button clicked\n");
			//DisplayList(m_prockKey.OnDownButton());
			m_procWordAidKey.OnButtonDown();
		}
		else if (m_pBtnUp->IsButtonEnabled() && m_pBtnUp->InRect(point))
		{
			TRACE("Up button clicked\n");
			//DisplayList(m_prockKey.OnUpButton());
			m_procWordAidKey.OnButtonUp();
		}
		else if (!m_pBtnDown->IsButtonEnabled()&& m_pBtnDown->InRect(point))	//WA0017
		{																		//WA0017
			if (m_ctrlSpeaker)													//WA0017
				m_ctrlSpeaker.Speak(strNoneBelow);								//WA0017
		}																		//WA0017
		else if (!m_pBtnUp->IsButtonEnabled()&& m_pBtnUp->InRect(point))		//WA0017
		{																		//WA0017
			if (m_ctrlSpeaker)													//WA0017
				m_ctrlSpeaker.Speak(strNoneAbove);								//WA0017
		}																		//WA0017
		else
		{
			for (int i =0;i<m_TextList.GetSize();i++)
			{
				if (m_TextList[i].rectBounding.PtInRect(point))
				{
					TRACE("word selected = %s\n",m_TextList[i].szText);
					if (GetKeyState(VK_CAPITAL) == 1)
						m_TextList[i].szText.MakeUpper();

					m_procWordAidKey.SelectWord(i);
					//OutputString(m_TextList[i].szText,m_procWordAidKey.GetCharsTyped().GetLength());
					//m_procWordAidKey.ClearTyped();
				}
			}
		}
	}
	else if(atoi(m_pSettings->GetSetting(ABBREV_ON)))	// ABBREV ON ONLY
	{
		if (m_pBtnDown->IsButtonEnabled() && m_pBtnDown->InRect(point))
		{
			m_procAbbrevKey.OnButtonDown();
		}
		else if (m_pBtnUp->IsButtonEnabled() && m_pBtnUp->InRect(point))
		{
			m_procAbbrevKey.OnButtonUp();
		}
		else if (!m_pBtnDown->IsButtonEnabled()&& m_pBtnDown->InRect(point))	//WA0017
		{																		//WA0017
			if (m_ctrlSpeaker)													//WA0017
				m_ctrlSpeaker.Speak(strNoneBelow);								//WA0017
		}																		//WA0017
		else if (!m_pBtnUp->IsButtonEnabled()&& m_pBtnUp->InRect(point))		//WA0017
		{																		//WA0017
			if (m_ctrlSpeaker)													//WA0017
				m_ctrlSpeaker.Speak(strNoneAbove);								//WA0017
		}																		//WA0017
		else
		{
			m_procAbbrevKey.MouseSelectAbbrev(m_nHighlighted);
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CChildWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDblClk(nFlags, point);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL CChildWnd::Create(HWND hWnd)
{
	// Create Window
	return CreateEx(WS_EX_TOPMOST,
		(LPCTSTR) GetClassName(),
					_T(""),
					WS_POPUP|WS_BORDER,
					0,0,200,200,
					hWnd,
					NULL);
	
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Get the class name for the registered class for the hotspot.//
//				:															  //
// RETURNS		: the classname for the class being created.				  //
////////////////////////////////////////////////////////////////////////////////

CString CChildWnd::GetClassName()
{
	CString strClassName;

	if (strClassName.IsEmpty())
	{
		CBrush cBrush;
		cBrush.CreateStockObject(GRAY_BRUSH);
		HBRUSH hBrush = (HBRUSH) cBrush.m_hObject;
		strClassName = AfxRegisterWndClass(CS_CLASSDC,
										   LoadCursor(NULL,IDC_ARROW),
										   hBrush,
										   0);
		cBrush.DeleteObject();
	}

	return strClassName;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//!handles mouse messages
BOOL CChildWnd::PreTranslateMessage( MSG* pMsg )
{
	switch(pMsg->message)
	{
		case WM_MOUSEMOVE:
			//if (pMsg->wParam == 0)  //no buttons down, update current active window
			{
				HWND hWnd = ::GetForegroundWindow();
				if (hWnd != NULL  && hWnd != this->m_hWnd)  //ignore self
					m_hActiveWnd = hWnd;
			}
			break;

		case WM_LBUTTONDOWN:
			if (m_hActiveWnd != NULL )  //restore focus
				::SetForegroundWindow(m_hActiveWnd);
			break;
		
		case WM_LBUTTONDBLCLK:
			if (m_hActiveWnd != NULL ) //restore focus
				::SetForegroundWindow(m_hActiveWnd);
			break;

		case WM_RBUTTONDOWN:
			if (m_hActiveWnd != NULL ) //restore focus
				::SetForegroundWindow(m_hActiveWnd);
			break;
		
		default:
			break;
	}
    
    return CWnd::PreTranslateMessage(pMsg);
}
//!disable the keyhook
void CChildWnd::DisableKeyhook()
{
	(lpfnDisable) ();
}
//!enable the keyhook 
void CChildWnd::EnableKeyhook()
{
	(lpfnEnable) (GetSafeHwnd());

	// WA0044 - Only set Hotkeys if the window is being shown
	if(!m_bShow)
	{
		SetMonitorKeys();
	}
	else
	{
		SetMonitorsHiddenWnd();
	}
}
//!check if keyhook dll has been loaded
BOOL CChildWnd::KeyhookLoaded()
{
	return (m_hLibraryInstance) ? TRUE:FALSE; 
}
//!keeps track of where the wordlist window should be displayed on the screen
//!params - forward = TRUE when right arrow pressed, FALSE when left arrow pressed
//!wordlist window is always displayed in top/left position (0) on startup 
int CChildWnd::GetNewWindowPos(BOOL forward)
{
	static int posnum = 0;
	if (forward && posnum < 14)
		posnum = posnum+1;
	else if (forward && posnum == 14)
		posnum = 0;
	if (!forward && posnum>0)
		posnum = posnum-1;
	else if (!forward && posnum == 0)
		posnum = 14;

	return posnum;
}
//!calculates a grid of 15 top/left positions that the wordlist window can be displayed in
//!params - w = width of current wordlist window, h = height of current wordlist window
void CChildWnd::CalculateWndGrid(int w, int h)
{
//	int maxwidth = GetSystemMetrics(SM_CXSCREEN);
//	int maxheight = GetSystemMetrics(SM_CYSCREEN);
	RECT size;
	SystemParametersInfo(SPI_GETWORKAREA,0,&size,0);
	int maxwidth = size.right - size.left;
	int maxheight = size.bottom - size.top;

	m_rectArrayWndPositions[0].x = 50;
	m_rectArrayWndPositions[0].y = 0;
	
	m_rectArrayWndPositions[1].x = maxwidth/4;	
	m_rectArrayWndPositions[1].y = 0 ;
	
	m_rectArrayWndPositions[2].x = (maxwidth/2)-(w/2); 
	m_rectArrayWndPositions[2].y = 0 ;
	
	m_rectArrayWndPositions[3].x = maxwidth - (maxwidth/4) - w;
	if (m_rectArrayWndPositions[3].x > maxwidth - w)
		m_rectArrayWndPositions[3].x -= w/2;
	m_rectArrayWndPositions[3].y = 0 ;
	
	m_rectArrayWndPositions[4].x = maxwidth-w; 
	m_rectArrayWndPositions[4].y = 0;
	
	m_rectArrayWndPositions[5].x = 0; 
	m_rectArrayWndPositions[5].y = (maxheight/2)-(h/2);
	
	m_rectArrayWndPositions[6].x = maxwidth/4; 
	m_rectArrayWndPositions[6].y = (maxheight/2)-(h/2);
	
	m_rectArrayWndPositions[7].x = (maxwidth/2)-(w/2);
	m_rectArrayWndPositions[7].y = (maxheight/2)-(h/2);
	
	m_rectArrayWndPositions[8].x = maxwidth - (maxwidth/4) - w; 
	if (m_rectArrayWndPositions[8].x > maxwidth - w)
		m_rectArrayWndPositions[8].x -= w/2;
	m_rectArrayWndPositions[8].y = (maxheight/2)-(h/2);	
	
	m_rectArrayWndPositions[9].x = (maxwidth)-w;
	m_rectArrayWndPositions[9].y = (maxheight/2)-(h/2);
	
	m_rectArrayWndPositions[10].x = 0; 
	m_rectArrayWndPositions[10].y = maxheight-h;
	
	m_rectArrayWndPositions[11].x = maxwidth/4; 
	m_rectArrayWndPositions[11].y = maxheight-h;
	
	m_rectArrayWndPositions[12].x = (maxwidth/2)-(w/2); 
	m_rectArrayWndPositions[12].y = maxheight-h ;
	
	m_rectArrayWndPositions[13].x = maxwidth - (maxwidth/4) - w; 
	if (m_rectArrayWndPositions[13].x > maxwidth - w)
		m_rectArrayWndPositions[13].x -= w/2;
	m_rectArrayWndPositions[13].y = maxheight-h;	
	
	m_rectArrayWndPositions[14].x = maxwidth-w;
	m_rectArrayWndPositions[14].y = maxheight-h ;
}

void CChildWnd::UpdateFont()
{
	LOGFONT log;
	m_fontList.GetLogFont(&log);
	//have list font settings changed since created?
	if (log.lfFaceName != m_pSettings->GetSetting(LABEL_FONT_NAME) 
		|| log.lfHeight != atoi((const char*)(m_pSettings->GetSetting(LABEL_FONT_SIZE))))
	{
		m_fontList.DeleteObject();
		m_fontList.CreateFont(atoi((const char*)(m_pSettings->GetSetting(LABEL_FONT_SIZE))), 
						0, 0, 0, FW_DONTCARE, FALSE, FALSE, 0, ANSI_CHARSET, 
					  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					  DEFAULT_PITCH, m_pSettings->GetSetting(LABEL_FONT_NAME));
	
	}
	
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Checks to see if sticky keys is on.  Sets the internal flag
//				: to TRUE if it is.
////////////////////////////////////////////////////////////////////////////////
void CChildWnd::CheckStickyKeys()
{
	//check if Sticky keys being used
	STICKYKEYS key;
	
	key.cbSize = sizeof(STICKYKEYS);
	
	SystemParametersInfo(SPI_GETSTICKYKEYS,sizeof(STICKYKEYS),&key,0);
	
	if (key.dwFlags & (SKF_STICKYKEYSON == TRUE)) 
	{
		m_bStickyKeysOn = TRUE;
	}
	else
	{
		m_bStickyKeysOn = FALSE;
	}

}

void CChildWnd::SetMonitorKeys()
{
		(lpfnClearMonitor) ();
	  /* Monitor navigation keys */
		(lpfnMonitor) (atoi(m_pSettings->GetSetting(DWNARR_KEY)),
					   GetKeyTypes(m_pSettings->GetSetting(DWNARR_SPEC)),
					   FALSE);

		(lpfnMonitor) (atoi(m_pSettings->GetSetting(UPARR_KEY)),
					   GetKeyTypes(m_pSettings->GetSetting(UPARR_SPEC)),
					   FALSE);

		(lpfnMonitor) (atoi(m_pSettings->GetSetting(LFTARR_KEY)),
					   GetKeyTypes(m_pSettings->GetSetting(LFTARR_SPEC)),
					   FALSE);

		(lpfnMonitor) (atoi(m_pSettings->GetSetting(RGHTARR_KEY)),
					   GetKeyTypes(m_pSettings->GetSetting(RGHTARR_SPEC)),
					   FALSE);

		(lpfnMonitor) (atoi(m_pSettings->GetSetting(PGUP_KEY)),
					   GetKeyTypes(m_pSettings->GetSetting(PGUP_SPEC)),
					   FALSE);

		(lpfnMonitor) (atoi(m_pSettings->GetSetting(PGDWN_KEY)),
					   GetKeyTypes(m_pSettings->GetSetting(PGDWN_SPEC)),
					   FALSE);

		//monitor selection keys
		(lpfnMonitor) (atoi(m_pSettings->GetSetting(INS_WORD_KEY)),
					   GetKeyTypes(m_pSettings->GetSetting(INS_WORD_SPEC)),
					   FALSE);

		(lpfnMonitor) (atoi(m_pSettings->GetSetting(SAY_KEY)),
					   GetKeyTypes(m_pSettings->GetSetting(SAY_SPEC)),
					   FALSE);

		(lpfnMonitor) (atoi(m_pSettings->GetSetting(ADD_WORD)),
					   GetKeyTypes(m_pSettings->GetSetting(ADD_WORD_SPEC)),
					   FALSE);

		(lpfnMonitor) (atoi(m_pSettings->GetSetting(DEL_WORD_KEY)),
					   GetKeyTypes(m_pSettings->GetSetting(DEL_WORD_SPEC)),
					   FALSE);

		CString strNumKeys = _T("");
		CString strFuncKeys = _T("");
		CString strNumPadKeys = _T("");

		strNumKeys.LoadString(IDS_NUM_KEYS);
		strFuncKeys.LoadString(IDS_FUNC_KEYS);
		strNumPadKeys.LoadString(IDS_NUMPAD_KEYS);

		// Get the combination of special keys required for the select word keys
		int nSpecKeys = GetKeyTypes(m_pSettings->GetSetting(SELECT_WORD_SPEC));

		/* Monitor F1 - F10 */
		if (m_pSettings->GetSetting(SELECT_WORD_KEYS) == strFuncKeys)
		{
			(lpfnMonitor) (VK_F1,nSpecKeys,TRUE); 
			(lpfnMonitor) (VK_F2,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_F3,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_F4,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_F5,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_F6,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_F7,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_F8,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_F9,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_F10,nSpecKeys,FALSE);

			// If no Special Keys then make them work when the SHIFT key is pressed as well
			if(nSpecKeys == 0)
			{
				nSpecKeys = nSpecKeys | KEY_SHIFT;

				(lpfnMonitor) (VK_F1,nSpecKeys,TRUE); 
				(lpfnMonitor) (VK_F2,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_F3,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_F4,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_F5,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_F6,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_F7,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_F8,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_F9,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_F10,nSpecKeys,FALSE);	
			}
		}
		else if (m_pSettings->GetSetting(SELECT_WORD_KEYS) == strNumKeys)
		{

			(lpfnMonitor) (VK_1,nSpecKeys,FALSE); 
			(lpfnMonitor) (VK_2,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_3,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_4,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_5,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_6,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_7,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_8,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_9,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_0,nSpecKeys,FALSE);

			// If no Special Keys then make them work when the SHIFT key is pressed as well
			if(nSpecKeys == 0)
			{
				nSpecKeys = nSpecKeys | KEY_SHIFT;

				(lpfnMonitor) (VK_1,nSpecKeys,FALSE); 
				(lpfnMonitor) (VK_2,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_3,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_4,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_5,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_6,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_7,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_8,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_9,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_0,nSpecKeys,FALSE);
			}
		}
		// Monitor Number Pad keys
		else if (m_pSettings->GetSetting(SELECT_WORD_KEYS) == strNumPadKeys)
		{
			(lpfnMonitor) (VK_NUMPAD0,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_NUMPAD1,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_NUMPAD2,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_NUMPAD3,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_NUMPAD4,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_NUMPAD5,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_NUMPAD6,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_NUMPAD7,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_NUMPAD8,nSpecKeys,FALSE);
			(lpfnMonitor) (VK_NUMPAD9,nSpecKeys,FALSE);

			// If no Special Keys then make them work when the SHIFT key is pressed as well
			if(nSpecKeys == 0)
			{
				nSpecKeys = nSpecKeys | KEY_SHIFT;

				(lpfnMonitor) (VK_NUMPAD0,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_NUMPAD1,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_NUMPAD2,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_NUMPAD3,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_NUMPAD4,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_NUMPAD5,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_NUMPAD6,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_NUMPAD7,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_NUMPAD8,nSpecKeys,FALSE);
				(lpfnMonitor) (VK_NUMPAD9,nSpecKeys,FALSE);
			}
		}

		(lpfnMonitor) (atoi(m_pSettings->GetSetting(SHOW_LIST_HOTKEY)),
						GetKeyTypes(m_pSettings->GetSetting(SHOW_LIST_SPEC)),
						FALSE);	//default show list hotkey
		(lpfnMonitor) (atoi(m_pSettings->GetSetting(UNDO_HOTKEY)),
						GetKeyTypes(m_pSettings->GetSetting(UNDO_SPEC)),
						FALSE);	//default undo hotkey

	// If Abbrev On then set up hotkeys
	if(atoi(m_pSettings->GetSetting(ABBREV_ON)))
	{
		// WA0021////////////////////////////////////////////////////////////////////////////////
		if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
		{
			(lpfnMonitor) (atoi(m_pSettings->GetSetting(ABBREV_BTN_HOTKEY)),
						   GetKeyTypes(m_pSettings->GetSetting(ABBREV_BTN_HOTKEY_SPEC)),
						   FALSE);
		}
		////////////////////////////////////////////////////////////////////////////////////////
		if(m_pSettings->GetSetting(ABBREV_EXPANSION_METHOD) == MANUAL_HOTKEY)
		{
			(lpfnMonitor) (atoi(m_pSettings->GetSetting(ABBREV_MANUAL_HOTKEY)), 
						   GetKeyTypes(m_pSettings->GetSetting(ABBREV_MANUAL_SPEC_KEY)),
						   FALSE);
		}

		// Un-expand Full hotkey
		(lpfnMonitor) (atoi(m_pSettings->GetSetting(ABBREV_FULL_EXPAN_HOTKEY)),
					   GetKeyTypes(m_pSettings->GetSetting(ABBREV_FULL_EXPAN_SPEC_KEY)),
					   FALSE);

		// Un-expand Partial hotkey
		(lpfnMonitor) (atoi(m_pSettings->GetSetting(ABBREV_PART_EXPAN_HOTKEY)),
					   GetKeyTypes(m_pSettings->GetSetting(ABBREV_PART_EXPAN_SPEC_KEY)),
					   FALSE);

		// TODO : Remove so that WordAid and Abbrev message go to WM_USER_KEYMATCHED
		(lpfnSetMessages)(WM_KEYDOWN,WM_KEYUP,WM_USER_KEYMATCHED);
	}
	else
	{
		if((lpfnSetMessages)(WM_KEYDOWN,WM_KEYUP,WM_USER_KEYMATCHED))
		{
			TRACE("setmessage = true\n");
		}
	}
	

}

void CChildWnd::SetMonitorsHiddenWnd()
{
	(lpfnClearMonitor) ();
	(lpfnMonitor) (atoi(m_pSettings->GetSetting(SHOW_LIST_HOTKEY)),
						GetKeyTypes(m_pSettings->GetSetting(SHOW_LIST_SPEC)),
						FALSE);	//default show list hotkey

	(lpfnSetMessages)(WM_KEYDOWN,WM_KEYUP,WM_USER_KEYMATCHED);
}

void CChildWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	SayCurrentSelection();
	/*if (m_ctrlSpeaker)
	{
		CString szNoMatch = _T("");
		szNoMatch.LoadString(IDS_NO_MATCH);

		if (m_TextList[m_nWordlistPos].szText != szNoMatch)
			m_ctrlSpeaker.Speak(m_TextList[m_nWordlistPos].szText);
		else
		{
			m_TextList[m_nWordlistPos].szText.Remove('[');
			m_TextList[m_nWordlistPos].szText.Remove(']');
			m_ctrlSpeaker.Speak(m_TextList[m_nWordlistPos].szText);
		}
	}*/

	CWnd::OnRButtonDown(nFlags, point);
}


void CChildWnd::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == TIMER_OUTPUT)
	{
		KillTimer(TIMER_OUTPUT);
		if (m_ctrlSpeaker)
		{//added 17th July
			if (atoi(m_pSettings->GetSetting(ABBREV_ON)) && 
				atoi(m_pSettings->GetSetting(WORDAID_ON)) &&
				!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
				m_ctrlSpeaker.Speak(m_procWordAidAbbrevKey.m_strText);
			else if (atoi(m_pSettings->GetSetting(WORDAID_ON)))
				m_ctrlSpeaker.Speak(m_procWordAidKey.m_strText);
			else
				m_ctrlSpeaker.Speak(m_procAbbrevKey.m_strText);
		}//added 17th July

		//m_procWordAidAbbrevKey.m_strText.Empty();
	}
	if(nIDEvent == TIMER_SCROLL)
	{
		KillTimer(TIMER_SCROLL);

		if (m_bSpeech)
		{
			if(!m_bWordInserted)
			{
				CString szNoMatch = _T("");
				szNoMatch.LoadString(IDS_NO_MATCH);

				if(m_TextList.GetSize())		// Make sure something to say
				{
					if (m_TextList[m_nWordlistPos].szText != szNoMatch)
					{
						if(m_ctrlSpeaker)
						{
							if(atoi(m_pSettings->GetSetting(ABBREV_ON)))
							{
								// Get string to say
								CString strSay = m_TextList[m_nWordlistPos].szText;
								
								if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
									atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
									!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
								{
									if(m_procWordAidAbbrevKey.IsAbbrevDisplayed())
									{
										m_procWordAidAbbrevKey.AbbrevSayFormat(strSay);
									}
								}
								else 
								{
									m_procAbbrevKey.AbbrevSayFormat(strSay);
								}
								
								// Speak string
								m_ctrlSpeaker.Speak(strSay);
							}
							else
							{
								m_ctrlSpeaker.Speak(m_TextList[m_nWordlistPos].szText);
							}
						}
					}
					else
					{
						m_TextList[m_nWordlistPos].szText.Remove('[');
						m_TextList[m_nWordlistPos].szText.Remove(']');
						if(m_ctrlSpeaker)
						{
							m_ctrlSpeaker.Speak(m_TextList[m_nWordlistPos].szText);
						}
					}
				}
			}
			else
			{
				// Reset Flag
				m_bWordInserted = FALSE;
			}
		}
	}
	// WA0053 - incorrect code
	//else if(nIDEvent = TIMER_ABBREV)
	else if(nIDEvent == TIMER_ABBREV)
	{
		KillTimer(TIMER_ABBREV);
//WA0027 - Stops Timer from going when the expansion method is not on - removed cos it stops backspace 
		// working properly?!!??
//        if(m_pSettings->GetSetting(ABBREV_EXPANSION_METHOD) == AUTOMATIC_EXPANSION &&   // WA0027
		if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
			atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
			!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
		{
			// Set whether the Abbreviation was expanded
			m_procWordAidAbbrevKey.SetExpanded(m_procWordAidAbbrevKey.ExpandAbbrev());
		}
		else if (atoi(m_pSettings->GetSetting(ABBREV_ON)))
		{
			// Set whether the Abbreviation was expanded
			m_procAbbrevKey.SetExpanded(m_procAbbrevKey.ExpandAbbrev());
		}
	}



	CWnd::OnTimer(nIDEvent);
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Displays the abbreviation list in the popup window.
//
// PARAMETERS	: list - Abbreviation list to display.
////////////////////////////////////////////////////////////////////////////////

// WA0047
//void CChildWnd::DisplayAbbrevList(CAbbrevList* list)
void CChildWnd::DisplayAbbrevList(CAbbrevList list)
{
	if(!m_bShow)	// If Popup window being displayed then display Abbrev list
	{
		if(list.GetSize() == 0)
		{
			HideWindow();
		}
		else
		{	
			int pos = 0;

			// Format the expansio list d
			if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
				atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
				!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
			{
				m_procWordAidAbbrevKey.FormatExpansion(list, m_procWordAidAbbrevKey.GetCharsTyped());
				pos = m_nMaxNumberOfItems * m_procWordAidAbbrevKey.GetNoScrolled();

				// Enable disable up button
				if(m_procWordAidAbbrevKey.GetNoScrolled() > 0)
				{
					m_pBtnUp->EnableButton();
				}
				else
				{
					m_pBtnUp->EnableButton(FALSE);
				}
			}
			else if(atoi(m_pSettings->GetSetting(ABBREV_ON)))
			{
				m_procAbbrevKey.FormatExpansion(list, m_procAbbrevKey.GetCharsTyped());
				pos = m_nMaxNumberOfItems * m_procAbbrevKey.GetNoScrolled();

				// Enable disable up button
				if(m_procAbbrevKey.GetNoScrolled() > 0)
				{
					m_pBtnUp->EnableButton();
				}
				else
				{
					m_pBtnUp->EnableButton(FALSE);
				}
			}

			ResetContent();	// Clear List
			
			// Set the up and down buttons enabled or disabled
			if(list.GetSize() <= pos + m_nMaxNumberOfItems)
			{
				m_pBtnDown->EnableButton(FALSE);
			}
			else
			{
				m_pBtnDown->EnableButton();
			}

			// Add String to list
			for (int i = pos;i<list.GetSize() && i<(pos+m_nMaxNumberOfItems);i++)
			{
				AddString(list.GetAt(i), FALSE);
			}

			// make textarea bounding rect size of window
			for (i=0;i<m_TextList.GetSize();++i)
				m_TextList[i].rectBounding.right = m_nLargestWordSize+10;

//			if(m_pDDEServer)
			{
				CString szDDEText = _T("");

				for (int i=0;i<list.GetSize();i++)
				{
					szDDEText += list.GetAt(i);
					szDDEText += (char)ETX;
				}
 				m_pDDEServer.SendDDEMessage(_T("abbrev"), _T("wordlist"), szDDEText);
			}

			ResizeWindow();

			// Resize buttons to fit new window
			m_pBtnDown->SetBoundingRect(CalculateDownButtonBoundingRect());
			m_pBtnUp->SetBoundingRect(CalculateUpButtonBoundingRect());

			
//			if(m_bAbbrevButton)
			if (m_bAbbrevButton || atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
			{
				m_pBtnAbbrev->SetBoundingRect(CalculateAbbrevButtonBoundingRect());
			}

			// Now Display the abbreviation list used so the the WordAid and Abbrev processing
			// class know which list is currently being displayed.
			if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
				atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
				!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
			{
				m_procWordAidAbbrevKey.AbbrevListDisplayed();
			}
		}
	}

	m_bShowingAbbrev = TRUE;		// Showing abbrev list
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Sends a string to the output window (the window with keyboard
//				: focus.
//
// PARAMETERS	: strSend - String to send
//
// RETURNS		: TRUE if the string was sent.
////////////////////////////////////////////////////////////////////////////////

BOOL CChildWnd::SendString(CString strSend)
{
	BOOL bReturn = TRUE;
// this is a desperate fix to ensure alt and ctrl keys work as special keys in 95/98/ME
// added 04/10/2002
	keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_KEYUP, 0);
	keybd_event(VK_MENU, MapVirtualKey(VK_MENU,0), KEYEVENTF_KEYUP, 0);

	if((lpfnOutputString))
	{
		(lpfnOutputString) (strSend);

		// Word just inserted
		m_bWordInserted = TRUE;
	}
	else
	{
		bReturn = FALSE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Sets the timer for the insertion of the Expansion going
//
// RETURNS		: TRUE if successfull
////////////////////////////////////////////////////////////////////////////////

BOOL CChildWnd::SetAbbrevSelectionTimer()
{
	return (BOOL)SetTimer(TIMER_ABBREV, atoi(m_pSettings->GetSetting(ABBREV_AUTOMATIC_DELAY))*1000, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Send the Alpha list to the correct procssing class depending
//				: on whether WordAid and Abbrev are on.
//
// PARAMETERS	: alphalist - alpha list to use in popup window
////////////////////////////////////////////////////////////////////////////////

void CChildWnd::SetWordlistA(CWordlist alphalist)
{
	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		m_procWordAidAbbrevKey.m_AlphaList = alphalist;
	}
	else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
	{
		m_procWordAidKey.m_AlphaList= alphalist;
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Send the Topic list to the correct procssing class depending
//				: on whether WordAid and Abbrev are on.
//
// PARAMETERS	: Topiclist - topic list to use in popup window
////////////////////////////////////////////////////////////////////////////////

void CChildWnd::SetWordlistT(CWordlist topiclist)
{
	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		m_procWordAidAbbrevKey.m_TopicList = topiclist;
		m_procWordAidAbbrevKey.m_TopicList.m_nType = TOPIC;
		m_procWordAidAbbrevKey.m_TopicList.m_bSorted = FALSE;
	}
	else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
	{
		m_procWordAidKey.m_TopicList = topiclist; 
		m_procWordAidKey.m_TopicList.m_nType = TOPIC;
		m_procWordAidKey.m_TopicList.m_bSorted = FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Send the Abbrev list to the correct procssing class depending
//				: on whether WordAid and Abbrev are on.
//
// PARAMETERS	: abbrevList - Abbrev list to use in popup window
////////////////////////////////////////////////////////////////////////////////

//WA0047
//void CChildWnd::SetAbbrevList(CAbbrevList* abbrevList)
void CChildWnd::SetAbbrevList(CAbbrevList abbrevList)
{
	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		m_procWordAidAbbrevKey.SetAbbrevList(abbrevList);
	}
	else
	{
		m_procAbbrevKey.m_AbbrevList = abbrevList;
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Gets the Alpha list from the apportiate process key class.
//				: this depends upon whether wordAid and Abbrev are running
//
// RETURNS		: Alpha wordlist if WordAid is running
////////////////////////////////////////////////////////////////////////////////

CWordlist CChildWnd::GetWordlistA()
{
	CWordlist list;

	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		return m_procWordAidAbbrevKey.m_AlphaList;
	}
	else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
	{
		return m_procWordAidKey.m_AlphaList;
	}

	return list;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Gets the Tpoic list from the apportiate process key class.
//				: this depends upon whether wordAid and Abbrev are running
//
// RETURNS		: Topic wordlist if WordAid is running
////////////////////////////////////////////////////////////////////////////////

CWordlist CChildWnd::GetWordlistT()
{
	CWordlist list;

	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		return m_procWordAidAbbrevKey.m_TopicList;
	}
	else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
	{
		return m_procWordAidKey.m_TopicList;
	}

	return list;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Gets the Abbrev list from the apportiate process key class.
//				: this depends upon whether wordAid and Abbrev are running
//
// RETURNS		: Abbrev wordlist if Abbrev is running
////////////////////////////////////////////////////////////////////////////////

//CAbbrevList* CChildWnd::GetAbbrevList()
CAbbrevList CChildWnd::GetAbbrevList()
{
	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		return m_procWordAidAbbrevKey.m_AbbrevList;
	}
	else
	{
		return m_procAbbrevKey.m_AbbrevList;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

LRESULT  CChildWnd::OnUserKeyMatched(WPARAM wParam, LPARAM lParam)
{
	if(!DefaultPoppupKeys(wParam))
	{
		if(atoi(m_pSettings->GetSetting(WORDAID_ON)) && 
			atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
			!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
		{
			if(!m_procWordAidAbbrevKey.ProcessAbbrevHotkey(wParam))
			{
				m_procWordAidAbbrevKey.ProcessWordAidHotkey(wParam);
			}
		}
		else if(atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
			!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))	// ABBREV ONLY 
		{
			m_procAbbrevKey.ProcessAbbrevHotkey(wParam);
		}
		else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))	// WORDAID ONLY
		{
			m_procWordAidKey.ProcessWordAidHotkey(wParam);
		}
	}

	return 0;
}

// Convert a function key registry setting to it's virtual key
// error return = -1
int CChildWnd::ConvertToFuncKey(CString key)
{
	int val = -1;
	if (key == FUNC1)	val = VK_F1;
	if (key == FUNC2)	val	= VK_F2;
	if (key == FUNC3)	val	= VK_F3;
	if (key == FUNC4)	val	= VK_F4;
	if (key == FUNC5)	val	= VK_F5;
	if (key == FUNC6)	val	= VK_F6;
	if (key == FUNC7)	val	= VK_F7;
	if (key == FUNC8)	val	= VK_F8;
	if (key == FUNC9)	val	= VK_F9;
	if (key == FUNC10)	val	= VK_F10;
	if (key == FUNC11)	val	= VK_F11;
	if (key == FUNC12)	val	= VK_F12;
	
	return val;
}

// convert special key string to virtual key value
// error return = -1
int CChildWnd::ConvertToSpecialKey(CString str)
{
	int val = -1;
	if (str.IsEmpty())		val = 0;	// no special key
	if (str == CTRL_KEY)	val = VK_CONTROL;
	if (str == ALT_KEY)		val = VK_MENU;
	if (str == SHIFT_KEY)	val = VK_SHIFT;
	
	return val;

}

// check whether this special key has been pressed
bool CChildWnd::CheckSpecialKey(int key)
{
	bool val = FALSE;

	if (key == 0)	val = TRUE;
	if (key == VK_CONTROL && m_bCtrlDown)	val = TRUE;
	if (key == VK_MENU && m_bAltDown)		val = TRUE;
	if (key == VK_SHIFT && m_bShiftDown)	val = TRUE;

	return val;
}

void CChildWnd::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_MENU)
		m_bAltDown = TRUE;
}

void CChildWnd::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_MENU)
		m_bAltDown = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Creates which special keys should be included in the monitor
//				: keys function call.
//
// PARAMETERS	: strKeys - The string that contains the special keys to 
//				: include.
//
// RETURNS		: A integer that contains the special keys codes. If 0 then 
//				: the CONTOL and ALT keys are not pressed.
////////////////////////////////////////////////////////////////////////////////

int CChildWnd::GetKeyTypes(CString strKeys)
{
	int nReturn = 0;

	if(strKeys.Find(CTRL_KEY) != -1)
	{
		nReturn = KEY_CTRL;
	}

	if(strKeys.Find(ALT_KEY) != -1)
	{
		nReturn = nReturn | KEY_ALT;
	}

	// Set Shift if not using Control or Alt key.
	if(strKeys.Find(CTRL_KEY) == -1 && strKeys.Find(ALT_KEY) == -1)
	{
		nReturn = 0;
	}

	return nReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Translates a character to its virtual key code.
//
// PARAMETERS	: strChar - Character to translate.
//
// RETURNS		: The virtual key code.
////////////////////////////////////////////////////////////////////////////////

BYTE CChildWnd::CharToVK(CString strChar)
{
	short sKey;// = VkKeyScan((char)strChar.GetAt(0));
	if (!strChar.IsEmpty())
		sKey = VkKeyScanEx((char)strChar.GetAt(0),GetKeyboardLayout(0));


	// Error no key to translate
//	ASSERT(LOBYTE(sKey) == 1 && HIBYTE(sKey) == 1);

	return LOBYTE(sKey);
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Moves the highlighted row down one row in the popup window.
//
// RETURNS		: TRUE if the highlight could be moved.
////////////////////////////////////////////////////////////////////////////////

BOOL CChildWnd::MoveDownList()
{
	BOOL bReturn = FALSE;

	if (m_nWordlistPos < m_TextList.GetSize()-1)
	{
		bReturn = TRUE;
		m_nWordlistPos += 1;

		//WA0032
		//HighlightWord(m_TextList[m_nWordlistPos].rectBounding,AddKeyToString(m_TextList[m_nWordlistPos].szText,m_nWordlistPos));
		HighlightWord(m_nWordlistPos);
		//Unhighlight(m_TextList[m_nHighlighted].rectBounding,AddKeyToString(m_TextList[m_nHighlighted].szText,m_nHighlighted));
		UnHighlight(m_nHighlighted);
		m_nHighlighted = m_nWordlistPos;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Moves the highlighted row up one row in the popup window.
//
// RETURNS		: TRUE if the highlight could be moved.
////////////////////////////////////////////////////////////////////////////////

BOOL CChildWnd::MoveUpList()
{
	BOOL bReturn = FALSE;

	//WA0032 - If ChildWnd currently has nothing highlighted then highlight the last
	//item
	if(m_nWordlistPos == -1)
	{
		m_nWordlistPos = m_TextList.GetSize();
	}

	if (m_nWordlistPos > 0)
	{
		bReturn = TRUE;
		m_nWordlistPos -= 1;

		//WA0032
		//HighlightWord(m_TextList[m_nWordlistPos].rectBounding,AddKeyToString(m_TextList[m_nWordlistPos].szText,m_nWordlistPos));
		HighlightWord(m_nWordlistPos);
		//Unhighlight(m_TextList[m_nHighlighted].rectBounding,AddKeyToString(m_TextList[m_nHighlighted].szText,m_nHighlighted));
		UnHighlight(m_nHighlighted);
		m_nHighlighted = m_nWordlistPos;
	}

	return bReturn;
}

//!Moves the popup window to the left one poisiton.

BOOL CChildWnd::MovePopupLeft()
{
	m_nWindowPos = GetNewWindowPos(FALSE);
	ResizeWindow();

	// WA0051 - Make sure the cursor is not on the window so that the selection does not change
	MoveCursorOffPopup();

	return TRUE;
}


//! Moves the popup window to the rihght one poisiton.

BOOL CChildWnd::MovePopupRight()
{
	m_nWindowPos = GetNewWindowPos(TRUE);
	ResizeWindow();

	// WA0051 - Make sure the cursor is not on the window so that the selection does not change
	MoveCursorOffPopup();

	return TRUE;
}

//! Moves the cursor off the popup window if the cursor is currently on it.  To do this it moves the cursor
//! 5 pixels to the left of the window.
// WA0051
void CChildWnd::MoveCursorOffPopup()
{
	CRect rc;
	CPoint pt;

	GetWindowRect(rc);

	::GetCursorPos(&pt);

	if(rc.PtInRect(pt))
	{
		::SetCursorPos(rc.left - 5, pt.y);
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Handles to default hotkeys that move up and down the popup
//				: window and the keys that move the popup window.
//
// PARAMETERS	: nChar - Virtual key code pressed
//
// RETURNS		: TRUE if it is one of the default hotkeys
////////////////////////////////////////////////////////////////////////////////

BOOL CChildWnd::DefaultPoppupKeys(UINT nChar)
{
	BOOL bReturn = FALSE;

	// handle up & down arrows to move thru word list if list window showing
	if (nChar == (UINT)atoi(m_pSettings->GetSetting(UPARR_KEY)) && !m_bShow)
	{
		bReturn = TRUE;
		MoveUpList();
	}
	else if (nChar == (UINT)atoi(m_pSettings->GetSetting(DWNARR_KEY)) && !m_bShow)
	{//! m_TextList is a 1 based index not 0
		bReturn = TRUE;
		MoveDownList();
	}
	else if (nChar == (UINT)atoi(m_pSettings->GetSetting(LFTARR_KEY)) && !m_bShow) // handle move window keys - left & right arrows - if list window showing
	{
		bReturn = TRUE;
		MovePopupLeft();
	}
	else if (nChar == (UINT)atoi(m_pSettings->GetSetting(RGHTARR_KEY)) && !m_bShow)
	{
		bReturn = TRUE;
		MovePopupRight();
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Speaks the currently selected word in the popup window.
////////////////////////////////////////////////////////////////////////////////

void CChildWnd::SayCurrentSelection()
{
	if (m_ctrlSpeaker)
	{
		CString szNoMatch = _T("");
		szNoMatch.LoadString(IDS_NO_MATCH);

		if (m_TextList[m_nWordlistPos].szText != szNoMatch)
		{
			if(atoi(m_pSettings->GetSetting(ABBREV_ON)))
			{
				// Remove <CR> from string
				CString strSay = m_TextList[m_nWordlistPos].szText;
				strSay.Replace(XML_CARRIAGE_RETURN, _T(" "));

				m_procAbbrevKey.AbbrevSayFormat(strSay);
				// Speak string
				m_ctrlSpeaker.Speak(strSay);
			}
			else
			{
				m_ctrlSpeaker.Speak(m_TextList[m_nWordlistPos].szText);
			}
		}
		else
		{
			m_TextList[m_nWordlistPos].szText.Remove('[');
			m_TextList[m_nWordlistPos].szText.Remove(']');
			m_ctrlSpeaker.Speak(m_TextList[m_nWordlistPos].szText);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called by a DDE client to open an alphabetic file.
//
// PARAMETERS	: strFilename - file to open
////////////////////////////////////////////////////////////////////////////////

BOOL CChildWnd::OpenAlpha(CString strFilename)
{
	BOOL bReturn = TRUE;

	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		m_procWordAidAbbrevKey.m_AlphaList.LoadWordlist(strFilename, ALPHA);

		// Set this file as last opened in main window
		((CWordAid2App*)AfxGetApp())->m_pWordAid->SetAlphaFileName(strFilename);

		// Display the new list
		DisplayList(m_procWordAidAbbrevKey.m_AlphaList.Search(m_procWordAidAbbrevKey.GetCharsTyped()));
	}
	else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
	{
		m_procWordAidKey.m_AlphaList.LoadWordlist(strFilename, ALPHA);

		// Set this file as last opened in main window
		((CWordAid2App*)AfxGetApp())->m_pWordAid->SetAlphaFileName(strFilename);

		// Display the new list
		DisplayList(m_procWordAidKey.m_AlphaList.Search(m_procWordAidAbbrevKey.GetCharsTyped()));
	}
	else
	{
		bReturn = FALSE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called by a DDE client to open an Topic file.
//
// PARAMETERS	: strFilename - file to open
////////////////////////////////////////////////////////////////////////////////

BOOL CChildWnd::OpenTopic(CString strFilename)
{
	BOOL bReturn = TRUE;

	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		m_procWordAidAbbrevKey.m_TopicList.LoadWordlist(strFilename, TOPIC);

		// Set this file as last opened in main window
		((CWordAid2App*)AfxGetApp())->m_pWordAid->SetTopicFileName(strFilename);

		// Display new list
		if(m_procWordAidAbbrevKey.GetCharsTyped().IsEmpty())
		{
			DisplayList(m_procWordAidAbbrevKey.m_TopicList);
		}
	}
	else if(atoi(m_pSettings->GetSetting(WORDAID_ON)))
	{
		m_procWordAidKey.m_TopicList.LoadWordlist(strFilename, TOPIC);

		// Set this file as last opened in main window
		((CWordAid2App*)AfxGetApp())->m_pWordAid->SetTopicFileName(strFilename);

		// Display new list
		if(m_procWordAidAbbrevKey.GetCharsTyped().IsEmpty())
		{
			DisplayList(m_procWordAidAbbrevKey.m_TopicList);
		}
	}
	else
	{
		bReturn = FALSE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called by a DDE client to open an Abbrev file.
//
// PARAMETERS	: strFilename - file to open
////////////////////////////////////////////////////////////////////////////////

BOOL CChildWnd::OpenAbbrev(CString strFilename)
{
	BOOL bReturn = TRUE;

	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		m_procWordAidAbbrevKey.m_AbbrevList.LoadWordlist(strFilename);
		
		// Set this file as last opened in main window
		((CWordAid2App*)AfxGetApp())->m_pWordAid->SetAbbrevFileName(strFilename);

		CAbbrevList abbrevList;

		if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
		{
//			if(m_procWordAidAbbrevKey.m_AbbrevList)		WA0047
			{
				abbrevList = m_procWordAidAbbrevKey.m_AbbrevList.Search(m_procWordAidAbbrevKey.GetCharsTyped());
			}
		}

		// Display whole Abbrev list if word bank is on
		if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) && 
			abbrevList.GetSize() == 0)
		{
//			if(m_procWordAidAbbrevKey.m_AbbrevList)		WA0047
			{
				if(m_procWordAidAbbrevKey.IsAbbrevDisplayed())
				{
					m_procWordAidAbbrevKey.m_lstDisplay = m_procWordAidAbbrevKey.m_AbbrevList.GetAll();
					DisplayAbbrevList(m_procWordAidAbbrevKey.m_AbbrevList);
					m_procWordAidAbbrevKey.AbbrevListDisplayed();
				}
				else
				{
					m_procWordAidAbbrevKey.m_AbbrevStoreList = m_procWordAidAbbrevKey.m_AbbrevList.GetAll();
				}
			}
		}
		else
		{
			if(m_procWordAidAbbrevKey.IsAbbrevDisplayed())
			{
				DisplayAbbrevList(abbrevList);
				m_procWordAidAbbrevKey.m_lstDisplay = abbrevList;
				m_procWordAidAbbrevKey.AbbrevListDisplayed();
			}
			else
			{
				m_procWordAidAbbrevKey.m_AbbrevStoreList = abbrevList;
			}
		}
	}
	else if(atoi(m_pSettings->GetSetting(ABBREV_ON)))
	{
		m_procAbbrevKey.m_AbbrevList.LoadWordlist(strFilename);

		// Set this file as last opened in main window
		((CWordAid2App*)AfxGetApp())->m_pWordAid->SetAbbrevFileName(strFilename);

		CAbbrevList abbrevList;

		if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)))
		{
//			if(m_procAbbrevKey.m_AbbrevList)		WA0047
			{
				abbrevList = m_procAbbrevKey.m_AbbrevList.Search(m_procAbbrevKey.GetCharsTyped());
			}
		}

		// Display whole Abbrev list if word bank is on
		if(atoi(m_pSettings->GetSetting(ABBREV_WORD_BANK)) && 
			abbrevList.GetSize() == 0)
		{
//			if(m_procAbbrevKey.m_AbbrevList)	WA0047
			{
				m_procAbbrevKey.m_lstDisplay = m_procAbbrevKey.m_AbbrevList.GetAll();
				DisplayAbbrevList(m_procAbbrevKey.m_AbbrevList);
				m_procAbbrevKey.AbbrevListDisplayed();
			}
		}
		else
		{
			DisplayAbbrevList(abbrevList);
			m_procAbbrevKey.m_lstDisplay = abbrevList;
			m_procAbbrevKey.AbbrevListDisplayed();
		}
	}
	else
	{
		bReturn = FALSE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Performs a Un-expand (full)
////////////////////////////////////////////////////////////////////////////////

BOOL CChildWnd::UndoFull()
{
	BOOL bReturn = TRUE;

	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		bReturn = m_procWordAidAbbrevKey.UnExpandFull();
	}
	else if(atoi(m_pSettings->GetSetting(ABBREV_ON)))
	{
		bReturn = m_procAbbrevKey.UnExpandFull();
	}
	else
	{
		bReturn = FALSE;
	}

	return bReturn;

}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Performs an Un-expand (part)
////////////////////////////////////////////////////////////////////////////////

BOOL CChildWnd::UndoPart()
{
	BOOL bReturn = TRUE;

	if(atoi(m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		bReturn = m_procWordAidAbbrevKey.UnExpandPartial();
	}
	else if(atoi(m_pSettings->GetSetting(ABBREV_ON)))
	{
		bReturn = m_procAbbrevKey.UnExpandPartial();
	}
	else
	{
		bReturn = FALSE;
	}

	return bReturn;
}

void CChildWnd::DeleteOutput(CString str)//added 17th July
{
	(lpfnOutputString) (str);
}

void CChildWnd::CreateButtons()
{
	// create up button
	CString szUp = _T("");;
	szUp.LoadString(IDS_UP_WORDLIST);

	CRect upRect;
	upRect = CalculateUpButtonBoundingRect();

	// Button Bitmaps then create bitmap buttons
	if(atoi(m_pSettings->GetSetting(BUTTON_BITMAPS_ON)))
	{
		// WA0031 - Make sure memory is cleaned up
		if(m_pBtnUp)
		{
			delete m_pBtnUp;
		}

		m_pBtnUp = new CUpButton(upRect, szUp,m_pSettings->GetColourSetting(BUTTON_BACKGROUND),
						m_pSettings->GetColourSetting(BUTTON_FOREGROUND),
						atoi((const char*)(m_pSettings->GetSetting(BUTTON_FONT_SIZE))),
						m_pSettings->GetSetting(BUTTON_FONT_NAME), IDB_BMP_ARROW_UP);
	}
	else
	{
		// WA0031 - Make sure memory is cleaned up
		if(m_pBtnUp)
		{
			delete m_pBtnUp;
		}

		m_pBtnUp = new CWAButton(upRect, szUp,m_pSettings->GetColourSetting(BUTTON_BACKGROUND),
						m_pSettings->GetColourSetting(BUTTON_FOREGROUND),
						atoi((const char*)(m_pSettings->GetSetting(BUTTON_FONT_SIZE))),
						m_pSettings->GetSetting(BUTTON_FONT_NAME));
	}

	// Create Down Button
	CRect downRect;
	CString szDown = _T("");

	downRect = CalculateDownButtonBoundingRect();
	
	szDown.LoadString(IDS_DOWN_WORDLIST);

	// Button Bitmaps then create bitmap buttons
	if(atoi(m_pSettings->GetSetting(BUTTON_BITMAPS_ON)))
	{
		// WA0031 - Make sure memory is cleaned up
		if(m_pBtnDown)
		{
			delete m_pBtnDown;
		}

		m_pBtnDown = new CDownButton(downRect, szDown,m_pSettings->GetColourSetting(BUTTON_BACKGROUND),
						m_pSettings->GetColourSetting(BUTTON_FOREGROUND),
						atoi((const char*)(m_pSettings->GetSetting(BUTTON_FONT_SIZE))),
						m_pSettings->GetSetting(BUTTON_FONT_NAME), IDB_BMP_ARROW_DOWN);
	}
	else
	{
		// WA0031 - Make sure memory is cleaned up
		if(m_pBtnDown)
		{
			delete m_pBtnDown;
		}

		m_pBtnDown = new CWAButton(downRect, szDown,m_pSettings->GetColourSetting(BUTTON_BACKGROUND),
						m_pSettings->GetColourSetting(BUTTON_FOREGROUND),
						atoi((const char*)(m_pSettings->GetSetting(BUTTON_FONT_SIZE))),
						m_pSettings->GetSetting(BUTTON_FONT_NAME), IDB_BMP_ARROW_DOWN);
	}
	//WA0017 Create Stop Button
	CRect stopRect;
	CString szStop = _T("");

	stopRect = CalculateDownButtonBoundingRect();
	
	szStop.LoadString(IDS_STOP_WORDLIST);

	// Button Bitmaps then create bitmap buttons
	if(atoi(m_pSettings->GetSetting(BUTTON_BITMAPS_ON)))
	{
		//WA0031 - Stop from creating more than one without being cleanup
		if(m_pBtnStop)
		{
			delete m_pBtnStop;
		}	//WA0031

		m_pBtnStop = new CStopButton(stopRect, szStop,m_pSettings->GetColourSetting(BUTTON_BACKGROUND),
						m_pSettings->GetColourSetting(BUTTON_FOREGROUND),
						atoi((const char*)(m_pSettings->GetSetting(BUTTON_FONT_SIZE))),
						m_pSettings->GetSetting(BUTTON_FONT_NAME));//, IDB_BMP_STOP);
	}
	else
	{
		//WA0031 - Stop from creating more than one without being cleanup
		if(m_pBtnStop)
		{
			delete m_pBtnStop;
		}   //WA0031

		m_pBtnStop = new CWAButton(stopRect, szStop,m_pSettings->GetColourSetting(BUTTON_BACKGROUND),
						m_pSettings->GetColourSetting(BUTTON_FOREGROUND),
						atoi((const char*)(m_pSettings->GetSetting(BUTTON_FONT_SIZE))),
						m_pSettings->GetSetting(BUTTON_FONT_NAME));//, IDB_BMP_STOP);
	}
}

//! Called to make sure the Abbrev button is displaying the correct text.  This method should 
//! only be called if the Abbrev and WordAid is turned on

//WA0033
void CChildWnd::SetAbbrevButtonText()
{
	// Only use if WordAid and Abbrev are on
	ASSERT(atoi(m_pSettings->GetSetting(WORDAID_ON)) && atoi(m_pSettings->GetSetting(ABBREV_ON)));

	CString strAbbrev = _T("");

	if(m_procWordAidAbbrevKey.IsAbbrevDisplayed())
	{
		if(	m_procWordAidAbbrevKey.GetAlphaOrTopic() == ALPHA)
		{
			strAbbrev.LoadString(IDS_ABBREV_BTN_ALPHA);
			m_pBtnAbbrev->SetText(strAbbrev);
		}
		else
		{
			strAbbrev.LoadString(IDS_ABBREV_BTN_TOPIC);
			m_pBtnAbbrev->SetText(strAbbrev);
		}
	}
	else
	{
		strAbbrev.LoadString(IDS_ABBREV_BUTTON);

		m_pBtnAbbrev->SetText(strAbbrev);
	}
}

//! Called to reset the DDE string to blank

void CChildWnd::ResetDDEString()
{
	m_pDDEServer.SendDDEMessage(_T("alpha"), _T("wordlist"), _T(""));
}