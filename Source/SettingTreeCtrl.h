#pragma once


// CSettingTreeCtrl

class CSettingTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CSettingTreeCtrl)

public:
	CSettingTreeCtrl();
	virtual ~CSettingTreeCtrl();

	CImageList m_ImageList;

	HTREEITEM Add(CString szItem, int nLevel);

protected:
	// Attributes for Setting Tree
	int m_nCount;

	DECLARE_MESSAGE_MAP()
};


