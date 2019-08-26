// Word Aid 2Dlg.cpp : implementation file
//////////////////////////////////////////////////////////////////////////////////////
//																					//
//	WA0002	- changes to allow abbreviation lists to be sorted alphabetically on	//
//			abbreviation															//
//																					//
//////////////////////////////////////////////////////////////////////////////////////
//																					//
//	WA0003 - changes to fix problem with Abbrev dialogue boxes displaying off		//
//			screen.																	//
//																					//
//////////////////////////////////////////////////////////////////////////////////////
//																				//

#include "stdafx.h"
#include "wordaid2.h"
#include "Word Aid 2Dlg.h"
//#include "settingsdlg.h"
#include "SettingIndex.h"		// Has index for Settings
#include "ButtonSettings.h"		// Popup Setting Page
#include "WordSettings.h"		// Label Settings page
#include "KeySettingsPage.h"	// Key Settings page
#include "GeneralSettings.h"	// General settings page
#include "WordAidTreeDialog.h"			// Settings dialog
#include "speechsettingsdlg.h"	// voice setup dialog page
#include "speechsetupdlg.h"		// speech setup dialog page
#include "SettingIndex.h"		// Has index for Settings
#include "AbbrevExpansion.h"	// Abbrev
#include "WordAidSettingsDlg.h"	// Word Aid settings

// dialogue classes for merge file Wizard
#include "mergeintodlg.h"
#include "mergefileselectdlg.h"
#include "mergeselectworddlg.h"
#include "mergetypedlg.h"

#include "NewListDlg.h"			// Select which list to create

#ifdef _SWED_DEMO
#include "ProgressWnd.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define TAB_ON		_T("1")

const CString APP_NAME_TXT	= _T("WordAid2");

#ifdef _DEMO
const CString VERSION_TXT	= _T("Ver 2.0.8 Demo");
#else
const CString VERSION_TXT = _T("Ver 2.0.8.11");
#endif

const int ITEM_BUFFER_SIZE	= 256;

// ToolBar Struct
static UINT auIDToolBar[] = {
   ID_FILE_NEWWORDLIST,
   ID_FILE_OPEN,
   ID_FILE_SAVE,
   ID_SEPARATOR,
   ID_EDIT_ADDWORD,
   ID_EDIT_EDITWORD,
   ID_EDIT_DELETEWORD,
   ID_SEPARATOR,
   57668
};

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWordAid2Dlg dialog

CWordAid2Dlg::CWordAid2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWordAid2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWordAid2Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_hIcon				= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_szAlphaFilename	= _T("");
	m_szTopicFilename   = _T("");
	m_szAbbrevFilename	= _T("");
	m_bStayActive = FALSE;
	m_bNewFile = FALSE;
	m_hAccelerator = NULL;

//	m_AbbrevList = new CAbbrevList();	WA0047

//	m_pTabImageList = new CImageList();
//	m_pTabImageList->Create(16, 16, ILC_COLOR8, 0, 4);

//	CBitmap bmpAlpha, bmpTopic, bmpAbbrev;
//	bmpAlpha.LoadBitmap(IDB_ALPHA_LIST);
//	bmpTopic.LoadBitmap(IDB_TOPIC_LIST);
//	bmpAbbrev.LoadBitmap(IDB_ABBREV_LIST);

//	m_pTabImageList->Add(&bmpAlpha, RGB(0,0,0));

//	m_pTabImageList->Add(&bmpTopic, RGB(0,0,0));
//	m_pTabImageList->Add(&bmpAbbrev, RGB(0,0,0));
}

/////////////////////////////////////////////////////////////////////////////

CWordAid2Dlg::~CWordAid2Dlg()
{
	m_AlphaList.RemoveAll();
	m_TopicList.RemoveAll();
	DestroyIcon(m_hIcon);

/*	if(m_AbbrevList)
	{
		m_AbbrevList.RemoveAll();
		delete m_AbbrevList;
	}	*/	//WA0047

	if (m_ctrlSpeaker)
	{
		m_ctrlSpeaker.DestroyWindow();
	}

//	m_pTabImageList->Detach();
//	delete m_pTabImageList;
}

/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWordAid2Dlg)
	DDX_Control(pDX, IDC_LIST_WORDLIST, m_wordlist);
	DDX_Control(pDX, IDC_TAB_WORDLIST, m_tab_wordlist);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CHK_HIDE_LIST, m_chkHideList);
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CWordAid2Dlg, CDialog)
	//{{AFX_MSG_MAP(CWordAid2Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_OPENALPHABETIC, OnFileOpenalphabetic)
	ON_COMMAND(ID_FILE_OPENTOPIC, OnFileOpentopic)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_WORDLIST, OnSelchangeTabWordlist)
	ON_COMMAND(ID_EDIT_ADDWORD, OnEditAddword)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_EDIT_EDITWORD, OnEditEditword)
	ON_BN_CLICKED(IDC_BTN_SORT_ALPHA, OnBtnSortAlpha)
	ON_COMMAND(ID_SETTINGS_POPUPSETTINGS, OnSettingsPopupsettings)
	ON_COMMAND(ID_EDIT_DELETEWORD, OnEditDeleteword)
	ON_COMMAND(ID_SAVESETTINGS, OnSavesettings)
	ON_COMMAND(ID_LOADSETTINGS, OnLoadsettings)
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_FILE_NEWWORDLIST, OnFileNewwordlist)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_DEFAULT_HELP, OnDefaultHelp)
	ON_COMMAND(ID_MERGE_FILES, OnMergeFiles)
	ON_WM_VKEYTOITEM()
	ON_WM_CHARTOITEM()
	ON_COMMAND(ID_SETTINGS_TOOLBAR, OnSettingsToolbar)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS_TOOLBAR, OnUpdateSettingsToolbar)
	ON_LBN_DBLCLK(IDC_LIST_WORDLIST, OnDblclkListWordlist)
	ON_COMMAND(ID_SETTINGS_LEXICON, OnSettingsLexicon)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_OPENABBREV, OnFileOpenAbbrev)
	ON_BN_CLICKED(IDC_CHK_HIDE_LIST, OnClickHideList)
	ON_COMMAND(ID_HELP_INDEX, CWnd::OnHelpIndex)

//	ON_COMMAND(ID_SETTINGS_TOOLBAR, OnSettingsToolbar)
//	ON_COMMAND(ID_MAIN_TOOLBAR, OnSettingsToolbar)
//	ON_UPDATE_COMMAND_UI(ID_SETTINGS_TOOLBAR, OnUpdateSettingsToolbar)
//	ON_UPDATE_COMMAND_UI(ID_MAIN_TOOLBAR, OnUpdateSettingsToolbar)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWordAid2Dlg message handlers

BOOL CWordAid2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	////////////////////////////////////////////////////////////////////////

	// Load Accelerator Table
	m_hAccelerator = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// Create toolbar at the top of the dialog window
	if (m_toolBar.Create(this))
	{
//		m_toolBar.LoadBitmap(IDR_MAIN_TOOLBAR);
		m_toolBar.LoadToolBar(IDR_MAIN_TOOLBAR);
//		m_toolBar.SetButtons(auIDToolBar, sizeof(auIDToolBar)/sizeof(UINT));
	}

	m_toolBar.SetBarStyle(m_toolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_TOP |	TBSTYLE_FLAT);
	
	// to allow tooltips to be displayed
	m_toolBar.EnableToolTips();

	// We need to resize the dialog to make room for control bars.
	// First, figure out how big the control bars are.
	CRect rcClientStart;
	CRect rcClientNow;
	GetClientRect(rcClientStart);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST,
				   0, reposQuery, rcClientNow);

	// Now move all the controls so they are in the same relative
	// position within the remaining client area as they would be
	// with no control bars.
	CPoint ptOffset(rcClientNow.left - rcClientStart.left,
					rcClientNow.top - rcClientStart.top);

	CRect  rcChild;
	CWnd* pwndChild = GetWindow(GW_CHILD);
	while (pwndChild)
	{
		pwndChild->GetWindowRect(rcChild);
		ScreenToClient(rcChild);
		rcChild.OffsetRect(ptOffset);
		pwndChild->MoveWindow(rcChild, FALSE);
		pwndChild = pwndChild->GetNextWindow();
	}

	// Adjust the dialog window dimensions
	CRect rcWindow;
	GetWindowRect(rcWindow);
	rcWindow.right += rcClientStart.Width() - rcClientNow.Width();
	rcWindow.bottom += rcClientStart.Height() - rcClientNow.Height();
	MoveWindow(rcWindow, FALSE);

	// And position the control bars
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);


	AddTabs();

	// If Text speaker could np be created then Disable speech menu items
	if(m_ctrlSpeaker.Create("Speaker",WS_CHILD,CRect(0,0,1,1),this,2) == 0)
	{
		CMenu* pFileMenu = GetSubMenu(32782);

		if(pFileMenu)
		{
			pFileMenu->EnableMenuItem(ID_SETTINGS_LEXICON, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		if(nID == SC_MINIMIZE)		// Dialog is minimized
		{
			if (!m_bStayActive)
			{// Switch to the popup
				CWordAid2App* pApp = (CWordAid2App*)AfxGetApp();
				m_bStayActive = TRUE;
				pApp->SwitchToPopup(m_AlphaList, m_TopicList, m_AbbrevList);
			}

		}
		else if(nID == SC_RESTORE)		// Window is restored
		{
			// Switch to the Wordaid dialog
			CWordAid2App* pApp = (CWordAid2App*)AfxGetApp();
			m_bStayActive = FALSE;

			pApp->SwitchToPopup(m_AlphaList,m_TopicList, m_AbbrevList, FALSE);
			CString strAbbrev = _T("");
			CString strAlpha = _T("");
			CString strTopic = _T("");
			TCITEM item;
			CString pstrString = _T("");

			item.mask = TCIF_TEXT;
			item.pszText = pstrString.GetBuffer(ITEM_BUFFER_SIZE);
			item.cchTextMax = ITEM_BUFFER_SIZE;

			strAbbrev.LoadString(IDS_ABBREV_WORDLIST);
			strAlpha.LoadString(IDS_ALPHA_WORDLIST);
			strTopic.LoadString(IDS_TOPIC_WORDLIST);

			// Get which tab has been selected
			int nTab = m_tab_wordlist.GetCurSel();

			if(m_tab_wordlist.GetItem(nTab, &item))
			{
				if(item.pszText == strAlpha)
				{
					DisplayAlphaList();
				}
				else if(item.pszText == strTopic)
				{
					DisplayTopicList();
				}
				if(item.pszText == strAbbrev)
				{
					DisplayAbbrevList();
				}
			}
	
//	if (m_AlphaList.m_bModified)
//		DisplayAlphaList();
//	if (m_TopicList.m_bModified)
//		DisplayTopicList();
//
//	m_tab_wordlist.SetCurSel(nTab);
		}

		CDialog::OnSysCommand(nID, lParam);

	}
}

/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnDestroy()
{
	WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWordAid2Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.

HCURSOR CWordAid2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Version info
	CStatic* lblVersion = (CStatic*)GetDlgItem(IDC_LBL_VERSION);

	CString szVersion = _T("");
	szVersion.Format(_T("%s %s"), APP_NAME_TXT, VERSION_TXT);
	lblVersion->SetWindowText(szVersion);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Adds the tabs to dialog depending upon registry setting.	//
//				:															//
// RETURNS		: TRUE if successfull.										//
//////////////////////////////////////////////////////////////////////////////

BOOL CWordAid2Dlg::AddTabs()
{
	BOOL bReturn = TRUE;
	CString szWordAid = _T("");
	CString szAbbrev = _T("");
	CString szTitle = _T("");
	int nIndex = 0;
	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT|TCIF_IMAGE;

	// Remove all tabs
	m_tab_wordlist.DeleteAllItems();

	// Add Image list for Tabs
//	m_tab_wordlist.SetImageList(m_pTabImageList);

	// Get whether WordAid is on
//	m_Settings.GetSetting(WORDAID_ON, szWordAid);
	szWordAid = m_Settings.GetSetting(WORDAID_ON);

	if(szWordAid == TAB_ON)
	{
		// TODO : Add image for Tab
		szTitle.LoadString(IDS_TOPIC_WORDLIST);
		tcItem.pszText	= szTitle.GetBuffer(szTitle.GetLength());
		
		if(m_tab_wordlist.InsertItem(nIndex++, szTitle.GetBuffer(szTitle.GetLength()), 1) < 0)
		//if(m_tab_wordlist.InsertItem(nIndex++, &tcItem) < 0)
		{
			bReturn = FALSE;
		}
		//get default topic file name
		CString szTopicFile = _T(" ");
		//m_Settings.GetSetting(TOPIC_FILE,szTopicFile);
		m_szTopicFilename = m_Settings.GetSetting(TOPIC_FILENAME);

#ifndef _DEMO
		// WA0029
		if(	m_TopicList.GetFilename() == m_szTopicFilename)
		{
			DisplayTopicList();
			SelectTab(szTitle);
		}	//// WA0029
#endif
#ifdef _DEMO
		if(LoadDemoTopicList())		// WA0029
#else
		else if(m_TopicList.LoadWordlist(m_szTopicFilename, TOPIC))		// WA0029
#endif
		{
			SetWindowTitle(m_szTopicFilename);
			DisplayTopicList();
			SelectTab(szTitle);
			//m_tab_wordlist.SetCurSel(0);
		}
		// Add Alphabetic Wordlist tab
		szTitle.LoadString(IDS_ALPHA_WORDLIST);
		tcItem.pszText	= szTitle.GetBuffer(szTitle.GetLength());

		if(m_tab_wordlist.InsertItem(nIndex++, szTitle.GetBuffer(szTitle.GetLength()), 0) < 0)
		//if(m_tab_wordlist.InsertItem(nIndex++, &tcItem) < 0)
		{
			bReturn = FALSE;
		}
		CString szAlphaFile = _T(" ");
		//get default alpha file name
		//m_Settings.GetSetting(ALPHA_FILE,szAlphaFile);
		//display default alpha list
		m_szAlphaFilename = m_Settings.GetSetting(ALPHA_FILENAME);

		// // WA0029
#ifndef _DEMO
		if(m_AlphaList.GetFilename() == m_szAlphaFilename)
		{
			DisplayAlphaList();

			// Set tab
			SelectTab(szTitle);
		}			// WA0029
#endif
#ifdef _DEMO
		if(LoadDemoAlphaList())		// WA0029
#else
		else if(m_AlphaList.LoadWordlist(m_szAlphaFilename, ALPHA))		// WA0029
#endif
		{
			SetWindowTitle(m_szAlphaFilename);
//			m_szAlphaFilename = fileDlg.GetPathName();

			DisplayAlphaList();

			// Set tab
			SelectTab(szTitle);
			//m_tab_wordlist.SetCurSel(1);

		// Add Topic Wordlist tab

		}

		// Add WordAid Open Topic etc to File... Menu
		AddWordAidMenus();
		SetWordAidEditText();
	}
	else
	{
		RemoveWordAidMenuItems();
		m_TopicList.RemoveAll();
		m_AlphaList.RemoveAll();
	}
	
	// Get whether Abbrev is on
//	m_Settings.GetSetting(ABBREV_ON, szAbbrev);

	szAbbrev = m_Settings.GetSetting(ABBREV_ON);
	if(szAbbrev == TAB_ON)
	{
		// TODO : add image for tab
		// Add Abbreviation Wordlist
		szTitle.LoadString(IDS_ABBREV_WORDLIST);
		tcItem.pszText	= szTitle.GetBuffer(szTitle.GetLength());

		if(m_tab_wordlist.InsertItem(nIndex++, szTitle.GetBuffer(szTitle.GetLength()), 2) < 0)
		//if(m_tab_wordlist.InsertItem(nIndex++, &tcItem) < 0)
		{
			bReturn = FALSE;
		}

		AddAbbrevMenuItems();
		SetWordAidEditText(FALSE);

		// Load last Opened file
		m_szAbbrevFilename = m_Settings.GetSetting(ABBREV_FILENAME);

		// WA0029
#ifndef _DEMO
		if(m_AbbrevList.GetFilename() == m_szAbbrevFilename)
		{
			DisplayAbbrevList();
			SelectTab(szTitle);
		}	// WA0029
#endif
#ifdef _DEMO
		if(LoadDemoAbbrevList())	// WA0029
#else
		else if(m_AbbrevList.LoadWordlist(m_szAbbrevFilename))		// WA0029
#endif
		{
			SetWindowTitle(m_szAbbrevFilename);
			DisplayAbbrevList();
			SelectTab(szTitle);
		}
	}
	else
	{
		RemoveAbbrevMenuItem();
		m_AbbrevList.RemoveAll();
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Enables the Open Abbrev to the File menu.
////////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::AddAbbrevMenuItems()
{
	CMenu *pFileMenu = GetSubMenu(ID_FILE_NEWWORDLIST);

	if(pFileMenu)
	{
		pFileMenu->EnableMenuItem(ID_FILE_OPENABBREV, MF_BYCOMMAND | MF_ENABLED);
		// REMOVED Menu Item will now be enabled or disabled
		/*CString szAbbrev = _T("");

		szAbbrev.LoadString(IDS_MENU_OPEN_ABBREV);

		InsertInMenu(pFileMenu, szAbbrev, ID_FILE_OPENABBREV, ID_FILE_OPEN);*/
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Disables the Open Abbrev menu item.
////////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::RemoveAbbrevMenuItem()
{
	CMenu* pFileMenu = GetSubMenu(ID_FILE_NEWWORDLIST);

	if(pFileMenu)
	{
		pFileMenu->EnableMenuItem(ID_FILE_OPENABBREV, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		// REMOVED Menu Item will now be enabled or disabled
		//RemoveMenuItem(pFileMenu, ID_FILE_OPENABBREV);
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Remvoe the Open Alphabeitc and Open Topic menu items.
////////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::RemoveWordAidMenuItems()
{
	CMenu* pFileMenu = GetSubMenu(ID_FILE_NEWWORDLIST);

	if(pFileMenu)
	{
		pFileMenu->EnableMenuItem(ID_FILE_OPENALPHABETIC, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pFileMenu->EnableMenuItem(ID_FILE_OPENTOPIC, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

		// REMOVED Menu Item will now be enabled or disabled
		//RemoveMenuItem(pFileMenu, ID_FILE_OPENALPHABETIC);
		//RemoveMenuItem(pFileMenu, ID_FILE_OPENTOPIC);
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Inserts the Open Alphabetic and Open Topic to the File menu.
////////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::AddWordAidMenus()
{
	CMenu* pFileMenu = GetSubMenu(ID_FILE_NEWWORDLIST);

	if(pFileMenu)
	{
		pFileMenu->EnableMenuItem(ID_FILE_OPENALPHABETIC, MF_BYCOMMAND | MF_ENABLED);
		pFileMenu->EnableMenuItem(ID_FILE_OPENTOPIC, MF_BYCOMMAND | MF_ENABLED);

		// REMOVED Menu Item will now be enabled or disabled
		/*CString szTopic = _T("");
		CString szAlpha = _T("");

		szAlpha.LoadString(IDS_MENU_OPEN_ALPHA);
		szTopic.LoadString(IDS_MENU_OPEN_TOPIC);
		
		if(atoi(m_Settings.GetSetting(ABBREV_ON)))	// Place below Abbrev if On
		{
			InsertInMenu(pFileMenu, szAlpha, ID_FILE_OPENALPHABETIC, ID_FILE_OPENABBREV);
			InsertInMenu(pFileMenu, szTopic, ID_FILE_OPENTOPIC, ID_FILE_OPENALPHABETIC);
		}
		else
		{
			InsertInMenu(pFileMenu, szAlpha, ID_FILE_OPENALPHABETIC, ID_FILE_OPEN);
			InsertInMenu(pFileMenu, szTopic, ID_FILE_OPENTOPIC, ID_FILE_OPENALPHABETIC);
		}*/
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Set the Edit Menus Text Depedening upon the list shown.
//
// PARAMETERS	: bWordAId - TRUE to Display Word Aid text FALSE for Abbrev
////////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::SetWordAidEditText(BOOL bWordAid /*= TRUE*/)
{
	CString szAdd = _T("");
	CString szEdit = _T("");
	CString szDelete = _T("");

	CMenu* pFileMenu = GetSubMenu(ID_EDIT_ADDWORD);

	if(bWordAid)
	{
		szAdd.LoadString(IDS_WORDAID_EDIT_ADD);
		szEdit.LoadString(IDS_WORDAID_EDIT_EDIT);
		szDelete.LoadString(IDS_WORDAID_EDIT_DELETE);
	}
	else
	{
		szAdd.LoadString(IDS_ABBREV_EDIT_ADD);
		szEdit.LoadString(IDS_ABBREV_EDIT_EDIT);
		szDelete.LoadString(IDS_ABBREV_EDIT_DELETE);
	}

	ModifyMenuText(pFileMenu, ID_EDIT_ADDWORD, szAdd);
	ModifyMenuText(pFileMenu, ID_EDIT_EDITWORD, szEdit);
	ModifyMenuText(pFileMenu, ID_EDIT_DELETEWORD, szDelete);

}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Gets a sub menu from the main application menu depeding upon
//				: the ID of the item sent to it.
//
// PARAMETERS	: nFirstID - ID of the first Menu Item in the sub menu that is
//				: to be returned.
//
// RETURNS		: The Sub Menu if found; Otherwise NULL
////////////////////////////////////////////////////////////////////////////////

CMenu* CWordAid2Dlg::GetSubMenu(UINT nFirstID)
{
	// Locate the File submenu
	CMenu* pSubMenu = NULL;
	
	CMenu* pTopMenu = this->GetMenu();
	int iPos;
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
	{
		CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == nFirstID)
		{
			pSubMenu = pMenu;
			break;
		}
	}
	ASSERT(pSubMenu != NULL);

	return pSubMenu;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Inserts a menu at a given position in a menu.
//
// PARAMETERS	: pMenu - Sub Menu to add item to.
//				: szText - Text to display.
//				: nID - ID to identify menu item.
//				: nPos - Position to add after.
//
// RETURNS		: TRUE if the menu item was successfully added.
////////////////////////////////////////////////////////////////////////////////

BOOL CWordAid2Dlg::InsertInMenu(CMenu* pMenu, CString szText, UINT nID, UINT nPos)
{
	MENUITEMINFO MInfo;

	MInfo.cbSize     = sizeof(MENUITEMINFO);
	MInfo.fMask      = MIIM_DATA | MIIM_ID |MIIM_TYPE; 
	MInfo.fType      = MFT_STRING;
	MInfo.fState     = MFS_DEFAULT;
	MInfo.wID		 = nID;
	MInfo.dwTypeData = szText.GetBuffer(255);
	MInfo.cch        = 40; //sizeof(Buffer);if(GetMenuItemInfo(hMenu, OrigPosition, TRUE, &MInfo))

	int nAfter = 0;

	for(int i = 0; i < (int)pMenu->GetMenuItemCount(); i++)
	{
		if(pMenu->GetMenuItemID(i) == nID)		// Already in menu
		{
			return FALSE;
		}

		if(pMenu->GetMenuItemID(i) == nPos)		// Position to add to menu
		{
			nAfter = i+1;
		}
	}
	return pMenu->InsertMenu(nAfter, MF_BYCOMMAND, nID, szText);
}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Removes a menu item from a menu.
//
// PARAMETERS	: pMenu - Menu the menu item is in.
//				: nID - ID of menu item to remove.
/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::RemoveMenuItem(CMenu* pMenu, UINT nID)
{
	for(int i =0; i < (int)pMenu->GetMenuItemCount(); i++)
	{
		if(pMenu->GetMenuItemID(i) == nID)
		{
			pMenu->RemoveMenu(i, MF_BYPOSITION);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Modifies the text on a menu item.
//
// PARAMETERS	: pMenu - Menu where the menu item exists.
//				: nID - ID of the menu item.
//				: strMenuText - New Text to display.
/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::ModifyMenuText(CMenu* pMenu, UINT nID, CString strMenuText)
{
	for(int i =0; i < (int)pMenu->GetMenuItemCount(); i++)
	{
		if(pMenu->GetMenuItemID(i) == nID)
		{
			pMenu->ModifyMenu(i, MF_BYPOSITION|MF_STRING, nID, strMenuText);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

BOOL CWordAid2Dlg::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	
	return CDialog::OnChildNotify(message, wParam, lParam, pLResult);
}

/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnClose() 
{

#ifndef _DEMO

	CString strTitle = _T("");
	m_bStayActive = TRUE;

	if(m_AbbrevList.m_bModified)
	{
		if(AfxMessageBox(IDS_SAVE_CHANGES_ABBREV, MB_YESNO) == IDYES)
		{
			strTitle.LoadString(IDS_SAVE_CHANGES_ABBREV);
			SelectTab(strTitle);
			OnFileSave();
			m_AbbrevList.SetModified(FALSE);
		}
	}

	if (m_AlphaList.m_bModified)
	{
		if (AfxMessageBox(IDS_SAVE_CHANGES_ALPHA,MB_YESNO) == IDYES)		//WA0030
		{
			strTitle.LoadString(IDS_ALPHA_WORDLIST);
			SelectTab(strTitle);
			//m_tab_wordlist.SetCurSel(1);
			// TODO : Should this not be FileSave?
			OnFileSaveas();
			m_AlphaList.SetModified(FALSE);
		}

	}
	m_bStayActive = TRUE;
	if (m_TopicList.m_bModified)
	{
		if (AfxMessageBox(IDS_SAVE_CHANGES_TOPIC,MB_YESNO) == IDYES)		//WA0030
		{
			strTitle.LoadString(IDS_TOPIC_WORDLIST);
			SelectTab(strTitle);
			//m_tab_wordlist.SetCurSel(0);
			// TODO : Should this not be FileSave?
			OnFileSaveas();
			m_TopicList.SetModified(FALSE);
		}
	}

#endif

	// Store the filenames to the registry
	m_Settings.SetSetting(ALPHA_FILENAME, m_szAlphaFilename);
	m_Settings.SetSetting(TOPIC_FILENAME, m_szTopicFilename);
	m_Settings.SetSetting(ABBREV_FILENAME, m_szAbbrevFilename);

	m_Settings.StoreSettings();
	m_bStayActive = FALSE;

	//WA0031 - This should be done in destructoir of CAbbrevList
	//WA0003 ensure abbrev edit dialogue is destroyed
/*	if (m_AbbrevList.m_pAbbrevEditDlg)					//WA0003
	{													//WA0003
		m_AbbrevList.m_pAbbrevEditDlg->DestroyWindow();//WA0003	
		delete m_AbbrevList.m_pAbbrevEditDlg;			//WA0003
	}	//WA0003*///WA0031 

	CWordAid2App* pApp = (CWordAid2App*)AfxGetApp();
	// Destroy the Dummy dialog so application destructor is called
	pApp->m_pDummy->DestroyWindow();
	delete pApp->m_pMainWnd;
	
	CDialog::OnClose();
}

/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnFileOpenalphabetic() 
{
#ifdef _DEMO
	m_bStayActive = TRUE;
	AfxMessageBox(IDS_DEMO_OPEN);
	m_bStayActive = FALSE;
#else
	CString szFileType = _T("");
	CString strTitle = _T("");
	strTitle.LoadString(ID_FILE_OPENALPHABETIC);

	m_bStayActive = TRUE;

	szFileType.LoadString(IDS_FILETYPE_ALPHA);

	CFileDialog fileDlg(TRUE, szFileType, m_szAlphaFilename, 0, szFileType);
	fileDlg.m_ofn.lpstrTitle = strTitle;

	if(fileDlg.DoModal() == IDOK)
	{
		if(m_AlphaList.LoadWordlist(fileDlg.GetPathName(), ALPHA))
		{
			m_szAlphaFilename = fileDlg.GetPathName();

			DisplayAlphaList();

			// Set Menu Items
			SetWordAidEditText();

			// Set tab
			CString strTitle = _T("");

			strTitle.LoadString(IDS_ALPHA_WORDLIST);
			SelectTab(strTitle);
			//m_tab_wordlist.SetCurSel(1);
			m_Settings.SetSetting(ALPHA_FILENAME,m_szAlphaFilename);
			m_Settings.StoreSettings();
		}
	}
	m_bStayActive = FALSE;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to display the alphabetic list in the list box.   //
//				: all code to display alphabetic list should be put here.  //
/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::DisplayAlphaList()
{
	m_wordlist.ResetContent();		// Clear Wordlist

//	if(m_szAlphaFilename != _T(""))
	{
		for(int i = 0; i < m_AlphaList.GetSize(); i++)
		{
			m_wordlist.AddString(m_AlphaList.GetAt(i));	
		}
	}

	m_chkHideList.SetCheck(atoi(m_Settings.GetSetting(ALPHA_LIST_HIDE)));
}

/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnFileOpentopic() 
{
#ifdef _DEMO
	m_bStayActive = TRUE;
	AfxMessageBox(IDS_DEMO_OPEN);
	m_bStayActive = FALSE;
#else
	CString szFileType = _T("");
	CString strTitle = _T("");
	strTitle.LoadString(ID_FILE_OPENTOPIC);

	m_bStayActive = TRUE;

	szFileType.LoadString(IDS_FILETYPE_TOPIC);

	CFileDialog fileDlg(TRUE, szFileType, m_szTopicFilename, 0, szFileType);
	fileDlg.m_ofn.lpstrTitle = strTitle;

	if(fileDlg.DoModal() == IDOK)
	{
		if(m_TopicList.LoadWordlist(fileDlg.GetPathName(), TOPIC))
		{
			m_szTopicFilename = fileDlg.GetPathName();

			DisplayTopicList();

			// Set Menu Items
			SetWordAidEditText();

			// Set tab
			CString strTitle = _T("");

			strTitle.LoadString(IDS_TOPIC_WORDLIST);
			SelectTab(strTitle);
			//m_tab_wordlist.SetCurSel(0);
			m_Settings.SetSetting(TOPIC_FILENAME,m_szTopicFilename);
			m_Settings.StoreSettings();
		}
	}
	m_bStayActive = FALSE;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to display the topic wordlist.  All code to       //
//				; display the wordlist should go here.					   //
/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::DisplayTopicList()
{
	m_wordlist.ResetContent();		// Clear Wordlist

//	if(m_szTopicFilename != _T(""))
	{
		for(int i = 0; i < m_TopicList.GetSize(); i++)
		{
			m_wordlist.AddString(m_TopicList.GetAt(i));	
		}
	}

	m_chkHideList.SetCheck(atoi(m_Settings.GetSetting(TOPIC_LIST_HIDE)));
}

/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnFileOpen() 
{
#ifdef _DEMO
	m_bStayActive = TRUE;
	AfxMessageBox(IDS_DEMO_OPEN);
	m_bStayActive = FALSE;
#else
	CString szFileType = _T("");
	CString szTopic = _T("");
	CString szAlpha = _T("");
	CString szAlphaOld = _T("");
	CString szAbbrev = _T("");
	CString szWordAid = _T("");
	CString szAbbrevOn = _T("");
	CString strTitle = _T("");
	strTitle.LoadString(ID_FILE_OPEN);

	m_bStayActive = TRUE;

	szWordAid = m_Settings.GetSetting(WORDAID_ON);
	szAbbrevOn = m_Settings.GetSetting(ABBREV_ON);

	if(szWordAid == TAB_ON && szAbbrevOn != TAB_ON)			// WordAid on, Abbrev off
	{
		szFileType.LoadString(IDS_FILETYPE_ALL);
	}
	else if(szWordAid == TAB_ON && szAbbrevOn == TAB_ON)	// WordAid and Abbrev on
	{
		szFileType.LoadString(IDS_FILETYPE_ALL_ABBREV);
	}
	else if(szWordAid != TAB_ON && szAbbrevOn == TAB_ON)	// WordAid off, Abbrev On
	{
		szFileType.LoadString(IDS_FILETYPE_ABBREV_ALL);
	}
	else 
	{
		return;
	}
	

	CFileDialog fileDlg(TRUE, szFileType, m_szTopicFilename, 0, szFileType);
	fileDlg.m_ofn.lpstrTitle = strTitle;

	if(fileDlg.DoModal() == IDOK)
	{
		CString szFilename = fileDlg.GetPathName();
		CString strTitle = _T("");

		// Load file extensions to check for
		szTopic.LoadString(IDS_TOPIC_EXT_TEST);
		CString szTopicUpper = szTopic;
		szTopic.MakeUpper();
		szAlpha.LoadString(IDS_ALPHA_EXT_TEST);
		CString szAlphaUpper = szAlpha;
		szAlphaUpper.MakeUpper();
		szAlphaOld.LoadString(IDS_ALPHA_EXT_OLD);
		CString szAlphaOldUpper = szAlphaOld;
		szAlphaOld.MakeUpper();
		szAbbrev.LoadString(IDS_ABBREV_EXT_TEST);
		szAbbrev.MakeUpper(); // WA0036

		// Check for each type and then load lists
		szFilename.MakeUpper();		// WA0036
		if(szFilename.Find(szTopic) > 0)
		{
			if(m_TopicList.LoadWordlist(fileDlg.GetPathName(), TOPIC) || szFilename.Find(szTopicUpper) > 0)
			{
				m_szTopicFilename = fileDlg.GetPathName();

				DisplayTopicList();

				// Set Menu Items
				SetWordAidEditText();

				// Set tab
				strTitle.LoadString(IDS_TOPIC_WORDLIST);
				SelectTab(strTitle);
				//m_tab_wordlist.SetCurSel(0);
				m_Settings.SetSetting(TOPIC_FILENAME,m_szTopicFilename);
				m_Settings.StoreSettings();
			}
		}
		else if(szFilename.Find(szAlpha) > 0 || szFilename.Find(szAlphaUpper) > 0
				|| szFilename.Find(szAlphaOld) > 0 || szFilename.Find(szAlphaOldUpper) > 0 )
		{
			if(m_AlphaList.LoadWordlist(fileDlg.GetPathName(), ALPHA))
			{
				m_szAlphaFilename = fileDlg.GetPathName();

				DisplayAlphaList();

				// Set Menu Items
				SetWordAidEditText();

				// Set tab
				strTitle.LoadString(IDS_ALPHA_WORDLIST);
				SelectTab(strTitle);
				//m_tab_wordlist.SetCurSel(1);
				m_Settings.SetSetting(ALPHA_FILENAME,m_szAlphaFilename);
				m_Settings.StoreSettings();
			}
		}
		else if(szFilename.Find(szAbbrev) > 0)
		{
			if(m_AbbrevList.LoadWordlist(fileDlg.GetPathName()))
			{
				m_szAbbrevFilename = fileDlg.GetPathName();

				DisplayAbbrevList();

				SetWordAidEditText(FALSE);

				strTitle.LoadString(IDS_ABBREV_WORDLIST);
				SelectTab(strTitle);
				//m_tab_wordlist.SetCurSel(2);
				m_Settings.SetSetting(ABBREV_FILENAME,m_szAbbrevFilename);
				m_Settings.StoreSettings();
			}
		}
		else
		{
			CString str = _T("");
			str.LoadString(IDS_ERROR_FILETYPE);
			AfxMessageBox(str);
		}
	}
	m_bStayActive = FALSE;
#endif
}

/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnSelchangeTabWordlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString strAbbrev = _T("");
	CString strAlpha = _T("");
	CString strTopic = _T("");
	TCITEM item;
	CString pstrString = _T("");

	// Setup to get Text from Tab
	item.mask = TCIF_TEXT;
	item.pszText = pstrString.GetBuffer(ITEM_BUFFER_SIZE);
	item.cchTextMax = ITEM_BUFFER_SIZE;

	strAbbrev.LoadString(IDS_ABBREV_WORDLIST);
	strAlpha.LoadString(IDS_ALPHA_WORDLIST);
	strTopic.LoadString(IDS_TOPIC_WORDLIST);

	// Get which tab has been selected
	int nTab = m_tab_wordlist.GetCurSel();

	if(m_tab_wordlist.GetItem(nTab, &item))
	{
		if(item.pszText == strAbbrev)
		{
			SetWindowTitle(m_szAbbrevFilename, (m_AbbrevList.m_bModified) ? FALSE : TRUE);
			DisplayAbbrevList();
			SetWordAidEditText(FALSE);
		}
		else if(item.pszText == strAlpha)
		{
			SetWindowTitle(m_szAlphaFilename, (m_AlphaList.m_bModified) ? FALSE : TRUE);
			DisplayAlphaList();
			SetWordAidEditText();
		}
		else if(item.pszText == strTopic)
		{	
			SetWindowTitle(m_szTopicFilename, (m_TopicList.m_bModified) ? FALSE : TRUE);
			DisplayTopicList();
			SetWordAidEditText();
		}
	}
		
	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////////////
// for topic lists default is to add at end unless a word is selected in which	//
// case the word is added after that position									//
// alphabetic lists are sorted so words are added at correct place in list		//
//////////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnEditAddword() 
{
	CString strAbbrev = _T("");
	CString strAlpha = _T("");
	CString strTopic = _T("");
	TCITEM item;
	CString pstrString = _T("");

	// Setup to get Text from Tab
	item.mask = TCIF_TEXT;
	item.pszText = pstrString.GetBuffer(ITEM_BUFFER_SIZE);
	item.cchTextMax = ITEM_BUFFER_SIZE;

	strAbbrev.LoadString(IDS_ABBREV_WORDLIST);
	strAlpha.LoadString(IDS_ALPHA_WORDLIST);
	strTopic.LoadString(IDS_TOPIC_WORDLIST);

	// Get which tab has been selected
	int nTab = m_tab_wordlist.GetCurSel();

	if(m_tab_wordlist.GetItem(nTab, &item))
	{
		m_bStayActive = TRUE;

		if(item.pszText == strAbbrev)
		{
			m_AbbrevList.AddWord();
			// TODO : This has been removed as the adding is now in a modeless dialog
			// and the dialog handles all of this
		/*	DisplayAbbrevList();

			if(m_AbbrevList.m_bModified)
			{
				if(AfxMessageBox(IDS_SAVE_CHANGES_ABBREV, MB_YESNO) == IDYES)
				{
					OnFileSave();
					m_AbbrevList.SetModified(FALSE);
				}
			}*/
		}
		else if(item.pszText == strAlpha)
		{
			m_AlphaList.AddWord();
			DisplayAlphaList();

			if (m_AlphaList.m_bModified)
			{
				if (AfxMessageBox(IDS_SAVE_CHANGES_ALPHA,MB_YESNO) == IDYES)		//WA0030
				{
					// WA0049 
					//OnFileSaveas();
					OnFileSave();
					
					m_AlphaList.SetModified(FALSE);
				}
				else
				{
					SetWindowTitle(m_szAlphaFilename, FALSE);
				}
			}

			m_bStayActive = FALSE;
		}
		else if(item.pszText == strTopic)
		{	
			if (m_wordlist.GetCurSel() != LB_ERR)
				m_TopicList.AddWord(m_wordlist.GetCurSel());
			else
				m_TopicList.AddWord();
			DisplayTopicList();

			if (m_TopicList.m_bModified)
			{
				if (AfxMessageBox(IDS_SAVE_CHANGES_TOPIC,MB_YESNO) == IDYES)		//WA0030
				{
					// WA0049 
					//OnFileSaveas();
					OnFileSave();

					m_TopicList.SetModified(FALSE);
				}
				else
				{
					SetWindowTitle(m_szTopicFilename, FALSE);
				}
			}

			m_bStayActive = FALSE;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnFileSaveas() 
{
#ifdef _DEMO
	m_bStayActive = TRUE;
	AfxMessageBox(IDS_DEMO_SAVE);
	m_bStayActive = FALSE;
#else
	CString szTopicType = _T("");
	CString szAlphaType = _T("");
	CString szAbbrevType = _T("");

	CString strAbbrev = _T("");
	CString strAlpha = _T("");
	CString strTopic = _T("");
	TCITEM item;
	CString pstrString = _T("");

	// Setup to get Text from Tab
	item.mask = TCIF_TEXT;
	item.pszText = pstrString.GetBuffer(ITEM_BUFFER_SIZE);
	item.cchTextMax = ITEM_BUFFER_SIZE;

	strAbbrev.LoadString(IDS_ABBREV_WORDLIST);
	strAlpha.LoadString(IDS_ALPHA_WORDLIST);
	strTopic.LoadString(IDS_TOPIC_WORDLIST);

	// Get which tab has been selected
	int nTab = m_tab_wordlist.GetCurSel();

	if(m_tab_wordlist.GetItem(nTab, &item))
	{
		m_bStayActive = TRUE;

		if(item.pszText == strAbbrev)
		{
			szAbbrevType.LoadString(IDS_FILETYPE_ABBREV);
			if(m_szAbbrevFilename.IsEmpty())
			{
				m_szAbbrevFilename = m_Settings.GetSetting(ABBREV_FILENAME);
			}

			SaveFile(ABBREV, m_szAbbrevFilename, szAbbrevType, m_AbbrevList);
			m_Settings.SetSetting(ABBREV_FILENAME, m_szAbbrevFilename);
			m_Settings.StoreSettings();
			SetWindowTitle(m_szAbbrevFilename);
		}
		else if(item.pszText == strAlpha)
		{
			szAlphaType.LoadString(IDS_FILETYPE_ALPHA);
			if (m_szAlphaFilename.IsEmpty())
				m_szAlphaFilename = m_Settings.GetSetting(ALPHA_FILENAME);

			SaveFile(ALPHA, m_szAlphaFilename, szAlphaType, m_AlphaList);
			m_Settings.SetSetting(ALPHA_FILENAME,m_szAlphaFilename);
			m_Settings.StoreSettings();
			SetWindowTitle(m_szAlphaFilename);
		}
		else if(item.pszText == strTopic)
		{	
			szTopicType.LoadString(IDS_FILETYPE_TOPIC);
			if (m_szTopicFilename.IsEmpty())
				m_szTopicFilename = m_Settings.GetSetting(TOPIC_FILENAME);

			SaveFile(TOPIC, m_szTopicFilename, szTopicType, m_TopicList);
			m_Settings.SetSetting(TOPIC_FILENAME,m_szTopicFilename);
			m_Settings.StoreSettings();
			SetWindowTitle(m_szTopicFilename);
		}

		m_bStayActive = FALSE;	
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnFileSave() 
{
#ifdef _DEMO
	m_bStayActive = TRUE;
	AfxMessageBox(IDS_DEMO_SAVE);
	m_bStayActive = FALSE;
#else
	CString strAbbrev = _T("");
	CString strAlpha = _T("");
	CString strTopic = _T("");
	TCITEM item;
	CString pstrString = _T("");

	// Setup to get Text from Tab
	item.mask = TCIF_TEXT;
	item.pszText = pstrString.GetBuffer(ITEM_BUFFER_SIZE);
	item.cchTextMax = ITEM_BUFFER_SIZE;

	strAbbrev.LoadString(IDS_ABBREV_WORDLIST);
	strAlpha.LoadString(IDS_ALPHA_WORDLIST);
	strTopic.LoadString(IDS_TOPIC_WORDLIST);

	// Get which tab has been selected
	int nTab = m_tab_wordlist.GetCurSel();

	if(m_tab_wordlist.GetItem(nTab, &item))
	{
		m_bStayActive = TRUE;

		if(item.pszText == strAbbrev)
		{
			if(m_szAbbrevFilename == _T(""))
			{
				OnFileSaveas();
			}
			else
			{
				m_AbbrevList.SaveList(m_szAbbrevFilename);
				SetWindowTitle(m_szAbbrevFilename);			//WA0041
			}
		}
		else if(item.pszText == strAlpha)
		{
			if(m_szAlphaFilename == _T(""))
			{
				OnFileSaveas();
			}
			else
			{
				m_AlphaList.SaveList(m_szAlphaFilename);
				SetWindowTitle(m_szAlphaFilename);			//WA0041
			}

		}
		else if(item.pszText == strTopic)
		{	
			if(m_szTopicFilename == _T(""))
			{
				OnFileSaveas();
			}
			else
			{
				m_TopicList.SaveList(m_szTopicFilename);
				SetWindowTitle(m_szTopicFilename);			//WA0041
			}

		}

		m_bStayActive = FALSE;
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to save a file using the common file dialog.	   //
//				:														   //
// PARAMETERS	: nType - type of wordlist being saved ALPHA, TOPIC		   //
//				: szFilename - used to pass in previous filename and to	   //
//				: store new filename.									   //
//				: szExt - file extension related to file type.			   //
//				: list - the wordlist to save.							   //
/////////////////////////////////////////////////////////////////////////////
					 
BOOL CWordAid2Dlg::SaveFile(int nType, CString& szFilename,
							CString szExt, CWordlist& list)
{
	BOOL bReturn = FALSE;

	m_bStayActive = TRUE;
	if (m_bNewFile)
		szFilename = _T("");

	CFileDialog fileDlg(FALSE, szExt, szFilename, 0, szExt);

	if(fileDlg.DoModal() == IDOK)
	{
		szFilename = fileDlg.GetPathName();
		if(list.SaveList(szFilename))
		{
			bReturn = FALSE;
		}
		m_bNewFile = FALSE;
	}
	m_bStayActive = FALSE;

	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to save a file using the common file dialog.	   //
//				:														   //
// PARAMETERS	: nType - type of wordlist being saved ALPHA, TOPIC		   //
//				: szFilename - used to pass in previous filename and to	   //
//				: store new filename.									   //
//				: szExt - file extension related to file type.			   //
//				: list - the Abbrevlist to save.						   //
/////////////////////////////////////////////////////////////////////////////
					 
BOOL CWordAid2Dlg::SaveFile(int nType, CString& szFilename,
//							CString szExt, CAbbrevList* list)		WA0047
							CString szExt, CAbbrevList list)
{
	BOOL bReturn = FALSE;
	CString strTitle = _T("");
	strTitle.LoadString(ID_FILE_SAVE);

	m_bStayActive = TRUE;
	if (m_bNewFile)
		szFilename = _T("");

	CFileDialog fileDlg(FALSE, szExt, szFilename, 0, szExt);
	fileDlg.m_ofn.lpstrTitle = strTitle;
	if(fileDlg.DoModal() == IDOK)
	{
		szFilename = fileDlg.GetPathName();
		if(list.SaveList(szFilename))
		{
			bReturn = FALSE;
		}
		m_bNewFile = FALSE;
	}
	m_bStayActive = FALSE;

	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnEditEditword() 
{
	CString szWord = _T("");

	CString strAbbrev = _T("");
	CString strAlpha = _T("");
	CString strTopic = _T("");
	TCITEM item;
	CString pstrString = _T("");

	// Setup to get Text from Tab
	item.mask = TCIF_TEXT;
	item.pszText = pstrString.GetBuffer(ITEM_BUFFER_SIZE);
	item.cchTextMax = ITEM_BUFFER_SIZE;

	strAbbrev.LoadString(IDS_ABBREV_WORDLIST);
	strAlpha.LoadString(IDS_ALPHA_WORDLIST);
	strTopic.LoadString(IDS_TOPIC_WORDLIST);

	// Get which tab has been selected
	int nTab = m_tab_wordlist.GetCurSel();

	if(m_tab_wordlist.GetItem(nTab, &item))
	{
		m_bStayActive = TRUE;

		// Get the word to edit
		if (m_wordlist.GetCurSel() != LB_ERR)
			m_wordlist.GetText(m_wordlist.GetCurSel(), szWord);
		if (szWord.IsEmpty())
			AfxMessageBox(IDS_NO_WORD_SELECTED,MB_OK);
		else
		{
			if(item.pszText == strAbbrev)
			{
				m_AbbrevList.EditWord(szWord);		
				DisplayAbbrevList();

				if(m_AbbrevList.m_bModified)
				{
					if(AfxMessageBox(IDS_SAVE_CHANGES_ABBREV, MB_YESNO) == IDYES)
					{
						OnFileSave();
						m_AbbrevList.SetModified(FALSE);
					}
					else
					{
						SetWindowTitle(m_szAbbrevFilename, FALSE);
					}
				}
			}
			else if(item.pszText == strAlpha)
			{
				m_AlphaList.EditWord(szWord);
				DisplayAlphaList();

				if (m_AlphaList.m_bModified)
				{
					if (AfxMessageBox(IDS_SAVE_CHANGES_ALPHA,MB_YESNO) == IDYES)		//WA0030
					{
						// WA0049 
						//OnFileSaveas();
						OnFileSave();
						m_AlphaList.SetModified(FALSE);
					}
					else
					{
						SetWindowTitle(m_szAlphaFilename, FALSE);
					}
				}
			}
			else if(item.pszText == strTopic)
			{	
				m_TopicList.EditWord(szWord);
				DisplayTopicList();

				if (m_TopicList.m_bModified)
				{
					if (AfxMessageBox(IDS_SAVE_CHANGES_TOPIC,MB_YESNO) == IDYES)		//WA0030
					{
						// WA0049 
						//OnFileSaveas();
						OnFileSave();
						m_TopicList.SetModified(FALSE);
					}
					else
					{
						SetWindowTitle(m_szTopicFilename, FALSE);
					}
				}
			}
		}		
		m_bStayActive = FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////

//void CWordAid2Dlg::OnSettings()
//{
//	CSettingsDlg dlg;

//	if (dlg.DoModal() == IDOK)
//	{
//		//store the new settings
//	}

//}

void CWordAid2Dlg::OnBtnSortAlpha() 
{
	CString strTopic = _T("");
	CString strAlpha = _T("");	//added 15th aug
	CString strAbbrev = _T("");	//WA0002
	TCITEM item;
	CString pstrString = _T("");

	// Setup to get Text from Tab
	item.mask = TCIF_TEXT;
	item.pszText = pstrString.GetBuffer(ITEM_BUFFER_SIZE);
	item.cchTextMax = ITEM_BUFFER_SIZE;

	strTopic.LoadString(IDS_TOPIC_WORDLIST);
	strAlpha.LoadString(IDS_ALPHA_WORDLIST);	//added 15th aug
	strAbbrev.LoadString(IDS_ABBREV_WORDLIST);	//WA0002 

	// Get which tab has been selected
	int nTab = m_tab_wordlist.GetCurSel();

	if(m_tab_wordlist.GetItem(nTab, &item))
	{
		if (item.pszText == strTopic)
		{
			m_TopicList.SortList();
			DisplayTopicList();
		}
		else if (item.pszText == strAlpha)	//added 15th aug
		{	//added 15th aug
			m_AlphaList.SortList();	//added 15th aug
			DisplayAlphaList();	//added 15th aug
		}	//added 15th aug
		else if (item.pszText == strAbbrev)	//WA0002
		{									//WA0002	
			m_AbbrevList.SortList();		//WA0002	
			DisplayAbbrevList();			//WA0002
		}									//WA0002

	}
}
void CWordAid2Dlg::OnSettingsPopupsettings() 
{
	m_bStayActive = TRUE;

	CWordAidTreeDialog treeDlg(&m_Settings);
	CButtonSettings aDlg;
	CWordSettings wordDlg;
	CKeySettingsPage keyDlg(this, &treeDlg);
	CGeneralSettings genDlg;
	CSpeechSettingsDlg speechDlg;
	CSpeechSetupDlg setupDlg;
	CAbbrevExpansion abbrevExpDlg(this, &treeDlg);
	CWordAidSettingsDlg wordAidDlg;

	aDlg.m_Settings = &m_Settings;
	wordDlg.m_Settings = &m_Settings;
	keyDlg.m_Settings = &m_Settings;
	genDlg.m_Settings = &m_Settings;
	speechDlg.m_Settings = &m_Settings;
	setupDlg.m_Settings = &m_Settings;
	abbrevExpDlg.m_Settings = &m_Settings;
	wordAidDlg.m_Settings = &m_Settings;

	CString strSetting		= _T("");
	CString strGeneral		= _T("");
	CString strButtons		= _T("");
	CString strWordlist		= _T("");
	CString strHotKeys		= _T("");
	CString strWordAid		= _T("");
	CString strWordAid1		= _T("");
	CString strAbbrev		= _T("");
	CString strExpansion	= _T("");
	CString strSpeech		= _T("");
	CString strSpeechSetup	= _T("");
	CString strVoiceSetup	= _T("");

	strSetting.LoadString(IDS_SETTING_SETTING);
	strGeneral.LoadString(IDS_SETTING_GENERAL);
	strButtons.LoadString(IDS_SETTING_BUTTONS);
	strWordlist.LoadString(IDS_SETTING_WORDLIST);
	strHotKeys.LoadString(IDS_SETTING_HOTKEYS);
	strWordAid.LoadString(IDS_SETTING_WORDAID);
	strWordAid1.LoadString(IDS_SETTING_WORDAID1);
	strAbbrev.LoadString(IDS_SETTING_ABBREV);
	strExpansion.LoadString(IDS_SETTING_EXPANSION);
	strSpeech.LoadString(IDS_SETTING_SPEECH);
	strSpeechSetup.LoadString(IDS_SETTING_SPEECH_SETUP);
	strVoiceSetup.LoadString(IDS_SETTING_VOICE_SETUP);

	treeDlg.AddPage(NULL, 0, strSetting, 0);
	treeDlg.AddPage(&genDlg, IDD_GENERAL_SETTINGS, strGeneral, 1);
	treeDlg.AddPage(&aDlg, IDD_POPUP_SETTINGS, strButtons, 1);
	treeDlg.AddPage(&wordDlg, IDD_WORD_SETTINGS, strWordlist, 1);
	treeDlg.AddPage(&keyDlg, IDD_KEY_SETTINGS, strHotKeys, 1);
	treeDlg.AddPage(NULL, 0, strWordAid, 0);
	treeDlg.AddPage(&wordAidDlg, IDD_WORDAID, strWordAid1, 1);
	treeDlg.AddPage(NULL, 0, strAbbrev, 0);
	treeDlg.AddPage(&abbrevExpDlg, IDD_ABBREV_EXPANSION, strExpansion, 1);
	treeDlg.AddPage(NULL, 0, strSpeech, 0);
	treeDlg.AddPage(&setupDlg, IDD_SPEECH_SETUP,strSpeechSetup,1);
	treeDlg.AddPage(&speechDlg,IDD_SPEECH_DLG, strVoiceSetup,1);
	

	((CWordAid2App*)AfxGetApp())->DisplaySettingPopup();

	if(treeDlg.DoModal() == IDOK)
	{
		m_Settings.StoreSettings();
		m_Settings.LoadSettings();	//make sure we have most up-to-date settings
		AddTabs();					// ReDisplay Tabs
	}
	else
	{
		m_Settings.LoadSettings();	// Load back chnages
	}

	((CWordAid2App*)AfxGetApp())->DisplaySettingPopup(FALSE);
	m_bStayActive = FALSE;
}


void CWordAid2Dlg::OnEditDeleteword() 
{
	CString szWord = _T("");
	m_bStayActive = TRUE;
	
	CString strAbbrev = _T("");
	CString strAlpha = _T("");
	CString strTopic = _T("");
	TCITEM item;
	CString pstrString = _T("");

	// Setup to get Text from Tab
	item.mask = TCIF_TEXT;
	item.pszText = pstrString.GetBuffer(ITEM_BUFFER_SIZE);
	item.cchTextMax = ITEM_BUFFER_SIZE;

	strAbbrev.LoadString(IDS_ABBREV_WORDLIST);
	strAlpha.LoadString(IDS_ALPHA_WORDLIST);
	strTopic.LoadString(IDS_TOPIC_WORDLIST);

	// Get which tab has been selected
	int nTab = m_tab_wordlist.GetCurSel();

	if(m_tab_wordlist.GetItem(nTab, &item))
	{	
		// Get the word to edit
		if (m_wordlist.GetCurSel() != LB_ERR)
		{
			m_wordlist.GetText(m_wordlist.GetCurSel(), szWord);
		}
		if (szWord.IsEmpty())
		{
			AfxMessageBox(IDS_NO_WORD_SELECTED,MB_OK);
		}
		else
		{
			if(item.pszText == strAbbrev)
			{
				m_AbbrevList.Delete(szWord);
				DisplayAbbrevList();

				if(m_AbbrevList.m_bModified)
				{
					if(AfxMessageBox(IDS_SAVE_CHANGES_ABBREV, MB_YESNO) == IDYES)
					{
						OnFileSave();
						m_AbbrevList.SetModified(FALSE);
					}
					else
					{
						SetWindowTitle(m_szAbbrevFilename, FALSE);
					}
				}
			}
			else if(item.pszText == strAlpha)
			{
				m_AlphaList.Delete(szWord);
				DisplayAlphaList();

				if (m_AlphaList.m_bModified)
				{
					if (AfxMessageBox(IDS_SAVE_CHANGES_ALPHA,MB_YESNO) == IDYES)		//WA0030
					{
						// WA0049 
						//OnFileSaveas();
						OnFileSave();
						m_AlphaList.SetModified(FALSE);
					}
					else
					{
						SetWindowTitle(m_szAlphaFilename, FALSE);
					}
				}
			}
			else if(item.pszText == strTopic)
			{
				m_TopicList.Delete(szWord);
				DisplayTopicList();

				if (m_TopicList.m_bModified)
				{
					if (AfxMessageBox(IDS_SAVE_CHANGES_TOPIC,MB_YESNO) == IDYES)		//WA0030
					{
						// WA0049 
						//OnFileSaveas();
						OnFileSave();
						m_TopicList.SetModified(FALSE);
					}
					else
					{
						SetWindowTitle(m_szTopicFilename, FALSE);
					}
				}
			}
		}
	}

	m_bStayActive = FALSE;

}

void CWordAid2Dlg::OnSavesettings() 
{

	CString szFileType = _T("");
	CString szFileExt = _T("");
	CString szFilename = _T("");

	m_bStayActive = TRUE;

	szFileType.LoadString(IDS_TEXT_EXT);
	szFileExt.LoadString(IDS_TEXT_EXT_ADD);

	CFileDialog fileDlg(FALSE, szFileExt, szFilename, 0, szFileType);

	if(fileDlg.DoModal() == IDOK)
	{
		szFilename = fileDlg.GetPathName();
	}

	m_Settings.SaveToTextFile(szFilename);//"c:\\wdtest.ini");
	m_bStayActive = FALSE;
}

void CWordAid2Dlg::OnLoadsettings() 
{
	CString szFileType = _T("");
	CString szFilename = _T("");

	m_bStayActive = TRUE;

	szFileType.LoadString(IDS_TEXT_EXT);

	CFileDialog fileDlg(TRUE, szFileType, szFilename, 0, szFileType);

	if(fileDlg.DoModal() == IDOK)
	{
		szFilename = fileDlg.GetPathName();
		m_Settings.LoadFromTextFile(szFilename);//"c:\\wdtest.ini");
		// now save the new settings to registry
		if (!m_Settings.StoreSettings())
			AfxMessageBox(IDS_SETTINGS_ERROR);
		else
			AfxMessageBox(IDS_SETTINGS_UPDATED);
	}
	m_bStayActive = FALSE;
	
}

void CWordAid2Dlg::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	if (nState == WA_INACTIVE && !m_bStayActive)
		PostMessage(WM_SYSCOMMAND,MAKEWPARAM(SC_MINIMIZE,0));
}

void CWordAid2Dlg::OnFileExit() 
{
	OnClose();
	
}

void CWordAid2Dlg::OnFileNewwordlist() 
{
#ifdef _DEMO
	m_bStayActive = TRUE;
	AfxMessageBox(IDS_DEMO_NEW);
	m_bStayActive = FALSE;
#else
	m_bNewFile = TRUE;
	CString strAbbrev = _T("");
	CString strAlpha = _T("");
	CString strTopic = _T("");
	TCITEM item;
	CString pstrString = _T("");

	// Setup to get Text from Tab
	item.mask = TCIF_TEXT;
	item.pszText = pstrString.GetBuffer(ITEM_BUFFER_SIZE);
	item.cchTextMax = ITEM_BUFFER_SIZE;

	strAbbrev.LoadString(IDS_ABBREV_WORDLIST);
	strAlpha.LoadString(IDS_ALPHA_WORDLIST);
	strTopic.LoadString(IDS_TOPIC_WORDLIST);

	CNewListDlg aDlg;

	aDlg.m_bAbbrevOn	= (BOOL)atoi(m_Settings.GetSetting(ABBREV_ON));
	aDlg.m_bWordAidOn	= (BOOL)atoi(m_Settings.GetSetting(WORDAID_ON));

	m_bStayActive = TRUE;

	if(aDlg.DoModal() == IDOK)
	{
		CString strTitle = _T("");

		if(aDlg.m_nListType == ABBREV)
		{
			// Move to the Abbrev Tab
			strTitle.LoadString(IDS_ABBREV_WORDLIST);
			SelectTab(strTitle);

			// Store old list if modified
			if(m_AbbrevList.m_bModified)
			{
				m_bStayActive = TRUE;

				// Save Changes to list
				if(AfxMessageBox(IDS_SAVE_CHANGES_ABBREV, MB_YESNO) == IDYES)
				{
					OnFileSave();
				}

				m_bStayActive = FALSE;
			}

			// Clear Abbrev file info
			m_szAbbrevFilename = _T("");			// Clear Filaname
			SetWindowTitle(m_szAbbrevFilename);		// Clear WindowTitle
			m_AbbrevList.RemoveAll();				// Clear list
			m_AbbrevList.SetModified(FALSE);		// Set list no modified
			DisplayAbbrevList();					// Display the empty list
		}
		else if(aDlg.m_nListType == ALPHA)
		{
			// Move to the Alpha list tab
			strTitle.LoadString(IDS_ALPHA_WORDLIST);
			SelectTab(strTitle);

			// Store the old list if modified
			if (m_AlphaList.m_bModified)
			{
				m_bStayActive = TRUE;

				if (AfxMessageBox(IDS_SAVE_CHANGES_ALPHA,MB_YESNO) == IDYES)		//WA0030
				{
					// TODO : Should this not be FileSave as if there is already a file it should just save?
					OnFileSave();
					m_AlphaList.SetModified(FALSE);
				}
				m_bStayActive = FALSE;
			}

			// WA0055 - Clear the Alpha Filename
			m_szAlphaFilename = _T("");

			// Clear Alpah list file info
			m_AlphaList.RemoveAll();
			m_AlphaList.SetModified(FALSE);
			DisplayAlphaList();
		}
		else if(aDlg.m_nListType == TOPIC)
		{
			// Move to the Topic tab
			strTitle.LoadString(IDS_TOPIC_WORDLIST);
			SelectTab(strTitle);

			// Store old list if modified
			if (m_TopicList.m_bModified)
			{
				m_bStayActive = TRUE;

				if (AfxMessageBox(IDS_SAVE_CHANGES_TOPIC,MB_YESNO) == IDYES)		//WA0030
				{
					// TODO : Should this not be FileSave as if there is already a file it should just save?
					OnFileSave();
					m_TopicList.SetModified(FALSE);
				}
				m_bStayActive = FALSE;
			}

			// WA0055 Clear the topic filename
			m_szTopicFilename = _T("");

			// Clear Topic list info
			m_TopicList.RemoveAll();
			m_TopicList.SetModified(FALSE);
			DisplayTopicList();
		}

		// go straight to adding words
		OnEditAddword();
	}

	m_bStayActive = FALSE;
#endif
}

void CWordAid2Dlg::OnAppAbout() 
{
	// TODO: Add your command handler code here
	CAboutDlg dlg;
	m_bStayActive = TRUE;
	dlg.DoModal();
	m_bStayActive = FALSE;
}

void CWordAid2Dlg::OnDefaultHelp() 
{
	// TODO: Add your command handler code here
	
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Displays the abbreviation list in the list control.  It also
//				: changes the 'Edit' menu items text to display abbreviation
//				: strings.
////////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::DisplayAbbrevList()
{
	m_wordlist.ResetContent();		// Clear Wordlist

	// Display abbreviationlist
	for(int i = 0; i < m_AbbrevList.GetSize(); i++)
	{
		m_wordlist.AddString(m_AbbrevList.GetAt(i));	
	}

	m_chkHideList.SetCheck(atoi(m_Settings.GetSetting(ABBREV_LIST_HIDE)));
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to Open an Abbrev text file.
////////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnFileOpenAbbrev()
{
#ifdef _DEMO
	m_bStayActive = TRUE;
	AfxMessageBox(IDS_DEMO_OPEN);
	m_bStayActive = FALSE;
#else
	CString strTitle = _T("");
	strTitle.LoadString(ID_FILE_OPENABBREV);

	// Only open if abbrev is on this is used to stop the accelerator key from
	// allowing abbrev files to be opened if pressed when Abbrev is turned off
	if(atoi(m_Settings.GetSetting(ABBREV_ON)))
	{
		CString szFileType = _T("");
		m_bStayActive = TRUE;

		szFileType.LoadString(IDS_FILETYPE_ABBREV);

		CFileDialog fileDlg(TRUE, szFileType, m_szAbbrevFilename, 0, szFileType);
		fileDlg.m_ofn.lpstrTitle = strTitle;

		if(fileDlg.DoModal() == IDOK)
		{
			if(m_AbbrevList.LoadWordlist(fileDlg.GetPathName()))
			{
				m_szAbbrevFilename = fileDlg.GetPathName();

				DisplayAbbrevList();

				// Set Menu Items
				SetWordAidEditText(FALSE);

				// Set tab
				CString strTitle = _T("");

				strTitle.LoadString(IDS_ABBREV_WORDLIST);
				SelectTab(strTitle);
				//m_tab_wordlist.SetCurSel(2);
				m_Settings.SetSetting(ABBREV_FILENAME,m_szAbbrevFilename);
				m_Settings.StoreSettings();
			}
		}
		m_bStayActive = FALSE;
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Selects the tab by name.
//
// PARAMETERS	: strTabName - Name of tab to select.
//
// RETURNS		: TRUE if the tab was found and selected.
////////////////////////////////////////////////////////////////////////////////

BOOL CWordAid2Dlg::SelectTab(CString strTabName)
{
	BOOL bReturn = FALSE;
	TCITEM item;
	CString pstrString = _T("");

	item.mask = TCIF_TEXT;
	item.pszText = pstrString.GetBuffer(ITEM_BUFFER_SIZE);
	item.cchTextMax = ITEM_BUFFER_SIZE;

	for(int i = 0; i < m_tab_wordlist.GetItemCount(); i++)
	{
		if(m_tab_wordlist.GetItem(i, &item))
		{
			if(item.pszText == strTabName)
			{
				m_tab_wordlist.SetCurSel(i);
				bReturn = TRUE;
				break;
			}
		}
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to set the window title text.  The title text is made
//				: up of the WordAid 2 title and the file that is being displayed
//				: in the list box.  If the file has not been sav then a * is 
//				: aded to the end of the filename.
//
// PARAMETERS	: strFIlePath - the file path and name of the file that is
//				: being displayed in the list box.
//				: bSaved - whether list has been seaved since the last 
//				: modification.
////////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::SetWindowTitle(CString strFilePath, BOOL bSaved /*= TRUE*/)
{
	//Load the TItle 
	CString strWordAid = _T("");
	strWordAid.LoadString(IDS_WORDAID_TITLE);

	// Remove path information
	// TODO : '\\' shoudl be a constant
	CString strFile = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('\\') - 1);

	if(!bSaved)
	{
		// TODO : Should be a constant
		strFile += _T("*");
	}

	// Set the filename in the title bar
	SetWindowText(strWordAid + _T(" - ") + strFile);

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnSettingsToolbar()
{
	if(m_toolBar.IsWindowVisible())
	{
		// Hide toolbar
		m_toolBar.ShowWindow(SW_HIDE);
	}
	else
	{
		// Show Toolbar
		m_toolBar.ShowWindow(SW_SHOW);
	}
}

void CWordAid2Dlg::OnUpdateSettingsToolbar(CCmdUI *pCmdUI)
{
	// Check or Uncheck menu item
	pCmdUI->SetCheck(m_toolBar.IsWindowVisible());
}

void CWordAid2Dlg::OnMergeFiles() 
{
	// TODO: Add your command handler code here
	m_bStayActive = TRUE;	// ensure don't switch to wordlist window

	CPropertySheet fileMerge;
	CMergeFileSelectDlg selectFileDlg;
	CMergeSelectWordDlg selectWordDlg;
	CMergeTypeDlg		selectTypeDlg;
	CMergeIntoDlg		intoDlg;

	fileMerge.AddPage(&intoDlg);
	fileMerge.AddPage(&selectFileDlg);
	fileMerge.AddPage(&selectTypeDlg);
	fileMerge.AddPage(&selectWordDlg);

	//initialise file select page
	selectFileDlg.m_strMainFilename = m_szAlphaFilename;
	selectFileDlg.m_strMergeFilename = m_szTopicFilename;
	selectFileDlg.m_strOutputFilename = m_szAlphaFilename;

	fileMerge.SetWizardMode();
	if (fileMerge.DoModal() == ID_WIZFINISH)
	{
		if (((CMergeTypeDlg*)fileMerge.GetPage(2))->m_bMergeAll)
		{
			// merge mainfile with mergefile
			if (selectFileDlg.m_strOutputFilename.Find(".alp",0) || selectFileDlg.m_strOutputFilename.Find(".ALP",0)
				|| selectFileDlg.m_strOutputFilename.Find(".abc",0) || selectFileDlg.m_strOutputFilename.Find(".ABC",0))
			{
				m_AlphaList.MergeFiles(selectFileDlg.m_strMainFilename,	selectFileDlg.m_strMergeFilename);
				m_AlphaList.SaveList(selectFileDlg.m_strOutputFilename);
			}
			else if (selectFileDlg.m_strOutputFilename.Find(".top",0) || selectFileDlg.m_strOutputFilename.Find(".TOP",0))
			{
				m_TopicList.MergeFiles(selectFileDlg.m_strMainFilename,	selectFileDlg.m_strMergeFilename);
				m_TopicList.SaveList(selectFileDlg.m_strOutputFilename);
			}
			else if (selectFileDlg.m_strOutputFilename.Find(".abv",0))
			{
				m_AbbrevList.MergeFiles(selectFileDlg.m_strMainFilename,selectFileDlg.m_strMergeFilename);
				m_AbbrevList.SaveList(selectFileDlg.m_strOutputFilename);
			}
		}
		else
		{
			// write the contents of output edit box to file
			//CListBox* listbox = (CListBox*)selectWordDlg.GetDlgItem(IDC_LIST_OUTPUT);

			if (selectFileDlg.m_strOutputFilename.Find(".alp",0) || selectFileDlg.m_strOutputFilename.Find(".ALP",0)
				|| selectFileDlg.m_strOutputFilename.Find(".abc",0) || selectFileDlg.m_strOutputFilename.Find(".ABC",0))
			{
				CWordlist temp;
				CString str = _T("");

				for (int i = 0; i<selectWordDlg.m_strArrayOutput.GetSize();i++)
				{
					//listbox->GetText(i,str);
					temp.Add(selectWordDlg.m_strArrayOutput[i]);
					TRACE("temp Size = %d\n", temp.GetSize());
				}
				m_AlphaList = temp;
				m_AlphaList.SaveList(selectFileDlg.m_strOutputFilename);
			}
			if (selectFileDlg.m_strOutputFilename.Find(".top",0) || selectFileDlg.m_strOutputFilename.Find(".TOP",0))
			{
				CWordlist temp;
				CString str = _T("");

				for (int i = 0; i<selectWordDlg.m_strArrayOutput.GetSize();i++)
				{
					//listbox->GetText(i,str);
					temp.Add(selectWordDlg.m_strArrayOutput[i]);
					TRACE("temp Size = %d\n", temp.GetSize());
				}
				m_TopicList = temp;
				m_TopicList.SaveList(selectFileDlg.m_strOutputFilename);
			}
			if (selectFileDlg.m_strOutputFilename.Find(".abv",0) || selectFileDlg.m_strOutputFilename.Find(".ABV",0))
			{
				CAbbrevList temp;
				CString str = _T("");

				for (int i=0; i<selectWordDlg.m_strArrayOutput.GetSize();i++)
				{
					//listbox->GetText(i,str);
					temp.InsertAtEnd(selectWordDlg.m_strArrayOutput[i]);
				}
				m_AbbrevList = temp;
				m_AbbrevList.SaveList(selectFileDlg.m_strOutputFilename);
			}
		}
	}

	m_bStayActive = FALSE;
}

int CWordAid2Dlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex) 
{
	// TODO: Add your message handler code here and/or call default
	static CString str = _T("");
	str = str + (char)nKey;
	pListBox->SelectString(0,str);
	
	return CDialog::OnVKeyToItem(nKey, pListBox, nIndex);
}

int CWordAid2Dlg::OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex) 
{
	// TODO: Add your message handler code here and/or call default
	static CString str = _T("");
	str = str + (char)nChar;
	pListBox->SelectString(0,str);
	
	return CDialog::OnCharToItem(nChar, pListBox, nIndex);
}
/*
BOOL CWordAid2Dlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR,LRESULT* pResult)
{
	BOOL bResult = TRUE;
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT nID =pNMHDR->idFrom;
    if (pTTT)//->uFlags & TTF_IDISHWND)
    {
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);
        if(nID)
        {
            pTTT->lpszText = MAKEINTRESOURCE(nID);
            pTTT->hinst = AfxGetResourceHandle();
        }
    }
	else
		bResult = FALSE;


	return bResult;
}*/


BOOL CWordAid2Dlg::PreTranslateMessage(MSG* pMsg)
{
	if(m_hAccelerator)
	{
		return ::TranslateAccelerator(m_hWnd, m_hAccelerator, pMsg);
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CWordAid2Dlg::OnClickHideList()
{
	CString strAbbrev = _T("");
	CString strAlpha = _T("");
	CString strTopic = _T("");
	TCITEM item;
	CString pstrString = _T("");

	// Setup to get Text from Tab
	item.mask = TCIF_TEXT;
	item.pszText = pstrString.GetBuffer(ITEM_BUFFER_SIZE);
	item.cchTextMax = ITEM_BUFFER_SIZE;

	strAbbrev.LoadString(IDS_ABBREV_WORDLIST);
	strAlpha.LoadString(IDS_ALPHA_WORDLIST);
	strTopic.LoadString(IDS_TOPIC_WORDLIST);

	// Set whether the list should be hidden
	CString strState = _T("");
	
	strState.Format(_T("%d"), m_chkHideList.GetCheck());

	// Get which tab has been selected
	int nTab = m_tab_wordlist.GetCurSel();

	// Store the state in the settings
	if(m_tab_wordlist.GetItem(nTab, &item))
	{
		if(item.pszText == strAbbrev)
		{
			m_Settings.SetSetting(ABBREV_LIST_HIDE, strState);
		}
		else if(item.pszText == strAlpha)
		{
			m_Settings.SetSetting(ALPHA_LIST_HIDE, strState);
		}
		else if(item.pszText == strTopic)
		{
			m_Settings.SetSetting(TOPIC_LIST_HIDE, strState);
		}
	}
}

void CWordAid2Dlg::OnDblclkListWordlist() 
{
	// If somthing selected then Edit
	if(m_wordlist.GetCurSel() != -1)
	{
		OnEditEditword();
	}
}

void CWordAid2Dlg::OnSettingsLexicon() 
{
	m_bStayActive = TRUE;
//	m_ctrlSpeaker.RestoreVoice("WORDAID");
	m_ctrlSpeaker.ShowLexiconDialog((LONG)m_hWnd);	
	m_bStayActive = FALSE;
}

////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when the window becomes hidden or shown.  
//				: When the window is shown then set the title text of
//				: the window to the filename of the selected tabs list.
//				: This is need as a new list could of been loaded via 
//				: DDE.
//
// PARAMETERS	: See MSDN Documentation for more details
////////////////////////////////////////////////////////////////////////

void CWordAid2Dlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if(bShow)
	{
		CString strAbbrev = _T("");
		CString strAlpha = _T("");
		CString strTopic = _T("");
		CString pstrString = _T("");
		TCITEM item;

		item.mask = TCIF_TEXT;
		item.pszText = pstrString.GetBuffer(ITEM_BUFFER_SIZE);
		item.cchTextMax = ITEM_BUFFER_SIZE;

		strAbbrev.LoadString(IDS_ABBREV_WORDLIST);
		strAlpha.LoadString(IDS_ALPHA_WORDLIST);
		strTopic.LoadString(IDS_TOPIC_WORDLIST);

		// Get which tab has been selected
		int nTab = m_tab_wordlist.GetCurSel();

		if(m_tab_wordlist.GetItem(nTab, &item))
		{
			if(item.pszText == strAbbrev)
			{
				SetWindowTitle(m_szAbbrevFilename, (m_AbbrevList.m_bModified) ? FALSE : TRUE);
			}
			else if(item.pszText == strAlpha)
			{
				SetWindowTitle(m_szAlphaFilename, (m_AlphaList.m_bModified) ? FALSE : TRUE);
			}
			else if(item.pszText == strTopic)
			{	
				SetWindowTitle(m_szTopicFilename, (m_TopicList.m_bModified) ? FALSE : TRUE);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: These methods are used to load lists for the DEMO version
//				: of WordAid.  They all return TRUE and are called in AddTabs
////////////////////////////////////////////////////////////////////////////////

#ifdef _DEMO

BOOL CWordAid2Dlg::LoadDemoTopicList()
{
	m_TopicList.RemoveAll();

#ifdef _SWED_DEMO

	m_TopicList.InsertAtEnd("apelsin");
	m_TopicList.InsertAtEnd("äpple");
	m_TopicList.InsertAtEnd("banan");
	m_TopicList.InsertAtEnd("päron");
	m_TopicList.InsertAtEnd("plommon");
	m_TopicList.InsertAtEnd("persika");
#else
	m_TopicList.InsertAtEnd("Monday");
	m_TopicList.InsertAtEnd("Tuesday");
	m_TopicList.InsertAtEnd("Wednesday");
	m_TopicList.InsertAtEnd("Thursday");
	m_TopicList.InsertAtEnd("Friday");
	m_TopicList.InsertAtEnd("Saturday");
	m_TopicList.InsertAtEnd("Sunday");
#endif

	return TRUE;
}

#ifdef _SWED_DEMO

void CWordAid2Dlg::LoadList1()
{
	m_AlphaList.InsertAtEnd("absorbera");
	m_AlphaList.InsertAtEnd("actionfigur");
	m_AlphaList.InsertAtEnd("actionprogram");
	m_AlphaList.InsertAtEnd("addition");
	m_AlphaList.InsertAtEnd("adjektiv");
	m_AlphaList.InsertAtEnd("adj");
	m_AlphaList.InsertAtEnd("adress");
	m_AlphaList.InsertAtEnd("adressbok");
	m_AlphaList.InsertAtEnd("adressera");
	m_AlphaList.InsertAtEnd("adressregister");
	m_AlphaList.InsertAtEnd("advent");
	m_AlphaList.InsertAtEnd("advokat");
	m_AlphaList.InsertAtEnd("aerobics");
	m_AlphaList.InsertAtEnd("affr");
	m_AlphaList.InsertAtEnd("affrscentrum");
	m_AlphaList.InsertAtEnd("affrsgata");
	m_AlphaList.InsertAtEnd("affrsverksamhet");
	m_AlphaList.InsertAtEnd("agera");
	m_AlphaList.InsertAtEnd("agna");
	m_AlphaList.InsertAtEnd("agnar");
	m_AlphaList.InsertAtEnd("akne");
	m_AlphaList.InsertAtEnd("akrobat");
	m_AlphaList.InsertAtEnd("aktiv");
	m_AlphaList.InsertAtEnd("aktivitet");
	m_AlphaList.InsertAtEnd("aktivitetscenter");
	m_AlphaList.InsertAtEnd("aktivitetsrum");
	m_AlphaList.InsertAtEnd("akutsjukhus");
	m_AlphaList.InsertAtEnd("akvariefisk");
	m_AlphaList.InsertAtEnd("akvarium");
	m_AlphaList.InsertAtEnd("akvedukt");
	m_AlphaList.InsertAtEnd("aldrig");
	m_AlphaList.InsertAtEnd("alfabet");
	m_AlphaList.InsertAtEnd("alkalisk");
	m_AlphaList.InsertAtEnd("alkohol");
	m_AlphaList.InsertAtEnd("alkoholfri");
	m_AlphaList.InsertAtEnd("alkoholfrbud");
	m_AlphaList.InsertAtEnd("all");
	m_AlphaList.InsertAtEnd("alla");
	m_AlphaList.InsertAtEnd("alldeles");
	m_AlphaList.InsertAtEnd("allergi");
	m_AlphaList.InsertAtEnd("alligator");
	m_AlphaList.InsertAtEnd("allmn");
	m_AlphaList.InsertAtEnd("allmnheten");
	m_AlphaList.InsertAtEnd("allra");
	m_AlphaList.InsertAtEnd("alls");
	m_AlphaList.InsertAtEnd("allt");
	m_AlphaList.InsertAtEnd("alltfr");
	m_AlphaList.InsertAtEnd("alltid");
	m_AlphaList.InsertAtEnd("allting");
	m_AlphaList.InsertAtEnd("allts");
	m_AlphaList.InsertAtEnd("allvetare");
	m_AlphaList.InsertAtEnd("almanacka");
	m_AlphaList.InsertAtEnd("altare");
	m_AlphaList.InsertAtEnd("aluminium");
	m_AlphaList.InsertAtEnd("ambassadr");
	m_AlphaList.InsertAtEnd("ambulans");
	m_AlphaList.InsertAtEnd("ambulanspersonal");
	m_AlphaList.InsertAtEnd("amen");
	m_AlphaList.InsertAtEnd("amerikanska");
	m_AlphaList.InsertAtEnd("amning");
	m_AlphaList.InsertAtEnd("amperemeter");
	m_AlphaList.InsertAtEnd("analysera");
	m_AlphaList.InsertAtEnd("analyserar");
	m_AlphaList.InsertAtEnd("analppning");
	m_AlphaList.InsertAtEnd("ananas");
	m_AlphaList.InsertAtEnd("ananasjuice");
	m_AlphaList.InsertAtEnd("ananassaft");
	m_AlphaList.InsertAtEnd("and");
	m_AlphaList.InsertAtEnd("andas");
	m_AlphaList.InsertAtEnd("andetag");
	m_AlphaList.InsertAtEnd("andning");
	m_AlphaList.InsertAtEnd("andra");
	m_AlphaList.InsertAtEnd("anfall");
	m_AlphaList.InsertAtEnd("anisfrn");
	m_AlphaList.InsertAtEnd("anka");
	m_AlphaList.InsertAtEnd("ankare");
	m_AlphaList.InsertAtEnd("ankel");
	m_AlphaList.InsertAtEnd("ankunge");
	m_AlphaList.InsertAtEnd("anledning");
	m_AlphaList.InsertAtEnd("anlnda");
	m_AlphaList.InsertAtEnd("anlnder");
	m_AlphaList.InsertAtEnd("annan");
	m_AlphaList.InsertAtEnd("annandagen");
	m_AlphaList.InsertAtEnd("annars");
	m_AlphaList.InsertAtEnd("annat");
	m_AlphaList.InsertAtEnd("annons");
	m_AlphaList.InsertAtEnd("anpassad");
	m_AlphaList.InsertAtEnd("anser");
	m_AlphaList.InsertAtEnd("ansikte");
	m_AlphaList.InsertAtEnd("ansiktet");
	m_AlphaList.InsertAtEnd("ansiktsmask");
	m_AlphaList.InsertAtEnd("anslagstavla");
	m_AlphaList.InsertAtEnd("ansluta");
	m_AlphaList.InsertAtEnd("ansluter");
	m_AlphaList.InsertAtEnd("anstlld");
	m_AlphaList.InsertAtEnd("anstllda");
	m_AlphaList.InsertAtEnd("ansvar");
	m_AlphaList.InsertAtEnd("anskan");
	m_AlphaList.InsertAtEnd("antal");
	m_AlphaList.InsertAtEnd("antalet");
	m_AlphaList.InsertAtEnd("anteckning");
	m_AlphaList.InsertAtEnd("anteckningsbok");
	m_AlphaList.InsertAtEnd("antenn");
	m_AlphaList.InsertAtEnd("antenner");
	m_AlphaList.InsertAtEnd("antingen");
	m_AlphaList.InsertAtEnd("antropologi");
	m_AlphaList.InsertAtEnd("anvnda");
	m_AlphaList.InsertAtEnd("anvnder");
	m_AlphaList.InsertAtEnd("apa");
	m_AlphaList.InsertAtEnd("apelsin");
	m_AlphaList.InsertAtEnd("apelsinjuice");
	m_AlphaList.InsertAtEnd("apelsinsaft");
	m_AlphaList.InsertAtEnd("apostel");
	m_AlphaList.InsertAtEnd("apostrof");
	m_AlphaList.InsertAtEnd("apotek");
	m_AlphaList.InsertAtEnd("apotekare");
	m_AlphaList.InsertAtEnd("appld");
	m_AlphaList.InsertAtEnd("aprikos");
	m_AlphaList.InsertAtEnd("aprikostrd");
	m_AlphaList.InsertAtEnd("april");
	m_AlphaList.InsertAtEnd("aptitretare");
	m_AlphaList.InsertAtEnd("arbeta");
	m_AlphaList.InsertAtEnd("arbetar");
	m_AlphaList.InsertAtEnd("arbetare");
	m_AlphaList.InsertAtEnd("arbete");
	m_AlphaList.InsertAtEnd("arbetet");
	m_AlphaList.InsertAtEnd("arbetsblad");
	m_AlphaList.InsertAtEnd("arbetsbok");
	m_AlphaList.InsertAtEnd("arbetsbord");
	m_AlphaList.InsertAtEnd("arbetsbnk");
	m_AlphaList.InsertAtEnd("arbetslag");
	m_AlphaList.InsertAtEnd("arbetslshet");
	m_AlphaList.InsertAtEnd("arbetsterapeut");
	m_AlphaList.InsertAtEnd("arbetsterapi");
	m_AlphaList.InsertAtEnd("arena");
	m_AlphaList.InsertAtEnd("arg");
	m_AlphaList.InsertAtEnd("ark");
	m_AlphaList.InsertAtEnd("arkitekt");
	m_AlphaList.InsertAtEnd("arkiv");
	m_AlphaList.InsertAtEnd("arkivskp");
	m_AlphaList.InsertAtEnd("arktisk");
	m_AlphaList.InsertAtEnd("arm");
	m_AlphaList.InsertAtEnd("armarna");
	m_AlphaList.InsertAtEnd("armband");
	m_AlphaList.InsertAtEnd("armbandsur");
	m_AlphaList.InsertAtEnd("armbrott");
	m_AlphaList.InsertAtEnd("armbge");
	m_AlphaList.InsertAtEnd("arm");
	m_AlphaList.InsertAtEnd("armhla");
	m_AlphaList.InsertAtEnd("armhlorna");
	m_AlphaList.InsertAtEnd("arrangera");
	m_AlphaList.InsertAtEnd("arrestera");
	m_AlphaList.InsertAtEnd("artist");
	m_AlphaList.InsertAtEnd("arton");
	m_AlphaList.InsertAtEnd("ask");
	m_AlphaList.InsertAtEnd("aska");
	m_AlphaList.InsertAtEnd("askfat");
	m_AlphaList.InsertAtEnd("assistent");
	m_AlphaList.InsertAtEnd("astma");
	m_AlphaList.InsertAtEnd("astronaut");
	m_AlphaList.InsertAtEnd("astronomi");
	m_AlphaList.InsertAtEnd("atom");
	m_AlphaList.InsertAtEnd("atomkrna");
	m_AlphaList.InsertAtEnd("att");
	m_AlphaList.InsertAtEnd("attack");
	m_AlphaList.InsertAtEnd("attrahera");
	m_AlphaList.InsertAtEnd("attraherar");
	m_AlphaList.InsertAtEnd("attraktiv");
	m_AlphaList.InsertAtEnd("audiometer");
	m_AlphaList.InsertAtEnd("augusti");
	m_AlphaList.InsertAtEnd("automat");
	m_AlphaList.InsertAtEnd("av");
	m_AlphaList.InsertAtEnd("avancera");
	m_AlphaList.InsertAtEnd("avancerar");
	m_AlphaList.InsertAtEnd("avbruten");
	m_AlphaList.InsertAtEnd("avbryt");
	m_AlphaList.InsertAtEnd("avdelning");
	m_AlphaList.InsertAtEnd("avfall");
	m_AlphaList.InsertAtEnd("avfyra");
	m_AlphaList.InsertAtEnd("avfyrar");
	m_AlphaList.InsertAtEnd("avfring");
	m_AlphaList.InsertAtEnd("avgng");
	m_AlphaList.InsertAtEnd("avhllsamhet");
	m_AlphaList.InsertAtEnd("avhmtning");
	m_AlphaList.InsertAtEnd("avigsida");
	m_AlphaList.InsertAtEnd("avlopp");
	m_AlphaList.InsertAtEnd("avlng ");
	m_AlphaList.InsertAtEnd("avokado");
	m_AlphaList.InsertAtEnd("avresa");
	m_AlphaList.InsertAtEnd("avses");
	m_AlphaList.InsertAtEnd("avsikt");
	m_AlphaList.InsertAtEnd("avsluta");
	m_AlphaList.InsertAtEnd("avslja");
	m_AlphaList.InsertAtEnd("avsljar");
	m_AlphaList.InsertAtEnd("avstnd");
	m_AlphaList.InsertAtEnd("avsndra");
	m_AlphaList.InsertAtEnd("avsndrar");
	m_AlphaList.InsertAtEnd("avtalat");
	m_AlphaList.InsertAtEnd("avundsjuk");
	m_AlphaList.InsertAtEnd("axel");
	m_AlphaList.InsertAtEnd("axeln");
	m_AlphaList.InsertAtEnd("axelvadd");
	m_AlphaList.InsertAtEnd("axelvaddar");
	m_AlphaList.InsertAtEnd("axlarna");
	m_AlphaList.InsertAtEnd("baby");
	m_AlphaList.InsertAtEnd("babydrkt");
	m_AlphaList.InsertAtEnd("babykorg");
	m_AlphaList.InsertAtEnd("babyskallra");
	m_AlphaList.InsertAtEnd("back");
	m_AlphaList.InsertAtEnd("backa");
	m_AlphaList.InsertAtEnd("backar");
	m_AlphaList.InsertAtEnd("backspegeln");
	m_AlphaList.InsertAtEnd("bacon");
	m_AlphaList.InsertAtEnd("bad");
	m_AlphaList.InsertAtEnd("bada");
	m_AlphaList.InsertAtEnd("badar");
	m_AlphaList.InsertAtEnd("badbyxor");
	m_AlphaList.InsertAtEnd("baddrkt");
	m_AlphaList.InsertAtEnd("badhandduk");
	m_AlphaList.InsertAtEnd("badkar");
	m_AlphaList.InsertAtEnd("badkarsmatta");
	m_AlphaList.InsertAtEnd("badklder");
	m_AlphaList.InsertAtEnd("badleksak");
	m_AlphaList.InsertAtEnd("badminton");
	m_AlphaList.InsertAtEnd("badmssa");
	m_AlphaList.InsertAtEnd("badplats");
	m_AlphaList.InsertAtEnd("badring");
	m_AlphaList.InsertAtEnd("badrum");
	m_AlphaList.InsertAtEnd("badrumsartiklar");
	m_AlphaList.InsertAtEnd("badstol");
	m_AlphaList.InsertAtEnd("bagage");
	m_AlphaList.InsertAtEnd("bagagelucka");
	m_AlphaList.InsertAtEnd("bagare");
	m_AlphaList.InsertAtEnd("bageri");
	m_AlphaList.InsertAtEnd("baguette");
	m_AlphaList.InsertAtEnd("bajs");
	m_AlphaList.InsertAtEnd("bajsa");
	m_AlphaList.InsertAtEnd("baka");
	m_AlphaList.InsertAtEnd("bakad");
	m_AlphaList.InsertAtEnd("bakar");
	m_AlphaList.InsertAtEnd("bakgrundsmusik");
	m_AlphaList.InsertAtEnd("bakmaskin");
	m_AlphaList.InsertAtEnd("bakom");
	m_AlphaList.InsertAtEnd("bakplt");
	m_AlphaList.InsertAtEnd("bakpulver");
	m_AlphaList.InsertAtEnd("baksida");
	m_AlphaList.InsertAtEnd("bakterie");
	m_AlphaList.InsertAtEnd("bakverk");
	m_AlphaList.InsertAtEnd("bakt");
	m_AlphaList.InsertAtEnd("balansbom");
	m_AlphaList.InsertAtEnd("balansera");
	m_AlphaList.InsertAtEnd("balanserad");
	m_AlphaList.InsertAtEnd("balett");
	m_AlphaList.InsertAtEnd("balettdanss");
	m_AlphaList.InsertAtEnd("balettsko");
	m_AlphaList.InsertAtEnd("balkong");
	m_AlphaList.InsertAtEnd("ballong");
	m_AlphaList.InsertAtEnd("balsal");
	m_AlphaList.InsertAtEnd("bambu");
	m_AlphaList.InsertAtEnd("bana");
	m_AlphaList.InsertAtEnd("banan");
	m_AlphaList.InsertAtEnd("bananblad");
	m_AlphaList.InsertAtEnd("bananbrd");
	m_AlphaList.InsertAtEnd("band");
	m_AlphaList.InsertAtEnd("bandage");
	m_AlphaList.InsertAtEnd("banderoll");
	m_AlphaList.InsertAtEnd("bandspelare");
	m_AlphaList.InsertAtEnd("bandsg");
	m_AlphaList.InsertAtEnd("bandtraktor");
	m_AlphaList.InsertAtEnd("bandy");
	m_AlphaList.InsertAtEnd("bangolf");
	m_AlphaList.InsertAtEnd("bank");
	m_AlphaList.InsertAtEnd("bankkassr");
	m_AlphaList.InsertAtEnd("bar");
	m_AlphaList.InsertAtEnd("bara");
	m_AlphaList.InsertAtEnd("bardisk");
	m_AlphaList.InsertAtEnd("barn");
	m_AlphaList.InsertAtEnd("barnavkomma");
	m_AlphaList.InsertAtEnd("barnbdd");
	m_AlphaList.InsertAtEnd("barnen");
	m_AlphaList.InsertAtEnd("barnkammare");
	m_AlphaList.InsertAtEnd("barnkrra");
	m_AlphaList.InsertAtEnd("barnmat");
	m_AlphaList.InsertAtEnd("barnmeny");
	m_AlphaList.InsertAtEnd("barnmisshandel");
	m_AlphaList.InsertAtEnd("barnmottagning");
	m_AlphaList.InsertAtEnd("barnpuder");
	m_AlphaList.InsertAtEnd("barnsbrd");
	m_AlphaList.InsertAtEnd("barnslig");
	m_AlphaList.InsertAtEnd("barnstol");
	m_AlphaList.InsertAtEnd("barnsng");
	m_AlphaList.InsertAtEnd("barnvagn");
	m_AlphaList.InsertAtEnd("barnvakt");
	m_AlphaList.InsertAtEnd("barnvisor");
	m_AlphaList.InsertAtEnd("barrtrd");
	m_AlphaList.InsertAtEnd("barstol");
	m_AlphaList.InsertAtEnd("bartender");
	m_AlphaList.InsertAtEnd("bas");
	m_AlphaList.InsertAtEnd("basketboll");
	m_AlphaList.InsertAtEnd("basketbollmatch");
	m_AlphaList.InsertAtEnd("bassng");
	
}

void CWordAid2Dlg::LoadList2()
{
	m_AlphaList.InsertAtEnd("bastu");
	m_AlphaList.InsertAtEnd("batteri");
	m_AlphaList.InsertAtEnd("batteriladdare");
	m_AlphaList.InsertAtEnd("beakta");
	m_AlphaList.InsertAtEnd("beaktar");
	m_AlphaList.InsertAtEnd("bebis");
	m_AlphaList.InsertAtEnd("bedja");
	m_AlphaList.InsertAtEnd("bedrift");
	m_AlphaList.InsertAtEnd("bedma");
	m_AlphaList.InsertAtEnd("bedmer");
	m_AlphaList.InsertAtEnd("befalla");
	m_AlphaList.InsertAtEnd("befaller");
	m_AlphaList.InsertAtEnd("befruktning");
	m_AlphaList.InsertAtEnd("begrava");
	m_AlphaList.InsertAtEnd("begravning");
	m_AlphaList.InsertAtEnd("begrnsad");
	m_AlphaList.InsertAtEnd("begrnsning");
	m_AlphaList.InsertAtEnd("begr ");
	m_AlphaList.InsertAtEnd("begra");
	m_AlphaList.InsertAtEnd("behov");
	m_AlphaList.InsertAtEnd("behlla");
	m_AlphaList.InsertAtEnd("behllare");
	m_AlphaList.InsertAtEnd("behller");
	m_AlphaList.InsertAtEnd("behva");
	m_AlphaList.InsertAtEnd("behvde");
	m_AlphaList.InsertAtEnd("behver");
	m_AlphaList.InsertAtEnd("behvs");
	m_AlphaList.InsertAtEnd("bekant");
	m_AlphaList.InsertAtEnd("bekanta");
	m_AlphaList.InsertAtEnd("bekantar");
	m_AlphaList.InsertAtEnd("bekantskap");
	m_AlphaList.InsertAtEnd("bekymmer");
	m_AlphaList.InsertAtEnd("bekymra");
	m_AlphaList.InsertAtEnd("bekymrad");
	m_AlphaList.InsertAtEnd("bekymrar");
	m_AlphaList.InsertAtEnd("beknna");
	m_AlphaList.InsertAtEnd("beknner");
	m_AlphaList.InsertAtEnd("beltenhet");
	m_AlphaList.InsertAtEnd("belning");
	m_AlphaList.InsertAtEnd("bemrkelsedag");
	m_AlphaList.InsertAtEnd("ben");
	m_AlphaList.InsertAtEnd("benbrott");
	m_AlphaList.InsertAtEnd("benen");
	m_AlphaList.InsertAtEnd("benklder");
	m_AlphaList.InsertAtEnd("benrrelse");
	m_AlphaList.InsertAtEnd("bensin");
	m_AlphaList.InsertAtEnd("bensinstation");
	m_AlphaList.InsertAtEnd("bensintank");
	m_AlphaList.InsertAtEnd("bensncka");
	m_AlphaList.InsertAtEnd("benvrmare");
	m_AlphaList.InsertAtEnd("ber");
	m_AlphaList.InsertAtEnd("berg");
	m_AlphaList.InsertAtEnd("bergsprngare");
	m_AlphaList.InsertAtEnd("bergstopp");
	m_AlphaList.InsertAtEnd("berusad");
	m_AlphaList.InsertAtEnd("berkna");
	m_AlphaList.InsertAtEnd("bertta");
	m_AlphaList.InsertAtEnd("berttade");
	m_AlphaList.InsertAtEnd("berttar");
	m_AlphaList.InsertAtEnd("berttelse");
	m_AlphaList.InsertAtEnd("berttiga");
	m_AlphaList.InsertAtEnd("berttigar");
	m_AlphaList.InsertAtEnd("bermd");
	m_AlphaList.InsertAtEnd("berr");
	m_AlphaList.InsertAtEnd("berra");
	m_AlphaList.InsertAtEnd("besk");
	m_AlphaList.InsertAtEnd("beskriva");
	m_AlphaList.InsertAtEnd("beskriver");
	m_AlphaList.InsertAtEnd("beskrivning");
	m_AlphaList.InsertAtEnd("beskydda");
	m_AlphaList.InsertAtEnd("beskyddar");
	m_AlphaList.InsertAtEnd("beslut");
	m_AlphaList.InsertAtEnd("besluta");
	m_AlphaList.InsertAtEnd("beslutar");
	m_AlphaList.InsertAtEnd("bestick");
	m_AlphaList.InsertAtEnd("bestiga ");
	m_AlphaList.InsertAtEnd("bestiger");
	m_AlphaList.InsertAtEnd("bestlla");
	m_AlphaList.InsertAtEnd("bestller");
	m_AlphaList.InsertAtEnd("bestmd");
	m_AlphaList.InsertAtEnd("bestmma");
	m_AlphaList.InsertAtEnd("bestmmer");
	m_AlphaList.InsertAtEnd("besvikelse");
	m_AlphaList.InsertAtEnd("besviken");
	m_AlphaList.InsertAtEnd("besvr");
	m_AlphaList.InsertAtEnd("besvra");
	m_AlphaList.InsertAtEnd("besvrande");
	m_AlphaList.InsertAtEnd("besvrar");
	m_AlphaList.InsertAtEnd("beska");
	m_AlphaList.InsertAtEnd("beskare");
	m_AlphaList.InsertAtEnd("besker");
	m_AlphaList.InsertAtEnd("beskskort");
	m_AlphaList.InsertAtEnd("besksrum");
	m_AlphaList.InsertAtEnd("beskstid");
	m_AlphaList.InsertAtEnd("beta");
	m_AlphaList.InsertAtEnd("betala");
	m_AlphaList.InsertAtEnd("betar");
	m_AlphaList.InsertAtEnd("beteende");
	m_AlphaList.InsertAtEnd("betong");
	m_AlphaList.InsertAtEnd("betrffande");
	m_AlphaList.InsertAtEnd("betyda");
	m_AlphaList.InsertAtEnd("betydelse");
	m_AlphaList.InsertAtEnd("betyder");
	m_AlphaList.InsertAtEnd("beundran");
	m_AlphaList.InsertAtEnd("bevakning");
	m_AlphaList.InsertAtEnd("bibel");
	m_AlphaList.InsertAtEnd("bibelstudium");
	m_AlphaList.InsertAtEnd("bibelstlle");
	m_AlphaList.InsertAtEnd("bibliotek");
	m_AlphaList.InsertAtEnd("bibliotekarie");
	m_AlphaList.InsertAtEnd("bidrag");
	m_AlphaList.InsertAtEnd("bifall");
	m_AlphaList.InsertAtEnd("biff");
	m_AlphaList.InsertAtEnd("biffktt");
	m_AlphaList.InsertAtEnd("bikarbonat");
	m_AlphaList.InsertAtEnd("bikini");
	m_AlphaList.InsertAtEnd("bikt");
	m_AlphaList.InsertAtEnd("bikupa");
	m_AlphaList.InsertAtEnd("bil");
	m_AlphaList.InsertAtEnd("bilbana");
	m_AlphaList.InsertAtEnd("bild");
	m_AlphaList.InsertAtEnd("bildlig");
	m_AlphaList.InsertAtEnd("bildschema");
	m_AlphaList.InsertAtEnd("bilen");
	m_AlphaList.InsertAtEnd("bilfrare");
	m_AlphaList.InsertAtEnd("biljard");
	m_AlphaList.InsertAtEnd("biljett");
	m_AlphaList.InsertAtEnd("biljettmottagare");
	m_AlphaList.InsertAtEnd("billig");
	m_AlphaList.InsertAtEnd("bilmekaniker");
	m_AlphaList.InsertAtEnd("bilparkering");
	m_AlphaList.InsertAtEnd("biltvtt");
	m_AlphaList.InsertAtEnd("biltvling");
	m_AlphaList.InsertAtEnd("binda");
	m_AlphaList.InsertAtEnd("binder");
	m_AlphaList.InsertAtEnd("bingo");
	m_AlphaList.InsertAtEnd("bingomrkpenna");
	m_AlphaList.InsertAtEnd("bingoutropare");
	m_AlphaList.InsertAtEnd("biograf");
	m_AlphaList.InsertAtEnd("biskop");
	m_AlphaList.InsertAtEnd("biskopsmssa");
	m_AlphaList.InsertAtEnd("bister");
	m_AlphaList.InsertAtEnd("bit");
	m_AlphaList.InsertAtEnd("bita");
	m_AlphaList.InsertAtEnd("bitar");
	m_AlphaList.InsertAtEnd("biter");
	m_AlphaList.InsertAtEnd("bitsocker");
	m_AlphaList.InsertAtEnd("bitter");
	m_AlphaList.InsertAtEnd("bjuda");
	m_AlphaList.InsertAtEnd("bjllra");
	m_AlphaList.InsertAtEnd("bjrk");
	m_AlphaList.InsertAtEnd("bjrklv");
	m_AlphaList.InsertAtEnd("bjrkris");
	m_AlphaList.InsertAtEnd("bjrn");
	m_AlphaList.InsertAtEnd("bjrnbr");
	m_AlphaList.InsertAtEnd("blad");
	m_AlphaList.InsertAtEnd("bladverk");
	m_AlphaList.InsertAtEnd("bland");
	m_AlphaList.InsertAtEnd("blanda");
	m_AlphaList.InsertAtEnd("blandade");
	m_AlphaList.InsertAtEnd("blank");
	m_AlphaList.InsertAtEnd("bleka ");
	m_AlphaList.InsertAtEnd("bleker");
	m_AlphaList.InsertAtEnd("blev");
	m_AlphaList.InsertAtEnd("bli");
	m_AlphaList.InsertAtEnd("blick");
	m_AlphaList.InsertAtEnd("blind");
	m_AlphaList.InsertAtEnd("blinka");
	m_AlphaList.InsertAtEnd("blinkar");
	m_AlphaList.InsertAtEnd("blir");
	m_AlphaList.InsertAtEnd("blissymbol");
	m_AlphaList.InsertAtEnd("blivit");
	m_AlphaList.InsertAtEnd("blixt");
	m_AlphaList.InsertAtEnd("blixtls");
	m_AlphaList.InsertAtEnd("blixtlset");
	m_AlphaList.InsertAtEnd("block");
	m_AlphaList.InsertAtEnd("blod");
	m_AlphaList.InsertAtEnd("blodomlopp");
	m_AlphaList.InsertAtEnd("blodtryck");
	m_AlphaList.InsertAtEnd("blodtrycksmtare");
	m_AlphaList.InsertAtEnd("blomkruka");
	m_AlphaList.InsertAtEnd("blomkl");
	m_AlphaList.InsertAtEnd("blomma");
	m_AlphaList.InsertAtEnd("blommig");
	m_AlphaList.InsertAtEnd("blommor");
	m_AlphaList.InsertAtEnd("blomsterkrans");
	m_AlphaList.InsertAtEnd("blomsterlda");
	m_AlphaList.InsertAtEnd("blomsterlk");
	m_AlphaList.InsertAtEnd("blond");
	m_AlphaList.InsertAtEnd("blottande");
	m_AlphaList.InsertAtEnd("blunda");
	m_AlphaList.InsertAtEnd("blundar");
	m_AlphaList.InsertAtEnd("blus");
	m_AlphaList.InsertAtEnd("blyertspenna");
	m_AlphaList.InsertAtEnd("blygd");
	m_AlphaList.InsertAtEnd("bl");
	m_AlphaList.InsertAtEnd("blbr");
	m_AlphaList.InsertAtEnd("blhake");
	m_AlphaList.InsertAtEnd("blmrke");
	m_AlphaList.InsertAtEnd("blsa");
	m_AlphaList.InsertAtEnd("blser");
	m_AlphaList.InsertAtEnd("blsig");
	m_AlphaList.InsertAtEnd("blst");
	m_AlphaList.InsertAtEnd("blckfisk");
	m_AlphaList.InsertAtEnd("blda");
	m_AlphaList.InsertAtEnd("blder");
	m_AlphaList.InsertAtEnd("blja");
	m_AlphaList.InsertAtEnd("bljor");
	m_AlphaList.InsertAtEnd("blta");
	m_AlphaList.InsertAtEnd("blter");
	m_AlphaList.InsertAtEnd("boccia");
	m_AlphaList.InsertAtEnd("bock");
	m_AlphaList.InsertAtEnd("boende");
	m_AlphaList.InsertAtEnd("bogsera");
	m_AlphaList.InsertAtEnd("boj");
	m_AlphaList.InsertAtEnd("bok");
	m_AlphaList.InsertAtEnd("bokbuss");
	m_AlphaList.InsertAtEnd("bokhandel");
	m_AlphaList.InsertAtEnd("bokmrke");
	m_AlphaList.InsertAtEnd("bokstav");
	m_AlphaList.InsertAtEnd("bokstver");
	m_AlphaList.InsertAtEnd("boll");
	m_AlphaList.InsertAtEnd("bom");
	m_AlphaList.InsertAtEnd("bomb");
	m_AlphaList.InsertAtEnd("bomull");
	m_AlphaList.InsertAtEnd("bonde");
	m_AlphaList.InsertAtEnd("bondgrd");
	m_AlphaList.InsertAtEnd("bor");
	m_AlphaList.InsertAtEnd("bord");
	m_AlphaList.InsertAtEnd("borde");
	m_AlphaList.InsertAtEnd("bordet");
	m_AlphaList.InsertAtEnd("bordsben");
	m_AlphaList.InsertAtEnd("bordsfotboll");
	m_AlphaList.InsertAtEnd("bordshockey");
	m_AlphaList.InsertAtEnd("bordsspel");
	m_AlphaList.InsertAtEnd("bordtennis");
	m_AlphaList.InsertAtEnd("borg");
	m_AlphaList.InsertAtEnd("borgerliga");
	m_AlphaList.InsertAtEnd("borgmstare");
	m_AlphaList.InsertAtEnd("borr");
	m_AlphaList.InsertAtEnd("borra");
	m_AlphaList.InsertAtEnd("borrdrill");
	m_AlphaList.InsertAtEnd("borrmaskin");
	m_AlphaList.InsertAtEnd("borrsvng");
	m_AlphaList.InsertAtEnd("borsta");
	m_AlphaList.InsertAtEnd("borstar");
	m_AlphaList.InsertAtEnd("borste");
	m_AlphaList.InsertAtEnd("borsten");
	m_AlphaList.InsertAtEnd("bort");
	m_AlphaList.InsertAtEnd("borta");
	m_AlphaList.InsertAtEnd("bortkommen");
	m_AlphaList.InsertAtEnd("bortom");
	m_AlphaList.InsertAtEnd("borttappad");
	m_AlphaList.InsertAtEnd("botten");
	m_AlphaList.InsertAtEnd("boule");
	m_AlphaList.InsertAtEnd("bowling");
	m_AlphaList.InsertAtEnd("bowlingbana");
	m_AlphaList.InsertAtEnd("bowlingklot");
	m_AlphaList.InsertAtEnd("bowlingkgla");
	m_AlphaList.InsertAtEnd("boxare");
	m_AlphaList.InsertAtEnd("boxning");
	m_AlphaList.InsertAtEnd("bra");
	m_AlphaList.InsertAtEnd("brandbil");
	m_AlphaList.InsertAtEnd("brandfarligt");
	m_AlphaList.InsertAtEnd("brandlarm");
	m_AlphaList.InsertAtEnd("brandman");
	m_AlphaList.InsertAtEnd("brandmanshjlm");
	m_AlphaList.InsertAtEnd("brandmansrock");
	m_AlphaList.InsertAtEnd("brandmansuniform");
	m_AlphaList.InsertAtEnd("brandpost");
	m_AlphaList.InsertAtEnd("brandstation");
	m_AlphaList.InsertAtEnd("brasa");
	m_AlphaList.InsertAtEnd("bred");
	m_AlphaList.InsertAtEnd("breda");
	m_AlphaList.InsertAtEnd("bredbart");
	m_AlphaList.InsertAtEnd("bredvid");
	m_AlphaList.InsertAtEnd("brer");
	m_AlphaList.InsertAtEnd("brev");
	m_AlphaList.InsertAtEnd("brevbrare");
	m_AlphaList.InsertAtEnd("brevbrarmssa");
	m_AlphaList.InsertAtEnd("brevbraruniform");
	m_AlphaList.InsertAtEnd("brevlda");
	m_AlphaList.InsertAtEnd("brevldeflagga");
	m_AlphaList.InsertAtEnd("bricka");
	m_AlphaList.InsertAtEnd("brinna");
	m_AlphaList.InsertAtEnd("brinnande");
	m_AlphaList.InsertAtEnd("brinner");
	m_AlphaList.InsertAtEnd("bro");
	m_AlphaList.InsertAtEnd("broccoli");
	m_AlphaList.InsertAtEnd("broder");
	m_AlphaList.InsertAtEnd("brodera");
	m_AlphaList.InsertAtEnd("broderar");
	m_AlphaList.InsertAtEnd("bromsa");
	m_AlphaList.InsertAtEnd("bror");
	m_AlphaList.InsertAtEnd("brorsdotter");
	m_AlphaList.InsertAtEnd("brorson");
	m_AlphaList.InsertAtEnd("brosch");
	m_AlphaList.InsertAtEnd("brott");
	m_AlphaList.InsertAtEnd("brottning");
	m_AlphaList.InsertAtEnd("brud");
	m_AlphaList.InsertAtEnd("brudgum");
	m_AlphaList.InsertAtEnd("brudklnning");
	m_AlphaList.InsertAtEnd("brudnbb");
	m_AlphaList.InsertAtEnd("brudpar");
	m_AlphaList.InsertAtEnd("brudtrna");
	m_AlphaList.InsertAtEnd("brukade");
	m_AlphaList.InsertAtEnd("brukar");
	m_AlphaList.InsertAtEnd("bruksanvisning");
	m_AlphaList.InsertAtEnd("brumma");
	m_AlphaList.InsertAtEnd("brummar");
	m_AlphaList.InsertAtEnd("brun");
	m_AlphaList.InsertAtEnd("brunett");
	m_AlphaList.InsertAtEnd("brunn");
	m_AlphaList.InsertAtEnd("bry ");
	m_AlphaList.InsertAtEnd("brygga");
	m_AlphaList.InsertAtEnd("brygger");
	m_AlphaList.InsertAtEnd("bryr");
	m_AlphaList.InsertAtEnd("brysselkl");
	m_AlphaList.InsertAtEnd("bryta");
	m_AlphaList.InsertAtEnd("bryter");
	m_AlphaList.InsertAtEnd("brk");
	m_AlphaList.InsertAtEnd("brda");
	m_AlphaList.InsertAtEnd("brdspel");
	m_AlphaList.InsertAtEnd("brnd");
	m_AlphaList.InsertAtEnd("brnna");
	m_AlphaList.InsertAtEnd("brnner");
	m_AlphaList.InsertAtEnd("brnsle");
	m_AlphaList.InsertAtEnd("brd");
	m_AlphaList.InsertAtEnd("brdkant");
	m_AlphaList.InsertAtEnd("brdkavel");
	m_AlphaList.InsertAtEnd("brdkorg");
	m_AlphaList.InsertAtEnd("brdpensel");
	m_AlphaList.InsertAtEnd("brdpinnar");
	m_AlphaList.InsertAtEnd("brdrost");
	m_AlphaList.InsertAtEnd("brdskiva");
	m_AlphaList.InsertAtEnd("brdsmulor");
	m_AlphaList.InsertAtEnd("brllop");
	m_AlphaList.InsertAtEnd("brst");
	m_AlphaList.InsertAtEnd("brstband");
	m_AlphaList.InsertAtEnd("brstkorg");
	m_AlphaList.InsertAtEnd("brstsmrtor");
	m_AlphaList.InsertAtEnd("brstvrtor");
	m_AlphaList.InsertAtEnd("bubbelbad");
	m_AlphaList.InsertAtEnd("bubbla");
	m_AlphaList.InsertAtEnd("bubblar ");
	m_AlphaList.InsertAtEnd("bubblor");
	m_AlphaList.InsertAtEnd("buckla");
	m_AlphaList.InsertAtEnd("bucklar");
	m_AlphaList.InsertAtEnd("buddist");
	m_AlphaList.InsertAtEnd("budgeten");
	m_AlphaList.InsertAtEnd("budord");
	m_AlphaList.InsertAtEnd("budskap");
	m_AlphaList.InsertAtEnd("buffel");
	m_AlphaList.InsertAtEnd("bukett");
	m_AlphaList.InsertAtEnd("bukt");
	m_AlphaList.InsertAtEnd("bula");
	m_AlphaList.InsertAtEnd("bulldozer");
	m_AlphaList.InsertAtEnd("bulle");
	m_AlphaList.InsertAtEnd("bult");
	m_AlphaList.InsertAtEnd("bungee jump");
	m_AlphaList.InsertAtEnd("bunsenbrnnare");
	m_AlphaList.InsertAtEnd("bur");
	m_AlphaList.InsertAtEnd("burk");
	m_AlphaList.InsertAtEnd("bus");
	m_AlphaList.InsertAtEnd("buske");
	m_AlphaList.InsertAtEnd("buss");
	m_AlphaList.InsertAtEnd("busschauffr");
	m_AlphaList.InsertAtEnd("bussdrr");
	m_AlphaList.InsertAtEnd("bussdrrar");
	m_AlphaList.InsertAtEnd("bussen");
	m_AlphaList.InsertAtEnd("bussfrare");
	m_AlphaList.InsertAtEnd("busshllplats");
	m_AlphaList.InsertAtEnd("busskort");
	m_AlphaList.InsertAtEnd("busstation");
	m_AlphaList.InsertAtEnd("busstidtabell");
	m_AlphaList.InsertAtEnd("busstur");
	m_AlphaList.InsertAtEnd("butter");
	m_AlphaList.InsertAtEnd("byff");
	m_AlphaList.InsertAtEnd("bygga");
	m_AlphaList.InsertAtEnd("bygger");
	m_AlphaList.InsertAtEnd("byggklotsar");
	m_AlphaList.InsertAtEnd("byggnad");
	m_AlphaList.InsertAtEnd("byggplats");
	m_AlphaList.InsertAtEnd("byr");
	m_AlphaList.InsertAtEnd("byrlda");
	m_AlphaList.InsertAtEnd("byta");
	m_AlphaList.InsertAtEnd("byter");
	m_AlphaList.InsertAtEnd("byxor");
	m_AlphaList.InsertAtEnd("byxorna");
	m_AlphaList.InsertAtEnd("bda");
	m_AlphaList.InsertAtEnd("bde");
	m_AlphaList.InsertAtEnd("bge");
	m_AlphaList.InsertAtEnd("bgskytt");
	m_AlphaList.InsertAtEnd("bgskytte");
	m_AlphaList.InsertAtEnd("bl");
	m_AlphaList.InsertAtEnd("brd");
	m_AlphaList.InsertAtEnd("bt");
	m_AlphaList.InsertAtEnd("bck");
	m_AlphaList.InsertAtEnd("bcken");
	m_AlphaList.InsertAtEnd("bdda");
	
}

void CWordAid2Dlg::LoadList3()
{
		m_AlphaList.InsertAtEnd("bddar");
	m_AlphaList.InsertAtEnd("bgare");
	m_AlphaList.InsertAtEnd("blta");
	m_AlphaList.InsertAtEnd("blte");
	m_AlphaList.InsertAtEnd("bltesvska");
	m_AlphaList.InsertAtEnd("bnk");
	m_AlphaList.InsertAtEnd("bnken");
	m_AlphaList.InsertAtEnd("bnktving");
	m_AlphaList.InsertAtEnd("br");
	m_AlphaList.InsertAtEnd("bra");
	m_AlphaList.InsertAtEnd("brbar");
	m_AlphaList.InsertAtEnd("brgningsbil");
	m_AlphaList.InsertAtEnd("bst");
	m_AlphaList.InsertAtEnd("bttre");
	m_AlphaList.InsertAtEnd("bver");
	m_AlphaList.InsertAtEnd("bj");
	m_AlphaList.InsertAtEnd("bja");
	m_AlphaList.InsertAtEnd("bjd");
	m_AlphaList.InsertAtEnd("bjer");
	m_AlphaList.InsertAtEnd("bna");
	m_AlphaList.InsertAtEnd("bnbok");
	m_AlphaList.InsertAtEnd("bnesjal");
	m_AlphaList.InsertAtEnd("bngroddar");
	m_AlphaList.InsertAtEnd("bnor");
	m_AlphaList.InsertAtEnd("bnstjlk");
	m_AlphaList.InsertAtEnd("brja");
	m_AlphaList.InsertAtEnd("brjan");
	m_AlphaList.InsertAtEnd("brjar");
	m_AlphaList.InsertAtEnd("cabriolet");
	m_AlphaList.InsertAtEnd("caf");
	m_AlphaList.InsertAtEnd("campingbil");
	m_AlphaList.InsertAtEnd("campingplats");
	m_AlphaList.InsertAtEnd("cape");
	m_AlphaList.InsertAtEnd("cd-spelare");
	m_AlphaList.InsertAtEnd("cello");
	m_AlphaList.InsertAtEnd("cement");
	m_AlphaList.InsertAtEnd("cementblandare");
	m_AlphaList.InsertAtEnd("central");
	m_AlphaList.InsertAtEnd("centralvrme");
	m_AlphaList.InsertAtEnd("centrum");
	m_AlphaList.InsertAtEnd("cerat");
	m_AlphaList.InsertAtEnd("certifikat");
	m_AlphaList.InsertAtEnd("chauffr");
	m_AlphaList.InsertAtEnd("check");
	m_AlphaList.InsertAtEnd("chef");
	m_AlphaList.InsertAtEnd("chili");
	m_AlphaList.InsertAtEnd("chips");
	m_AlphaList.InsertAtEnd("choklad");
	m_AlphaList.InsertAtEnd("chokladdryck ");
	m_AlphaList.InsertAtEnd("chokladflingor");
	m_AlphaList.InsertAtEnd("chokladkaka");
	m_AlphaList.InsertAtEnd("chokladpulver");
	m_AlphaList.InsertAtEnd("chokladstrssel");
	m_AlphaList.InsertAtEnd("chokladss");
	m_AlphaList.InsertAtEnd("chokladgg");
	m_AlphaList.InsertAtEnd("cider");
	m_AlphaList.InsertAtEnd("cigarett");
	m_AlphaList.InsertAtEnd("cigarettndare");
	m_AlphaList.InsertAtEnd("cigarr");
	m_AlphaList.InsertAtEnd("cirka");
	m_AlphaList.InsertAtEnd("cirkel");
	m_AlphaList.InsertAtEnd("cirkeldiagram");
	m_AlphaList.InsertAtEnd("cirkulera");
	m_AlphaList.InsertAtEnd("cirkulerar");
	m_AlphaList.InsertAtEnd("cirkus");
	m_AlphaList.InsertAtEnd("cirkusdirektr");
	m_AlphaList.InsertAtEnd("citron");
	m_AlphaList.InsertAtEnd("citronsaft");
	m_AlphaList.InsertAtEnd("citrusfrukt");
	m_AlphaList.InsertAtEnd("clown");
	m_AlphaList.InsertAtEnd("coach");
	m_AlphaList.InsertAtEnd("cola");
	m_AlphaList.InsertAtEnd("container");
	m_AlphaList.InsertAtEnd("cornflakes");
	m_AlphaList.InsertAtEnd("countrymusik");
	m_AlphaList.InsertAtEnd("cowboy");
	m_AlphaList.InsertAtEnd("cykel");
	m_AlphaList.InsertAtEnd("cykelbana");
	m_AlphaList.InsertAtEnd("cykeldck");
	m_AlphaList.InsertAtEnd("cykelpump");
	m_AlphaList.InsertAtEnd("cykelringklocka");
	m_AlphaList.InsertAtEnd("cykelslang");
	m_AlphaList.InsertAtEnd("cykelstll");
	m_AlphaList.InsertAtEnd("cykla");
	m_AlphaList.InsertAtEnd("cyklar");
	m_AlphaList.InsertAtEnd("cyklingsfrbud");
	m_AlphaList.InsertAtEnd("cymbaler");
	m_AlphaList.InsertAtEnd("dadel");
	m_AlphaList.InsertAtEnd("dag");
	m_AlphaList.InsertAtEnd("dagar");
	m_AlphaList.InsertAtEnd("dagbok");
	m_AlphaList.InsertAtEnd("dagcenter");
	m_AlphaList.InsertAtEnd("dagdrmma");
	m_AlphaList.InsertAtEnd("dagdrmmer");
	m_AlphaList.InsertAtEnd("dagen");
	m_AlphaList.InsertAtEnd("dagens");
	m_AlphaList.InsertAtEnd("daghem");
	m_AlphaList.InsertAtEnd("daglger");
	m_AlphaList.InsertAtEnd("dags");
	m_AlphaList.InsertAtEnd("dagstidning");
	m_AlphaList.InsertAtEnd("dagvrd");
	m_AlphaList.InsertAtEnd("dal");
	m_AlphaList.InsertAtEnd("dalmatin");
	m_AlphaList.InsertAtEnd("dam");
	m_AlphaList.InsertAtEnd("damm");
	m_AlphaList.InsertAtEnd("damma");
	m_AlphaList.InsertAtEnd("dammar");
	m_AlphaList.InsertAtEnd("dammsuga");
	m_AlphaList.InsertAtEnd("dammsugare");
	m_AlphaList.InsertAtEnd("dammsuger");
	m_AlphaList.InsertAtEnd("dammtrasa");
	m_AlphaList.InsertAtEnd("dammvippa");
	m_AlphaList.InsertAtEnd("damspel");
	m_AlphaList.InsertAtEnd("damspelsbrde");
	m_AlphaList.InsertAtEnd("dans");
	m_AlphaList.InsertAtEnd("dansa");
	m_AlphaList.InsertAtEnd("dansar");
	m_AlphaList.InsertAtEnd("dansare");
	m_AlphaList.InsertAtEnd("dansmelodier");
	m_AlphaList.InsertAtEnd("dansstlle");
	m_AlphaList.InsertAtEnd("darra");
	m_AlphaList.InsertAtEnd("darrar");
	m_AlphaList.InsertAtEnd("dart");
	m_AlphaList.InsertAtEnd("daska");
	m_AlphaList.InsertAtEnd("daskar");
	m_AlphaList.InsertAtEnd("datalra");
	m_AlphaList.InsertAtEnd("dataspel");
	m_AlphaList.InsertAtEnd("dator");
	m_AlphaList.InsertAtEnd("datorspel");
	m_AlphaList.InsertAtEnd("datum");
	m_AlphaList.InsertAtEnd("de");
	m_AlphaList.InsertAtEnd("december");
	m_AlphaList.InsertAtEnd("deg");
	m_AlphaList.InsertAtEnd("degen");
	m_AlphaList.InsertAtEnd("dej");
	m_AlphaList.InsertAtEnd("dekoration");
	m_AlphaList.InsertAtEnd("dekorera");
	m_AlphaList.InsertAtEnd("dekorerar");
	m_AlphaList.InsertAtEnd("del");
	m_AlphaList.InsertAtEnd("dela");
	m_AlphaList.InsertAtEnd("delar");
	m_AlphaList.InsertAtEnd("delen");
	m_AlphaList.InsertAtEnd("delfin");
	m_AlphaList.InsertAtEnd("dels");
	m_AlphaList.InsertAtEnd("delvis");
	m_AlphaList.InsertAtEnd("dem");
	m_AlphaList.InsertAtEnd("demonstration");
	m_AlphaList.InsertAtEnd("demonstrera");
	m_AlphaList.InsertAtEnd("demonstrerar");
	m_AlphaList.InsertAtEnd("den");
	m_AlphaList.InsertAtEnd("denna");
	m_AlphaList.InsertAtEnd("deodorant");
	m_AlphaList.InsertAtEnd("depression");
	m_AlphaList.InsertAtEnd("deras");
	m_AlphaList.InsertAtEnd("dess");
	m_AlphaList.InsertAtEnd("dessa");
	m_AlphaList.InsertAtEnd("dessert");
	m_AlphaList.InsertAtEnd("dessertskl");
	m_AlphaList.InsertAtEnd("dessutom");
	m_AlphaList.InsertAtEnd("det");
	m_AlphaList.InsertAtEnd("detta");
	m_AlphaList.InsertAtEnd("diabild");
	m_AlphaList.InsertAtEnd("diagram");
	m_AlphaList.InsertAtEnd("diamant");
	m_AlphaList.InsertAtEnd("diarr");
	m_AlphaList.InsertAtEnd("diet");
	m_AlphaList.InsertAtEnd("dietist");
	m_AlphaList.InsertAtEnd("dig");
	m_AlphaList.InsertAtEnd("digitaliseringsbord");
	m_AlphaList.InsertAtEnd("dikt");
	m_AlphaList.InsertAtEnd("diligens");
	m_AlphaList.InsertAtEnd("dimma");
	m_AlphaList.InsertAtEnd("dimmig");
	m_AlphaList.InsertAtEnd("din");
	m_AlphaList.InsertAtEnd("dina");
	m_AlphaList.InsertAtEnd("dinosaurie");
	m_AlphaList.InsertAtEnd("diplom");
	m_AlphaList.InsertAtEnd("direkt");
	m_AlphaList.InsertAtEnd("dirigent");
	m_AlphaList.InsertAtEnd("disk");
	m_AlphaList.InsertAtEnd("diska");
	m_AlphaList.InsertAtEnd("diskar");
	m_AlphaList.InsertAtEnd("diskbalja");
	m_AlphaList.InsertAtEnd("diskborste");
	m_AlphaList.InsertAtEnd("disken");
	m_AlphaList.InsertAtEnd("diskett");
	m_AlphaList.InsertAtEnd("diskettenhet");
	m_AlphaList.InsertAtEnd("diskhandduk");
	m_AlphaList.InsertAtEnd("diskjockey");
	m_AlphaList.InsertAtEnd("diskmaskin");
	m_AlphaList.InsertAtEnd("diskmaskinen");
	m_AlphaList.InsertAtEnd("diskmedel");
	m_AlphaList.InsertAtEnd("diskotek");
	m_AlphaList.InsertAtEnd("diskstll");
	m_AlphaList.InsertAtEnd("disktrasa");
	m_AlphaList.InsertAtEnd("diskussion");
	m_AlphaList.InsertAtEnd("diskutera");
	m_AlphaList.InsertAtEnd("diskuterar");
	m_AlphaList.InsertAtEnd("dissekera");
	m_AlphaList.InsertAtEnd("dissekerar");
	m_AlphaList.InsertAtEnd("dit");
	m_AlphaList.InsertAtEnd("ditt");
	m_AlphaList.InsertAtEnd("dividera");
	m_AlphaList.InsertAtEnd("division");
	m_AlphaList.InsertAtEnd("djungel");
	m_AlphaList.InsertAtEnd("djup");
	m_AlphaList.InsertAtEnd("djupfryst");
	m_AlphaList.InsertAtEnd("djupt");
	m_AlphaList.InsertAtEnd("djur");
	m_AlphaList.InsertAtEnd("djurhalsband");
	m_AlphaList.InsertAtEnd("djurhud");
	m_AlphaList.InsertAtEnd("djurhus");
	m_AlphaList.InsertAtEnd("djurpark");
	m_AlphaList.InsertAtEnd("djursjukhus");
	m_AlphaList.InsertAtEnd("djursktare");
	m_AlphaList.InsertAtEnd("djurunge");
	m_AlphaList.InsertAtEnd("djurutstllning");
	m_AlphaList.InsertAtEnd("djvul");
	m_AlphaList.InsertAtEnd("dock");
	m_AlphaList.InsertAtEnd("docka");
	m_AlphaList.InsertAtEnd("dockskp");
	m_AlphaList.InsertAtEnd("dockteater");
	m_AlphaList.InsertAtEnd("dockvagn");
	m_AlphaList.InsertAtEnd("doktor");
	m_AlphaList.InsertAtEnd("doktorsvska");
	m_AlphaList.InsertAtEnd("dokument");
	m_AlphaList.InsertAtEnd("dokumentfrstrare");
	m_AlphaList.InsertAtEnd("dolk");
	m_AlphaList.InsertAtEnd("dollar");
	m_AlphaList.InsertAtEnd("dom");
	m_AlphaList.InsertAtEnd("domare");
	m_AlphaList.InsertAtEnd("domino");
	m_AlphaList.InsertAtEnd("domstol");
	m_AlphaList.InsertAtEnd("dop");
	m_AlphaList.InsertAtEnd("dopfunt");
	m_AlphaList.InsertAtEnd("dopklnning");
	m_AlphaList.InsertAtEnd("dotter");
	m_AlphaList.InsertAtEnd("dra");
	m_AlphaList.InsertAtEnd("dra ");
	m_AlphaList.InsertAtEnd("drag");
	m_AlphaList.InsertAtEnd("dragkedja");
	m_AlphaList.InsertAtEnd("dragleksak");
	m_AlphaList.InsertAtEnd("dragspel");
	m_AlphaList.InsertAtEnd("drake");
	m_AlphaList.InsertAtEnd("drama");
	m_AlphaList.InsertAtEnd("dramatik");
	m_AlphaList.InsertAtEnd("drar");
	m_AlphaList.InsertAtEnd("dregla");
	m_AlphaList.InsertAtEnd("dreglar");
	m_AlphaList.InsertAtEnd("dribbla");
	m_AlphaList.InsertAtEnd("dribblar");
	m_AlphaList.InsertAtEnd("dricka");
	m_AlphaList.InsertAtEnd("dricker");
	m_AlphaList.InsertAtEnd("drillflicka");
	m_AlphaList.InsertAtEnd("drink");
	m_AlphaList.InsertAtEnd("drinklock");
	m_AlphaList.InsertAtEnd("driva");
	m_AlphaList.InsertAtEnd("drive-inrestaurang");
	m_AlphaList.InsertAtEnd("drog");
	m_AlphaList.InsertAtEnd("drogfrbud");
	m_AlphaList.InsertAtEnd("dromedar");
	m_AlphaList.InsertAtEnd("dropp");
	m_AlphaList.InsertAtEnd("droppa");
	m_AlphaList.InsertAtEnd("droppar");
	m_AlphaList.InsertAtEnd("droppstllning");
	m_AlphaList.InsertAtEnd("droppstllningen");
	m_AlphaList.InsertAtEnd("drottning");
	m_AlphaList.InsertAtEnd("drunkna");
	m_AlphaList.InsertAtEnd("druvjuice ");
	m_AlphaList.InsertAtEnd("druvor");
	m_AlphaList.InsertAtEnd("dryck");
	m_AlphaList.InsertAtEnd("dryckeskrl");
	m_AlphaList.InsertAtEnd("drkt");
	m_AlphaList.InsertAtEnd("drm");
	m_AlphaList.InsertAtEnd("drmma");
	m_AlphaList.InsertAtEnd("drmmer");
	m_AlphaList.InsertAtEnd("du");
	m_AlphaList.InsertAtEnd("dubbel");
	m_AlphaList.InsertAtEnd("dubbeldeckare");
	m_AlphaList.InsertAtEnd("dubbelklicka");
	m_AlphaList.InsertAtEnd("dubbelklickar");
	m_AlphaList.InsertAtEnd("duk");
	m_AlphaList.InsertAtEnd("duka");
	m_AlphaList.InsertAtEnd("dukar");
	m_AlphaList.InsertAtEnd("dukning");
	m_AlphaList.InsertAtEnd("duktigt");
	m_AlphaList.InsertAtEnd("dum");
	m_AlphaList.InsertAtEnd("dun");
	m_AlphaList.InsertAtEnd("dunge");
	m_AlphaList.InsertAtEnd("duntcke");
	m_AlphaList.InsertAtEnd("durga");
	m_AlphaList.InsertAtEnd("durkslag");
	m_AlphaList.InsertAtEnd("dusch");
	m_AlphaList.InsertAtEnd("duscha");
	m_AlphaList.InsertAtEnd("duschar ");
	m_AlphaList.InsertAtEnd("duschbnk");
	m_AlphaList.InsertAtEnd("duschgel");
	m_AlphaList.InsertAtEnd("duschmssa");
	m_AlphaList.InsertAtEnd("duva");
	m_AlphaList.InsertAtEnd("DVD");
	m_AlphaList.InsertAtEnd("DVD-spelare");
	m_AlphaList.InsertAtEnd("dyka");
	m_AlphaList.InsertAtEnd("dykare");
	m_AlphaList.InsertAtEnd("dyker");
	m_AlphaList.InsertAtEnd("dykring");
	m_AlphaList.InsertAtEnd("dyr");
	m_AlphaList.InsertAtEnd("d");
	m_AlphaList.InsertAtEnd("dlig");
	m_AlphaList.InsertAtEnd("dligt ");
	m_AlphaList.InsertAtEnd("dck");
	m_AlphaList.InsertAtEnd("dr");
	m_AlphaList.InsertAtEnd("drefter");
	m_AlphaList.InsertAtEnd("dremot");
	m_AlphaList.InsertAtEnd("drfr");
	m_AlphaList.InsertAtEnd("drmed");
	m_AlphaList.InsertAtEnd("dd");
	m_AlphaList.InsertAtEnd("dda");
	m_AlphaList.InsertAtEnd("ddar");
	m_AlphaList.InsertAtEnd("ddlig");
	m_AlphaList.InsertAtEnd("ddskalle");
	m_AlphaList.InsertAtEnd("dpa");
	m_AlphaList.InsertAtEnd("dpare");
	m_AlphaList.InsertAtEnd("dper");
	m_AlphaList.InsertAtEnd("drr");
	m_AlphaList.InsertAtEnd("drren");
	m_AlphaList.InsertAtEnd("drrhandtag");
	m_AlphaList.InsertAtEnd("drrklocka");
	m_AlphaList.InsertAtEnd("drrklockan");
	m_AlphaList.InsertAtEnd("drrknopp");
	m_AlphaList.InsertAtEnd("drrmatta");
	m_AlphaList.InsertAtEnd("drrvaktmstare");
	m_AlphaList.InsertAtEnd("dv");
	m_AlphaList.InsertAtEnd("dvhet");
	m_AlphaList.InsertAtEnd("effekt");
	m_AlphaList.InsertAtEnd("efter");
	m_AlphaList.InsertAtEnd("efterfrga");
	m_AlphaList.InsertAtEnd("efterfrgar");
	m_AlphaList.InsertAtEnd("eftermiddag");
	m_AlphaList.InsertAtEnd("efterrtt");
	m_AlphaList.InsertAtEnd("eftersom");
	m_AlphaList.InsertAtEnd("eftert");
	m_AlphaList.InsertAtEnd("egen");
	m_AlphaList.InsertAtEnd("egentligen");
	m_AlphaList.InsertAtEnd("eget");
	m_AlphaList.InsertAtEnd("egna");
	m_AlphaList.InsertAtEnd("ej");
	m_AlphaList.InsertAtEnd("eka");
	m_AlphaList.InsertAtEnd("eklv");
	m_AlphaList.InsertAtEnd("eko");
	m_AlphaList.InsertAtEnd("ekollon");
	m_AlphaList.InsertAtEnd("ekonomisk");
	m_AlphaList.InsertAtEnd("ekonomiska");
	m_AlphaList.InsertAtEnd("ekorre");
	m_AlphaList.InsertAtEnd("elak");
	m_AlphaList.InsertAtEnd("elastisk");
	m_AlphaList.InsertAtEnd("elastiskt");
	m_AlphaList.InsertAtEnd("eld");
	m_AlphaList.InsertAtEnd("eldfarlig");
	m_AlphaList.InsertAtEnd("eldfluga");
	m_AlphaList.InsertAtEnd("eldslckare");
	m_AlphaList.InsertAtEnd("eldsvda");
	m_AlphaList.InsertAtEnd("elefant");
	m_AlphaList.InsertAtEnd("elektricitet");
	m_AlphaList.InsertAtEnd("elektrisk");
	m_AlphaList.InsertAtEnd("elektriska");
	m_AlphaList.InsertAtEnd("elektromagnetism");
	m_AlphaList.InsertAtEnd("elev");
	m_AlphaList.InsertAtEnd("elkontakt");
	m_AlphaList.InsertAtEnd("eller");
	m_AlphaList.InsertAtEnd("elrullstol");
	m_AlphaList.InsertAtEnd("elva");
	m_AlphaList.InsertAtEnd("embryo");
	m_AlphaList.InsertAtEnd("emellertid");
	m_AlphaList.InsertAtEnd("emot");
	m_AlphaList.InsertAtEnd("en");
	m_AlphaList.InsertAtEnd("ena");
	m_AlphaList.InsertAtEnd("enbart");
	m_AlphaList.InsertAtEnd("enda");
	m_AlphaList.InsertAtEnd("endast");
	m_AlphaList.InsertAtEnd("endiv");
	m_AlphaList.InsertAtEnd("energi");
	m_AlphaList.InsertAtEnd("enhet");
	m_AlphaList.InsertAtEnd("enighet");
	m_AlphaList.InsertAtEnd("enkel");
	m_AlphaList.InsertAtEnd("enkelriktat");
	m_AlphaList.InsertAtEnd("enkelt");
	m_AlphaList.InsertAtEnd("enligt");
	m_AlphaList.InsertAtEnd("ens");
	m_AlphaList.InsertAtEnd("ensam");
	m_AlphaList.InsertAtEnd("entr");
	m_AlphaList.InsertAtEnd("entrdrr");
	m_AlphaList.InsertAtEnd("envis");
	m_AlphaList.InsertAtEnd("epilepsi");
	m_AlphaList.InsertAtEnd("e-post");
	m_AlphaList.InsertAtEnd("er");
	m_AlphaList.InsertAtEnd("era");
	m_AlphaList.InsertAtEnd("ergonomi");
	m_AlphaList.InsertAtEnd("erhlla");
	m_AlphaList.InsertAtEnd("erknna");
	m_AlphaList.InsertAtEnd("erknner");
	m_AlphaList.InsertAtEnd("erstta");
	m_AlphaList.InsertAtEnd("erstter");
	m_AlphaList.InsertAtEnd("ert");
	m_AlphaList.InsertAtEnd("eskim");
	m_AlphaList.InsertAtEnd("etikett");
	m_AlphaList.InsertAtEnd("etikettbok");
	m_AlphaList.InsertAtEnd("etiketter");
	m_AlphaList.InsertAtEnd("ett");
	m_AlphaList.InsertAtEnd("etta");
	m_AlphaList.InsertAtEnd("euro");
	m_AlphaList.InsertAtEnd("evig");
	m_AlphaList.InsertAtEnd("evigt");
	m_AlphaList.InsertAtEnd("examen");
	m_AlphaList.InsertAtEnd("exempel");
	m_AlphaList.InsertAtEnd("exemplar");
	m_AlphaList.InsertAtEnd("expedit");
	m_AlphaList.InsertAtEnd("experiment");
	m_AlphaList.InsertAtEnd("explodera");
	m_AlphaList.InsertAtEnd("exploderar");
	m_AlphaList.InsertAtEnd("explosion");
	m_AlphaList.InsertAtEnd("extra");
	m_AlphaList.InsertAtEnd("fabrik");
	m_AlphaList.InsertAtEnd("fack");
	m_AlphaList.InsertAtEnd("faktiskt");
	m_AlphaList.InsertAtEnd("faktum");
	m_AlphaList.InsertAtEnd("falafel");
	m_AlphaList.InsertAtEnd("fall");
	m_AlphaList.InsertAtEnd("falla");
	m_AlphaList.InsertAtEnd("faller");
	m_AlphaList.InsertAtEnd("fallet");
	m_AlphaList.InsertAtEnd("fallskrm");
	m_AlphaList.InsertAtEnd("fallskrmsjgare");
	m_AlphaList.InsertAtEnd("familj");
	m_AlphaList.InsertAtEnd("familjeplanering");
	m_AlphaList.InsertAtEnd("fann");
	m_AlphaList.InsertAtEnd("fanns");
	m_AlphaList.InsertAtEnd("fantasi");
	m_AlphaList.InsertAtEnd("fantisera");
	m_AlphaList.InsertAtEnd("fantiserar");
	m_AlphaList.InsertAtEnd("far");
	m_AlphaList.InsertAtEnd("fara");
	m_AlphaList.InsertAtEnd("farbror");
	m_AlphaList.InsertAtEnd("farfar");
	m_AlphaList.InsertAtEnd("farfrlder");
	m_AlphaList.InsertAtEnd("farinsocker");
	m_AlphaList.InsertAtEnd("farlig ");
	m_AlphaList.InsertAtEnd("farmor");
	m_AlphaList.InsertAtEnd("farvl");
	m_AlphaList.InsertAtEnd("fasan");
	m_AlphaList.InsertAtEnd("fast");
	m_AlphaList.InsertAtEnd("fasta");
	m_AlphaList.InsertAtEnd("fastan");
	m_AlphaList.InsertAtEnd("fastar");
	m_AlphaList.InsertAtEnd("fastedag");
	m_AlphaList.InsertAtEnd("fastlagen");
	m_AlphaList.InsertAtEnd("fastn");
	m_AlphaList.InsertAtEnd("fat");
	m_AlphaList.InsertAtEnd("fattig");
	m_AlphaList.InsertAtEnd("favorit");
	m_AlphaList.InsertAtEnd("fax");
	m_AlphaList.InsertAtEnd("feber");
	m_AlphaList.InsertAtEnd("febertermometer");
	m_AlphaList.InsertAtEnd("februari");
	m_AlphaList.InsertAtEnd("fel");
	m_AlphaList.InsertAtEnd("fem");
	m_AlphaList.InsertAtEnd("femtio");
	m_AlphaList.InsertAtEnd("femton");
	m_AlphaList.InsertAtEnd("fena");
	m_AlphaList.InsertAtEnd("fest");
	m_AlphaList.InsertAtEnd("festhatt");
	m_AlphaList.InsertAtEnd("festpse");
	m_AlphaList.InsertAtEnd("festvning");
	m_AlphaList.InsertAtEnd("fet");
	m_AlphaList.InsertAtEnd("fett");
	m_AlphaList.InsertAtEnd("fettisdag");
	m_AlphaList.InsertAtEnd("fiber");
	m_AlphaList.InsertAtEnd("fick");
	m_AlphaList.InsertAtEnd("ficka");
	m_AlphaList.InsertAtEnd("fickkniv");
	m_AlphaList.InsertAtEnd("ficklampa");
	m_AlphaList.InsertAtEnd("fickorna");
	m_AlphaList.InsertAtEnd("fickpengar");
	m_AlphaList.InsertAtEnd("figursg");
	m_AlphaList.InsertAtEnd("fil");
	m_AlphaList.InsertAtEnd("fila");
	m_AlphaList.InsertAtEnd("filar");
	m_AlphaList.InsertAtEnd("film");
	m_AlphaList.InsertAtEnd("filma");
	m_AlphaList.InsertAtEnd("filmar");
	m_AlphaList.InsertAtEnd("filmjlk");
	m_AlphaList.InsertAtEnd("filmstjrna");
	m_AlphaList.InsertAtEnd("filt");
	m_AlphaList.InsertAtEnd("filttavla");
	m_AlphaList.InsertAtEnd("finger");
	m_AlphaList.InsertAtEnd("fingermlning");
	m_AlphaList.InsertAtEnd("fingernagel");
	m_AlphaList.InsertAtEnd("fingrar");
	m_AlphaList.InsertAtEnd("fingrarna");
	m_AlphaList.InsertAtEnd("finna");
	m_AlphaList.InsertAtEnd("finnar");
	m_AlphaList.InsertAtEnd("finnas");
	m_AlphaList.InsertAtEnd("finne");
	m_AlphaList.InsertAtEnd("finner");
	m_AlphaList.InsertAtEnd("finns");
	m_AlphaList.InsertAtEnd("fint");
	m_AlphaList.InsertAtEnd("fira");
	m_AlphaList.InsertAtEnd("firar");
	m_AlphaList.InsertAtEnd("fisk");
	m_AlphaList.InsertAtEnd("fiska");
	m_AlphaList.InsertAtEnd("fiskaffr");
	m_AlphaList.InsertAtEnd("fiskar ");
	m_AlphaList.InsertAtEnd("fiskare");
	m_AlphaList.InsertAtEnd("fiskben");
	m_AlphaList.InsertAtEnd("fiske");
	m_AlphaList.InsertAtEnd("fiskekrok");
	m_AlphaList.InsertAtEnd("fiskesp");
	m_AlphaList.InsertAtEnd("fiskskl");
	m_AlphaList.InsertAtEnd("fixa");
	m_AlphaList.InsertAtEnd("fixar");
	m_AlphaList.InsertAtEnd("fjunig");
	m_AlphaList.InsertAtEnd("fjder");
	m_AlphaList.InsertAtEnd("fjderboll");
	m_AlphaList.InsertAtEnd("fjll");
	m_AlphaList.InsertAtEnd("fjrde");
	m_AlphaList.InsertAtEnd("fjrdedel");
	m_AlphaList.InsertAtEnd("fjril");
	m_AlphaList.InsertAtEnd("fjrran");
	m_AlphaList.InsertAtEnd("fjrrkontroll");
	m_AlphaList.InsertAtEnd("fladdermus");
	m_AlphaList.InsertAtEnd("fladdra");
	m_AlphaList.InsertAtEnd("fladdrar");
	m_AlphaList.InsertAtEnd("flagga");
	m_AlphaList.InsertAtEnd("flamingo");
	m_AlphaList.InsertAtEnd("flanellograf");
	m_AlphaList.InsertAtEnd("flaska");
	m_AlphaList.InsertAtEnd("flaskppnare");
	m_AlphaList.InsertAtEnd("fler");
	m_AlphaList.InsertAtEnd("flera");
	m_AlphaList.InsertAtEnd("flervningshus");
	m_AlphaList.InsertAtEnd("flest");
	m_AlphaList.InsertAtEnd("flesta");
	m_AlphaList.InsertAtEnd("flicka");
	m_AlphaList.InsertAtEnd("flickan");
	m_AlphaList.InsertAtEnd("flickvn");
	m_AlphaList.InsertAtEnd("flimmerhr");
	m_AlphaList.InsertAtEnd("flingor");
	m_AlphaList.InsertAtEnd("flintskallig");
	m_AlphaList.InsertAtEnd("flipperspel");
	m_AlphaList.InsertAtEnd("flod ");
	m_AlphaList.InsertAtEnd("flodhst");
	m_AlphaList.InsertAtEnd("fluga");
	m_AlphaList.InsertAtEnd("flugfiske");
	m_AlphaList.InsertAtEnd("flugsmlla");
	m_AlphaList.InsertAtEnd("flyga");
	m_AlphaList.InsertAtEnd("flygande");
	m_AlphaList.InsertAtEnd("flygbiljett");
	m_AlphaList.InsertAtEnd("flyger");
	m_AlphaList.InsertAtEnd("flygledsagare");
	m_AlphaList.InsertAtEnd("flygplan");
	m_AlphaList.InsertAtEnd("flygplanskrasch");
	m_AlphaList.InsertAtEnd("flygplats");
	m_AlphaList.InsertAtEnd("flygstart");
	m_AlphaList.InsertAtEnd("flyguppvisning");
	m_AlphaList.InsertAtEnd("flyta");
	m_AlphaList.InsertAtEnd("flytande");
	m_AlphaList.InsertAtEnd("flyter");
	m_AlphaList.InsertAtEnd("flytplatta");
	m_AlphaList.InsertAtEnd("flytta");
	m_AlphaList.InsertAtEnd("flyttar");
	m_AlphaList.InsertAtEnd("flyttbart");
	m_AlphaList.InsertAtEnd("flyttbil");
	m_AlphaList.InsertAtEnd("flck");
	m_AlphaList.InsertAtEnd("flckig");
	m_AlphaList.InsertAtEnd("flkt");
	m_AlphaList.InsertAtEnd("flmta");
	m_AlphaList.InsertAtEnd("flmtar");
	m_AlphaList.InsertAtEnd("flsk");
	m_AlphaList.InsertAtEnd("flta");
	m_AlphaList.InsertAtEnd("fltar");
	m_AlphaList.InsertAtEnd("fljt");
	m_AlphaList.InsertAtEnd("flrt");
	m_AlphaList.InsertAtEnd("flte");
	m_AlphaList.InsertAtEnd("fogsvans");
	m_AlphaList.InsertAtEnd("folie");
	m_AlphaList.InsertAtEnd("folk");
	m_AlphaList.InsertAtEnd("folkmassa");
	m_AlphaList.InsertAtEnd("fontn");
	m_AlphaList.InsertAtEnd("fordon");
	m_AlphaList.InsertAtEnd("form");
	m_AlphaList.InsertAtEnd("formel");
	m_AlphaList.InsertAtEnd("formge");
	m_AlphaList.InsertAtEnd("formger");
	m_AlphaList.InsertAtEnd("forska");
	m_AlphaList.InsertAtEnd("forskar");
	m_AlphaList.InsertAtEnd("fort");
	m_AlphaList.InsertAtEnd("fortare");
	m_AlphaList.InsertAtEnd("fortfarande");
	m_AlphaList.InsertAtEnd("fortplantning");
	m_AlphaList.InsertAtEnd("fortsatte");
	m_AlphaList.InsertAtEnd("fortstta");
	m_AlphaList.InsertAtEnd("fortstter");
	m_AlphaList.InsertAtEnd("fortstter ");
	m_AlphaList.InsertAtEnd("fossil");
	m_AlphaList.InsertAtEnd("foster");
	m_AlphaList.InsertAtEnd("fosterfamilj");
	m_AlphaList.InsertAtEnd("fosterfrlder");
	m_AlphaList.InsertAtEnd("fosterhem");
	m_AlphaList.InsertAtEnd("fostermor");
	m_AlphaList.InsertAtEnd("fostersck");
	m_AlphaList.InsertAtEnd("fostervatten");
	m_AlphaList.InsertAtEnd("fot");
	m_AlphaList.InsertAtEnd("fotbad");
	m_AlphaList.InsertAtEnd("fotboll");
	m_AlphaList.InsertAtEnd("fotbollsmatch");
	m_AlphaList.InsertAtEnd("fotbollsplan");
	m_AlphaList.InsertAtEnd("fotbollsspelare");
	m_AlphaList.InsertAtEnd("foten");
	m_AlphaList.InsertAtEnd("fotled");
	m_AlphaList.InsertAtEnd("foto");
	m_AlphaList.InsertAtEnd("fotoalbum");
	m_AlphaList.InsertAtEnd("fotogen");
	m_AlphaList.InsertAtEnd("fotograf");
	m_AlphaList.InsertAtEnd("fotografera");
	m_AlphaList.InsertAtEnd("fotograferar");
	m_AlphaList.InsertAtEnd("fotomodell");
	m_AlphaList.InsertAtEnd("fotrem");
	m_AlphaList.InsertAtEnd("fotring");
	m_AlphaList.InsertAtEnd("fotspr");
	m_AlphaList.InsertAtEnd("fotstd");
	m_AlphaList.InsertAtEnd("fram");
	m_AlphaList.InsertAtEnd("framfr");
	m_AlphaList.InsertAtEnd("framgng");
	m_AlphaList.InsertAtEnd("framhrda");
	m_AlphaList.InsertAtEnd("framhrdar");
	m_AlphaList.InsertAtEnd("framme");
	m_AlphaList.InsertAtEnd("framsida");
	m_AlphaList.InsertAtEnd("framtid");
	m_AlphaList.InsertAtEnd("framtiden");
	m_AlphaList.InsertAtEnd("framtrda");
	m_AlphaList.InsertAtEnd("framtrder");
	m_AlphaList.InsertAtEnd("framt");
	m_AlphaList.InsertAtEnd("franc");
	m_AlphaList.InsertAtEnd("frankera");
	m_AlphaList.InsertAtEnd("frankerar");
	m_AlphaList.InsertAtEnd("frans");
	m_AlphaList.InsertAtEnd("fred");
	m_AlphaList.InsertAtEnd("fredag");
	m_AlphaList.InsertAtEnd("freestyle");
	m_AlphaList.InsertAtEnd("fri");
	m_AlphaList.InsertAtEnd("frid");
	m_AlphaList.InsertAtEnd("fridfull");
	m_AlphaList.InsertAtEnd("fridlyst");
	m_AlphaList.InsertAtEnd("frihet");
	m_AlphaList.InsertAtEnd("friidrott");
	m_AlphaList.InsertAtEnd("frimrke");
	m_AlphaList.InsertAtEnd("frisbee");
	m_AlphaList.InsertAtEnd("frisersalong");
	m_AlphaList.InsertAtEnd("frisk");
	m_AlphaList.InsertAtEnd("frispark");
	m_AlphaList.InsertAtEnd("frisyr");
	m_AlphaList.InsertAtEnd("frisr");
	m_AlphaList.InsertAtEnd("fritera");
	m_AlphaList.InsertAtEnd("friterade");
	m_AlphaList.InsertAtEnd("fritid");
	m_AlphaList.InsertAtEnd("fritidshus");
	m_AlphaList.InsertAtEnd("frivillig");
	m_AlphaList.InsertAtEnd("frost");
	m_AlphaList.InsertAtEnd("fru");
	m_AlphaList.InsertAtEnd("frukost");
	m_AlphaList.InsertAtEnd("frukt");
	m_AlphaList.InsertAtEnd("fruktcocktail");
	m_AlphaList.InsertAtEnd("fruktpress");
	m_AlphaList.InsertAtEnd("fruktsam");
	m_AlphaList.InsertAtEnd("frukttrd");
	m_AlphaList.InsertAtEnd("frukttrdgrd");
	m_AlphaList.InsertAtEnd("fruktyoghurt");
	m_AlphaList.InsertAtEnd("frusen");
	m_AlphaList.InsertAtEnd("frustrerad");
	m_AlphaList.InsertAtEnd("frustrerande");
	m_AlphaList.InsertAtEnd("frys");
	m_AlphaList.InsertAtEnd("frysa");
	m_AlphaList.InsertAtEnd("fryser");
	m_AlphaList.InsertAtEnd("fryspse");
	m_AlphaList.InsertAtEnd("fryst");
	m_AlphaList.InsertAtEnd("frga");
	m_AlphaList.InsertAtEnd("frgade");
	m_AlphaList.InsertAtEnd("frgan");
	m_AlphaList.InsertAtEnd("frgar");
	m_AlphaList.InsertAtEnd("frgesport");
	m_AlphaList.InsertAtEnd("frgor");
	m_AlphaList.InsertAtEnd("frn");
	m_AlphaList.InsertAtEnd("frnvarande");
	m_AlphaList.InsertAtEnd("frck");
	m_AlphaList.InsertAtEnd("frlsning");
	m_AlphaList.InsertAtEnd("frmling");
	m_AlphaList.InsertAtEnd("frmst");
	m_AlphaList.InsertAtEnd("fr");
	m_AlphaList.InsertAtEnd("frken");
	m_AlphaList.InsertAtEnd("frn");
	m_AlphaList.InsertAtEnd("ful");
	m_AlphaList.InsertAtEnd("full");
	m_AlphaList.InsertAtEnd("fullt");
	m_AlphaList.InsertAtEnd("funktion");
	m_AlphaList.InsertAtEnd("funktionshindrade");
	m_AlphaList.InsertAtEnd("funktionsnedsttning");
	m_AlphaList.InsertAtEnd("fura");
	m_AlphaList.InsertAtEnd("fusk");
	m_AlphaList.InsertAtEnd("fuska");
	m_AlphaList.InsertAtEnd("fuskar");
	m_AlphaList.InsertAtEnd("fylla");
	m_AlphaList.InsertAtEnd("fylld");
	m_AlphaList.InsertAtEnd("fyller");
	m_AlphaList.InsertAtEnd("fyllning");
	m_AlphaList.InsertAtEnd("fyr");
	m_AlphaList.InsertAtEnd("fyra");
	m_AlphaList.InsertAtEnd("fyrkant");
	m_AlphaList.InsertAtEnd("fyrkantig");
	m_AlphaList.InsertAtEnd("fyrtio");
	m_AlphaList.InsertAtEnd("fyrverkeri");
	m_AlphaList.InsertAtEnd("fysik");
	m_AlphaList.InsertAtEnd("fysiskt");
	m_AlphaList.InsertAtEnd("f");
	m_AlphaList.InsertAtEnd("ffng");
	m_AlphaList.InsertAtEnd("fgel");
	m_AlphaList.InsertAtEnd("fgelbo");
	m_AlphaList.InsertAtEnd("fgelbur");
	m_AlphaList.InsertAtEnd("fgelfr");
	m_AlphaList.InsertAtEnd("fgelhus");
	m_AlphaList.InsertAtEnd("fgelskrmma");
	m_AlphaList.InsertAtEnd("fnga");
	m_AlphaList.InsertAtEnd("fngar");
	m_AlphaList.InsertAtEnd("fr");
	m_AlphaList.InsertAtEnd("ftt");
	m_AlphaList.InsertAtEnd("ftlj");
	m_AlphaList.InsertAtEnd("flt");
	m_AlphaList.InsertAtEnd("fngelse");
	m_AlphaList.InsertAtEnd("frdig");
	m_AlphaList.InsertAtEnd("frdigt");
	m_AlphaList.InsertAtEnd("frdtjnst");
	m_AlphaList.InsertAtEnd("frdtjnstbuss");
	m_AlphaList.InsertAtEnd("frg");
	m_AlphaList.InsertAtEnd("frga");
	m_AlphaList.InsertAtEnd("frgar");
	m_AlphaList.InsertAtEnd("frgburk");
	m_AlphaList.InsertAtEnd("frger");
	m_AlphaList.InsertAtEnd("frgkrita");
	m_AlphaList.InsertAtEnd("frgkritor");
	m_AlphaList.InsertAtEnd("frglda");
	m_AlphaList.InsertAtEnd("frglgga");
	m_AlphaList.InsertAtEnd("frglgger");
}

void CWordAid2Dlg::LoadList4()
{
	m_AlphaList.InsertAtEnd("frgpennor");
	m_AlphaList.InsertAtEnd("frja");
	m_AlphaList.InsertAtEnd("frre");
	m_AlphaList.InsertAtEnd("frsk");
	m_AlphaList.InsertAtEnd("fsta");
	m_AlphaList.InsertAtEnd("fster");
	m_AlphaList.InsertAtEnd("fsting");
	m_AlphaList.InsertAtEnd("fdelse");
	m_AlphaList.InsertAtEnd("fdelsedag");
	m_AlphaList.InsertAtEnd("fdelsekontroll");
	m_AlphaList.InsertAtEnd("fdelsen");
	m_AlphaList.InsertAtEnd("fdsel");
	m_AlphaList.InsertAtEnd("fl");
	m_AlphaList.InsertAtEnd("flja");
	m_AlphaList.InsertAtEnd("fljande");
	m_AlphaList.InsertAtEnd("fljde");
	m_AlphaList.InsertAtEnd("fljer");
	m_AlphaList.InsertAtEnd("fll");
	m_AlphaList.InsertAtEnd("fnster");
	m_AlphaList.InsertAtEnd("fnsterbrde");
	m_AlphaList.InsertAtEnd("fnstret");
	m_AlphaList.InsertAtEnd("fr");
	m_AlphaList.InsertAtEnd("frband");
	m_AlphaList.InsertAtEnd("frbandslda");
	m_AlphaList.InsertAtEnd("frbannad");
	m_AlphaList.InsertAtEnd("frbereda");
	m_AlphaList.InsertAtEnd("frberedd");
	m_AlphaList.InsertAtEnd("frbereder");
	m_AlphaList.InsertAtEnd("frbi");
	m_AlphaList.InsertAtEnd("frbinda");
	m_AlphaList.InsertAtEnd("frbinder");
	m_AlphaList.InsertAtEnd("frbjuden");
	m_AlphaList.InsertAtEnd("frbjudet");
	m_AlphaList.InsertAtEnd("frbluffa");
	m_AlphaList.InsertAtEnd("frbluffar");
	m_AlphaList.InsertAtEnd("frbundna");
	m_AlphaList.InsertAtEnd("frbttra");
	m_AlphaList.InsertAtEnd("frbttrar");
	m_AlphaList.InsertAtEnd("frdmning");
	m_AlphaList.InsertAtEnd("fre");
	m_AlphaList.InsertAtEnd("fredra");
	m_AlphaList.InsertAtEnd("fredrar");
	m_AlphaList.InsertAtEnd("frefalla");
	m_AlphaList.InsertAtEnd("frefaller");
	m_AlphaList.InsertAtEnd("fregende");
	m_AlphaList.InsertAtEnd("frekomma ");
	m_AlphaList.InsertAtEnd("frekommer");
	m_AlphaList.InsertAtEnd("frena");
	m_AlphaList.InsertAtEnd("frenar");
	m_AlphaList.InsertAtEnd("frening");
	m_AlphaList.InsertAtEnd("fresatser");
	m_AlphaList.InsertAtEnd("fresl");
	m_AlphaList.InsertAtEnd("frestllning");
	m_AlphaList.InsertAtEnd("fretag");
	m_AlphaList.InsertAtEnd("fretaget");
	m_AlphaList.InsertAtEnd("frevisa");
	m_AlphaList.InsertAtEnd("frevisar ");
	m_AlphaList.InsertAtEnd("frfader");
	m_AlphaList.InsertAtEnd("frfalla");
	m_AlphaList.InsertAtEnd("frfaller");
	m_AlphaList.InsertAtEnd("frfluten");
	m_AlphaList.InsertAtEnd("frflytta");
	m_AlphaList.InsertAtEnd("frflyttar");
	m_AlphaList.InsertAtEnd("frflyttning");
	m_AlphaList.InsertAtEnd("frhllande");
	m_AlphaList.InsertAtEnd("frhr");
	m_AlphaList.InsertAtEnd("frhra");
	m_AlphaList.InsertAtEnd("frkasta");
	m_AlphaList.InsertAtEnd("frkastar");
	m_AlphaList.InsertAtEnd("frklara");
	m_AlphaList.InsertAtEnd("frklarar");
	m_AlphaList.InsertAtEnd("frklde");
	m_AlphaList.InsertAtEnd("frkyld");
	m_AlphaList.InsertAtEnd("frlagd");
	m_AlphaList.InsertAtEnd("frlora");
	m_AlphaList.InsertAtEnd("frlorar");
	m_AlphaList.InsertAtEnd("frlorare");
	m_AlphaList.InsertAtEnd("frlossning");
	m_AlphaList.InsertAtEnd("frlovad");
	m_AlphaList.InsertAtEnd("frlt");
	m_AlphaList.InsertAtEnd("frltelse");
	m_AlphaList.InsertAtEnd("frlter");
	m_AlphaList.InsertAtEnd("frlgen");
	m_AlphaList.InsertAtEnd("frmedla ");
	m_AlphaList.InsertAtEnd("frmedlar");
	m_AlphaList.InsertAtEnd("frmiddag");
	m_AlphaList.InsertAtEnd("frmga");
	m_AlphaList.InsertAtEnd("frordning");
	m_AlphaList.InsertAtEnd("frorening");
	m_AlphaList.InsertAtEnd("frpackning");
	m_AlphaList.InsertAtEnd("frr");
	m_AlphaList.InsertAtEnd("frra");
	m_AlphaList.InsertAtEnd("frrd");
	m_AlphaList.InsertAtEnd("frrn");
	m_AlphaList.InsertAtEnd("frsiktig");
	m_AlphaList.InsertAtEnd("frsiktighet");
	m_AlphaList.InsertAtEnd("frslag");
	m_AlphaList.InsertAtEnd("frslaget");
	m_AlphaList.InsertAtEnd("frst");
	m_AlphaList.InsertAtEnd("frsta");
	m_AlphaList.InsertAtEnd("frstod");
	m_AlphaList.InsertAtEnd("frstoppning");
	m_AlphaList.InsertAtEnd("frstora");
	m_AlphaList.InsertAtEnd("frstorar");
	m_AlphaList.InsertAtEnd("frstoringsglas");
	m_AlphaList.InsertAtEnd("frst");
	m_AlphaList.InsertAtEnd("frstnd");
	m_AlphaList.InsertAtEnd("frstndshandikappad");
	m_AlphaList.InsertAtEnd("frstr");
	m_AlphaList.InsertAtEnd("frsts");
	m_AlphaList.InsertAtEnd("frstrkning");
	m_AlphaList.InsertAtEnd("frstr");
	m_AlphaList.InsertAtEnd("frstra ");
	m_AlphaList.InsertAtEnd("frsvaga");
	m_AlphaList.InsertAtEnd("frsvagar");
	m_AlphaList.InsertAtEnd("frsvar ");
	m_AlphaList.InsertAtEnd("frsvara");
	m_AlphaList.InsertAtEnd("frsvarar");
	m_AlphaList.InsertAtEnd("frsvinna");
	m_AlphaList.InsertAtEnd("frsvinner");
	m_AlphaList.InsertAtEnd("frsvunnet");
	m_AlphaList.InsertAtEnd("frsljare");
	m_AlphaList.InsertAtEnd("frsmra");
	m_AlphaList.InsertAtEnd("frsmrar");
	m_AlphaList.InsertAtEnd("frsk");
	m_AlphaList.InsertAtEnd("frska");
	m_AlphaList.InsertAtEnd("frsker");
	m_AlphaList.InsertAtEnd("frskte");
	m_AlphaList.InsertAtEnd("frtjusande");
	m_AlphaList.InsertAtEnd("frtroende");
	m_AlphaList.InsertAtEnd("frtrolig");
	m_AlphaList.InsertAtEnd("frtrolla");
	m_AlphaList.InsertAtEnd("frtrollat");
	m_AlphaList.InsertAtEnd("frut");
	m_AlphaList.InsertAtEnd("frutsp");
	m_AlphaList.InsertAtEnd("frutspr");
	m_AlphaList.InsertAtEnd("frutsga");
	m_AlphaList.InsertAtEnd("frutsger");
	m_AlphaList.InsertAtEnd("frvandla");
	m_AlphaList.InsertAtEnd("frvandlar");
	m_AlphaList.InsertAtEnd("frvaringsfack");
	m_AlphaList.InsertAtEnd("frvaringshylla");
	m_AlphaList.InsertAtEnd("frvaringslda");
	m_AlphaList.InsertAtEnd("frvaringsskp");
	m_AlphaList.InsertAtEnd("frvirrad");
	m_AlphaList.InsertAtEnd("frvna");
	m_AlphaList.InsertAtEnd("frvnad");
	m_AlphaList.InsertAtEnd("frvnar");
	m_AlphaList.InsertAtEnd("frvnta");
	m_AlphaList.InsertAtEnd("frvntan");
	m_AlphaList.InsertAtEnd("frvntar");
	m_AlphaList.InsertAtEnd("frvxla");
	m_AlphaList.InsertAtEnd("frvxlar");
	m_AlphaList.InsertAtEnd("frlder");
	m_AlphaList.InsertAtEnd("frldrar");
	m_AlphaList.InsertAtEnd("frlskad");
	m_AlphaList.InsertAtEnd("frndra");
	m_AlphaList.InsertAtEnd("ftter");
	m_AlphaList.InsertAtEnd("gaffel");
	m_AlphaList.InsertAtEnd("gaffeltruck");
	m_AlphaList.InsertAtEnd("gal");
	m_AlphaList.InsertAtEnd("gala");
	m_AlphaList.InsertAtEnd("galen");
	m_AlphaList.InsertAtEnd("galge");
	m_AlphaList.InsertAtEnd("gallblsa");
	m_AlphaList.InsertAtEnd("gallerfnster");
	m_AlphaList.InsertAtEnd("galoppera");
	m_AlphaList.InsertAtEnd("galopperar");
	m_AlphaList.InsertAtEnd("gam");
	m_AlphaList.InsertAtEnd("gamla");
	m_AlphaList.InsertAtEnd("gammal");
	m_AlphaList.InsertAtEnd("gammalt");
	m_AlphaList.InsertAtEnd("ganska");
	m_AlphaList.InsertAtEnd("gapa");
	m_AlphaList.InsertAtEnd("gapar");
	m_AlphaList.InsertAtEnd("garage");
	m_AlphaList.InsertAtEnd("garageinfart");
	m_AlphaList.InsertAtEnd("garderob");
	m_AlphaList.InsertAtEnd("gardin");
	m_AlphaList.InsertAtEnd("gardinen");
	m_AlphaList.InsertAtEnd("gardiner");
	m_AlphaList.InsertAtEnd("garn");
	m_AlphaList.InsertAtEnd("garnera");
	m_AlphaList.InsertAtEnd("garnerar");
	m_AlphaList.InsertAtEnd("gas");
	m_AlphaList.InsertAtEnd("gata");
	m_AlphaList.InsertAtEnd("gatukk");
	m_AlphaList.InsertAtEnd("gav");
	m_AlphaList.InsertAtEnd("ge");
	m_AlphaList.InsertAtEnd("gem");
	m_AlphaList.InsertAtEnd("gemenskap");
	m_AlphaList.InsertAtEnd("genant");
	m_AlphaList.InsertAtEnd("genast");
	m_AlphaList.InsertAtEnd("generalisering");
	m_AlphaList.InsertAtEnd("genom");
	m_AlphaList.InsertAtEnd("genomskinlig");
	m_AlphaList.InsertAtEnd("geometrilektion");
	m_AlphaList.InsertAtEnd("gepard");
	m_AlphaList.InsertAtEnd("ger");
	m_AlphaList.InsertAtEnd("get");
	m_AlphaList.InsertAtEnd("geting");
	m_AlphaList.InsertAtEnd("gett");
	m_AlphaList.InsertAtEnd("gevr");
	m_AlphaList.InsertAtEnd("gick");
	m_AlphaList.InsertAtEnd("gift");
	m_AlphaList.InsertAtEnd("gifta");
	m_AlphaList.InsertAtEnd("giftas");
	m_AlphaList.InsertAtEnd("gifter");
	m_AlphaList.InsertAtEnd("gipsa");
	m_AlphaList.InsertAtEnd("gipsar");
	m_AlphaList.InsertAtEnd("gipsfrband");
	m_AlphaList.InsertAtEnd("giraff");
	m_AlphaList.InsertAtEnd("girlang");
	m_AlphaList.InsertAtEnd("gissa");
	m_AlphaList.InsertAtEnd("gitarr");
	m_AlphaList.InsertAtEnd("gjorde");
	m_AlphaList.InsertAtEnd("gjort");
	m_AlphaList.InsertAtEnd("glad");
	m_AlphaList.InsertAtEnd("gladpack");
	m_AlphaList.InsertAtEnd("glas");
	m_AlphaList.InsertAtEnd("glasen");
	m_AlphaList.InsertAtEnd("glasigloo");
	m_AlphaList.InsertAtEnd("glass");
	m_AlphaList.InsertAtEnd("glassbil");
	m_AlphaList.InsertAtEnd("glasskiosk");
	m_AlphaList.InsertAtEnd("glasskjutdrrar");
	m_AlphaList.InsertAtEnd("glasvaror");
	m_AlphaList.InsertAtEnd("glasyr");
	m_AlphaList.InsertAtEnd("glastervinning");
	m_AlphaList.InsertAtEnd("glasgon");
	m_AlphaList.InsertAtEnd("glasgonen");
	m_AlphaList.InsertAtEnd("glida");
	m_AlphaList.InsertAtEnd("glider");
	m_AlphaList.InsertAtEnd("glidflygplan");
	m_AlphaList.InsertAtEnd("glitter");
	m_AlphaList.InsertAtEnd("glder");
	m_AlphaList.InsertAtEnd("gldja");
	m_AlphaList.InsertAtEnd("gldje");
	m_AlphaList.InsertAtEnd("gldlampa");
	m_AlphaList.InsertAtEnd("glm ");
	m_AlphaList.InsertAtEnd("glmma");
	m_AlphaList.InsertAtEnd("glmmer");
	m_AlphaList.InsertAtEnd("gnida");
	m_AlphaList.InsertAtEnd("gnider");
	m_AlphaList.InsertAtEnd("gngga");
	m_AlphaList.InsertAtEnd("gnggar");
	m_AlphaList.InsertAtEnd("gnlla");
	m_AlphaList.InsertAtEnd("gnller");
	m_AlphaList.InsertAtEnd("go-cart");
	m_AlphaList.InsertAtEnd("god");
	m_AlphaList.InsertAtEnd("goda");
	m_AlphaList.InsertAtEnd("godhet");
	m_AlphaList.InsertAtEnd("godis");
	m_AlphaList.InsertAtEnd("godknna");
	m_AlphaList.InsertAtEnd("godknner");
	m_AlphaList.InsertAtEnd("gods");
	m_AlphaList.InsertAtEnd("godta");
	m_AlphaList.InsertAtEnd("godtar");
	m_AlphaList.InsertAtEnd("golf");
	m_AlphaList.InsertAtEnd("golftee");
	m_AlphaList.InsertAtEnd("golv");
	m_AlphaList.InsertAtEnd("golvbelggning");
	m_AlphaList.InsertAtEnd("golvet");
	m_AlphaList.InsertAtEnd("gondol");
	m_AlphaList.InsertAtEnd("gott");
	m_AlphaList.InsertAtEnd("gottaffr");
	m_AlphaList.InsertAtEnd("gottkiosk");
	m_AlphaList.InsertAtEnd("gottpse");
	m_AlphaList.InsertAtEnd("grammofonskiva");
	m_AlphaList.InsertAtEnd("gran");
	m_AlphaList.InsertAtEnd("granne");
	m_AlphaList.InsertAtEnd("granska");
	m_AlphaList.InsertAtEnd("granskar");
	m_AlphaList.InsertAtEnd("granulat");
	m_AlphaList.InsertAtEnd("grapefrukt");
	m_AlphaList.InsertAtEnd("gratulation");
	m_AlphaList.InsertAtEnd("grav");
	m_AlphaList.InsertAtEnd("gravid");
	m_AlphaList.InsertAtEnd("graviditet");
	m_AlphaList.InsertAtEnd("gredelin");
	m_AlphaList.InsertAtEnd("gren");
	m_AlphaList.InsertAtEnd("greppa ");
	m_AlphaList.InsertAtEnd("greppar");
	m_AlphaList.InsertAtEnd("grill");
	m_AlphaList.InsertAtEnd("grilla");
	m_AlphaList.InsertAtEnd("grillar ");
	m_AlphaList.InsertAtEnd("grillen");
	m_AlphaList.InsertAtEnd("grillning");
	m_AlphaList.InsertAtEnd("grillss");
	m_AlphaList.InsertAtEnd("grimaser");
	m_AlphaList.InsertAtEnd("grind");
	m_AlphaList.InsertAtEnd("gripa");
	m_AlphaList.InsertAtEnd("griper");
	m_AlphaList.InsertAtEnd("griptng");
	m_AlphaList.InsertAtEnd("gris");
	m_AlphaList.InsertAtEnd("grisftter");
	m_AlphaList.InsertAtEnd("griskulting");
	m_AlphaList.InsertAtEnd("gro");
	m_AlphaList.InsertAtEnd("groda");
	m_AlphaList.InsertAtEnd("groddjur");
	m_AlphaList.InsertAtEnd("grodyngel");
	m_AlphaList.InsertAtEnd("grotta");
	m_AlphaList.InsertAtEnd("grottmlning");
	m_AlphaList.InsertAtEnd("grund");
	m_AlphaList.InsertAtEnd("grundskola");
	m_AlphaList.InsertAtEnd("grupp");
	m_AlphaList.InsertAtEnd("grupparbete");
	m_AlphaList.InsertAtEnd("gruppen");
	m_AlphaList.InsertAtEnd("grus");
	m_AlphaList.InsertAtEnd("gruva");
	m_AlphaList.InsertAtEnd("grymta");
	m_AlphaList.InsertAtEnd("grymtar");
	m_AlphaList.InsertAtEnd("grytan");
	m_AlphaList.InsertAtEnd("grytlapp");
	m_AlphaList.InsertAtEnd("gr");
	m_AlphaList.InsertAtEnd("grta");
	m_AlphaList.InsertAtEnd("grdde");
	m_AlphaList.InsertAtEnd("grddost");
	m_AlphaList.InsertAtEnd("grla");
	m_AlphaList.InsertAtEnd("grlar");
	m_AlphaList.InsertAtEnd("grs");
	m_AlphaList.InsertAtEnd("grshoppa");
	m_AlphaList.InsertAtEnd("grsklippare");
	m_AlphaList.InsertAtEnd("grsmatta");
	m_AlphaList.InsertAtEnd("grstrimmer");
	m_AlphaList.InsertAtEnd("grva");
	m_AlphaList.InsertAtEnd("grver");
	m_AlphaList.InsertAtEnd("grvling");
	m_AlphaList.InsertAtEnd("grvmaskin");
	m_AlphaList.InsertAtEnd("grn");
	m_AlphaList.InsertAtEnd("grna");
	m_AlphaList.InsertAtEnd("grnsak");
	m_AlphaList.InsertAtEnd("grnsaker");
	m_AlphaList.InsertAtEnd("grnsaksaffr");
	m_AlphaList.InsertAtEnd("grt");
	m_AlphaList.InsertAtEnd("gubben");
	m_AlphaList.InsertAtEnd("gud");
	m_AlphaList.InsertAtEnd("gudstjnst");
	m_AlphaList.InsertAtEnd("gul");
	m_AlphaList.InsertAtEnd("gulasch");
	m_AlphaList.InsertAtEnd("guld");
	m_AlphaList.InsertAtEnd("guldfisk");
	m_AlphaList.InsertAtEnd("gummiband");
	m_AlphaList.InsertAtEnd("gummibt");
	m_AlphaList.InsertAtEnd("gummiklubba");
	m_AlphaList.InsertAtEnd("gunga");
	m_AlphaList.InsertAtEnd("gungar");
	m_AlphaList.InsertAtEnd("gungbrda");
	m_AlphaList.InsertAtEnd("gunghst");
	m_AlphaList.InsertAtEnd("gungstol");
	m_AlphaList.InsertAtEnd("gurgla");
	m_AlphaList.InsertAtEnd("gurglar ");
	m_AlphaList.InsertAtEnd("gurka");
	m_AlphaList.InsertAtEnd("gymnastik");
	m_AlphaList.InsertAtEnd("gymnastiklektion");
	m_AlphaList.InsertAtEnd("gymnastikmatta");
	m_AlphaList.InsertAtEnd("gymnastikredskap");
	m_AlphaList.InsertAtEnd("gymnastiksal");
	m_AlphaList.InsertAtEnd("gyttja");
	m_AlphaList.InsertAtEnd("g");
	m_AlphaList.InsertAtEnd("gng");
	m_AlphaList.InsertAtEnd("gngbana");
	m_AlphaList.InsertAtEnd("gngen");
	m_AlphaList.InsertAtEnd("gnger");
	m_AlphaList.InsertAtEnd("gngfrbud");
	m_AlphaList.InsertAtEnd("gnghjlpmedel");
	m_AlphaList.InsertAtEnd("gngtvling");
	m_AlphaList.InsertAtEnd("gngvg");
	m_AlphaList.InsertAtEnd("gr");
	m_AlphaList.InsertAtEnd("grd");
	m_AlphaList.InsertAtEnd("gs");
	m_AlphaList.InsertAtEnd("gtt");
	m_AlphaList.InsertAtEnd("gva");
	m_AlphaList.InsertAtEnd("glar");
	m_AlphaList.InsertAtEnd("gllde");
	m_AlphaList.InsertAtEnd("gller");
	m_AlphaList.InsertAtEnd("grna");
	m_AlphaList.InsertAtEnd("gspa");
	m_AlphaList.InsertAtEnd("gspar");
	m_AlphaList.InsertAtEnd("gk");
	m_AlphaList.InsertAtEnd("gmma");
	m_AlphaList.InsertAtEnd("gr");
	m_AlphaList.InsertAtEnd("gra");
	m_AlphaList.InsertAtEnd("gra ");
	m_AlphaList.InsertAtEnd("ha");
	m_AlphaList.InsertAtEnd("hacka");
	m_AlphaList.InsertAtEnd("hackar");
	m_AlphaList.InsertAtEnd("hade");
	m_AlphaList.InsertAtEnd("haft");
	m_AlphaList.InsertAtEnd("hage");
	m_AlphaList.InsertAtEnd("hagel");
	m_AlphaList.InsertAtEnd("haj");
	m_AlphaList.InsertAtEnd("haka");
	m_AlphaList.InsertAtEnd("hakan");
	m_AlphaList.InsertAtEnd("haklapp");
	m_AlphaList.InsertAtEnd("hakskgg");
	m_AlphaList.InsertAtEnd("hal");
	m_AlphaList.InsertAtEnd("halka");
	m_AlphaList.InsertAtEnd("halkar");
	m_AlphaList.InsertAtEnd("hallon");
	m_AlphaList.InsertAtEnd("halloween");
	m_AlphaList.InsertAtEnd("hall");
	m_AlphaList.InsertAtEnd("hallman");
	m_AlphaList.InsertAtEnd("hals");
	m_AlphaList.InsertAtEnd("halsband");
	m_AlphaList.InsertAtEnd("halsduk");
	m_AlphaList.InsertAtEnd("halv");
	m_AlphaList.InsertAtEnd("hamburgare");
	m_AlphaList.InsertAtEnd("hammare");
	m_AlphaList.InsertAtEnd("hammock");
	m_AlphaList.InsertAtEnd("hamn");
	m_AlphaList.InsertAtEnd("hamra");
	m_AlphaList.InsertAtEnd("hamrar");
	m_AlphaList.InsertAtEnd("hamster");
	m_AlphaList.InsertAtEnd("hamsterbur");
	m_AlphaList.InsertAtEnd("han");
	m_AlphaList.InsertAtEnd("hand");
	m_AlphaList.InsertAtEnd("handband");
	m_AlphaList.InsertAtEnd("handbojor");
	m_AlphaList.InsertAtEnd("handboll");
	m_AlphaList.InsertAtEnd("handdocka");
	m_AlphaList.InsertAtEnd("handduk");
	m_AlphaList.InsertAtEnd("handen");
	m_AlphaList.InsertAtEnd("handfat");
	m_AlphaList.InsertAtEnd("handflata");
	m_AlphaList.InsertAtEnd("handgjord");
	m_AlphaList.InsertAtEnd("handikapp");
	m_AlphaList.InsertAtEnd("handikappad");
	m_AlphaList.InsertAtEnd("handkrm");
	m_AlphaList.InsertAtEnd("handla");
	m_AlphaList.InsertAtEnd("handlar");
	m_AlphaList.InsertAtEnd("handled");
	m_AlphaList.InsertAtEnd("handledsskydd");
	m_AlphaList.InsertAtEnd("handling");
	m_AlphaList.InsertAtEnd("handlov");
	m_AlphaList.InsertAtEnd("handskar");
	m_AlphaList.InsertAtEnd("handske");
	m_AlphaList.InsertAtEnd("handtag");
	m_AlphaList.InsertAtEnd("handtork");
	m_AlphaList.InsertAtEnd("handvska");
	m_AlphaList.InsertAtEnd("hankn");
	m_AlphaList.InsertAtEnd("hans");
	m_AlphaList.InsertAtEnd("hantlar");
	m_AlphaList.InsertAtEnd("hantverk");
	m_AlphaList.InsertAtEnd("hantverksbord");
	m_AlphaList.InsertAtEnd("hantverksfrsljning");
	m_AlphaList.InsertAtEnd("har");
	m_AlphaList.InsertAtEnd("hare");
	m_AlphaList.InsertAtEnd("harpa");
	m_AlphaList.InsertAtEnd("hata");
	m_AlphaList.InsertAtEnd("hatar");
	m_AlphaList.InsertAtEnd("hatt");
	m_AlphaList.InsertAtEnd("hatten");
	m_AlphaList.InsertAtEnd("hav");
	m_AlphaList.InsertAtEnd("hej");
	m_AlphaList.InsertAtEnd("heja");
	m_AlphaList.InsertAtEnd("hejaklacksledare");
	m_AlphaList.InsertAtEnd("hejar");
	m_AlphaList.InsertAtEnd("hel");
	m_AlphaList.InsertAtEnd("hela");
	m_AlphaList.InsertAtEnd("helgdag");
	m_AlphaList.InsertAtEnd("helgon");
	m_AlphaList.InsertAtEnd("helikopter");
	m_AlphaList.InsertAtEnd("heller");
	m_AlphaList.InsertAtEnd("hellre");
	m_AlphaList.InsertAtEnd("helst");
	m_AlphaList.InsertAtEnd("helt");
	m_AlphaList.InsertAtEnd("helvete");
	m_AlphaList.InsertAtEnd("hem");
	m_AlphaList.InsertAtEnd("hemkunskap");
	m_AlphaList.InsertAtEnd("hemkunskapslektion");
	m_AlphaList.InsertAtEnd("hemlig");
	m_AlphaList.InsertAtEnd("hemlighet");
	m_AlphaList.InsertAtEnd("hemlngtan");
	m_AlphaList.InsertAtEnd("hemma");
	m_AlphaList.InsertAtEnd("hemmet");
	m_AlphaList.InsertAtEnd("hemskt");
	m_AlphaList.InsertAtEnd("hemsljd");
	m_AlphaList.InsertAtEnd("henne");
	m_AlphaList.InsertAtEnd("hennes");
	m_AlphaList.InsertAtEnd("herde");
	m_AlphaList.InsertAtEnd("het");
	m_AlphaList.InsertAtEnd("heter");
	m_AlphaList.InsertAtEnd("hicka");
	m_AlphaList.InsertAtEnd("himmel");
	m_AlphaList.InsertAtEnd("hinderlopp");
	m_AlphaList.InsertAtEnd("hink");
	m_AlphaList.InsertAtEnd("hinna");
	m_AlphaList.InsertAtEnd("hiss");
	m_AlphaList.InsertAtEnd("historia");
	m_AlphaList.InsertAtEnd("historisk");
	m_AlphaList.InsertAtEnd("historiskt");
	m_AlphaList.InsertAtEnd("hit");
	m_AlphaList.InsertAtEnd("hitta");
	m_AlphaList.InsertAtEnd("hittar");
	m_AlphaList.InsertAtEnd("hittegods");
	m_AlphaList.InsertAtEnd("hittills");
	m_AlphaList.InsertAtEnd("hjord");
	m_AlphaList.InsertAtEnd("hjort");
	m_AlphaList.InsertAtEnd("hjorthorn");
	m_AlphaList.InsertAtEnd("hjul");
	m_AlphaList.InsertAtEnd("hjlm");
	m_AlphaList.InsertAtEnd("hjlp");
	m_AlphaList.InsertAtEnd("hjlpa");
	m_AlphaList.InsertAtEnd("hjlper");
	m_AlphaList.InsertAtEnd("hjlpmedel");
	m_AlphaList.InsertAtEnd("hjlpsam");
	m_AlphaList.InsertAtEnd("hjlpverksamhet");
	m_AlphaList.InsertAtEnd("hjlte");
	m_AlphaList.InsertAtEnd("hjrna");
	m_AlphaList.InsertAtEnd("hjrnskada");
	m_AlphaList.InsertAtEnd("hjrta");
	m_AlphaList.InsertAtEnd("hjrtattack");
	m_AlphaList.InsertAtEnd("hjrter");
	m_AlphaList.InsertAtEnd("hjrtfrmak");
	m_AlphaList.InsertAtEnd("hjrtlig");
	m_AlphaList.InsertAtEnd("hjrtproblem");
	m_AlphaList.InsertAtEnd("hobby");
	m_AlphaList.InsertAtEnd("hobbyarbete");
	m_AlphaList.InsertAtEnd("hobbyrum");
	m_AlphaList.InsertAtEnd("hockey");
	m_AlphaList.InsertAtEnd("hockeyklubba");
	m_AlphaList.InsertAtEnd("hockeypuck");
	m_AlphaList.InsertAtEnd("homosexualitet");
	m_AlphaList.InsertAtEnd("hon");
	m_AlphaList.InsertAtEnd("honkn");
	m_AlphaList.InsertAtEnd("honom");
	m_AlphaList.InsertAtEnd("honung");
	m_AlphaList.InsertAtEnd("hopbunden");
	m_AlphaList.InsertAtEnd("hopp");
	m_AlphaList.InsertAtEnd("hoppa");
	m_AlphaList.InsertAtEnd("hoppar");
	m_AlphaList.InsertAtEnd("hoppas");
	m_AlphaList.InsertAtEnd("hopprep");
	m_AlphaList.InsertAtEnd("hoppsan");
	m_AlphaList.InsertAtEnd("horisont");
	m_AlphaList.InsertAtEnd("horn");
	m_AlphaList.InsertAtEnd("hos");
	m_AlphaList.InsertAtEnd("hosta");
	m_AlphaList.InsertAtEnd("hostar");
	m_AlphaList.InsertAtEnd("hotell");
	m_AlphaList.InsertAtEnd("hov");
	m_AlphaList.InsertAtEnd("hovtng");
	m_AlphaList.InsertAtEnd("hud");
	m_AlphaList.InsertAtEnd("hudfrgad");
	m_AlphaList.InsertAtEnd("hudutslag");
	m_AlphaList.InsertAtEnd("hugga");
	m_AlphaList.InsertAtEnd("hugger");
	m_AlphaList.InsertAtEnd("humla");
	m_AlphaList.InsertAtEnd("humr");
	m_AlphaList.InsertAtEnd("hund");
	m_AlphaList.InsertAtEnd("hunden");
	m_AlphaList.InsertAtEnd("hundkoja");
	m_AlphaList.InsertAtEnd("hundmat");
	m_AlphaList.InsertAtEnd("hundvalp");
	m_AlphaList.InsertAtEnd("hungrig");
	m_AlphaList.InsertAtEnd("hur");
	m_AlphaList.InsertAtEnd("hus");
	m_AlphaList.InsertAtEnd("husbil");
	m_AlphaList.InsertAtEnd("husdjur");
	m_AlphaList.InsertAtEnd("husdjursterapi");
	m_AlphaList.InsertAtEnd("huset");
	m_AlphaList.InsertAtEnd("hushllsassistent");
	m_AlphaList.InsertAtEnd("hushllsekonomi");
	m_AlphaList.InsertAtEnd("hushllspapper");
	m_AlphaList.InsertAtEnd("husmor");
	m_AlphaList.InsertAtEnd("husvagn");
	m_AlphaList.InsertAtEnd("huva");
	m_AlphaList.InsertAtEnd("huvud");
	m_AlphaList.InsertAtEnd("huvudet");
	m_AlphaList.InsertAtEnd("huvudkontakt");
	m_AlphaList.InsertAtEnd("huvudstad");
	m_AlphaList.InsertAtEnd("huvudstd");
	m_AlphaList.InsertAtEnd("huvudvrk");
	m_AlphaList.InsertAtEnd("hydda");
	m_AlphaList.InsertAtEnd("hyena");
	m_AlphaList.InsertAtEnd("hylla");
	m_AlphaList.InsertAtEnd("hyllan");
	m_AlphaList.InsertAtEnd("hyllar");
	m_AlphaList.InsertAtEnd("hyr");
	m_AlphaList.InsertAtEnd("hyra");
	m_AlphaList.InsertAtEnd("hytten");
	m_AlphaList.InsertAtEnd("hyvel");
	m_AlphaList.InsertAtEnd("hl");
	m_AlphaList.InsertAtEnd("hla");
	m_AlphaList.InsertAtEnd("hlfotsinlgg");
	m_AlphaList.InsertAtEnd("hll");
	m_AlphaList.InsertAtEnd("hlla");
	m_AlphaList.InsertAtEnd("hllbarhet");
	m_AlphaList.InsertAtEnd("hller");
	m_AlphaList.InsertAtEnd("hlslag");
	m_AlphaList.InsertAtEnd("hr");
	m_AlphaList.InsertAtEnd("hrbalsam");
	m_AlphaList.InsertAtEnd("hrband");
	m_AlphaList.InsertAtEnd("hrborste");
	m_AlphaList.InsertAtEnd("hrd");
	m_AlphaList.InsertAtEnd("hret");
	m_AlphaList.InsertAtEnd("hrfrgningsmedel");
	m_AlphaList.InsertAtEnd("hrklippning");
	m_AlphaList.InsertAtEnd("hrknut");
	m_AlphaList.InsertAtEnd("hrrullar");
	m_AlphaList.InsertAtEnd("hrrulle");
	m_AlphaList.InsertAtEnd("hrspray");
	m_AlphaList.InsertAtEnd("hrspnne");
	m_AlphaList.InsertAtEnd("hrspnnen");
	m_AlphaList.InsertAtEnd("hrt");
	m_AlphaList.InsertAtEnd("hrtork");
	m_AlphaList.InsertAtEnd("hv");
	m_AlphaList.InsertAtEnd("hck");
	m_AlphaList.InsertAtEnd("hcksax");
	m_AlphaList.InsertAtEnd("hfta");
	m_AlphaList.InsertAtEnd("hftapparat");
	m_AlphaList.InsertAtEnd("hftar");
	m_AlphaList.InsertAtEnd("hftstift");
	m_AlphaList.InsertAtEnd("hger");
	m_AlphaList.InsertAtEnd("hl");
	m_AlphaList.InsertAtEnd("hlla");
	m_AlphaList.InsertAtEnd("hller");
	m_AlphaList.InsertAtEnd("hlsa");
	m_AlphaList.InsertAtEnd("hlsar");
	m_AlphaList.InsertAtEnd("hlsning");
	m_AlphaList.InsertAtEnd("hmta");
	m_AlphaList.InsertAtEnd("hmtar");
	m_AlphaList.InsertAtEnd("hnda");
	m_AlphaList.InsertAtEnd("hndelse");
	m_AlphaList.InsertAtEnd("hnder");
	m_AlphaList.InsertAtEnd("hnderna");
	m_AlphaList.InsertAtEnd("hnga");
	m_AlphaList.InsertAtEnd("hngare");
	m_AlphaList.InsertAtEnd("hnger");
	m_AlphaList.InsertAtEnd("hngls");
	m_AlphaList.InsertAtEnd("hngmatta");
	m_AlphaList.InsertAtEnd("hngrnna");
	m_AlphaList.InsertAtEnd("hngselbyxor");
	m_AlphaList.InsertAtEnd("hnsyn");
	m_AlphaList.InsertAtEnd("hnsynsfull");
	m_AlphaList.InsertAtEnd("hnt");
	m_AlphaList.InsertAtEnd("hr");
	m_AlphaList.InsertAtEnd("hrifrn");
	m_AlphaList.InsertAtEnd("hst");
	m_AlphaList.InsertAtEnd("hstkapplpning");
	m_AlphaList.InsertAtEnd("hstryttare");
	m_AlphaList.InsertAtEnd("hstsko");
	m_AlphaList.InsertAtEnd("hststall");
	m_AlphaList.InsertAtEnd("hstsvans");
	m_AlphaList.InsertAtEnd("hvstng");
	m_AlphaList.InsertAtEnd("hxa");
	m_AlphaList.InsertAtEnd("hxhatt");
	m_AlphaList.InsertAtEnd("hft");
	m_AlphaList.InsertAtEnd("hg");
	m_AlphaList.InsertAtEnd("hga");
	m_AlphaList.InsertAtEnd("hgaffel");
	m_AlphaList.InsertAtEnd("hger");
	m_AlphaList.InsertAtEnd("hgerklicka");
	m_AlphaList.InsertAtEnd("hgerklickar");
	m_AlphaList.InsertAtEnd("hgerparentes");
	m_AlphaList.InsertAtEnd("hgre");
	m_AlphaList.InsertAtEnd("hgst");
	m_AlphaList.InsertAtEnd("hgt");
	m_AlphaList.InsertAtEnd("hgtalare");
	m_AlphaList.InsertAtEnd("hgvatten");
	m_AlphaList.InsertAtEnd("hk");
	m_AlphaList.InsertAtEnd("hlass");
	m_AlphaList.InsertAtEnd("hll");
	m_AlphaList.InsertAtEnd("hna");
	m_AlphaList.InsertAtEnd("hnshus");
	m_AlphaList.InsertAtEnd("hr");
	m_AlphaList.InsertAtEnd("hra");
	m_AlphaList.InsertAtEnd("hrapparat");
	m_AlphaList.InsertAtEnd("hrde");
	m_AlphaList.InsertAtEnd("hrlurar");
	m_AlphaList.InsertAtEnd("hrn");
	m_AlphaList.InsertAtEnd("hrna");
	m_AlphaList.InsertAtEnd("hrnbord");
	m_AlphaList.InsertAtEnd("hrntand");
	m_AlphaList.InsertAtEnd("hrsel");
	m_AlphaList.InsertAtEnd("hrselprov");
	m_AlphaList.InsertAtEnd("hrselskadad");
	m_AlphaList.InsertAtEnd("hrselskydd");
	m_AlphaList.InsertAtEnd("hrsncka");
	m_AlphaList.InsertAtEnd("hrt");
	m_AlphaList.InsertAtEnd("hst");
	m_AlphaList.InsertAtEnd("hstack");
	m_AlphaList.InsertAtEnd("hvagn");
	m_AlphaList.InsertAtEnd("i");
	m_AlphaList.InsertAtEnd("ibland");
	m_AlphaList.InsertAtEnd("icke");
	m_AlphaList.InsertAtEnd("idag");
	m_AlphaList.InsertAtEnd("id");
	m_AlphaList.InsertAtEnd("identitetsband");
	m_AlphaList.InsertAtEnd("idiotisk");
	m_AlphaList.InsertAtEnd("ID-kort");
	m_AlphaList.InsertAtEnd("idrottsman");
	m_AlphaList.InsertAtEnd("idrottsplats");
	m_AlphaList.InsertAtEnd("ifall");
	m_AlphaList.InsertAtEnd("ifrn");
	m_AlphaList.InsertAtEnd("igelkott");
	m_AlphaList.InsertAtEnd("igen");
	m_AlphaList.InsertAtEnd("igenom");
	m_AlphaList.InsertAtEnd("ignorera");
	m_AlphaList.InsertAtEnd("igr");
	m_AlphaList.InsertAtEnd("ihop");
	m_AlphaList.InsertAtEnd("ihg");
	m_AlphaList.InsertAtEnd("ihlig");
	m_AlphaList.InsertAtEnd("illa");
	m_AlphaList.InsertAtEnd("illamende");
	m_AlphaList.InsertAtEnd("imorgon");
	m_AlphaList.InsertAtEnd("in");
	m_AlphaList.InsertAtEnd("inbjuda");
	m_AlphaList.InsertAtEnd("inbjudan");
	m_AlphaList.InsertAtEnd("inbjuder");
	m_AlphaList.InsertAtEnd("inbrdeskrig");
	m_AlphaList.InsertAtEnd("indian");
	m_AlphaList.InsertAtEnd("indiantlt");
	m_AlphaList.InsertAtEnd("indigo");
	m_AlphaList.InsertAtEnd("industriarbete");
	m_AlphaList.InsertAtEnd("infart");
	m_AlphaList.InsertAtEnd("information");
	m_AlphaList.InsertAtEnd("infr");
	m_AlphaList.InsertAtEnd("inga");
	m_AlphaList.InsertAtEnd("ingen");
	m_AlphaList.InsertAtEnd("ingenstans");
	m_AlphaList.InsertAtEnd("ingenting");
	m_AlphaList.InsertAtEnd("inget");
	m_AlphaList.InsertAtEnd("ingifta");
	m_AlphaList.InsertAtEnd("ingredienser");
	m_AlphaList.InsertAtEnd("ingripa");
	m_AlphaList.InsertAtEnd("ingriper");
	m_AlphaList.InsertAtEnd("ingng");
	m_AlphaList.InsertAtEnd("inhalera");
	m_AlphaList.InsertAtEnd("inhalerar");
	m_AlphaList.InsertAtEnd("injektion");
	m_AlphaList.InsertAtEnd("inkommande");
	m_AlphaList.InsertAtEnd("inkp");
	m_AlphaList.InsertAtEnd("inkpslista");
	m_AlphaList.InsertAtEnd("innan");
	m_AlphaList.InsertAtEnd("innanfr");
	m_AlphaList.InsertAtEnd("innanmtet");
	m_AlphaList.InsertAtEnd("inne");
	m_AlphaList.InsertAtEnd("innebr");
	m_AlphaList.InsertAtEnd("innehlla");
	m_AlphaList.InsertAtEnd("innehller");
	m_AlphaList.InsertAtEnd("inom");
	m_AlphaList.InsertAtEnd("inomhusbandy");
	m_AlphaList.InsertAtEnd("inre");
	m_AlphaList.InsertAtEnd("inreda");
	m_AlphaList.InsertAtEnd("inreder");
	m_AlphaList.InsertAtEnd("insatser");
	m_AlphaList.InsertAtEnd("insekt");
	m_AlphaList.InsertAtEnd("insektsbett");
	m_AlphaList.InsertAtEnd("insektsmedel");
	m_AlphaList.InsertAtEnd("insektsspray");
	m_AlphaList.InsertAtEnd("instrument");
	m_AlphaList.InsertAtEnd("instrumentbrda");
	m_AlphaList.InsertAtEnd("instllning");
	m_AlphaList.InsertAtEnd("insulin");
	m_AlphaList.InsertAtEnd("inte");
	m_AlphaList.InsertAtEnd("intellektuell");
	m_AlphaList.InsertAtEnd("internat");
	m_AlphaList.InsertAtEnd("internationell");
	m_AlphaList.InsertAtEnd("Internet");
	m_AlphaList.InsertAtEnd("intervju");
	m_AlphaList.InsertAtEnd("intervjua");
	m_AlphaList.InsertAtEnd("intervjuar");
	m_AlphaList.InsertAtEnd("intill");
	m_AlphaList.InsertAtEnd("intressant");
	m_AlphaList.InsertAtEnd("intresse");
	m_AlphaList.InsertAtEnd("inuti");
	m_AlphaList.InsertAtEnd("inviga ");
	m_AlphaList.InsertAtEnd("inviger");
	m_AlphaList.InsertAtEnd("irriterad");
	m_AlphaList.InsertAtEnd("isbana");
	m_AlphaList.InsertAtEnd("isberg");
	m_AlphaList.InsertAtEnd("isbjrn");
	m_AlphaList.InsertAtEnd("isblsa");
	m_AlphaList.InsertAtEnd("isglass");
	m_AlphaList.InsertAtEnd("ishockey");
	m_AlphaList.InsertAtEnd("ishockeyspelare");
	m_AlphaList.InsertAtEnd("isig");
	m_AlphaList.InsertAtEnd("ismaskin");
	m_AlphaList.InsertAtEnd("istapp");
	m_AlphaList.InsertAtEnd("iste");
	m_AlphaList.InsertAtEnd("istllet");
	m_AlphaList.InsertAtEnd("istrning");
	m_AlphaList.InsertAtEnd("isr");
	m_AlphaList.InsertAtEnd("iver");
	m_AlphaList.InsertAtEnd("ivrig");
	m_AlphaList.InsertAtEnd("ivg");
	m_AlphaList.InsertAtEnd("ja");
	m_AlphaList.InsertAtEnd("jacka");
	m_AlphaList.InsertAtEnd("jackan");
	m_AlphaList.InsertAtEnd("jag");
	m_AlphaList.InsertAtEnd("jaga");
	m_AlphaList.InsertAtEnd("jagar");
	m_AlphaList.InsertAtEnd("jama");
	m_AlphaList.InsertAtEnd("jamar");
	m_AlphaList.InsertAtEnd("januari");
	m_AlphaList.InsertAtEnd("jeans");
	m_AlphaList.InsertAtEnd("jetplan");
	m_AlphaList.InsertAtEnd("jo");
	m_AlphaList.InsertAtEnd("jobb");
	m_AlphaList.InsertAtEnd("jogga");
	m_AlphaList.InsertAtEnd("joggar");
	m_AlphaList.InsertAtEnd("joggingdress");
	m_AlphaList.InsertAtEnd("joggingskor");
	m_AlphaList.InsertAtEnd("jojo");
	m_AlphaList.InsertAtEnd("joker");
	m_AlphaList.InsertAtEnd("jonglera");
	m_AlphaList.InsertAtEnd("jonglerar");
	m_AlphaList.InsertAtEnd("jord");
	m_AlphaList.InsertAtEnd("jordbvning");
	m_AlphaList.InsertAtEnd("jorden");
	m_AlphaList.InsertAtEnd("jordglob");
	m_AlphaList.InsertAtEnd("jordgubbe");
	m_AlphaList.InsertAtEnd("jordntssmr");
	m_AlphaList.InsertAtEnd("jordntter");
	m_AlphaList.InsertAtEnd("jordskorpa");
	m_AlphaList.InsertAtEnd("joystick");
	m_AlphaList.InsertAtEnd("ju");
	m_AlphaList.InsertAtEnd("jubileum");
	m_AlphaList.InsertAtEnd("judisk");
	m_AlphaList.InsertAtEnd("juice");
	m_AlphaList.InsertAtEnd("jul");
	m_AlphaList.InsertAtEnd("julafton");
	m_AlphaList.InsertAtEnd("julbrd");
	m_AlphaList.InsertAtEnd("juldagen");
	m_AlphaList.InsertAtEnd("julgodis");
	m_AlphaList.InsertAtEnd("julgran");
	m_AlphaList.InsertAtEnd("julgranen");
	m_AlphaList.InsertAtEnd("julgransljus");
	m_AlphaList.InsertAtEnd("julgransprydnader");
	m_AlphaList.InsertAtEnd("julgranssaker");
	m_AlphaList.InsertAtEnd("juli");
	m_AlphaList.InsertAtEnd("julkort");
	m_AlphaList.InsertAtEnd("julstjrna");
	m_AlphaList.InsertAtEnd("julsnger");
	m_AlphaList.InsertAtEnd("jultomte");
	m_AlphaList.InsertAtEnd("juni");
	m_AlphaList.InsertAtEnd("just");
	m_AlphaList.InsertAtEnd("justera");
	m_AlphaList.InsertAtEnd("justerar");
	m_AlphaList.InsertAtEnd("justering");
	m_AlphaList.InsertAtEnd("juvel");
	m_AlphaList.InsertAtEnd("juver");
	m_AlphaList.InsertAtEnd("jgare");
	m_AlphaList.InsertAtEnd("jmfr");
	m_AlphaList.InsertAtEnd("jmfra");
	m_AlphaList.InsertAtEnd("jmn");
	m_AlphaList.InsertAtEnd("jmra");
	m_AlphaList.InsertAtEnd("jmrar");
	m_AlphaList.InsertAtEnd("jmt");
	m_AlphaList.InsertAtEnd("jrn");
	m_AlphaList.InsertAtEnd("jrnek");
	m_AlphaList.InsertAtEnd("jrnvgsbom");
	m_AlphaList.InsertAtEnd("jrnvgsspr");
	m_AlphaList.InsertAtEnd("jrnvgsstation");
	m_AlphaList.InsertAtEnd("jrnvgsvagn");
	m_AlphaList.InsertAtEnd("jrnvgsvergng");
	m_AlphaList.InsertAtEnd("jsa");
	m_AlphaList.InsertAtEnd("jser");
	m_AlphaList.InsertAtEnd("jst");
	m_AlphaList.InsertAtEnd("jtte");
	m_AlphaList.InsertAtEnd("jttebra");
	m_AlphaList.InsertAtEnd("kabel");
	m_AlphaList.InsertAtEnd("kackerlacka");
	m_AlphaList.InsertAtEnd("kaf");
	m_AlphaList.InsertAtEnd("kaffe");
	m_AlphaList.InsertAtEnd("kaffebryggare");
	m_AlphaList.InsertAtEnd("kaffebrd");
	m_AlphaList.InsertAtEnd("kaffebutik");
	m_AlphaList.InsertAtEnd("kaffebnor");
	m_AlphaList.InsertAtEnd("kaffefilter");
	m_AlphaList.InsertAtEnd("kaffegrdde");
	m_AlphaList.InsertAtEnd("kaffekanna");
	m_AlphaList.InsertAtEnd("kagge");
	m_AlphaList.InsertAtEnd("kaj");
	m_AlphaList.InsertAtEnd("kaka");
	m_AlphaList.InsertAtEnd("kakao");
	m_AlphaList.InsertAtEnd("kakbit");
	m_AlphaList.InsertAtEnd("kakbord");
	m_AlphaList.InsertAtEnd("kakburk");
	m_AlphaList.InsertAtEnd("kakform");
	m_AlphaList.InsertAtEnd("kakfrsljning");
	m_AlphaList.InsertAtEnd("kakmix");
	m_AlphaList.InsertAtEnd("kakmtt");
	m_AlphaList.InsertAtEnd("kakor");
	m_AlphaList.InsertAtEnd("kaktus");
	m_AlphaList.InsertAtEnd("kal");
	m_AlphaList.InsertAtEnd("kalender");
	m_AlphaList.InsertAtEnd("kalkon");
	m_AlphaList.InsertAtEnd("kalkylator");
	m_AlphaList.InsertAtEnd("kall");
	m_AlphaList.InsertAtEnd("kalla");
	m_AlphaList.InsertAtEnd("kallskuret");
	m_AlphaList.InsertAtEnd("kalott");
	m_AlphaList.InsertAtEnd("kalsonger");
	m_AlphaList.InsertAtEnd("kalv");
	m_AlphaList.InsertAtEnd("kalvktt");
	m_AlphaList.InsertAtEnd("kam");
	m_AlphaList.InsertAtEnd("kamel");
	
}

void CWordAid2Dlg::LoadList5()
{
	m_AlphaList.InsertAtEnd("kameleont");
	m_AlphaList.InsertAtEnd("kamera");
	m_AlphaList.InsertAtEnd("kamma");
	m_AlphaList.InsertAtEnd("kammare");
	m_AlphaList.InsertAtEnd("kamouflage");
	m_AlphaList.InsertAtEnd("kampanj");
	m_AlphaList.InsertAtEnd("kan");
	m_AlphaList.InsertAtEnd("kana");
	m_AlphaList.InsertAtEnd("kanal ");
	m_AlphaList.InsertAtEnd("kanariefgel");
	m_AlphaList.InsertAtEnd("kanel");
	m_AlphaList.InsertAtEnd("kanin");
	m_AlphaList.InsertAtEnd("kaninbur");
	m_AlphaList.InsertAtEnd("kanna");
	m_AlphaList.InsertAtEnd("kanon");
	m_AlphaList.InsertAtEnd("kanot");
	m_AlphaList.InsertAtEnd("kanotpaddling");
	m_AlphaList.InsertAtEnd("kanske");
	m_AlphaList.InsertAtEnd("kanslist");
	m_AlphaList.InsertAtEnd("kant");
	m_AlphaList.InsertAtEnd("kantklippare");
	m_AlphaList.InsertAtEnd("kapell");
	m_AlphaList.InsertAtEnd("kappa");
	m_AlphaList.InsertAtEnd("kapsyl");
	m_AlphaList.InsertAtEnd("kapten");
	m_AlphaList.InsertAtEnd("kapuschong ");
	m_AlphaList.InsertAtEnd("karamell");
	m_AlphaList.InsertAtEnd("karamellfrg");
	m_AlphaList.InsertAtEnd("karamellss");
	m_AlphaList.InsertAtEnd("karate");
	m_AlphaList.InsertAtEnd("karioke");
	m_AlphaList.InsertAtEnd("karneval");
	m_AlphaList.InsertAtEnd("karp");
	m_AlphaList.InsertAtEnd("karta");
	m_AlphaList.InsertAtEnd("kartong");
	m_AlphaList.InsertAtEnd("karusell");
	m_AlphaList.InsertAtEnd("kassaapparat");
	m_AlphaList.InsertAtEnd("kassettband");
	m_AlphaList.InsertAtEnd("kassettbandspelare");
	m_AlphaList.InsertAtEnd("kassettbok");
	m_AlphaList.InsertAtEnd("kassettminne");
	m_AlphaList.InsertAtEnd("kassrska");
	m_AlphaList.InsertAtEnd("kasta");
	m_AlphaList.InsertAtEnd("kastanj");
	m_AlphaList.InsertAtEnd("kastanjett");
	m_AlphaList.InsertAtEnd("kastar");
	m_AlphaList.InsertAtEnd("kastare");
	m_AlphaList.InsertAtEnd("kastrull");
	m_AlphaList.InsertAtEnd("katalog");
	m_AlphaList.InsertAtEnd("katedral");
	m_AlphaList.InsertAtEnd("kategori");
	m_AlphaList.InsertAtEnd("katrinplommon");
	m_AlphaList.InsertAtEnd("katt");
	m_AlphaList.InsertAtEnd("kattkorg");
	m_AlphaList.InsertAtEnd("kattlda");
	m_AlphaList.InsertAtEnd("kattmat");
	m_AlphaList.InsertAtEnd("kattunge");
	m_AlphaList.InsertAtEnd("kavaj");
	m_AlphaList.InsertAtEnd("kaveldun");
	m_AlphaList.InsertAtEnd("kaviar");
	m_AlphaList.InsertAtEnd("kavla");
	m_AlphaList.InsertAtEnd("kavlar");
	m_AlphaList.InsertAtEnd("kazoo");
	m_AlphaList.InsertAtEnd("kedja");
	m_AlphaList.InsertAtEnd("keldjur");
	m_AlphaList.InsertAtEnd("kemi");
	m_AlphaList.InsertAtEnd("kemilektion");
	m_AlphaList.InsertAtEnd("kemisk");
	m_AlphaList.InsertAtEnd("keps");
	m_AlphaList.InsertAtEnd("keramik");
	m_AlphaList.InsertAtEnd("keso");
	m_AlphaList.InsertAtEnd("ketchup");
	m_AlphaList.InsertAtEnd("kex");
	m_AlphaList.InsertAtEnd("kibbutz");
	m_AlphaList.InsertAtEnd("kid");
	m_AlphaList.InsertAtEnd("kika");
	m_AlphaList.InsertAtEnd("kikar");
	m_AlphaList.InsertAtEnd("kikare");
	m_AlphaList.InsertAtEnd("kil");
	m_AlphaList.InsertAtEnd("kille");
	m_AlphaList.InsertAtEnd("killing");
	m_AlphaList.InsertAtEnd("kinaschack");
	m_AlphaList.InsertAtEnd("kind");
	m_AlphaList.InsertAtEnd("kinesisk");
	m_AlphaList.InsertAtEnd("kiosk");
	m_AlphaList.InsertAtEnd("kirurg");
	m_AlphaList.InsertAtEnd("kiss");
	m_AlphaList.InsertAtEnd("kissa");
	m_AlphaList.InsertAtEnd("kissar");
	m_AlphaList.InsertAtEnd("kista");
	m_AlphaList.InsertAtEnd("kittel");
	m_AlphaList.InsertAtEnd("kittla");
	m_AlphaList.InsertAtEnd("kittlar");
	m_AlphaList.InsertAtEnd("kiwi ");
	m_AlphaList.InsertAtEnd("kjol");
	m_AlphaList.InsertAtEnd("klack");
	m_AlphaList.InsertAtEnd("kladdig");
	m_AlphaList.InsertAtEnd("klaga");
	m_AlphaList.InsertAtEnd("klappa");
	m_AlphaList.InsertAtEnd("klappar");
	m_AlphaList.InsertAtEnd("klar");
	m_AlphaList.InsertAtEnd("klara");
	m_AlphaList.InsertAtEnd("klarinett");
	m_AlphaList.InsertAtEnd("klart");
	m_AlphaList.InsertAtEnd("klass");
	m_AlphaList.InsertAtEnd("klassificera");
	m_AlphaList.InsertAtEnd("klassificerar");
	m_AlphaList.InsertAtEnd("klassisk ");
	m_AlphaList.InsertAtEnd("klassrum");
	m_AlphaList.InsertAtEnd("klaviatur");
	m_AlphaList.InsertAtEnd("kli");
	m_AlphaList.InsertAtEnd("klia");
	m_AlphaList.InsertAtEnd("kliar");
	m_AlphaList.InsertAtEnd("klimp");
	m_AlphaList.InsertAtEnd("klinga");
	m_AlphaList.InsertAtEnd("klingar");
	m_AlphaList.InsertAtEnd("klinik");
	m_AlphaList.InsertAtEnd("klippa");
	m_AlphaList.InsertAtEnd("klipper");
	m_AlphaList.InsertAtEnd("klippning");
	m_AlphaList.InsertAtEnd("klippor");
	m_AlphaList.InsertAtEnd("klister");
	m_AlphaList.InsertAtEnd("klistermrke");
	m_AlphaList.InsertAtEnd("klistra");
	m_AlphaList.InsertAtEnd("klistrar");
	m_AlphaList.InsertAtEnd("klo");
	m_AlphaList.InsertAtEnd("klocka");
	m_AlphaList.InsertAtEnd("klockan");
	m_AlphaList.InsertAtEnd("klockradio");
	m_AlphaList.InsertAtEnd("klockspel");
	m_AlphaList.InsertAtEnd("klohammare");
	m_AlphaList.InsertAtEnd("klok");
	m_AlphaList.InsertAtEnd("kloss");
	m_AlphaList.InsertAtEnd("klossar");
	m_AlphaList.InsertAtEnd("klosslda");
	m_AlphaList.InsertAtEnd("kloster");
	m_AlphaList.InsertAtEnd("klot");
	m_AlphaList.InsertAtEnd("klubb");
	m_AlphaList.InsertAtEnd("klubba");
	m_AlphaList.InsertAtEnd("klubbdrkt");
	m_AlphaList.InsertAtEnd("klyftig");
	m_AlphaList.InsertAtEnd("kl");
	m_AlphaList.InsertAtEnd("klcka");
	m_AlphaList.InsertAtEnd("klcker");
	m_AlphaList.InsertAtEnd("kldaffr");
	m_AlphaList.InsertAtEnd("klder");
	m_AlphaList.InsertAtEnd("kldhngare");
	m_AlphaList.InsertAtEnd("kldkrok");
	m_AlphaList.InsertAtEnd("kldlina");
	m_AlphaList.InsertAtEnd("kldnypa");
	m_AlphaList.InsertAtEnd("kldsel");
	m_AlphaList.InsertAtEnd("klm");
	m_AlphaList.InsertAtEnd("klmflaska");
	m_AlphaList.InsertAtEnd("klmma");
	m_AlphaList.InsertAtEnd("klmmer");
	m_AlphaList.InsertAtEnd("klnning");
	m_AlphaList.InsertAtEnd("klr");
	m_AlphaList.InsertAtEnd("kls");
	m_AlphaList.InsertAtEnd("kltterstativ");
	m_AlphaList.InsertAtEnd("kltterstllning");
	m_AlphaList.InsertAtEnd("klttra");
	m_AlphaList.InsertAtEnd("klttrar");
	m_AlphaList.InsertAtEnd("klv");
	m_AlphaList.InsertAtEnd("klver");
	m_AlphaList.InsertAtEnd("knacka");
	m_AlphaList.InsertAtEnd("knackar");
	m_AlphaList.InsertAtEnd("knapp");
	m_AlphaList.InsertAtEnd("knappast");
	m_AlphaList.InsertAtEnd("knapphl");
	m_AlphaList.InsertAtEnd("knappnl");
	m_AlphaList.InsertAtEnd("knappt");
	m_AlphaList.InsertAtEnd("knaprig");
	m_AlphaList.InsertAtEnd("kniv");
	m_AlphaList.InsertAtEnd("knoge");
	m_AlphaList.InsertAtEnd("knopp");
	m_AlphaList.InsertAtEnd("knuffa");
	m_AlphaList.InsertAtEnd("knuffar");
	m_AlphaList.InsertAtEnd("knut");
	m_AlphaList.InsertAtEnd("knyppla ");
	m_AlphaList.InsertAtEnd("knypplar");
	m_AlphaList.InsertAtEnd("knyta");
	m_AlphaList.InsertAtEnd("knyter");
	m_AlphaList.InsertAtEnd("knytnve");
	m_AlphaList.InsertAtEnd("knda");
	m_AlphaList.InsertAtEnd("kndar");
	m_AlphaList.InsertAtEnd("kn");
	m_AlphaList.InsertAtEnd("knbja");
	m_AlphaList.InsertAtEnd("knbjer");
	m_AlphaList.InsertAtEnd("kncka");
	m_AlphaList.InsertAtEnd("knckebrd");
	m_AlphaList.InsertAtEnd("kncker");
	m_AlphaList.InsertAtEnd("knckpple");
	m_AlphaList.InsertAtEnd("knppa ");
	m_AlphaList.InsertAtEnd("knpper");
	m_AlphaList.InsertAtEnd("knskydd");
	m_AlphaList.InsertAtEnd("knstd");
	m_AlphaList.InsertAtEnd("knl");
	m_AlphaList.InsertAtEnd("kock");
	m_AlphaList.InsertAtEnd("kofot");
	m_AlphaList.InsertAtEnd("kofngare");
	m_AlphaList.InsertAtEnd("koja");
	m_AlphaList.InsertAtEnd("koka");
	m_AlphaList.InsertAtEnd("kokar");
	m_AlphaList.InsertAtEnd("kokbok");
	m_AlphaList.InsertAtEnd("kokosnt");
	m_AlphaList.InsertAtEnd("kokpunkt");
	m_AlphaList.InsertAtEnd("kokt");
	m_AlphaList.InsertAtEnd("kol");
	m_AlphaList.InsertAtEnd("kolhydrat");
	m_AlphaList.InsertAtEnd("kolibri");
	m_AlphaList.InsertAtEnd("kollega");
	m_AlphaList.InsertAtEnd("kollidera");
	m_AlphaList.InsertAtEnd("kollision");
	m_AlphaList.InsertAtEnd("koltrast");
	m_AlphaList.InsertAtEnd("kom");
	m_AlphaList.InsertAtEnd("kombination");
	m_AlphaList.InsertAtEnd("kombinera");
	m_AlphaList.InsertAtEnd("kombinerar ");
	m_AlphaList.InsertAtEnd("komet");
	m_AlphaList.InsertAtEnd("komma");
	m_AlphaList.InsertAtEnd("kommando");
	m_AlphaList.InsertAtEnd("kommer");
	m_AlphaList.InsertAtEnd("kommit");
	m_AlphaList.InsertAtEnd("kommun");
	m_AlphaList.InsertAtEnd("kommunen");
	m_AlphaList.InsertAtEnd("kommunicera");
	m_AlphaList.InsertAtEnd("kommunicerar ");
	m_AlphaList.InsertAtEnd("kommunikation");
	m_AlphaList.InsertAtEnd("kommunikationshjlpmedel");
	m_AlphaList.InsertAtEnd("kommunikationsprm");
	m_AlphaList.InsertAtEnd("kommunikator");
	m_AlphaList.InsertAtEnd("kompass");
	m_AlphaList.InsertAtEnd("kompis");
	m_AlphaList.InsertAtEnd("kompositr");
	m_AlphaList.InsertAtEnd("kompost");
	m_AlphaList.InsertAtEnd("koncentrera");
	m_AlphaList.InsertAtEnd("koncentrerad");
	m_AlphaList.InsertAtEnd("koncentrerar ");
	m_AlphaList.InsertAtEnd("konduktr");
	m_AlphaList.InsertAtEnd("konferensrum");
	m_AlphaList.InsertAtEnd("konfetti");
	m_AlphaList.InsertAtEnd("konfirmation");
	m_AlphaList.InsertAtEnd("konsert");
	m_AlphaList.InsertAtEnd("konserthus");
	m_AlphaList.InsertAtEnd("konserv");
	m_AlphaList.InsertAtEnd("konservppnare");
	m_AlphaList.InsertAtEnd("konst");
	m_AlphaList.InsertAtEnd("konstgjord");
	m_AlphaList.InsertAtEnd("konstgdsel");
	m_AlphaList.InsertAtEnd("konstig");
	m_AlphaList.InsertAtEnd("konstmuseum");
	m_AlphaList.InsertAtEnd("konstnr");
	m_AlphaList.InsertAtEnd("konstverk");
	m_AlphaList.InsertAtEnd("kontakt");
	m_AlphaList.InsertAtEnd("kontaktlinser");
	m_AlphaList.InsertAtEnd("kontanter");
	m_AlphaList.InsertAtEnd("kontinent");
	m_AlphaList.InsertAtEnd("konto");
	m_AlphaList.InsertAtEnd("kontor");
	m_AlphaList.InsertAtEnd("kontorsarbete");
	m_AlphaList.InsertAtEnd("kontorstillbehr");
	m_AlphaList.InsertAtEnd("kontrollera");
	m_AlphaList.InsertAtEnd("kontrollerar");
	m_AlphaList.InsertAtEnd("kopia");
	m_AlphaList.InsertAtEnd("kopiera");
	m_AlphaList.InsertAtEnd("kopierar");
	m_AlphaList.InsertAtEnd("kopieringsapparat");
	m_AlphaList.InsertAtEnd("kopieringsmaskin");
	m_AlphaList.InsertAtEnd("kopp");
	m_AlphaList.InsertAtEnd("koppel");
	m_AlphaList.InsertAtEnd("koppla ");
	m_AlphaList.InsertAtEnd("kopplar");
	m_AlphaList.InsertAtEnd("korg");
	m_AlphaList.InsertAtEnd("korgboll");
	m_AlphaList.InsertAtEnd("korgbollsmatch");
	m_AlphaList.InsertAtEnd("korintbrd");
	m_AlphaList.InsertAtEnd("korintbulle");
	m_AlphaList.InsertAtEnd("kork");
	m_AlphaList.InsertAtEnd("korken");
	m_AlphaList.InsertAtEnd("korkskruv");
	m_AlphaList.InsertAtEnd("korn");
	m_AlphaList.InsertAtEnd("korrekt");
	m_AlphaList.InsertAtEnd("kors");
	m_AlphaList.InsertAtEnd("korsa");
	m_AlphaList.InsertAtEnd("korsar");
	m_AlphaList.InsertAtEnd("kort");
	m_AlphaList.InsertAtEnd("kortalbum");
	m_AlphaList.InsertAtEnd("kortbyxor");
	m_AlphaList.InsertAtEnd("korv");
	m_AlphaList.InsertAtEnd("korvstnd");
	m_AlphaList.InsertAtEnd("kosta");
	m_AlphaList.InsertAtEnd("kostar");
	m_AlphaList.InsertAtEnd("kostym");
	m_AlphaList.InsertAtEnd("krabba");
	m_AlphaList.InsertAtEnd("kraft");
	m_AlphaList.InsertAtEnd("kraftuttryck");
	m_AlphaList.InsertAtEnd("krage");
	m_AlphaList.InsertAtEnd("krama");
	m_AlphaList.InsertAtEnd("kramar");
	m_AlphaList.InsertAtEnd("kramas");
	m_AlphaList.InsertAtEnd("kramp");
	m_AlphaList.InsertAtEnd("kran");
	m_AlphaList.InsertAtEnd("kranen");
	m_AlphaList.InsertAtEnd("krans");
	m_AlphaList.InsertAtEnd("krasch");
	m_AlphaList.InsertAtEnd("krascha");
	m_AlphaList.InsertAtEnd("kraschar");
	m_AlphaList.InsertAtEnd("kratta");
	m_AlphaList.InsertAtEnd("krattar");
	m_AlphaList.InsertAtEnd("krav");
	m_AlphaList.InsertAtEnd("kraxa");
	m_AlphaList.InsertAtEnd("kraxar");
	m_AlphaList.InsertAtEnd("kreativ");
	m_AlphaList.InsertAtEnd("kreditkort");
	m_AlphaList.InsertAtEnd("kremera");
	m_AlphaList.InsertAtEnd("kremerar");
	m_AlphaList.InsertAtEnd("krig");
	m_AlphaList.InsertAtEnd("kring");
	m_AlphaList.InsertAtEnd("kringla");
	m_AlphaList.InsertAtEnd("kristall");
	m_AlphaList.InsertAtEnd("kristallolja");
	m_AlphaList.InsertAtEnd("kristen");
	m_AlphaList.InsertAtEnd("kristet");
	m_AlphaList.InsertAtEnd("krita");
	m_AlphaList.InsertAtEnd("kritstreck");
	m_AlphaList.InsertAtEnd("krock");
	m_AlphaList.InsertAtEnd("krocket");
	m_AlphaList.InsertAtEnd("krok");
	m_AlphaList.InsertAtEnd("krokodil");
	m_AlphaList.InsertAtEnd("krokus");
	m_AlphaList.InsertAtEnd("krona");
	m_AlphaList.InsertAtEnd("kronblad");
	m_AlphaList.InsertAtEnd("kronor");
	m_AlphaList.InsertAtEnd("kropp");
	m_AlphaList.InsertAtEnd("kroppen");
	m_AlphaList.InsertAtEnd("kroppsdel");
	m_AlphaList.InsertAtEnd("kroppsvrdsartiklar");
	m_AlphaList.InsertAtEnd("kroppsvrmare");
	m_AlphaList.InsertAtEnd("kroppsvningar");
	m_AlphaList.InsertAtEnd("krossa");
	m_AlphaList.InsertAtEnd("krossar");
	m_AlphaList.InsertAtEnd("krubba");
	m_AlphaList.InsertAtEnd("kruka");
	m_AlphaList.InsertAtEnd("krukvxt");
	m_AlphaList.InsertAtEnd("krusbr");
	m_AlphaList.InsertAtEnd("kry");
	m_AlphaList.InsertAtEnd("krya");
	m_AlphaList.InsertAtEnd("kryar ");
	m_AlphaList.InsertAtEnd("kryckor");
	m_AlphaList.InsertAtEnd("krydda");
	m_AlphaList.InsertAtEnd("kryddad");
	m_AlphaList.InsertAtEnd("kryddburk");
	m_AlphaList.InsertAtEnd("kryddstark");
	m_AlphaList.InsertAtEnd("krypa");
	m_AlphaList.InsertAtEnd("kryper");
	m_AlphaList.InsertAtEnd("krka");
	m_AlphaList.InsertAtEnd("krfta");
	m_AlphaList.InsertAtEnd("krkas");
	m_AlphaList.InsertAtEnd("krldjur");
	m_AlphaList.InsertAtEnd("krva");
	m_AlphaList.InsertAtEnd("krver");
	m_AlphaList.InsertAtEnd("krk");
	m_AlphaList.InsertAtEnd("kudde");
	m_AlphaList.InsertAtEnd("kugghjul");
	m_AlphaList.InsertAtEnd("kul");
	m_AlphaList.InsertAtEnd("kula");
	m_AlphaList.InsertAtEnd("kulhammare");
	m_AlphaList.InsertAtEnd("kulle");
	m_AlphaList.InsertAtEnd("kullerbytta");
	m_AlphaList.InsertAtEnd("kulram");
	m_AlphaList.InsertAtEnd("kund");
	m_AlphaList.InsertAtEnd("kunde");
	m_AlphaList.InsertAtEnd("kung");
	m_AlphaList.InsertAtEnd("kunglig");
	m_AlphaList.InsertAtEnd("kungr");
	m_AlphaList.InsertAtEnd("kungra");
	m_AlphaList.InsertAtEnd("kunna");
	m_AlphaList.InsertAtEnd("kunnat");
	m_AlphaList.InsertAtEnd("kunskap");
	m_AlphaList.InsertAtEnd("kupvrmare");
	m_AlphaList.InsertAtEnd("kupong");
	m_AlphaList.InsertAtEnd("kurator");
	m_AlphaList.InsertAtEnd("kurva");
	m_AlphaList.InsertAtEnd("kusin");
	m_AlphaList.InsertAtEnd("kuvert");
	m_AlphaList.InsertAtEnd("kvadrat");
	m_AlphaList.InsertAtEnd("kvalitet");
	m_AlphaList.InsertAtEnd("kvalster");
	m_AlphaList.InsertAtEnd("kvar");
	m_AlphaList.InsertAtEnd("kvarn");
	m_AlphaList.InsertAtEnd("kvast");
	m_AlphaList.InsertAtEnd("kvinna");
	m_AlphaList.InsertAtEnd("kvinnlig");
	m_AlphaList.InsertAtEnd("kvinnor");
	m_AlphaList.InsertAtEnd("kvitto");
	m_AlphaList.InsertAtEnd("kvittra");
	m_AlphaList.InsertAtEnd("kvittrar");
	m_AlphaList.InsertAtEnd("kvll");
	m_AlphaList.InsertAtEnd("kvllen");
	m_AlphaList.InsertAtEnd("kvllsmat");
	m_AlphaList.InsertAtEnd("kyckling");
	m_AlphaList.InsertAtEnd("kycklingrtt");
	m_AlphaList.InsertAtEnd("kyl");
	m_AlphaList.InsertAtEnd("kylig");
	m_AlphaList.InsertAtEnd("kylskp");
	m_AlphaList.InsertAtEnd("kypare");
	m_AlphaList.InsertAtEnd("kyrka");
	m_AlphaList.InsertAtEnd("kyrkbnk");
	m_AlphaList.InsertAtEnd("kyrkogrd");
	m_AlphaList.InsertAtEnd("kyskhet");
	m_AlphaList.InsertAtEnd("kyss");
	m_AlphaList.InsertAtEnd("kyssa");
	m_AlphaList.InsertAtEnd("kyssas");
	m_AlphaList.InsertAtEnd("kysser");
	m_AlphaList.InsertAtEnd("kl");
	m_AlphaList.InsertAtEnd("kldolm");
	m_AlphaList.InsertAtEnd("kldolmar");
	m_AlphaList.InsertAtEnd("klrot");
	m_AlphaList.InsertAtEnd("ksa");
	m_AlphaList.InsertAtEnd("kgla");
	m_AlphaList.InsertAtEnd("kglor");
	m_AlphaList.InsertAtEnd("kke");
	m_AlphaList.InsertAtEnd("klke");
	m_AlphaList.InsertAtEnd("kllare");
	m_AlphaList.InsertAtEnd("knde");
	m_AlphaList.InsertAtEnd("knguru");
	m_AlphaList.InsertAtEnd("knna");
	m_AlphaList.InsertAtEnd("knner");
	m_AlphaList.InsertAtEnd("knsla");
	m_AlphaList.InsertAtEnd("kpp");
	m_AlphaList.InsertAtEnd("kr");
	m_AlphaList.InsertAtEnd("kra");
	m_AlphaList.InsertAtEnd("krlek");
	m_AlphaList.InsertAtEnd("krna");
	m_AlphaList.InsertAtEnd("krnhus");
	m_AlphaList.InsertAtEnd("krnkraft");
	m_AlphaList.InsertAtEnd("krnmjlk");
	m_AlphaList.InsertAtEnd("krra");
	m_AlphaList.InsertAtEnd("kk");
	m_AlphaList.InsertAtEnd("kket");
	m_AlphaList.InsertAtEnd("kkshandduk");
	m_AlphaList.InsertAtEnd("kksredskap");
	m_AlphaList.InsertAtEnd("kksskp");
	m_AlphaList.InsertAtEnd("kkstrdgrd");
	m_AlphaList.InsertAtEnd("klapp");
	m_AlphaList.InsertAtEnd("kn");
	m_AlphaList.InsertAtEnd("kpa");
	m_AlphaList.InsertAtEnd("kr");
	m_AlphaList.InsertAtEnd("kra");
	m_AlphaList.InsertAtEnd("krkort");
	m_AlphaList.InsertAtEnd("krriktning");
	m_AlphaList.InsertAtEnd("krsbr");
	m_AlphaList.InsertAtEnd("krsbrstrd");
	m_AlphaList.InsertAtEnd("ktt");
	m_AlphaList.InsertAtEnd("kttbulle");
	m_AlphaList.InsertAtEnd("kttfrs");
	m_AlphaList.InsertAtEnd("kttfrsrulle");
	m_AlphaList.InsertAtEnd("kttkorv");
	m_AlphaList.InsertAtEnd("laboratorium");
	m_AlphaList.InsertAtEnd("lada");
	m_AlphaList.InsertAtEnd("ladda");
	m_AlphaList.InsertAtEnd("laddar");
	m_AlphaList.InsertAtEnd("lade");
	m_AlphaList.InsertAtEnd("ladugrd");
	m_AlphaList.InsertAtEnd("lag");
	m_AlphaList.InsertAtEnd("laga");
	m_AlphaList.InsertAtEnd("lagad");
	m_AlphaList.InsertAtEnd("lagar");
	m_AlphaList.InsertAtEnd("lagen");
	m_AlphaList.InsertAtEnd("lagerblad");
	m_AlphaList.InsertAtEnd("lagledare");
	m_AlphaList.InsertAtEnd("lagra");
	m_AlphaList.InsertAtEnd("lagrar");
	m_AlphaList.InsertAtEnd("lagt");
	m_AlphaList.InsertAtEnd("lakan");
	m_AlphaList.InsertAtEnd("lamm");
	m_AlphaList.InsertAtEnd("lammktt");
	m_AlphaList.InsertAtEnd("lampa");
	m_AlphaList.InsertAtEnd("lampskrm");
	m_AlphaList.InsertAtEnd("land");
	m_AlphaList.InsertAtEnd("landet");
	m_AlphaList.InsertAtEnd("landhockey");
	m_AlphaList.InsertAtEnd("landning");
	m_AlphaList.InsertAtEnd("landsbygd");
	m_AlphaList.InsertAtEnd("landskap");
	m_AlphaList.InsertAtEnd("landsvg");
	m_AlphaList.InsertAtEnd("lapp");
	m_AlphaList.InsertAtEnd("laptop");
	m_AlphaList.InsertAtEnd("larmknapp");
	m_AlphaList.InsertAtEnd("larv");
	m_AlphaList.InsertAtEnd("lasagne");
	m_AlphaList.InsertAtEnd("last");
	m_AlphaList.InsertAtEnd("lastbil");
	m_AlphaList.InsertAtEnd("lat");
	m_AlphaList.InsertAtEnd("latitud");
	m_AlphaList.InsertAtEnd("lavalampa");
	m_AlphaList.InsertAtEnd("led");
	m_AlphaList.InsertAtEnd("leda");
	m_AlphaList.InsertAtEnd("ledare");
	m_AlphaList.InsertAtEnd("leder");
	m_AlphaList.InsertAtEnd("ledighet");
	m_AlphaList.InsertAtEnd("ledsaga");
	m_AlphaList.InsertAtEnd("ledsagar");
	m_AlphaList.InsertAtEnd("ledsen");
	m_AlphaList.InsertAtEnd("ledtrd");
	m_AlphaList.InsertAtEnd("lego");
	m_AlphaList.InsertAtEnd("lejon");
	m_AlphaList.InsertAtEnd("lejongap");
	m_AlphaList.InsertAtEnd("lek");
	m_AlphaList.InsertAtEnd("leka");
	m_AlphaList.InsertAtEnd("leka ");
	m_AlphaList.InsertAtEnd("leker");
	m_AlphaList.InsertAtEnd("lekhage");
	m_AlphaList.InsertAtEnd("lekhrna");
	m_AlphaList.InsertAtEnd("lekkk");
	m_AlphaList.InsertAtEnd("lekotek");
	m_AlphaList.InsertAtEnd("lekplats");
	m_AlphaList.InsertAtEnd("lekrum");
	m_AlphaList.InsertAtEnd("leksak");
	m_AlphaList.InsertAtEnd("leksaker");
	m_AlphaList.InsertAtEnd("leksaksaffr");
	m_AlphaList.InsertAtEnd("leksaksbil");
	m_AlphaList.InsertAtEnd("leksaksdjur");
	m_AlphaList.InsertAtEnd("leksaksgarage");
	m_AlphaList.InsertAtEnd("leksakslda");
	m_AlphaList.InsertAtEnd("leksakssnurra");
	m_AlphaList.InsertAtEnd("leksakstg");
	m_AlphaList.InsertAtEnd("lekstuga");
	m_AlphaList.InsertAtEnd("lektion");
	m_AlphaList.InsertAtEnd("leopard");
	m_AlphaList.InsertAtEnd("ler");
	m_AlphaList.InsertAtEnd("lera");
	m_AlphaList.InsertAtEnd("lerig");
	m_AlphaList.InsertAtEnd("leta ");
	m_AlphaList.InsertAtEnd("letar");
	m_AlphaList.InsertAtEnd("leva");
	m_AlphaList.InsertAtEnd("levande");
	m_AlphaList.InsertAtEnd("lever");
	m_AlphaList.InsertAtEnd("leverantr");
	m_AlphaList.InsertAtEnd("leverera");
	m_AlphaList.InsertAtEnd("levererar");
	m_AlphaList.InsertAtEnd("levereras");
	m_AlphaList.InsertAtEnd("lexikon");
	m_AlphaList.InsertAtEnd("lie");
	m_AlphaList.InsertAtEnd("ligga");
	m_AlphaList.InsertAtEnd("ligger");
	m_AlphaList.InsertAtEnd("lika");
	m_AlphaList.InsertAtEnd("likgiltig");
	m_AlphaList.InsertAtEnd("likkista");
	m_AlphaList.InsertAtEnd("likna");
	m_AlphaList.InsertAtEnd("liknar ");
	m_AlphaList.InsertAtEnd("liksom");
	m_AlphaList.InsertAtEnd("likvagn");
	m_AlphaList.InsertAtEnd("lila");
	m_AlphaList.InsertAtEnd("lilla");
	m_AlphaList.InsertAtEnd("lillfinger");
	m_AlphaList.InsertAtEnd("lim");
	m_AlphaList.InsertAtEnd("lime");
	m_AlphaList.InsertAtEnd("limma");
	m_AlphaList.InsertAtEnd("limmar");
	m_AlphaList.InsertAtEnd("limpistol");
	m_AlphaList.InsertAtEnd("linbana");
	m_AlphaList.InsertAtEnd("lingon");
	m_AlphaList.InsertAtEnd("lingonkrans");
	m_AlphaList.InsertAtEnd("linjal");
	m_AlphaList.InsertAtEnd("linje");
	m_AlphaList.InsertAtEnd("linjedans");
	m_AlphaList.InsertAtEnd("linjeman");
	m_AlphaList.InsertAtEnd("linser");
	m_AlphaList.InsertAtEnd("lista");
	m_AlphaList.InsertAtEnd("lita");
	m_AlphaList.InsertAtEnd("litar");
	m_AlphaList.InsertAtEnd("lite");
	m_AlphaList.InsertAtEnd("liten");
	m_AlphaList.InsertAtEnd("litermtt");
	m_AlphaList.InsertAtEnd("litet");
	m_AlphaList.InsertAtEnd("liv");
	m_AlphaList.InsertAtEnd("livet");
	m_AlphaList.InsertAtEnd("livsmedelsaffr");
	m_AlphaList.InsertAtEnd("livvakt");
	m_AlphaList.InsertAtEnd("livvst");
	m_AlphaList.InsertAtEnd("ljud");
	m_AlphaList.InsertAtEnd("ljudlig");
	m_AlphaList.InsertAtEnd("ljudls");
	m_AlphaList.InsertAtEnd("ljuga");
	m_AlphaList.InsertAtEnd("ljuger");
	m_AlphaList.InsertAtEnd("ljummen");
	m_AlphaList.InsertAtEnd("ljus");
	m_AlphaList.InsertAtEnd("ljusast");
	m_AlphaList.InsertAtEnd("ljusfilter");
	m_AlphaList.InsertAtEnd("ljusorgel");
	m_AlphaList.InsertAtEnd("ljusstakar");
	m_AlphaList.InsertAtEnd("ljusstake");
	m_AlphaList.InsertAtEnd("lock");
	m_AlphaList.InsertAtEnd("locka");
	m_AlphaList.InsertAtEnd("lockar");
	m_AlphaList.InsertAtEnd("locket");
	m_AlphaList.InsertAtEnd("lockig");
	m_AlphaList.InsertAtEnd("lod");
	m_AlphaList.InsertAtEnd("log");
	m_AlphaList.InsertAtEnd("logement");
	m_AlphaList.InsertAtEnd("logi");
	m_AlphaList.InsertAtEnd("logoped");
	m_AlphaList.InsertAtEnd("logopedi");
	m_AlphaList.InsertAtEnd("lok");
	m_AlphaList.InsertAtEnd("lokal");
	m_AlphaList.InsertAtEnd("lokalvrdare");
	m_AlphaList.InsertAtEnd("longitud");
	m_AlphaList.InsertAtEnd("lopp");
	m_AlphaList.InsertAtEnd("loppa");
	m_AlphaList.InsertAtEnd("lossa");
	m_AlphaList.InsertAtEnd("lossar");
	m_AlphaList.InsertAtEnd("lotto ");
	m_AlphaList.InsertAtEnd("lova");
	m_AlphaList.InsertAtEnd("lovar ");
	m_AlphaList.InsertAtEnd("Lucia ");
	m_AlphaList.InsertAtEnd("lucka");
	m_AlphaList.InsertAtEnd("luddig");
	m_AlphaList.InsertAtEnd("luft");
	m_AlphaList.InsertAtEnd("luften");
	m_AlphaList.InsertAtEnd("luftkonditionering");
	m_AlphaList.InsertAtEnd("luftmadrass");
	m_AlphaList.InsertAtEnd("luftrenare");
	m_AlphaList.InsertAtEnd("luftskepp");
	m_AlphaList.InsertAtEnd("luftvgar");
	m_AlphaList.InsertAtEnd("lugn");
	m_AlphaList.InsertAtEnd("lugna ");
	m_AlphaList.InsertAtEnd("lugnar");
	m_AlphaList.InsertAtEnd("lukt");
	m_AlphaList.InsertAtEnd("lukta ");
	m_AlphaList.InsertAtEnd("luktar");
	m_AlphaList.InsertAtEnd("luktterapi");
	m_AlphaList.InsertAtEnd("lunch");
	m_AlphaList.InsertAtEnd("lunchdags");
	m_AlphaList.InsertAtEnd("lunchmusik");
	m_AlphaList.InsertAtEnd("lunchpaket");
	m_AlphaList.InsertAtEnd("lunchrum");
	m_AlphaList.InsertAtEnd("lungor");
	m_AlphaList.InsertAtEnd("lutande");
	m_AlphaList.InsertAtEnd("lyckas");
	m_AlphaList.InsertAtEnd("lycklig");
	m_AlphaList.InsertAtEnd("lyckohjul");
	m_AlphaList.InsertAtEnd("lycknska");
	m_AlphaList.InsertAtEnd("lycknskar");
	m_AlphaList.InsertAtEnd("lyda");
	m_AlphaList.InsertAtEnd("lyder");
	m_AlphaList.InsertAtEnd("lyft");
	m_AlphaList.InsertAtEnd("lyfta");
	m_AlphaList.InsertAtEnd("lyfter");
	m_AlphaList.InsertAtEnd("lyftkran");
	m_AlphaList.InsertAtEnd("lysa ");
	m_AlphaList.InsertAtEnd("lyser");
	m_AlphaList.InsertAtEnd("lysrrsarmatur");
	m_AlphaList.InsertAtEnd("lyssna");
	m_AlphaList.InsertAtEnd("lyssnar");
	m_AlphaList.InsertAtEnd("lda");
	m_AlphaList.InsertAtEnd("ldan");
	m_AlphaList.InsertAtEnd("lg");
	m_AlphaList.InsertAtEnd("lga");
	m_AlphaList.InsertAtEnd("lna");
	m_AlphaList.InsertAtEnd("lnar");
	m_AlphaList.InsertAtEnd("lng");
	m_AlphaList.InsertAtEnd("lnga");
	m_AlphaList.InsertAtEnd("lngboll");
	m_AlphaList.InsertAtEnd("lngfinger");
	m_AlphaList.InsertAtEnd("lngfredag");
	m_AlphaList.InsertAtEnd("lngsam");
	m_AlphaList.InsertAtEnd("lngsammare");
	m_AlphaList.InsertAtEnd("lngsamt");
	m_AlphaList.InsertAtEnd("lngt");
	m_AlphaList.InsertAtEnd("ls");
	m_AlphaList.InsertAtEnd("lsa");
	m_AlphaList.InsertAtEnd("lser");
	m_AlphaList.InsertAtEnd("lst");
	m_AlphaList.InsertAtEnd("lt");
	m_AlphaList.InsertAtEnd("lta");
	m_AlphaList.InsertAtEnd("lter");
	m_AlphaList.InsertAtEnd("lcka");
	m_AlphaList.InsertAtEnd("lcker");
	m_AlphaList.InsertAtEnd("lder");
	m_AlphaList.InsertAtEnd("lge");
	m_AlphaList.InsertAtEnd("lgenhet");
	m_AlphaList.InsertAtEnd("lger");
	m_AlphaList.InsertAtEnd("lgga");
	m_AlphaList.InsertAtEnd("lgger");
	m_AlphaList.InsertAtEnd("lkare");
	m_AlphaList.InsertAtEnd("lmna");
	m_AlphaList.InsertAtEnd("lmnade");
	m_AlphaList.InsertAtEnd("lmnar");
	m_AlphaList.InsertAtEnd("ln");
	m_AlphaList.InsertAtEnd("lnder");
	m_AlphaList.InsertAtEnd("lngd");
	m_AlphaList.InsertAtEnd("lngdmtt");
	m_AlphaList.InsertAtEnd("lnge");
	m_AlphaList.InsertAtEnd("lngre");
	m_AlphaList.InsertAtEnd("lngs");
	m_AlphaList.InsertAtEnd("lngst");
	m_AlphaList.InsertAtEnd("lngta");
	m_AlphaList.InsertAtEnd("lppar");
	m_AlphaList.InsertAtEnd("lppstift");
	m_AlphaList.InsertAtEnd("lra");
	m_AlphaList.InsertAtEnd("lrare");
	m_AlphaList.InsertAtEnd("lsa");
	m_AlphaList.InsertAtEnd("lser");
	m_AlphaList.InsertAtEnd("lsk");
	m_AlphaList.InsertAtEnd("lskedryck");
	m_AlphaList.InsertAtEnd("lt");
	m_AlphaList.InsertAtEnd("ltt");
	m_AlphaList.InsertAtEnd("lxa");
	m_AlphaList.InsertAtEnd("ldpistol");
	m_AlphaList.InsertAtEnd("lgn");
	m_AlphaList.InsertAtEnd("lk");
	m_AlphaList.InsertAtEnd("lkringar");
	m_AlphaList.InsertAtEnd("lkvxt");
	m_AlphaList.InsertAtEnd("ln");
	m_AlphaList.InsertAtEnd("lnnlv");
	m_AlphaList.InsertAtEnd("lrdag");
	m_AlphaList.InsertAtEnd("ls");
	m_AlphaList.InsertAtEnd("lsa");
	m_AlphaList.InsertAtEnd("lshr");
	m_AlphaList.InsertAtEnd("lsning");
	m_AlphaList.InsertAtEnd("lstnder");
	m_AlphaList.InsertAtEnd("lv");
	m_AlphaList.InsertAtEnd("lvhydda");
	m_AlphaList.InsertAtEnd("lvhg");
	m_AlphaList.InsertAtEnd("lvsg");
	m_AlphaList.InsertAtEnd("madrass");
	m_AlphaList.InsertAtEnd("mage");
	m_AlphaList.InsertAtEnd("magen");
	m_AlphaList.InsertAtEnd("mager");
	m_AlphaList.InsertAtEnd("magisk");
	m_AlphaList.InsertAtEnd("magkramp");
	m_AlphaList.InsertAtEnd("magnet");
	m_AlphaList.InsertAtEnd("magnetiskt");
	m_AlphaList.InsertAtEnd("maj");
	m_AlphaList.InsertAtEnd("majonns");
	m_AlphaList.InsertAtEnd("majs");
	m_AlphaList.InsertAtEnd("majsbrd");
	m_AlphaList.InsertAtEnd("majsflingor");
	m_AlphaList.InsertAtEnd("majsgrt");
	m_AlphaList.InsertAtEnd("majskakor");
	m_AlphaList.InsertAtEnd("majsmjl");
	m_AlphaList.InsertAtEnd("maka");
	m_AlphaList.InsertAtEnd("makar");
	m_AlphaList.InsertAtEnd("makaroner");
	m_AlphaList.InsertAtEnd("make");
	m_AlphaList.InsertAtEnd("makeup");
	m_AlphaList.InsertAtEnd("mal");
	m_AlphaList.InsertAtEnd("mala");
	m_AlphaList.InsertAtEnd("maler");
	m_AlphaList.InsertAtEnd("mamma");
	m_AlphaList.InsertAtEnd("man");
	m_AlphaList.InsertAtEnd("manager");
	m_AlphaList.InsertAtEnd("mandarin");
	m_AlphaList.InsertAtEnd("manet");
	m_AlphaList.InsertAtEnd("mangel");
	m_AlphaList.InsertAtEnd("mango");
	m_AlphaList.InsertAtEnd("manlig");
	m_AlphaList.InsertAtEnd("mannen");
	m_AlphaList.InsertAtEnd("mantel");
	m_AlphaList.InsertAtEnd("mapp");
	m_AlphaList.InsertAtEnd("maracas");
	m_AlphaList.InsertAtEnd("maraton");
	m_AlphaList.InsertAtEnd("mardrm");
	m_AlphaList.InsertAtEnd("margarin");
	m_AlphaList.InsertAtEnd("marinen");
	m_AlphaList.InsertAtEnd("mark");
	m_AlphaList.InsertAtEnd("markering");
	m_AlphaList.InsertAtEnd("marknad");
	m_AlphaList.InsertAtEnd("marknadsstnd");
	m_AlphaList.InsertAtEnd("markr ");
	m_AlphaList.InsertAtEnd("marmelad");
	m_AlphaList.InsertAtEnd("marmorkula");
	m_AlphaList.InsertAtEnd("marmorkulor");
	m_AlphaList.InsertAtEnd("mars");
	m_AlphaList.InsertAtEnd("marschera");
	m_AlphaList.InsertAtEnd("marscherar");
	m_AlphaList.InsertAtEnd("marshmallows");
	m_AlphaList.InsertAtEnd("marsvin");
	m_AlphaList.InsertAtEnd("mascara");
	m_AlphaList.InsertAtEnd("mask");
	m_AlphaList.InsertAtEnd("maskeradklder");
	m_AlphaList.InsertAtEnd("maskin");
	m_AlphaList.InsertAtEnd("massa");
	m_AlphaList.InsertAtEnd("massageapparat");
	m_AlphaList.InsertAtEnd("massera");
	m_AlphaList.InsertAtEnd("massiv");
	m_AlphaList.InsertAtEnd("mat");
	m_AlphaList.InsertAtEnd("mata");
	m_AlphaList.InsertAtEnd("matar");
	m_AlphaList.InsertAtEnd("matask");
	m_AlphaList.InsertAtEnd("matberedare");
	m_AlphaList.InsertAtEnd("matbit");
	m_AlphaList.InsertAtEnd("matcha");
	m_AlphaList.InsertAtEnd("matchar");
	m_AlphaList.InsertAtEnd("matematik");
	m_AlphaList.InsertAtEnd("matematiklektion");
	m_AlphaList.InsertAtEnd("material");
	m_AlphaList.InsertAtEnd("matlagning");
	m_AlphaList.InsertAtEnd("matrtt");
	m_AlphaList.InsertAtEnd("matsal");
	m_AlphaList.InsertAtEnd("matsmltning");
	m_AlphaList.InsertAtEnd("matt");
	m_AlphaList.InsertAtEnd("matta");
	m_AlphaList.InsertAtEnd("mattkniv");
	m_AlphaList.InsertAtEnd("mattsopare");
	m_AlphaList.InsertAtEnd("matvagn");
	m_AlphaList.InsertAtEnd("med");
	m_AlphaList.InsertAtEnd("medalj");
	m_AlphaList.InsertAtEnd("medan");
	m_AlphaList.InsertAtEnd("medarbetare");
	m_AlphaList.InsertAtEnd("medborgare");
	m_AlphaList.InsertAtEnd("meddelande");
	m_AlphaList.InsertAtEnd("medel");
	m_AlphaList.InsertAtEnd("medhjlpare");
	m_AlphaList.InsertAtEnd("medicin");
	m_AlphaList.InsertAtEnd("medicinboll");
	m_AlphaList.InsertAtEnd("medicinsk");
	m_AlphaList.InsertAtEnd("meditera");
	m_AlphaList.InsertAtEnd("mediterar");
	m_AlphaList.InsertAtEnd("medknsla");
	m_AlphaList.InsertAtEnd("medstrms");
	m_AlphaList.InsertAtEnd("mej");
	m_AlphaList.InsertAtEnd("mejeri");
	m_AlphaList.InsertAtEnd("mejeriprodukter");
	m_AlphaList.InsertAtEnd("mekaniker");
	m_AlphaList.InsertAtEnd("mellan");
	m_AlphaList.InsertAtEnd("mellanml");
	m_AlphaList.InsertAtEnd("mellanrum");
	m_AlphaList.InsertAtEnd("mellanslagstangent");
	m_AlphaList.InsertAtEnd("melon");
	m_AlphaList.InsertAtEnd("memory ");
	m_AlphaList.InsertAtEnd("men");
	m_AlphaList.InsertAtEnd("mena ");
	m_AlphaList.InsertAtEnd("menar");
	m_AlphaList.InsertAtEnd("mening");
	m_AlphaList.InsertAtEnd("meny");
	m_AlphaList.InsertAtEnd("mer");
	m_AlphaList.InsertAtEnd("mera");
	m_AlphaList.InsertAtEnd("mest");
	m_AlphaList.InsertAtEnd("metafor");
	m_AlphaList.InsertAtEnd("metall");
	m_AlphaList.InsertAtEnd("metalltrd");
	m_AlphaList.InsertAtEnd("meter");
	m_AlphaList.InsertAtEnd("metod");
	m_AlphaList.InsertAtEnd("middag");
	m_AlphaList.InsertAtEnd("midja");
	m_AlphaList.InsertAtEnd("midsommar");
	m_AlphaList.InsertAtEnd("midsommarafton");
	m_AlphaList.InsertAtEnd("mig");
	m_AlphaList.InsertAtEnd("mikrofon");
	m_AlphaList.InsertAtEnd("mikroskop");
	m_AlphaList.InsertAtEnd("mikrovgsugn");
	m_AlphaList.InsertAtEnd("militrflygplan");
	m_AlphaList.InsertAtEnd("militrtjnst");
	m_AlphaList.InsertAtEnd("miljoner");
	m_AlphaList.InsertAtEnd("milj");
	m_AlphaList.InsertAtEnd("milkshake");
	m_AlphaList.InsertAtEnd("min");
	m_AlphaList.InsertAtEnd("mina");
	m_AlphaList.InsertAtEnd("mindre");
	m_AlphaList.InsertAtEnd("mineralvatten");
	m_AlphaList.InsertAtEnd("miniatyrgolf");
	m_AlphaList.InsertAtEnd("minibuss");
	m_AlphaList.InsertAtEnd("minnesdag ");
	m_AlphaList.InsertAtEnd("minnesmrke");
	m_AlphaList.InsertAtEnd("minns");
	m_AlphaList.InsertAtEnd("minska");
	m_AlphaList.InsertAtEnd("minskar");
	m_AlphaList.InsertAtEnd("minst");
	m_AlphaList.InsertAtEnd("minus");
	m_AlphaList.InsertAtEnd("minut");
	m_AlphaList.InsertAtEnd("minuter");
	m_AlphaList.InsertAtEnd("miss");
	m_AlphaList.InsertAtEnd("missa");
	m_AlphaList.InsertAtEnd("missar");
	m_AlphaList.InsertAtEnd("missbruk");
	m_AlphaList.InsertAtEnd("missfall");
	m_AlphaList.InsertAtEnd("missfrstnd");
	m_AlphaList.InsertAtEnd("misshandel");
	m_AlphaList.InsertAtEnd("missionera");
	m_AlphaList.InsertAtEnd("missionerar");
	m_AlphaList.InsertAtEnd("misslyckas");
	m_AlphaList.InsertAtEnd("misstag");
	m_AlphaList.InsertAtEnd("mistel");
	m_AlphaList.InsertAtEnd("mitella");
	m_AlphaList.InsertAtEnd("mitt");
	m_AlphaList.InsertAtEnd("mitten");
	m_AlphaList.InsertAtEnd("mittgng");
	m_AlphaList.InsertAtEnd("mixa");
	m_AlphaList.InsertAtEnd("mixer");
	m_AlphaList.InsertAtEnd("mjuk");
	m_AlphaList.InsertAtEnd("mjuka");
	m_AlphaList.InsertAtEnd("mjukdjur");
	m_AlphaList.InsertAtEnd("mjukglass");
	m_AlphaList.InsertAtEnd("mjukost");
	m_AlphaList.InsertAtEnd("mjl");
	m_AlphaList.InsertAtEnd("mjlk");
	m_AlphaList.InsertAtEnd("mjlka");
	m_AlphaList.InsertAtEnd("mjlkar");
	m_AlphaList.InsertAtEnd("mjlkbud");
	m_AlphaList.InsertAtEnd("mjlkkanna");
	m_AlphaList.InsertAtEnd("mjlkmaskin");
	m_AlphaList.InsertAtEnd("mjlnare");
	m_AlphaList.InsertAtEnd("mobiltelefon");
	m_AlphaList.InsertAtEnd("mocka");
	m_AlphaList.InsertAtEnd("mockar");
	m_AlphaList.InsertAtEnd("modeller");
	m_AlphaList.InsertAtEnd("modellera");
	m_AlphaList.InsertAtEnd("modem");
	m_AlphaList.InsertAtEnd("modevisning");
	m_AlphaList.InsertAtEnd("modflld");
	m_AlphaList.InsertAtEnd("modig");
	m_AlphaList.InsertAtEnd("mognad");
	m_AlphaList.InsertAtEnd("moln");
	m_AlphaList.InsertAtEnd("molnig");
	m_AlphaList.InsertAtEnd("molnigt");
	m_AlphaList.InsertAtEnd("monitor");
	m_AlphaList.InsertAtEnd("monster");
	m_AlphaList.InsertAtEnd("montera");
	m_AlphaList.InsertAtEnd("monterar");
	m_AlphaList.InsertAtEnd("moped");
	m_AlphaList.InsertAtEnd("mopp");
	m_AlphaList.InsertAtEnd("mor");
	m_AlphaList.InsertAtEnd("moraliskt");
	m_AlphaList.InsertAtEnd("morbror");
	m_AlphaList.InsertAtEnd("morfar");
	m_AlphaList.InsertAtEnd("morfrlder");
	m_AlphaList.InsertAtEnd("morgon");
	m_AlphaList.InsertAtEnd("morgonrock");
	m_AlphaList.InsertAtEnd("mormor");
	m_AlphaList.InsertAtEnd("morot");
	m_AlphaList.InsertAtEnd("morra");
	m_AlphaList.InsertAtEnd("morrar");
	m_AlphaList.InsertAtEnd("mosa");
	m_AlphaList.InsertAtEnd("mosar");
	m_AlphaList.InsertAtEnd("mosk");
	m_AlphaList.InsertAtEnd("moster");
	m_AlphaList.InsertAtEnd("mot");
	m_AlphaList.InsertAtEnd("motell");
	m_AlphaList.InsertAtEnd("motion");
	m_AlphaList.InsertAtEnd("motionera");
	m_AlphaList.InsertAtEnd("motionerar ");
	m_AlphaList.InsertAtEnd("motionscykel");
	m_AlphaList.InsertAtEnd("motor");
	m_AlphaList.InsertAtEnd("motorbt");
	m_AlphaList.InsertAtEnd("motorcykel");
	m_AlphaList.InsertAtEnd("motorcykellopp");
	m_AlphaList.InsertAtEnd("motordriven");
	m_AlphaList.InsertAtEnd("motorfordon");
	m_AlphaList.InsertAtEnd("motorhuv");
	m_AlphaList.InsertAtEnd("motorolja");
	m_AlphaList.InsertAtEnd("motorsg");
	m_AlphaList.InsertAtEnd("motorvg");
	m_AlphaList.InsertAtEnd("motsats");
	m_AlphaList.InsertAtEnd("motsatt");
	m_AlphaList.InsertAtEnd("motstnd");
	m_AlphaList.InsertAtEnd("mott");
	m_AlphaList.InsertAtEnd("mottagning");
	m_AlphaList.InsertAtEnd("mottagningsrum");
	m_AlphaList.InsertAtEnd("motvilja");
	m_AlphaList.InsertAtEnd("muffins");
	m_AlphaList.InsertAtEnd("mugg");
	m_AlphaList.InsertAtEnd("mullvad");
	m_AlphaList.InsertAtEnd("multiplicera");
	m_AlphaList.InsertAtEnd("multiplicerar");
	m_AlphaList.InsertAtEnd("multiplikation");
	m_AlphaList.InsertAtEnd("mulsna");
	m_AlphaList.InsertAtEnd("mumie");
	m_AlphaList.InsertAtEnd("mun");
	m_AlphaList.InsertAtEnd("munk");
	m_AlphaList.InsertAtEnd("munnen");
	m_AlphaList.InsertAtEnd("munskydd");
	m_AlphaList.InsertAtEnd("munspel");
	m_AlphaList.InsertAtEnd("munter");
	m_AlphaList.InsertAtEnd("muntvtt");
	m_AlphaList.InsertAtEnd("mura");
	m_AlphaList.InsertAtEnd("murare");
	m_AlphaList.InsertAtEnd("murslev");
	m_AlphaList.InsertAtEnd("mus");
	m_AlphaList.InsertAtEnd("museum");
	m_AlphaList.InsertAtEnd("musik");
	m_AlphaList.InsertAtEnd("musikal");
	m_AlphaList.InsertAtEnd("musiker");
	m_AlphaList.InsertAtEnd("musikgrupp");
	m_AlphaList.InsertAtEnd("musikinstrument");
	m_AlphaList.InsertAtEnd("musikkr");
	m_AlphaList.InsertAtEnd("musiklektion");
	m_AlphaList.InsertAtEnd("musiksal");
	m_AlphaList.InsertAtEnd("musikstycke");
	m_AlphaList.InsertAtEnd("musikterapeut");
	m_AlphaList.InsertAtEnd("musikterapi");
	m_AlphaList.InsertAtEnd("musikvideo");
	m_AlphaList.InsertAtEnd("muskel");
	m_AlphaList.InsertAtEnd("muskeln");
	m_AlphaList.InsertAtEnd("musklick");
	m_AlphaList.InsertAtEnd("muslimsk");
	m_AlphaList.InsertAtEnd("mussla");
	m_AlphaList.InsertAtEnd("mustasch");
	m_AlphaList.InsertAtEnd("mutter");
	m_AlphaList.InsertAtEnd("mycket");
	m_AlphaList.InsertAtEnd("mygga");
	m_AlphaList.InsertAtEnd("mynt");
	m_AlphaList.InsertAtEnd("myra");
	m_AlphaList.InsertAtEnd("mysbyxor");
	m_AlphaList.InsertAtEnd("myskoxe");
	m_AlphaList.InsertAtEnd("mysli");
	m_AlphaList.InsertAtEnd("msli");
	m_AlphaList.InsertAtEnd("mysrum");
	m_AlphaList.InsertAtEnd("ml");
	m_AlphaList.InsertAtEnd("mla");
	m_AlphaList.InsertAtEnd("mlar");
	m_AlphaList.InsertAtEnd("mlarbok");
	m_AlphaList.InsertAtEnd("mlare");
	m_AlphaList.InsertAtEnd("mlarpensel");
	m_AlphaList.InsertAtEnd("mlspark");
	m_AlphaList.InsertAtEnd("mlstolpe");
	m_AlphaList.InsertAtEnd("mlstllning");
	m_AlphaList.InsertAtEnd("mltavla");
	m_AlphaList.InsertAtEnd("mltid");
	m_AlphaList.InsertAtEnd("mlvakt");
	m_AlphaList.InsertAtEnd("mnad");
	m_AlphaList.InsertAtEnd("mnaden");
	m_AlphaList.InsertAtEnd("mnader");
	m_AlphaList.InsertAtEnd("mnadstidning");
	m_AlphaList.InsertAtEnd("mnbil");
	m_AlphaList.InsertAtEnd("mndag");
	m_AlphaList.InsertAtEnd("mne");
	m_AlphaList.InsertAtEnd("mnen");
	m_AlphaList.InsertAtEnd("mnga");
	m_AlphaList.InsertAtEnd("mr");
	m_AlphaList.InsertAtEnd("ms");
	m_AlphaList.InsertAtEnd("mste");
	m_AlphaList.InsertAtEnd("mtt");
	m_AlphaList.InsertAtEnd("mttband");
	m_AlphaList.InsertAtEnd("mttskedar");
	m_AlphaList.InsertAtEnd("mn");
	m_AlphaList.InsertAtEnd("mngd");
	m_AlphaList.InsertAtEnd("mnniska");
	m_AlphaList.InsertAtEnd("mnniskor");
	m_AlphaList.InsertAtEnd("mnniskorna");
	m_AlphaList.InsertAtEnd("mrke");
	m_AlphaList.InsertAtEnd("mrkeskarta");
	m_AlphaList.InsertAtEnd("mrkestavla");
	m_AlphaList.InsertAtEnd("mrknl");
	m_AlphaList.InsertAtEnd("mrkpennor");
	m_AlphaList.InsertAtEnd("mssling");
	m_AlphaList.InsertAtEnd("mta");
	m_AlphaList.InsertAtEnd("mter");
	m_AlphaList.InsertAtEnd("mbel");
	m_AlphaList.InsertAtEnd("mbler");
	m_AlphaList.InsertAtEnd("mjlig");
	m_AlphaList.InsertAtEnd("mjliggr");
	m_AlphaList.InsertAtEnd("mjliggra");
	m_AlphaList.InsertAtEnd("mjlighet");
	m_AlphaList.InsertAtEnd("mjligheter");
	m_AlphaList.InsertAtEnd("mjligt");
	m_AlphaList.InsertAtEnd("mrda");
	m_AlphaList.InsertAtEnd("mrdar");
	m_AlphaList.InsertAtEnd("mrk");
	m_AlphaList.InsertAtEnd("mrkbl");
	m_AlphaList.InsertAtEnd("mssa");
	m_AlphaList.InsertAtEnd("mta");
	m_AlphaList.InsertAtEnd("mtas");
	m_AlphaList.InsertAtEnd("mte");
	m_AlphaList.InsertAtEnd("mter");
	m_AlphaList.InsertAtEnd("nacke");
	m_AlphaList.InsertAtEnd("nacken");
	m_AlphaList.InsertAtEnd("nafsa ");
	m_AlphaList.InsertAtEnd("nafsar");
	m_AlphaList.InsertAtEnd("nagelborste");
	m_AlphaList.InsertAtEnd("nagelfil");
	m_AlphaList.InsertAtEnd("nagellack");
	m_AlphaList.InsertAtEnd("nagellacksborttagare");
	m_AlphaList.InsertAtEnd("nageltng");
	m_AlphaList.InsertAtEnd("nagelvrd");
	m_AlphaList.InsertAtEnd("naglar");
	m_AlphaList.InsertAtEnd("naglarna");
	m_AlphaList.InsertAtEnd("naken");
	m_AlphaList.InsertAtEnd("namn");
	m_AlphaList.InsertAtEnd("namnstmpel");
	m_AlphaList.InsertAtEnd("namnteckning");
	m_AlphaList.InsertAtEnd("napp");
	m_AlphaList.InsertAtEnd("nappflaska");
	m_AlphaList.InsertAtEnd("narkotika");
	m_AlphaList.InsertAtEnd("narr");
	m_AlphaList.InsertAtEnd("nationalpark");
	m_AlphaList.InsertAtEnd("natt");
	m_AlphaList.InsertAtEnd("nattdrkt");
	m_AlphaList.InsertAtEnd("nattduksbord");
	m_AlphaList.InsertAtEnd("natten");
	m_AlphaList.InsertAtEnd("nattlig");
	m_AlphaList.InsertAtEnd("nattljus");
	m_AlphaList.InsertAtEnd("nattvard");
	m_AlphaList.InsertAtEnd("natur");
	m_AlphaList.InsertAtEnd("naturligtvis");
	m_AlphaList.InsertAtEnd("navel");
	m_AlphaList.InsertAtEnd("navelstrng");
	m_AlphaList.InsertAtEnd("necessr ");
	m_AlphaList.InsertAtEnd("ned");
	m_AlphaList.InsertAtEnd("nedfall");
	m_AlphaList.InsertAtEnd("nedt");
	m_AlphaList.InsertAtEnd("nej");
	m_AlphaList.InsertAtEnd("ner");
	m_AlphaList.InsertAtEnd("nere");
	m_AlphaList.InsertAtEnd("nervs");
	m_AlphaList.InsertAtEnd("ni");
	m_AlphaList.InsertAtEnd("nick");
	m_AlphaList.InsertAtEnd("nicka");
	m_AlphaList.InsertAtEnd("nickar");
	m_AlphaList.InsertAtEnd("nittio");
	m_AlphaList.InsertAtEnd("nitton");
	m_AlphaList.InsertAtEnd("njurar");
	m_AlphaList.InsertAtEnd("njursten");
	m_AlphaList.InsertAtEnd("nog");
	m_AlphaList.InsertAtEnd("noggrann");
	m_AlphaList.InsertAtEnd("nonsens");
	m_AlphaList.InsertAtEnd("nordlig");
	m_AlphaList.InsertAtEnd("nordpolen");
	m_AlphaList.InsertAtEnd("norr");
	m_AlphaList.InsertAtEnd("norra ");
	m_AlphaList.InsertAtEnd("nos");
	m_AlphaList.InsertAtEnd("noshrning");
	m_AlphaList.InsertAtEnd("not");
	m_AlphaList.InsertAtEnd("november");
	m_AlphaList.InsertAtEnd("nu");
	m_AlphaList.InsertAtEnd("nudlar");
	m_AlphaList.InsertAtEnd("nummer");
	m_AlphaList.InsertAtEnd("nutid");
	m_AlphaList.InsertAtEnd("nuvarande");
	m_AlphaList.InsertAtEnd("ny");
	m_AlphaList.InsertAtEnd("nya");
	m_AlphaList.InsertAtEnd("nyckel");
	m_AlphaList.InsertAtEnd("nyckelhl");
	m_AlphaList.InsertAtEnd("nyckelpiga");
	m_AlphaList.InsertAtEnd("nyckelring");
	m_AlphaList.InsertAtEnd("nyfiken");
	m_AlphaList.InsertAtEnd("nyhet");
	m_AlphaList.InsertAtEnd("nymlat");
	m_AlphaList.InsertAtEnd("nynna");
	m_AlphaList.InsertAtEnd("nynnar");
	m_AlphaList.InsertAtEnd("nypa");
	m_AlphaList.InsertAtEnd("nyper");
	m_AlphaList.InsertAtEnd("nysa");
	m_AlphaList.InsertAtEnd("nyser");
	m_AlphaList.InsertAtEnd("nyss");
	m_AlphaList.InsertAtEnd("nytt");
	m_AlphaList.InsertAtEnd("nyrsafton");
	m_AlphaList.InsertAtEnd("nyrsdagen");
	m_AlphaList.InsertAtEnd("n");
	m_AlphaList.InsertAtEnd("ngon");
	m_AlphaList.InsertAtEnd("ngons");
	m_AlphaList.InsertAtEnd("ngonsin");
	m_AlphaList.InsertAtEnd("ngonstans");
	m_AlphaList.InsertAtEnd("ngonting");
	m_AlphaList.InsertAtEnd("ngot");
	m_AlphaList.InsertAtEnd("ngra");
	m_AlphaList.InsertAtEnd("nl");
	m_AlphaList.InsertAtEnd("nldyna");
	m_AlphaList.InsertAtEnd("nn");
	m_AlphaList.InsertAtEnd("nnting");
	m_AlphaList.InsertAtEnd("nr");
	m_AlphaList.InsertAtEnd("nt");
	m_AlphaList.InsertAtEnd("nbb");
	m_AlphaList.InsertAtEnd("nmligen");
	m_AlphaList.InsertAtEnd("nr");
	m_AlphaList.InsertAtEnd("nra");
	m_AlphaList.InsertAtEnd("nrmande");
	m_AlphaList.InsertAtEnd("nrmare");
	m_AlphaList.InsertAtEnd("nrmast");
	m_AlphaList.InsertAtEnd("nsa");
	m_AlphaList.InsertAtEnd("nsan");
	m_AlphaList.InsertAtEnd("nsblod");
	m_AlphaList.InsertAtEnd("nsborre");
	m_AlphaList.InsertAtEnd("nsdropp");
	m_AlphaList.InsertAtEnd("nsdroppar");
	m_AlphaList.InsertAtEnd("nsduk");
	m_AlphaList.InsertAtEnd("nsta");
	m_AlphaList.InsertAtEnd("nstan");
	m_AlphaList.InsertAtEnd("nven");
	m_AlphaList.InsertAtEnd("ndlge");
	m_AlphaList.InsertAtEnd("ndvndig");
	m_AlphaList.InsertAtEnd("ndvndigt");
	m_AlphaList.InsertAtEnd("njd");
	m_AlphaList.InsertAtEnd("nt");
	m_AlphaList.InsertAtEnd("ntkreatur");
	m_AlphaList.InsertAtEnd("ntktt");
	m_AlphaList.InsertAtEnd("obegrnsad");
	m_AlphaList.InsertAtEnd("obehag");
	m_AlphaList.InsertAtEnd("obekvm");
	m_AlphaList.InsertAtEnd("oberoende");
	m_AlphaList.InsertAtEnd("obestmd");
	m_AlphaList.InsertAtEnd("observatorium");
	m_AlphaList.InsertAtEnd("observera");
	m_AlphaList.InsertAtEnd("observerar");
	m_AlphaList.InsertAtEnd("och");
	m_AlphaList.InsertAtEnd("ocks");
	m_AlphaList.InsertAtEnd("offentlig");
	m_AlphaList.InsertAtEnd("offer");
	m_AlphaList.InsertAtEnd("offra");
	m_AlphaList.InsertAtEnd("offrar");
	m_AlphaList.InsertAtEnd("ofredande");
	m_AlphaList.InsertAtEnd("ofruktsam");
	m_AlphaList.InsertAtEnd("ofta");
	m_AlphaList.InsertAtEnd("oftast");
	m_AlphaList.InsertAtEnd("ofrberedd");
	m_AlphaList.InsertAtEnd("ofrmga");
	m_AlphaList.InsertAtEnd("ofrskmd");
	m_AlphaList.InsertAtEnd("ogilla");
	m_AlphaList.InsertAtEnd("ogillar");
	m_AlphaList.InsertAtEnd("ogrs");
	m_AlphaList.InsertAtEnd("ojmn");
	m_AlphaList.InsertAtEnd("oktober");
	m_AlphaList.InsertAtEnd("okvda");
	m_AlphaList.InsertAtEnd("okvdar");
	m_AlphaList.InsertAtEnd("olika");
	m_AlphaList.InsertAtEnd("oliv");
	m_AlphaList.InsertAtEnd("olja");
	m_AlphaList.InsertAtEnd("oljeklla");
	m_AlphaList.InsertAtEnd("oljud");
	m_AlphaList.InsertAtEnd("olycka");
	m_AlphaList.InsertAtEnd("olydig");
	m_AlphaList.InsertAtEnd("olympiad ");
	m_AlphaList.InsertAtEnd("om");
	m_AlphaList.InsertAtEnd("omfatta");
	m_AlphaList.InsertAtEnd("omfattar");
	m_AlphaList.InsertAtEnd("omgivning");
	m_AlphaList.InsertAtEnd("omkldningsrum");
	m_AlphaList.InsertAtEnd("omkring");
	m_AlphaList.InsertAtEnd("omrde");
	m_AlphaList.InsertAtEnd("omrdet");
	m_AlphaList.InsertAtEnd("omslag");
	m_AlphaList.InsertAtEnd("omslagsflicka");
	m_AlphaList.InsertAtEnd("omslagspapper");
	m_AlphaList.InsertAtEnd("omtnksam");
	m_AlphaList.InsertAtEnd("omvandla");
	m_AlphaList.InsertAtEnd("omvandlar");
	m_AlphaList.InsertAtEnd("omvxling");
	m_AlphaList.InsertAtEnd("omjlig");
	m_AlphaList.InsertAtEnd("ond");
	m_AlphaList.InsertAtEnd("onsdag");
	m_AlphaList.InsertAtEnd("ont");
	m_AlphaList.InsertAtEnd("opera");
	m_AlphaList.InsertAtEnd("operation");
	m_AlphaList.InsertAtEnd("opposition ");
	m_AlphaList.InsertAtEnd("optiker");
	m_AlphaList.InsertAtEnd("orange");
	m_AlphaList.InsertAtEnd("ord");
	m_AlphaList.InsertAtEnd("ordbok");
	m_AlphaList.InsertAtEnd("order");
	m_AlphaList.InsertAtEnd("ordfrande");
	m_AlphaList.InsertAtEnd("ordination");
	m_AlphaList.InsertAtEnd("ordna");
	m_AlphaList.InsertAtEnd("ordnar");
	m_AlphaList.InsertAtEnd("ordning");
	m_AlphaList.InsertAtEnd("organ");
	m_AlphaList.InsertAtEnd("organisera");
	m_AlphaList.InsertAtEnd("orgasm");
	m_AlphaList.InsertAtEnd("orkan");
	m_AlphaList.InsertAtEnd("orkester");
	m_AlphaList.InsertAtEnd("orm");
	m_AlphaList.InsertAtEnd("oro");
	m_AlphaList.InsertAtEnd("oroa");
	m_AlphaList.InsertAtEnd("oroar");
	m_AlphaList.InsertAtEnd("orolig");
	m_AlphaList.InsertAtEnd("orsak");
	m_AlphaList.InsertAtEnd("orsaka");
	m_AlphaList.InsertAtEnd("orsakar");
	m_AlphaList.InsertAtEnd("orsakas");
	m_AlphaList.InsertAtEnd("orttvisa");
	m_AlphaList.InsertAtEnd("oskuld");
	m_AlphaList.InsertAtEnd("oskyldig");
	m_AlphaList.InsertAtEnd("oss");
	m_AlphaList.InsertAtEnd("ost");
	m_AlphaList.InsertAtEnd("osthyvel");
	m_AlphaList.InsertAtEnd("ostron");
	m_AlphaList.InsertAtEnd("ostskivor");
	m_AlphaList.InsertAtEnd("ostsnacks");
	m_AlphaList.InsertAtEnd("ostsuffl");
	m_AlphaList.InsertAtEnd("osker");
	m_AlphaList.InsertAtEnd("otrevlig");
	m_AlphaList.InsertAtEnd("otrevligt");
	m_AlphaList.InsertAtEnd("otrevnad");
	m_AlphaList.InsertAtEnd("otur");
	m_AlphaList.InsertAtEnd("otck");
	m_AlphaList.InsertAtEnd("oval");
	m_AlphaList.InsertAtEnd("ovan");
	m_AlphaList.InsertAtEnd("ovanfr");
	m_AlphaList.InsertAtEnd("overall");
	m_AlphaList.InsertAtEnd("overheadprojekter");
	m_AlphaList.InsertAtEnd("overklig");
	m_AlphaList.InsertAtEnd("oxe");
	m_AlphaList.InsertAtEnd("ondligt");
	m_AlphaList.InsertAtEnd("packa");
	m_AlphaList.InsertAtEnd("packar ");
	m_AlphaList.InsertAtEnd("padda");
	m_AlphaList.InsertAtEnd("paddel");
	m_AlphaList.InsertAtEnd("paddla ");
	m_AlphaList.InsertAtEnd("paddlar");
	m_AlphaList.InsertAtEnd("paella");
	m_AlphaList.InsertAtEnd("paj");
	m_AlphaList.InsertAtEnd("pajbotten");
	m_AlphaList.InsertAtEnd("paket");
	m_AlphaList.InsertAtEnd("paketera");
	m_AlphaList.InsertAtEnd("palett");
	m_AlphaList.InsertAtEnd("pall");
	m_AlphaList.InsertAtEnd("palm");
	m_AlphaList.InsertAtEnd("palmsndagen");
	m_AlphaList.InsertAtEnd("palsternacka");
	m_AlphaList.InsertAtEnd("panda");
	m_AlphaList.InsertAtEnd("panna");
	m_AlphaList.InsertAtEnd("pannan");
	m_AlphaList.InsertAtEnd("pannband");
	m_AlphaList.InsertAtEnd("pannkaka");
	m_AlphaList.InsertAtEnd("pannkakor");
	m_AlphaList.InsertAtEnd("pannlampa");
	m_AlphaList.InsertAtEnd("pannpinne");
	m_AlphaList.InsertAtEnd("papaya");
	m_AlphaList.InsertAtEnd("papegoja");
	m_AlphaList.InsertAtEnd("pappa");
	m_AlphaList.InsertAtEnd("papper");
	m_AlphaList.InsertAtEnd("pappershandduk");
	m_AlphaList.InsertAtEnd("pappershatt");
	m_AlphaList.InsertAtEnd("pappersklmma");
	m_AlphaList.InsertAtEnd("papperskorg");
	m_AlphaList.InsertAtEnd("pappersnsduk");
	m_AlphaList.InsertAtEnd("papperspse");
	m_AlphaList.InsertAtEnd("pappersrulle");
	m_AlphaList.InsertAtEnd("paprika");
	m_AlphaList.InsertAtEnd("par");
	m_AlphaList.InsertAtEnd("parad");
	m_AlphaList.InsertAtEnd("parallell");
	m_AlphaList.InsertAtEnd("Paralympics");
	m_AlphaList.InsertAtEnd("paraply");
	m_AlphaList.InsertAtEnd("parasoll");
	m_AlphaList.InsertAtEnd("parentes");
	m_AlphaList.InsertAtEnd("parfym");
	m_AlphaList.InsertAtEnd("pariserhjul");
	m_AlphaList.InsertAtEnd("park");
	m_AlphaList.InsertAtEnd("parkera ");
	m_AlphaList.InsertAtEnd("parkerar");
	m_AlphaList.InsertAtEnd("parkeringsbiljett");
	m_AlphaList.InsertAtEnd("parkeringsbter");
	m_AlphaList.InsertAtEnd("parkeringsplats");
	m_AlphaList.InsertAtEnd("parlament");
	m_AlphaList.InsertAtEnd("pass");
	m_AlphaList.InsertAtEnd("passa");
	m_AlphaList.InsertAtEnd("passagerare");
	m_AlphaList.InsertAtEnd("passar");
	m_AlphaList.InsertAtEnd("passare");
	m_AlphaList.InsertAtEnd("pastej");
	m_AlphaList.InsertAtEnd("pastor");
	m_AlphaList.InsertAtEnd("pat");
	m_AlphaList.InsertAtEnd("patiens");
	m_AlphaList.InsertAtEnd("patient");
	m_AlphaList.InsertAtEnd("paus");
	m_AlphaList.InsertAtEnd("paviljong");
	m_AlphaList.InsertAtEnd("pedal ");
	m_AlphaList.InsertAtEnd("pedaltunna");
	m_AlphaList.InsertAtEnd("pedikyr");
	m_AlphaList.InsertAtEnd("peka");
	m_AlphaList.InsertAtEnd("pekar");
	m_AlphaList.InsertAtEnd("pekfinger");
	m_AlphaList.InsertAtEnd("pekskrm");
	m_AlphaList.InsertAtEnd("pelikan");
	m_AlphaList.InsertAtEnd("pengar");
	m_AlphaList.InsertAtEnd("penna");
	m_AlphaList.InsertAtEnd("pennfodral");
	m_AlphaList.InsertAtEnd("pennskrin");
	m_AlphaList.InsertAtEnd("pennvssare");
	m_AlphaList.InsertAtEnd("pensel");
	m_AlphaList.InsertAtEnd("pension");
	m_AlphaList.InsertAtEnd("pensionerad");
	m_AlphaList.InsertAtEnd("peppar");
	m_AlphaList.InsertAtEnd("pepparkaka");
	m_AlphaList.InsertAtEnd("pepparkvarn");
	m_AlphaList.InsertAtEnd("pepparrot");
	m_AlphaList.InsertAtEnd("per");
	m_AlphaList.InsertAtEnd("period");
	m_AlphaList.InsertAtEnd("permanent");
	m_AlphaList.InsertAtEnd("persienner");
	m_AlphaList.InsertAtEnd("persika");
	m_AlphaList.InsertAtEnd("persilja");
	m_AlphaList.InsertAtEnd("person");
	m_AlphaList.InsertAtEnd("personal");
	m_AlphaList.InsertAtEnd("personalrum");
	m_AlphaList.InsertAtEnd("personer");
	m_AlphaList.InsertAtEnd("personlift");
	m_AlphaList.InsertAtEnd("personlig");
	m_AlphaList.InsertAtEnd("personskare");
	m_AlphaList.InsertAtEnd("peruk");
	m_AlphaList.InsertAtEnd("pesetas");
	m_AlphaList.InsertAtEnd("peta");
	m_AlphaList.InsertAtEnd("petar");
	m_AlphaList.InsertAtEnd("piano");
	m_AlphaList.InsertAtEnd("pickles");
	m_AlphaList.InsertAtEnd("picknick");
	m_AlphaList.InsertAtEnd("picknickbord");
	m_AlphaList.InsertAtEnd("picknickkorg");
	m_AlphaList.InsertAtEnd("pictogram");
	m_AlphaList.InsertAtEnd("piggsvin");
	m_AlphaList.InsertAtEnd("pil");
	m_AlphaList.InsertAtEnd("pilar");
	m_AlphaList.InsertAtEnd("pilgrim");
	m_AlphaList.InsertAtEnd("piller");
	m_AlphaList.InsertAtEnd("pilot");
	m_AlphaList.InsertAtEnd("pilspel");
	m_AlphaList.InsertAtEnd("pincett");
	m_AlphaList.InsertAtEnd("pingpong");
	m_AlphaList.InsertAtEnd("pingst");
	m_AlphaList.InsertAtEnd("pingvin");
	m_AlphaList.InsertAtEnd("pinnar");
	m_AlphaList.InsertAtEnd("pinne");
	m_AlphaList.InsertAtEnd("pinsamhet");
	m_AlphaList.InsertAtEnd("pipa");
	m_AlphaList.InsertAtEnd("piper");
	m_AlphaList.InsertAtEnd("pipett");
	m_AlphaList.InsertAtEnd("pipmugg");
	m_AlphaList.InsertAtEnd("pirat");
	m_AlphaList.InsertAtEnd("piska");
	m_AlphaList.InsertAtEnd("pistol");
	m_AlphaList.InsertAtEnd("pitabrd");
	m_AlphaList.InsertAtEnd("pizza");
	m_AlphaList.InsertAtEnd("pizzass");
	m_AlphaList.InsertAtEnd("pizzeria");
	m_AlphaList.InsertAtEnd("pjs");
	m_AlphaList.InsertAtEnd("pjxor");
	m_AlphaList.InsertAtEnd("plan");
	m_AlphaList.InsertAtEnd("planera");
	m_AlphaList.InsertAtEnd("planerar");
	m_AlphaList.InsertAtEnd("planeringstavla");
	m_AlphaList.InsertAtEnd("planet");
	m_AlphaList.InsertAtEnd("planetarium");
	m_AlphaList.InsertAtEnd("planta");
	m_AlphaList.InsertAtEnd("plantera");
	m_AlphaList.InsertAtEnd("planterar");
	m_AlphaList.InsertAtEnd("planteringsspade");
	m_AlphaList.InsertAtEnd("plaska");
	m_AlphaList.InsertAtEnd("plaskar");
	m_AlphaList.InsertAtEnd("plaskdamm");
	m_AlphaList.InsertAtEnd("plast");
	m_AlphaList.InsertAtEnd("plastpse");
	m_AlphaList.InsertAtEnd("plats");
	m_AlphaList.InsertAtEnd("platt");
	m_AlphaList.InsertAtEnd("plattform");
	m_AlphaList.InsertAtEnd("plocka");
	m_AlphaList.InsertAtEnd("plockar");
	m_AlphaList.InsertAtEnd("plog");
	m_AlphaList.InsertAtEnd("plommon");
	m_AlphaList.InsertAtEnd("plommonpur");
	m_AlphaList.InsertAtEnd("plunta");
	m_AlphaList.InsertAtEnd("plus");
	m_AlphaList.InsertAtEnd("plnbok");
	m_AlphaList.InsertAtEnd("plster");
	m_AlphaList.InsertAtEnd("plttar");
	m_AlphaList.InsertAtEnd("plja");
	m_AlphaList.InsertAtEnd("pljer");
	m_AlphaList.InsertAtEnd("pltsligt");
	m_AlphaList.InsertAtEnd("podium");
	m_AlphaList.InsertAtEnd("poesi");
	m_AlphaList.InsertAtEnd("poet");
	m_AlphaList.InsertAtEnd("pojke");
	m_AlphaList.InsertAtEnd("pojkvn");
	m_AlphaList.InsertAtEnd("polera");
	m_AlphaList.InsertAtEnd("polerar");
	m_AlphaList.InsertAtEnd("polis");
	m_AlphaList.InsertAtEnd("polisbil");
	m_AlphaList.InsertAtEnd("polisen");
	m_AlphaList.InsertAtEnd("polismssa");
	m_AlphaList.InsertAtEnd("polisstation");
	m_AlphaList.InsertAtEnd("polisuniform");
	m_AlphaList.InsertAtEnd("politik");
	m_AlphaList.InsertAtEnd("politiska");
	m_AlphaList.InsertAtEnd("pollinera");
	m_AlphaList.InsertAtEnd("pollinerar");
	m_AlphaList.InsertAtEnd("ponny");
	m_AlphaList.InsertAtEnd("ponnykrra");
	m_AlphaList.InsertAtEnd("ponnyvagn");
	m_AlphaList.InsertAtEnd("pool");
	m_AlphaList.InsertAtEnd("popcorn");
	m_AlphaList.InsertAtEnd("populrmusik");
	m_AlphaList.InsertAtEnd("porslin");
	m_AlphaList.InsertAtEnd("portflj");
	m_AlphaList.InsertAtEnd("portmonn");
	m_AlphaList.InsertAtEnd("post");
	m_AlphaList.InsertAtEnd("posta");
	m_AlphaList.InsertAtEnd("postbil");
	m_AlphaList.InsertAtEnd("posten");
	m_AlphaList.InsertAtEnd("poster");
	m_AlphaList.InsertAtEnd("postkontor");
	m_AlphaList.InsertAtEnd("postlda");
	m_AlphaList.InsertAtEnd("postsck");
	m_AlphaList.InsertAtEnd("postvska");
	m_AlphaList.InsertAtEnd("potatis");
	m_AlphaList.InsertAtEnd("potatiskniv");
	m_AlphaList.InsertAtEnd("potatismos");
	m_AlphaList.InsertAtEnd("potatispltt");
	m_AlphaList.InsertAtEnd("potatisskal");
	m_AlphaList.InsertAtEnd("potatisskalare");
	m_AlphaList.InsertAtEnd("potta");
	m_AlphaList.InsertAtEnd("pong");
	m_AlphaList.InsertAtEnd("prata");
	m_AlphaList.InsertAtEnd("pratar");
	m_AlphaList.InsertAtEnd("precis");
	m_AlphaList.InsertAtEnd("predika");
	m_AlphaList.InsertAtEnd("predikar");
	m_AlphaList.InsertAtEnd("premirminister");
	m_AlphaList.InsertAtEnd("present");
	m_AlphaList.InsertAtEnd("presentaffr");
	m_AlphaList.InsertAtEnd("presentera");
	m_AlphaList.InsertAtEnd("presenterar");
	m_AlphaList.InsertAtEnd("pressa");
	m_AlphaList.InsertAtEnd("pressar");
	m_AlphaList.InsertAtEnd("preventivmedel");
	m_AlphaList.InsertAtEnd("prickig");
	m_AlphaList.InsertAtEnd("prins");
	m_AlphaList.InsertAtEnd("prinsessa");
	m_AlphaList.InsertAtEnd("pris");
	m_AlphaList.InsertAtEnd("prisa");
	m_AlphaList.InsertAtEnd("prisar");
	m_AlphaList.InsertAtEnd("prisceremoni");
	m_AlphaList.InsertAtEnd("prislapp");
	m_AlphaList.InsertAtEnd("privat");
	m_AlphaList.InsertAtEnd("problem");
	m_AlphaList.InsertAtEnd("procent");
	m_AlphaList.InsertAtEnd("produkt");
	m_AlphaList.InsertAtEnd("program");
	m_AlphaList.InsertAtEnd("programmera");
	m_AlphaList.InsertAtEnd("programmerar");
	m_AlphaList.InsertAtEnd("projektera");
	m_AlphaList.InsertAtEnd("projekterar");
	m_AlphaList.InsertAtEnd("projektor");
	m_AlphaList.InsertAtEnd("promenad");
	m_AlphaList.InsertAtEnd("propp");
	m_AlphaList.InsertAtEnd("protein");
	m_AlphaList.InsertAtEnd("protestera");
	m_AlphaList.InsertAtEnd("protesterar");
	m_AlphaList.InsertAtEnd("protokoll");
	m_AlphaList.InsertAtEnd("prov");
	m_AlphaList.InsertAtEnd("prova");
	m_AlphaList.InsertAtEnd("provar");
	m_AlphaList.InsertAtEnd("provrr");
	m_AlphaList.InsertAtEnd("provrrshllare");
	m_AlphaList.InsertAtEnd("prydnadssak");
	m_AlphaList.InsertAtEnd("prrie");
	m_AlphaList.InsertAtEnd("prrievarg");
	m_AlphaList.InsertAtEnd("prst");
	m_AlphaList.InsertAtEnd("prstkrage");
	m_AlphaList.InsertAtEnd("psykolog");
	m_AlphaList.InsertAtEnd("pubertet");
	m_AlphaList.InsertAtEnd("publik");
	m_AlphaList.InsertAtEnd("pudding");
	m_AlphaList.InsertAtEnd("puder");
	m_AlphaList.InsertAtEnd("pulver");
	m_AlphaList.InsertAtEnd("pulverkaffe");
	m_AlphaList.InsertAtEnd("pulverte");
	m_AlphaList.InsertAtEnd("pumpa");
	m_AlphaList.InsertAtEnd("pumpar");
	m_AlphaList.InsertAtEnd("pund");
	m_AlphaList.InsertAtEnd("pung");
	m_AlphaList.InsertAtEnd("punkt");
	m_AlphaList.InsertAtEnd("punktering");
	m_AlphaList.InsertAtEnd("punktskriftskrivare");
	m_AlphaList.InsertAtEnd("puppa");
	m_AlphaList.InsertAtEnd("purjolk");
	m_AlphaList.InsertAtEnd("pussa");
	m_AlphaList.InsertAtEnd("pussar ");
	m_AlphaList.InsertAtEnd("pussas ");
	m_AlphaList.InsertAtEnd("pussel");
	m_AlphaList.InsertAtEnd("pusselbit");
	m_AlphaList.InsertAtEnd("pussla");
	m_AlphaList.InsertAtEnd("pusslar");
	m_AlphaList.InsertAtEnd("pyjamas");
	m_AlphaList.InsertAtEnd("pyramid");
	m_AlphaList.InsertAtEnd("p");
	m_AlphaList.InsertAtEnd("pfgel");
	m_AlphaList.InsertAtEnd("plgg");
	m_AlphaList.InsertAtEnd("pminna");
	m_AlphaList.InsertAtEnd("pminner");
	m_AlphaList.InsertAtEnd("pse");
	m_AlphaList.InsertAtEnd("psk");
	m_AlphaList.InsertAtEnd("pskbjllra");
	m_AlphaList.InsertAtEnd("pskhare");
	m_AlphaList.InsertAtEnd("psklamm");
	m_AlphaList.InsertAtEnd("psklilja");
	m_AlphaList.InsertAtEnd("pskljus");
	m_AlphaList.InsertAtEnd("pskris");
	m_AlphaList.InsertAtEnd("pskgg");
	m_AlphaList.InsertAtEnd("pskggsjakt");
	m_AlphaList.InsertAtEnd("pve");
	m_AlphaList.InsertAtEnd("pls");
	m_AlphaList.InsertAtEnd("prla");
	m_AlphaList.InsertAtEnd("prlor");
	m_AlphaList.InsertAtEnd("prm");
	m_AlphaList.InsertAtEnd("pron");
	m_AlphaList.InsertAtEnd("pl");
	m_AlphaList.InsertAtEnd("psstol");
	m_AlphaList.InsertAtEnd("rabarber");
	m_AlphaList.InsertAtEnd("rabbi");
	m_AlphaList.InsertAtEnd("rabies");
	m_AlphaList.InsertAtEnd("racerbil");
	m_AlphaList.InsertAtEnd("racket");
	m_AlphaList.InsertAtEnd("rad");
	m_AlphaList.InsertAtEnd("radband");
	m_AlphaList.InsertAtEnd("radera");
	m_AlphaList.InsertAtEnd("raderar");
	m_AlphaList.InsertAtEnd("radergummi");
	m_AlphaList.InsertAtEnd("radio");
	m_AlphaList.InsertAtEnd("radioaktiv");
	m_AlphaList.InsertAtEnd("radioaktivt");
	m_AlphaList.InsertAtEnd("radiobil");
	m_AlphaList.InsertAtEnd("rak");
	m_AlphaList.InsertAtEnd("raka");
	m_AlphaList.InsertAtEnd("rakapparat");
	m_AlphaList.InsertAtEnd("rakapparaten");
	m_AlphaList.InsertAtEnd("rakar");
	m_AlphaList.InsertAtEnd("raket");
	m_AlphaList.InsertAtEnd("raketpistol");
	m_AlphaList.InsertAtEnd("rakhyvel");
	m_AlphaList.InsertAtEnd("rakkrm");
	m_AlphaList.InsertAtEnd("rakt");
	m_AlphaList.InsertAtEnd("rakvatten");
	m_AlphaList.InsertAtEnd("ram");
	m_AlphaList.InsertAtEnd("ramla");
	m_AlphaList.InsertAtEnd("ramlar");
	m_AlphaList.InsertAtEnd("ramp");
	m_AlphaList.InsertAtEnd("randig");
	m_AlphaList.InsertAtEnd("rap");
	m_AlphaList.InsertAtEnd("rapa");
	m_AlphaList.InsertAtEnd("rapar");
	m_AlphaList.InsertAtEnd("rapmusik");
	m_AlphaList.InsertAtEnd("rapport");
	m_AlphaList.InsertAtEnd("rar");
	m_AlphaList.InsertAtEnd("rast");
	m_AlphaList.InsertAtEnd("rastplats");
	m_AlphaList.InsertAtEnd("ratt");
	m_AlphaList.InsertAtEnd("ravioli");
	m_AlphaList.InsertAtEnd("recept");
	m_AlphaList.InsertAtEnd("reception ");
	m_AlphaList.InsertAtEnd("receptlda");
	m_AlphaList.InsertAtEnd("reda");
	m_AlphaList.InsertAtEnd("redan");
	m_AlphaList.InsertAtEnd("redskap");
	m_AlphaList.InsertAtEnd("reflektera");
	m_AlphaList.InsertAtEnd("reflekterar");
	m_AlphaList.InsertAtEnd("reflexhammare");
	m_AlphaList.InsertAtEnd("regel");
	m_AlphaList.InsertAtEnd("regera");
	m_AlphaList.InsertAtEnd("regerar");
	m_AlphaList.InsertAtEnd("regering");
	m_AlphaList.InsertAtEnd("regeringen");
	m_AlphaList.InsertAtEnd("regeringsbyggnad");
	m_AlphaList.InsertAtEnd("registrera");
	m_AlphaList.InsertAtEnd("registrerar");
	m_AlphaList.InsertAtEnd("regn");
	m_AlphaList.InsertAtEnd("regna");
	m_AlphaList.InsertAtEnd("regnar");
	m_AlphaList.InsertAtEnd("regnig");
	m_AlphaList.InsertAtEnd("regnkappa");
	m_AlphaList.InsertAtEnd("regnklder");
	m_AlphaList.InsertAtEnd("regnskog");
	m_AlphaList.InsertAtEnd("regnstav");
	m_AlphaList.InsertAtEnd("rekommendation");
	m_AlphaList.InsertAtEnd("rektangel");
	m_AlphaList.InsertAtEnd("rektor");
	m_AlphaList.InsertAtEnd("religion");
	m_AlphaList.InsertAtEnd("religis");
	m_AlphaList.InsertAtEnd("ren");
	m_AlphaList.InsertAtEnd("rengr");
	m_AlphaList.InsertAtEnd("rengring");
	m_AlphaList.InsertAtEnd("rengringsmedel");
	m_AlphaList.InsertAtEnd("rensa");
	m_AlphaList.InsertAtEnd("rensar");
	m_AlphaList.InsertAtEnd("rent");
	m_AlphaList.InsertAtEnd("reparera");
	m_AlphaList.InsertAtEnd("reparerar");
	m_AlphaList.InsertAtEnd("repellera");
	m_AlphaList.InsertAtEnd("repellerar");
	m_AlphaList.InsertAtEnd("resa");
	m_AlphaList.InsertAtEnd("resebroschyr");
	m_AlphaList.InsertAtEnd("resenr");
	m_AlphaList.InsertAtEnd("reser");
	m_AlphaList.InsertAtEnd("reservoarpenna");
	m_AlphaList.InsertAtEnd("resonansskiva ");
	m_AlphaList.InsertAtEnd("respekt");
	m_AlphaList.InsertAtEnd("respirator");
	m_AlphaList.InsertAtEnd("restaurang");
	m_AlphaList.InsertAtEnd("reste");
	m_AlphaList.InsertAtEnd("resten");
	m_AlphaList.InsertAtEnd("resultat");
	m_AlphaList.InsertAtEnd("resultattavla");
	m_AlphaList.InsertAtEnd("resurscentral");
	m_AlphaList.InsertAtEnd("resurser");
	m_AlphaList.InsertAtEnd("resvska");
	m_AlphaList.InsertAtEnd("reta");
	m_AlphaList.InsertAtEnd("retar");
	m_AlphaList.InsertAtEnd("reva");
	m_AlphaList.InsertAtEnd("revben");
	m_AlphaList.InsertAtEnd("revolution");
	m_AlphaList.InsertAtEnd("ribba");
	m_AlphaList.InsertAtEnd("rida ");
	m_AlphaList.InsertAtEnd("riddare");
	m_AlphaList.InsertAtEnd("rider");
	m_AlphaList.InsertAtEnd("ridhjlm");
	m_AlphaList.InsertAtEnd("ridning");
	m_AlphaList.InsertAtEnd("ridskola");
	m_AlphaList.InsertAtEnd("rik");
	m_AlphaList.InsertAtEnd("riksdagen");
	m_AlphaList.InsertAtEnd("riktig");
	m_AlphaList.InsertAtEnd("riktigt");
	m_AlphaList.InsertAtEnd("riktning");
	m_AlphaList.InsertAtEnd("rim");
	m_AlphaList.InsertAtEnd("ring");
	m_AlphaList.InsertAtEnd("ringa");
	m_AlphaList.InsertAtEnd("ringar");
	m_AlphaList.InsertAtEnd("ringer");
	m_AlphaList.InsertAtEnd("ringfinger");
	m_AlphaList.InsertAtEnd("ringklocka");
	m_AlphaList.InsertAtEnd("rinnande");
	m_AlphaList.InsertAtEnd("ris");
	m_AlphaList.InsertAtEnd("riskaka");
	m_AlphaList.InsertAtEnd("rita");
	m_AlphaList.InsertAtEnd("ritar");
	m_AlphaList.InsertAtEnd("ritpapper");
	m_AlphaList.InsertAtEnd("ritverktyg");
	m_AlphaList.InsertAtEnd("riva");
	m_AlphaList.InsertAtEnd("river");
	m_AlphaList.InsertAtEnd("rivjrn");
	m_AlphaList.InsertAtEnd("roa");
	m_AlphaList.InsertAtEnd("robot");
	m_AlphaList.InsertAtEnd("rock");
	m_AlphaList.InsertAtEnd("rocken");
	m_AlphaList.InsertAtEnd("rockmusik");
	m_AlphaList.InsertAtEnd("roddbt");
	m_AlphaList.InsertAtEnd("rodna");
	m_AlphaList.InsertAtEnd("rodnar");
	m_AlphaList.InsertAtEnd("rolig");
	m_AlphaList.InsertAtEnd("roligt");
	m_AlphaList.InsertAtEnd("roll");
	m_AlphaList.InsertAtEnd("roller");
	m_AlphaList.InsertAtEnd("romantik");
	m_AlphaList.InsertAtEnd("romantisk ");
	m_AlphaList.InsertAtEnd("romb");
	m_AlphaList.InsertAtEnd("ropa");
	m_AlphaList.InsertAtEnd("ropa ");
	m_AlphaList.InsertAtEnd("ropar");
	m_AlphaList.InsertAtEnd("ror ");
	m_AlphaList.InsertAtEnd("ros");
	m_AlphaList.InsertAtEnd("rosa");
	m_AlphaList.InsertAtEnd("rosett");
	m_AlphaList.InsertAtEnd("rosta");
	m_AlphaList.InsertAtEnd("rostar");
	m_AlphaList.InsertAtEnd("rostat");
	m_AlphaList.InsertAtEnd("rostbiff");
	m_AlphaList.InsertAtEnd("rota");
	m_AlphaList.InsertAtEnd("rotar");
	m_AlphaList.InsertAtEnd("rotfrukt");
	m_AlphaList.InsertAtEnd("rouge");
	m_AlphaList.InsertAtEnd("rova");
	m_AlphaList.InsertAtEnd("rovdjur");
	m_AlphaList.InsertAtEnd("ruin");
	m_AlphaList.InsertAtEnd("rulla");
	m_AlphaList.InsertAtEnd("rullar");
	m_AlphaList.InsertAtEnd("rullator");
	m_AlphaList.InsertAtEnd("rullboll");
	m_AlphaList.InsertAtEnd("rullbrda");
	m_AlphaList.InsertAtEnd("rulle");
	m_AlphaList.InsertAtEnd("rullskridskor");
	m_AlphaList.InsertAtEnd("rullstol");
	m_AlphaList.InsertAtEnd("rullstolstvling");
	m_AlphaList.InsertAtEnd("rulltrappa");
	m_AlphaList.InsertAtEnd("rum");
	m_AlphaList.InsertAtEnd("rummet");
	m_AlphaList.InsertAtEnd("rumpa");
	m_AlphaList.InsertAtEnd("rund");
	m_AlphaList.InsertAtEnd("runt");
	m_AlphaList.InsertAtEnd("rusa");
	m_AlphaList.InsertAtEnd("rusar");
	m_AlphaList.InsertAtEnd("russin");
	m_AlphaList.InsertAtEnd("ruta");
	m_AlphaList.InsertAtEnd("rutat");
	m_AlphaList.InsertAtEnd("ruter");
	m_AlphaList.InsertAtEnd("rutschbana");
	m_AlphaList.InsertAtEnd("rutschkana");
	m_AlphaList.InsertAtEnd("rycka");
	m_AlphaList.InsertAtEnd("rycker");
	m_AlphaList.InsertAtEnd("rygg");
	m_AlphaList.InsertAtEnd("ryggrad");
	m_AlphaList.InsertAtEnd("ryggsim");
	m_AlphaList.InsertAtEnd("ryggskott");
	m_AlphaList.InsertAtEnd("ryggstd");
	m_AlphaList.InsertAtEnd("ryggsg");
	m_AlphaList.InsertAtEnd("ryggsck");
	m_AlphaList.InsertAtEnd("rymddrkt");
	m_AlphaList.InsertAtEnd("rymdhandskar");
	m_AlphaList.InsertAtEnd("rymdhjlm");
	m_AlphaList.InsertAtEnd("rymdskepp");
	m_AlphaList.InsertAtEnd("rymdstation");
	m_AlphaList.InsertAtEnd("rymma");
	m_AlphaList.InsertAtEnd("rymmer");
	m_AlphaList.InsertAtEnd("rynka");
	m_AlphaList.InsertAtEnd("rynkar");
	m_AlphaList.InsertAtEnd("rysk");
	m_AlphaList.InsertAtEnd("ryta");
	m_AlphaList.InsertAtEnd("ryter");
	m_AlphaList.InsertAtEnd("rytmpinnar");
	m_AlphaList.InsertAtEnd("rd");
	m_AlphaList.InsertAtEnd("rda");
	m_AlphaList.InsertAtEnd("rdgivare");
	m_AlphaList.InsertAtEnd("rdhus");
	m_AlphaList.InsertAtEnd("rdjur");
	m_AlphaList.InsertAtEnd("rgbrd");
	m_AlphaList.InsertAtEnd("rgpudding");
	m_AlphaList.InsertAtEnd("rma");
	m_AlphaList.InsertAtEnd("rmar");
	m_AlphaList.InsertAtEnd("rriven");
	m_AlphaList.InsertAtEnd("rtt");
	m_AlphaList.InsertAtEnd("rtta");
	m_AlphaList.InsertAtEnd("rttsvans");
	m_AlphaList.InsertAtEnd("rdd");
	m_AlphaList.InsertAtEnd("rdda");
	m_AlphaList.InsertAtEnd("rddar");
	m_AlphaList.InsertAtEnd("rdisa");
	m_AlphaList.InsertAtEnd("rka");
	m_AlphaList.InsertAtEnd("rkna");
	m_AlphaList.InsertAtEnd("rknar");
	m_AlphaList.InsertAtEnd("rkning");
	m_AlphaList.InsertAtEnd("rnna");
	m_AlphaList.InsertAtEnd("rnnan");
	m_AlphaList.InsertAtEnd("rt");
	m_AlphaList.InsertAtEnd("rtt");
	m_AlphaList.InsertAtEnd("rtta");
	m_AlphaList.InsertAtEnd("rttvis");
	m_AlphaList.InsertAtEnd("rv");
	m_AlphaList.InsertAtEnd("rd");
	m_AlphaList.InsertAtEnd("rdbeta");
	m_AlphaList.InsertAtEnd("rdhrig");
	m_AlphaList.InsertAtEnd("rdkl");
	m_AlphaList.InsertAtEnd("rk");
	m_AlphaList.InsertAtEnd("rka");
	m_AlphaList.InsertAtEnd("rkdetektor");
	m_AlphaList.InsertAtEnd("rkelse");
	m_AlphaList.InsertAtEnd("rker");
	m_AlphaList.InsertAtEnd("rkfrbud");
	m_AlphaList.InsertAtEnd("rntgen");
	m_AlphaList.InsertAtEnd("rr");
	m_AlphaList.InsertAtEnd("rra");
	m_AlphaList.InsertAtEnd("rrde");
	m_AlphaList.InsertAtEnd("rrmokare");
	m_AlphaList.InsertAtEnd("rst");
	m_AlphaList.InsertAtEnd("rsta");
	m_AlphaList.InsertAtEnd("rstar");
	m_AlphaList.InsertAtEnd("rstfrstrkare");
	m_AlphaList.InsertAtEnd("rtter");
	m_AlphaList.InsertAtEnd("sa");
	m_AlphaList.InsertAtEnd("sabbatsdag");
	m_AlphaList.InsertAtEnd("sabbatskvll");
	m_AlphaList.InsertAtEnd("sade");
	m_AlphaList.InsertAtEnd("sadel");
	m_AlphaList.InsertAtEnd("saffran");
	m_AlphaList.InsertAtEnd("saftis");
	m_AlphaList.InsertAtEnd("saga");
	m_AlphaList.InsertAtEnd("sagoberttare");
	m_AlphaList.InsertAtEnd("sagoprins");
	m_AlphaList.InsertAtEnd("sagt");
	m_AlphaList.InsertAtEnd("sak");
	m_AlphaList.InsertAtEnd("saken");
	m_AlphaList.InsertAtEnd("saker");
	m_AlphaList.InsertAtEnd("sakna");
	m_AlphaList.InsertAtEnd("saknad");
	m_AlphaList.InsertAtEnd("sakrament");
	m_AlphaList.InsertAtEnd("sakta");
	m_AlphaList.InsertAtEnd("saliv");
	m_AlphaList.InsertAtEnd("sallad");
	m_AlphaList.InsertAtEnd("salladsbar");
	m_AlphaList.InsertAtEnd("salladsdressing");
	m_AlphaList.InsertAtEnd("salladshuvud");
	m_AlphaList.InsertAtEnd("salsa");
	m_AlphaList.InsertAtEnd("salt");
	m_AlphaList.InsertAtEnd("salta");
	m_AlphaList.InsertAtEnd("salva");
	m_AlphaList.InsertAtEnd("samband");
	m_AlphaList.InsertAtEnd("samhlle");
	m_AlphaList.InsertAtEnd("samhllet");
	m_AlphaList.InsertAtEnd("samla");
	m_AlphaList.InsertAtEnd("samlar");
	m_AlphaList.InsertAtEnd("samlas");
	m_AlphaList.InsertAtEnd("samling");
	m_AlphaList.InsertAtEnd("samlingsdags");
	m_AlphaList.InsertAtEnd("samlingsplats");
	m_AlphaList.InsertAtEnd("samlingsstund");
	m_AlphaList.InsertAtEnd("samma");
	m_AlphaList.InsertAtEnd("samman");
	m_AlphaList.InsertAtEnd("sammanbrott");
	m_AlphaList.InsertAtEnd("sammandragning");
	m_AlphaList.InsertAtEnd("sammanfattning");
	m_AlphaList.InsertAtEnd("sammanhang");
	m_AlphaList.InsertAtEnd("sammanstta");
	m_AlphaList.InsertAtEnd("sammanstter");
	m_AlphaList.InsertAtEnd("sammantrde");
	m_AlphaList.InsertAtEnd("samt");
	m_AlphaList.InsertAtEnd("samtal");
	m_AlphaList.InsertAtEnd("samtala");
	m_AlphaList.InsertAtEnd("samtalar ");
	m_AlphaList.InsertAtEnd("samtidigt");
	m_AlphaList.InsertAtEnd("samtycka");
	m_AlphaList.InsertAtEnd("samtycker");
	m_AlphaList.InsertAtEnd("samvete");
	m_AlphaList.InsertAtEnd("sand");
	m_AlphaList.InsertAtEnd("sandal");
	m_AlphaList.InsertAtEnd("sandaler");
	m_AlphaList.InsertAtEnd("sanddyna");
	m_AlphaList.InsertAtEnd("sandlekbord");
	m_AlphaList.InsertAtEnd("sandleksak");
	m_AlphaList.InsertAtEnd("sandlda");
	m_AlphaList.InsertAtEnd("sandpapper");
	m_AlphaList.InsertAtEnd("sandslott");
	m_AlphaList.InsertAtEnd("sangria");
	m_AlphaList.InsertAtEnd("sankmark");
	m_AlphaList.InsertAtEnd("sanning");
}

#endif

BOOL CWordAid2Dlg::LoadDemoAlphaList()
{
	m_AlphaList.RemoveAll();

	m_AlphaList.m_bSorted = FALSE;
#ifdef _SWED_DEMO

	CProgressWnd* pWnd = new CProgressWnd(this, _T("Hämtar alfabetisk fil..."), FALSE, FALSE);

	pWnd->SetRange(0,1000);
	pWnd->SetText("Hämtar fil...");
	pWnd->SetStep(20);

	LoadList1();
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	LoadList2();
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	LoadList3();
	pWnd->SetStep(100);
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	LoadList4();
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	LoadList5();
	pWnd->SetStep(200);
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	
	pWnd->SetStep(10);
	m_AlphaList.InsertAtEnd("sant");pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("sari");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("satt");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("satte");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("sax");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("saxofon");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("scarf");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("scen");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("schack");
	m_AlphaList.InsertAtEnd("schackbrde");
	m_AlphaList.InsertAtEnd("schampo");
	m_AlphaList.InsertAtEnd("schema");
	m_AlphaList.InsertAtEnd("schimpans");
	m_AlphaList.InsertAtEnd("schysst");
	m_AlphaList.InsertAtEnd("scout");
	m_AlphaList.InsertAtEnd("se");
	m_AlphaList.InsertAtEnd("sebra");
	m_AlphaList.InsertAtEnd("sedan");
	m_AlphaList.InsertAtEnd("sedel");
	m_AlphaList.InsertAtEnd("sedlar");
	m_AlphaList.InsertAtEnd("segel");
	m_AlphaList.InsertAtEnd("segelbt");
	m_AlphaList.InsertAtEnd("segla");
	m_AlphaList.InsertAtEnd("seglar");
	m_AlphaList.InsertAtEnd("segra");
	m_AlphaList.InsertAtEnd("segrar");
	m_AlphaList.InsertAtEnd("segrare");
	m_AlphaList.InsertAtEnd("sej");
	m_AlphaList.InsertAtEnd("sekatr");
	m_AlphaList.InsertAtEnd("sekreterare");
	m_AlphaList.InsertAtEnd("sekund");
	m_AlphaList.InsertAtEnd("selleri");
	m_AlphaList.InsertAtEnd("semester");
	m_AlphaList.InsertAtEnd("sen");
	m_AlphaList.InsertAtEnd("senap");
	m_AlphaList.InsertAtEnd("senapsblad");
	m_AlphaList.InsertAtEnd("senare");
	m_AlphaList.InsertAtEnd("senaste");
	m_AlphaList.InsertAtEnd("sent");
	m_AlphaList.InsertAtEnd("september");
	m_AlphaList.InsertAtEnd("ser");
	m_AlphaList.InsertAtEnd("serie");
	m_AlphaList.InsertAtEnd("seriefigurer");
	m_AlphaList.InsertAtEnd("serier");
	m_AlphaList.InsertAtEnd("serietidning");
	m_AlphaList.InsertAtEnd("servera");
	m_AlphaList.InsertAtEnd("serverar");
	m_AlphaList.InsertAtEnd("serveringsvagn");
	m_AlphaList.InsertAtEnd("servett");
	m_AlphaList.InsertAtEnd("servitris");
	m_AlphaList.InsertAtEnd("ses");
	m_AlphaList.InsertAtEnd("sett");
	m_AlphaList.InsertAtEnd("sex");
	m_AlphaList.InsertAtEnd("sexig");
	m_AlphaList.InsertAtEnd("sextio");
	m_AlphaList.InsertAtEnd("sexton");
	m_AlphaList.InsertAtEnd("sexualitet");
	m_AlphaList.InsertAtEnd("sfinx");
	m_AlphaList.InsertAtEnd("shoppingvska");
	m_AlphaList.InsertAtEnd("shorts");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("sida");
	m_AlphaList.InsertAtEnd("sidan");
	m_AlphaList.InsertAtEnd("siffra");
	m_AlphaList.InsertAtEnd("sifon");
	m_AlphaList.InsertAtEnd("sig");
	m_AlphaList.InsertAtEnd("signal");
	m_AlphaList.InsertAtEnd("signalhorn");
	m_AlphaList.InsertAtEnd("signalkort");
	m_AlphaList.InsertAtEnd("signera");
	m_AlphaList.InsertAtEnd("signerar");
	m_AlphaList.InsertAtEnd("sil");
	m_AlphaList.InsertAtEnd("sila");
	m_AlphaList.InsertAtEnd("silar");
	m_AlphaList.InsertAtEnd("silver");
	m_AlphaList.InsertAtEnd("simbassng");
	m_AlphaList.InsertAtEnd("simdrkt");
	m_AlphaList.InsertAtEnd("simftter");
	m_AlphaList.InsertAtEnd("simglasgon");
	m_AlphaList.InsertAtEnd("simhall");
	m_AlphaList.InsertAtEnd("simlektion");
	m_AlphaList.InsertAtEnd("simma");
	m_AlphaList.InsertAtEnd("simmar");
	m_AlphaList.InsertAtEnd("simmare");
	m_AlphaList.InsertAtEnd("simring");
	m_AlphaList.InsertAtEnd("sin");
	m_AlphaList.InsertAtEnd("sina");
	m_AlphaList.InsertAtEnd("sinnen");
	m_AlphaList.InsertAtEnd("sinnesbord");
	m_AlphaList.InsertAtEnd("sirap");
	m_AlphaList.InsertAtEnd("sirn");
	m_AlphaList.InsertAtEnd("sist");
	m_AlphaList.InsertAtEnd("sista");
	m_AlphaList.InsertAtEnd("sits");
	m_AlphaList.InsertAtEnd("sitt");
	m_AlphaList.InsertAtEnd("sitta");
	m_AlphaList.InsertAtEnd("sitter");
	m_AlphaList.InsertAtEnd("situation");
	m_AlphaList.InsertAtEnd("situationen");
	m_AlphaList.InsertAtEnd("sjakal ");
	m_AlphaList.InsertAtEnd("sjal");
	m_AlphaList.InsertAtEnd("sju");
	m_AlphaList.InsertAtEnd("sjuda");
	m_AlphaList.InsertAtEnd("sjuder");
	m_AlphaList.InsertAtEnd("sjuk");
	m_AlphaList.InsertAtEnd("sjukdom");
	m_AlphaList.InsertAtEnd("sjukfrskring");
	m_AlphaList.InsertAtEnd("sjukgymnast");
	m_AlphaList.InsertAtEnd("sjukgymnastik");
	m_AlphaList.InsertAtEnd("sjukhus");
	m_AlphaList.InsertAtEnd("sjukhussng");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("sjukhustossor");
	m_AlphaList.InsertAtEnd("sjukhustrja");
	m_AlphaList.InsertAtEnd("sjukskterska");
	m_AlphaList.InsertAtEnd("sjunga");
	m_AlphaList.InsertAtEnd("sjunger");
	m_AlphaList.InsertAtEnd("sjunka");
	m_AlphaList.InsertAtEnd("sjunker");
	m_AlphaList.InsertAtEnd("sjuttio");
	m_AlphaList.InsertAtEnd("sjutton");
	m_AlphaList.InsertAtEnd("sjlv");
	m_AlphaList.InsertAtEnd("sjlva");
	m_AlphaList.InsertAtEnd("sjlvbehrskning");
	m_AlphaList.InsertAtEnd("sjlvbiografi");
	m_AlphaList.InsertAtEnd("sjlvisk");
	m_AlphaList.InsertAtEnd("sjlvklart");
	m_AlphaList.InsertAtEnd("sj");
	m_AlphaList.InsertAtEnd("sjhst");
	m_AlphaList.InsertAtEnd("sjjungfru");
	m_AlphaList.InsertAtEnd("sjman");
	m_AlphaList.InsertAtEnd("sjstjrna");
	m_AlphaList.InsertAtEnd("ska");
	m_AlphaList.InsertAtEnd("skada");
	m_AlphaList.InsertAtEnd("skadar");
	m_AlphaList.InsertAtEnd("skadeinsekt");
	m_AlphaList.InsertAtEnd("skaft");
	m_AlphaList.InsertAtEnd("skaka");
	m_AlphaList.InsertAtEnd("skakar");
	m_AlphaList.InsertAtEnd("skal");
	m_AlphaList.InsertAtEnd("skala");
	m_AlphaList.InsertAtEnd("skalar");
	m_AlphaList.InsertAtEnd("skalbagge");
	m_AlphaList.InsertAtEnd("skall");
	m_AlphaList.InsertAtEnd("skallerorm");
	m_AlphaList.InsertAtEnd("skallra");
	m_AlphaList.InsertAtEnd("skam");
	m_AlphaList.InsertAtEnd("skamsen");
	m_AlphaList.InsertAtEnd("skamvr");
	m_AlphaList.InsertAtEnd("skapa");
	m_AlphaList.InsertAtEnd("skapar");
	m_AlphaList.InsertAtEnd("skapelse");
	m_AlphaList.InsertAtEnd("skata");
	m_AlphaList.InsertAtEnd("skateboard");
	m_AlphaList.InsertAtEnd("skattkista");
	m_AlphaList.InsertAtEnd("ske");
	m_AlphaList.InsertAtEnd("sked");
	m_AlphaList.InsertAtEnd("skelett");
	m_AlphaList.InsertAtEnd("skepp");
	m_AlphaList.InsertAtEnd("sker");
	m_AlphaList.InsertAtEnd("skev");
	m_AlphaList.InsertAtEnd("skicka");
	m_AlphaList.InsertAtEnd("skickar ");
	m_AlphaList.InsertAtEnd("skidglasgon");
	m_AlphaList.InsertAtEnd("skidlift");
	m_AlphaList.InsertAtEnd("skidor");
	m_AlphaList.InsertAtEnd("skidkare");
	m_AlphaList.InsertAtEnd("skiftnyckel");
	m_AlphaList.InsertAtEnd("skilja");
	m_AlphaList.InsertAtEnd("skiljer");
	m_AlphaList.InsertAtEnd("skilsmssa");
	m_AlphaList.InsertAtEnd("skina");
	m_AlphaList.InsertAtEnd("skiner ");
	m_AlphaList.InsertAtEnd("skingra");
	m_AlphaList.InsertAtEnd("skingrar");
	m_AlphaList.InsertAtEnd("skinka");
	m_AlphaList.InsertAtEnd("skinn");
	m_AlphaList.InsertAtEnd("skitsnack");
	m_AlphaList.InsertAtEnd("skiva");
	m_AlphaList.InsertAtEnd("skivaffr");
	m_AlphaList.InsertAtEnd("skivar");
	m_AlphaList.InsertAtEnd("skivomslag");
	m_AlphaList.InsertAtEnd("skivspelare");
	m_AlphaList.InsertAtEnd("skivstng");
	m_AlphaList.InsertAtEnd("skjorta");
	m_AlphaList.InsertAtEnd("skjul");
	m_AlphaList.InsertAtEnd("skjuta");
	m_AlphaList.InsertAtEnd("skjutbrda");
	m_AlphaList.InsertAtEnd("skjuter");
	m_AlphaList.InsertAtEnd("skjutvapen");
	m_AlphaList.InsertAtEnd("sko");
	m_AlphaList.InsertAtEnd("skoaffr");
	m_AlphaList.InsertAtEnd("skog");
	m_AlphaList.InsertAtEnd("skogsarbetare");
	m_AlphaList.InsertAtEnd("skoja");
	m_AlphaList.InsertAtEnd("skojar");
	m_AlphaList.InsertAtEnd("skokrm");
	m_AlphaList.InsertAtEnd("skola");
	m_AlphaList.InsertAtEnd("skolan");
	m_AlphaList.InsertAtEnd("skolbutik");
	m_AlphaList.InsertAtEnd("skolbnk");
	m_AlphaList.InsertAtEnd("skolomrde");
	m_AlphaList.InsertAtEnd("skolstart");
	m_AlphaList.InsertAtEnd("skolvska");
	m_AlphaList.InsertAtEnd("skopa");
	m_AlphaList.InsertAtEnd("skoputsning");
	m_AlphaList.InsertAtEnd("skor");
	m_AlphaList.InsertAtEnd("skorna");
	m_AlphaList.InsertAtEnd("skorpa");
	m_AlphaList.InsertAtEnd("skorpion");
	m_AlphaList.InsertAtEnd("skorsten");
	m_AlphaList.InsertAtEnd("skosnre");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("skottkrra");
	m_AlphaList.InsertAtEnd("skrapa");
	m_AlphaList.InsertAtEnd("skrapar");
	m_AlphaList.InsertAtEnd("skratt");
	m_AlphaList.InsertAtEnd("skratta");
	m_AlphaList.InsertAtEnd("skrattade");
	m_AlphaList.InsertAtEnd("skrattar");
	m_AlphaList.InsertAtEnd("skrev");
	m_AlphaList.InsertAtEnd("skridskor");
	m_AlphaList.InsertAtEnd("skridskokare");
	m_AlphaList.InsertAtEnd("skrika");
	m_AlphaList.InsertAtEnd("skriv");
	m_AlphaList.InsertAtEnd("skriva");
	m_AlphaList.InsertAtEnd("skrivare");
	m_AlphaList.InsertAtEnd("skrivbok");
	m_AlphaList.InsertAtEnd("skrivbord");
	m_AlphaList.InsertAtEnd("skriver");
	m_AlphaList.InsertAtEnd("skrivmaskin");
	m_AlphaList.InsertAtEnd("skrivplatta");
	m_AlphaList.InsertAtEnd("skrubbsr");
	m_AlphaList.InsertAtEnd("skruv");
	m_AlphaList.InsertAtEnd("skruva");
	m_AlphaList.InsertAtEnd("skruvar");
	m_AlphaList.InsertAtEnd("skruvmejsel");
	m_AlphaList.InsertAtEnd("skrynklig");
	m_AlphaList.InsertAtEnd("skrckslagen");
	m_AlphaList.InsertAtEnd("skrddare");
	m_AlphaList.InsertAtEnd("skrmma");
	m_AlphaList.InsertAtEnd("skrmmande");
	m_AlphaList.InsertAtEnd("skrmmas");
	m_AlphaList.InsertAtEnd("skrmmer");
	m_AlphaList.InsertAtEnd("skrp");
	m_AlphaList.InsertAtEnd("skrpet");
	m_AlphaList.InsertAtEnd("skugga");
	m_AlphaList.InsertAtEnd("skuld");
	m_AlphaList.InsertAtEnd("skuldfri");
	m_AlphaList.InsertAtEnd("skuldra");
	m_AlphaList.InsertAtEnd("skull");
	m_AlphaList.InsertAtEnd("skulle");
	m_AlphaList.InsertAtEnd("skum");
	m_AlphaList.InsertAtEnd("skunk");
	m_AlphaList.InsertAtEnd("skurtrasa");
	m_AlphaList.InsertAtEnd("skutta");
	m_AlphaList.InsertAtEnd("skuttar");
	m_AlphaList.InsertAtEnd("skydd");
	m_AlphaList.InsertAtEnd("skydda");
	m_AlphaList.InsertAtEnd("skyddar");
	m_AlphaList.InsertAtEnd("skyddshjlm");
	m_AlphaList.InsertAtEnd("skyddsklder");
	m_AlphaList.InsertAtEnd("skyddsmedel");
	m_AlphaList.InsertAtEnd("skyddsrock");
	m_AlphaList.InsertAtEnd("skyddsrum");
	m_AlphaList.InsertAtEnd("skyddsutrustning");
	m_AlphaList.InsertAtEnd("skyffel");
	m_AlphaList.InsertAtEnd("skyffla");
	m_AlphaList.InsertAtEnd("skyfflar");
	m_AlphaList.InsertAtEnd("skyldig");
	m_AlphaList.InsertAtEnd("skynda");
	m_AlphaList.InsertAtEnd("skyndar");
	m_AlphaList.InsertAtEnd("skl");
	m_AlphaList.InsertAtEnd("skp");
	m_AlphaList.InsertAtEnd("skpbil");
	m_AlphaList.InsertAtEnd("skgg");
	m_AlphaList.InsertAtEnd("skl");
	m_AlphaList.InsertAtEnd("sklla");
	m_AlphaList.InsertAtEnd("skller");
	m_AlphaList.InsertAtEnd("skr");
	m_AlphaList.InsertAtEnd("skra");
	m_AlphaList.InsertAtEnd("skrbrda");
	m_AlphaList.InsertAtEnd("skrbna");
	m_AlphaList.InsertAtEnd("skrm");
	m_AlphaList.InsertAtEnd("skrp");
	m_AlphaList.InsertAtEnd("skld");
	m_AlphaList.InsertAtEnd("skldpadda");
	m_AlphaList.InsertAtEnd("sklja");
	m_AlphaList.InsertAtEnd("skljer");
	m_AlphaList.InsertAtEnd("skljmedel");
	m_AlphaList.InsertAtEnd("skn");
	m_AlphaList.InsertAtEnd("sknhetssalong");
	m_AlphaList.InsertAtEnd("sknhetstvling");
	m_AlphaList.InsertAtEnd("skr");
	m_AlphaList.InsertAtEnd("skrd");
	m_AlphaList.InsertAtEnd("skrda");
	m_AlphaList.InsertAtEnd("skrdar");
	m_AlphaList.InsertAtEnd("skrdefest ");
	m_AlphaList.InsertAtEnd("sladd");
	m_AlphaList.InsertAtEnd("slag");
	m_AlphaList.InsertAtEnd("slags");
	m_AlphaList.InsertAtEnd("slagtr");
	m_AlphaList.InsertAtEnd("slaktare");
	m_AlphaList.InsertAtEnd("slang");
	m_AlphaList.InsertAtEnd("slant");
	m_AlphaList.InsertAtEnd("slappna");
	m_AlphaList.InsertAtEnd("slappnar ");
	m_AlphaList.InsertAtEnd("slasken");
	m_AlphaList.InsertAtEnd("slaveri");
	m_AlphaList.InsertAtEnd("slem");
	m_AlphaList.InsertAtEnd("slev");
	m_AlphaList.InsertAtEnd("slicka");
	m_AlphaList.InsertAtEnd("slickar");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("slickepinne");
	m_AlphaList.InsertAtEnd("slickepott");
	m_AlphaList.InsertAtEnd("slida");
	m_AlphaList.InsertAtEnd("slingor");
	m_AlphaList.InsertAtEnd("slingra");
	m_AlphaList.InsertAtEnd("slingrar");
	m_AlphaList.InsertAtEnd("slingrig");
	m_AlphaList.InsertAtEnd("slipa");
	m_AlphaList.InsertAtEnd("slipar");
	m_AlphaList.InsertAtEnd("slipmaskin");
	m_AlphaList.InsertAtEnd("slips");
	m_AlphaList.InsertAtEnd("slira");
	m_AlphaList.InsertAtEnd("slog");
	m_AlphaList.InsertAtEnd("slott");
	m_AlphaList.InsertAtEnd("slump");
	m_AlphaList.InsertAtEnd("slut");
	m_AlphaList.InsertAtEnd("sluta");
	m_AlphaList.InsertAtEnd("slutar");
	m_AlphaList.InsertAtEnd("sluten");
	m_AlphaList.InsertAtEnd("slutet");
	m_AlphaList.InsertAtEnd("slutligen");
	m_AlphaList.InsertAtEnd("slutmlet");
	m_AlphaList.InsertAtEnd("sl");
	m_AlphaList.InsertAtEnd("slr");
	m_AlphaList.InsertAtEnd("slss");
	m_AlphaList.InsertAtEnd("slckt");
	m_AlphaList.InsertAtEnd("slde");
	m_AlphaList.InsertAtEnd("slgga");
	m_AlphaList.InsertAtEnd("slkting");
	m_AlphaList.InsertAtEnd("slngkyss");
	m_AlphaList.InsertAtEnd("slppa");
	m_AlphaList.InsertAtEnd("slpper");
	m_AlphaList.InsertAtEnd("slt");
	m_AlphaList.InsertAtEnd("sl");
	m_AlphaList.InsertAtEnd("slja");
	m_AlphaList.InsertAtEnd("smacka");
	m_AlphaList.InsertAtEnd("smak");
	m_AlphaList.InsertAtEnd("smaka");
	m_AlphaList.InsertAtEnd("smakar");
	m_AlphaList.InsertAtEnd("smal");
	m_AlphaList.InsertAtEnd("smart");
	m_AlphaList.InsertAtEnd("smed");
	m_AlphaList.InsertAtEnd("smeka");
	m_AlphaList.InsertAtEnd("smeker");
	m_AlphaList.InsertAtEnd("smeta");
	m_AlphaList.InsertAtEnd("smetar");
	m_AlphaList.InsertAtEnd("smink");
	m_AlphaList.InsertAtEnd("sminka ");
	m_AlphaList.InsertAtEnd("sminkar");
	m_AlphaList.InsertAtEnd("smula ");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("smular");
	m_AlphaList.InsertAtEnd("smulas");
	m_AlphaList.InsertAtEnd("smultron");
	m_AlphaList.InsertAtEnd("smutsig");
	m_AlphaList.InsertAtEnd("smycken");
	m_AlphaList.InsertAtEnd("smyga");
	m_AlphaList.InsertAtEnd("smyger");
	m_AlphaList.InsertAtEnd("smygtittande");
	m_AlphaList.InsertAtEnd("smygvinkel");
	m_AlphaList.InsertAtEnd("sm");
	m_AlphaList.InsertAtEnd("smprata");
	m_AlphaList.InsertAtEnd("smpratar");
	m_AlphaList.InsertAtEnd("smlla");
	m_AlphaList.InsertAtEnd("smllkaramell");
	m_AlphaList.InsertAtEnd("smlta");
	m_AlphaList.InsertAtEnd("smlter");
	m_AlphaList.InsertAtEnd("smrta");
	m_AlphaList.InsertAtEnd("smrtar");
	m_AlphaList.InsertAtEnd("smrtsam");
	m_AlphaList.InsertAtEnd("smr");
	m_AlphaList.InsertAtEnd("smrgs");
	m_AlphaList.InsertAtEnd("smrgsplgg");
	m_AlphaList.InsertAtEnd("smrja");
	m_AlphaList.InsertAtEnd("snabb");
	m_AlphaList.InsertAtEnd("snabbmat");
	m_AlphaList.InsertAtEnd("snabbt");
	m_AlphaList.InsertAtEnd("snabel-a");
	m_AlphaList.InsertAtEnd("snacka");
	m_AlphaList.InsertAtEnd("snackar");
	m_AlphaList.InsertAtEnd("snacks");
	m_AlphaList.InsertAtEnd("snart");
	m_AlphaList.InsertAtEnd("snaska");
	m_AlphaList.InsertAtEnd("snaskar");
	m_AlphaList.InsertAtEnd("snattra");
	m_AlphaList.InsertAtEnd("snattrar");
	m_AlphaList.InsertAtEnd("snickare");
	m_AlphaList.InsertAtEnd("snickeri");
	m_AlphaList.InsertAtEnd("snigel");
	m_AlphaList.InsertAtEnd("snok");
	m_AlphaList.InsertAtEnd("snoka");
	m_AlphaList.InsertAtEnd("snokar");
	m_AlphaList.InsertAtEnd("snorkel");
	m_AlphaList.InsertAtEnd("snowboard");
	m_AlphaList.InsertAtEnd("snurra");
	m_AlphaList.InsertAtEnd("snurrar");
	m_AlphaList.InsertAtEnd("snurrig");
	m_AlphaList.InsertAtEnd("snyta");
	m_AlphaList.InsertAtEnd("snyter");
	m_AlphaList.InsertAtEnd("snckskal");
	m_AlphaList.InsertAtEnd("sn");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("sna");
	m_AlphaList.InsertAtEnd("snar");
	m_AlphaList.InsertAtEnd("snboll");
	m_AlphaList.InsertAtEnd("snflinga");
	m_AlphaList.InsertAtEnd("sngubbe");
	m_AlphaList.InsertAtEnd("snkngor");
	m_AlphaList.InsertAtEnd("snre");
	m_AlphaList.InsertAtEnd("snskor");
	m_AlphaList.InsertAtEnd("snskoter");
	m_AlphaList.InsertAtEnd("snstorm");
	m_AlphaList.InsertAtEnd("snstvlar");
	m_AlphaList.InsertAtEnd("socialdemokrat");
	m_AlphaList.InsertAtEnd("socker");
	m_AlphaList.InsertAtEnd("sockerbeta");
	m_AlphaList.InsertAtEnd("sockerskl");
	m_AlphaList.InsertAtEnd("sockor");
	m_AlphaList.InsertAtEnd("sockrad");
	m_AlphaList.InsertAtEnd("soffa");
	m_AlphaList.InsertAtEnd("sol");
	m_AlphaList.InsertAtEnd("sola");
	m_AlphaList.InsertAtEnd("solar");
	m_AlphaList.InsertAtEnd("solarium");
	m_AlphaList.InsertAtEnd("solbada");
	m_AlphaList.InsertAtEnd("solbadar");
	m_AlphaList.InsertAtEnd("solbrnna");
	m_AlphaList.InsertAtEnd("soldat");
	m_AlphaList.InsertAtEnd("solfjder");
	m_AlphaList.InsertAtEnd("solglasgon");
	m_AlphaList.InsertAtEnd("solig");
	m_AlphaList.InsertAtEnd("solkrm");
	m_AlphaList.InsertAtEnd("solnedgng");
	m_AlphaList.InsertAtEnd("solros");
	m_AlphaList.InsertAtEnd("solskrm");
	m_AlphaList.InsertAtEnd("solsystem");
	m_AlphaList.InsertAtEnd("soluppgng");
	m_AlphaList.InsertAtEnd("som");
	m_AlphaList.InsertAtEnd("sommar");
	m_AlphaList.InsertAtEnd("sommarlov");
	m_AlphaList.InsertAtEnd("sommarstlle");
	m_AlphaList.InsertAtEnd("son");
	m_AlphaList.InsertAtEnd("sopa");
	m_AlphaList.InsertAtEnd("sopar");
	m_AlphaList.InsertAtEnd("sopbil");
	m_AlphaList.InsertAtEnd("sopcontainer");
	m_AlphaList.InsertAtEnd("sophmtare");
	m_AlphaList.InsertAtEnd("soppa");
	m_AlphaList.InsertAtEnd("soppskl");
	m_AlphaList.InsertAtEnd("sopptallrik");
	m_AlphaList.InsertAtEnd("soppse");
	m_AlphaList.InsertAtEnd("sopskyffel");pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("soptunna");
	m_AlphaList.InsertAtEnd("sorg");
	m_AlphaList.InsertAtEnd("sork");
	m_AlphaList.InsertAtEnd("sort");
	m_AlphaList.InsertAtEnd("sortera");
	m_AlphaList.InsertAtEnd("sorterar");
	m_AlphaList.InsertAtEnd("sorteringslda");
	m_AlphaList.InsertAtEnd("sorteringsskiva");
	m_AlphaList.InsertAtEnd("sova");
	m_AlphaList.InsertAtEnd("sovrum");
	m_AlphaList.InsertAtEnd("sovsal");
	m_AlphaList.InsertAtEnd("sovsck");
	m_AlphaList.InsertAtEnd("spade");
	m_AlphaList.InsertAtEnd("spader");
	m_AlphaList.InsertAtEnd("spagetti");
	m_AlphaList.InsertAtEnd("spagettiss");
	m_AlphaList.InsertAtEnd("spam");
	m_AlphaList.InsertAtEnd("spara");
	m_AlphaList.InsertAtEnd("sparar");
	m_AlphaList.InsertAtEnd("spargris");
	m_AlphaList.InsertAtEnd("sparka");
	m_AlphaList.InsertAtEnd("sparkar");
	m_AlphaList.InsertAtEnd("sparkcykel");
	m_AlphaList.InsertAtEnd("sparris");
	m_AlphaList.InsertAtEnd("sparsam");
	m_AlphaList.InsertAtEnd("spatel");
	m_AlphaList.InsertAtEnd("speciell");
	m_AlphaList.InsertAtEnd("spegel");
	m_AlphaList.InsertAtEnd("spegla");
	m_AlphaList.InsertAtEnd("speglar");
	m_AlphaList.InsertAtEnd("spel");
	m_AlphaList.InsertAtEnd("spela");
	m_AlphaList.InsertAtEnd("spelar");
	m_AlphaList.InsertAtEnd("spelbord");
	m_AlphaList.InsertAtEnd("spelhall");
	m_AlphaList.InsertAtEnd("spelkort");
	m_AlphaList.InsertAtEnd("spenat");
	m_AlphaList.InsertAtEnd("spets");
	m_AlphaList.InsertAtEnd("spetsig");
	m_AlphaList.InsertAtEnd("spett");
	m_AlphaList.InsertAtEnd("spik");
	m_AlphaList.InsertAtEnd("spika");
	m_AlphaList.InsertAtEnd("spikar");
	m_AlphaList.InsertAtEnd("spilla");
	m_AlphaList.InsertAtEnd("spiller");
	m_AlphaList.InsertAtEnd("spindel");
	m_AlphaList.InsertAtEnd("spindelnt");
	m_AlphaList.InsertAtEnd("spinna");
	m_AlphaList.InsertAtEnd("spinner");
	m_AlphaList.InsertAtEnd("spinnrock");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("spionera");
	m_AlphaList.InsertAtEnd("spionerar");
	m_AlphaList.InsertAtEnd("spira");
	m_AlphaList.InsertAtEnd("spiral");
	m_AlphaList.InsertAtEnd("spiralform");
	m_AlphaList.InsertAtEnd("spis");
	m_AlphaList.InsertAtEnd("spjla");
	m_AlphaList.InsertAtEnd("spola");
	m_AlphaList.InsertAtEnd("spolar");
	m_AlphaList.InsertAtEnd("sporrar");
	m_AlphaList.InsertAtEnd("sport");
	m_AlphaList.InsertAtEnd("sporthndelse");
	m_AlphaList.InsertAtEnd("sportjacka");
	m_AlphaList.InsertAtEnd("sportklder");
	m_AlphaList.InsertAtEnd("sportprogram");
	m_AlphaList.InsertAtEnd("sportvska");
	m_AlphaList.InsertAtEnd("spotta");
	m_AlphaList.InsertAtEnd("spottar");
	m_AlphaList.InsertAtEnd("spottskl");
	m_AlphaList.InsertAtEnd("spraya");
	m_AlphaList.InsertAtEnd("sprayar");
	m_AlphaList.InsertAtEnd("sprayflaska");
	m_AlphaList.InsertAtEnd("sprida");
	m_AlphaList.InsertAtEnd("sprider");
	m_AlphaList.InsertAtEnd("springa");
	m_AlphaList.InsertAtEnd("springer");
	m_AlphaList.InsertAtEnd("sprit");
	m_AlphaList.InsertAtEnd("sprits");
	m_AlphaList.InsertAtEnd("spruta");
	m_AlphaList.InsertAtEnd("sprutar");
	m_AlphaList.InsertAtEnd("sprk");
	m_AlphaList.InsertAtEnd("sprklektion");
	m_AlphaList.InsertAtEnd("sprngmne");
	m_AlphaList.InsertAtEnd("spr");
	m_AlphaList.InsertAtEnd("spra");
	m_AlphaList.InsertAtEnd("sprar");
	m_AlphaList.InsertAtEnd("sprvagn");
	m_AlphaList.InsertAtEnd("sprvagnskort");
	m_AlphaList.InsertAtEnd("spdgris");
	m_AlphaList.InsertAtEnd("spnd");
	m_AlphaList.InsertAtEnd("spnna");
	m_AlphaList.InsertAtEnd("spnnande");
	m_AlphaList.InsertAtEnd("spnne");
	m_AlphaList.InsertAtEnd("spnner");
	m_AlphaList.InsertAtEnd("sprr");
	m_AlphaList.InsertAtEnd("sp");
	m_AlphaList.InsertAtEnd("spke");
	m_AlphaList.InsertAtEnd("spkhus");
	m_AlphaList.InsertAtEnd("squash");
	m_AlphaList.InsertAtEnd("stad");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("staden");
	m_AlphaList.InsertAtEnd("stadium");
	m_AlphaList.InsertAtEnd("stadshus");
	m_AlphaList.InsertAtEnd("staffli");
	m_AlphaList.InsertAtEnd("staka");
	m_AlphaList.InsertAtEnd("stakar");
	m_AlphaList.InsertAtEnd("staket");
	m_AlphaList.InsertAtEnd("stall");
	m_AlphaList.InsertAtEnd("stallet ");
	m_AlphaList.InsertAtEnd("stam");
	m_AlphaList.InsertAtEnd("stampa");
	m_AlphaList.InsertAtEnd("stampar");
	m_AlphaList.InsertAtEnd("stanna");
	m_AlphaList.InsertAtEnd("stannar");
	m_AlphaList.InsertAtEnd("stapla");
	m_AlphaList.InsertAtEnd("staplar");
	m_AlphaList.InsertAtEnd("stark");
	m_AlphaList.InsertAtEnd("starkt");
	m_AlphaList.InsertAtEnd("start");
	m_AlphaList.InsertAtEnd("starta");
	m_AlphaList.InsertAtEnd("startar");
	m_AlphaList.InsertAtEnd("startramp");
	m_AlphaList.InsertAtEnd("startsignal");
	m_AlphaList.InsertAtEnd("staten");
	m_AlphaList.InsertAtEnd("statens");
	m_AlphaList.InsertAtEnd("statliga");
	m_AlphaList.InsertAtEnd("status");
	m_AlphaList.InsertAtEnd("staty");
	m_AlphaList.InsertAtEnd("stava");
	m_AlphaList.InsertAtEnd("stavar");
	m_AlphaList.InsertAtEnd("stearinljus");
	m_AlphaList.InsertAtEnd("steg");
	m_AlphaList.InsertAtEnd("stege");
	m_AlphaList.InsertAtEnd("steka");
	m_AlphaList.InsertAtEnd("steker");
	m_AlphaList.InsertAtEnd("steknl");
	m_AlphaList.InsertAtEnd("stekpanna");
	m_AlphaList.InsertAtEnd("stekspade");
	m_AlphaList.InsertAtEnd("stekspett");
	m_AlphaList.InsertAtEnd("stekt");
	m_AlphaList.InsertAtEnd("sten");
	m_AlphaList.InsertAtEnd("stepp");
	m_AlphaList.InsertAtEnd("stereo");
	m_AlphaList.InsertAtEnd("sterilisering");
	m_AlphaList.InsertAtEnd("stetoskop");
	m_AlphaList.InsertAtEnd("stick");
	m_AlphaList.InsertAtEnd("sticka");
	m_AlphaList.InsertAtEnd("stickar");
	m_AlphaList.InsertAtEnd("sticker");
	m_AlphaList.InsertAtEnd("stickning");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("stickor");
	m_AlphaList.InsertAtEnd("sticksg");
	m_AlphaList.InsertAtEnd("stift");
	m_AlphaList.InsertAtEnd("stig");
	m_AlphaList.InsertAtEnd("stiga");
	m_AlphaList.InsertAtEnd("stilla");
	m_AlphaList.InsertAtEnd("stirrade");
	m_AlphaList.InsertAtEnd("stjl");
	m_AlphaList.InsertAtEnd("stjla");
	m_AlphaList.InsertAtEnd("stjlk");
	m_AlphaList.InsertAtEnd("stjrna");
	m_AlphaList.InsertAtEnd("stjrnbild");
	m_AlphaList.InsertAtEnd("stjrnfall");
	m_AlphaList.InsertAtEnd("stjrt");
	m_AlphaList.InsertAtEnd("stock");
	m_AlphaList.InsertAtEnd("stockrnning");
	m_AlphaList.InsertAtEnd("stod");
	m_AlphaList.InsertAtEnd("stol");
	m_AlphaList.InsertAtEnd("stolar");
	m_AlphaList.InsertAtEnd("stolt");
	m_AlphaList.InsertAtEnd("stopp");
	m_AlphaList.InsertAtEnd("stoppa");
	m_AlphaList.InsertAtEnd("stoppar");
	m_AlphaList.InsertAtEnd("stoppur");
	m_AlphaList.InsertAtEnd("stor");
	m_AlphaList.InsertAtEnd("stora");
	m_AlphaList.InsertAtEnd("stork");
	m_AlphaList.InsertAtEnd("storlek");
	m_AlphaList.InsertAtEnd("storm");
	m_AlphaList.InsertAtEnd("storma ");
	m_AlphaList.InsertAtEnd("stormar");
	m_AlphaList.InsertAtEnd("stormarknad");
	m_AlphaList.InsertAtEnd("stormig");
	m_AlphaList.InsertAtEnd("stort");
	m_AlphaList.InsertAtEnd("straff");
	m_AlphaList.InsertAtEnd("straffa");
	m_AlphaList.InsertAtEnd("straffar");
	m_AlphaList.InsertAtEnd("strand");
	m_AlphaList.InsertAtEnd("strandpromenad");
	m_AlphaList.InsertAtEnd("strandstol");
	m_AlphaList.InsertAtEnd("strandtennis");
	m_AlphaList.InsertAtEnd("strax");
	m_AlphaList.InsertAtEnd("streck");
	m_AlphaList.InsertAtEnd("streckkod");
	m_AlphaList.InsertAtEnd("stridsvagn");
	m_AlphaList.InsertAtEnd("struktur");
	m_AlphaList.InsertAtEnd("strumpbyxor");
	m_AlphaList.InsertAtEnd("strumpor");
	m_AlphaList.InsertAtEnd("strumporna");
	m_AlphaList.InsertAtEnd("struntprat");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("strupe");
	m_AlphaList.InsertAtEnd("struts");
	m_AlphaList.InsertAtEnd("stryka");
	m_AlphaList.InsertAtEnd("strykbrda");
	m_AlphaList.InsertAtEnd("stryker");
	m_AlphaList.InsertAtEnd("strykjrn");
	m_AlphaList.InsertAtEnd("str");
	m_AlphaList.InsertAtEnd("strhatt");
	m_AlphaList.InsertAtEnd("strke");
	m_AlphaList.InsertAtEnd("strle");
	m_AlphaList.InsertAtEnd("strlkastare");
	m_AlphaList.InsertAtEnd("strlning");
	m_AlphaList.InsertAtEnd("strcka ");
	m_AlphaList.InsertAtEnd("strcker");
	m_AlphaList.InsertAtEnd("str");
	m_AlphaList.InsertAtEnd("strmbrytare");
	m_AlphaList.InsertAtEnd("strr");
	m_AlphaList.InsertAtEnd("strsocker");
	m_AlphaList.InsertAtEnd("strssel");
	m_AlphaList.InsertAtEnd("strva");
	m_AlphaList.InsertAtEnd("studiecirkel");
	m_AlphaList.InsertAtEnd("studio");
	m_AlphaList.InsertAtEnd("studsa");
	m_AlphaList.InsertAtEnd("studsar");
	m_AlphaList.InsertAtEnd("studsmadrass");
	m_AlphaList.InsertAtEnd("studsmatta");
	m_AlphaList.InsertAtEnd("stuga");
	m_AlphaList.InsertAtEnd("stukning");
	m_AlphaList.InsertAtEnd("stum");
	m_AlphaList.InsertAtEnd("stund");
	m_AlphaList.InsertAtEnd("stup");
	m_AlphaList.InsertAtEnd("stuprr");
	m_AlphaList.InsertAtEnd("stuvning");
	m_AlphaList.InsertAtEnd("stycke");
	m_AlphaList.InsertAtEnd("stycket");
	m_AlphaList.InsertAtEnd("stygn");
	m_AlphaList.InsertAtEnd("styre");
	m_AlphaList.InsertAtEnd("styrka");
	m_AlphaList.InsertAtEnd("styrkula");
	m_AlphaList.InsertAtEnd("styrplatta");
	m_AlphaList.InsertAtEnd("styrspak");
	m_AlphaList.InsertAtEnd("styvfar");
	m_AlphaList.InsertAtEnd("styvmor");
	m_AlphaList.InsertAtEnd("st");
	m_AlphaList.InsertAtEnd("stl");
	m_AlphaList.InsertAtEnd("stnd");
	m_AlphaList.InsertAtEnd("str");
	m_AlphaList.InsertAtEnd("ststd");
	m_AlphaList.InsertAtEnd("std");
	m_AlphaList.InsertAtEnd("stda");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("stdar");
	m_AlphaList.InsertAtEnd("stdare");
	m_AlphaList.InsertAtEnd("stdpatrull");
	m_AlphaList.InsertAtEnd("stdredskap");
	m_AlphaList.InsertAtEnd("stdvagn");
	m_AlphaList.InsertAtEnd("stlla");
	m_AlphaList.InsertAtEnd("stllde");
	m_AlphaList.InsertAtEnd("stlle");
	m_AlphaList.InsertAtEnd("stller");
	m_AlphaList.InsertAtEnd("stllet");
	m_AlphaList.InsertAtEnd("stllning");
	m_AlphaList.InsertAtEnd("stmjrn");
	m_AlphaList.InsertAtEnd("stmpel");
	m_AlphaList.InsertAtEnd("stmpeldyna");
	m_AlphaList.InsertAtEnd("stmpelklocka");
	m_AlphaList.InsertAtEnd("stmpla");
	m_AlphaList.InsertAtEnd("stmplar");
	m_AlphaList.InsertAtEnd("stnga");
	m_AlphaList.InsertAtEnd("stngd");
	m_AlphaList.InsertAtEnd("stnger");
	m_AlphaList.InsertAtEnd("std");
	m_AlphaList.InsertAtEnd("stder");
	m_AlphaList.InsertAtEnd("stdja");
	m_AlphaList.InsertAtEnd("stdjer");
	m_AlphaList.InsertAtEnd("stdkudde");
	m_AlphaList.InsertAtEnd("stdstol");
	m_AlphaList.InsertAtEnd("str");
	m_AlphaList.InsertAtEnd("stra");
	m_AlphaList.InsertAtEnd("strre");
	m_AlphaList.InsertAtEnd("strst");
	m_AlphaList.InsertAtEnd("strsta");
	m_AlphaList.InsertAtEnd("strta");
	m_AlphaList.InsertAtEnd("strtar");
	m_AlphaList.InsertAtEnd("stt");
	m_AlphaList.InsertAtEnd("sttta");
	m_AlphaList.InsertAtEnd("stttar");
	m_AlphaList.InsertAtEnd("stvlar");
	m_AlphaList.InsertAtEnd("subtraktion");
	m_AlphaList.InsertAtEnd("suga");
	m_AlphaList.InsertAtEnd("suger");
	m_AlphaList.InsertAtEnd("sugrr");
	m_AlphaList.InsertAtEnd("sulky");
	m_AlphaList.InsertAtEnd("sup");
	m_AlphaList.InsertAtEnd("sur");
	m_AlphaList.InsertAtEnd("surfa");
	m_AlphaList.InsertAtEnd("surfar");
	m_AlphaList.InsertAtEnd("surfbrda");
	m_AlphaList.InsertAtEnd("surkl");
	m_AlphaList.InsertAtEnd("surra");
	m_AlphaList.InsertAtEnd("surrar");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("svag");
	m_AlphaList.InsertAtEnd("svagare");
	m_AlphaList.InsertAtEnd("svala");
	m_AlphaList.InsertAtEnd("svamp");
	m_AlphaList.InsertAtEnd("svan");
	m_AlphaList.InsertAtEnd("svans");
	m_AlphaList.InsertAtEnd("svar");
	m_AlphaList.InsertAtEnd("svara");
	m_AlphaList.InsertAtEnd("svarar");
	m_AlphaList.InsertAtEnd("svart");
	m_AlphaList.InsertAtEnd("svarta");
	m_AlphaList.InsertAtEnd("svartsjuk");
	m_AlphaList.InsertAtEnd("svarv");
	m_AlphaList.InsertAtEnd("svensk");
	m_AlphaList.InsertAtEnd("Sverige");
	m_AlphaList.InsertAtEnd("svett");
	m_AlphaList.InsertAtEnd("svettas");
	m_AlphaList.InsertAtEnd("svika");
	m_AlphaList.InsertAtEnd("sviker");
	m_AlphaList.InsertAtEnd("svimma");
	m_AlphaList.InsertAtEnd("swimmingpool");
	m_AlphaList.InsertAtEnd("svinstia");
	m_AlphaList.InsertAtEnd("svngrem");
	m_AlphaList.InsertAtEnd("svr");
	m_AlphaList.InsertAtEnd("svrt");
	m_AlphaList.InsertAtEnd("svlja");
	m_AlphaList.InsertAtEnd("svljer");
	m_AlphaList.InsertAtEnd("svlla ");
	m_AlphaList.InsertAtEnd("svller");
	m_AlphaList.InsertAtEnd("svnga");
	m_AlphaList.InsertAtEnd("svnger");
	m_AlphaList.InsertAtEnd("svra");
	m_AlphaList.InsertAtEnd("svrd");
	m_AlphaList.InsertAtEnd("svvare");
	m_AlphaList.InsertAtEnd("sydlig");
	m_AlphaList.InsertAtEnd("syl");
	m_AlphaList.InsertAtEnd("sylt");
	m_AlphaList.InsertAtEnd("symaskin");
	m_AlphaList.InsertAtEnd("symbol");
	m_AlphaList.InsertAtEnd("symboler");
	m_AlphaList.InsertAtEnd("symbolkarta");
	m_AlphaList.InsertAtEnd("symbolsprk");
	m_AlphaList.InsertAtEnd("synagoga");
	m_AlphaList.InsertAtEnd("synas");
	m_AlphaList.InsertAtEnd("synd");
	m_AlphaList.InsertAtEnd("synda");
	m_AlphaList.InsertAtEnd("syndare");
	m_AlphaList.InsertAtEnd("synpunkt");
	m_AlphaList.InsertAtEnd("synskadad");
	m_AlphaList.InsertAtEnd("syntest");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("syntesttavla");
	m_AlphaList.InsertAtEnd("synthesizer");
	m_AlphaList.InsertAtEnd("synl");
	m_AlphaList.InsertAtEnd("syr");
	m_AlphaList.InsertAtEnd("syra");
	m_AlphaList.InsertAtEnd("syre");
	m_AlphaList.InsertAtEnd("syrgas");
	m_AlphaList.InsertAtEnd("syrum");
	m_AlphaList.InsertAtEnd("syskon");
	m_AlphaList.InsertAtEnd("syster");
	m_AlphaList.InsertAtEnd("systerdotter");
	m_AlphaList.InsertAtEnd("systerson");
	m_AlphaList.InsertAtEnd("sytrd");
	m_AlphaList.InsertAtEnd("s");
	m_AlphaList.InsertAtEnd("sg");
	m_AlphaList.InsertAtEnd("sga");
	m_AlphaList.InsertAtEnd("sgar");
	m_AlphaList.InsertAtEnd("sng");
	m_AlphaList.InsertAtEnd("sngare");
	m_AlphaList.InsertAtEnd("sngfgel");
	m_AlphaList.InsertAtEnd("spbubblor");
	m_AlphaList.InsertAtEnd("spopera");
	m_AlphaList.InsertAtEnd("sr");
	m_AlphaList.InsertAtEnd("sra");
	m_AlphaList.InsertAtEnd("srad");
	m_AlphaList.InsertAtEnd("srar");
	m_AlphaList.InsertAtEnd("srskorpa");
	m_AlphaList.InsertAtEnd("ss");
	m_AlphaList.InsertAtEnd("ssskl");
	m_AlphaList.InsertAtEnd("sck");
	m_AlphaList.InsertAtEnd("sckpipblsare");
	m_AlphaList.InsertAtEnd("sd");
	m_AlphaList.InsertAtEnd("sga");
	m_AlphaList.InsertAtEnd("sger");
	m_AlphaList.InsertAtEnd("sker");
	m_AlphaList.InsertAtEnd("skerhet");
	m_AlphaList.InsertAtEnd("skerhetsblte");
	m_AlphaList.InsertAtEnd("skerhetsglasgon");
	m_AlphaList.InsertAtEnd("skerhetsnl");
	m_AlphaList.InsertAtEnd("skerhetsnt");
	m_AlphaList.InsertAtEnd("skert");
	m_AlphaList.InsertAtEnd("sl");
	m_AlphaList.InsertAtEnd("slja");
	m_AlphaList.InsertAtEnd("sljer ");
	m_AlphaList.InsertAtEnd("sllskapsdjur");
	m_AlphaList.InsertAtEnd("sllskapsspel");
	m_AlphaList.InsertAtEnd("smskskinn");
	m_AlphaList.InsertAtEnd("sng");
	m_AlphaList.InsertAtEnd("sngbord");
	m_AlphaList.InsertAtEnd("sngdags");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("sngen");
	m_AlphaList.InsertAtEnd("sngklder");
	m_AlphaList.InsertAtEnd("sngvrmare");
	m_AlphaList.InsertAtEnd("snka");
	m_AlphaList.InsertAtEnd("snker");
	m_AlphaList.InsertAtEnd("srskild");
	m_AlphaList.InsertAtEnd("srskilda");
	m_AlphaList.InsertAtEnd("srskilja");
	m_AlphaList.InsertAtEnd("srskilt");
	m_AlphaList.InsertAtEnd("stt");
	m_AlphaList.InsertAtEnd("stt ");
	m_AlphaList.InsertAtEnd("stta");
	m_AlphaList.InsertAtEnd("stter");
	m_AlphaList.InsertAtEnd("sttet");
	m_AlphaList.InsertAtEnd("sder");
	m_AlphaList.InsertAtEnd("sdra ");
	m_AlphaList.InsertAtEnd("ska");
	m_AlphaList.InsertAtEnd("sker");
	m_AlphaList.InsertAtEnd("smn");
	m_AlphaList.InsertAtEnd("sndag");
	m_AlphaList.InsertAtEnd("snder");
	m_AlphaList.InsertAtEnd("srja");
	m_AlphaList.InsertAtEnd("srjer");
	m_AlphaList.InsertAtEnd("st");
	m_AlphaList.InsertAtEnd("stpotatis");
	m_AlphaList.InsertAtEnd("stsak");
	m_AlphaList.InsertAtEnd("ta");
	m_AlphaList.InsertAtEnd("tablett");
	m_AlphaList.InsertAtEnd("tack");
	m_AlphaList.InsertAtEnd("tack ");
	m_AlphaList.InsertAtEnd("tacka");
	m_AlphaList.InsertAtEnd("tackar");
	m_AlphaList.InsertAtEnd("tacksgelsedagen");
	m_AlphaList.InsertAtEnd("tag");
	m_AlphaList.InsertAtEnd("taget");
	m_AlphaList.InsertAtEnd("taggar");
	m_AlphaList.InsertAtEnd("tagit");
	m_AlphaList.InsertAtEnd("tak");
	m_AlphaList.InsertAtEnd("takflkt");
	m_AlphaList.InsertAtEnd("tal");
	m_AlphaList.InsertAtEnd("tala");
	m_AlphaList.InsertAtEnd("talade");
	m_AlphaList.InsertAtEnd("talar");
	m_AlphaList.InsertAtEnd("talare");
	m_AlphaList.InsertAtEnd("talbok");
	m_AlphaList.InsertAtEnd("talet");
	m_AlphaList.InsertAtEnd("talja");
	m_AlphaList.InsertAtEnd("talkpuder");
	m_AlphaList.InsertAtEnd("tall");
	m_AlphaList.InsertAtEnd("tallkotte");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("tallrik");
	m_AlphaList.InsertAtEnd("tallriken");
	m_AlphaList.InsertAtEnd("tallriksskydd");
	m_AlphaList.InsertAtEnd("talpedagog");
	m_AlphaList.InsertAtEnd("talskadad");
	m_AlphaList.InsertAtEnd("talsyntes");
	m_AlphaList.InsertAtEnd("tamburin");
	m_AlphaList.InsertAtEnd("tampong");
	m_AlphaList.InsertAtEnd("tand");
	m_AlphaList.InsertAtEnd("tandborste");
	m_AlphaList.InsertAtEnd("tandborsten");
	m_AlphaList.InsertAtEnd("tandem");
	m_AlphaList.InsertAtEnd("tandens");
	m_AlphaList.InsertAtEnd("tandhygienist");
	m_AlphaList.InsertAtEnd("tandkrm");
	m_AlphaList.InsertAtEnd("tandlkare");
	m_AlphaList.InsertAtEnd("tandlkarstol");
	m_AlphaList.InsertAtEnd("tandpetare");
	m_AlphaList.InsertAtEnd("tandstllning");
	m_AlphaList.InsertAtEnd("tandtrd");
	m_AlphaList.InsertAtEnd("tandvrk");
	m_AlphaList.InsertAtEnd("tangent");
	m_AlphaList.InsertAtEnd("tangentbord");
	m_AlphaList.InsertAtEnd("tank");
	m_AlphaList.InsertAtEnd("tankbil");
	m_AlphaList.InsertAtEnd("tanke");
	m_AlphaList.InsertAtEnd("tanken");
	m_AlphaList.InsertAtEnd("tape");
	m_AlphaList.InsertAtEnd("tapet");
	m_AlphaList.InsertAtEnd("tapetsera");
	m_AlphaList.InsertAtEnd("tapetserar");
	m_AlphaList.InsertAtEnd("tappa");
	m_AlphaList.InsertAtEnd("tappar");
	m_AlphaList.InsertAtEnd("tar");
	m_AlphaList.InsertAtEnd("tarm");
	m_AlphaList.InsertAtEnd("tartarss");
	m_AlphaList.InsertAtEnd("tas");
	m_AlphaList.InsertAtEnd("tass");
	m_AlphaList.InsertAtEnd("tatuering");
	m_AlphaList.InsertAtEnd("tavla");
	m_AlphaList.InsertAtEnd("taxi");
	m_AlphaList.InsertAtEnd("taxifrare");
	m_AlphaList.InsertAtEnd("taxistation");
	m_AlphaList.InsertAtEnd("teater");
	m_AlphaList.InsertAtEnd("tecken");
	m_AlphaList.InsertAtEnd("teckenbokstavering");
	m_AlphaList.InsertAtEnd("teckensprk");
	m_AlphaList.InsertAtEnd("teckna");
	m_AlphaList.InsertAtEnd("tecknar");
	m_AlphaList.InsertAtEnd("teckning");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("teckningslektion");
	m_AlphaList.InsertAtEnd("teckningstvling");
	m_AlphaList.InsertAtEnd("teddybjrn");
	m_AlphaList.InsertAtEnd("tegelpanna");
	m_AlphaList.InsertAtEnd("tegelsten");
	m_AlphaList.InsertAtEnd("tekanna");
	m_AlphaList.InsertAtEnd("telefon");
	m_AlphaList.InsertAtEnd("telefonbok");
	m_AlphaList.InsertAtEnd("telefonhytt");
	m_AlphaList.InsertAtEnd("telefonnummer");
	m_AlphaList.InsertAtEnd("telefonstolpe");
	m_AlphaList.InsertAtEnd("telefonsvarare");
	m_AlphaList.InsertAtEnd("telefonvxel");
	m_AlphaList.InsertAtEnd("teleskop");
	m_AlphaList.InsertAtEnd("teleslinga");
	m_AlphaList.InsertAtEnd("television");
	m_AlphaList.InsertAtEnd("temperatur");
	m_AlphaList.InsertAtEnd("temperaturen");
	m_AlphaList.InsertAtEnd("tennis");
	m_AlphaList.InsertAtEnd("tennisboll");
	m_AlphaList.InsertAtEnd("tennismatch");
	m_AlphaList.InsertAtEnd("tepse");
	m_AlphaList.InsertAtEnd("terapeut");
	m_AlphaList.InsertAtEnd("terapi");
	m_AlphaList.InsertAtEnd("termometer");
	m_AlphaList.InsertAtEnd("termos");
	m_AlphaList.InsertAtEnd("termostat");
	m_AlphaList.InsertAtEnd("terrass");
	m_AlphaList.InsertAtEnd("terrorisera");
	m_AlphaList.InsertAtEnd("terroriserar");
	m_AlphaList.InsertAtEnd("terrorist");
	m_AlphaList.InsertAtEnd("terrngfordon");
	m_AlphaList.InsertAtEnd("test");
	m_AlphaList.InsertAtEnd("text");
	m_AlphaList.InsertAtEnd("tid");
	m_AlphaList.InsertAtEnd("tiden");
	m_AlphaList.InsertAtEnd("tidig");
	m_AlphaList.InsertAtEnd("tidigare");
	m_AlphaList.InsertAtEnd("tidigt");
	m_AlphaList.InsertAtEnd("tidkort");
	m_AlphaList.InsertAtEnd("tidning");
	m_AlphaList.InsertAtEnd("tidningskiosk");
	m_AlphaList.InsertAtEnd("tidtagarur");
	m_AlphaList.InsertAtEnd("tiger");
	m_AlphaList.InsertAtEnd("tiggare");
	m_AlphaList.InsertAtEnd("till");
	m_AlphaList.InsertAtEnd("tillbaka");
	m_AlphaList.InsertAtEnd("tillbringare");
	m_AlphaList.InsertAtEnd("tillfredsstllelse");
	m_AlphaList.InsertAtEnd("tillfllighet");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("tillhr");
	m_AlphaList.InsertAtEnd("tillhrighet");
	m_AlphaList.InsertAtEnd("tillrckligt");
	m_AlphaList.InsertAtEnd("tills");
	m_AlphaList.InsertAtEnd("tillsammans");
	m_AlphaList.InsertAtEnd("tilltrde");
	m_AlphaList.InsertAtEnd("tillverka");
	m_AlphaList.InsertAtEnd("tillverkad");
	m_AlphaList.InsertAtEnd("tillverkar");
	m_AlphaList.InsertAtEnd("tillverkare");
	m_AlphaList.InsertAtEnd("tillta");
	m_AlphaList.InsertAtEnd("tilltelse");
	m_AlphaList.InsertAtEnd("tillter");
	m_AlphaList.InsertAtEnd("tillgg");
	m_AlphaList.InsertAtEnd("timglas");
	m_AlphaList.InsertAtEnd("timmar");
	m_AlphaList.InsertAtEnd("timme");
	m_AlphaList.InsertAtEnd("tina");
	m_AlphaList.InsertAtEnd("tinar");
	m_AlphaList.InsertAtEnd("tio");
	m_AlphaList.InsertAtEnd("tippbrda");
	m_AlphaList.InsertAtEnd("tisdag");
	m_AlphaList.InsertAtEnd("titta");
	m_AlphaList.InsertAtEnd("tittade");
	m_AlphaList.InsertAtEnd("tittar");
	m_AlphaList.InsertAtEnd("tivoli");
	m_AlphaList.InsertAtEnd("tjata");
	m_AlphaList.InsertAtEnd("tjatar");
	m_AlphaList.InsertAtEnd("tjock");
	m_AlphaList.InsertAtEnd("tjugoen");
	m_AlphaList.InsertAtEnd("tjugofem");
	m_AlphaList.InsertAtEnd("tjugofyra");
	m_AlphaList.InsertAtEnd("tjugonio");
	m_AlphaList.InsertAtEnd("tjugosex");
	m_AlphaList.InsertAtEnd("tjugosju");
	m_AlphaList.InsertAtEnd("tjugotre");
	m_AlphaList.InsertAtEnd("tjugotv");
	m_AlphaList.InsertAtEnd("tjugotta");
	m_AlphaList.InsertAtEnd("tjur");
	m_AlphaList.InsertAtEnd("tjuv");
	m_AlphaList.InsertAtEnd("tjna");
	m_AlphaList.InsertAtEnd("toalett");
	m_AlphaList.InsertAtEnd("toalettbord");
	m_AlphaList.InsertAtEnd("toaletten");
	m_AlphaList.InsertAtEnd("toalettlock");
	m_AlphaList.InsertAtEnd("toalettpapper");
	m_AlphaList.InsertAtEnd("toalettring");
	m_AlphaList.InsertAtEnd("toalettrulle");
	m_AlphaList.InsertAtEnd("toalettstol");
	m_AlphaList.InsertAtEnd("toalettstolen");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("tobak");
	m_AlphaList.InsertAtEnd("toffeldjur");
	m_AlphaList.InsertAtEnd("tofflor");
	m_AlphaList.InsertAtEnd("tofsar");
	m_AlphaList.InsertAtEnd("tog");
	m_AlphaList.InsertAtEnd("tom");
	m_AlphaList.InsertAtEnd("tomat");
	m_AlphaList.InsertAtEnd("tomatjuice");
	m_AlphaList.InsertAtEnd("tomatpur");
	m_AlphaList.InsertAtEnd("tomte");
	m_AlphaList.InsertAtEnd("tonfisk");
	m_AlphaList.InsertAtEnd("tonring");
	m_AlphaList.InsertAtEnd("toppen");
	m_AlphaList.InsertAtEnd("toppluva");
	m_AlphaList.InsertAtEnd("tops");
	m_AlphaList.InsertAtEnd("torg");
	m_AlphaList.InsertAtEnd("torka");
	m_AlphaList.InsertAtEnd("torkad");
	m_AlphaList.InsertAtEnd("torkar");
	m_AlphaList.InsertAtEnd("torkmaskin");
	m_AlphaList.InsertAtEnd("torkstllning");
	m_AlphaList.InsertAtEnd("torktumlare");
	m_AlphaList.InsertAtEnd("torn");
	m_AlphaList.InsertAtEnd("tornado");
	m_AlphaList.InsertAtEnd("tornera");
	m_AlphaList.InsertAtEnd("tornerar");
	m_AlphaList.InsertAtEnd("tornspira");
	m_AlphaList.InsertAtEnd("torr");
	m_AlphaList.InsertAtEnd("torsdag");
	m_AlphaList.InsertAtEnd("torsk");
	m_AlphaList.InsertAtEnd("trafik");
	m_AlphaList.InsertAtEnd("trafikk");
	m_AlphaList.InsertAtEnd("trafikljus");
	m_AlphaList.InsertAtEnd("trafikolycka");
	m_AlphaList.InsertAtEnd("trafikskylt");
	m_AlphaList.InsertAtEnd("traktor");
	m_AlphaList.InsertAtEnd("trampa");
	m_AlphaList.InsertAtEnd("trampar");
	m_AlphaList.InsertAtEnd("trampbt");
	m_AlphaList.InsertAtEnd("trampkvarn");
	m_AlphaList.InsertAtEnd("trampolin");
	m_AlphaList.InsertAtEnd("tranbr");
	m_AlphaList.InsertAtEnd("transportband");
	m_AlphaList.InsertAtEnd("transportmedel");
	m_AlphaList.InsertAtEnd("trapets");
	m_AlphaList.InsertAtEnd("trappa");
	m_AlphaList.InsertAtEnd("trapprcke");
	m_AlphaList.InsertAtEnd("trasig");
	m_AlphaList.InsertAtEnd("tratt");
	m_AlphaList.InsertAtEnd("trav");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("trava");
	m_AlphaList.InsertAtEnd("travar");
	m_AlphaList.InsertAtEnd("tre");
	m_AlphaList.InsertAtEnd("trea");
	m_AlphaList.InsertAtEnd("tredje");
	m_AlphaList.InsertAtEnd("tredjedel");
	m_AlphaList.InsertAtEnd("trehjuling");
	m_AlphaList.InsertAtEnd("trekantig");
	m_AlphaList.InsertAtEnd("trettondagen");
	m_AlphaList.InsertAtEnd("trevlig");
	m_AlphaList.InsertAtEnd("trevligt");
	m_AlphaList.InsertAtEnd("triangel");
	m_AlphaList.InsertAtEnd("tricks");
	m_AlphaList.InsertAtEnd("trimmer");
	m_AlphaList.InsertAtEnd("tro");
	m_AlphaList.InsertAtEnd("trodde");
	m_AlphaList.InsertAtEnd("trof");
	m_AlphaList.InsertAtEnd("trohet");
	m_AlphaList.InsertAtEnd("trolig");
	m_AlphaList.InsertAtEnd("troll");
	m_AlphaList.InsertAtEnd("trolla");
	m_AlphaList.InsertAtEnd("trollar");
	m_AlphaList.InsertAtEnd("trolldom");
	m_AlphaList.InsertAtEnd("trollkarl");
	m_AlphaList.InsertAtEnd("trollkonstnr");
	m_AlphaList.InsertAtEnd("trollslnda");
	m_AlphaList.InsertAtEnd("trollsp");
	m_AlphaList.InsertAtEnd("trombon");
	m_AlphaList.InsertAtEnd("tropisk");
	m_AlphaList.InsertAtEnd("tror");
	m_AlphaList.InsertAtEnd("trosa");
	m_AlphaList.InsertAtEnd("trosskydd");
	m_AlphaList.InsertAtEnd("trots");
	m_AlphaList.InsertAtEnd("trottoar");
	m_AlphaList.InsertAtEnd("trottoarkant");
	m_AlphaList.InsertAtEnd("trumma");
	m_AlphaList.InsertAtEnd("trumpet");
	m_AlphaList.InsertAtEnd("trumpinnar");
	m_AlphaList.InsertAtEnd("trumpinne");
	m_AlphaList.InsertAtEnd("trumset");
	m_AlphaList.InsertAtEnd("trumslagare");
	m_AlphaList.InsertAtEnd("trut ");
	m_AlphaList.InsertAtEnd("trycka");
	m_AlphaList.InsertAtEnd("trycker");
	m_AlphaList.InsertAtEnd("tryckeri");
	m_AlphaList.InsertAtEnd("tryckknapp");
	m_AlphaList.InsertAtEnd("tryckpress");
	m_AlphaList.InsertAtEnd("trygg");
	m_AlphaList.InsertAtEnd("trygghet");
	m_AlphaList.InsertAtEnd("tryne");pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("trckla");
	m_AlphaList.InsertAtEnd("trcklar");
	m_AlphaList.InsertAtEnd("trd");
	m_AlphaList.InsertAtEnd("trkig");
	m_AlphaList.InsertAtEnd("trnande");
	m_AlphaList.InsertAtEnd("trng");
	m_AlphaList.InsertAtEnd("tr");
	m_AlphaList.InsertAtEnd("trd");
	m_AlphaList.InsertAtEnd("trdgrd");
	m_AlphaList.InsertAtEnd("trdgrdsarbete");
	m_AlphaList.InsertAtEnd("trdgrdsbord");
	m_AlphaList.InsertAtEnd("trdgrdsmstare");
	m_AlphaList.InsertAtEnd("trdgrdsmbel");
	m_AlphaList.InsertAtEnd("trdgrdsredskap");
	m_AlphaList.InsertAtEnd("trdgrdsstol");
	m_AlphaList.InsertAtEnd("trdkoja");
	m_AlphaList.InsertAtEnd("trff");
	m_AlphaList.InsertAtEnd("trffa");
	m_AlphaList.InsertAtEnd("trffar");
	m_AlphaList.InsertAtEnd("trffas");
	m_AlphaList.InsertAtEnd("trkol");
	m_AlphaList.InsertAtEnd("trna");
	m_AlphaList.InsertAtEnd("trnar");
	m_AlphaList.InsertAtEnd("trningsoverall");
	m_AlphaList.InsertAtEnd("trns");
	m_AlphaList.InsertAtEnd("trr");
	m_AlphaList.InsertAtEnd("trsko");
	m_AlphaList.InsertAtEnd("trsljd");
	m_AlphaList.InsertAtEnd("trsvarv");
	m_AlphaList.InsertAtEnd("trja");
	m_AlphaList.InsertAtEnd("trsta");
	m_AlphaList.InsertAtEnd("trstar ");
	m_AlphaList.InsertAtEnd("trtt");
	m_AlphaList.InsertAtEnd("t-shirt");
	m_AlphaList.InsertAtEnd("tub");
	m_AlphaList.InsertAtEnd("tugga");
	m_AlphaList.InsertAtEnd("tuggummi");
	m_AlphaList.InsertAtEnd("tulpan");
	m_AlphaList.InsertAtEnd("tum");
	m_AlphaList.InsertAtEnd("tummar");
	m_AlphaList.InsertAtEnd("tumme");
	m_AlphaList.InsertAtEnd("tummen");
	m_AlphaList.InsertAtEnd("tumstock");
	m_AlphaList.InsertAtEnd("tung");
	m_AlphaList.InsertAtEnd("tunga");
	m_AlphaList.InsertAtEnd("tungan");
	m_AlphaList.InsertAtEnd("tunn");
	m_AlphaList.InsertAtEnd("tunna");
	m_AlphaList.InsertAtEnd("tunnbrd");
	m_AlphaList.InsertAtEnd("tunnel");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("tunnelbana");
	m_AlphaList.InsertAtEnd("tup");
	m_AlphaList.InsertAtEnd("tupp");
	m_AlphaList.InsertAtEnd("tur");
	m_AlphaList.InsertAtEnd("tursam");
	m_AlphaList.InsertAtEnd("tusenfoting");
	m_AlphaList.InsertAtEnd("tv");
	m_AlphaList.InsertAtEnd("tveka");
	m_AlphaList.InsertAtEnd("tvekar");
	m_AlphaList.InsertAtEnd("TV-hylla");
	m_AlphaList.InsertAtEnd("tvillingar");
	m_AlphaList.InsertAtEnd("tving");
	m_AlphaList.InsertAtEnd("tvingas");
	m_AlphaList.InsertAtEnd("tvivla");
	m_AlphaList.InsertAtEnd("tvivlar ");
	m_AlphaList.InsertAtEnd("TV-kanal");
	m_AlphaList.InsertAtEnd("TV-program");
	m_AlphaList.InsertAtEnd("TV-serie");
	m_AlphaList.InsertAtEnd("tv");
	m_AlphaList.InsertAtEnd("tva");
	m_AlphaList.InsertAtEnd("tvl");
	m_AlphaList.InsertAtEnd("tvlldder");
	m_AlphaList.InsertAtEnd("tvsitsig");
	m_AlphaList.InsertAtEnd("tvvningsbuss");
	m_AlphaList.InsertAtEnd("tvrfljt");
	m_AlphaList.InsertAtEnd("tvrs");
	m_AlphaList.InsertAtEnd("tvtt");
	m_AlphaList.InsertAtEnd("tvtta ");
	m_AlphaList.InsertAtEnd("tvttar");
	m_AlphaList.InsertAtEnd("tvttbalja");
	m_AlphaList.InsertAtEnd("tvttbar");
	m_AlphaList.InsertAtEnd("tvttbjrn");
	m_AlphaList.InsertAtEnd("tvttfat");
	m_AlphaList.InsertAtEnd("tvttinrttning");
	m_AlphaList.InsertAtEnd("tvttkorg");
	m_AlphaList.InsertAtEnd("tvttkorgen");
	m_AlphaList.InsertAtEnd("tvttlapp");
	m_AlphaList.InsertAtEnd("tvttlappen");
	m_AlphaList.InsertAtEnd("tvttmaskin");
	m_AlphaList.InsertAtEnd("tvttmedel");
	m_AlphaList.InsertAtEnd("tvttpse");
	m_AlphaList.InsertAtEnd("tvttstuga");
	m_AlphaList.InsertAtEnd("tvttstll");
	m_AlphaList.InsertAtEnd("tvttstllet");
	m_AlphaList.InsertAtEnd("tvttsvamp");
	m_AlphaList.InsertAtEnd("tycka");
	m_AlphaList.InsertAtEnd("tycker");
	m_AlphaList.InsertAtEnd("tycks");
	m_AlphaList.InsertAtEnd("tyckte");
	m_AlphaList.InsertAtEnd("tydligen");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("tyg");
	m_AlphaList.InsertAtEnd("tyngdlyftning");
	m_AlphaList.InsertAtEnd("typ");
	m_AlphaList.InsertAtEnd("tyst");
	m_AlphaList.InsertAtEnd("tysta");
	m_AlphaList.InsertAtEnd("tystnad");
	m_AlphaList.InsertAtEnd("tg");
	m_AlphaList.InsertAtEnd("tlamod");
	m_AlphaList.InsertAtEnd("tnagel");
	m_AlphaList.InsertAtEnd("tng");
	m_AlphaList.InsertAtEnd("tr");
	m_AlphaList.InsertAtEnd("trta");
	m_AlphaList.InsertAtEnd("tcka");
	m_AlphaList.InsertAtEnd("tcke");
	m_AlphaList.InsertAtEnd("tcker");
	m_AlphaList.InsertAtEnd("tckt");
	m_AlphaList.InsertAtEnd("tlt");
	m_AlphaList.InsertAtEnd("tlta");
	m_AlphaList.InsertAtEnd("tltar");
	m_AlphaList.InsertAtEnd("tnd");
	m_AlphaList.InsertAtEnd("tnda");
	m_AlphaList.InsertAtEnd("tnder");
	m_AlphaList.InsertAtEnd("tnderna");
	m_AlphaList.InsertAtEnd("tndsticka");
	m_AlphaList.InsertAtEnd("tnka");
	m_AlphaList.InsertAtEnd("tnkande");
	m_AlphaList.InsertAtEnd("tnkbar");
	m_AlphaList.InsertAtEnd("tnker");
	m_AlphaList.InsertAtEnd("tnkt");
	m_AlphaList.InsertAtEnd("tnkte");
	m_AlphaList.InsertAtEnd("trning");
	m_AlphaList.InsertAtEnd("tvla");
	m_AlphaList.InsertAtEnd("tvlar");
	m_AlphaList.InsertAtEnd("tvling");
	m_AlphaList.InsertAtEnd("tvlingscyklist");
	m_AlphaList.InsertAtEnd("tvlingsprogram");
	m_AlphaList.InsertAtEnd("tmma");
	m_AlphaList.InsertAtEnd("tmmer");
	m_AlphaList.InsertAtEnd("trstig");
	m_AlphaList.InsertAtEnd("ubt");
	m_AlphaList.InsertAtEnd("ufo");
	m_AlphaList.InsertAtEnd("uggla");
	m_AlphaList.InsertAtEnd("ugn");
	m_AlphaList.InsertAtEnd("ugnen");
	m_AlphaList.InsertAtEnd("ugnssteka");
	m_AlphaList.InsertAtEnd("ugnssteker");
	m_AlphaList.InsertAtEnd("ugnsvante");
	m_AlphaList.InsertAtEnd("ull");
	m_AlphaList.InsertAtEnd("umgnge");
	m_AlphaList.InsertAtEnd("undan");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("under");
	m_AlphaList.InsertAtEnd("underbyxor");
	m_AlphaList.InsertAtEnd("underjordiskt");
	m_AlphaList.InsertAtEnd("underkjol");
	m_AlphaList.InsertAtEnd("underklder");
	m_AlphaList.InsertAtEnd("underklderna");
	m_AlphaList.InsertAtEnd("underliv");
	m_AlphaList.InsertAtEnd("undersida");
	m_AlphaList.InsertAtEnd("underska");
	m_AlphaList.InsertAtEnd("undersker");
	m_AlphaList.InsertAtEnd("underskning");
	m_AlphaList.InsertAtEnd("underskningsbrits");
	m_AlphaList.InsertAtEnd("undertrja");
	m_AlphaList.InsertAtEnd("undervisa");
	m_AlphaList.InsertAtEnd("undervisar");
	m_AlphaList.InsertAtEnd("undervisning");
	m_AlphaList.InsertAtEnd("undra");
	m_AlphaList.InsertAtEnd("undrade");
	m_AlphaList.InsertAtEnd("undulat");
	m_AlphaList.InsertAtEnd("ung");
	m_AlphaList.InsertAtEnd("unga");
	m_AlphaList.InsertAtEnd("ungdomsrrelse");
	m_AlphaList.InsertAtEnd("unge");
	m_AlphaList.InsertAtEnd("ungefr");
	m_AlphaList.InsertAtEnd("universitet");
	m_AlphaList.InsertAtEnd("upp");
	m_AlphaList.InsertAtEnd("uppbyggnad ");
	m_AlphaList.InsertAtEnd("uppdragbar");
	m_AlphaList.InsertAtEnd("uppe");
	m_AlphaList.InsertAtEnd("uppfatta");
	m_AlphaList.InsertAtEnd("uppfattar");
	m_AlphaList.InsertAtEnd("uppfattning");
	m_AlphaList.InsertAtEnd("uppfinna");
	m_AlphaList.InsertAtEnd("uppfda");
	m_AlphaList.InsertAtEnd("uppfder");
	m_AlphaList.InsertAtEnd("uppfrande");
	m_AlphaList.InsertAtEnd("uppfrslutning");
	m_AlphaList.InsertAtEnd("uppgift");
	m_AlphaList.InsertAtEnd("uppgifter");
	m_AlphaList.InsertAtEnd("upphetsad");
	m_AlphaList.InsertAtEnd("upphetsning");
	m_AlphaList.InsertAtEnd("upplevelse");
	m_AlphaList.InsertAtEnd("uppmaning");
	m_AlphaList.InsertAtEnd("uppmuntra ");
	m_AlphaList.InsertAtEnd("uppmuntrar");
	m_AlphaList.InsertAtEnd("uppmrksam");
	m_AlphaList.InsertAtEnd("uppmrksamma");
	m_AlphaList.InsertAtEnd("uppmrksammar");
	m_AlphaList.InsertAtEnd("upprepa");
	m_AlphaList.InsertAtEnd("upprepar");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("uppriven");
	m_AlphaList.InsertAtEnd("upprrd");
	m_AlphaList.InsertAtEnd("uppsyn ");
	m_AlphaList.InsertAtEnd("upptagen");
	m_AlphaList.InsertAtEnd("upptill");
	m_AlphaList.InsertAtEnd("upptrda");
	m_AlphaList.InsertAtEnd("upptrder");
	m_AlphaList.InsertAtEnd("uppvxttid");
	m_AlphaList.InsertAtEnd("uppt");
	m_AlphaList.InsertAtEnd("ur");
	m_AlphaList.InsertAtEnd("urin");
	m_AlphaList.InsertAtEnd("urinoar");
	m_AlphaList.InsertAtEnd("urskt");
	m_AlphaList.InsertAtEnd("urskta");
	m_AlphaList.InsertAtEnd("ursktar");
	m_AlphaList.InsertAtEnd("urtavla");
	m_AlphaList.InsertAtEnd("usch");
	m_AlphaList.InsertAtEnd("ut");
	m_AlphaList.InsertAtEnd("utan");
	m_AlphaList.InsertAtEnd("utanfr");
	m_AlphaList.InsertAtEnd("utbildning");
	m_AlphaList.InsertAtEnd("ute");
	m_AlphaList.InsertAtEnd("uterum");
	m_AlphaList.InsertAtEnd("utesluta");
	m_AlphaList.InsertAtEnd("utesluter");
	m_AlphaList.InsertAtEnd("utestnga");
	m_AlphaList.InsertAtEnd("utestnger");
	m_AlphaList.InsertAtEnd("utflykt");
	m_AlphaList.InsertAtEnd("utgng");
	m_AlphaList.InsertAtEnd("utgngspunkt ");
	m_AlphaList.InsertAtEnd("utltande");
	m_AlphaList.InsertAtEnd("utlsning");
	m_AlphaList.InsertAtEnd("utmana");
	m_AlphaList.InsertAtEnd("utmanar");
	m_AlphaList.InsertAtEnd("utmanare");
	m_AlphaList.InsertAtEnd("utom");
	m_AlphaList.InsertAtEnd("utomhusmltid");
	m_AlphaList.InsertAtEnd("utomjordisk");
	m_AlphaList.InsertAtEnd("utomlands");
	m_AlphaList.InsertAtEnd("utrymningsvg");
	m_AlphaList.InsertAtEnd("utsidan");
	m_AlphaList.InsertAtEnd("utstllning");
	m_AlphaList.InsertAtEnd("utsugning");
	m_AlphaList.InsertAtEnd("utsndra");
	m_AlphaList.InsertAtEnd("utsndrar");
	m_AlphaList.InsertAtEnd("utter");
	m_AlphaList.InsertAtEnd("uttrkad");
	m_AlphaList.InsertAtEnd("utveckla");
	m_AlphaList.InsertAtEnd("utvecklar ");
	m_AlphaList.InsertAtEnd("utveckling");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("utvecklingen");
	m_AlphaList.InsertAtEnd("vacker");
	m_AlphaList.InsertAtEnd("vad");
	m_AlphaList.InsertAtEnd("vada");
	m_AlphaList.InsertAtEnd("vadar");
	m_AlphaList.InsertAtEnd("vagga");
	m_AlphaList.InsertAtEnd("vagn");
	m_AlphaList.InsertAtEnd("vaja");
	m_AlphaList.InsertAtEnd("vajar");
	m_AlphaList.InsertAtEnd("vaken");
	m_AlphaList.InsertAtEnd("vakna");
	m_AlphaList.InsertAtEnd("vaknar");
	m_AlphaList.InsertAtEnd("vaktmstare");
	m_AlphaList.InsertAtEnd("vakttorn");
	m_AlphaList.InsertAtEnd("val");
	m_AlphaList.InsertAtEnd("valnt");
	m_AlphaList.InsertAtEnd("valross");
	m_AlphaList.InsertAtEnd("valuta");
	m_AlphaList.InsertAtEnd("vampyr");
	m_AlphaList.InsertAtEnd("vandra");
	m_AlphaList.InsertAtEnd("vandrar");
	m_AlphaList.InsertAtEnd("vanilj");
	m_AlphaList.InsertAtEnd("vanlig");
	m_AlphaList.InsertAtEnd("vanliga");
	m_AlphaList.InsertAtEnd("vanligt");
	m_AlphaList.InsertAtEnd("vanljsocker");
	m_AlphaList.InsertAtEnd("vantar");
	m_AlphaList.InsertAtEnd("vante");
	m_AlphaList.InsertAtEnd("var");
	m_AlphaList.InsertAtEnd("vara");
	m_AlphaList.InsertAtEnd("varandra");
	m_AlphaList.InsertAtEnd("varar");
	m_AlphaList.InsertAtEnd("vardagsfrdigheter");
	m_AlphaList.InsertAtEnd("vardagsrum");
	m_AlphaList.InsertAtEnd("varelse");
	m_AlphaList.InsertAtEnd("varenda");
	m_AlphaList.InsertAtEnd("varfr");
	m_AlphaList.InsertAtEnd("varg");
	m_AlphaList.InsertAtEnd("variation");
	m_AlphaList.InsertAtEnd("varit");
	m_AlphaList.InsertAtEnd("varje");
	m_AlphaList.InsertAtEnd("varken");
	m_AlphaList.InsertAtEnd("varm");
	m_AlphaList.InsertAtEnd("varmluftsballong");
	m_AlphaList.InsertAtEnd("varmvatten");
	m_AlphaList.InsertAtEnd("varna");
	m_AlphaList.InsertAtEnd("varnar");
	m_AlphaList.InsertAtEnd("varor");
	m_AlphaList.InsertAtEnd("vars");
	m_AlphaList.InsertAtEnd("varsgod");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("vart");
	m_AlphaList.InsertAtEnd("varuautomat");
	m_AlphaList.InsertAtEnd("varuhus");
	m_AlphaList.InsertAtEnd("varuvagn");
	m_AlphaList.InsertAtEnd("vas");
	m_AlphaList.InsertAtEnd("vask");
	m_AlphaList.InsertAtEnd("vass");
	m_AlphaList.InsertAtEnd("vatten");
	m_AlphaList.InsertAtEnd("vattendrag");
	m_AlphaList.InsertAtEnd("vattenfall");
	m_AlphaList.InsertAtEnd("vattenfontn");
	m_AlphaList.InsertAtEnd("vattenhjul");
	m_AlphaList.InsertAtEnd("vattenkanna");
	m_AlphaList.InsertAtEnd("vattenkokare");
	m_AlphaList.InsertAtEnd("vattenmelon");
	m_AlphaList.InsertAtEnd("vattenpass");
	m_AlphaList.InsertAtEnd("vattenpistol");
	m_AlphaList.InsertAtEnd("vattenrutschkana");
	m_AlphaList.InsertAtEnd("vattensport");
	m_AlphaList.InsertAtEnd("vattenspridare");
	m_AlphaList.InsertAtEnd("vattensng");
	m_AlphaList.InsertAtEnd("vattentemperaturen");
	m_AlphaList.InsertAtEnd("vattentorn");
	m_AlphaList.InsertAtEnd("vattna");
	m_AlphaList.InsertAtEnd("vattnar");
	m_AlphaList.InsertAtEnd("vattnet");
	m_AlphaList.InsertAtEnd("vecka");
	m_AlphaList.InsertAtEnd("veckan");
	m_AlphaList.InsertAtEnd("veckodag");
	m_AlphaList.InsertAtEnd("veckodagar");
	m_AlphaList.InsertAtEnd("veckokalender");
	m_AlphaList.InsertAtEnd("veckor");
	m_AlphaList.InsertAtEnd("veckoslut");
	m_AlphaList.InsertAtEnd("vegetarian");
	m_AlphaList.InsertAtEnd("vem");
	m_AlphaList.InsertAtEnd("vems");
	m_AlphaList.InsertAtEnd("veranda");
	m_AlphaList.InsertAtEnd("verb");
	m_AlphaList.InsertAtEnd("verka");
	m_AlphaList.InsertAtEnd("verkade");
	m_AlphaList.InsertAtEnd("verkan");
	m_AlphaList.InsertAtEnd("verkar");
	m_AlphaList.InsertAtEnd("verket");
	m_AlphaList.InsertAtEnd("verklig");
	m_AlphaList.InsertAtEnd("verkligen");
	m_AlphaList.InsertAtEnd("verksam");
	m_AlphaList.InsertAtEnd("verksamhet");
	m_AlphaList.InsertAtEnd("verksamheten");
	m_AlphaList.InsertAtEnd("verkstad");
	m_AlphaList.InsertAtEnd("verktyg");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("verktygsfrrd");
	m_AlphaList.InsertAtEnd("verktygslda");
	m_AlphaList.InsertAtEnd("vestibul");
	m_AlphaList.InsertAtEnd("vet");
	m_AlphaList.InsertAtEnd("veta");
	m_AlphaList.InsertAtEnd("vetenskap");
	m_AlphaList.InsertAtEnd("vetenskapsman");
	m_AlphaList.InsertAtEnd("veteran");
	m_AlphaList.InsertAtEnd("veteranbil");
	m_AlphaList.InsertAtEnd("veterinr");
	m_AlphaList.InsertAtEnd("vetskap");
	m_AlphaList.InsertAtEnd("vi");
	m_AlphaList.InsertAtEnd("vid");
	m_AlphaList.InsertAtEnd("vidare");
	m_AlphaList.InsertAtEnd("video");
	m_AlphaList.InsertAtEnd("videoband");
	m_AlphaList.InsertAtEnd("videobandspelare");
	m_AlphaList.InsertAtEnd("videofodral");
	m_AlphaList.InsertAtEnd("videokamera");
	m_AlphaList.InsertAtEnd("videokassett");
	m_AlphaList.InsertAtEnd("vifta");
	m_AlphaList.InsertAtEnd("viftar");
	m_AlphaList.InsertAtEnd("vigsel");
	m_AlphaList.InsertAtEnd("vigselring");
	m_AlphaList.InsertAtEnd("vigvatten");
	m_AlphaList.InsertAtEnd("vik");
	m_AlphaList.InsertAtEnd("vika");
	m_AlphaList.InsertAtEnd("viker");
	m_AlphaList.InsertAtEnd("vikt");
	m_AlphaList.InsertAtEnd("viktig");
	m_AlphaList.InsertAtEnd("viktiga");
	m_AlphaList.InsertAtEnd("viktigt");
	m_AlphaList.InsertAtEnd("vila");
	m_AlphaList.InsertAtEnd("vilar");
	m_AlphaList.InsertAtEnd("vild");
	m_AlphaList.InsertAtEnd("vildsvin");
	m_AlphaList.InsertAtEnd("vilja");
	m_AlphaList.InsertAtEnd("vilka");
	m_AlphaList.InsertAtEnd("vilken");
	m_AlphaList.InsertAtEnd("vilkens");
	m_AlphaList.InsertAtEnd("vilket");
	m_AlphaList.InsertAtEnd("vill");
	m_AlphaList.InsertAtEnd("ville");
	m_AlphaList.InsertAtEnd("vilodag");
	m_AlphaList.InsertAtEnd("vilostund");
	m_AlphaList.InsertAtEnd("vilrum");
	m_AlphaList.InsertAtEnd("vilt ");
	m_AlphaList.InsertAtEnd("vimpel");
	m_AlphaList.InsertAtEnd("vin");
	m_AlphaList.InsertAtEnd("vinbr");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("vinbrssaft");
	m_AlphaList.InsertAtEnd("vind");
	m_AlphaList.InsertAtEnd("vindrutetorkare");
	m_AlphaList.InsertAtEnd("vindruvor");
	m_AlphaList.InsertAtEnd("vindsnurra");
	m_AlphaList.InsertAtEnd("vindsurfa ");
	m_AlphaList.InsertAtEnd("vindsurfar");
	m_AlphaList.InsertAtEnd("vinflaska");
	m_AlphaList.InsertAtEnd("vingar");
	m_AlphaList.InsertAtEnd("vinge");
	m_AlphaList.InsertAtEnd("vinglas");
	m_AlphaList.InsertAtEnd("vinka");
	m_AlphaList.InsertAtEnd("vinkar");
	m_AlphaList.InsertAtEnd("vinkel");
	m_AlphaList.InsertAtEnd("vinkelhake");
	m_AlphaList.InsertAtEnd("vinkelrt");
	m_AlphaList.InsertAtEnd("vinna");
	m_AlphaList.InsertAtEnd("vinnare");
	m_AlphaList.InsertAtEnd("vinner");
	m_AlphaList.InsertAtEnd("vinranka");
	m_AlphaList.InsertAtEnd("vinter");
	m_AlphaList.InsertAtEnd("vinterklder");
	m_AlphaList.InsertAtEnd("vinger");
	m_AlphaList.InsertAtEnd("violett");
	m_AlphaList.InsertAtEnd("violin");
	m_AlphaList.InsertAtEnd("vippa");
	m_AlphaList.InsertAtEnd("virka");
	m_AlphaList.InsertAtEnd("virknl");
	m_AlphaList.InsertAtEnd("virvelstorm");
	m_AlphaList.InsertAtEnd("visa");
	m_AlphaList.InsertAtEnd("visade");
	m_AlphaList.InsertAtEnd("visar");
	m_AlphaList.InsertAtEnd("visartavla");
	m_AlphaList.InsertAtEnd("visir");
	m_AlphaList.InsertAtEnd("viska");
	m_AlphaList.InsertAtEnd("viskar");
	m_AlphaList.InsertAtEnd("visp");
	m_AlphaList.InsertAtEnd("vispa");
	m_AlphaList.InsertAtEnd("vispad");
	m_AlphaList.InsertAtEnd("vispar");
	m_AlphaList.InsertAtEnd("viss");
	m_AlphaList.InsertAtEnd("vissa");
	m_AlphaList.InsertAtEnd("visseljohanna");
	m_AlphaList.InsertAtEnd("visselpipa");
	m_AlphaList.InsertAtEnd("vissla");
	m_AlphaList.InsertAtEnd("visslar");
	m_AlphaList.InsertAtEnd("vit");
	m_AlphaList.InsertAtEnd("vitamin");
	m_AlphaList.InsertAtEnd("vitkl");
	m_AlphaList.InsertAtEnd("vitlk");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("vits");
	m_AlphaList.InsertAtEnd("vittra");
	m_AlphaList.InsertAtEnd("vittrar");
	m_AlphaList.InsertAtEnd("vodka");
	m_AlphaList.InsertAtEnd("volleyboll");
	m_AlphaList.InsertAtEnd("volym");
	m_AlphaList.InsertAtEnd("volymen");
	m_AlphaList.InsertAtEnd("vore");
	m_AlphaList.InsertAtEnd("workshop");
	m_AlphaList.InsertAtEnd("vrida");
	m_AlphaList.InsertAtEnd("vrider");
	m_AlphaList.InsertAtEnd("vrist");
	m_AlphaList.InsertAtEnd("vrla");
	m_AlphaList.InsertAtEnd("vrlar");
	m_AlphaList.InsertAtEnd("vulkan");
	m_AlphaList.InsertAtEnd("vuxen");
	m_AlphaList.InsertAtEnd("vffeljrn");
	m_AlphaList.InsertAtEnd("vffla");
	m_AlphaList.InsertAtEnd("vg");
	m_AlphaList.InsertAtEnd("vga");
	m_AlphaList.InsertAtEnd("vgar");
	m_AlphaList.InsertAtEnd("vgig");
	m_AlphaList.InsertAtEnd("vldtkt");
	m_AlphaList.InsertAtEnd("vning");
	m_AlphaList.InsertAtEnd("vningssng");
	m_AlphaList.InsertAtEnd("vr");
	m_AlphaList.InsertAtEnd("vrda");
	m_AlphaList.InsertAtEnd("vrdnad");
	m_AlphaList.InsertAtEnd("vrrulle");
	m_AlphaList.InsertAtEnd("vrt");
	m_AlphaList.InsertAtEnd("vt");
	m_AlphaList.InsertAtEnd("vcka");
	m_AlphaList.InsertAtEnd("vckarklocka");
	m_AlphaList.InsertAtEnd("vcker");
	m_AlphaList.InsertAtEnd("vder");
	m_AlphaList.InsertAtEnd("vg");
	m_AlphaList.InsertAtEnd("vga");
	m_AlphaList.InsertAtEnd("vgbula");
	m_AlphaList.InsertAtEnd("vgen");
	m_AlphaList.InsertAtEnd("vger");
	m_AlphaList.InsertAtEnd("vgg");
	m_AlphaList.InsertAtEnd("vggen");
	m_AlphaList.InsertAtEnd("vggskp");
	m_AlphaList.InsertAtEnd("vgguttag");
	m_AlphaList.InsertAtEnd("vgvisare");
	m_AlphaList.InsertAtEnd("vja");
	m_AlphaList.InsertAtEnd("vjer");
	m_AlphaList.InsertAtEnd("vl");
	m_AlphaList.InsertAtEnd("vldigt");
	m_AlphaList.InsertAtEnd("vlja");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("vljer");
	m_AlphaList.InsertAtEnd("vlkommen");
	m_AlphaList.InsertAtEnd("vlsigna");
	m_AlphaList.InsertAtEnd("vlsignar");
	m_AlphaList.InsertAtEnd("vlsignelse");
	m_AlphaList.InsertAtEnd("vn");
	m_AlphaList.InsertAtEnd("vnda");
	m_AlphaList.InsertAtEnd("vnde");
	m_AlphaList.InsertAtEnd("vnder");
	m_AlphaList.InsertAtEnd("vnlig");
	m_AlphaList.InsertAtEnd("vnner");
	m_AlphaList.InsertAtEnd("vnster");
	m_AlphaList.InsertAtEnd("vnsterklick");
	m_AlphaList.InsertAtEnd("vnsterparentes");
	m_AlphaList.InsertAtEnd("vnt");
	m_AlphaList.InsertAtEnd("vnta");
	m_AlphaList.InsertAtEnd("vntade");
	m_AlphaList.InsertAtEnd("vntar");
	m_AlphaList.InsertAtEnd("vntrum");
	m_AlphaList.InsertAtEnd("vrde");
	m_AlphaList.InsertAtEnd("vrdering");
	m_AlphaList.InsertAtEnd("vrk");
	m_AlphaList.InsertAtEnd("vrld");
	m_AlphaList.InsertAtEnd("vrlden");
	m_AlphaList.InsertAtEnd("vrldshistoria");
	m_AlphaList.InsertAtEnd("vrldsrymd");
	m_AlphaList.InsertAtEnd("vrma");
	m_AlphaList.InsertAtEnd("vrme");
	m_AlphaList.InsertAtEnd("vrmedyna");
	m_AlphaList.InsertAtEnd("vrmen");
	m_AlphaList.InsertAtEnd("vrmer");
	m_AlphaList.InsertAtEnd("vrpa");
	m_AlphaList.InsertAtEnd("vrper");
	m_AlphaList.InsertAtEnd("vrst");
	m_AlphaList.InsertAtEnd("vsen");
	m_AlphaList.InsertAtEnd("vska");
	m_AlphaList.InsertAtEnd("vssa");
	m_AlphaList.InsertAtEnd("vssar");
	m_AlphaList.InsertAtEnd("vst");
	m_AlphaList.InsertAtEnd("vster");
	m_AlphaList.InsertAtEnd("vstlig");
	m_AlphaList.InsertAtEnd("vstra");
	m_AlphaList.InsertAtEnd("vtska");
	m_AlphaList.InsertAtEnd("vva");
	m_AlphaList.InsertAtEnd("vvnad");
	m_AlphaList.InsertAtEnd("vvsked");
	m_AlphaList.InsertAtEnd("vvstol");
	m_AlphaList.InsertAtEnd("vxa");
	m_AlphaList.InsertAtEnd("vxel");
	m_AlphaList.InsertAtEnd("vxer ");
	m_AlphaList.InsertAtEnd("vxla ");
	m_AlphaList.InsertAtEnd("vxlar");
	m_AlphaList.InsertAtEnd("vxt");
	m_AlphaList.InsertAtEnd("vxthus");
	m_AlphaList.InsertAtEnd("xylofon");
	m_AlphaList.InsertAtEnd("ymnighetshorn");
	m_AlphaList.InsertAtEnd("yngre");
	m_AlphaList.InsertAtEnd("yngst");
	m_AlphaList.InsertAtEnd("yoga");
	m_AlphaList.InsertAtEnd("yoghurt");
	m_AlphaList.InsertAtEnd("yrke");
	m_AlphaList.InsertAtEnd("yrkesmssig");
	m_AlphaList.InsertAtEnd("ytterklder");
	m_AlphaList.InsertAtEnd("ytterligare");
	m_AlphaList.InsertAtEnd("yttre");
	m_AlphaList.InsertAtEnd("yxa");
	m_AlphaList.InsertAtEnd("zebra");
	m_AlphaList.InsertAtEnd("zonbyte");
	m_AlphaList.InsertAtEnd("zoo");
	m_AlphaList.InsertAtEnd("zoomlins");
	m_AlphaList.InsertAtEnd("");
	m_AlphaList.InsertAtEnd("ka");
	m_AlphaList.InsertAtEnd("ker");
	m_AlphaList.InsertAtEnd("lder");
	m_AlphaList.InsertAtEnd("nga");
	m_AlphaList.InsertAtEnd("ngar");
	m_AlphaList.InsertAtEnd("ngbad");
	m_AlphaList.InsertAtEnd("ngra");
	m_AlphaList.InsertAtEnd("r");
	m_AlphaList.InsertAtEnd("ra");
	m_AlphaList.InsertAtEnd("ren");
	m_AlphaList.InsertAtEnd("ret");
	m_AlphaList.InsertAtEnd("rhundrade");
	m_AlphaList.InsertAtEnd("rs");
	m_AlphaList.InsertAtEnd("rsdag");
	m_AlphaList.InsertAtEnd("rstid");
	m_AlphaList.InsertAtEnd("rstider");
	m_AlphaList.InsertAtEnd("rtionde");
	m_AlphaList.InsertAtEnd("sikt");
	m_AlphaList.InsertAtEnd("ska");
	m_AlphaList.InsertAtEnd("skvder");
	m_AlphaList.InsertAtEnd("skdarlktare");
	m_AlphaList.InsertAtEnd("sna");
	m_AlphaList.InsertAtEnd("t");
	m_AlphaList.InsertAtEnd("ter");
	m_AlphaList.InsertAtEnd("terblicka");
	m_AlphaList.InsertAtEnd("terblickar");
	m_AlphaList.InsertAtEnd("terkomst");
	m_AlphaList.InsertAtEnd("tervinna");
	m_AlphaList.InsertAtEnd("tervinner");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("tervinningslda");
	m_AlphaList.InsertAtEnd("tervinningstunna");
	m_AlphaList.InsertAtEnd("tervnda");
	m_AlphaList.InsertAtEnd("tgrder");
	m_AlphaList.InsertAtEnd("tminstone");
	m_AlphaList.InsertAtEnd("trvrd");
	m_AlphaList.InsertAtEnd("tta");
	m_AlphaList.InsertAtEnd("ttio");
	m_AlphaList.InsertAtEnd("ttondel");
	m_AlphaList.InsertAtEnd("cklig");
	m_AlphaList.InsertAtEnd("ga");
	m_AlphaList.InsertAtEnd("ger");
	m_AlphaList.InsertAtEnd("gg");
	m_AlphaList.InsertAtEnd("ggkopp");
	m_AlphaList.InsertAtEnd("ggkorg");
	m_AlphaList.InsertAtEnd("ggledare");
	m_AlphaList.InsertAtEnd("ggplanta");
	m_AlphaList.InsertAtEnd("ggrra");
	m_AlphaList.InsertAtEnd("ggsallad");
	m_AlphaList.InsertAtEnd("ggskrare");
	m_AlphaList.InsertAtEnd("ggstock");
	m_AlphaList.InsertAtEnd("ggtoddy");
	m_AlphaList.InsertAtEnd("ggula");
	m_AlphaList.InsertAtEnd("ggvita");
	m_AlphaList.InsertAtEnd("kta");
	m_AlphaList.InsertAtEnd("ktenskap");
	m_AlphaList.InsertAtEnd("ldre");
	m_AlphaList.InsertAtEnd("lg");
	m_AlphaList.InsertAtEnd("lska");
	m_AlphaList.InsertAtEnd("lskade");
	m_AlphaList.InsertAtEnd("lskar");
	m_AlphaList.InsertAtEnd("lsklig");
	m_AlphaList.InsertAtEnd("lskling");
	m_AlphaList.InsertAtEnd("mne");
	m_AlphaList.InsertAtEnd("n");
	m_AlphaList.InsertAtEnd("nda");
	m_AlphaList.InsertAtEnd("ndan");
	m_AlphaList.InsertAtEnd("ndra");
	m_AlphaList.InsertAtEnd("ndrar");
	m_AlphaList.InsertAtEnd("nd");
	m_AlphaList.InsertAtEnd("ng");
	m_AlphaList.InsertAtEnd("ngel");
	m_AlphaList.InsertAtEnd("ngslan");
	m_AlphaList.InsertAtEnd("nnu");
	m_AlphaList.InsertAtEnd("ntligen");
	m_AlphaList.InsertAtEnd("ppeljuice");
	m_AlphaList.InsertAtEnd("ppelmos");
	m_AlphaList.InsertAtEnd("ppelsaft");
	m_AlphaList.InsertAtEnd("ppelsirap");
	m_AlphaList.InsertAtEnd("ppeltrd");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("pple");
	m_AlphaList.InsertAtEnd("pplen");
	m_AlphaList.InsertAtEnd("ppleurkrnare");
	m_AlphaList.InsertAtEnd("r");
	m_AlphaList.InsertAtEnd("rm");
	m_AlphaList.InsertAtEnd("rmen");
	m_AlphaList.InsertAtEnd("rr");
	m_AlphaList.InsertAtEnd("rtor");
	m_AlphaList.InsertAtEnd("ta");
	m_AlphaList.InsertAtEnd("ter");
	m_AlphaList.InsertAtEnd("ts");
	m_AlphaList.InsertAtEnd("dla");
	m_AlphaList.InsertAtEnd("dmjuk");
	m_AlphaList.InsertAtEnd("ga");
	m_AlphaList.InsertAtEnd("gat");
	m_AlphaList.InsertAtEnd("gla");
	m_AlphaList.InsertAtEnd("gon");
	m_AlphaList.InsertAtEnd("gonbryn");
	m_AlphaList.InsertAtEnd("gonbrynspenna");
	m_AlphaList.InsertAtEnd("gondroppar");
	m_AlphaList.InsertAtEnd("gonen");
	m_AlphaList.InsertAtEnd("gonfrans");
	m_AlphaList.InsertAtEnd("gonlock");
	m_AlphaList.InsertAtEnd("gonpekningsram");
	m_AlphaList.InsertAtEnd("gonskugga");
	m_AlphaList.InsertAtEnd("ka");
	m_AlphaList.InsertAtEnd("kar");
	m_AlphaList.InsertAtEnd("ken");
	m_AlphaList.InsertAtEnd("mtlig");
	m_AlphaList.InsertAtEnd("nska");
	m_AlphaList.InsertAtEnd("nskvrd");
	m_AlphaList.InsertAtEnd("ppen");
	m_AlphaList.InsertAtEnd("ppna");
	m_AlphaList.InsertAtEnd("ppnar");
	m_AlphaList.InsertAtEnd("ppning");
	m_AlphaList.InsertAtEnd("ra");
	m_AlphaList.InsertAtEnd("rat");
	m_AlphaList.InsertAtEnd("rhngen");
	m_AlphaList.InsertAtEnd("rn");
	m_AlphaList.InsertAtEnd("rngott");
	m_AlphaList.InsertAtEnd("rondroppar");
	m_AlphaList.InsertAtEnd("ronen");
	m_AlphaList.InsertAtEnd("ronlappar");
	m_AlphaList.InsertAtEnd("ronlkare");
	m_AlphaList.InsertAtEnd("ronpropp");
	m_AlphaList.InsertAtEnd("rtkryddad");
	m_AlphaList.InsertAtEnd("sa ");
	m_AlphaList.InsertAtEnd("ser");
	m_AlphaList.InsertAtEnd("ster");
	m_AlphaList.InsertAtEnd("stlig");
	pWnd->StepIt();
	pWnd->PeekAndPump();	
	m_AlphaList.InsertAtEnd("stra");
	m_AlphaList.InsertAtEnd("va");
	m_AlphaList.InsertAtEnd("ver");
	m_AlphaList.InsertAtEnd("verdragsklder");
	m_AlphaList.InsertAtEnd("verenskommelse");
	m_AlphaList.InsertAtEnd("vergrepp");
	m_AlphaList.InsertAtEnd("vergngsstlle");
	m_AlphaList.InsertAtEnd("verkast");
	m_AlphaList.InsertAtEnd("verliggare");
	m_AlphaList.InsertAtEnd("verraska");
	m_AlphaList.InsertAtEnd("verraskar ");
	m_AlphaList.InsertAtEnd("verraskning");
	m_AlphaList.InsertAtEnd("versida");
	m_AlphaList.InsertAtEnd("versvmning");
	m_AlphaList.InsertAtEnd("verstta");
	m_AlphaList.InsertAtEnd("verstter");
	m_AlphaList.InsertAtEnd("vertala");
	m_AlphaList.InsertAtEnd("vertalar");
	pWnd->StepIt();
	pWnd->PeekAndPump();	

	delete pWnd;
#else
	m_AlphaList.InsertAtEnd("able");	
	m_AlphaList.InsertAtEnd("abort");	
	m_AlphaList.InsertAtEnd("about");	
	m_AlphaList.InsertAtEnd("above");	
	m_AlphaList.InsertAtEnd("abroad");	
	m_AlphaList.InsertAtEnd("abrupt");	
	m_AlphaList.InsertAtEnd("abruptly");	
	m_AlphaList.InsertAtEnd("absent");	
	m_AlphaList.InsertAtEnd("absolute");	
	m_AlphaList.InsertAtEnd("absorb");	
	m_AlphaList.InsertAtEnd("absurd");	
	m_AlphaList.InsertAtEnd("abundance");	
	m_AlphaList.InsertAtEnd("abuse");	
	m_AlphaList.InsertAtEnd("academic");	
	m_AlphaList.InsertAtEnd("acceleration");	
	m_AlphaList.InsertAtEnd("accept");	
	m_AlphaList.InsertAtEnd("acceptable");	
	m_AlphaList.InsertAtEnd("acceptance");	
	m_AlphaList.InsertAtEnd("access");	
	m_AlphaList.InsertAtEnd("accident");	
	m_AlphaList.InsertAtEnd("accommodate");	
	m_AlphaList.InsertAtEnd("accomplish");	
	m_AlphaList.InsertAtEnd("accomplished");	
	m_AlphaList.InsertAtEnd("according");	
	m_AlphaList.InsertAtEnd("accordingly");	
	m_AlphaList.InsertAtEnd("account");	
	m_AlphaList.InsertAtEnd("accuracy");	
	m_AlphaList.InsertAtEnd("accurate");	
	m_AlphaList.InsertAtEnd("achieve");	
	m_AlphaList.InsertAtEnd("achievement");	
	m_AlphaList.InsertAtEnd("acknowledge");	
	m_AlphaList.InsertAtEnd("acquire");	
	m_AlphaList.InsertAtEnd("across");	
	m_AlphaList.InsertAtEnd("acting");	
	m_AlphaList.InsertAtEnd("action");	
	m_AlphaList.InsertAtEnd("active");	
	m_AlphaList.InsertAtEnd("activity");	
	m_AlphaList.InsertAtEnd("actor");	
	m_AlphaList.InsertAtEnd("actual");	
	m_AlphaList.InsertAtEnd("actually");	
	m_AlphaList.InsertAtEnd("acute");	
	m_AlphaList.InsertAtEnd("addition");	
	m_AlphaList.InsertAtEnd("additional");	
	m_AlphaList.InsertAtEnd("address");	
	m_AlphaList.InsertAtEnd("addressed");	
	m_AlphaList.InsertAtEnd("adequate");	
	m_AlphaList.InsertAtEnd("adjacent");	
	m_AlphaList.InsertAtEnd("adjust");	
	m_AlphaList.InsertAtEnd("adjustment");	
	m_AlphaList.InsertAtEnd("admission");	
	m_AlphaList.InsertAtEnd("admit");	
	m_AlphaList.InsertAtEnd("adopt");	
	m_AlphaList.InsertAtEnd("adoption");	
	m_AlphaList.InsertAtEnd("adult");	
	m_AlphaList.InsertAtEnd("advance");	
	m_AlphaList.InsertAtEnd("advanced");	
	m_AlphaList.InsertAtEnd("advantage");	
	m_AlphaList.InsertAtEnd("adventure");	
	m_AlphaList.InsertAtEnd("adverse");	
	m_AlphaList.InsertAtEnd("advice");	
	m_AlphaList.InsertAtEnd("advised");	
	m_AlphaList.InsertAtEnd("advisory");	
	m_AlphaList.InsertAtEnd("aeroplane");	
	m_AlphaList.InsertAtEnd("affair");	
	m_AlphaList.InsertAtEnd("affect");	
	m_AlphaList.InsertAtEnd("affected");	
	m_AlphaList.InsertAtEnd("affection");	
	m_AlphaList.InsertAtEnd("afford");	
	m_AlphaList.InsertAtEnd("afraid");	
	m_AlphaList.InsertAtEnd("after");	
	m_AlphaList.InsertAtEnd("afternoon");	
	m_AlphaList.InsertAtEnd("afterwards");	
	m_AlphaList.InsertAtEnd("again");	
	m_AlphaList.InsertAtEnd("against");	
	m_AlphaList.InsertAtEnd("aged");	
	m_AlphaList.InsertAtEnd("aggressive");	
	m_AlphaList.InsertAtEnd("agree");	
	m_AlphaList.InsertAtEnd("agreeable");	
	m_AlphaList.InsertAtEnd("agreement");	
	m_AlphaList.InsertAtEnd("ahead");	
	m_AlphaList.InsertAtEnd("aircraft");	
	m_AlphaList.InsertAtEnd("airport");	
	m_AlphaList.InsertAtEnd("alarm");	
	m_AlphaList.InsertAtEnd("alcohol");	
	m_AlphaList.InsertAtEnd("alert");	
	m_AlphaList.InsertAtEnd("alien");	
	m_AlphaList.InsertAtEnd("alike");	
	m_AlphaList.InsertAtEnd("alive");	
	m_AlphaList.InsertAtEnd("alliance");	
	m_AlphaList.InsertAtEnd("allied");	
	m_AlphaList.InsertAtEnd("allocate");	
	m_AlphaList.InsertAtEnd("allow");	
	m_AlphaList.InsertAtEnd("allowance");	
	m_AlphaList.InsertAtEnd("allowed");	
	m_AlphaList.InsertAtEnd("almost");	
	m_AlphaList.InsertAtEnd("alone");	
	m_AlphaList.InsertAtEnd("along");	
	m_AlphaList.InsertAtEnd("aloud");	
	m_AlphaList.InsertAtEnd("already");	
	m_AlphaList.InsertAtEnd("also");	
	m_AlphaList.InsertAtEnd("alter");	
	m_AlphaList.InsertAtEnd("alternate");	
	m_AlphaList.InsertAtEnd("alternative");	
	m_AlphaList.InsertAtEnd("although");	
	m_AlphaList.InsertAtEnd("altogether");	
	m_AlphaList.InsertAtEnd("always");	
	m_AlphaList.InsertAtEnd("amazed");	
	m_AlphaList.InsertAtEnd("amazing");	
	m_AlphaList.InsertAtEnd("ambiguity");	
	m_AlphaList.InsertAtEnd("ambiguous");	
	m_AlphaList.InsertAtEnd("ambition");	
	m_AlphaList.InsertAtEnd("ambitious");	
	m_AlphaList.InsertAtEnd("amendment");	
	m_AlphaList.InsertAtEnd("ammunition");	
	m_AlphaList.InsertAtEnd("among");	
	m_AlphaList.InsertAtEnd("amount");	
	m_AlphaList.InsertAtEnd("ample");	
	m_AlphaList.InsertAtEnd("analyse");	
	m_AlphaList.InsertAtEnd("analysis");	
	m_AlphaList.InsertAtEnd("anchor");	
	m_AlphaList.InsertAtEnd("ancient");	
	m_AlphaList.InsertAtEnd("angel");	
	m_AlphaList.InsertAtEnd("anger");	
	m_AlphaList.InsertAtEnd("angle");	
	m_AlphaList.InsertAtEnd("angry");	
	m_AlphaList.InsertAtEnd("animal");	
	m_AlphaList.InsertAtEnd("anniversary");	
	m_AlphaList.InsertAtEnd("announce");	
	m_AlphaList.InsertAtEnd("announcement");	
	m_AlphaList.InsertAtEnd("annual");	
	m_AlphaList.InsertAtEnd("anonymous");	
	m_AlphaList.InsertAtEnd("another");	
	m_AlphaList.InsertAtEnd("answer");	
	m_AlphaList.InsertAtEnd("antenna");	
	m_AlphaList.InsertAtEnd("anticipate");	
	m_AlphaList.InsertAtEnd("anticipation");	
	m_AlphaList.InsertAtEnd("antique");	
	m_AlphaList.InsertAtEnd("anxiety");	
	m_AlphaList.InsertAtEnd("anxious");	
	m_AlphaList.InsertAtEnd("anybody");	
	m_AlphaList.InsertAtEnd("anyhow");	
	m_AlphaList.InsertAtEnd("anyone");	
	m_AlphaList.InsertAtEnd("anything");	
	m_AlphaList.InsertAtEnd("anyway");	
	m_AlphaList.InsertAtEnd("anywhere");	
	m_AlphaList.InsertAtEnd("apart");	
	m_AlphaList.InsertAtEnd("apartment");	
	m_AlphaList.InsertAtEnd("apparent");	
	m_AlphaList.InsertAtEnd("apparently");	
	m_AlphaList.InsertAtEnd("appeal");	
	m_AlphaList.InsertAtEnd("appear");	
	m_AlphaList.InsertAtEnd("appearance");	
	m_AlphaList.InsertAtEnd("appetite");	
	m_AlphaList.InsertAtEnd("applause");	
	m_AlphaList.InsertAtEnd("application");	
	m_AlphaList.InsertAtEnd("applied");	
	m_AlphaList.InsertAtEnd("apply");	
	m_AlphaList.InsertAtEnd("appointed");	
	m_AlphaList.InsertAtEnd("appointment");	
	m_AlphaList.InsertAtEnd("appreciate");	
	m_AlphaList.InsertAtEnd("appreciation");	
	m_AlphaList.InsertAtEnd("apprehension");	
	m_AlphaList.InsertAtEnd("apprentice");	
	m_AlphaList.InsertAtEnd("approach");	
	m_AlphaList.InsertAtEnd("appropriate");	
	m_AlphaList.InsertAtEnd("approval");	
	m_AlphaList.InsertAtEnd("approve");	
	m_AlphaList.InsertAtEnd("approved");	
	m_AlphaList.InsertAtEnd("approximate");	
	m_AlphaList.InsertAtEnd("April");	
	m_AlphaList.InsertAtEnd("arbitrary");	
	m_AlphaList.InsertAtEnd("arch");	
	m_AlphaList.InsertAtEnd("arched");	
	m_AlphaList.InsertAtEnd("architect");	
	m_AlphaList.InsertAtEnd("architecture");	
	m_AlphaList.InsertAtEnd("area");	
	m_AlphaList.InsertAtEnd("argue");	
	m_AlphaList.InsertAtEnd("argument");	
	m_AlphaList.InsertAtEnd("arise");	
	m_AlphaList.InsertAtEnd("armed");	
	m_AlphaList.InsertAtEnd("army");	
	m_AlphaList.InsertAtEnd("arose");	
	m_AlphaList.InsertAtEnd("around");	
	m_AlphaList.InsertAtEnd("arrangement");	
	m_AlphaList.InsertAtEnd("array");	
	m_AlphaList.InsertAtEnd("arrest");	
	m_AlphaList.InsertAtEnd("arrival");	
	m_AlphaList.InsertAtEnd("arrive");	
	m_AlphaList.InsertAtEnd("arrow");	
	m_AlphaList.InsertAtEnd("article");	
	m_AlphaList.InsertAtEnd("artificial");
	m_AlphaList.InsertAtEnd("artist");	
	m_AlphaList.InsertAtEnd("artistic");	
	m_AlphaList.InsertAtEnd("ashamed");	
	m_AlphaList.InsertAtEnd("aside");	
	m_AlphaList.InsertAtEnd("asleep");	
	m_AlphaList.InsertAtEnd("aspect");	
	m_AlphaList.InsertAtEnd("assault");	
	m_AlphaList.InsertAtEnd("assembly");	
	m_AlphaList.InsertAtEnd("assessment");	
	m_AlphaList.InsertAtEnd("assets");	
	m_AlphaList.InsertAtEnd("assign");	
	m_AlphaList.InsertAtEnd("assignment");	
	m_AlphaList.InsertAtEnd("assist");	
	m_AlphaList.InsertAtEnd("assistance");	
	m_AlphaList.InsertAtEnd("assistant");	
	m_AlphaList.InsertAtEnd("associate");	
	m_AlphaList.InsertAtEnd("associated");	
	m_AlphaList.InsertAtEnd("association");	
	m_AlphaList.InsertAtEnd("assume");	
	m_AlphaList.InsertAtEnd("assumption");	
	m_AlphaList.InsertAtEnd("assurance");	
	m_AlphaList.InsertAtEnd("assure");	
	m_AlphaList.InsertAtEnd("atlas");	
	m_AlphaList.InsertAtEnd("atmosphere");	
	m_AlphaList.InsertAtEnd("attach");	
	m_AlphaList.InsertAtEnd("attached");	
	m_AlphaList.InsertAtEnd("attack");	
	m_AlphaList.InsertAtEnd("attain");	
	m_AlphaList.InsertAtEnd("attempt");	
	m_AlphaList.InsertAtEnd("attend");	
	m_AlphaList.InsertAtEnd("attendance");	
	m_AlphaList.InsertAtEnd("attendant");	
	m_AlphaList.InsertAtEnd("attention");	
	m_AlphaList.InsertAtEnd("attitude");	
	m_AlphaList.InsertAtEnd("attract");	
	m_AlphaList.InsertAtEnd("attraction");	
	m_AlphaList.InsertAtEnd("attractive");	
	m_AlphaList.InsertAtEnd("audience");	
	m_AlphaList.InsertAtEnd("august");	
	m_AlphaList.InsertAtEnd("aunt");	
	m_AlphaList.InsertAtEnd("authentic");	
	m_AlphaList.InsertAtEnd("author");	
	m_AlphaList.InsertAtEnd("authority");	
	m_AlphaList.InsertAtEnd("authorized");
	m_AlphaList.InsertAtEnd("automatic");
	m_AlphaList.InsertAtEnd("autumn");
	m_AlphaList.InsertAtEnd("availability");	
	m_AlphaList.InsertAtEnd("available");	
	m_AlphaList.InsertAtEnd("average");	
	m_AlphaList.InsertAtEnd("avoid");	
	m_AlphaList.InsertAtEnd("awake");	
	m_AlphaList.InsertAtEnd("award");	
	m_AlphaList.InsertAtEnd("aware");	
	m_AlphaList.InsertAtEnd("away");	
	m_AlphaList.InsertAtEnd("awful");	
	m_AlphaList.InsertAtEnd("awkward");	
	m_AlphaList.InsertAtEnd("back");	
	m_AlphaList.InsertAtEnd("be");	
	m_AlphaList.InsertAtEnd("been");	
	m_AlphaList.InsertAtEnd("before");	
	m_AlphaList.InsertAtEnd("big");	
	m_AlphaList.InsertAtEnd("but");	
	m_AlphaList.InsertAtEnd("by");	
	m_AlphaList.InsertAtEnd("call");	
	m_AlphaList.InsertAtEnd("came");	
	m_AlphaList.InsertAtEnd("can");	
	m_AlphaList.InsertAtEnd("come");	
	m_AlphaList.InsertAtEnd("could");	
	m_AlphaList.InsertAtEnd("did");	
	m_AlphaList.InsertAtEnd("do");	
	m_AlphaList.InsertAtEnd("down");	
	m_AlphaList.InsertAtEnd("for");	
	m_AlphaList.InsertAtEnd("forst");	
	m_AlphaList.InsertAtEnd("from");	
	m_AlphaList.InsertAtEnd("get");	
	m_AlphaList.InsertAtEnd("go");	
	m_AlphaList.InsertAtEnd("had");	
	m_AlphaList.InsertAtEnd("has");	
	m_AlphaList.InsertAtEnd("have");	
	m_AlphaList.InsertAtEnd("he");	
	m_AlphaList.InsertAtEnd("her");	
	m_AlphaList.InsertAtEnd("here");	
	m_AlphaList.InsertAtEnd("him");	
	m_AlphaList.InsertAtEnd("his");	
	m_AlphaList.InsertAtEnd("I");	
	m_AlphaList.InsertAtEnd("if");	
	m_AlphaList.InsertAtEnd("in");	
	m_AlphaList.InsertAtEnd("into");	
	m_AlphaList.InsertAtEnd("is");	
	m_AlphaList.InsertAtEnd("it");	
	m_AlphaList.InsertAtEnd("just");	
	m_AlphaList.InsertAtEnd("like");	
	m_AlphaList.InsertAtEnd("little");	
	m_AlphaList.InsertAtEnd("look");	
	m_AlphaList.InsertAtEnd("made");	
	m_AlphaList.InsertAtEnd("make");	
	m_AlphaList.InsertAtEnd("me");	
	m_AlphaList.InsertAtEnd("more");	
	m_AlphaList.InsertAtEnd("much");	
	m_AlphaList.InsertAtEnd("must");	
	m_AlphaList.InsertAtEnd("my");	
	m_AlphaList.InsertAtEnd("new");	
	m_AlphaList.InsertAtEnd("no");	
	m_AlphaList.InsertAtEnd("not");	
	m_AlphaList.InsertAtEnd("now");	
	m_AlphaList.InsertAtEnd("of");	
	m_AlphaList.InsertAtEnd("off");	
	m_AlphaList.InsertAtEnd("old");	
	m_AlphaList.InsertAtEnd("on");
	m_AlphaList.InsertAtEnd("one");	
	m_AlphaList.InsertAtEnd("only");	
	m_AlphaList.InsertAtEnd("or");	
	m_AlphaList.InsertAtEnd("other");	
	m_AlphaList.InsertAtEnd("our");	
	m_AlphaList.InsertAtEnd("out");	
	m_AlphaList.InsertAtEnd("over");	
	m_AlphaList.InsertAtEnd("right");
	m_AlphaList.InsertAtEnd("said");		
	m_AlphaList.InsertAtEnd("see");	
	m_AlphaList.InsertAtEnd("she");	
	m_AlphaList.InsertAtEnd("so");	
	m_AlphaList.InsertAtEnd("some");	
	m_AlphaList.InsertAtEnd("that");
	m_AlphaList.InsertAtEnd("the");	
	m_AlphaList.InsertAtEnd("their");	
	m_AlphaList.InsertAtEnd("them");	
	m_AlphaList.InsertAtEnd("then");	
	m_AlphaList.InsertAtEnd("there");	
	m_AlphaList.InsertAtEnd("they");
	m_AlphaList.InsertAtEnd("this");	
	m_AlphaList.InsertAtEnd("to");
	m_AlphaList.InsertAtEnd("two");	
	m_AlphaList.InsertAtEnd("up");
	m_AlphaList.InsertAtEnd("want");	
	m_AlphaList.InsertAtEnd("was");	
	m_AlphaList.InsertAtEnd("we");	
	m_AlphaList.InsertAtEnd("well");	
	m_AlphaList.InsertAtEnd("went");	
	m_AlphaList.InsertAtEnd("were");	
	m_AlphaList.InsertAtEnd("what");	
	m_AlphaList.InsertAtEnd("when");	
	m_AlphaList.InsertAtEnd("where");
	m_AlphaList.InsertAtEnd("which");	
	m_AlphaList.InsertAtEnd("who");	
	m_AlphaList.InsertAtEnd("will");	
	m_AlphaList.InsertAtEnd("with");
	m_AlphaList.InsertAtEnd("yard");
	m_AlphaList.InsertAtEnd("yarn");	
	m_AlphaList.InsertAtEnd("year");
	m_AlphaList.InsertAtEnd("yearly");	
	m_AlphaList.InsertAtEnd("yellow");	
	m_AlphaList.InsertAtEnd("yesterday");
	m_AlphaList.InsertAtEnd("yield");	
	m_AlphaList.InsertAtEnd("York");	
	m_AlphaList.InsertAtEnd("young");	
	m_AlphaList.InsertAtEnd("your");	
	m_AlphaList.InsertAtEnd("yours");	
	m_AlphaList.InsertAtEnd("yourself");	
	m_AlphaList.InsertAtEnd("youth");	
	m_AlphaList.InsertAtEnd("zebra");	
	m_AlphaList.InsertAtEnd("zero");	
	m_AlphaList.InsertAtEnd("zone");	
#endif

	m_AlphaList.m_bSorted = TRUE;

	return TRUE;
}

BOOL CWordAid2Dlg::LoadDemoAbbrevList()
{
	m_AbbrevList.RemoveAll();

#ifdef _SWED_DEMO

		m_AbbrevList.InsertAtEnd("ac", "ACE Centre");
	m_AbbrevList.InsertAtEnd("kru", "Kruthusgatan");
	m_AbbrevList.InsertAtEnd("SIT", "Specialpedagogiska institutet");
#else
	m_AbbrevList.InsertAtEnd("ACE", "ACE Centre Advisory Trust");
	m_AbbrevList.InsertAtEnd("92", "92 Windmill Road,<CR>Headington,<CR>Oxford,<CR>OX3 7DR<CR>");
	m_AbbrevList.InsertAtEnd("RNIB", "Royal National Institute for the Blind");
	m_AbbrevList.InsertAtEnd("RNID", "Royal Natioanl Institute for the Deaf");
	m_AbbrevList.InsertAtEnd("AA", "Alcoholics Anonamous");
#endif

	return TRUE;
}

#endif