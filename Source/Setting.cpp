// Setting.cpp: implementation of the CSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#include <atlbase.h>			// CRegKey

#include "Setting.h"
#include "SettingStruct.h"		// Stucture with settings in
#include "SettingIndex.h"		// Has setting that should be got 
								// From registry

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSetting::CSetting()
{
	LoadSettings();
}

//////////////////////////////////////////////////////////////////////

CSetting::~CSetting()
{
	// Make sure settings changes have been stored to the registry
	StoreSettings();

	m_SettingArray.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Loads all the setting from the registry.  Uses	//
//				: the SettingIndex.h header to know which registry  //
//				: values to get.									//
//				:													//
// RETURNS		: TRUE if successfull.								//
//////////////////////////////////////////////////////////////////////

BOOL CSetting::LoadSettings()
{
	BOOL bReturn = TRUE;
	CString szKey = _T("");
	CString szValue = _T("");

	m_SettingArray.RemoveAll();		// WA0045

	for(int i = 0; i < SETTING_MAX; i++)
	{
		szKey = KeyArray[i].szKey;

		szValue = CSetting::GetSettingFromReg(SETTING_PATH, szKey, KeyArray[i].szValue);
		
		m_SettingArray.Add(szValue);
	}

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Get a value from the registry. If the value is 	//
//				: in the registry the default value is added for	//
//				: key.												//
//				:													//
// PARAMETERS	: szPath - the path to the registry value from.     //
//				: szKey - the key to get the value from.			//
//				: szDefValue - the default value for the key.		//
//				:													//
// RETURNS		: The value from the registry; otherwise blank.		//
//////////////////////////////////////////////////////////////////////

CString CSetting::GetSettingFromReg(CString szPath, CString szKey,
									CString szDefValue /*= _T("")*/)
{
	CRegKey reg;
	CString szValue = _T("");
	unsigned long lSize = _MAX_PATH;

	if(reg.Create(HKEY_CURRENT_USER, szPath) ==
		ERROR_SUCCESS)
	{
		// Get value from registry
		if(reg.QueryValue(szValue.GetBuffer(lSize), szKey, &lSize)
			== ERROR_SUCCESS)
		{
			szValue.ReleaseBuffer();
		}
		else	// Key not found add default and return it as value
		{
			szValue.ReleaseBuffer();

			if(SetSettingInReg(szPath, szKey, szDefValue))
			{
				szValue = szDefValue;
			}
		}
		reg.Close();
	}

	return szValue;
}

//////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Places a setting value into the registry key.		//
//				:													//
// PARAMETERS	: szPath - the path to the registry value from.     //
//				: szKey - to key to store the value against.		//
//				: szValue - the value to store against the key.		//
//				:													//
// RETURNS		: TRUE if the key was succesfully stored in the		//
//				: registry.											//
//////////////////////////////////////////////////////////////////////

BOOL CSetting::SetSettingInReg(CString szPath, CString szKey, 
								 CString szValue)
{
	BOOL bReturn = TRUE;
	CRegKey reg;
	
	if(reg.Create(HKEY_CURRENT_USER, szPath) ==	ERROR_SUCCESS)
	{
		if(reg.SetValue(szValue, szKey) != ERROR_SUCCESS)
		{
#ifdef _DEBUG
			AfxMessageBox(_T("Failed to store configuration filename"));
#endif
			bReturn = FALSE;
		}
		reg.Close();
	}
	else
	{
		bReturn = FALSE;
	}
	return bReturn;
}

//////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Gets the setting from the array.					//
//				:													//
// PARAMETERS	: nIndex - the index of the value that is wanted.   //
//				:													//
// REUTURNS		: The Value from registry.							//
//////////////////////////////////////////////////////////////////////

CString CSetting::GetSetting(int nIndex)
{
	ASSERT(nIndex <= m_SettingArray.GetSize());
	
	return m_SettingArray.GetAt(nIndex);;
}

//////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Get a settings and formats it as a COLORREF.	    //
//				:													//
// PARAMETERS	: nIndex : The setting to get						//
//				:													//
// RETURNS		: Setting as a COLORREF								//
//////////////////////////////////////////////////////////////////////

COLORREF CSetting::GetColourSetting(int nIndex)
{
	int nRGB[3] = {0,0,0};
	int nPos = 0;
	CString szText = GetSetting(nIndex);

	// Format into COLORREF
	for(int i = 0; i < 3; i++)
	{
		nPos = szText.Find(_T(","));

		if(nPos != -1)
		{
			nRGB[i] = atoi(szText.Left(nPos));

			// Remove the colour from string so next can be got
			szText = szText.Right((szText.GetLength() - (nPos+1)));
		}
		else
		{
			nRGB[i] = atoi(szText);
		}
	}

	return COLORREF(RGB(nRGB[0], nRGB[1], nRGB[2]));
}

//////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Replaces a setting in the setting array.			//
//				:													//
// PARAMETERS	: nIndex - the setting to replace.					//
//				: szSetting - the setting value to replace with.	//
//				:													//
// RETURNS		: TRUE if successfull.								//
//////////////////////////////////////////////////////////////////////

BOOL CSetting::SetSetting(int nIndex, CString szSetting)
{

	// Make sure that index is valid
	ASSERT(nIndex < m_SettingArray.GetSize());

	//m_SettingArray.RemoveAt(nIndex);

	// Put back in array if not the last setting otherwise add to
	// tail
	if(nIndex < m_SettingArray.GetSize()-1)
	{
		m_SettingArray.SetAt(nIndex, szSetting);
	}
	else
	{
		m_SettingArray.Add(szSetting);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Stores the settings to the registry.				//
//				:													//
// RETURNS		: TRUE if all the settings were successfully stored //
//				: in the registry; otherwise FALSE if one failed to //
//				: be stored.										//
//////////////////////////////////////////////////////////////////////

BOOL CSetting::StoreSettings()
{
	BOOL bReturn = TRUE;
	CString szKey = _T("");
	CString szValue = _T("");

	for(int i = 0; i < SETTING_MAX; i++)
	{
		szKey = KeyArray[i].szKey;

		if(!SetSettingInReg(SETTING_PATH, szKey, m_SettingArray.GetAt(i)))
		{
			bReturn = FALSE;
#ifdef _DEBUG
			CString szMessage = _T("");
			szMessage.Format(_T("%s could not be saved to registry"), KeyArray[i].szValue);

			AfxMessageBox(szMessage);
#endif
		}	
	}

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

BOOL CSetting::SetColourSetting(int nIndex, COLORREF colour)
{
	CString szSetting = _T("");

	szSetting.Format(_T("%i,%i,%i"), GetRValue(colour), GetGValue(colour), GetBValue(colour));

	return SetSetting(nIndex, szSetting);


}

BOOL CSetting::SaveToTextFile(CString szFilename)
{
	BOOL bReturn = TRUE;
	CString szKey = _T("");
	CString szValue = _T("");

	CStdioFile file;
	if (!file.Open(szFilename,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite,0))
		bReturn = FALSE;

	for (int i = 0; i<SETTING_MAX && bReturn; i++)
	{
		szKey = KeyArray[i].szKey;
		file.WriteString((const char*)m_SettingArray.GetAt(i));
		file.WriteString("\n");
			

	}
	if (file)
		file.Close();

	return bReturn;
}

BOOL CSetting::LoadFromTextFile(CString szFilename)
{
	BOOL bReturn = TRUE;
	CString szKey = _T("");
	CString szValue = _T("");
	CStdioFile file;
	
	if (!file.Open(szFilename,CFile::modeRead|CFile::typeText,0))
		bReturn = FALSE;
	
	for (int i =0; i<SETTING_MAX && bReturn;i++)
	{
		szKey = KeyArray[i].szKey;

		bReturn = file.ReadString(szValue);
		if (bReturn)
			m_SettingArray.SetAt(i,szValue);
		
	}
	
	if (file)
		file.Close();

	return TRUE;
}