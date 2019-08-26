#include "stdafx.h"
#include "wordaidtreedialog.h"

#include "VKConversion.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CWordAidTreeDialog::CWordAidTreeDialog(CSetting* pSettings)
{
	m_pSettings = pSettings;

	InitialiseKeys();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CWordAidTreeDialog::~CWordAidTreeDialog(void)
{
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Sets the keys to the current settings in the registry
////////////////////////////////////////////////////////////////////////////////

void CWordAidTreeDialog::InitialiseKeys()
{

	ASSERT(m_pSettings);	// Make sure the settings pointer has been set

	// Set the Select Word Keys
	SetSelectWordKeys(m_pSettings->GetSetting(SELECT_WORD_KEYS),
					  m_pSettings->GetSetting(SELECT_WORD_SPEC));

	
	// Set the Undo hotkey
	KEYS_USED undo;

	undo.nVK = atoi(m_pSettings->GetSetting(UNDO_HOTKEY));
	undo.strSpecKey = m_pSettings->GetSetting(UNDO_SPEC);

	SetUndoKey(undo);

	// Set Show List Hotkey
	KEYS_USED showList;

	showList.nVK = atoi(m_pSettings->GetSetting(SHOW_LIST_HOTKEY));
	showList.strSpecKey = m_pSettings->GetSetting(SHOW_LIST_SPEC);

	SetShowListKey(showList);

	// Set Abbrev Manual Expansion Hotkey
	KEYS_USED expanKey;

	expanKey.nVK = atoi(m_pSettings->GetSetting(ABBREV_MANUAL_HOTKEY));
	expanKey.strSpecKey = m_pSettings->GetSetting(ABBREV_MANUAL_SPEC_KEY);

	SetAbbrevManualKey(expanKey);

	// Set the Un-expand (full) hotkey
	KEYS_USED unexpandFullKey;

	unexpandFullKey.nVK = atoi(m_pSettings->GetSetting(ABBREV_FULL_EXPAN_HOTKEY));
	unexpandFullKey.strSpecKey = m_pSettings->GetSetting(ABBREV_FULL_EXPAN_SPEC_KEY);

	SetUnExpandFullKey(unexpandFullKey);

	// Set the Un-expand (part) hotkey
	KEYS_USED unexpandPartKey;

	unexpandPartKey.nVK = atoi(m_pSettings->GetSetting(ABBREV_PART_EXPAN_HOTKEY));
	unexpandPartKey.strSpecKey = m_pSettings->GetSetting(ABBREV_PART_EXPAN_SPEC_KEY);

	SetUnExpandPartKey(unexpandPartKey);

	KEYS_USED addWordKey;

	addWordKey.nVK			= atoi(m_pSettings->GetSetting(ADD_WORD));
	addWordKey.strSpecKey	= m_pSettings->GetSetting(ADD_WORD_SPEC);

	SetAddWordKey(addWordKey, FALSE);

	KEYS_USED delWordKey;

	delWordKey.nVK			= atoi(m_pSettings->GetSetting(DEL_WORD_KEY));
	delWordKey.strSpecKey	= m_pSettings->GetSetting(DEL_WORD_SPEC);

	SetDeleteWordKey(delWordKey, FALSE);

	KEYS_USED insKey;

	insKey.nVK			= atoi(m_pSettings->GetSetting(INS_WORD_KEY));
	insKey.strSpecKey	= m_pSettings->GetSetting(INS_WORD_SPEC);

	SetInsertWordKey(insKey, FALSE);

	KEYS_USED sayKey;

	sayKey.nVK			= atoi(m_pSettings->GetSetting(SAY_KEY));
	sayKey.strSpecKey	= m_pSettings->GetSetting(SAY_SPEC);

	SetSayWordKey(sayKey, FALSE);

	KEYS_USED pgUpKey;

	pgUpKey.nVK			= atoi(m_pSettings->GetSetting(PGUP_KEY));
	pgUpKey.strSpecKey	= m_pSettings->GetSetting(PGUP_SPEC);

	SetPageUpKey(pgUpKey, FALSE);

	KEYS_USED pgDwnKey;

	pgDwnKey.nVK			= atoi(m_pSettings->GetSetting(PGDWN_KEY));
	pgDwnKey.strSpecKey		= m_pSettings->GetSetting(PGDWN_SPEC);

	SetPageDownKey(pgDwnKey, FALSE);

	KEYS_USED upArrKey;

	upArrKey.nVK			= atoi(m_pSettings->GetSetting(UPARR_KEY));
	upArrKey.strSpecKey		= m_pSettings->GetSetting(UPARR_SPEC);

	SetUpArrowKey(upArrKey, FALSE);

	KEYS_USED dwnArrKey;

	dwnArrKey.nVK			= atoi(m_pSettings->GetSetting(DWNARR_KEY));
	dwnArrKey.strSpecKey		= m_pSettings->GetSetting(DWNARR_SPEC);

	SetDownArrowKey(dwnArrKey, FALSE);

	KEYS_USED lftArrKey;

	lftArrKey.nVK			= atoi(m_pSettings->GetSetting(LFTARR_KEY));
	lftArrKey.strSpecKey		= m_pSettings->GetSetting(LFTARR_SPEC);

	SetLeftArrowKey(lftArrKey, FALSE);

	KEYS_USED rghtArrKey;

	rghtArrKey.nVK			= atoi(m_pSettings->GetSetting(RGHTARR_KEY));
	rghtArrKey.strSpecKey		= m_pSettings->GetSetting(RGHTARR_SPEC);

	SetRightArrowKey(rghtArrKey, FALSE);
//WA0016
	KEYS_USED abbrevBtnHK;

	abbrevBtnHK.nVK			= atoi(m_pSettings->GetSetting(ABBREV_BTN_HOTKEY));
//	abbrevBtnHK.strSpecKey		= "";
	abbrevBtnHK.strSpecKey		= m_pSettings->GetSetting(ABBREV_BTN_HOTKEY_SPEC); //WA0021

	SetAbbrevBtnHotkey(abbrevBtnHK, FALSE);


}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Sets the Select Word keys depending upon the string that
//				: is sent.  It uses a number of predefined arrays to set the
//				: virtual key codes that relate to the select word string.
//
// PARAMETERS	: strType - the type of selection keys.  The valid string are
//				: IDS_FUNC_KEYS, IDS_NUM_KEYS, IDS_NUMPAD_KEYS and IDS_NO_KEYS.
//				: strSpecKeys - The string that represents the special keys.
//				: this should be formated so that 'a' = ALT, 'c' = CONTROL.
//
// RETURNS		: TRUE if the strType was valid.
////////////////////////////////////////////////////////////////////////////////

BOOL CWordAidTreeDialog::SetSelectWordKeys(CString strType, 
										   CString strSpecKeys)
{
	// Make sure that the strSpecKeys are valid
	ASSERT((strSpecKeys.FindOneOf(_T("a")) != -1 ||
			strSpecKeys.FindOneOf(_T("c")) != -1 ||
			strSpecKeys == _T("")));

	BOOL bReturn = TRUE;
	CString strFuncKeys = _T("");
	CString strNumKeys = _T("");
	CString strNumPadKeys = _T("");
	CString strNoKeys = _T("");

	strFuncKeys.LoadString(IDS_FUNC_KEYS);
	strNumKeys.LoadString(IDS_NUM_KEYS);
	strNumPadKeys.LoadString(IDS_NUMPAD_KEYS);
	strNoKeys.LoadString(IDS_NO_KEYS);

	if(strType == strFuncKeys)
	{
		for(int i = 0; i < MAX_SELECT_KEYS; i++)
		{
			m_SelectWord[i].nVK = FUNCTION_KEYS_ARR[i];
			m_SelectWord[i].strSpecKey = strSpecKeys;

			// Make sure hotkey not selected by other hotkey
			if(!CheckKeySelection(FUNCTION_KEYS_ARR[i], strSpecKeys, KT_SELECT_WORD))
			{		
				bReturn = FALSE;
			}
		}
	}
	else if(strType == strNumKeys)
	{
		for(int i = 0; i < MAX_SELECT_KEYS; i++)
		{
			m_SelectWord[i].nVK = NUMBER_KEYS_ARR[i];
			m_SelectWord[i].strSpecKey = strSpecKeys;

			// Make sure hotkey not selected by other hotkey
			if(!CheckKeySelection(NUMBER_KEYS_ARR[i], strSpecKeys, KT_SELECT_WORD))
			{
				bReturn = FALSE;
			}
		}
	}
	else if(strType == strNumPadKeys)
	{
		for(int i = 0; i < MAX_SELECT_KEYS; i++)
		{
			m_SelectWord[i].nVK = NUMPAD_KEYS_ARR[i];
			m_SelectWord[i].strSpecKey = strSpecKeys;
			
			// Make sure hotkey not selected by other hotkey
			if(!CheckKeySelection(NUMPAD_KEYS_ARR[i], strSpecKeys, KT_SELECT_WORD))
			{
				bReturn = FALSE;
			}
		}
	}
	else if(strType == strNoKeys)
	{
		for(int i = 0; i < MAX_SELECT_KEYS; i++)
		{
			m_SelectWord[i].nVK = 0;
			m_SelectWord[i].strSpecKey = _T("");
		}
	}
	else
	{
		ASSERT(FALSE);	// ERROR no such keys
		bReturn = FALSE;
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the undo key.  The function checks to make sure
//				: that the special keys are valid and the virtual key code is 
//				: valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the undo hotkey.
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

BOOL CWordAidTreeDialog::SetUndoKey(KEYS_USED keys)
{
	BOOL bReturn = TRUE;

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	if(!CheckKeySelection(keys.nVK, keys.strSpecKey, KT_UNDO))
	{
		bReturn = FALSE;
	}

	if(bReturn)
	{
		m_Undo.nVK = keys.nVK;
		m_Undo.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_Undo.nVK = 0;
		m_Undo.strSpecKey = _T("");
	}


	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the undo key.  The function checks to make sure
//				: that the special keys are valid and the virtual key code is 
//				: valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the undo hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetUndoKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_UNDO, bMessage);

	if(type == KT_NONE)
	{
		m_Undo.nVK = keys.nVK;
		m_Undo.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_Undo.nVK = 0;
		m_Undo.strSpecKey = _T("");
	}

	return type;

}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the showlist key.  The function checks to make sure
//				: that the special keys are valid and the virtual key code is 
//				: valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the show list hotkey.
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

BOOL CWordAidTreeDialog::SetShowListKey(KEYS_USED keys)
{
	BOOL bReturn = TRUE;

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	if(!CheckKeySelection(keys.nVK, keys.strSpecKey, KT_SHOWLIST))
	{
		bReturn = FALSE;
	}

	if(bReturn)
	{
		m_ShowList.nVK			= keys.nVK;
		m_ShowList.strSpecKey	= keys.strSpecKey;
	}
	else 
	{
		m_ShowList.nVK			= 0;
		m_ShowList.strSpecKey	= _T("");
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the show list key.  The function checks to make sure
//				: that the special keys are valid and the virtual key code is 
//				: valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the show list hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetShowListKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_SHOWLIST, bMessage);

	if(type == KT_NONE)
	{
		m_ShowList.nVK = keys.nVK;
		m_ShowList.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_ShowList.nVK = 0;
		m_ShowList.strSpecKey = _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Abbrev Manual Expansion key.  The function 
//				: checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the manual expansion hotkey.
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

BOOL CWordAidTreeDialog::SetAbbrevManualKey(KEYS_USED keys)
{
	BOOL bReturn = TRUE;

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	if(!CheckKeySelection(keys.nVK, keys.strSpecKey, KT_ABBREV_MANUAL))
	{
		bReturn = FALSE;
	}

	if(bReturn)
	{
		m_AbbrevManual.nVK = keys.nVK;
		m_AbbrevManual.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_AbbrevManual.nVK = 0;
		m_AbbrevManual.strSpecKey = _T("");
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Abbrev Un-expand full key.  The function 
//				: checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Un-expand full hotkey.
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

BOOL CWordAidTreeDialog::SetUnExpandFullKey(KEYS_USED keys)
{
	BOOL bReturn = TRUE;

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	if(!CheckKeySelection(keys.nVK, keys.strSpecKey, KT_UNEXPAND_FULL))
	{
		bReturn = FALSE;
	}

	if(bReturn)
	{
		m_UnExpandFull.nVK = keys.nVK;
		m_UnExpandFull.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_UnExpandFull.nVK = 0;
		m_UnExpandFull.strSpecKey = _T("");
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the show Abbrev Un-expand full key.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Abbrev Un-expand full hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetUnExpandFullKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_UNEXPAND_FULL, bMessage);

	if(type == KT_NONE)
	{
		m_UnExpandFull.nVK = keys.nVK;
		m_UnExpandFull.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_UnExpandFull.nVK = 0;
		m_UnExpandFull.strSpecKey = _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Abbrev Un-expand part key.  The function 
//				: checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Un-expand part hotkey.
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

BOOL CWordAidTreeDialog::SetUnExpandPartKey(KEYS_USED keys)
{
	BOOL bReturn = TRUE;

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	if(!CheckKeySelection(keys.nVK, keys.strSpecKey, KT_UNEXPAND_PART))
	{
		bReturn = FALSE;
	}

	if(bReturn)
	{
		m_UnExpandPart.nVK = keys.nVK;
		m_UnExpandPart.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_UnExpandPart.nVK = 0;
		m_UnExpandPart.strSpecKey = _T("");
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the show Abbrev Un-expand part key.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Abbrev Un-expand part hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetUnExpandPartKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_UNEXPAND_PART, bMessage);

	if(type == KT_NONE)
	{
		m_UnExpandPart.nVK = keys.nVK;
		m_UnExpandPart.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_UnExpandPart.nVK = 0;
		m_UnExpandPart.strSpecKey = _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Add Word key.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the add word hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetAddWordKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_ADD_WORD, bMessage);

	if(type == KT_NONE)
	{
		m_AddWord.nVK = keys.nVK;
		m_AddWord.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_AddWord.nVK = 0;
		m_AddWord.strSpecKey = _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Delete Word key.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the add word hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetDeleteWordKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_DELETE_WORD, bMessage);

	if(type == KT_NONE)
	{
		m_DeleteWord.nVK = keys.nVK;
		m_DeleteWord.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_DeleteWord.nVK = 0;
		m_DeleteWord.strSpecKey = _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Insert Word key.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Insert word hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetInsertWordKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_INSERT, bMessage);

	if(type == KT_NONE)
	{
		m_InsertWord.nVK = keys.nVK;
		m_InsertWord.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_InsertWord.nVK = 0;
		m_InsertWord.strSpecKey = _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Say Word key.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Say word hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetSayWordKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_SAY, bMessage);

	if(type == KT_NONE)
	{
		m_SayWord.nVK = keys.nVK;
		m_SayWord.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_SayWord.nVK = 0;
		m_SayWord.strSpecKey = _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Page Up key.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Page Up hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetPageUpKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_PAGEUP, bMessage);

	if(type == KT_NONE)
	{
		m_PageUp.nVK = keys.nVK;
		m_PageUp.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_PageUp.nVK = 0;
		m_PageUp.strSpecKey = _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Page Down key.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Page Down hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetPageDownKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_PAGEDWN, bMessage);

	if(type == KT_NONE)
	{
		m_PageDown.nVK = keys.nVK;
		m_PageDown.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_PageDown.nVK = 0;
		m_PageDown.strSpecKey = _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Up Arrow key.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Up Arrow hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetUpArrowKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_UPARR, bMessage);

	if(type == KT_NONE)
	{
		m_UpArrow.nVK			= keys.nVK;
		m_UpArrow.strSpecKey	= keys.strSpecKey;
	}
	else
	{
		m_UpArrow.nVK			= 0;
		m_UpArrow.strSpecKey	= _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Down Arrow key.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Down Arrow hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetDownArrowKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_DWNARR, bMessage);

	if(type == KT_NONE)
	{
		m_DownArrow.nVK			= keys.nVK;
		m_DownArrow.strSpecKey	= keys.strSpecKey;
	}
	else
	{
		m_DownArrow.nVK			= 0;
		m_DownArrow.strSpecKey	= _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Left Arrow key.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Left Arrow hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetLeftArrowKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_LEFTARR, bMessage);

	if(type == KT_NONE)
	{
		m_LeftArrow.nVK			= keys.nVK;
		m_LeftArrow.strSpecKey	= keys.strSpecKey;
	}
	else
	{
		m_LeftArrow.nVK			= 0;
		m_LeftArrow.strSpecKey	= _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Right Arrow key.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Right Arrow hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetRightArrowKey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_RIGHTARR, bMessage);

	if(type == KT_NONE)
	{
		m_RightArrow.nVK		= keys.nVK;
		m_RightArrow.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_RightArrow.nVK		= 0;
		m_RightArrow.strSpecKey = _T("");
	}

	return type;
}
//WA0016
////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Used to set the Abbrev button hotkey.  The function
//				:  checks to make sure that the special keys are valid and the 
//				: virtual key code is valid.
//
// PARAMETERS	: keys - KEYS_USED structure that contains the virtual key code
//				: and the special keys string for the Abbrev button hotkey.
//				: bMessage - TRUE if the message should be displayed
//
// RETURNS		: TRUE if the keys were valid and it is not set by any other
//				: hotkey.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::SetAbbrevBtnHotkey(KEYS_USED keys, BOOL bMessage /*= FALSE*/)
{

	// Make sure that the strSpecKeys are valid
	ASSERT((keys.strSpecKey.FindOneOf(_T("a")) != -1 ||
			keys.strSpecKey.FindOneOf(_T("c")) != -1 ||
			keys.strSpecKey == _T("")));

	// Make sure hotkey not selected by other hotkey
	KEY_TYPE type = CheckKeySelection(keys.nVK, keys.strSpecKey, KT_ABBREVBTN, bMessage);

	if(type == KT_NONE)
	{
		m_AbbrevBtnHK.nVK		= keys.nVK;
		m_AbbrevBtnHK.strSpecKey = keys.strSpecKey;
	}
	else
	{
		m_AbbrevBtnHK.nVK		= 0;
		m_AbbrevBtnHK.strSpecKey = _T("");
	}

	return type;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: This functions checks to make sure that when a hotkey is 
//				: being set that it does not match one that has been already set.
//				: The function does not check against itself.
//
// PARAMETERS	: nCk - Virtual key code to check
//				: strSpec - Special keys string to check
//				: type - the type of hotkey that is being set, this must be one
//				: of the KEY_TYPE enumerated values.
//
// RETURNS		: TRUE if the key is not already set; otherwise false.
////////////////////////////////////////////////////////////////////////////////

BOOL CWordAidTreeDialog::CheckKeySelection(UINT nVK, CString strSpec, KEY_TYPE type)
{
	BOOL bReturn = TRUE;
	CString strMessage = _T("");
	CString strCannot = _T("");

	static BOOL first = TRUE;	//added to stop warning dialogue in release version

	strCannot.LoadString(IDS_CANNOT_SET);

	// Check against Select Word keys
	if(type != KT_SELECT_WORD)
	{
		for(int i = 0; i < MAX_SELECT_KEYS; i++)
		{
			if((nVK == m_SelectWord[i].nVK && strSpec == m_SelectWord[i].strSpecKey) && m_SelectWord[i].nVK != 0)
			{
				strMessage.LoadString(IDS_SELECT_WORD_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

				return FALSE;
			}
		}
	}

	// Check against undo keys
	if(type != KT_UNDO)
	{
		if((nVK == m_Undo.nVK && strSpec == m_Undo.strSpecKey) && m_Undo.nVK != 0)
		{
			strMessage.LoadString(IDS_UNDO_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check against Show list keys
	if(type != KT_SHOWLIST)
	{
		if((nVK == m_ShowList.nVK && strSpec == m_ShowList.strSpecKey) && m_ShowList.nVK != 0)
		{
			strMessage.LoadString(IDS_SHOWLIST_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check against Abbrev Manual Expansion key
	if(type != KT_ABBREV_MANUAL)
	{
		if((nVK == m_AbbrevManual.nVK && strSpec == m_AbbrevManual.strSpecKey) && m_AbbrevManual.nVK != 0)
		{
			strMessage.LoadString(IDS_ABBREV_MANUAL_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check Against Un-expand full key
	if(type != KT_UNEXPAND_FULL)
	{
		if((nVK == m_UnExpandFull.nVK && strSpec == m_UnExpandFull.strSpecKey) && m_UnExpandFull.nVK != 0)
		{
			strMessage.LoadString(IDS_UNEXPAND_FULL_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check against Un-expand part key
	if(type != KT_UNEXPAND_PART)
	{
		if((nVK == m_UnExpandPart.nVK && strSpec == m_UnExpandPart.strSpecKey) && m_UnExpandPart.nVK != 0)
		{
			strMessage.LoadString(IDS_UNEXPAND_PART_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check against Add Word key
	if(type != KT_ADD_WORD)
	{
		if((nVK == m_AddWord.nVK && strSpec == m_AddWord.strSpecKey) && m_AddWord.nVK != 0)
		{
			strMessage.LoadString(IDS_ADD_WORD_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check against Delete Word key
	if(type != KT_DELETE_WORD)
	{
		if((nVK == m_DeleteWord.nVK && strSpec == m_DeleteWord.strSpecKey) && m_DeleteWord.nVK)
		{
			strMessage.LoadString(IDS_DELETE_WORD_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check against Insert key
	if(type != KT_INSERT)
	{
		if((nVK == m_InsertWord.nVK && strSpec == m_InsertWord.strSpecKey) && m_InsertWord.nVK)
		{
			strMessage.LoadString(IDS_INSERT_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check against Say key
	if(type != KT_SAY)
	{
		if((nVK == m_SayWord.nVK && strSpec == m_SayWord.strSpecKey) && m_SayWord.nVK)
		{
			strMessage.LoadString(IDS_SAY_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check against Page Up key
	if(type != KT_PAGEUP)
	{
		if((nVK == m_PageUp.nVK && strSpec == m_PageUp.strSpecKey) && m_PageUp.nVK)
		{
			strMessage.LoadString(IDS_PAGEUP_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check against Page Down key
	if(type != KT_PAGEDWN)
	{
		if((nVK == m_PageDown.nVK && strSpec == m_PageDown.strSpecKey) && m_PageDown.nVK)
		{
			strMessage.LoadString(IDS_PAGEDWN_SAME);
			if (!first)			//added to stop warning dialogue in release version
				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			else				//added to stop warning dialogue in release version
				first = FALSE;	//added to stop warning dialogue in release version

			return FALSE;
		}
	}

	// Check against Up Arrow key
	if(type != KT_UPARR)
	{
		if((nVK == m_UpArrow.nVK && strSpec == m_UpArrow.strSpecKey) && m_UpArrow.nVK)
		{
			strMessage.LoadString(IDS_UPARR_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check against Down Arrow key
	if(type != KT_DWNARR)
	{
		if((nVK == m_DownArrow.nVK && strSpec == m_DownArrow.strSpecKey) && m_DownArrow.nVK)
		{
			strMessage.LoadString(IDS_DWNARR_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check against Left Arrow key
	if(type != KT_LEFTARR)
	{
		if((nVK == m_LeftArrow.nVK && strSpec == m_LeftArrow.strSpecKey) && m_LeftArrow.nVK)
		{
			strMessage.LoadString(IDS_LEFTARR_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}

	// Check against Right Arrow key
	if(type != KT_RIGHTARR)
	{
		if((nVK == m_RightArrow.nVK && strSpec == m_RightArrow.strSpecKey) && m_RightArrow.nVK)
		{
			strMessage.LoadString(IDS_RIGHTARR_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}
//WA0016
	if(type != KT_ABBREVBTN)
	{
		if((nVK == m_AbbrevBtnHK.nVK && strSpec == m_AbbrevBtnHK.strSpecKey) && m_AbbrevBtnHK.nVK)
		{
			strMessage.LoadString(IDS_ABBREVBTN_SAME);

			AfxMessageBox(strCannot + GetTypeString(type) + strMessage);

			return FALSE;
		}
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: This functions checks to make sure that when a hotkey is 
//				: being set that it does not match one that has been already set.
//				: The function does not check against itself.
//
// PARAMETERS	: nCk - Virtual key code to check
//				: strSpec - Special keys string to check
//				: type - the type of hotkey that is being set, this must be one
//				: of the KEY_TYPE enumerated values.
//
// RETURNS		: TRUE if the key is not already set; otherwise false.
////////////////////////////////////////////////////////////////////////////////

KEY_TYPE CWordAidTreeDialog::CheckKeySelection(UINT nVK, CString strSpec, KEY_TYPE type, BOOL bMessage)
{
	BOOL bReturn = TRUE;
	CString strMessage = _T("");
	CString strCannot = _T("");

	strCannot.LoadString(IDS_CANNOT_SET);

	// Check against Select Word keys
	if(type != KT_SELECT_WORD)
	{
		for(int i = 0; i < MAX_SELECT_KEYS; i++)
		{
			if((nVK == m_SelectWord[i].nVK && strSpec == m_SelectWord[i].strSpecKey) && m_SelectWord[i].nVK != 0)
			{
				if(bMessage)
				{
					strMessage.LoadString(IDS_SELECT_WORD_SAME);

					AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
				}

				return KT_SELECT_WORD;
			}
		}
	}

	// Check against undo keys
	if(type != KT_UNDO)
	{
		if((nVK == m_Undo.nVK && strSpec == m_Undo.strSpecKey) && m_Undo.nVK != 0)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_UNDO_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_UNDO;
		}
	}

	// Check against Show list keys
	if(type != KT_SHOWLIST)
	{
		if((nVK == m_ShowList.nVK && strSpec == m_ShowList.strSpecKey) && m_ShowList.nVK != 0)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_SHOWLIST_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_SHOWLIST;
		}
	}

	// Check against Abbrev Manual Expansion key
	if(type != KT_ABBREV_MANUAL)
	{
		if((nVK == m_AbbrevManual.nVK && strSpec == m_AbbrevManual.strSpecKey) && m_AbbrevManual.nVK != 0)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_ABBREV_MANUAL_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_ABBREV_MANUAL;
		}
	}

	// Check Against Un-expand full key
	if(type != KT_UNEXPAND_FULL)
	{
		if((nVK == m_UnExpandFull.nVK && strSpec == m_UnExpandFull.strSpecKey) && m_UnExpandFull.nVK != 0)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_UNEXPAND_FULL_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_UNEXPAND_FULL;
		}
	}

	// Check against Un-expand part key
	if(type != KT_UNEXPAND_PART)
	{
		if((nVK == m_UnExpandPart.nVK && strSpec == m_UnExpandPart.strSpecKey) && m_UnExpandPart.nVK != 0)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_UNEXPAND_PART_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_UNEXPAND_PART;
		}
	}

	// Check against Add Word key
	if(type != KT_ADD_WORD)
	{
		if((nVK == m_AddWord.nVK && strSpec == m_AddWord.strSpecKey) && m_AddWord.nVK != 0)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_ADD_WORD_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_ADD_WORD;
		}
	}

	// Check against Delete Word key
	if(type != KT_DELETE_WORD)
	{
		if((nVK == m_DeleteWord.nVK && strSpec == m_DeleteWord.strSpecKey) && m_DeleteWord.nVK)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_DELETE_WORD_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_DELETE_WORD;
		}
	}

	// Check against Insert key
	if(type != KT_INSERT)
	{
		if((nVK == m_InsertWord.nVK && strSpec == m_InsertWord.strSpecKey) && m_InsertWord.nVK)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_INSERT_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_INSERT;
		}
	}

	// Check against Say key
	if(type != KT_SAY)
	{
		if((nVK == m_SayWord.nVK && strSpec == m_SayWord.strSpecKey) && m_SayWord.nVK)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_SAY_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_SAY;
		}
	}

	// Check against Page Up key
	if(type != KT_PAGEUP)
	{
		if((nVK == m_PageUp.nVK && strSpec == m_PageUp.strSpecKey) && m_PageUp.nVK)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_PAGEUP_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_PAGEUP;
		}
	}

	// Check against Page Down key
	if(type != KT_PAGEDWN)
	{
		if((nVK == m_PageDown.nVK && strSpec == m_PageDown.strSpecKey) && m_PageDown.nVK)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_PAGEDWN_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_PAGEDWN;
		}
	}

	// Check against Up Arrow key
	if(type != KT_UPARR)
	{
		if((nVK == m_UpArrow.nVK && strSpec == m_UpArrow.strSpecKey) && m_UpArrow.nVK)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_UPARR_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_UPARR;
		}
	}

	// Check against Down Arrow key
	if(type != KT_DWNARR)
	{
		if((nVK == m_DownArrow.nVK && strSpec == m_DownArrow.strSpecKey) && m_DownArrow.nVK)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_DWNARR_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_DWNARR;
		}
	}

	// Check against Left Arrow key
	if(type != KT_LEFTARR)
	{
		if((nVK == m_LeftArrow.nVK && strSpec == m_LeftArrow.strSpecKey) && m_LeftArrow.nVK)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_LEFTARR_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_LEFTARR;
		}
	}

	// Check against Right Arrow key
	if(type != KT_RIGHTARR)
	{
		if((nVK == m_RightArrow.nVK && strSpec == m_RightArrow.strSpecKey) && m_RightArrow.nVK)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_RIGHTARR_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_RIGHTARR;
		}
	}
//WA0016
	if(type != KT_ABBREVBTN)
	{
		if((nVK == m_AbbrevBtnHK.nVK && strSpec == m_AbbrevBtnHK.strSpecKey) && m_AbbrevBtnHK.nVK)
		{
			if(bMessage)
			{
				strMessage.LoadString(IDS_ABBREVBTN_SAME);

				AfxMessageBox(strCannot + GetTypeString(type) + strMessage);
			}

			return KT_ABBREVBTN;
		}
	}
	return KT_NONE;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Gets the string related to the type.
//
// PARAMETERS	: type - Type to get the string for. It myst be one of the 
//				: KEY_TYPE enumerated values.
//
// RETURNS		: A string relating to the type
////////////////////////////////////////////////////////////////////////////////

CString CWordAidTreeDialog::GetTypeString(KEY_TYPE type)
{
	CString strType = _T("");

	switch(type)
	{
	case KT_SELECT_WORD:
		strType.LoadString(IDS_SELECT_WORD_TYPE);

		return strType;
	case KT_UNDO:
		strType.LoadString(IDS_UNDO_TYPE);

		return strType;
	case KT_SHOWLIST:
		strType.LoadString(IDS_SHOWLIST_TYPE);

		return strType;
	case KT_ABBREV_MANUAL:
		strType.LoadString(IDS_ABBREV_MANUAL_TYPE);

		return strType;
	case KT_UNEXPAND_FULL:
		strType.LoadString(IDS_UNEXPAND_FULL_TYPE);

		return strType;
	case KT_UNEXPAND_PART:
		strType.LoadString(IDS_UNEXPAND_PART_TYPE);

		return strType;
	case KT_ADD_WORD:
		strType.LoadString(IDS_ADD_WORD_TYPE);

		return strType;
	case KT_DELETE_WORD:
		strType.LoadString(IDS_DELETE_WORD_TYPE);

		return strType;
	case KT_INSERT:
		strType.LoadString(IDS_INSERT_TYPE);

		return strType;
	case KT_SAY:
		strType.LoadString(IDS_SAY_TYPE);

		return strType;
	case KT_PAGEUP:
		strType.LoadString(IDS_PAGEUP_TYPE);

		return strType;
	case KT_PAGEDWN:
		strType.LoadString(IDS_PAGEDWN_TYPE);

		return strType;
	case KT_UPARR:
		strType.LoadString(IDS_UPARR_TYPE);

		return strType;
	case KT_DWNARR:
		strType.LoadString(IDS_DWNARR_TYPE);

		return strType;
	case KT_LEFTARR:
		strType.LoadString(IDS_LEFTARR_TYPE);
		
		return strType;
	case KT_RIGHTARR:
		strType.LoadString(IDS_RIGHTARR_TYPE);

		return strType;
	case KT_ABBREVBTN:	//WA0016
		strType.LoadString(IDS_ABBREVBTN_TYPE);

		return strType;
	default:
		ASSERT(FALSE);
	}

	return _T("");

}