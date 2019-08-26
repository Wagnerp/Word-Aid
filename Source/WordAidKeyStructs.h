#ifndef  _WORDAIDKEYSTRUCTS
#define  _WORDAIDKEYSTRUCTS

////////////////////////////////////////////////////////////////////////////////
// The following constant and structures are used to check to make sure that
// the hotkeys in the settings are set to valid values.  The FUNCTION_KEYS,
// NUMBER_KEYS and NUMPAD_KEYS are used to set the virtual key codes that
// relate to the type of Select word either the function keys, number keys or
// the Number pad keys.
////////////////////////////////////////////////////////////////////////////////

// Set to the number ites that appear in the FUNCTION_KEYS,
// NUMBER_KEYS and NUMPAD_KEYS arrays below

const int MAX_SELECT_KEYS = 10;

const UINT FUNCTION_KEYS_ARR[] =
{
	VK_F1,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
};

const UINT NUMBER_KEYS_ARR[] = 
{
	48,
	49,
	50,
	51,
	52,
	53,
	54,
	55,
	56,
	57,
};

const UINT NUMPAD_KEYS_ARR[] =
{
	VK_NUMPAD0,
	VK_NUMPAD1,
	VK_NUMPAD2,
	VK_NUMPAD3,
	VK_NUMPAD4,
	VK_NUMPAD5,
	VK_NUMPAD6,
	VK_NUMPAD7,
	VK_NUMPAD8,
	VK_NUMPAD9,
};

// Set to the number of items in the UNDO_KEYS array below
const int NUMBER_OF_VALID_UNDOKEYS = 12;

const UINT UNDO_KEYS_ARR[] = 
{
	VK_F1,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
	VK_F11,
	VK_F12,
};

// Set to the number of items in the SHOWLIST_KEYS array below
const int NUMBER_OF_VALID_SHOWLISTKEYS = 12;

const UINT SHOWLIST_KEYS_ARR[] = 
{
	VK_F1,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
	VK_F11,
	VK_F12,
};

#endif //  _WORDAIDKEYSTRUCTS