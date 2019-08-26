#if !defined(AFX_POPUPSETTINGS_H__36B85D04_3074_11D6_92B8_ABF09B2BBC71__INCLUDED_)
#define AFX_POPUPSETTINGS_H__36B85D04_3074_11D6_92B8_ABF09B2BBC71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PopupSettings.h : header file
//

#include "Setting.h"		// Setting class
#include "ColorPickerCB.h"  // Colour picker Combo box
#include "TreeDialog.h"		// CTreePage class
#include "afxwin.h"
#include "afxcmn.h"

/////////////////////////////////////////////////////////////////////////////
// CButtonSettings dialog

class CButtonSettings : public CTreePage
{
// Construction
public:
	CButtonSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CButtonSettings)
	enum { IDD = IDD_BUTTON_SETTINGS };
	CColorPickerCB	m_cmb_font_colour;
	CColorPickerCB	m_cmb_back_colour;
	CComboBox	m_cmb_font_sizes;
	CComboBox	m_cmb_font_name;
	//}}AFX_DATA
//	CButton m_chkButtonBitmaps;


	CSetting* m_Settings;		// Store the settings

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual void OnOK();
	virtual void DialogSelected() {InitialiseSettings();}
	virtual void LeavingDialog() {OnOK();}

// Implementation
protected:

	// Methods to Initialise the dialog
	void FillCombo(CComboBox& combo, CString szComboText);
	BOOL FillFontCombo();
	void FillFontSizesCombo();
	void InitialiseSettings();
	void ClearSettings();
	void SetModified(BOOL bModified =  TRUE) {m_bModified = bModified;}
	BOOL GetModified() {return m_bModified;}

	// Methods to set the settings in the dialog
	void SetButtonSettings();

	// Methods to store settings from the dialog
	void SaveSettings();
	void SaveButtonSettings();
private:
	BOOL m_bModified;		// TRUE if a settings has changed

	// Generated message map functions
	//{{AFX_MSG(CButtonSettings)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnCustomColour();
	CButton m_btn_custom_back_colour;
	afx_msg void OnBnClickedBtnCustomFontColour();
	CButton m_btn_custom_font_colour;
	afx_msg void OnCbnSelchangeCmbColour();
	afx_msg void OnCbnSelchangeCmbFontName();
	afx_msg void OnCbnSelchangeCmbFontColour();
	afx_msg void OnCbnSelchangeCmbFontSizes();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POPUPSETTINGS_H__36B85D04_3074_11D6_92B8_ABF09B2BBC71__INCLUDED_)
