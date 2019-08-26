#include "stdafx.h"
#include "vkconversion.h"

const CString ZERO_LOWER	= _T("0");
const CString ZERO_UPPER	= _T(")");
const CString ONE_LOWER		= _T("1");
const CString ONE_UPPER		= _T("!");
const CString TWO_LOWER		= _T("2");
const CString TWO_UPPER		= _T("""");
const CString THREE_LOWER	= _T("3");
const CString THREE_UPPER	= _T("£");
const CString FOUR_LOWER	= _T("4");
const CString FOUR_UPPER	= _T("$");
const CString FIVE_LOWER	= _T("5");
const CString FIVE_UPPER	= _T("%");
const CString SIX_LOWER		= _T("6");
const CString SIX_UPPER		= _T("^");
const CString SEVEN_LOWER	= _T("7");
const CString SEVEN_UPPER	= _T("&");
const CString EIGHT_LOWER	= _T("8");
const CString EIGHT_UPPER	= _T("*");
const CString NINE_LOWER	= _T("9");
const CString NINE_UPPER	= _T("(");
const CString MINUS_LOWER	= _T("-");
const CString MINUS_UPPER	= _T("_");
const CString EQUALS_LOWER	= _T("=");
const CString EQUALS_UPPER	= _T("+");
const CString SQUARE_BRACKETO_LOWER = _T("[");
const CString SQUARE_BRACKETO_UPPER = _T("{");
const CString SQUARE_BRACKETC_LOWER = _T("]");
const CString SQUARE_BRACKETC_UPPER = _T("}");
const CString HASH_LOWER	= _T("#");
const CString HASH_UPPER	= _T("~");
const CString SEMI_COLON_LOWER	= _T(";");
const CString SEMI_COLON_UPPER	= _T(":");
const CString QUOTE_LOWER	= _T("'");
const CString QUOTE_UPPER	= _T("@");
const CString COMMA_LOWER	= _T(",");
const CString COMMA_UPPER	= _T("<");
const CString FULL_STOP_LOWER	= _T(".");
const CString FULL_STOP_UPPER	= _T(">");
const CString BACKSLASH_LOWER	= _T("/");
const CString BACKSLASH_UPPER	= _T("?");
const CString FORWARDSLASH_LOWER	= _T("\\");
const CString FORWARDSLASH_UPPER	= _T("|");

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

/*CVKConversion::CVKConversion(void)
{
}

CVKConversion::~CVKConversion(void)
{
}*/

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Translates a character to its virtual key code.
//
// PARAMETERS	: strChar - Character to translate.
//
// RETURNS		: The virtual key code.
////////////////////////////////////////////////////////////////////////////////

BYTE CVKConversion::CharToVK(char strChar)
{
//	short sKey = VkKeyScan((char)strChar.GetAt(0));
	short sKey ;
	
	sKey = VkKeyScanEx(strChar,GetKeyboardLayout(0));

	// Error no key to translate
//	ASSERT(LOBYTE(sKey) == 1 && HIBYTE(sKey) == 1);

	return LOBYTE(sKey);
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: This function takes a virtual key and then finds out which
//				: character key was pressed.  Looking at the Keystate to know
//				: whether it is Upper case or lower case.
//
// PARAMETERS	: nVK - Virtual key to transfer to character.
//
// RETURNS		: Character that virtual keys represents.
////////////////////////////////////////////////////////////////////////////////

CString CVKConversion::VKToChar(UINT nVK)
{
	//WCHAR buffer[10];
	BYTE byte[256];

	// Initialise Keyboard state array
	for(int i =0; i < 256; i++)
	{
		byte[i] = 0;		
	}

	GetKeyboardState((PBYTE)byte);	// Get Keyboard State

	// Convert the Virtual key to the lower case character.
#ifdef _ACE_UNICODE
	ToUnicodeEx(nVK, 0, byte, (LPWSTR)buffer, 10, 0, 0);
#else
	WORD wKeyAsc = 0;
	CString buffer = _T("");
	ToAscii(nVK, MapVirtualKey(nVK, 0), byte, &wKeyAsc, 0);
 	//CString buffer = LOBYTE(MapVirtualKeyEx(nVK,2,GetKeyboardLayout(0)));
	buffer = (char)wKeyAsc;

	TRACE("Convert VK to %s", buffer);


/*	buffer.MakeLower();

	if (HIBYTE(GetKeyState(VK_SHIFT)))
	{

		buffer.MakeUpper();

		SwitchCharacter(buffer);
	}*/
#endif

	return buffer;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Switches the character to the upper case value.  This should
//				: be used to convert from lower to upper case.  Any language
//				: dependent conversion should happen here.  So that a full
//				: function is setup to cater for as many different languages as
//				: possible.  A future step maybe to place this in a DLL.
//
// PARMETERS	: str - Character to convert to its upper case value
//
// RETURNS		: TRUE if succesfulll.
////////////////////////////////////////////////////////////////////////////////

BOOL CVKConversion::SwitchCharacter(CString& str)
{
	BOOL bReturn = FALSE;

	if(str == ZERO_LOWER)
	{
		str = ZERO_UPPER;
		bReturn = TRUE;
	}
	else if(str == ONE_LOWER)
	{
		str = ONE_UPPER;
		bReturn = TRUE;
	}
	else if(str == TWO_LOWER)
	{
		str = TWO_UPPER;
		bReturn = TRUE;
	}
	else if(str == THREE_LOWER)
	{
		str = THREE_UPPER;
		bReturn = TRUE;
	}
	else if(str == FOUR_LOWER)
	{
		str = FOUR_UPPER;
		bReturn = TRUE;
	}
	else if(str == FIVE_LOWER)
	{
		str = FIVE_UPPER;
		bReturn = TRUE;
	}
	else if(str == SIX_LOWER)
	{
		str = SIX_UPPER;
		bReturn = TRUE;
	}
	else if(str == SEVEN_LOWER)
	{
		str = SEVEN_UPPER;
		bReturn = TRUE;
	}
	else if(str == EIGHT_LOWER)
	{
		str = EIGHT_UPPER;
		bReturn = TRUE;
	}
	else if(str == NINE_LOWER)
	{
		str = NINE_UPPER;
		bReturn = TRUE;
	}
	else if(str == MINUS_LOWER)
	{
		str = MINUS_UPPER;
		bReturn = TRUE;
	}
	else if(str == EQUALS_LOWER)
	{
		str = EQUALS_UPPER;
		bReturn = TRUE;
	}
	else if(str == SQUARE_BRACKETO_LOWER)
	{
		str = SQUARE_BRACKETO_UPPER;
		bReturn = TRUE;
	}
	else if(str == SQUARE_BRACKETC_LOWER)
	{
		str = SQUARE_BRACKETC_UPPER;
		bReturn = TRUE;
	}
	else if(str == HASH_LOWER)
	{
		str = HASH_UPPER;
		bReturn = TRUE;
	}
	else if(str == SEMI_COLON_LOWER)
	{
		str = SEMI_COLON_UPPER;
		bReturn = TRUE;
	}
	else if(str == QUOTE_LOWER)
	{
		str = QUOTE_UPPER;
		bReturn = TRUE;
	}
	else if(str == COMMA_LOWER)
	{
		str = COMMA_UPPER;
		bReturn = TRUE;
	}
	else if(str == FULL_STOP_LOWER)
	{
		str = FULL_STOP_UPPER;
		bReturn = TRUE;
	}
	else if(str == BACKSLASH_LOWER)
	{
		str = BACKSLASH_UPPER;
		bReturn = TRUE;
	}
	else if(str == FORWARDSLASH_LOWER)
	{
		str = FORWARDSLASH_UPPER;
		bReturn = TRUE;
	}
	else
	{
		str.MakeUpper();
	}

	return bReturn;
}

BYTE CVKConversion::StringToVK(CString str)
{
	if(str.GetLength() == 1)
	{
		return LOBYTE(VkKeyScanEx((char)str.GetAt(0),GetKeyboardLayout(0)));
	}
	else
	{
		if(str == F1)
		{
			return VK_F1;
		}
		else if(str == F2)
		{
			return VK_F2;
		}
		else if(str == F3)
		{
			return VK_F3;
		}
		else if(str ==  F4)
		{
			return VK_F4;
		}
		else if(str ==F5)
		{
			return VK_F5;
		}
		else if(str == F6)
		{
			return VK_F6;
		}
		else if(str == F7)
		{
			return VK_F7;
		}
		else if(str == F8)
		{
			return VK_F8;
		}
		else if(str == F9)
		{
			return VK_F9;
		}
		else if(str == F10)
		{
			return VK_F10;
		}
		else if(str ==  F11)
		{
			return VK_F11;
		}
		else if(str == F12)
		{
			return VK_F12;
		}
		else if(str == PAGEUP)
		{
			return VK_PRIOR;
		}
		else if(str == PAGEDOWN)
		{
			return VK_NEXT;
		}
		else if(str == END)
		{
			return VK_END;
		}
		else if(str == HOME)
		{
			return VK_HOME;
		}
		else if(str == ARR_LEFT)
		{
			return VK_LEFT;
		}
		else if(str == ARR_RIGHT)
		{
			return VK_RIGHT;
		}
		else if(str == ARR_UP)
		{
			return VK_UP;
		}
		else if(str == ARR_DOWN)
		{
			return VK_DOWN;
		}
		else if(str == INSERT)
		{
			return VK_INSERT;
		}
		else if(str == DELETE_KEY)
		{
			return VK_DELETE;
		}
		else
		{
			return -1;
		}
	}
}