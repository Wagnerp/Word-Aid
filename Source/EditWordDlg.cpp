// EditWordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wordaid2.h"
#include "EditWordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditWordDlg dialog


CEditWordDlg::CEditWordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditWordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditWordDlg)
	m_editWord = _T("");
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
//	m_ctrlSpeaker = NULL;
}

CEditWordDlg::~CEditWordDlg()
{
	if (m_ctrlSpeaker)
		m_ctrlSpeaker.DestroyWindow();
}

void CEditWordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditWordDlg)
	DDX_Text(pDX, IDC_EDIT1, m_editWord);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BTN_SAY, m_btnSay);
	DDX_Control(pDX, IDC_BTN_LEXICON, m_btnLexicon);
}


BEGIN_MESSAGE_MAP(CEditWordDlg, CDialog)
	//{{AFX_MSG_MAP(CEditWordDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_SAY, OnBtnSay)
	ON_BN_CLICKED(IDC_BTN_LEXICON, OnBtnLexicon)
	ON_EN_CHANGE(IDC_EDIT1, OnEditChangeWord)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditWordDlg message handlers

BOOL CEditWordDlg::OnInitDialog()
{
	m_ctrlSpeaker.Create("Speaker",WS_CHILD,CRect(0,0,1,1),this,2);

	CDialog::OnInitDialog();

	return TRUE;
}

void CEditWordDlg::OnBtnSay()
{
	UpdateData(TRUE);

	m_ctrlSpeaker.Speak(m_editWord);
}

void CEditWordDlg::OnBtnLexicon()
{
	m_ctrlSpeaker.ShowLexiconDialog((LONG)m_hWnd);
}

void CEditWordDlg::OnEditChangeWord()
{
	UpdateData(TRUE);

	if(m_editWord != _T(""))
	{
		if(m_ctrlSpeaker)
		{
			m_btnSay.EnableWindow(TRUE);
			m_btnLexicon.EnableWindow(TRUE);
		}
	}
	else
	{
		if(m_ctrlSpeaker)
		{
			m_btnSay.EnableWindow(FALSE);
			m_btnLexicon.EnableWindow(FALSE);
		}
	}
}
