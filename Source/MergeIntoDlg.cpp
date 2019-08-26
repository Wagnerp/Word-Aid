// MergeIntoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wordaid2.h"
#include "MergeIntoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeIntoDlg property page

IMPLEMENT_DYNCREATE(CMergeIntoDlg, CPropertyPage)

CMergeIntoDlg::CMergeIntoDlg() : CPropertyPage(CMergeIntoDlg::IDD)
{
	//{{AFX_DATA_INIT(CMergeIntoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CMergeIntoDlg::~CMergeIntoDlg()
{
}

void CMergeIntoDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMergeIntoDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMergeIntoDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CMergeIntoDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeIntoDlg message handlers

BOOL CMergeIntoDlg::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*) GetParent();

	pSheet->SetWizardButtons(PSWIZB_NEXT);

	return CPropertyPage::OnSetActive();
}
