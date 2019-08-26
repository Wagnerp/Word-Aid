// SettingTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "WordAid2.h"
#include "SettingTreeCtrl.h"


// CSettingTreeCtrl

IMPLEMENT_DYNAMIC(CSettingTreeCtrl, CTreeCtrl)
CSettingTreeCtrl::CSettingTreeCtrl()
{
	m_nCount = 0;
}

CSettingTreeCtrl::~CSettingTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CSettingTreeCtrl, CTreeCtrl)
END_MESSAGE_MAP()



// CSettingTreeCtrl message handlers

HTREEITEM CSettingTreeCtrl::Add(CString szItem, int nLevel)
{
	TV_INSERTSTRUCT tv_is;		// Used to add item to tree
	static HTREEITEM  hRoot = NULL;
	static HTREEITEM  hNextLevel = NULL;
	static int nPrevLevel = 0;


	SetIndent(10);
	if(nLevel == 0)
	{
		tv_is.hParent = TVI_ROOT;
		tv_is.hInsertAfter = TVI_LAST;
		tv_is.item.mask = TVIF_TEXT | TVIF_STATE;
		tv_is.item.stateMask = TVIS_STATEIMAGEMASK;
		tv_is.item.state = INDEXTOSTATEIMAGEMASK(1);
		tv_is.item.pszText = szItem.GetBuffer(1);
		tv_is.item.cchTextMax = szItem.GetLength();

		hNextLevel = hRoot = InsertItem(&tv_is);
		SetItemData(hRoot, m_nCount);
		m_nCount++;

		szItem.ReleaseBuffer();

		SetItemState(hRoot, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK);

		nPrevLevel = 0;
	}
	else if(nLevel > nPrevLevel)
	{
		SetIndent(30);
		tv_is.hParent = hNextLevel;
		tv_is.hInsertAfter = TVI_LAST;
		tv_is.item.mask = TVIF_TEXT | TVIF_STATE;
		tv_is.item.stateMask = TVIS_STATEIMAGEMASK;
		tv_is.item.state = INDEXTOSTATEIMAGEMASK(1);
		tv_is.item.pszText = szItem.GetBuffer(1);
		tv_is.item.cchTextMax = szItem.GetLength();

		 hNextLevel = InsertItem(&tv_is);

		SetItemData(hNextLevel, m_nCount);
		SetItemState(hNextLevel, INDEXTOSTATEIMAGEMASK(0), TVIS_STATEIMAGEMASK);
		m_nCount++;
		nPrevLevel = nLevel;
	}
	else
	{
		tv_is.hParent = hRoot;
		tv_is.hInsertAfter = TVI_LAST;
		tv_is.item.mask = TVIF_TEXT | TVIF_STATE;
		tv_is.item.stateMask = TVIS_STATEIMAGEMASK;
		tv_is.item.state = INDEXTOSTATEIMAGEMASK(1);
		tv_is.item.pszText = szItem.GetBuffer(1);
		tv_is.item.cchTextMax = szItem.GetLength();

		hNextLevel = InsertItem(&tv_is);

		SetItemData(hNextLevel, m_nCount);
		SetItemState(hNextLevel, INDEXTOSTATEIMAGEMASK(0), TVIS_STATEIMAGEMASK);
		m_nCount++;
		nPrevLevel = nLevel;
	}

	return hNextLevel;
}



