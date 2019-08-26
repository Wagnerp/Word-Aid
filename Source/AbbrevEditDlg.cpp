// AbbrevEditDlg.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////////////////////
//																					//
//	WA0003 - changes to fix problem with Abbrev dialogue boxes displaying off		//
//			screen.																	//
//																					//
//	WA0013 - Fixes the problem with the say buttons disabling when they should be	//
//			 be enabled.															//
//////////////////////////////////////////////////////////////////////////////////////
//																					//

#include "stdafx.h"
#include "WordAid2.h"
#include "AbbrevEditDlg.h"


// CAbbrevEditDlg dialog

IMPLEMENT_DYNAMIC(CAbbrevEditDlg, CDialog)
CAbbrevEditDlg::CAbbrevEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAbbrevEditDlg::IDD, pParent)
	, m_strAbbreviation(_T(""))
	, m_strExpansion(_T(""))
	, m_bEditMode(FALSE)
	, m_pAbbrevList(NULL)
{
	m_bVisible = FALSE;		//WA0003
}

CAbbrevEditDlg::~CAbbrevEditDlg()
{
	if (m_ctrlSpeaker)
	{
		m_ctrlSpeaker.DestroyWindow();
	}
}

void CAbbrevEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_ABBREV, m_edtAbbreviation);
	DDX_Control(pDX, IDC_EDT_EXPAN, m_edtExpansion);
	DDX_Text(pDX, IDC_EDT_ABBREV, m_strAbbreviation);
	DDX_Text(pDX, IDC_EDT_EXPAN, m_strExpansion);
	DDX_Control(pDX, IDD_BTN_ADD, m_btnAdd);
	DDX_Control(pDX, IDCANCEL, m_btnFinish);
	DDX_Control(pDX, IDC_BTN_SAY_ABBREV, m_btnSayAbbrev);
	DDX_Control(pDX, IDC_BTN_SAY_EXPAN, m_btnSayExpan);
	DDX_Control(pDX, IDC_BTN_LEXICON, m_btnLexicon);
}


BEGIN_MESSAGE_MAP(CAbbrevEditDlg, CDialog)
	ON_BN_CLICKED(IDD_BTN_ADD, OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_SAY_ABBREV, OnBtnSayAbbrev)
	ON_BN_CLICKED(IDC_BTN_SAY_EXPAN, OnBtnSayExpan)
	ON_BN_CLICKED(IDC_BTN_LEXICON, OnBtnLexicon)
	ON_EN_CHANGE(IDC_EDT_ABBREV, OnEditChangeAbbrev)
	ON_EN_CHANGE(IDC_EDT_EXPAN, OnEditChangeExpan)
END_MESSAGE_MAP()


// CAbbrevEditDlg message handlers

BOOL CAbbrevEditDlg::OnInitDialog() 
{
	m_ctrlSpeaker.Create("Speaker",WS_CHILD,CRect(0,0,1,1),this,2);

	CDialog::OnInitDialog();

	//WA0047 - Make sure the Dialog does not display on top of the main window
	PositionDialogOffMainWindow();

	if(m_ctrlSpeaker)
	{
		m_ctrlSpeaker.RestoreVoice("WORDAID");
	}
	else
	{
		m_btnLexicon.EnableWindow(FALSE);
	}

	CString szTitle = _T("");

	// Set the dialogs title
	if(m_bEditMode)
	{
		CString strOk = _T("");
		CString strCancel = _T("");

		strOk.LoadString(IDS_BTN_OK);
		strCancel.LoadString(IDS_BTN_CANCEL);

		m_btnAdd.SetWindowText(strOk);
		m_btnFinish.SetWindowText(strCancel);

		szTitle.LoadString(IDS_ABBREV_EDIT_TXT);
	}
	else
	{
		szTitle.LoadString(IDS_ABBREV_ADD_TXT);

		m_btnSayAbbrev.EnableWindow(FALSE);
		m_btnSayExpan.EnableWindow(FALSE);
	}

	SetWindowText(szTitle);

	m_bVisible = FALSE;	//WA0003


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////////////////////////////////////////////////////////////////////

void CAbbrevEditDlg::OnBnClickedBtnAdd()
{
	if(m_bEditMode)
	{
		CDialog::OnOK();
	}
	else
	{
		UpdateData(TRUE);	// Get abbreviation and expansion

		if(m_strAbbreviation == _T(""))	// No abbreviation do not save
		{
			CString strError = _T("");
			strError.LoadString(IDS_ERR_NO_ABBREVIATION_ENTERED);

			::MessageBox(this->m_hWnd, strError, _T(""), MB_OK|MB_ICONEXCLAMATION);
		}
		else if(m_strExpansion == _T(""))	// No expansion request whether they want to save
		{
			CString strError = _T("");
			strError.LoadString(IDS_ERR_NO_EXPANSION_ENTERED);

			if(::MessageBox(m_hWnd, strError, _T(""), MB_YESNO|MB_ICONQUESTION) == IDYES)
			{
				m_pAbbrevList->InsertAtEnd(m_strAbbreviation, m_strExpansion);
			}
		}
		else	// Save
		{
			m_pAbbrevList->InsertAtEnd(m_strAbbreviation, m_strExpansion);
		}

		// Redisplay list in main window
		CWordAid2App* pApp = (CWordAid2App*)AfxGetApp();
		CWordAid2Dlg* pWordAidDlg = pApp->GetWordAidWindow();
		pWordAidDlg->DisplayAbbrevList();

		// Clear controls of text
		m_edtAbbreviation.SetWindowText(_T(""));
		m_edtExpansion.SetWindowText(_T(""));
	}
}

////////////////////////////////////////////////////////////////////////////////

void CAbbrevEditDlg::OnBnClickedCancel()
{
	if(m_bEditMode)
	{
		OnCancel();
	}
	else	// Add word and then hide dialog
	{
		UpdateData(TRUE);

		if(m_strAbbreviation != _T("") || m_strExpansion != _T(""))
		{
			m_pAbbrevList->InsertAtEnd(m_strAbbreviation, m_strExpansion);
		}

		// Redisplay list in main window
		CWordAid2App* pApp = (CWordAid2App*)AfxGetApp();
		CWordAid2Dlg* pWordAidDlg = pApp->GetWordAidWindow();
		pWordAidDlg->DisplayAbbrevList();

		if(m_pAbbrevList->m_bModified)
		{
			if(AfxMessageBox(IDS_SAVE_CHANGES_ABBREV, MB_YESNO) == IDYES)
			{
				pWordAidDlg->Save();
				m_pAbbrevList->SetModified(FALSE);
			}
			else
			{
				pWordAidDlg->SetWindowTitle(pWordAidDlg->GetAbbrevFilename(), FALSE);
			}
		}

		//ShowWindow(SW_HIDE);		//WA0047
		m_bVisible = FALSE;	//WA0003

		// Cancel Stay active for wordAid window now add or edting has finished.
		pWordAidDlg->StayActive(FALSE);

		OnCancel();		//WA0047

	}
}

void CAbbrevEditDlg::OnBtnSayAbbrev()
{
	UpdateData(TRUE);

	m_ctrlSpeaker.Speak(m_strAbbreviation);
}

void CAbbrevEditDlg::OnBtnSayExpan()
{
	UpdateData(TRUE);

	m_ctrlSpeaker.Speak(m_strExpansion);
}

void CAbbrevEditDlg::OnBtnLexicon()
{
	m_ctrlSpeaker.ShowLexiconDialog((LONG)m_hWnd);
}

void CAbbrevEditDlg::OnEditChangeAbbrev()
{
	UpdateData(TRUE);

	if(m_strAbbreviation != _T(""))
	{
		if(m_ctrlSpeaker)
		{
			// WA0013
			m_btnSayAbbrev.EnableWindow(TRUE);		
		}
	}
	else
	{
		// WA0013
		m_btnSayAbbrev.EnableWindow(FALSE);
		
	}
}

void CAbbrevEditDlg::OnEditChangeExpan()
{
	// WA0013
	UpdateData(TRUE);

	if(m_strExpansion != _T(""))
	{
		if(m_ctrlSpeaker)
		{
			// WA0013
			m_btnSayExpan.EnableWindow(TRUE);
		}
	}
	else
	{
		// WA0013
		m_btnSayExpan.EnableWindow(FALSE);
	}
}

void CAbbrevEditDlg::OnCancel()
{
	//DestroyWindow();
//	this->EndDialog(IDCANCEL);  // WA0028
	CDialog::OnCancel();		// WA0047
}

void CAbbrevEditDlg::PostNcDestroy()
{
	//delete this;				// WA0028
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Moves the dialog so that it is displayed off an applications
//				: main window.  It first tries to display it above the window 
//				: if there is room; otherwise it displays it below.
//
// RETURNS		: The new dialog rectangle if successful; otherwise an empty
//				: CRect(0,0,0,0)
////////////////////////////////////////////////////////////////////////////////

//WA0047
CRect CAbbrevEditDlg::PositionDialogOffMainWindow()
{
	CWordAid2App* pApp = (CWordAid2App*)AfxGetApp();
	
	CWnd* pMainWnd = AfxGetMainWnd();

	CRect rectMain,
		  rectDlg,
		  rectRet;
	
	// Get Screen position for main window and the dialog
	CWordAid2Dlg* pWordAidDlg = pApp->GetWordAidWindow();
	pWordAidDlg->GetWindowRect(rectMain);

	//pMainWnd->GetWindowRect(rectMain);
	this->GetWindowRect(rectDlg);

	// Get Height of Screen
	int nScreenY = GetSystemMetrics(SM_CYSCREEN);

	// Can Dialog be shown Above main window
	int nMainTop = rectMain.top;
	int nDialogHeight = rectDlg.Height();

//WA0003	if(nMainTop + nDialogHeight < nScreenY)		// Above window
	if(nMainTop - nDialogHeight > 10)		// Above window	//WA0003
	{
		rectRet = CRect(rectDlg.left, rectMain.top - rectDlg.Height(), rectDlg.right, rectMain.top);
		this->MoveWindow(rectRet.left, rectRet.top, rectRet.Width(), rectRet.Height(), TRUE);//rectRet);
	}
//WA0003else	// Below Window
	else if (rectMain.bottom + nDialogHeight < (nScreenY-20))	// Below Window	//WA0003
	{
//WA0003rectRet = CRect(rectDlg.left, rectMain.bottom, rectDlg.right, rectMain.bottom - rectDlg.Height());	//WA0003
		rectRet = CRect(rectDlg.left, rectMain.bottom, rectDlg.right, rectMain.bottom + rectDlg.Height());
		this->MoveWindow(rectRet.left, rectRet.top, rectRet.Width(), rectRet.Height(), TRUE);
	}
	else							//WA0003
		this->MoveWindow(0,0);	//WA0003

	return rectRet;
}