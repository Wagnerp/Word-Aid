////////////////////////////////////////////////////////////////////////////////
//CKeyEdit - Version 1.0
//
//Specification
//
//To create a edit box that will capture a combination of system keys from a 
//choice of CONTROL, SHIFT and ALT and then add a character key that represents 
//a accelerator key combination.  It should be able to handle any other key ie 
//function keys to be the character key.
//
//Comments
//
//Key Combinations and where the message is sent to.
//
//The ALT key when pressed is captured in the WM_SYSKEYDOWN message.  If you 
//press a character along with this it does not produce a WM_CHAR, but only 
//WM_KEYDOWN and WM_KEYUP messages.  The code to display which keys and 
//combination of system keys has been pressed has to be captured in WM_CHAR 
//(it probably does not have to be but its where I am as) to stop the character
//typed by the user being entered into the edit box you should not send 
//nothing on from WM_CHAR.
//
//The 'Control' and 'Shift' keys state are got in WM_CHAR by using the API 
//function GetKeyState() and the ALT key is being set via a boolean.  I 
//probably do not have to do this but just make sure that when the ALT key
//is pressed with a character key that is is processed by the WM_CHAR message 
//handler.
//
//The other things that I found to be a problem were that if you pressed 
//SHIFT+ALT the ALT keys keyup message would be sent to WM_SYSKEYDOWN but the 
//keyup would be sent to WM_KEYUP.  This was the opposite for CTRL+ALT where 
//the keydown was sent to WM_KEYDOWN and the keyup to the WM_SYSKEYUP.  
//
//The best way to see this effect is to use Spy++ and look at the 
//different possible combinations a where the keys are sent to.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// KeyEdit Version 0.2	: Created 20/05/02 By Jason Walsh
//						: Updated 30/05/02 by Jason Walsh
//						: Added - Get And Set the Character as Virtual keys
//						: and a function to convert a Virtual key to a char.
// Version 0.3			: Updated 25/06/02 by Jason Walsh
//						: Added - Functionality so that different special keys 
//						: can be stopped from being used in the edit box. 
// Version 0.4			: Updated 03/07/02 by Jason Walsh
//						: Fixed - Allows for use with Win 9x.  For NT, 2000 and 
//						: XP versions use ACE_UNICODE.  If this is not included
//						: there is only support for ENgilsh(United Kingdom) 
//						: keyboards.
// Version 0.5			: Updated 15/07/02 by Jason Walsh
//						: Added - Functionality so that HOME, END, PAGEUP, 
//						: PAGEDOWN, LEFT, RIGHT, UP, DOWN, INSERT and DELETE
//						: can be handled by the key edit control. Changed the
//						: name of GetFunctionKey to GetOtherKey.
////////////////////////////////////////////////////////////////////////////////

#pragma once

// Const defination for setting the Special key pressed
const CString SHIFT_DOWN	= _T("s");
const CString ALT_DOWN		= _T("a");
const CString CTRL_DOWN		= _T("c");


// CKeyEdit

class CKeyEdit : public CEdit
{
	DECLARE_DYNAMIC(CKeyEdit)

public:
	CKeyEdit();
	virtual ~CKeyEdit();

	CString GetSpecialKeysPressed();				// Returns in a string which special keys have been pressed
	BOOL SetSpecialKeysPressed(CString szSpecKey);	// Set which special keys are set in short cut 		

	BOOL GetALTPressed() {return m_bAltPressed;}
	BOOL GetCONTOLPressed() {return m_bCtrlPressed;}
	BOOL GetSHIFTPressed() {return m_bShiftPressed;}
	CString GetCHARPressed() {return m_szCharPressed;}
	UINT GetVKPressed()	{return m_nVKPressed;}
	void SetALTPressed(BOOL bALTPressed = TRUE) {m_bAltPressed = bALTPressed;}
	void SetCONTROLPressed(BOOL bCONTROLPressed = TRUE) {m_bCtrlPressed = bCONTROLPressed;}
	void SetSHIFTPressed(BOOL bSHIFTPressed = TRUE) {m_bShiftPressed = bSHIFTPressed;}
	void SetCHARPressed(CString szCHAR);
	void SetVKPressed(UINT nVK);
	void DisplayShortCut();

	void SHIFTOn(BOOL bOn = TRUE) {m_bSHIFTOn = bOn;}
	void ALTOn(BOOL bOn = TRUE) {m_bALTOn = bOn;}
	void CONTROLOn(BOOL bOn = TRUE) {m_bCONTROLOn = bOn;}

	void ClearEditBox();

protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); 
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags); 
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags); 
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags); 

	DECLARE_MESSAGE_MAP()

	CString GetOtherKey(UINT nChar);

	CString m_szKeyPressed;
	CString m_szKey;
	BOOL m_bAltDown;

	// Used to tell what has been pressed
	BOOL m_bAltPressed;
	BOOL m_bCtrlPressed;
	BOOL m_bShiftPressed;
	CString m_szCharPressed;
	UINT m_nVKPressed;				// Virtual Key Pressed
	BOOL m_bSHIFTOn;				// TRUE if displaying SHIFT
	BOOL m_bALTOn;					// TRUE if displaying ALT
	BOOL m_bCONTROLOn;				// TRUE if display CONTROL
};


