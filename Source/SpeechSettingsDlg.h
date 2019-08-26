#if !defined(AFX_SPEECHSETTINGSDLG_H__330EB2A4_990F_4EB2_9F9E_45BA08C33470__INCLUDED_)
#define AFX_SPEECHSETTINGSDLG_H__330EB2A4_990F_4EB2_9F9E_45BA08C33470__INCLUDED_

#include "Setting.h"	// Added by ClassView
#include "TreeDialog.h"
#include "voice.h"	// Added by ClassView
#include "textspeaker.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpeechSettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpeechSettingsDlg dialog

class CSpeechSettingsDlg : public CTreePage
{
// Construction
public:
	BOOL m_bSpeakAll;
	BOOL m_bSpeechOn;
	BOOL m_bModified;
	void SetModified(BOOL bModified =  TRUE) {m_bModified = bModified;}

	CSetting* m_Settings;
	CSpeechSettingsDlg(CWnd* pParent = NULL);   // standard constructor

	virtual void DialogSelected();
	virtual void LeavingDialog() {OnOK();}
// Dialog Data
	//{{AFX_DATA(CSpeechSettingsDlg)
	enum { IDD = IDD_SPEECH_DLG };
	CComboBox*	m_ctrlVoice;
	CSliderCtrl	m_ctrlVolume;
	CSliderCtrl	m_ctrlSpeed;
	CSliderCtrl	m_ctrlPitch;
	int		m_nPitch;
	int		m_nSpeed;
	int		m_nVolume;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpeechSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnChkSpeechOn();
	void EnableControls();
	void DisableControls();
	void InitialisePage();
	void DisablePage();

	CTextSpeaker m_ctrlSpeaker;
	CVoice m_cCurrentVoice;
	BOOL m_bNoSAPI;		// Set to TRUE if speech cannot be loaded

	// Generated message map functions
	//{{AFX_MSG(CSpeechSettingsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBtnTest();
	afx_msg void OnSelchangeComboVoice();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClose();
	afx_msg void OnKillfocusEditPitch();
	afx_msg void OnKillfocusEditRate();
	afx_msg void OnKillfocusEditVol();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPEECHSETTINGSDLG_H__330EB2A4_990F_4EB2_9F9E_45BA08C33470__INCLUDED_)
