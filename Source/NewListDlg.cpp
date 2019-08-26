// NewListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NewListDlg.h"

#include "Wordlist.h"	// For list types

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewListDlg dialog


CNewListDlg::CNewListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bAbbrevOn		= FALSE;
	m_bWordAidOn	= FALSE;
	m_nListType		= ALPHA;
}


void CNewListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewListDlg)
	DDX_Control(pDX, IDC_LIST_TYPE, m_lstTypes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewListDlg, CDialog)
	//{{AFX_MSG_MAP(CNewListDlg)
	ON_LBN_DBLCLK(IDC_LIST_TYPE, OnDblclkListType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewListDlg message handlers

void CNewListDlg::OnDblclkListType() 
{
	OnOK();
	
}

BOOL CNewListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString strAbbrev	= _T("");
	CString strAlpha	= _T("");
	CString strTopic	= _T("");
	int nIndex			= 0;

	if(m_bAbbrevOn)
	{
		strAbbrev.LoadString(IDS_NEW_ABBREV_LIST);
		nIndex = m_lstTypes.AddString(strAbbrev);
		m_lstTypes.SetItemData(nIndex, ABBREV);
	}

	if(m_bWordAidOn)
	{
		strAlpha.LoadString(IDS_NEW_ALPHA_LIST);
		nIndex = m_lstTypes.AddString(strAlpha);
		m_lstTypes.SetItemData(nIndex, ALPHA);
	
		strTopic.LoadString(IDS_NEW_TOPIC_LIST);
		nIndex = m_lstTypes.AddString(strTopic);
		m_lstTypes.SetItemData(nIndex, TOPIC);
	}
	m_lstTypes.SetCurSel(0);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewListDlg::OnOK() 
{
	m_nListType = m_lstTypes.GetItemData(m_lstTypes.GetCurSel());
	
	CDialog::OnOK();
}
