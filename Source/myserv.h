// myserv.h

#ifndef _MYSERV_
#define _MYSERV_

#include "stddde.h"


typedef HANDLE  HDDE;

class CMyStringItem : public CDDEStringItem
{
protected:
    virtual void OnPoke();
};

class CMyTopic : public CDDETopic
{
public:
    CMyTopic();
    virtual BOOL Exec(void* pData, DWORD dwSize);

protected:
	BOOL OpenAlpha(CString strFilename);
	BOOL OpenTopic(CString strFilename);
	BOOL OpenAbbrev(CString strFilename);
	BOOL UndoFull();
	BOOL UndoPart();
	BOOL InsertAlpha();
	BOOL DeleteAlpha();
	BOOL InsertTopic();
	BOOL DeleteTopic();
	BOOL FirstUppercase();		// WA0057
	BOOL AllUppercase();		// WA0057
	BOOL AllLowercase();		// WA0057
};

class CMyServer : public CDDEServer
{
public:
    CMyServer();
    virtual ~CMyServer();
 //   virtual void Status(const char* pszFormat, ...);
    virtual BOOL OnCreate();

	BOOL CMyServer::SendDDEMessage(LPCSTR lpszTopic, LPCSTR lpszItem, CString szTextList);

public:
	CMyTopic m_Alpha;
	CMyTopic m_Abbrev;
	CMyTopic m_Topic;
	CMyTopic m_AlphaTopic;
	CMyTopic m_AbbrevTopic;
	CMyTopic m_AbbrevAlpha;
	CMyTopic m_AbbrevAlphaTopic;

    CMyStringItem m_StringItem1;
    CMyStringItem m_StringItem2;
protected:

	CString m_szTextList;			// Stores list of words to send to client

	BOOL m_bDDEAbbrev;
	BOOL m_bDDEAlpha;
	BOOL m_bDDETopic;

	CDDETopic* FindTopic(const char* pszTopic);
};

#endif // _MYSERV_
