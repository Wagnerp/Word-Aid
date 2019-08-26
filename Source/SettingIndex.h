#ifndef _SETTINGINDEX
#define _SETTINGINDEX

// Number of settings in array ( This most be the number if the KeyArray +1 )
const int SETTING_MAX	=		78;	//WA0016+21+27 incremented

// Path to find settings in
const CString  SETTING_PATH	=	_T("Software\\WordAid\\2.0");

// Array index to access settings from array

const int USER_NAME						= 0;
const int WORDAID_ON					= 1;
const int ABBREV_ON						= 2;
const int BUTTON_FONT_NAME				= 3;
const int BUTTON_FONT_SIZE				= 4;
const int BUTTON_FOREGROUND				= 5;
const int BUTTON_BACKGROUND				= 6;
const int HIGHLIGHT_COLOUR				= 7;
const int LABEL_FONT_NAME				= 8;
const int LABEL_FONT_SIZE				= 9;
const int LABEL_FOREGROUND				= 10;
const int LABEL_BACKGROUND				= 11;
const int SELECT_WORD_KEYS				= 12;
const int SELECT_WORD_SPEC				= 13;
const int UNDO_HOTKEY					= 14;
const int UNDO_SPEC						= 15;
const int SHOW_LIST						= 16;
const int SHOW_LIST_HOTKEY				= 17;
const int SHOW_LIST_SPEC				= 18;
const int AUTO_SPACE					= 19;
const int AUTO_CAPS						= 20;
const int AUTO_SPACE_WORD				= 21;
const int STARTUP_MINIMIZE				= 22;
const int STICKY_SHIFT					= 23;
const int ALPHA_FILENAME				= 24;
const int TOPIC_FILENAME				= 25;
const int BUTTONS_ON					= 26;
const int MAX_WORDS						= 27;
const int USE_SPEECH					= 28;
const int SPEECH_VOICE					= 29;
const int SPEECH_SPEED					= 30;
const int SPEECH_PITCH					= 31;
const int SPEECH_VOLUME					= 32;
const int SPEAK_ALL						= 33;
const int SPEAK_SELECTED				= 34;
const int DWELL_TIME_ALL				= 35;
const int DWELL_TIME_SELECTED			= 36;
const int ABBREV_EXPANSION_METHOD		= 37;
const int ABBREV_AUTOMATIC_DELAY		= 38;
const int ABBREV_MANUAL_HOTKEY			= 39;
const int ABBREV_MANUAL_SPEC_KEY		= 40;
const int ABBREV_WORD_BANK				= 41;
const int ABBREV_LISTED_EXPANSION		= 42;
const int ABBREV_FULL_EXPAN_HOTKEY		= 43;
const int ABBREV_FULL_EXPAN_SPEC_KEY	= 44;
const int ABBREV_PART_EXPAN_HOTKEY		= 45;
const int ABBREV_PART_EXPAN_SPEC_KEY	= 46;
const int ABBREV_FILENAME				= 47;
const int MIN_WORD_LENGTH				= 48;
const int MAX_WORD_LENGTH				= 49;
const int ABBREV_LIST_HIDE				= 50;
const int ALPHA_LIST_HIDE				= 51;
const int TOPIC_LIST_HIDE				= 52;
const int ADD_WORD						= 53;
const int ADD_WORD_SPEC					= 54;
const int DEL_WORD_KEY					= 55;
const int DEL_WORD_SPEC					= 56;
const int INS_WORD_KEY					= 57;
const int INS_WORD_SPEC					= 58;
const int SAY_KEY						= 59;
const int SAY_SPEC						= 60;
const int PGUP_KEY						= 61;
const int PGUP_SPEC						= 62;
const int PGDWN_KEY						= 63;
const int PGDWN_SPEC					= 64;
const int UPARR_KEY						= 65;
const int UPARR_SPEC					= 66;
const int DWNARR_KEY					= 67;
const int DWNARR_SPEC					= 68;
const int LFTARR_KEY					= 69;
const int LFTARR_SPEC					= 70;
const int RGHTARR_KEY					= 71;
const int RGHTARR_SPEC					= 72;
const int BUTTON_BITMAPS_ON				= 73;
const int ABBREV_BTN_HOTKEY				= 74;	//WA0016
const int ABBREV_BTN_HOTKEY_SPEC		= 75;	//WA0021
const int SPEAK_FIRST_AUTO				= 76;	//WA0027
const int LAST							= 77;	//WA0016+21+27 incremented

// Set check box on or off
const CString CHECK_BOX_ON	= _T("1");
const CString CHECK_BOX_OFF = _T("0");

// Settings for Expansion Method
const CString AUTOMATIC_EXPANSION		= _T("0");
const CString SEMI_AUTOMATIC			= _T("1");
const CString MANUAL_HOTKEY				= _T("2");

// TODO : Use in Settings
// Settings for WordAid selection keys
const int FUNCTION_KEYS					= 0;
const int NUMBER_KEYS					= 1;
const int NUMPAD_KEYS					= 2;

// Hide and show list
const int SHOWLIST	= 0;
const int HIDELIST = 1;

#endif // _SETTINGINDEX