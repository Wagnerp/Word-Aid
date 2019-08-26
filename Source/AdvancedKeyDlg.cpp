// AdvancedKeyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wordaid2.h"
#include "AdvancedKeyDlg.h"

//#include "VKConversion.h"
#include "DefaultKeys.h"		// To set back default short cuts

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const CString SPLITTER = _T(".");

const int I_ADD_WORD		= 0;
const int I_DELETE_WORD		= 1;
const int I_INSERT_WORD		= 2;
const int I_SAY_WORD		= 3;
const int I_PAGEUP			= 4;
const int I_PAGEDWN			= 5;
const int I_UPARR			= 6;
const int I_DWNARR			= 7;
const int I_LFTARR			= 8;
const int I_RGHTARR			= 9;
const int I_UNDO			= 10;
const int I_SHOWLIST		= 11;
const int I_PARTUNDO		= 12;
const int I_FULLUNDO		= 13;
const int I_ABBREVBTN		= 14;	//WA0016

/////////////////////////////////////////////////////////////////////////////
// CAdvancedKeyDlg dialog


CAdvancedKeyDlg::CAdvancedKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvancedKeyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdvancedKeyDlg)
	//}}AFX_DATA_INIT

	m_pWordAidSetting = NULL;
}


void CAdvancedKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvancedKeyDlg)
	DDX_Control(pDX, IDC_LST_KEYS, m_lstKeys);
	DDX_Control(pDX, IDC_EDT_NEW, m_edtNew);
	DDX_Control(pDX, IDC_EDT_DESCRIPTION, m_edtDescription);
	DDX_Control(pDX, IDC_EDT_CURRENT, m_edtCurrent);
	DDX_Control(pDX, IDC_CMB_USED_BY, m_cmbUsedBy);
	DDX_Control(pDX, IDC_BTN_ASSIGN, m_btnAsign);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvancedKeyDlg, CDialog)
	//{{AFX_MSG_MAP(CAdvancedKeyDlg)
	ON_LBN_SELCHANGE(IDC_LST_KEYS, OnSelchangeLstKeys)
	ON_EN_CHANGE(IDC_EDT_NEW, OnChangeEdtNew)
	ON_BN_CLICKED(IDC_BTN_ASSIGN, OnBtnAssign)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_DEFAULT, OnBtnDefault)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvancedKeyDlg message handlers

BOOL CAdvancedKeyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LoadKeys();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called when the user selects a short cut.  The short cut and
//				: description is displayed.
////////////////////////////////////////////////////////////////////////////////

void CAdvancedKeyDlg::OnSelchangeLstKeys() 
{
	int nIndex = m_lstKeys.GetItemData(m_lstKeys.GetCurSel());

	SHORTCUT_INFO info = m_arrShortCuts.GetAt(nIndex);

	m_edtDescription.SetWindowText(info.strDescription);

	m_edtCurrent.ClearEditBox();

	// Set the current short-cut combination
	m_edtCurrent.SetVKPressed(info.nVK);
	m_edtCurrent.SetSpecialKeysPressed(info.strSpec);
	m_edtCurrent.DisplayShortCut();

	// Clear new
	m_edtNew.ClearEditBox();
	m_edtNew.SetWindowText(_T(""));
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Checks to see if the short cut is already used by another
//				: action.  If it is then it displays which short-cut is the 
//				: same.
////////////////////////////////////////////////////////////////////////////////

void CAdvancedKeyDlg::OnChangeEdtNew() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	KEY_TYPE type, typeSel;
	KEYS_USED keys;

	keys.nVK		= m_edtNew.GetVKPressed();
	keys.strSpecKey = m_edtNew.GetSpecialKeysPressed();

	switch(m_lstKeys.GetItemData(m_lstKeys.GetCurSel()))
	{
	case I_ADD_WORD:
		typeSel = KT_ADD_WORD;
		break;
	case I_DELETE_WORD:
		typeSel = KT_DELETE_WORD;
		break;
	case I_INSERT_WORD:
		typeSel = KT_INSERT;
		break;
	case I_SAY_WORD:
		typeSel = KT_SAY;
		break;
	case I_PAGEUP:
		typeSel = KT_PAGEUP;
		break;
	case I_PAGEDWN:
		typeSel = KT_PAGEDWN;
		break;
	case I_UPARR:
		typeSel = KT_UPARR;
		break;
	case I_DWNARR:
		typeSel = KT_DWNARR;
		break;
	case I_LFTARR:
		typeSel = KT_LEFTARR;
		break;
	case I_RGHTARR:
		typeSel = KT_RIGHTARR;
		break;
	case I_UNDO:
		typeSel = KT_UNDO;
		break;
	case I_SHOWLIST:
		typeSel = KT_SHOWLIST;
		break;
	case I_PARTUNDO:
		typeSel = KT_UNEXPAND_PART;
		break;
	case I_FULLUNDO:
		typeSel = KT_UNEXPAND_FULL;
		break;
	case I_ABBREVBTN:		//WA0016
		typeSel = KT_ABBREVBTN;
	}

	type = m_pWordAidSetting->CheckKeySelection(keys.nVK, keys.strSpecKey, typeSel, FALSE);
	
	if(type == KT_NONE)
	{
		m_btnAsign.EnableWindow(TRUE);
		m_cmbUsedBy.ResetContent();
		m_cmbUsedBy.SetCurSel(-1);
	}
	else
	{
		m_btnAsign.EnableWindow(FALSE);
		SetUsedBy(type);
	}
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Sets the short cut for the action
////////////////////////////////////////////////////////////////////////////////

void CAdvancedKeyDlg::OnBtnAssign() 
{
	KEYS_USED keys;
	CString strVK = _T("");

	keys.nVK		= m_edtNew.GetVKPressed();
	keys.strSpecKey = m_edtNew.GetSpecialKeysPressed();

	strVK.Format(_T("%d"), keys.nVK);

	int nSel = m_lstKeys.GetCurSel();

	switch(m_lstKeys.GetItemData(nSel))
	{
	case I_ADD_WORD:
		m_pWordAidSetting->SetAddWordKey(keys, FALSE);

		m_pSetting->SetSetting(ADD_WORD, strVK);
		m_pSetting->SetSetting(ADD_WORD_SPEC, keys.strSpecKey);
		break;
	case I_DELETE_WORD:
		m_pWordAidSetting->SetDeleteWordKey(keys, FALSE);

		m_pSetting->SetSetting(DEL_WORD_KEY, strVK);
		m_pSetting->SetSetting(DEL_WORD_SPEC, keys.strSpecKey);
		break;
	case I_INSERT_WORD:
		m_pWordAidSetting->SetInsertWordKey(keys, FALSE);

		m_pSetting->SetSetting(INS_WORD_KEY, strVK);
		m_pSetting->SetSetting(INS_WORD_SPEC, keys.strSpecKey);
		break;
	case I_SAY_WORD:
		m_pWordAidSetting->SetSayWordKey(keys, FALSE);

		m_pSetting->SetSetting(SAY_KEY, strVK);
		m_pSetting->SetSetting(SAY_SPEC, keys.strSpecKey);
		break;
	case I_PAGEUP:
		m_pWordAidSetting->SetPageUpKey(keys, FALSE);

		m_pSetting->SetSetting(PGUP_KEY, strVK);
		m_pSetting->SetSetting(PGUP_SPEC, keys.strSpecKey);
		break;
	case I_PAGEDWN:
		m_pWordAidSetting->SetPageDownKey(keys, FALSE);

		m_pSetting->SetSetting(PGDWN_KEY, strVK);
		m_pSetting->SetSetting(PGDWN_SPEC, keys.strSpecKey);
		break;
	case I_UPARR:
		m_pWordAidSetting->SetUpArrowKey(keys, FALSE);

		m_pSetting->SetSetting(UPARR_KEY, strVK);
		m_pSetting->SetSetting(UPARR_SPEC, keys.strSpecKey);
		break;
	case I_DWNARR:
		m_pWordAidSetting->SetDownArrowKey(keys, FALSE);

		m_pSetting->SetSetting(DWNARR_KEY, strVK);
		m_pSetting->SetSetting(DWNARR_SPEC, keys.strSpecKey);
		break;
	case I_LFTARR:
		m_pWordAidSetting->SetLeftArrowKey(keys, FALSE);

		m_pSetting->SetSetting(LFTARR_KEY, strVK);
		m_pSetting->SetSetting(LFTARR_SPEC, keys.strSpecKey);
		break;
	case I_RGHTARR:
		m_pWordAidSetting->SetRightArrowKey(keys, FALSE);

		m_pSetting->SetSetting(RGHTARR_KEY, strVK);
		m_pSetting->SetSetting(RGHTARR_SPEC, keys.strSpecKey);
		break;
	case I_UNDO:
		m_pWordAidSetting->SetUndoKey(keys, FALSE);

		m_pSetting->SetSetting(UNDO_HOTKEY, strVK);
		m_pSetting->SetSetting(UNDO_SPEC, keys.strSpecKey);
		break;
	case I_SHOWLIST:
		m_pWordAidSetting->SetShowListKey(keys, FALSE);

		m_pSetting->SetSetting(SHOW_LIST_HOTKEY, strVK);
		m_pSetting->SetSetting(SHOW_LIST_SPEC, keys.strSpecKey);
		break;
	case I_PARTUNDO:
		m_pWordAidSetting->SetUnExpandPartKey(keys, FALSE);

		m_pSetting->SetSetting(ABBREV_PART_EXPAN_HOTKEY, strVK);
		m_pSetting->SetSetting(ABBREV_PART_EXPAN_SPEC_KEY, keys.strSpecKey);
		break;
	case I_FULLUNDO:
		m_pWordAidSetting->SetUnExpandFullKey(keys, FALSE);

		m_pSetting->SetSetting(ABBREV_FULL_EXPAN_HOTKEY, strVK);
		m_pSetting->SetSetting(ABBREV_FULL_EXPAN_SPEC_KEY, keys.strSpecKey);
		break;
	case I_ABBREVBTN:	//WA0016
		m_pWordAidSetting->SetAbbrevBtnHotkey(keys, FALSE);

		m_pSetting->SetSetting(ABBREV_BTN_HOTKEY,strVK);
		m_pSetting->SetSetting(ABBREV_BTN_HOTKEY_SPEC,keys.strSpecKey);
		break;
	}

	LoadKeys();

	m_lstKeys.SetCurSel(nSel);
	OnSelchangeLstKeys();
}

void CAdvancedKeyDlg::OnOK() 
{
	
	SHORTCUT_INFO info = m_arrShortCuts.GetAt(I_ADD_WORD);

	CString strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);
	m_pSetting->SetSetting(ADD_WORD, strVK);
	m_pSetting->SetSetting(ADD_WORD_SPEC, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_DELETE_WORD);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(DEL_WORD_KEY, strVK);
	m_pSetting->SetSetting(DEL_WORD_SPEC, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_INSERT_WORD);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(INS_WORD_KEY, strVK);
	m_pSetting->SetSetting(INS_WORD_SPEC, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_SAY_WORD);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(SAY_KEY, strVK);
	m_pSetting->SetSetting(SAY_SPEC, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_PAGEUP);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(PGUP_KEY, strVK);
	m_pSetting->SetSetting(PGUP_SPEC, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_PAGEDWN);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(PGDWN_KEY, strVK);
	m_pSetting->SetSetting(PGDWN_SPEC, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_UPARR);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(UPARR_KEY, strVK);
	m_pSetting->SetSetting(UPARR_SPEC, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_DWNARR);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(DWNARR_KEY, strVK);
	m_pSetting->SetSetting(DWNARR_SPEC, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_LFTARR);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(LFTARR_KEY, strVK);
	m_pSetting->SetSetting(LFTARR_SPEC, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_RGHTARR);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(RGHTARR_KEY, strVK);
	m_pSetting->SetSetting(RGHTARR_SPEC, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_UNDO);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(UNDO_HOTKEY, strVK);
	m_pSetting->SetSetting(UNDO_SPEC, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_SHOWLIST);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(SHOW_LIST_HOTKEY, strVK);
	m_pSetting->SetSetting(SHOW_LIST_SPEC, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_PARTUNDO);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(ABBREV_PART_EXPAN_HOTKEY, strVK);
	m_pSetting->SetSetting(ABBREV_PART_EXPAN_SPEC_KEY, info.strSpec);

	//////////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_FULLUNDO);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(ABBREV_FULL_EXPAN_HOTKEY, strVK);
	m_pSetting->SetSetting(ABBREV_FULL_EXPAN_SPEC_KEY, info.strSpec);

	//////////////////////////////////////////////////////////////
//WA0016
	info = m_arrShortCuts.GetAt(I_ABBREVBTN);

	strVK = _T("");

	strVK.Format(_T("%d"), info.nVK);

	m_pSetting->SetSetting(ABBREV_BTN_HOTKEY, strVK);
	m_pSetting->SetSetting(ABBREV_BTN_HOTKEY_SPEC,info.strSpec);	//WA0021
//	m_pSetting->SetSetting(ABBREV_FULL_EXPAN_SPEC_KEY, info.strSpec);

	//////////////////////////////////////////////////////////////
	CDialog::OnOK();
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Loads the keys so that a string describing them can be 
//				: displayed in the list box and that the short cut and a 
//				: description can be displayed when selected from the list box.
////////////////////////////////////////////////////////////////////////////////

void CAdvancedKeyDlg::LoadKeys()
{
	m_arrShortCuts.RemoveAll();

	SHORTCUT_INFO info;

	info.nVK = 0;
	info.strDescription = _T("");
	info.strName = _T("");
	info.strSpec = _T("");

	CString strWordlist		= _T("");
	CString strAdd			= _T("");
	CString strDel			= _T("");
	CString strInsert		= _T("");
	CString strSay			= _T("");
	CString strPgUp			= _T("");
	CString strPgDwn		= _T("");
	CString strUpArr		= _T("");
	CString strDwnArr		= _T("");
	CString strLftArr		= _T("");
	CString strRghtArr		= _T("");
	CString strUndo			= _T("");
	CString strShowList		= _T("");
	CString strPartUndo		= _T("");
	CString strFullUndo		= _T("");
	CString strAbbrevBtnHK	= _T("");	//WA0016
	CString strWordlistDes	= _T("");
	CString strAddDes		= _T("");
	CString strDelDes		= _T("");
	CString strInsertDes	= _T("");
	CString strSayDes		= _T("");
	CString strPgUpDes		= _T("");
	CString strPgDwnDes		= _T("");
	CString strUpArrDes		= _T("");
	CString strDwnArrDes	= _T("");
	CString strLftArrDes	= _T("");
	CString strRghtArrDes	= _T("");
	CString strUndoDes		= _T("");
	CString strShowListDes	= _T("");
	CString strPartUndoDes	= _T("");
	CString strFullUndoDes	= _T("");
	CString strAbbrevBtnHKDes	= _T("");	//WA0016

	// Load Short cuts
	strWordlist.LoadString(IDS_KEYBOARD_WORDLIST);
	strAdd.LoadString(IDS_KEYBOARD_ADD);
	strDel.LoadString(IDS_KEYBOARD_DELETE);
	strInsert.LoadString(IDS_KEYBOARD_INSERT);
	strSay.LoadString(IDS_KEYBOARD_SAYWORD);
	strPgUp.LoadString(IDS_KEYBOARD_PAGEUP);
	strPgDwn.LoadString(IDS_KEYBOARD_PAGEDWN);
	strUpArr.LoadString(IDS_KEYBOARD_UPARROW);
	strDwnArr.LoadString(IDS_KEYBOARD_DWNARROW);
	strLftArr.LoadString(IDS_KEYBOARD_LFTARROW);
	strRghtArr.LoadString(IDS_KEYBOARD_RGHTARROW);
	strUndo.LoadString(IDS_KEYBOARD_UNDO);
	strShowList.LoadString(IDS_KEYBOARD_SHOWLIST);
	strPartUndo.LoadString(IDS_KEYBOARD_UNDOPART);
	strFullUndo.LoadString(IDS_KEYBOARD_UNDOFULL);
	strAbbrevBtnHK.LoadString(IDS_ABBREV_BTN_HK);		//WA0016
	
	// Load short-cut descriptions
	strAddDes.LoadString(IDS_KEYBOARD_ADD_DES);
	strDelDes.LoadString(IDS_KEYBOARD_DELETE_DES);
	strInsertDes.LoadString(IDS_KEYBOARD_INSERT_DES);
	strSayDes.LoadString(IDS_KEYBOARD_SAYWORD_DES);
	strPgUpDes.LoadString(IDS_KEYBOARD_PAGEUP_DES);
	strPgDwnDes.LoadString(IDS_KEYBOARD_PAGEDWN_DES);
	strUpArrDes.LoadString(IDS_KEYBOARD_UPARROW_DES);
	strDwnArrDes.LoadString(IDS_KEYBOARD_DWNARROW_DES);
	strLftArrDes.LoadString(IDS_KEYBOARD_LFTARROW_DES);
	strRghtArrDes.LoadString(IDS_KEYBOARD_RGHTARROW_DES);
	strUndoDes.LoadString(IDS_KEYBOARD_UNDO_DES);
	strShowListDes.LoadString(IDS_KEYBOARD_SHOWLIST_DES);
	strPartUndoDes.LoadString(IDS_KEYBOARD_UNDOPART_DES);
	strFullUndoDes.LoadString(IDS_KEYBOARD_UNDOFULL_DES);
	strAbbrevBtnHKDes.LoadString(IDS_ABBREV_BTN_HK_DES);		//WA0016

	// Add string to list box
	m_lstKeys.ResetContent();

	int nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strAdd);
	m_lstKeys.SetItemData(nIndex, I_ADD_WORD);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strDel);
	m_lstKeys.SetItemData(nIndex, I_DELETE_WORD);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strInsert);
	m_lstKeys.SetItemData(nIndex, I_INSERT_WORD);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strSay);
	m_lstKeys.SetItemData(nIndex, I_SAY_WORD);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strPgUp);
	m_lstKeys.SetItemData(nIndex, I_PAGEUP);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strPgDwn);
	m_lstKeys.SetItemData(nIndex, I_PAGEDWN);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strUpArr);
	m_lstKeys.SetItemData(nIndex, I_UPARR);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strDwnArr);
	m_lstKeys.SetItemData(nIndex, I_DWNARR);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strLftArr);
	m_lstKeys.SetItemData(nIndex, I_LFTARR);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strRghtArr);
	m_lstKeys.SetItemData(nIndex, I_RGHTARR);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strUndo);
	m_lstKeys.SetItemData(nIndex, I_UNDO);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strShowList);
	m_lstKeys.SetItemData(nIndex, I_SHOWLIST);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strPartUndo);
	m_lstKeys.SetItemData(nIndex, I_PARTUNDO);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strFullUndo);
	m_lstKeys.SetItemData(nIndex, I_FULLUNDO);
	nIndex = m_lstKeys.AddString(strWordlist + SPLITTER + strAbbrevBtnHK);		//WA0016
	m_lstKeys.SetItemData(nIndex, I_ABBREVBTN);									//WA0016

	// Store Short-cut info
	info.strName = strWordlist + SPLITTER + strAdd;
	info.strDescription = strAddDes;
	info.nVK = atoi(m_pSetting->GetSetting(ADD_WORD));
	info.strSpec = m_pSetting->GetSetting(ADD_WORD_SPEC);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName = strWordlist + SPLITTER + strDel;
	info.strDescription = strDelDes;
	info.nVK = atoi(m_pSetting->GetSetting(DEL_WORD_KEY));
	info.strSpec = m_pSetting->GetSetting(DEL_WORD_SPEC);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName = strWordlist + SPLITTER + strInsert;
	info.strDescription = strInsertDes;
	info.nVK = atoi(m_pSetting->GetSetting(INS_WORD_KEY));
	info.strSpec = m_pSetting->GetSetting(INS_WORD_SPEC);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName = strWordlist + SPLITTER + strSay;
	info.strDescription = strSayDes;
	info.nVK = atoi(m_pSetting->GetSetting(SAY_KEY));
	info.strSpec = m_pSetting->GetSetting(SAY_SPEC);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName = strWordlist + SPLITTER + strPgUp;
	info.strDescription = strPgUpDes;
	info.nVK = atoi(m_pSetting->GetSetting(PGUP_KEY));
	info.strSpec = m_pSetting->GetSetting(PGUP_SPEC);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName = strWordlist + SPLITTER + strPgDwn;
	info.strDescription = strPgDwnDes;
	info.nVK = atoi(m_pSetting->GetSetting(PGDWN_KEY));
	info.strSpec = m_pSetting->GetSetting(PGDWN_SPEC);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName = strWordlist + SPLITTER + strUpArr;
	info.strDescription = strUpArrDes;
	info.nVK = atoi(m_pSetting->GetSetting(UPARR_KEY));
	info.strSpec = m_pSetting->GetSetting(UPARR_SPEC);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName = strWordlist + SPLITTER + strDwnArr;
	info.strDescription = strDwnArrDes;
	info.nVK = atoi(m_pSetting->GetSetting(DWNARR_KEY));
	info.strSpec = m_pSetting->GetSetting(DWNARR_SPEC);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName = strWordlist + SPLITTER + strLftArr;
	info.strDescription = strLftArrDes;
	info.nVK = atoi(m_pSetting->GetSetting(LFTARR_KEY));
	info.strSpec = m_pSetting->GetSetting(LFTARR_SPEC);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName = strWordlist + SPLITTER + strRghtArr;
	info.strDescription = strRghtArrDes;
	info.nVK = atoi(m_pSetting->GetSetting(RGHTARR_KEY));
	info.strSpec = m_pSetting->GetSetting(RGHTARR_SPEC);

	m_arrShortCuts.Add(info);

	//////////////////////////////////////////////////////
	info.strName		= strWordlist + SPLITTER + strUndo;
	info.strDescription = strUndoDes;
	info.nVK			= atoi(m_pSetting->GetSetting(UNDO_HOTKEY));
	info.strSpec		= m_pSetting->GetSetting(UNDO_SPEC);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName = strWordlist + SPLITTER + strShowList;
	info.strDescription = strShowListDes;

	info.nVK		= atoi(m_pSetting->GetSetting(SHOW_LIST_HOTKEY));
	info.strSpec	= m_pSetting->GetSetting(SHOW_LIST_SPEC);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName		= strWordlist + SPLITTER + strPartUndo;
	info.strDescription = strPartUndoDes;
	info.nVK			= atoi(m_pSetting->GetSetting(ABBREV_PART_EXPAN_HOTKEY));
	info.strSpec		= m_pSetting->GetSetting(ABBREV_PART_EXPAN_SPEC_KEY);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName		= strWordlist + SPLITTER + strFullUndo;
	info.strDescription = strFullUndoDes;
	info.nVK			= atoi(m_pSetting->GetSetting(ABBREV_FULL_EXPAN_HOTKEY));
	info.strSpec		= m_pSetting->GetSetting(ABBREV_FULL_EXPAN_SPEC_KEY);

	m_arrShortCuts.Add(info);
	//////////////////////////////////////////////////////
	info.strName		= strWordlist + SPLITTER + strAbbrevBtnHK;			//WA0016
	info.strDescription = strAbbrevBtnHKDes;								//WA0016
	info.nVK			= atoi(m_pSetting->GetSetting(ABBREV_BTN_HOTKEY));	//WA0016
//	info.strSpec		= "";												//WA0016
	info.strSpec		= m_pSetting->GetSetting(ABBREV_BTN_HOTKEY_SPEC);	//WA0021

	m_arrShortCuts.Add(info);												//WA0016
	//////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Sets the Used By combo to the short-cut that is
//				: being used by.
//
// PARAMETERS	: type - the type of short-cut that should be used
////////////////////////////////////////////////////////////////////////

void CAdvancedKeyDlg::SetUsedBy(KEY_TYPE type)
{
	int nShortcut = -1;		// Used to store the row to select from short
							// cut list box
	// Clear the combo box
	m_cmbUsedBy.ResetContent();

	switch(type)
	{
	case KT_SELECT_WORD:
		{
			CString strWordlist = _T("");
			CString strSelectWord = _T("");

			strWordlist.LoadString(IDS_KEYBOARD_WORDLIST);
			strSelectWord.LoadString(IDS_KEYBOARD_SELECTWORD);

			int nIndex = m_cmbUsedBy.AddString(strWordlist + SPLITTER + strSelectWord);
			m_cmbUsedBy.SetCurSel(nIndex);

			return;
		}
	case KT_UNDO:
		nShortcut = I_UNDO;
		break;
	case KT_SHOWLIST:
		nShortcut = I_SHOWLIST;
		break;
	case KT_ABBREV_MANUAL:
		{
			CString strWordlist = _T("");
			CString strAbbrevMan = _T("");

			strWordlist.LoadString(IDS_KEYBOARD_WORDLIST);
			strAbbrevMan.LoadString(IDS_KEYBOARD_ABBREVMAN);

			int nIndex = m_cmbUsedBy.AddString(strWordlist + SPLITTER + strAbbrevMan);
			m_cmbUsedBy.SetCurSel(nIndex);

			return;
		}
	case KT_UNEXPAND_FULL:
		nShortcut = I_FULLUNDO;
		break;
	case KT_UNEXPAND_PART:
		nShortcut = I_PARTUNDO;
		break;
	case KT_ADD_WORD:
		nShortcut = I_ADD_WORD;
		break;
	case KT_DELETE_WORD:
		nShortcut = I_DELETE_WORD;
		break;
	case KT_INSERT:
		nShortcut = I_INSERT_WORD;
		break;
	case KT_SAY:
		nShortcut = I_SAY_WORD;
		break;
	case KT_PAGEUP:
		nShortcut = I_PAGEUP;
		break;
	case KT_PAGEDWN:
		nShortcut = I_PAGEDWN;
		break;
	case KT_UPARR:
		nShortcut = I_UPARR;
		break;
	case KT_DWNARR:
		nShortcut = I_DWNARR;
		break;
	case KT_LEFTARR:
		nShortcut = I_LFTARR;
		break;
	case KT_RIGHTARR:
		nShortcut = I_RGHTARR;
		break;
	case KT_ABBREVBTN:		//WA0016
		nShortcut = I_ABBREVBTN;
		break;
	default:
		ASSERT(FALSE);
	}
	
	CString str = _T("");

	for(int i = 0; i < m_lstKeys.GetCount(); i++)
	{
		if((int)m_lstKeys.GetItemData(i) == nShortcut)
		{
			// Get the text to display
			m_lstKeys.GetText(i, str);
		}
	}

	// Add to the used by combo box
	int nIndex = m_cmbUsedBy.AddString(str);
	m_cmbUsedBy.SetCurSel(nIndex);
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Called to set back the default short cut keys.
////////////////////////////////////////////////////////////////////////////////

void CAdvancedKeyDlg::OnBtnDefault()
{
	// Go though array and set back existing ones.
	SHORTCUT_INFO info = m_arrShortCuts.GetAt(I_ADD_WORD);

	info.nVK		= DEFAULT_ADD_WORD_KEY;
	info.strSpec	= DEFAULT_ADD_WORD_SPEC;

	m_arrShortCuts.SetAt(I_ADD_WORD, info);

	/////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_DELETE_WORD);

	info.nVK		= DEFAULT_DELETE_WORD_KEY;
	info.strSpec	= DEFAULT_DELETE_WORD_SPEC;

	m_arrShortCuts.SetAt(I_DELETE_WORD, info);

	/////////////////////////////////////////////////////////
	
	info = m_arrShortCuts.GetAt(I_INSERT_WORD);

	info.nVK		= DEFAULT_INSERT_WORD_KEY;
	info.strSpec	= DEFAULT_INSERT_WORD_SPEC;

	m_arrShortCuts.SetAt(I_INSERT_WORD, info);

	/////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_SAY_WORD);

	info.nVK		= DEFAULT_SAY_WORD_KEY;
	info.strSpec	= DEFAULT_SAY_WORD_SPEC;

	m_arrShortCuts.SetAt(I_SAY_WORD, info);

	/////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_PAGEUP);

	info.nVK		= DEFAULT_PAGE_UP_KEY;
	info.strSpec	= DEFAULT_PAGE_UP_SPEC;

	m_arrShortCuts.SetAt(I_PAGEUP, info);

	/////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_PAGEDWN);

	info.nVK		= DEFAULT_PAGE_DOWN_KEY;
	info.strSpec	= DEFAULT_PAGE_DOWN_SPEC;

	m_arrShortCuts.SetAt(I_PAGEDWN, info);

	/////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_UPARR);

	info.nVK		= DEFAULT_UPARROW_KEY;
	info.strSpec	= DEFAULT_UPARROW_SPEC;

	m_arrShortCuts.SetAt(I_UPARR, info);

	/////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_DWNARR);

	info.nVK		= DEFAULT_DOWNARROW_KEY;
	info.strSpec	= DEFAULT_DOWNARROW_SPEC;

	m_arrShortCuts.SetAt(I_DWNARR, info);

	/////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_LFTARR);

	info.nVK		= DEFAULT_LEFTARROW_KEY;
	info.strSpec	= DEFAULT_LEFTARROW_SPEC;

	m_arrShortCuts.SetAt(I_LFTARR, info);

	/////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_RGHTARR);

	info.nVK		= DEFAULT_RIGHTARROW_KEY;
	info.strSpec	= DEFAULT_RIGHTARROW_SPEC;

	m_arrShortCuts.SetAt(I_RGHTARR, info);

	/////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_UNDO);

	info.nVK		= DEFAULT_UNDO_KEY;
	info.strSpec	= DEFAULT_UNDO_SPEC_KEY;

	m_arrShortCuts.SetAt(I_UNDO, info);

	/////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_SHOWLIST);

	info.nVK		= DEFAULT_SHOWLIST_KEY;
	info.strSpec	= DEFAULT_SHOWLIST_SPEC_KEY;

	m_arrShortCuts.SetAt(I_SHOWLIST, info);

	/////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_PARTUNDO);

	info.nVK		= DEFAULT_PART_EXPAN_KEY;
	info.strSpec	= DEFAULT_PART_EXPAN_SPEC_KEY;

	m_arrShortCuts.SetAt(I_PARTUNDO, info);

	/////////////////////////////////////////////////////////

	info = m_arrShortCuts.GetAt(I_FULLUNDO);

	info.nVK		= DEFAULT_FULL_EXPAN_KEY;
	info.strSpec	= DEFAULT_FULL_EXPAN_SPEC_KEY;

	m_arrShortCuts.SetAt(I_FULLUNDO, info);

	/////////////////////////////////////////////////////////
//WA0016
	info = m_arrShortCuts.GetAt(I_ABBREVBTN);

	info.nVK		= DEFAULT_ABBREV_BTN_HOTKEY;
	info.strSpec	= DEFAULT_ABBREVBTN_HOTKEY_SPEC;	//WA0021

	m_arrShortCuts.SetAt(I_FULLUNDO, info);

	/////////////////////////////////////////////////////////
}
