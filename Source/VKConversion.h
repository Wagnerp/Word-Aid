#pragma once

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: This class has two static functions that can be called to 
//				: convert a virtual key code to a character and one to convert
//				: a character to its virtual key code.
//
// CREATED		: 17/06/02
// BY			: Jason Walsh
// LAST UPDATED	: 15/07/02
// BY			: Jason Walsh
// CHANGES		: New Method to convert all keys from a string to its Virtual
//				: key code value.  See documentation for more details.
//
// UPDATED		: 25/10/04
// BY			: Jason Walsh
//				: Updated the VKToChar method this now does not rely upon the
//				: the switch characters method and can work in different language 
//				: locals
////////////////////////////////////////////////////////////////////////////////

class CVKConversion
{
public:
	//CVKConversion(void);
	//~CVKConversion(void);

	static CString VKToChar(UINT nVK);		// Gets the character from the virtual key	
	static BYTE CharToVK(char strChar);			// Get the Virtual key code from the char
	static BYTE StringToVK(CString strChar);	// Get Virtual key code from character.  This
												// can also convert some other keys as F1 to its
												// virtual key code
	static BOOL SwitchCharacter(CString& str);
};
