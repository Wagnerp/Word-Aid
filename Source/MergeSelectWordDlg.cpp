// MergeSelectWordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wordaid2.h"
#include "MergeSelectWordDlg.h"
#include "MergeFileSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 

struct WORD_ARRAY
	{	//! the word selected
		CString szText;			
		//! it's position in list box
		int pos;	
	};

/////////////////////////////////////////////////////////////////////////////
// CMergeSelectWordDlg property page

IMPLEMENT_DYNCREATE(CMergeSelectWordDlg, CPropertyPage)

CMergeSelectWordDlg::CMergeSelectWordDlg() : CPropertyPage(CMergeSelectWordDlg::IDD)
{
	//{{AFX_DATA_INIT(CMergeSelectWordDlg)
	m_strFileInput = _T("");
	m_strFileOutput = _T("");
	//}}AFX_DATA_INIT
}

CMergeSelectWordDlg::~CMergeSelectWordDlg()
{
}

void CMergeSelectWordDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMergeSelectWordDlg)
	DDX_Control(pDX, IDC_LIST_INPUT, m_listInput);
	DDX_Control(pDX, IDC_LIST_OUTPUT, m_listOutput);
	DDX_Text(pDX, IDC_TXT_INPUT_FILE, m_strFileInput);
	DDX_Text(pDX, IDC_TXT_OUTPUT_FILE, m_strFileOutput);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMergeSelectWordDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CMergeSelectWordDlg)
	ON_BN_CLICKED(IDC_BTN_ALL_IN, OnBtnAllIn)
	ON_BN_CLICKED(IDC_BTN_ALL_OUT, OnBtnAllOut)
	ON_BN_CLICKED(IDC_BTN_WORD_IN, OnBtnWordIn)
	ON_BN_CLICKED(IDC_BTN_WORD_OUT, OnBtnWordOut)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeSelectWordDlg message handlers

BOOL CMergeSelectWordDlg::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*) GetParent();

	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

	m_listInput.ResetContent();
	m_listOutput.ResetContent();

	CMergeFileSelectDlg* dlg = (CMergeFileSelectDlg*)pSheet->GetPage(1);
	
	if (dlg->m_strMainFilename.Find(".alp",0) > -1 || dlg->m_strMainFilename.Find(".ALP",0) > -1
		|| dlg->m_strMainFilename.Find(".abc",0) >-1 || dlg->m_strMainFilename.Find(".ABC",0) >-1)
	{
		CWordlist alpha;
		alpha.LoadWordlist(dlg->m_strMainFilename,ALPHA);

		for (int i=0; i < alpha.GetSize();i++)
			m_listOutput.AddString(alpha.GetAt(i));

	}
	if (dlg->m_strMainFilename.Find(".top",0) > -1 || dlg->m_strMainFilename.Find(".TOP",0) > -1)
	{
		CWordlist topic;
		topic.LoadWordlist(dlg->m_strMainFilename,ALPHA);

		for (int i=0; i<topic.GetSize();i++)
			m_listOutput.AddString(topic.GetAt(i));

	}
	if (dlg->m_strMainFilename.Find(".abv",0) > -1 || dlg->m_strMainFilename.Find(".ABV",0) > -1)
	{
		CAbbrevList abbrev;
		abbrev.LoadWordlist(dlg->m_strMainFilename);

		for (int i=0;i<abbrev.GetSize();i++)
			m_listOutput.AddString(abbrev.GetAt(i));

	}

	if (dlg->m_strMergeFilename.Find(".alp",0) > -1 || dlg->m_strMergeFilename.Find(".abc",0) > -1 
		||dlg->m_strMergeFilename.Find(".ALP",0) > -1 || dlg->m_strMergeFilename.Find(".ABC",0) > -1 )
	{
		CWordlist alpha;
		alpha.LoadWordlist(dlg->m_strMergeFilename,ALPHA);
		for (int i=0; i<alpha.GetSize();i++)
			m_listInput.AddString(alpha.GetAt(i));
	}
	if (dlg->m_strMergeFilename.Find(".top",0) > -1 || 	dlg->m_strMergeFilename.Find(".TOP",0) > -1)
	{
		CWordlist topic;
		topic.LoadWordlist(dlg->m_strMergeFilename,TOPIC);
		for (int i=0; i<topic.GetSize();i++)
			m_listInput.AddString(topic.GetAt(i));
	}
	if (dlg->m_strMergeFilename.Find(".abv",0) > -1 || dlg->m_strMergeFilename.Find(".ABV",0) > -1) 
	{
		CAbbrevList abbrev;
		abbrev.LoadWordlist(dlg->m_strMergeFilename);
		for (int i=0; i<abbrev.GetSize();i++)
			m_listInput.AddString(abbrev.GetAt(i));
	}
	CString str = _T("");
	for (int i =0;i<m_listOutput.GetCount();i++)
	{
		m_listOutput.GetText(i,str);
		m_strArrayOutput.Add(str);
	}

	return CPropertyPage::OnSetActive();
}

void CMergeSelectWordDlg::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnOK();
}

void CMergeSelectWordDlg::OnBtnAllIn() 
{
	// TODO: Add your control notification handler code here
	CString str;

	for (int i=0;i<m_listInput.GetCount();i++)
	{
		m_listInput.GetText(i,str);
		m_listOutput.AddString(str);
		m_strArrayOutput.Add(str);
	}

	m_listInput.ResetContent();
}

void CMergeSelectWordDlg::OnBtnAllOut() 
{
	// TODO: Add your control notification handler code here
	CString str;

	for (int i=0;i<m_listOutput.GetCount();i++)
	{
		m_listOutput.GetText(i,str);
		m_listInput.AddString(str);
	}
	m_strArrayOutput.RemoveAll();	
	m_listOutput.ResetContent();
}

void CMergeSelectWordDlg::OnBtnWordIn() 
{
	// TODO: Add your control notification handler code here
	int nCount = m_listInput.GetSelCount();
	CArray<int,int> arrSelections;
	CString str = _T("");

	arrSelections.SetSize(nCount);
	m_listInput.GetSelItems(nCount,arrSelections.GetData());

	// add selected strings to output list
	for ( int i=0;i<nCount;i++)
	{
		m_listInput.GetText(arrSelections.GetAt(i),str);
		m_listOutput.AddString(str);
		m_strArrayOutput.Add(str);
	}
	// now delete the string from the input list
	for (i=nCount;i>0;i--)
	{
	//	if (arrSelections.GetAt(i-1) < nCount)
			m_listInput.DeleteString(arrSelections.GetAt(i-1));
	//	else
	//		m_listInput.DeleteString(arrSelections.GetAt(i));
	}
}

void CMergeSelectWordDlg::OnBtnWordOut() 
{
	// TODO: Add your control notification handler code here
	int nCount = m_listOutput.GetSelCount();
	CArray<int,int> arrSelections;
	CString str = _T("");

	arrSelections.SetSize(nCount);
	m_listOutput.GetSelItems(nCount,arrSelections.GetData());

	for ( int i=0;i<nCount;i++)
	{
		m_listOutput.GetText(arrSelections.GetAt(i),str);
		m_listInput.AddString(str);
		if (FindString(str) > -1)
			m_strArrayOutput.RemoveAt(FindString(str));
	}

	for (i =nCount;i>0;i--)
	{
	//	if (arrSelections.GetAt(i-1)<nCount)
	//		m_listOutput.DeleteString(arrSelections.GetAt(i));
	//	else
			m_listOutput.DeleteString(arrSelections.GetAt(i-1));
	}
}

int CMergeSelectWordDlg::FindString(CString str)
{
	for (int i =0;i < m_strArrayOutput.GetSize();i++)
	{
		if ( m_strArrayOutput[i] == str)
			return i;
	}
	return -1;
}
