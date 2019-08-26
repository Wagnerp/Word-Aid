#if !defined(AFX_ADVANCEDKEYDLG_H__22913D1F_DF5D_4886_96F9_59A1F3250030__INCLUDED_)
#define AFX_ADVANCEDKEYDLG_H__22913D1F_DF5D_4886_96F9_59A1F3250030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvancedKeyDlg.h : header file
//

#include "Setting.h"
#include "KeyEdit.h"

#include "WordAidTreeDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CAdvancedKeyDlg dialog

class CAdvancedKeyDlg : public CDialog
{
// Construction
public:
	CAdvancedKeyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdvancedKeyDlg)
	enum { IDD = IDD_ADVANCED_KEYS_DLG };
	CListBox	m_lstKeys;
	CKeyEdit	m_edtNew;
	CEdit	m_edtDescription;
	CKeyEdit	m_edtCurrent;
	CComboBox	m_cmbUsedBy;
	CButton	m_btnAsign;
	//}}AFX_DATA

	void SetSettings(CSetting* pSetting) {m_pSetting = pSetting;}

	CWordAidTreeDialog* m_pWordAidSetting;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvancedKeyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	struct SHORTCUT_INFO
	{
		CString strName;			// Name identifying shortcut
		CString strDescription;		// Description of short cut
		int nVK;					// Short cut virtual key code
		CString strSpec;			// Short cut special key
	};

	CArray<SHORTCUT_INFO, SHORTCUT_INFO&> m_arrShortCuts;

	CSetting* m_pSetting;

	void LoadKeys();
	void SetUsedBy(KEY_TYPE type);

	// Generated message map functions
	//{{AFX_MSG(CAdvancedKeyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLstKeys();
	afx_msg void OnChangeEdtNew();
	afx_msg void OnBtnAssign();
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg void OnBtnDefault();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVANCEDKEYDLG_H__22913D1F_DF5D_4886_96F9_59A1F3250030__INCLUDED_)
