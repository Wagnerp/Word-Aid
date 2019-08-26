// MergeFileSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wordaid2.h"
#include "MergeFileSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeFileSelectDlg property page

IMPLEMENT_DYNCREATE(CMergeFileSelectDlg, CPropertyPage)

CMergeFileSelectDlg::CMergeFileSelectDlg() : CPropertyPage(CMergeFileSelectDlg::IDD)
{
	//{{AFX_DATA_INIT(CMergeFileSelectDlg)
	m_strOutputFilename = _T("");
	m_strMainFilename = _T("");
	m_strMergeFilename = _T("");
	//}}AFX_DATA_INIT
}

CMergeFileSelectDlg::~CMergeFileSelectDlg()
{
}

void CMergeFileSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMergeFileSelectDlg)
	DDX_Text(pDX, IDC_EDIT_FILE_OUT, m_strOutputFilename);
	DDX_Text(pDX, IDC_EDIT_FILE1, m_strMainFilename);
	DDX_Text(pDX, IDC_EDIT_FILE2, m_strMergeFilename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMergeFileSelectDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CMergeFileSelectDlg)
	ON_BN_CLICKED(IDC_BTN_BROWSE_FILE1, OnBtnBrowseFile1)
	ON_BN_CLICKED(IDC_BTN_BROWSE_FILE2, OnBtnBrowseFile2)
	ON_BN_CLICKED(IDC_BTN_BROWSE_OUTPUT, OnBtnBrowseOutput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeFileSelectDlg message handlers

BOOL CMergeFileSelectDlg::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*) GetParent();

	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	return CPropertyPage::OnSetActive();
}

void CMergeFileSelectDlg::OnBtnBrowseFile1() 
{
	// TODO: Add your control notification handler code here
	CString szFileType = _T("");
	CString szFilename = _T("");

	szFileType.LoadString(IDS_FILETYPE_ALL);
	
	CFileDialog fileDlg(TRUE, szFileType, m_strMainFilename, 0, szFileType);
	
	if (fileDlg.DoModal() == IDOK)
	{
		CString szFilename = fileDlg.GetPathName();
		CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT_FILE1);
		edit->SetWindowText(szFilename);
		m_strMainFilename = szFilename;
	}
}

void CMergeFileSelectDlg::OnBtnBrowseFile2() 
{
	// TODO: Add your control notification handler code here
	CString szFileType = _T("");
	CString szFilename = _T("");

	szFileType.LoadString(IDS_FILETYPE_ALL);
	
	CFileDialog fileDlg(TRUE, szFileType, m_strMergeFilename, 0, szFileType);
	
	if (fileDlg.DoModal() == IDOK)
	{
		CString szFilename = fileDlg.GetPathName();
		CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT_FILE2);
		edit->SetWindowText(szFilename);
		m_strMergeFilename = szFilename;
	}
	
}

void CMergeFileSelectDlg::OnBtnBrowseOutput() 
{
	// TODO: Add your control notification handler code here
	CString szFileType = _T("");
	CString szFilename = _T("");

	szFileType.LoadString(IDS_FILETYPE_ALL);
	
	CFileDialog fileDlg(TRUE, szFileType, m_strOutputFilename, 0, szFileType);
	
	if (fileDlg.DoModal() == IDOK)
	{
		CString szFilename = fileDlg.GetPathName();
		CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT_FILE_OUT);
		edit->SetWindowText(szFilename);
		m_strOutputFilename = szFilename;
	}
	
}

void CMergeFileSelectDlg::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnOK();
}
