// DummyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wordaid2.h"
#include "DummyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDummyDlg dialog


CDummyDlg::CDummyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDummyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDummyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDummyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDummyDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDummyDlg, CDialog)
	//{{AFX_MSG_MAP(CDummyDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDummyDlg message handlers

BOOL CDummyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Resize to nothing
	SetWindowPos(&wndTop, 0, 0, 0, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
