// WordAidSettingsDlg.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////////////
//																			//
//	WA0024 - fix problem with min/max wordlength and error messages			//
//																			//
//////////////////////////////////////////////////////////////////////////////
//																			//

#include "stdafx.h"
#include "wordaid2.h"
#include "WordAidSettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_MINLENGTH	3	//added 17th July
#define DEFAULT_MAXLENGTH	15	//added 17th July

/////////////////////////////////////////////////////////////////////////////
// CWordAidSettingsDlg dialog


CWordAidSettingsDlg::CWordAidSettingsDlg(CWnd* pParent /*=NULL*/)
	: CTreePage(CWordAidSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWordAidSettingsDlg)
	m_nMaxWordLength = DEFAULT_MAXLENGTH;	//added 17th July
	m_nMinWordLength = DEFAULT_MINLENGTH;	//added 17th July
	//}}AFX_DATA_INIT
}


void CWordAidSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CTreePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWordAidSettingsDlg)
	DDX_Text(pDX, IDC_EDIT_MAXWORD, m_nMaxWordLength);
//WA0024	DDV_MinMaxInt(pDX, m_nMaxWordLength, 1, 100);
	DDV_MinMaxInt(pDX, m_nMaxWordLength, 1, 20);
	DDX_Text(pDX, IDC_EDIT_MINWORD, m_nMinWordLength);
//WA0024	DDV_MinMaxInt(pDX, m_nMinWordLength, 1, 100);
	DDV_MinMaxInt(pDX, m_nMinWordLength, 1, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWordAidSettingsDlg, CTreePage)
	//{{AFX_MSG_MAP(CWordAidSettingsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT_MAXWORD, OnChangeEditMaxword)	//added 17th July
	ON_EN_CHANGE(IDC_EDIT_MINWORD, OnChangeEditMinword)	//added 17th July
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWordAidSettingsDlg message handlers

void CWordAidSettingsDlg::InitialiseSettings()
{
	m_nMinWordLength = atoi(m_Settings->GetSetting(MIN_WORD_LENGTH));	
	m_nMaxWordLength = atoi(m_Settings->GetSetting(MAX_WORD_LENGTH));

	UpdateData(FALSE);
	
}

void CWordAidSettingsDlg::SaveSettings()
{
	UpdateData(TRUE);

	CString strMax = _T("");
	CString strMin = _T("");

	strMax.Format(_T("%d"), m_nMaxWordLength);
	strMin.Format(_T("%d"), m_nMinWordLength);

	m_Settings->SetSetting(MIN_WORD_LENGTH,strMin);

	
	m_Settings->SetSetting(MAX_WORD_LENGTH,strMax);

	m_Settings->StoreSettings();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CWordAidSettingsDlg::OnOK()
{
	UpdateData();											//WA0024
	if (m_nMaxWordLength < m_nMinWordLength)				//WA0024
	{
		m_nMaxWordLength = m_nMinWordLength + 2;			//WA0024
		char num[3];										//WA0024
		itoa(m_nMaxWordLength,num,10);						//WA0024
		CString msg = _T("");								//WA0024
		msg.LoadString(IDS_WORDLENGTH_ERR);					//WA0024
		msg += num;											//WA0024
		AfxMessageBox(msg);									//WA0024
		GetDlgItem(IDC_EDIT_MAXWORD)->SetWindowText(num);	//WA0024
	}

	SaveSettings();
}

void CWordAidSettingsDlg::OnChangeEditMaxword() //added 17th July
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CTreePage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
//WA0024	if (m_nMaxWordLength < m_nMinWordLength)
//WA0024	{
//WA0024		AfxMessageBox(IDS_WORDLENGTH_ERR);
//WA0024		GetDlgItem(IDC_EDIT_MAXWORD)->SetFocus();
//WA0024	}
}

void CWordAidSettingsDlg::OnChangeEditMinword() //added 17th July
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CTreePage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
//WA0024	if (m_nMinWordLength > m_nMaxWordLength)
//WA0024	{
//WA0024		AfxMessageBox(IDS_MINWORDLENGTH_ERROR);
//WA0024		(GetDlgItem(IDC_EDIT_MINWORD))->SetFocus();
//WA0024	}
}


