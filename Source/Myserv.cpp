// myserv.cpp

#include "stdafx.h"
#include "myserv.h"

#include "dde.h"

#define ETX 0x03  //control-C separator

#define MYDDE_CBWNDEXTRA    2   /* Total Extra Bytes per Dde Window */
#define GWL_PMYDDEDATA      0   /* offset of handle to pMyDdeData */

#define MAXAPPLICATION      32
#define MAXTOPIC            32
#define MAXITEM             32
#define MAXTERMINATE        40
#define MAXCONVERSATIONS    50

typedef enum _tagACKPENDING
{
    NONE,
    ADVISE,
    UNADVISE,
    POKE,
    REQUEST,
    DATA,
    EXECUTE,
    INITIATE,
    TERMINATE
} ACKPENDING;

typedef struct _tagTERM
{
    short nCount;
    HWND  hWnd[MAXTERMINATE];
} TERM;
typedef TERM *PTERM;

BOOL TxServerAdviseData (HANDLE hData, short nItemSize, HWND hWnd);
BOOL   duServerSetText   (HWND hWndMain,
                         LPCSTR lpszApplication, LPCSTR lpszTopic,
                         LPCSTR lpszItem, LPCSTR lpszText);

static short nConv;
static HDDE  hDdeConv[MAXCONVERSATIONS];
static short    nTimeout = 20;     // 20 * 100mS = 2 seconds

#include "wordaid2.h"

CMyServer::CMyServer()
{
	m_bDDEAbbrev	= FALSE;
	m_bDDEAlpha		= FALSE;
	m_bDDETopic		= FALSE;
}

CMyServer::~CMyServer()
{


}

/*void CMyServer::Status(const char* pszFormat, ...)
{
    char buf[1024];
	va_list arglist;
	va_start(arglist, pszFormat);
    vsprintf(buf, pszFormat, arglist);
	va_end(arglist);

    STATUS(buf);
}*/

BOOL CMyServer::OnCreate()
{
    //
    // Add our own topics and items
    //

	m_Abbrev.Create("abbrev");
	AddTopic(&m_Abbrev);

	m_Abbrev.AddItem(&m_StringItem1);

	/////////////////////////////////////////

	m_Alpha.Create("alpha");
	AddTopic(&m_Alpha);

	m_Alpha.AddItem(&m_StringItem1);
    
	/////////////////////////////////////////

	m_Topic.Create("topic");
	AddTopic(&m_Topic);

	m_Topic.AddItem(&m_StringItem1);

	/////////////////////////////////////////

	m_AlphaTopic.Create("topic/alpha");
    AddTopic(&m_AlphaTopic);

    m_StringItem1.Create("wordlist");
    m_AlphaTopic.AddItem(&m_StringItem1);

    /////////////////////////////////////////

	m_AbbrevAlpha.Create("alpha/abbrev");
    AddTopic(&m_AbbrevAlpha);

    m_StringItem1.Create("wordlist");
    m_AbbrevAlpha.AddItem(&m_StringItem1);

	/////////////////////////////////////////

	m_AbbrevTopic.Create("topic/abbrev");
    AddTopic(&m_AbbrevTopic);

    m_StringItem1.Create("wordlist");
    m_AbbrevTopic.AddItem(&m_StringItem1);

	/////////////////////////////////////////

	m_AbbrevAlphaTopic.Create("topic/alpha/abbrev");
    AddTopic(&m_AbbrevAlphaTopic);

    m_StringItem1.Create("wordlist");
    m_AbbrevAlphaTopic.AddItem(&m_StringItem1);

    //
    // Set up some data in the strings
    //7

/*	CString szText;
	
	szText = "One";
	szText += ETX;
	szText += "Two";

    m_StringItem1.SetData(szText);
    m_StringItem2.SetData("This is string 2");
*/
    return TRUE;
}

BOOL CMyServer::SendDDEMessage(LPCSTR lpszTopic, LPCSTR lpszItem, CString szTextList)
{
	HSZ hTopic, hItem;
	LPSTR lpTopic, lpItem;
	BOOL bReturn = TRUE;
	CString strTopic = lpszTopic;

	// Set Text list to new one
	if((strTopic.Find("topic") != -1 && m_bDDETopic) ||
		(strTopic.Find("alpha")!= -1 && m_bDDEAlpha) ||
		(strTopic.Find("abbrev") != -1 && m_bDDEAbbrev))
	{
		m_StringItem1.SetData(szTextList);
		m_szTextList = szTextList;
	}
	else
	{
		m_StringItem1.SetData(_T(""));
		m_szTextList = _T("");
	}

	// Create request to send new message to Client
	hTopic = (HSZ)GlobalAlloc (GHND, (LONG) lstrlen (lpszTopic) + 1);

	if(hTopic)
	{
		lpTopic = (LPSTR)GlobalLock (hTopic);

		if(lpTopic != 0L)
		{
			lstrcpy (lpTopic, lpszTopic);
			GlobalUnlock (hTopic);

			hItem = (HSZ)GlobalAlloc(GHND, (LONG)lstrlen (lpszItem) + 1);

			if(hItem)
			{
				lpItem = (LPSTR)GlobalLock(hItem);

				if(lpItem != 0L)
				{
					lstrcpy(lpItem, lpszItem);
					GlobalUnlock(hItem);

					if(!::DdePostAdvise(m_dwDDEInstance, NULL, NULL))//hTopic, hItem))
					{
						bReturn = FALSE;
					//	AfxMessageBox(_T("Didn't work"));
					}
				}

				GlobalFree(hItem);
			}
		}
		GlobalFree(hTopic);
	}

	return bReturn;
}

CDDETopic* CMyServer::FindTopic(const char* pszTopic)
{
    POSITION pos = m_TopicList.GetHeadPosition();
    while (pos) {
        CDDETopic* pTopic = m_TopicList.GetNext(pos);
        if (pTopic->m_strName.CompareNoCase(pszTopic) == 0)
		{
			CString strTopic = pszTopic;

			if(strTopic.Find("abbrev") != -1)
			{
				m_bDDEAbbrev = TRUE;
			}
			else
			{
				m_bDDEAbbrev = FALSE;
			}

			if(strTopic.Find("alpha") != -1)
			{
				m_bDDEAlpha = TRUE;
			}
			else 
			{
				m_bDDEAlpha = FALSE;
			}

			if(strTopic.Find("topic") != -1)
			{
				m_bDDETopic = TRUE;
			}
			else
			{
				m_bDDETopic = FALSE;
			}

			return pTopic;
		}
    }
    return NULL;
}

/*
BOOL  duServerSetText   (HWND hWndMain,
                         LPCSTR lpszApplication, LPCSTR lpszTopic,
                         LPCSTR lpszItem, LPCSTR lpszText)
{
//    short i;
//    PMYDDEDATA  pMyDdeData;
//    HDDE hDde;

    HANDLE hData;
    short nItemSize;
    LPSTR lpData;
   BOOL bSuccess;
    short nMatch;*/

    /* if (!lstrlen (lpszItem))
        return (FALSE); */

    /* setup nItemSize and hData */
    /*nItemSize = lstrlen (lpszText) + 1;     // one for null

    lpData = 0L;*/
    //hData = GlobalAlloc (GHND, (LONG) nItemSize);
    //if (hData)
    //    lpData = (LPSTR)GlobalLock (hData);

    //if (lpData == 0L)
      //  return (FALSE);

    //lstrcpy (lpData, lpszText);
    //GlobalUnlock (hData);

    /* copy data to all matching servers, idle or not */
    //nMatch = 0;

    /* Scan for Server that matches */
   // for (i = 0; i < nConv; i++)
//    {
    //    hDde = hDdeConv[i];

      //  pMyDdeData = (PMYDDEDATA) GetWindowLong ((HWND)hDde, GWL_PMYDDEDATA);
      //  if (pMyDdeData && pMyDdeData->fServer)
      //  {
            /* got server */
        //    BOOL bCheckAdvise = FALSE;

            /* does it belong to calling hWndMain ?? */
          //  if (hWndMain != pMyDdeData->hWndParent)
            //    continue;       // no match
            
       //     if (lstrcmpi (lpszApplication, pMyDdeData->szApplication) != 0)
         //       continue;       // no match

           // if (lstrcmpi (lpszTopic, pMyDdeData->szTopic) != 0)
             //   continue;       // no match

            /* we have match */
           // nMatch++;

           // if (lstrcmpi (lpszItem, pMyDdeData->szItem) == 0)
             //   bCheckAdvise = TRUE;    // update on same item
            //else
           // {
             //   // change single item name supported
               // lstrcpy (pMyDdeData->szItem, lpszItem);
              //  pMyDdeData->fAdvise = FALSE;
//                UpdateDebug (pMyDdeData);   REMOVE MAY NEED ADDING BACK
            //}

            /* all matching app/topic share single hData */
//            if (nMatch == 1)                        // first match
  //              if (pMyDdeData->hItemData)          // not null
    //                GlobalFree (pMyDdeData->hItemData);

      //      pMyDdeData->hItemData = hData;          // copy
        //    pMyDdeData->nItemSize = nItemSize;      // copy

          //  if (
            //    (bCheckAdvise) &&
              //  (pMyDdeData->fServerIdle == FALSE) &&
              //  (pMyDdeData->fAdvise)
              // )
           // {
                /* send out advise for this client */
   /*             TxServerAdviseData (hData, nItemSize, hWndMain);
            }

//            continue;       // found match and handled it
                            // there may be other matches
//        }
//    }

    if (nMatch)
        bSuccess = TRUE;
    else
    {
        GlobalFree (hData);     // not been used
        bSuccess = FALSE;
    }

    return (bSuccess);
}*/

/*BOOL TxServerAdviseData (HANDLE hData, short nItemSize, HWND hWnd)
{
    ATOM  aItem;
    LPSTR lpItemData;
    HANDLE hDdeData;
    DDEDATA FAR *lpDdeData;
    BOOL bSuccess;

    bSuccess = FALSE;

    /* Server Initiated routine */
	
    /* Alloc DdeData First */
    /* DdeData */
  /*  lpDdeData = 0L;
    hDdeData = GlobalAlloc (GHND | GMEM_DDESHARE,
                           (LONG) sizeof (DDEDATA) + nItemSize);
    if (hDdeData)
        lpDdeData = (DDEDATA FAR *) GlobalLock (hDdeData);
    if (lpDdeData == 0L)
        return (FALSE);

    /* ItemData */
    /*lpItemData = 0L;
    if (hData)
	{
        lpItemData = (LPSTR)GlobalLock (hData);
	}
    if (lpItemData == 0L)
    {
        GlobalUnlock (hDdeData);
        GlobalFree   (hDdeData);
        return (FALSE);
    }

    aItem = GlobalAddAtom ("wordlist");

    lpDdeData->fResponse = FALSE;   // not response to request
    lpDdeData->fRelease  = TRUE;    // get client to release data (if ACK)
    lpDdeData->fAckReq   = TRUE;    // ask for reply
    lpDdeData->cfFormat  = CF_TEXT;

    lstrcpy ((LPTSTR)lpDdeData->Value, lpItemData);
//    GlobalUnlock (pMyDdeData->hItemData);
    GlobalUnlock (hDdeData);

//    if (pMyDdeData->fDeferUpd)
    {
        GlobalFree (hDdeData);
        hDdeData = NULL;
    }

    if (!PostMessage (HWND_BROADCAST,   // wParam,
                      WM_DDE_DATA, (WPARAM)hWnd,
					  PackDDElParam(WM_DDE_DATA, (UINT)hDdeData, GlobalAddAtom(_T("wordlist")))));
    {
        if (hDdeData)
		{
            GlobalFree (hDdeData);
		}
        GlobalDeleteAtom (aItem);
    }
    //else
    {
        
        bSuccess = TRUE;
    }

    return (bSuccess);
}*/

void CMyStringItem::OnPoke()
{
//    STATUS("%s is now %s",
  //         (const char*)m_strName,
    //       GetData());
}

CMyTopic::CMyTopic()
{
}

BOOL CMyTopic::Exec(void* pData, DWORD dwSize)
{
	CString szNum = _T("");
	int nFirstBracket = 0;
	int nSecondBracket = 0;

  // STATUS("Exec: %s", (char*)pData);
	//pData contains [selectword (n)]
	TRACE("word selected is %s\n",(char*)pData);

	CString szItem = (char*)pData;

 	CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;

	// TODO : add to string table
	if(szItem.Find(_T("selectword")) > 0)
	{
		nFirstBracket = szItem.Find(_T("("));
		nSecondBracket = szItem.Find(_T(")"));
	
		szNum = szItem.Mid(nFirstBracket+1, nSecondBracket - nFirstBracket);

		CString str;

		if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)) && 
			atoi(pWnd->m_pSettings->GetSetting(ABBREV_ON)) &&
			!atoi(pWnd->m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
		{
			if(pWnd->m_procWordAidAbbrevKey.IsAbbrevDisplayed())
			{
				pWnd->m_procWordAidAbbrevKey.SelectAbbrev(atoi(szNum)-1);
			}
			else
			{
				int count = pWnd->m_procWordAidAbbrevKey.GetCharsTyped().GetLength();
				pWnd->m_procWordAidAbbrevKey.SelectWord(atoi(szNum)-1, TRUE);//;.SelectWord(atoi(szNum)-1,&str);
			}
		}
		else if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)))
		{
			int count = pWnd->m_procWordAidKey.GetCharsTyped().GetLength();
			pWnd->m_procWordAidKey.SelectWord(atoi(szNum)-1, TRUE);//;.SelectWord(atoi(szNum)-1,&str);
		}
		
	}
	else if(szItem.Find(_T("Undo")) > 0)
	{
		if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)) && 
			atoi(pWnd->m_pSettings->GetSetting(ABBREV_ON)) &&
			!atoi(pWnd->m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
		{
			pWnd->m_procWordAidAbbrevKey.Undo();
		}
		else if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)))
		{
			pWnd->m_procWordAidKey.Undo();
		}
	}
	else if(szItem.Find(_T("OpenAlpha")) > 0)
	{
		return OpenAlpha(szItem);
	}
	else if(szItem.Find(_T("OpenTopic")) > 0)
	{
		return OpenTopic(szItem);
	}
	else if(szItem.Find(_T("OpenAbbrev")) > 0)
	{
		return OpenAbbrev(szItem);
	}
	else if(szItem.Find(_T("UndoFull")) > 0)
	{
		return UndoFull();
	}
	else if(szItem.Find(_T("UndoPart")) > 0)
	{
		return UndoPart();
	}
	else if(szItem.Find(_T("InsertAlpha")) > 0)
	{
		return InsertAlpha();
	}
	else if(szItem.Find(_T("DeleteAlpha")) > 0)
	{
		return DeleteAlpha();
	}
	else if(szItem.Find(_T("InsertTopic")) > 0)
	{
		return InsertTopic();
	}
	else if(szItem.Find(_T("DeleteTopic")) > 0)
	{
		return DeleteTopic();
	}
	else if(szItem.Find(_T("Initialize")) > 0)
	{
		CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;

		if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)) && 
			atoi(pWnd->m_pSettings->GetSetting(ABBREV_ON)))
		{			
			pWnd->m_procWordAidAbbrevKey.ClearTyped();
		}
		else if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)))
		{
			pWnd->m_procWordAidKey.ClearTyped();
		}
		else if(atoi(pWnd->m_pSettings->GetSetting(ABBREV_ON)))
		{
			pWnd->m_procAbbrevKey.ClearTyped();
		}
	}
	else if(szItem.Find(_T("FirstUppercase")) > 0)		// WA0057
	{
		return FirstUppercase();
	}
	else if(szItem.Find(_T("AllUppercase")) > 0 )		// WA0057
	{
		return AllUppercase();
	}
	else if(szItem.Find(_T("AllLowercase")) > 0)		// WA0057
	{
		return AllLowercase();
	}

    return TRUE;

}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Opens an alphabeitc file
////////////////////////////////////////////////////////////////////////////////

BOOL CMyTopic::OpenAlpha(CString strFilename)
{
	CString strExt = _T("");

	// Check to make sure the file that is trying to be opened is 
	// an alphabetic file
	strExt.LoadString(IDS_ALPHA_EXT_TEST);

	if(strFilename.CompareNoCase(strExt) == 0)
	{
		return FALSE;
	}

	// Find File and Path
	int nFirstBracket = strFilename.Find(_T("("));
	int nSecondBracket = strFilename.Find(_T(")"));

	CString strFile = strFilename.Mid(nFirstBracket+1, nSecondBracket - nFirstBracket-1);

	CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;

	return pWnd->OpenAlpha(strFile);
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Opens an Topic file
////////////////////////////////////////////////////////////////////////////////

BOOL CMyTopic::OpenTopic(CString strFilename)
{
	CString strExt = _T("");

	// Check to make sure the file that is trying to be opened is 
	// an Topic file
	strExt.LoadString(IDS_TOPIC_EXT_TEST);

	if(strFilename.CompareNoCase(strExt) == 0)
	{
		return FALSE;
	}

	// Find File and Path
	int nFirstBracket = strFilename.Find(_T("("));
	int nSecondBracket = strFilename.Find(_T(")"));

	CString strFile = strFilename.Mid(nFirstBracket+1, nSecondBracket - nFirstBracket-1);

	CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;

	return pWnd->OpenTopic(strFile);
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Opens an Abbrev file
////////////////////////////////////////////////////////////////////////////////

BOOL CMyTopic::OpenAbbrev(CString strFilename)
{
	CString strExt = _T("");

	// Check to make sure the file that is trying to be opened is 
	// an Topic file
	strExt.LoadString(IDS_ABBREV_EXT_TEST);

	if(strFilename.CompareNoCase(strExt) == 0)
	{
		return FALSE;
	}

	// Find File and Path
	int nFirstBracket = strFilename.Find(_T("("));
	int nSecondBracket = strFilename.Find(_T(")"));

	CString strFile = strFilename.Mid(nFirstBracket+1, nSecondBracket - nFirstBracket-1);

	CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;

	return pWnd->OpenAbbrev(strFile);
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Performs a Un-expand (full)
////////////////////////////////////////////////////////////////////////////////

BOOL CMyTopic::UndoFull()
{
	return ((CWordAid2App*)AfxGetApp())->m_pPopUp->UndoFull();
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Performs an Un-expand (part)
////////////////////////////////////////////////////////////////////////////////

BOOL CMyTopic::UndoPart()
{
	return ((CWordAid2App*)AfxGetApp())->m_pPopUp->UndoPart();
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Inserts the last typed word into the alphabetic wordlist.
//
// RETURNS		: TRUE if the Word was successfully inserted
////////////////////////////////////////////////////////////////////////////////

BOOL CMyTopic::InsertAlpha()
{
	CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;

	if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(pWnd->m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(pWnd->m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		return pWnd->m_procWordAidAbbrevKey.InsertAlpha();
	}
	else if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)))
	{
		return pWnd->m_procWordAidKey.InsertAlpha();
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Delete the last typed word from the alphabetic wordlist.
//
// RETURNS		: TRUE if the Word was successfully deleted
////////////////////////////////////////////////////////////////////////////////

BOOL CMyTopic::DeleteAlpha()
{
	CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;

	if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(pWnd->m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(pWnd->m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		return pWnd->m_procWordAidAbbrevKey.DeleteAlpha();
	}
	else if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)))
	{
		return pWnd->m_procWordAidKey.DeleteAlpha();
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Inserts the last typed word into the Topic wordlist.
//
// RETURNS		: TRUE if the Word was successfully inserted
////////////////////////////////////////////////////////////////////////////////

BOOL CMyTopic::InsertTopic()
{
	CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;

	if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(pWnd->m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(pWnd->m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		return pWnd->m_procWordAidAbbrevKey.InsertTopic();
	}
	else if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)))
	{
		return pWnd->m_procWordAidKey.InsertTopic();
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION	: Delete the last typed word from the Topic wordlist.
//
// RETURNS		: TRUE if the Word was successfully deleted
////////////////////////////////////////////////////////////////////////////////

BOOL CMyTopic::DeleteTopic()
{
	CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;

	if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(pWnd->m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(pWnd->m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		return pWnd->m_procWordAidAbbrevKey.DeleteTopic();
	}
	else if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)))
	{
		return pWnd->m_procWordAidKey.DeleteTopic();
	}

	return FALSE;
}

//! WA0057 - Called to make the wordlist have all the first characters made uppercase

BOOL CMyTopic::FirstUppercase()
{
	CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;

	if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(pWnd->m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(pWnd->m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		pWnd->m_procWordAidAbbrevKey.SendDDEFirstUpperCase();
	}
	else if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)))
	{
		pWnd->m_procWordAidKey.SendDDEFirstUpperCase();
	}

	return TRUE;
}

//! WA0057 - Called to make the wordlist be all uppercase

BOOL CMyTopic::AllUppercase()
{
	CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;

	if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(pWnd->m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(pWnd->m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		pWnd->m_procWordAidAbbrevKey.SendDDEAllUpperCase();
	}
	else if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)))
	{
		pWnd->m_procWordAidKey.SendDDEAllUpperCase();
	}

	return TRUE;

}

//! WA0057 0 Called to make the wordlist be all lowercase

BOOL CMyTopic::AllLowercase()
{
	CChildWnd* pWnd = ((CWordAid2App*)AfxGetApp())->m_pPopUp;

	if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)) &&
		atoi(pWnd->m_pSettings->GetSetting(ABBREV_ON)) &&
		!atoi(pWnd->m_pSettings->GetSetting(ABBREV_LIST_HIDE)))
	{
		pWnd->m_procWordAidAbbrevKey.SendDDEAllLowerCase();
	}
	else if(atoi(pWnd->m_pSettings->GetSetting(WORDAID_ON)))
	{
		pWnd->m_procWordAidKey.SendDDEAllLowerCase();
	}

	return TRUE;
}