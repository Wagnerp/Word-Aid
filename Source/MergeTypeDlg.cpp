// MergeTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wordaid2.h"
#include "MergeTypeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeTypeDlg property page

IMPLEMENT_DYNCREATE(CMergeTypeDlg, CPropertyPage)

CMergeTypeDlg::CMergeTypeDlg() : CPropertyPage(CMergeTypeDlg::IDD)
{
	//{{AFX_DATA_INIT(CMergeTypeDlg)
//	m_radMergeAll.SetCheck(1);
	m_bMergeAll = TRUE;
	m_bMergeSelected = FALSE;

		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CMergeTypeDlg::~CMergeTypeDlg()
{
}

void CMergeTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMergeTypeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_RADIO_MERGE_ALL, m_radMergeAll);
	DDX_Control(pDX, IDC_RADIO_MERGE_SELECTED, m_radMergeSelected);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMergeTypeDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CMergeTypeDlg)
	ON_BN_CLICKED(IDC_RADIO_MERGE_ALL, OnRadioMergeAll)
	ON_BN_CLICKED(IDC_RADIO_MERGE_SELECTED, OnRadioMergeSelected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeTypeDlg message handlers

BOOL CMergeTypeDlg::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*) GetParent();

	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);	
	m_radMergeAll.SetCheck(m_bMergeAll);
	m_radMergeSelected.SetCheck(m_bMergeSelected);
	
	return CPropertyPage::OnSetActive();
}

void CMergeTypeDlg::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnOK();
}

void CMergeTypeDlg::OnRadioMergeAll() 
{
	// TODO: Add your control notification handler code here
	CPropertySheet* pSheet = (CPropertySheet*) GetParent();

	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

	m_bMergeAll = TRUE;
	m_bMergeSelected = FALSE;
}

void CMergeTypeDlg::OnRadioMergeSelected() 
{
	// TODO: Add your control notification handler code here
	CPropertySheet* pSheet = (CPropertySheet*) GetParent();

	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	m_bMergeSelected = TRUE;
	m_bMergeAll = FALSE;
}
