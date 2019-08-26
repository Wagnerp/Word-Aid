// AddDelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wordaid2.h"
#include "AddDelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddDelDlg dialog


CAddDelDlg::CAddDelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddDelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddDelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAddDelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddDelDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddDelDlg, CDialog)
	//{{AFX_MSG_MAP(CAddDelDlg)
	ON_BN_CLICKED(IDC_CHK_TOPIC, OnChkTopic)
	ON_BN_CLICKED(IDC_CHK_ALPHA, OnChkAlpha)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CAddDelDlg::GetType()
{
	return m_nType;
}
/////////////////////////////////////////////////////////////////////////////
// CAddDelDlg message handlers

void CAddDelDlg::OnChkTopic() 
{
	// TODO: Add your control notification handler code here
	m_nType = IDC_CHK_TOPIC;
	OnOK();
}

void CAddDelDlg::OnChkAlpha() 
{
	// TODO: Add your control notification handler code here
	m_nType = IDC_CHK_ALPHA;
	OnOK();
}

void CAddDelDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CAddDelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CStatic* text = (CStatic*)GetDlgItem(IDC_STATIC_5);
	CString szTitle = _T("");
		
	// TODO: Add extra initialization here
	if (m_bAdd)
		szTitle.LoadString(IDS_INSERT_WORD);
	else
		szTitle.LoadString(IDS_DELETE_WORD);

	text->SetWindowText(szTitle);

	CButton* button = (CButton*)GetDlgItem(IDC_CHK_ALPHA);
	szTitle.LoadString(IDS_ALPHA_WORDLIST);
	button->SetWindowText(szTitle);

	button = (CButton*)GetDlgItem(IDC_CHK_TOPIC);
	szTitle.LoadString(IDS_TOPIC_WORDLIST);
	button->SetWindowText(szTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
