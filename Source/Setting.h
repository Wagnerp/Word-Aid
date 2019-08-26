// Setting.h: interface for the CSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTING_H__F04D46C1_295A_11D6_92B8_BD08A0215871__INCLUDED_)
#define AFX_SETTING_H__F04D46C1_295A_11D6_92B8_BD08A0215871__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "SettingIndex.h"


class CSetting  
{
public:
	CSetting();
	virtual ~CSetting();

	
	BOOL LoadSettings();
	BOOL StoreSettings();

	BOOL	 SetSetting(int nIndex, CString szSetting);
	BOOL	 SetColourSetting(int nIndex, COLORREF colour);
	CString  GetSetting(int nIndex);
	COLORREF GetColourSetting(int nIndex);
	
	BOOL SaveToTextFile(CString szFilename);
	BOOL LoadFromTextFile(CString szFilename);

protected:
	CArray<CString, CString&> m_SettingArray;	// Used to store setting from reg

	CString GetSettingFromReg(CString szPath, CString szKey, CString szDefValue = _T(""));
	BOOL    SetSettingInReg(CString szPath, CString szKey, CString szValue);

};

#endif // !defined(AFX_SETTING_H__F04D46C1_295A_11D6_92B8_BD08A0215871__INCLUDED_)
