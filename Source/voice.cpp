// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "voice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// IVoice properties

/////////////////////////////////////////////////////////////////////////////
// IVoice operations

short IVoice::GetAge()
{
	short result;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
	return result;
}

short IVoice::GetGender()
{
	short result;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
	return result;
}

long IVoice::GetLanguage()
{
	long result;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

CString IVoice::GetManufacturerName()
{
	CString result;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVoice::GetVoiceName()
{
	CString result;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVoice::GetSpeaker()
{
	CString result;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVoice::GetStyle()
{
	CString result;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

void IVoice::SetAge(short age)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 age);
}

void IVoice::SetGender(short gender)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 gender);
}

void IVoice::SetLanguage(long language, long dialect)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 language, dialect);
}

long IVoice::GetDialect()
{
	long result;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// CVoice properties

/////////////////////////////////////////////////////////////////////////////
// CVoice operations

short CVoice::GetAge()
{
	short result;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
	return result;
}

short CVoice::GetGender()
{
	short result;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
	return result;
}

long CVoice::GetLanguage()
{
	long result;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

CString CVoice::GetManufacturerName()
{
	CString result;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString CVoice::GetVoiceName()
{
	CString result;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString CVoice::GetSpeaker()
{
	CString result;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString CVoice::GetStyle()
{
	CString result;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

void CVoice::SetAge(short age)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 age);
}

void CVoice::SetGender(short gender)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 gender);
}

void CVoice::SetLanguage(long language, long dialect)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 language, dialect);
}

long CVoice::GetDialect()
{
	long result;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}
