#ifndef _SETTINGSTRUCT
#define _SETTINGSTRUCT

struct KeyArrayStruct
{
	char szKey[30];			// Key name
	char szValue[30];		// Default value
}; 

// Settings
struct KeyArrayStruct KeyArray[] = 
{
	_T("User Name"),					_T("User"),			// 0
	_T("WordAid On"),					_T("1"),			// 1
	_T("Abbrev On"),					_T("0"),			// 2
	_T("Button Font Name"),				_T("Arial"),		// 3
	_T("Button Font Size"),				_T("20"),			// 4	WA0004 
	_T("Button Foreground"),			_T("88,88,88"),		// 5
	_T("Button Background"),			_T("150,150,150"),	// 6	WA0004 
	_T("Highlight Colour"),				_T("255,255,0"),	// 7
	_T("Label Font Name"),				_T("Arial"),		// 8	WA0004 
	_T("Label Font Size"),				_T("24"),			// 9
	_T("Label Foreground"),				_T("0,0,0"),		// 10	WA0004 
	_T("Label Background"),				_T("192,192,192"),	// 11
	_T("Select Word Keys"),				_T("F1 to F10"),	// 12
	_T("Select Word Special Key"),		_T(""),				// 13
	_T("Undo Hotkey"),					_T("122"),			// 14
	_T("Undo Special Key"),				_T(""),				// 15
	_T("Show List"),					_T("1"),			// 16
	_T("Show List Hotkey"),				_T("123"),			// 17
	_T("Show List Special Key"),		_T(""),				// 18 
	_T("Auto Space"),					_T("1"),			// 19
	_T("Auto Caps"),					_T("1"),			// 20
	_T("Auto Space - word"),			_T("1"),			// 21	//WA0027 changed to true
	_T("Startup Minimize"),				_T("0"),			// 22
	_T("Sticky Shift"),					_T("0"),			// 23
	_T("Alpha Filename"),				_T(""),				// 24
	_T("Topic Filename"),				_T(""),				// 25
	_T("Buttons On"),					_T("1"),			// 26
	_T("Max Words"),					_T("10"),			// 27
	_T("Speech On"),					_T("0"),			// 28
	_T("Voice Name"),					_T("Mike"),			// 29
	_T("Voice Speed"),					_T("170"),			// 30
	_T("Voice Pitch"),					_T("113"),			// 31
	_T("Voice Volume"),					_T("93"),			// 32
	_T("Speak all words"),				_T("0"),			// 33
	_T("Speak selected"),				_T("0"),			// 34
	_T("Dwell time all words"),			_T("1"),			// 35
	_T("Dwell time selection"),			_T("2"),			// 36
	_T("Expansion Method"),				_T("1"),			// 37
	_T("Automatic Delay"),				_T("3"),			// 38
	_T("Manual Hotkey"),				_T("34"),			// 39
	_T("Manual Special Key"),			_T("ac"),			// 40
	_T("Word Bank"),					_T("0"),			// 41
	_T("Expansion List"),				_T("1"),			// 42
	_T("Full Expansion Hotkey"),		_T("70"),			// 43
	_T("Full Expansion Special Key"),	_T("c"),			// 44
	_T("Part Expansion Hotkey"),		_T("80"),			// 45
	_T("Part Expansion Special Key"),	_T("c"),			// 46
	_T("Abbrev Filename"),				_T(""),				// 47
	_T("Min Word Length"),				_T("3"),			// 48
	_T("Max Word Length"),				_T("15"),			// 49
	_T("Abbrev List Hidden"),			_T("0"),			// 50
	_T("Alpha List Hidden"),			_T("0"),			// 51
	_T("Topic List Hidden"),			_T("0"),			// 52
	_T("Add Word Key"),					_T("45"),			// 53
	_T("Add Word Spec"),				_T(""),				// 54
	_T("Delete Word Key"),				_T("46"),			// 55
	_T("Delete Word Spec"),				_T(""),				// 56
	_T("Insert Key"),					_T("36"),			// 57
	_T("Insert Spec"),					_T(""),				// 58
	_T("Say Key"),						_T("35"),			// 59
	_T("Say Spec"),						_T(""),				// 60
	_T("PageUp Key"),					_T("33"),			// 61
	_T("PageUp Spec"),					_T(""),				// 62
	_T("PageDwn Key"),					_T("34"),			// 63
	_T("PageDwn Spec"),					_T(""),				// 64
	_T("Up List Key"),					_T("38"),			// 65
	_T("Up List Spec"),					_T(""),				// 66
	_T("Down List Key"),				_T("40"),			// 67
	_T("Down List Spec"),				_T(""),				// 68
	_T("Left Move List Key"),			_T("37"),			// 69
	_T("Left Move List Spec"),			_T(""),				// 70
	_T("Right Move List Key"),			_T("39"),			// 71
	_T("Right Move List Spec"),			_T(""),				// 72
	_T("Button Bitmaps On"),			_T("1"),			// 73
	_T("Abbrev Button Hotkey"),			_T("223"),			// 74	//WA0016 default = `
	_T("Abbrev Btn Hotkey Spec"),		_T(""),				// 75	//WA0021
	_T("Speak First Word"),				_T("0"),			// 76	//WA0027
	_T("LAST"),_T("LAST"),									// 77
};

#endif //_SETTINGSTRUCT
