// DlgToolBar.cpp : implementation file
//

#include "stdafx.h"
#include <afxpriv.h>
#include "DlgToolBar.h"


/////////////////////////////////////////////////////////////////////////////
// CDlgToolBar

BEGIN_MESSAGE_MAP(CDlgToolBar, CToolBar)
	//{{AFX_MSG_MAP(CDlgToolBar)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify )
	// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgToolBar Construction/Destruction

CDlgToolBar::CDlgToolBar()
{
}

CDlgToolBar::~CDlgToolBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDlgToolBar::OnIdleUpdateCmdUI
//      OnIdleUpdateCmdUI handles the WM_IDLEUPDATECMDUI message, which is
//      used to update the status of user-interface elements within the MFC
//      framework.
//
//      We have to get a little tricky here: CToolBar::OnUpdateCmdUI
//      expects a CFrameWnd pointer as its first parameter.  However, it
//      doesn't do anything but pass the parameter on to another function
//      which only requires a CCmdTarget pointer.  We can get a CWnd pointer
//      to the parent window, which is a CCmdTarget, but may not be a
//      CFrameWnd.  So, to make CToolBar::OnUpdateCmdUI happy, we will call
//      our CWnd pointer a CFrameWnd pointer temporarily.

LRESULT CDlgToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
	if (IsWindowVisible())
	{
		CFrameWnd *pParent = (CFrameWnd *)GetParent();
		if (pParent)
			OnUpdateCmdUI(pParent, (BOOL)wParam);
	}
	return 0L;
}

BOOL CDlgToolBar::OnToolTipNotify(UINT id, NMHDR* pNMHDR,LRESULT* pResult)
{
		BOOL bResult = TRUE;

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT nID =pNMHDR->idFrom;

    if(nID)
    {
		pTTT->lpszText = MAKEINTRESOURCE(nID);//ID_FILE_NEWWORDLIST);
		pTTT->hinst = AfxGetResourceHandle();
    }
	else
	{
		bResult = FALSE;
	}

	return bResult;
}