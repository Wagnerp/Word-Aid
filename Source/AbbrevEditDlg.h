#pragma once
//////////////////////////////////////////////////////////////////////////////////////
//																					//
//	WA0003 - changes to fix problem with Abbrev dialogue boxes displaying off		//
//			screen.																	//
//																					//
//////////////////////////////////////////////////////////////////////////////////////
//																					//

#include "stdafx.h"
#include "afxwin.h"

#include "Wordlist.h"		// For CAbbrevList
#include "resource.h"
#include "TextSpeaker.h"
class CAbbrevList;
// CAbbrevEditDlg dialog

class CAbbrevEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CAbbrevEditDlg)

public:
	CAbbrevEditDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAbbrevEditDlg();

	void SetEditMode(BOOL bEdit = TRUE)	{m_bEditMode = bEdit;}
	void SetAbbviationList(CAbbrevList* list) {m_pAbbrevList = list;}

	BOOL IsVisible() {return m_bVisible;}	//WA0003
	BOOL m_bVisible;						//WA0003

// Dialog Data
	enum { IDD = IDD_ADD_EDIT_ABBREV };

protected:

	CTextSpeaker m_ctrlSpeaker;		// Text Speaker for lexicon

	CRect PositionDialogOffMainWindow();		//WA0047

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	void PostNcDestroy();
	void OnCancel();
	CEdit m_edtAbbreviation;
	CEdit m_edtExpansion;
	CString m_strAbbreviation;
	CString m_strExpansion;
	CButton m_btnSayAbbrev;
	CButton m_btnSayExpan;
	CButton m_btnLexicon;

	BOOL m_bEditMode;				// TURE if Editing an existing Abbreviation 
	CAbbrevList* m_pAbbrevList;		// Pointer to the Abbrev list that is being added to

	// Event Handlers
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBtnSayAbbrev();
	afx_msg void OnBtnSayExpan();
	afx_msg void OnBtnLexicon();
	afx_msg void OnEditChangeAbbrev();
	afx_msg void OnEditChangeExpan();

	CButton m_btnAdd;
	CButton m_btnFinish;
};
