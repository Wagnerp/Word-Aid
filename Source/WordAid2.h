// Word Aid 2.h : main header file for the WORD AID 2 application
//

#if !defined(AFX_WORDAID2_H__E212C0C5_23DE_11D6_92B8_A495EED9BA71__INCLUDED_)
#define AFX_WORDAID2_H__E212C0C5_23DE_11D6_92B8_A495EED9BA71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "ChildWnd.h"		// For Popup Window
#include "Word Aid 2Dlg.h"	// For WordAid dialog
#include "DummyDlg.h"		// For Dummy dialog

/////////////////////////////////////////////////////////////////////////////
// CWordAid2App:
// See Word Aid 2.cpp for the implementation of this class
//

class CWordAid2App : public CWinApp
{
public:
	CWordAid2App();
	~CWordAid2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWordAid2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL
	
//	BOOL SwitchToPopup(CWordlist alphalist,CWordlist topiclist, CAbbrevList* abbrevList, BOOL bSwitch = TRUE);	WA0047
	BOOL SwitchToPopup(CWordlist alphalist,CWordlist topiclist, CAbbrevList abbrevList, BOOL bSwitch = TRUE);
	void DisplaySettingPopup(BOOL bDisplay = TRUE);
	CWordAid2Dlg* GetWordAidWindow() { return m_pWordAid; }
	
	CDummyDlg* m_pDummy;			// Pointer to the Dummy dialog
	CChildWnd* m_pPopUp;			// Pointer for popup window
	CWordAid2Dlg* m_pWordAid;		// Pointer to wordaid dialog

protected:
	
	
// Implementation

	//{{AFX_MSG(CWordAid2App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORDAID2_H__E212C0C5_23DE_11D6_92B8_A495EED9BA71__INCLUDED_)
