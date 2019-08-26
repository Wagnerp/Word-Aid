// Word Aid 2.cpp : Defines the class behaviors for the application.
//
//////////////////////////////////////////////////////////////////////////////////////
//																					//
//	WA0003 - changes to fix problem with Abbrev dialogue boxes displaying off		//
//			screen.																	//
//																					//
//////////////////////////////////////////////////////////////////////////////////////
//																					//

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>	

#include "stdafx.h"
#include "wordaid2.h"
#include "Word Aid 2Dlg.h"
#include "settingindex.h"

#include "Keyhook.h"		// To get name of Keyhook

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWordAid2App

BEGIN_MESSAGE_MAP(CWordAid2App, CWinApp)
	//{{AFX_MSG_MAP(CWordAid2App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWordAid2App construction

CWordAid2App::CWordAid2App()
{
	// Place all significant initialization in InitInstance
}

CWordAid2App::~CWordAid2App()
{
	if (m_pWordAid)
	{
		m_pWordAid->m_AlphaList.RemoveAll();
		m_pWordAid->m_TopicList.RemoveAll();
	//	m_pWordAid->m_AbbrevList->RemoveAll();
	/*	if (m_pWordAid->m_AbbrevList->m_pAbbrevEditDlg)
		{
			m_pWordAid->m_AbbrevList->m_pAbbrevEditDlg->DestroyWindow();
			delete m_pWordAid->m_AbbrevList->m_pAbbrevEditDlg;
		}
	*/
		m_pWordAid->DestroyWindow();
		delete m_pWordAid;
	}
	if (m_pPopUp)
	{
		m_pPopUp->DestroyWindow();
		delete m_pPopUp;
	}
	if (m_pDummy)
	{
		m_pDummy->DestroyWindow();
		delete m_pDummy;
	}

	_CrtDumpMemoryLeaks();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWordAid2App object

CWordAid2App theApp;

/////////////////////////////////////////////////////////////////////////////
// CWordAid2App initialization

BOOL CWordAid2App::InitInstance()
{
	AfxOleInit();
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// If Keyhook.dll is missing then stop application
	if(LoadLibrary(KEYHOOK_DLL) == NULL)
	{
		CString strError = _T("");

		strError.LoadString(IDS_NO_KEYHOOK_DLL);

		AfxMessageBox(strError, MB_OK | MB_ICONSTOP);

		return FALSE;
	}
	
	m_pDummy = new CDummyDlg();

	m_pDummy->Create(IDD_DUMMY_DLG, NULL);

	ASSERT(m_pDummy);
	m_pMainWnd = m_pDummy;

	// Create Wordaid dialog
	m_pWordAid = new CWordAid2Dlg();

	ASSERT(m_pWordAid);		// Check that pointer is valid

	m_pWordAid->Create(IDD_WORDAID2_DIALOG, NULL);

	m_pWordAid->StayActive();	// Make WordAid Dialog stay active
	// Set the main window to be the word aid window
	
	// Create the child window
	m_pPopUp = new CChildWnd();

	ASSERT(m_pPopUp);		// Check the pointer is valid
	m_pPopUp->m_pSettings = &(m_pWordAid->m_Settings);

	if(!m_pPopUp->Create(m_pDummy->m_hWnd))
	{
#ifdef _DEBUG
		AfxMessageBox(_T("Popup Failed to load"));
#endif
	}

	m_pWordAid->StayActive(FALSE);  // Make WordAid Dialog stay active
	m_pWordAid->SetFocus();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//WA0047
//BOOL CWordAid2App::SwitchToPopup(CWordlist alphalist, CWordlist topiclist, CAbbrevList* abbrevList, BOOL bSwitch /*= TRUE*/)
BOOL CWordAid2App::SwitchToPopup(CWordlist alphalist, CWordlist topiclist, CAbbrevList abbrevList, BOOL bSwitch /*= TRUE*/)
{
	if(bSwitch)
	{
		m_pMainWnd = m_pPopUp;
		m_pWordAid->ShowWindow(SW_HIDE);

		// WA0047 not required as Dialog is now going to work from outside of the class
		//if (abbrevList->m_pAbbrevEditDlg.IsVisible())				//WA0003 WA0031
	//	{
		//	abbrevList->m_pAbbrevEditDlg->m_bEditMode = TRUE;		//WA0003
		//	abbrevList->m_pAbbrevEditDlg.OnBnClickedCancel();		//WA0003 WA0031
			//abbrevList->m_pAbbrevEditDlg->ShowWindow(SW_HIDE);	//WA0003
		//}

		if(!atoi(m_pWordAid->m_Settings.GetSetting(ALPHA_LIST_HIDE)))
		{
			m_pPopUp->SetWordlistA(alphalist);
		}
		else
		{
			CWordlist list;

			m_pPopUp->SetWordlistA(list);
		}

		if(!atoi(m_pWordAid->m_Settings.GetSetting(TOPIC_LIST_HIDE)))
		{
			m_pPopUp->SetWordlistT(topiclist);
		}
		else
		{
			CWordlist list;

			m_pPopUp->SetWordlistT(list);
		}

		if(!atoi(m_pWordAid->m_Settings.GetSetting(ABBREV_LIST_HIDE)))
		{
			m_pPopUp->SetAbbrevList(abbrevList);
		}
		else
		{
			//m_pPopUp->SetAbbrevList(NULL);		WA0047
		}

		m_pPopUp->ShowWindow(SW_SHOW);
		// if wordlist window should be minimized when running
		if (m_pPopUp->m_pSettings->GetSetting(STARTUP_MINIMIZE) == "1")
		{
			/*m_pPopUp->ShowWindow(SW_HIDE);
			m_pPopUp->SetVisible(TRUE);
			m_pPopUp->SetMonitorsHiddenWnd();*/
			m_pPopUp->SetWindowPos(&CWnd::wndTop,0,0,1,1,SWP_HIDEWINDOW);
		}
		
		// check if keyhook already loaded
		if (m_pPopUp->KeyhookLoaded())
			m_pPopUp->EnableKeyhook();
	}
	else
	{
		m_pWordAid->StayActive();			// Added so WordAid stays active
		m_pMainWnd = m_pWordAid;
		m_pWordAid->ShowWindow(SW_SHOW);
		m_pPopUp->ShowWindow(SW_HIDE);
		m_pPopUp->DisableKeyhook();

		if(!atoi(m_pWordAid->m_Settings.GetSetting(ALPHA_LIST_HIDE)))
		{
			m_pWordAid->m_AlphaList = m_pPopUp->GetWordlistA();
		}

		if(!atoi(m_pWordAid->m_Settings.GetSetting(TOPIC_LIST_HIDE)))
		{
			m_pWordAid->m_TopicList = m_pPopUp->GetWordlistT();
		}

		if(!atoi(m_pWordAid->m_Settings.GetSetting(ABBREV_LIST_HIDE)))
		{
			m_pWordAid->m_AbbrevList = m_pPopUp->GetAbbrevList();
		}

		m_pWordAid->StayActive(FALSE);		// Added so WordAid stays active
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CWordAid2App::DisplaySettingPopup(BOOL bDisplay /*= TRUE*/)
{
/*	if(bDisplay)
	{
		CWordlist list;
		list.Add("Word 1");
		list.Add("Word 2");
		m_pPopUp->SetWordlistT(list);
		m_pPopUp->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pPopUp->ShowWindow(SW_HIDE);
	}*/
}
