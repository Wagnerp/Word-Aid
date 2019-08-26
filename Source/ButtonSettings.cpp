// PopupSettings.cpp : implementation file
//

#include "stdafx.h"
#include "WordAid2.h"
#include "ButtonSettings.h"

#include "SettingIndex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: CALLBACK function to display the fonts in a control.	   //
/////////////////////////////////////////////////////////////////////////////

int CALLBACK EnumFontFamiliesExProc(ENUMLOGFONTEX *lpelfe,
									NEWTEXTMETRICEX *lpntme,
									int FontType,
									LPARAM lParam)
{
	// Make sure blanks are not added
	CString szText = (LPSTR)(lpelfe->elfFullName);
	if(!szText.IsEmpty())
	{
		// Add font to combo box
		if(::SendMessage((HWND)lParam, CB_FINDSTRING, 0,
					  (LONG)(LPSTR)(lpelfe->elfFullName)) == LB_ERR)
		{
			::SendMessage((HWND)lParam, CB_ADDSTRING, 0,
						 (LONG)(LPSTR)(lpelfe->elfFullName));
		}
	}

    return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CButtonSettings dialog

CButtonSettings::CButtonSettings(CWnd* pParent /*=NULL*/)
	: CTreePage(CButtonSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CButtonSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bModified = FALSE;
}


void CButtonSettings::DoDataExchange(CDataExchange* pDX)
{
	CTreePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CButtonSettings)
	DDX_Control(pDX, IDC_CMB_FONT_COLOUR, m_cmb_font_colour);
	DDX_Control(pDX, IDC_CMB_COLOUR, m_cmb_back_colour);
	DDX_Control(pDX, IDC_CMB_FONT_SIZES, m_cmb_font_sizes);
	DDX_Control(pDX, IDC_CMB_FONT_NAME, m_cmb_font_name);
	DDX_Control(pDX, IDC_BTN_CUSTOM_COLOUR, m_btn_custom_back_colour);
	DDX_Control(pDX, IDC_BTN_CUSTOM_FONT_COLOUR, m_btn_custom_font_colour);
	//}}AFX_DATA_MAP
//	DDX_Control(pDX, IDC_CHK_BUTTON_BITMAPS, m_chkButtonBitmaps);
}


BEGIN_MESSAGE_MAP(CButtonSettings, CTreePage)
	//{{AFX_MSG_MAP(CButtonSettings)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CUSTOM_COLOUR, OnBnClickedBtnCustomColour)
	ON_BN_CLICKED(IDC_BTN_CUSTOM_FONT_COLOUR, OnBnClickedBtnCustomFontColour)
	ON_CBN_SELCHANGE(IDC_CMB_COLOUR, OnCbnSelchangeCmbColour)
	ON_CBN_SELCHANGE(IDC_CMB_FONT_NAME, OnCbnSelchangeCmbFontName)
	ON_CBN_SELCHANGE(IDC_CMB_FONT_COLOUR, OnCbnSelchangeCmbFontColour)
	ON_CBN_SELCHANGE(IDC_CMB_FONT_SIZES, OnCbnSelchangeCmbFontSizes)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonSettings message handlers

BOOL CButtonSettings::OnInitDialog() 
{
	CString szComboText = _T("");
	int nIndex = 0;

	CTreePage::OnInitDialog();


	FillFontCombo();

	FillFontSizesCombo();
	
	// Initialise the settings from registry
	InitialiseSettings();		

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Call to fill the font name combobox with all the		   //
//				: fonts on the system.									   //
//				:														   //
// RETURNS		: TRUE if it was successful.							   //
/////////////////////////////////////////////////////////////////////////////

BOOL CButtonSettings::FillFontCombo()
{
	BOOL bReturn = TRUE;

    HDC hDC = ::GetDC(NULL);

    LOGFONT lf = { 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0,
				   0, "" };

    EnumFontFamiliesEx( hDC, &lf, (FONTENUMPROC)EnumFontFamiliesExProc, (long)m_cmb_font_name.m_hWnd, 0 );

    ::ReleaseDC( NULL, hDC );
  
	return bReturn;
} 
/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Fill the combobox with the font size.					   //
/////////////////////////////////////////////////////////////////////////////

void CButtonSettings::FillFontSizesCombo()
{
	CString szComboText = _T("");

	TRY
	{
		// Load Combo Strings
		szComboText.LoadString(IDS_FONT_SIZES);
		
		FillCombo(m_cmb_font_sizes, szComboText);
	}
	CATCH(CMemoryException, e)
    {
    }
    END_CATCH

}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Call to fill a combobox with a structured strig with '|' //
//				: as a seperator.										   //
//				:														   //
// PARAMETERS	: combo - combobox to fill.								   //
//				: szComboBox - string to fill combobox with.			   //
/////////////////////////////////////////////////////////////////////////////
void CButtonSettings::FillCombo(CComboBox& combo, CString szComboText)
{
	int nSplitPos = 0;

	// Add each item to the combo box
	for(nSplitPos = szComboText.Find(_T("|")); nSplitPos != -1; nSplitPos = szComboText.Find(_T("|")))
	{
		combo.AddString(szComboText.Left(nSplitPos));
		szComboText = szComboText.Right(szComboText.GetLength() - nSplitPos-1);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CButtonSettings::InitialiseSettings()
{
	 SetButtonSettings();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CButtonSettings::SetButtonSettings()
{
	ClearSettings();

	m_cmb_back_colour.SetSelectedColorValue(m_Settings->GetColourSetting(BUTTON_BACKGROUND));
	m_cmb_font_name.SelectString(0, m_Settings->GetSetting(BUTTON_FONT_NAME));
	m_cmb_font_colour.SetSelectedColorValue(m_Settings->GetColourSetting(BUTTON_FOREGROUND));
	m_cmb_font_sizes.SelectString(-1, m_Settings->GetSetting(BUTTON_FONT_SIZE));

//	m_chkButtonBitmaps.SetCheck(atoi(m_Settings->GetSetting(BUTTON_BITMAPS_ON)));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CButtonSettings::OnBnClickedBtnCustomColour()
{
	CColorDialog aDlg;

	if(aDlg.DoModal() == IDOK)
	{
		// TODO : Add String to string table
		// Add the colour and set it to be current selection
		COLORREF colour = aDlg.GetColor();
		int nIndex = m_cmb_back_colour.AddColor(_T("Custom"), colour);
		m_cmb_back_colour.SetCurSel(nIndex);
		SetModified();
	}
	
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CButtonSettings::OnBnClickedBtnCustomFontColour()
{
	CColorDialog aDlg;

	if(aDlg.DoModal() == IDOK)
	{
		// TODO : Add String to string table
		// Add the colour and set it to be current selection
		COLORREF colour = aDlg.GetColor();
		int nIndex = m_cmb_font_colour.AddColor(_T("Custom"), colour);
		m_cmb_font_colour.SetCurSel(nIndex);
		SetModified();
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CButtonSettings::ClearSettings()
{
	m_cmb_back_colour.SetCurSel(-1);
	m_cmb_font_name.SetCurSel(-1);
	m_cmb_font_sizes.SetCurSel(-1);
	m_cmb_font_colour.SetCurSel(-1);
}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when combo box is changed.  It sets the modified  //
//				: flag to TRUE so user can be requested to save changes    //
/////////////////////////////////////////////////////////////////////////////

void CButtonSettings::OnCbnSelchangeCmbColour()
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when combo box is changed.  It sets the modified  //
//				: flag to TRUE so user can be requested to save changes    //
/////////////////////////////////////////////////////////////////////////////

void CButtonSettings::OnCbnSelchangeCmbFontName()
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when combo box is changed.  It sets the modified  //
//				: flag to TRUE so user can be requested to save changes    //
/////////////////////////////////////////////////////////////////////////////

void CButtonSettings::OnCbnSelchangeCmbFontColour()
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when combo box is changed.  It sets the modified  //
//				: flag to TRUE so user can be requested to save changes    //
/////////////////////////////////////////////////////////////////////////////

void CButtonSettings::OnCbnSelchangeCmbFontSizes()
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CButtonSettings::SaveSettings()	
{
	//if (m_bModified)
/*	{
		CString szSave = _T("");
		CString szWordAid = _T("");

		TRY
		{
			szSave.LoadString(IDS_SAVE_SETTING);
			szWordAid.LoadString(IDS_WORDAID2);
		}
		CATCH(CMemoryException, e)
		{
		}
		END_CATCH

		if(::MessageBox(this->m_hWnd, szSave, szWordAid, MB_YESNO) == IDYES)
		{*/
			SaveButtonSettings();
//		}
//	}
}

/////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Stores the changes to the button font settings.		   //
/////////////////////////////////////////////////////////////////////////////

void CButtonSettings::SaveButtonSettings()
{
	CString szFontName = _T("");
	CString szFontSize = _T("");

	m_Settings->SetColourSetting(BUTTON_BACKGROUND, m_cmb_back_colour.GetSelectedColorValue());
	m_cmb_font_name.GetLBText(m_cmb_font_name.GetCurSel(), szFontName);
	m_Settings->SetSetting(BUTTON_FONT_NAME, szFontName);
	m_Settings->SetColourSetting(BUTTON_FOREGROUND, m_cmb_font_colour.GetSelectedColorValue());
	m_cmb_font_sizes.GetLBText(m_cmb_font_sizes.GetCurSel(), szFontSize);
	m_Settings->SetSetting(BUTTON_FONT_SIZE, szFontSize);

	CString strBtnBmps = _T("");

//	strBtnBmps.Format(_T("%d"), m_chkButtonBitmaps.GetCheck());
//	m_Settings->SetSetting(BUTTON_BITMAPS_ON, strBtnBmps);

	m_Settings->StoreSettings();
}

void CButtonSettings::OnOK()
{
	//if (m_bModified)
	{
		SaveSettings();
		SetModified(FALSE);
	}
}