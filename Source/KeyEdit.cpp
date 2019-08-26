// KeyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "KeyEdit.h"

#include "VKConversion.h"

////////////////////////////////////////////////////////////////////////////////

const CString SHIFT_TXT	= _T("Shift");
const CString CTRL_TXT	= _T("Ctrl");
const CString ALT_TXT	= _T("Alt");
const CString PLUS_TXT	= _T("+");

const CString F1		= _T("F1");
const CString F2		= _T("F2");
const CString F3		= _T("F3");
const CString F4		= _T("F4");
const CString F5		= _T("F5");
const CString F6		= _T("F6");
const CString F7		= _T("F7");
const CString F8		= _T("F8");
const CString F9		= _T("F9");
const CString F10		= _T("F10");
const CString F11		= _T("F11");
const CString F12		= _T("F12");
const CString HOME		= _T("Home");
const CString END		= _T("End");
const CString PAGEUP	= _T("Page Up");
const CString PAGEDOWN	= _T("Page Down");
const CString INSERT	= _T("Ins");
const CString DELETE_KEY= _T("Del");
const CString ARR_LEFT	= _T("Left Arrow");
const CString ARR_RIGHT	= _T("Right Arrow");
const CString ARR_UP	= _T("Up Arrow");
const CString ARR_DOWN	= _T("Down Arrow");

////////////////////////////////////////////////////////////////////////////////

// CKeyEdit

IMPLEMENT_DYNAMIC(CKeyEdit, CEdit)

////////////////////////////////////////////////////////////////////////////////

CKeyEdit::CKeyEdit()
{
//	EnableActiveAccessibility();

	m_szKeyPressed  = _T("");
	m_szKey			= _T("");
	m_bAltDown		= FALSE;
	m_bAltPressed	= FALSE;
	m_bCtrlPressed	= FALSE;
	m_bShiftPressed	= FALSE;
	m_szCharPressed	= _T("");
	m_nVKPressed		= 0;
}

////////////////////////////////////////////////////////////////////////////////

CKeyEdit::~CKeyEdit()
{
}

////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CKeyEdit, CEdit)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////

void CKeyEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
//	WCHAR buffer[10];
	BYTE byte[256];

	// Initialise Keyboard state array
	for(int i =0; i < 256; i++)
	{
		byte[i] = 0;		
	}

	// Convert the Virtual key to the lower case character.
//	ToUnicodeEx(nChar, 0, byte, (LPWSTR)buffer, 10, 0, 0);
	CString buffer = MapVirtualKeyEx(nChar,2,GetKeyboardLayout(0));
//	if (GetKeyState(VK_SHIFT))
//	{
//		for (int i=0;i<sizeof(buffer);i++)
//			buffer.MakeUpper();
//	}

	if(nChar == VK_MENU)	// If Alt key down set Alt Down flag
	{
		m_bAltDown = TRUE;
	}

	// If a function a key pressed then get the text for it then force to display it
	if(nChar == VK_F1 || nChar == VK_F2 || nChar == VK_F3 || nChar == VK_F4 || nChar == VK_F5 || 
		nChar == VK_F6 || nChar == VK_F7 || nChar == VK_F8 || nChar == VK_F9 || 
		nChar == VK_F10 || nChar == VK_F11 || nChar == VK_F12 || nChar == VK_HOME || nChar == VK_END ||
		nChar == VK_PRIOR || nChar == VK_NEXT || nChar == VK_INSERT || nChar == VK_DELETE ||
		nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_UP || nChar == VK_DOWN)
	{
		m_szKey = GetOtherKey(nChar);	// Get function key text
		m_nVKPressed = nChar;
		OnChar(nChar, nRepCnt, nFlags);		// Display in edit box
		return;
	}

	// If the Alt key has been pressed then store character and display
	if(m_bAltDown && nChar != VK_CONTROL && nChar != VK_MENU && nChar != VK_SHIFT)
	{
		m_szKey = buffer;					// Store character
		m_szKey.MakeUpper();				// Make the character keys upper case when displaying
		m_nVKPressed = nChar;
		OnChar(nChar, nRepCnt, nFlags);		// Display in edit box
		return;
	}

	short sShiftState;
	short sCtrlState;

	sShiftState = GetKeyState(VK_SHIFT);	// Get Shift key state
	sCtrlState = GetKeyState(VK_CONTROL);	// GEt Ctrl key state

	// If the Shift or Ctrl key is pressed then store and display short-cut
	if(HIWORD(sShiftState) >0 || HIWORD(sCtrlState) > 0)
	{
		m_szKey = buffer;					// Store Character
		m_szKey.MakeUpper();				// Make the character keys upper case when displaying
		m_nVKPressed = nChar;
		OnChar(nChar, nRepCnt, nFlags);		// Display in edit box
		return;
	}

	// If any character is pressed then store the key
	if(nChar != VK_SHIFT && nChar != VK_CONTROL && nChar != VK_MENU)
	{	
		m_szKey = buffer;	
		m_szKey.MakeUpper();				// Make the character keys upper case when displaying
		m_nVKPressed = nChar;
	}
}

////////////////////////////////////////////////////////////////////////////////
// CKeyEdit message handlers
void CKeyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	short sShiftState = 0;
	short sCtrlState = 0;

	if((nChar != VK_SHIFT && nChar != VK_CONTROL && nChar != VK_MENU) || (m_bAltDown))
	{
		sShiftState = GetKeyState(VK_SHIFT);		// Get Shift key state
		sCtrlState = GetKeyState(VK_CONTROL);		// Get Ctrl key state

		if(HIWORD(sShiftState) > 0 && m_bSHIFTOn)					// Is the shift key pressed
		{
			m_szKeyPressed = SHIFT_TXT + PLUS_TXT;	// Add shift text
			m_bShiftPressed = TRUE;					// Set Shift Pressed
		}
		else
		{
			m_bShiftPressed = FALSE;				// Set Shift not pressed
		}

		if(HIWORD(sCtrlState) > 0 && m_bCONTROLOn)					// Is the Ctrl key pressed
		{
			m_szKeyPressed += CTRL_TXT + PLUS_TXT;	// Add Ctrl text
			m_bCtrlPressed = TRUE;					// Ctrl Pressed
		}
		else
		{
			m_bCtrlPressed = FALSE;					// Ctrl not pressed
		}

		if(m_bAltDown  && m_bALTOn)								// Is the Alt key pressed
		{
			m_szKeyPressed += ALT_TXT + PLUS_TXT;	// Add Alt to text
			m_bAltPressed = TRUE;					// Alt Pressed
		}
		else
		{
			m_bAltPressed = FALSE;					// Alt not pressed
		}

		m_szCharPressed = m_szKey;					// Character that has been pressed
		
		m_szKeyPressed += m_szKey;					// Add the key pressed
		SetWindowText(m_szKeyPressed);				// Display the short cut text
		m_szKeyPressed = _T("");					// Clear text pressed
	}
	
}

////////////////////////////////////////////////////////////////////////////////

void CKeyEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// If Alt key being release then Set Alt Flag to FALSE
	if(nChar == VK_MENU)
	{
		m_bAltDown = FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////

void CKeyEdit::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// If Alt key then set flag
	if(nChar == VK_MENU)
	{
		m_bAltDown = TRUE;
	}
	else if(nChar != VK_SHIFT)
	{
		// Function Keys are sent here if ALT is pressed and they need to be tranlated into text
		if(nChar == VK_F1 || nChar == VK_F2 || nChar == VK_F3 || nChar == VK_F4 || nChar == VK_F5 || 
			nChar == VK_F6 || nChar == VK_F7 || nChar == VK_F8 || nChar == VK_F9 || 
			nChar == VK_F10 || nChar == VK_F11 || nChar == VK_F12 || nChar == VK_HOME || nChar == VK_END ||
			nChar == VK_PRIOR || nChar == VK_NEXT || nChar == VK_INSERT || nChar == VK_DELETE ||
			nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_UP || nChar == VK_DOWN)
		{
			m_szKey = GetOtherKey(nChar);
			m_nVKPressed = nChar;
		}
		else
		{
		//	WCHAR buffer[10];
			BYTE byte[256];
			for(int i = 0; i < 256; i++)
			{
				byte[i] = 0;
			}

			// Get character from virtual key
//			ToUnicodeEx(nChar, 0, byte, (LPWSTR)buffer, 10, 0, 0);
	CString buffer = MapVirtualKeyEx(nChar,2,GetKeyboardLayout(0));
	if (GetKeyState(VK_SHIFT))
	{
//		for (int i=0;i<sizeof(buffer);i++)
			buffer.MakeUpper();
	}
			m_szKey = buffer;
			m_szKey.MakeUpper();				// Make the character keys upper case when displaying
			m_nVKPressed = nChar;
		}
		
		OnChar(nChar, nRepCnt, nFlags);		// Display in edit box
	}
}

////////////////////////////////////////////////////////////////////////////////

void CKeyEdit::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Alt up set flag to FALSE
	if(nChar == VK_MENU)
	{
		m_bAltDown = FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Gets the text that relates to the other keys that do not 
//				: have a character representation.
////////////////////////////////////////////////////////////////////////////////

CString CKeyEdit::GetOtherKey(UINT nChar)
{
	switch(nChar)
	{
	case VK_F1:
		return F1;
	case VK_F2:
		return F2;
	case VK_F3:
		return F3;
	case VK_F4:
		return F4;
	case VK_F5:
		return F5;
	case VK_F6:
		return F6;
	case VK_F7:
		return F7;
	case VK_F8:
		return F8;
	case VK_F9:
		return F9;
	case VK_F10:
		return F10;
	case VK_F11:
		return F11;
	case VK_F12:
		return F12;
	case VK_PRIOR:
		return PAGEUP;
	case VK_NEXT:
		return PAGEDOWN;
	case VK_END:
		return END;
	case VK_HOME:
		return HOME;
	case VK_LEFT:
		return ARR_LEFT;
	case VK_RIGHT:
		return ARR_RIGHT;
	case VK_UP:
		return ARR_UP;
	case VK_DOWN:
		return ARR_DOWN;
	case VK_INSERT:
		return INSERT;
	case VK_DELETE:
		return DELETE_KEY;
	}
	
	return _T("");
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Displays the shortcut in the edit windoiw
////////////////////////////////////////////////////////////////////////////////

void CKeyEdit::DisplayShortCut()
{
	if(m_bShiftPressed && m_bSHIFTOn)							// Is the shift key pressed
	{
		m_szKeyPressed = SHIFT_TXT + PLUS_TXT;	// Add shift text
	}

	if(m_bCtrlPressed && m_bCONTROLOn)							// Is the Ctrl key pressed
	{
		m_szKeyPressed += CTRL_TXT + PLUS_TXT;	// Add Ctrl text
	}

	if(m_bAltPressed && m_bALTOn)							// Is the Alt key pressed
	{
		m_szKeyPressed += ALT_TXT + PLUS_TXT;	// Add Alt to text
	}

	m_szKeyPressed += m_szCharPressed;			// Character that has been pressed
	SetWindowText(m_szKeyPressed);				// Display the short cut text
	m_szKeyPressed = _T("");					// Clear text pressed
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Gets a string that says which Special keys were set for the
//				: short cut combination in the Edit box.
//
// RETURNS		: A string formated with 'a' if ALT key pressed, 'c' if CONTROL
//				: key pressed and 's' if SHIFT key pressed.
////////////////////////////////////////////////////////////////////////////////

CString CKeyEdit::GetSpecialKeysPressed()
{
	CString szSpecKeys = _T("");

	if(m_bAltPressed)
	{
		szSpecKeys = ALT_DOWN;
	}

	if(m_bCtrlPressed)
	{
		szSpecKeys += CTRL_DOWN;
	}

	if(m_bShiftPressed)
	{
		szSpecKeys += SHIFT_DOWN;
	}

	return szSpecKeys;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Sets the Special keys that are pressed.
//
// PARAMETERS	: szSpecKey - String containg 'a' to set ALT key pressed. 'c' 
//				: to set CONTROL key pressed and 's' to set SHIFT key pressed.
//
// RETURNS		: TRUE if the special keys were set; otherwise FALSE if the
//				: string sent to the function was invalid.
////////////////////////////////////////////////////////////////////////////////

BOOL CKeyEdit::SetSpecialKeysPressed(CString szSpecKey)
{
	BOOL bReturn = TRUE;
	
	// Make sure string is lower case
	szSpecKey.MakeLower();

	// Cehck to make sure valid string
	if(szSpecKey.GetLength() <= 3)
	{
		for(int i = 0; i < szSpecKey.GetLength() || !bReturn; i++)
		{
			if(szSpecKey.GetAt(i) == ALT_DOWN)
			{
				m_bAltPressed = TRUE;
			}
			else if(szSpecKey.GetAt(i) == CTRL_DOWN)
			{
				m_bCtrlPressed = TRUE;
			}
			else if(szSpecKey.GetAt(i) == SHIFT_DOWN)
			{
				m_bShiftPressed = TRUE;
			}
			else
			{
				bReturn = FALSE;
			}
		}
	}
	else	// Too long
	{
		bReturn = FALSE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////

void CKeyEdit::ClearEditBox()
{
	m_bAltPressed	= FALSE;
	m_bCtrlPressed	= FALSE;
	m_bShiftPressed = FALSE;
	m_nVKPressed	= 0;
	m_szCharPressed = _T("");

//	SetWindowText(_T(""));
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Set the virtual key pressed and finds the character that 
//				: the virtual key represents.
//
// PARAMETERS	: nVK - Virtual key code to set
////////////////////////////////////////////////////////////////////////////////
void CKeyEdit::SetVKPressed(UINT nVK)
{
	m_nVKPressed = nVK; 
	
	// Check to see if the virtual key is a non character key
	m_szCharPressed = GetOtherKey(nVK);

	// If character key then get it
	if(m_szCharPressed.IsEmpty())
	{
		m_szCharPressed = CVKConversion::VKToChar(m_nVKPressed);	
		m_szCharPressed.MakeUpper();
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Sets the character and the virtual key code value that is 
//				: to be displayed.
//
// PARAMETERS	: szChar - the character to set
////////////////////////////////////////////////////////////////////////////////

void CKeyEdit::SetCHARPressed(CString szCHAR)
{
	m_szCharPressed = szCHAR;

	// Set Virtual key from character
	m_nVKPressed = CVKConversion::StringToVK(szCHAR);
}