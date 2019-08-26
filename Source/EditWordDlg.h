#if !defined(AFX_EDITWORDDLG_H__953333C3_F720_4479_8A55_0D3DC250BB60__INCLUDED_)
#define AFX_EDITWORDDLG_H__953333C3_F720_4479_8A55_0D3DC250BB60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditWordDlg.h : header file
//
#include "resource.h"
#include "Word Aid 2Dlg.h"
#include "Wordlist.h"

/////////////////////////////////////////////////////////////////////////////
// CEditWordDlg dialog

class CEditWordDlg : public CDialog
{
// Construction
public:
	CEditWordDlg(CWnd* pParent = NULL);   // standard constructor
	~CEditWordDlg();

	CWordlist* wordlist;
	int m_nPosInList;

// Dialog Data
	//{{AFX_DATA(CEditWordDlg)
	enum { IDD = IDD_EDITWORD_DLG };
	CString	m_editWord;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditWordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CTextSpeaker m_ctrlSpeaker;		// Text Speaker for lexicon

	CButton m_btnSay;
	CButton m_btnLexicon;

	// Generated message map functions
	//{{AFX_MSG(CEditWordDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnSay();
	afx_msg void OnBtnLexicon();
	afx_msg void OnEditChangeWord();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITWORDDLG_H__953333C3_F720_4479_8A55_0D3DC250BB60__INCLUDED_)
