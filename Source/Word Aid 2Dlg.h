// Word Aid 2Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// WA0010 - Change list box so that it allows for the list to be searched
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORDAID2DLG_H__E212C0C7_23DE_11D6_92B8_A495EED9BA71__INCLUDED_)
#define AFX_WORDAID2DLG_H__E212C0C7_23DE_11D6_92B8_A495EED9BA71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Setting.h"		// Class to operate on settings
#include "WordList.h"		// Class Wordlist
#include "DlgToolbar.h"		// ToolBar for the dialog
#include "textspeaker.h"				// Added by ClassView

#include "SearchListBox.h"	// For Searchable list box		// WA0010
/////////////////////////////////////////////////////////////////////////////
// CWordAid2Dlg dialog

class CWordAid2Dlg : public CDialog
{
// Construction
public:
	CWordAid2Dlg(CWnd* pParent = NULL);	// standard constructor
	~CWordAid2Dlg();

	void StayActive(BOOL bStayActive = TRUE) {m_bStayActive = bStayActive;}
	CWordlist m_AlphaList;			// Alphabetic wordlist
	CWordlist m_TopicList;			// Topic wordlist
	//WA0047
	//CAbbrevList* m_AbbrevList;		// Abbrev list
	CAbbrevList m_AbbrevList;		// Abbrev list

	CSetting m_Settings;

	void SetWindowTitle(CString strFilePath, BOOL bSaved = TRUE);

	CString GetAbbrevFilename() {return m_szAbbrevFilename;}

	// Methods to display wordlist
	void DisplayAlphaList();
	void DisplayTopicList();
	void DisplayAbbrevList();

	void Save() {OnFileSave();}

	void SetAlphaFileName(CString strFilename)  
	{m_szAlphaFilename = strFilename;m_Settings.SetSetting(ALPHA_FILENAME, strFilename);}
	void SetAbbrevFileName(CString strFilename)  
	{m_szAbbrevFilename = strFilename;m_Settings.SetSetting(ABBREV_FILENAME, strFilename);}
	void SetTopicFileName(CString strFilename)  
	{m_szTopicFilename = strFilename;m_Settings.SetSetting(TOPIC_FILENAME, strFilename);}

// Dialog Data
	//{{AFX_DATA(CWordAid2Dlg)
	enum { IDD = IDD_WORDAID2_DIALOG };
	CSearchListBox	m_wordlist;			// WA0010
	CTabCtrl	m_tab_wordlist;
	//}}AFX_DATA
//	CImageList* m_pTabImageList;

	CButton m_chkHideList;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWordAid2Dlg)
	public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	BOOL m_bStayActive;
	BOOL m_bNewFile;

	HACCEL m_hAccelerator;

	CDlgToolBar m_toolBar;			// Toolbar control

	CString m_szAlphaFilename;		// Filename for Alphabetic Wordlist
	CString m_szTopicFilename;		// Filename for Topic Wordlist
	CString m_szAbbrevFilename;		// Filename for the Abbrev List

	CTextSpeaker m_ctrlSpeaker;		// Text Speaker for lexicon

	// Methods to Initialise the Tab control
	BOOL AddTabs();
	void AddWordAidMenus();
	void AddAbbrevMenuItems();
	void RemoveAbbrevMenuItem();
	void RemoveWordAidMenuItems();
	void SetWordAidEditText(BOOL bWordAid = TRUE);
	BOOL InsertInMenu(CMenu* pMenu, CString szText, UINT nID, UINT nPos);
	CMenu* GetSubMenu(UINT nFirstID);
	void RemoveMenuItem(CMenu* pMenu, UINT nID);
	void ModifyMenuText(CMenu* pMenu, UINT nID, CString strMenuText);
	BOOL SelectTab(CString strTabName);

	// Methods to add to wordlist
	BOOL SaveFile(int nType, CString& szFilename, CString szExt, CWordlist& list);
	//BOOL SaveFile(int nType, CString& szFilename, CString szExt, CAbbrevList* list);	WA0047
	BOOL SaveFile(int nType, CString& szFilename, CString szExt, CAbbrevList list);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

#ifdef _DEMO
	BOOL LoadDemoTopicList();
	BOOL LoadDemoAlphaList();
	BOOL LoadDemoAbbrevList();
#ifdef _SWED_DEMO
	void LoadList1();
	void LoadList2();
	void LoadList3();
	void LoadList4();
	void LoadList5();
#endif
#endif

	// Generated message map functions
	//{{AFX_MSG(CWordAid2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnFileOpenalphabetic();
	afx_msg void OnFileOpentopic();
	afx_msg void OnFileOpen();
	afx_msg void OnSelchangeTabWordlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditAddword();
	afx_msg void OnFileSaveas();
	afx_msg void OnFileSave();
	afx_msg void OnEditEditword();
	afx_msg void OnBtnSortAlpha();
	afx_msg void OnSettingsPopupsettings();
	afx_msg void OnEditDeleteword();
	afx_msg void OnSavesettings();
	afx_msg void OnLoadsettings();
	afx_msg void OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized );
	afx_msg void OnFileExit();
	afx_msg void OnFileNewwordlist();
	afx_msg void OnAppAbout();
	afx_msg void OnDefaultHelp();
	afx_msg void OnMergeFiles();
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	afx_msg int OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex);
	afx_msg void OnSettingsToolbar();
	afx_msg void OnUpdateSettingsToolbar(CCmdUI* pCmdUI);
	afx_msg void OnDblclkListWordlist();
	afx_msg void OnSettingsLexicon();
	//}}AFX_MSG
	afx_msg void OnFileOpenAbbrev();
	afx_msg void OnClickHideList();
	afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnSettingsToolbar();
//	afx_msg void OnUpdateSettingsToolbar(CCmdUI *pCmdUI);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORDAID2DLG_H__E212C0C7_23DE_11D6_92B8_A495EED9BA71__INCLUDED_)
