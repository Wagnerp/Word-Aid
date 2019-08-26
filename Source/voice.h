// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// IVoice wrapper class

class IVoice : public COleDispatchDriver
{
public:
	IVoice() {}		// Calls COleDispatchDriver default constructor
	IVoice(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVoice(const IVoice& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	short GetAge();
	short GetGender();
	long GetLanguage();
	CString GetManufacturerName();
	CString GetVoiceName();
	CString GetSpeaker();
	CString GetStyle();
	void SetAge(short age);
	void SetGender(short gender);
	void SetLanguage(long language, long dialect);
	long GetDialect();
};
/////////////////////////////////////////////////////////////////////////////
// CVoice wrapper class

class CVoice : public COleDispatchDriver
{
public:
	CVoice() {}		// Calls COleDispatchDriver default constructor
	CVoice(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CVoice(const CVoice& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	short GetAge();
	short GetGender();
	long GetLanguage();
	CString GetManufacturerName();
	CString GetVoiceName();
	CString GetSpeaker();
	CString GetStyle();
	void SetAge(short age);
	void SetGender(short gender);
	void SetLanguage(long language, long dialect);
	long GetDialect();
};
