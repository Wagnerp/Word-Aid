# Microsoft Developer Studio Project File - Name="WordAid2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WordAid2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WordAid2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WordAid2.mak" CFG="WordAid2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WordAid2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WordAid2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/WordAid2 Beta", VUEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WordAid2 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WORDAID_APP" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "WordAid2 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WORDAID_APP" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WordAid2 - Win32 Release"
# Name "WordAid2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AbbrevEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AbbrevExpansion.cpp
# End Source File
# Begin Source File

SOURCE=.\AbbrevProcessKey.cpp
# End Source File
# Begin Source File

SOURCE=.\AddDelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvancedKeyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ButtonSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorPickerCB.cpp
# End Source File
# Begin Source File

SOURCE=.\DefaultProcessKey.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\DownButton.cpp
# End Source File
# Begin Source File

SOURCE=.\DummyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditWordDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\KeySettingsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\MergeFileSelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MergeIntoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MergeSelectWordDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MergeTypeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Myserv.cpp
# End Source File
# Begin Source File

SOURCE=.\NewListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Setting.cpp
# End Source File
# Begin Source File

SOURCE="..\..\..\..\ace software\controls\Tree Dialog\SettingTreeCtrl.cpp"
# End Source File
# Begin Source File

SOURCE=.\SpeechSettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeechSetupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\stddde.cpp
# End Source File
# Begin Source File

SOURCE=.\StopButton.cpp
# End Source File
# Begin Source File

SOURCE=.\textspeaker.cpp
# End Source File
# Begin Source File

SOURCE="..\..\..\..\ace software\controls\Tree Dialog\TreeDialog.cpp"
# End Source File
# Begin Source File

SOURCE=.\UpButton.cpp
# End Source File
# Begin Source File

SOURCE=.\VKConversion.cpp
# End Source File
# Begin Source File

SOURCE=.\voice.cpp
# End Source File
# Begin Source File

SOURCE=.\WAButton.cpp
# End Source File
# Begin Source File

SOURCE=.\WorAidAbbrevProccessKey.cpp
# End Source File
# Begin Source File

SOURCE=".\Word Aid 2.cpp"
# End Source File
# Begin Source File

SOURCE=".\Word Aid 2.rc"
# End Source File
# Begin Source File

SOURCE=".\Word Aid 2Dlg.cpp"
# End Source File
# Begin Source File

SOURCE=.\WordAidProcessKey.cpp
# End Source File
# Begin Source File

SOURCE=.\WordAidSettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WordAidTreeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\WordDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Wordlist.cpp
# End Source File
# Begin Source File

SOURCE=.\WordSettings.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AbbrevEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\AbbrevExpansion.h
# End Source File
# Begin Source File

SOURCE=.\AbbrevProcessKey.h
# End Source File
# Begin Source File

SOURCE=.\AddDelDlg.h
# End Source File
# Begin Source File

SOURCE=.\AdvancedKeyDlg.h
# End Source File
# Begin Source File

SOURCE=.\ButtonSettings.h
# End Source File
# Begin Source File

SOURCE=.\ChildWnd.h
# End Source File
# Begin Source File

SOURCE=.\ColorPickerCB.h
# End Source File
# Begin Source File

SOURCE=.\DefaultKeys.h
# End Source File
# Begin Source File

SOURCE=.\DefaultProcessKey.h
# End Source File
# Begin Source File

SOURCE=.\DlgToolBar.h
# End Source File
# Begin Source File

SOURCE=.\DownButton.h
# End Source File
# Begin Source File

SOURCE=.\DummyDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditWordDlg.h
# End Source File
# Begin Source File

SOURCE=.\GeneralSettings.h
# End Source File
# Begin Source File

SOURCE=.\KeyEdit.h
# End Source File
# Begin Source File

SOURCE=.\Keyhook.h
# End Source File
# Begin Source File

SOURCE=.\KeySettingsPage.h
# End Source File
# Begin Source File

SOURCE=.\MergeFileSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\MergeIntoDlg.h
# End Source File
# Begin Source File

SOURCE=.\MergeSelectWordDlg.h
# End Source File
# Begin Source File

SOURCE=.\MergeTypeDlg.h
# End Source File
# Begin Source File

SOURCE=.\myserv.h
# End Source File
# Begin Source File

SOURCE=.\NewListDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProgressWnd.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SearchListBox.h
# End Source File
# Begin Source File

SOURCE=.\Setting.h
# End Source File
# Begin Source File

SOURCE=.\SettingIndex.h
# End Source File
# Begin Source File

SOURCE=.\SettingStruct.h
# End Source File
# Begin Source File

SOURCE="..\..\..\..\ace software\controls\Tree Dialog\SettingTreeCtrl.h"
# End Source File
# Begin Source File

SOURCE=.\SpeechSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\SpeechSetupDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\stddde.h
# End Source File
# Begin Source File

SOURCE=.\StopButton.h
# End Source File
# Begin Source File

SOURCE=.\textspeaker.h
# End Source File
# Begin Source File

SOURCE="..\..\..\..\ace software\controls\Tree Dialog\TreeDialog.h"
# End Source File
# Begin Source File

SOURCE=.\UpButton.h
# End Source File
# Begin Source File

SOURCE=.\VKConversion.h
# End Source File
# Begin Source File

SOURCE=.\voice.h
# End Source File
# Begin Source File

SOURCE=.\WAButton.h
# End Source File
# Begin Source File

SOURCE=.\WorAidAbbrevProccessKey.h
# End Source File
# Begin Source File

SOURCE=".\Word Aid 2Dlg.h"
# End Source File
# Begin Source File

SOURCE=.\WordAid2.h
# End Source File
# Begin Source File

SOURCE=.\WordAidKeyStructs.h
# End Source File
# Begin Source File

SOURCE=.\WordAidProcessKey.h
# End Source File
# Begin Source File

SOURCE=.\WordAidSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\WordAidTreeDialog.h
# End Source File
# Begin Source File

SOURCE=.\WordDlg.h
# End Source File
# Begin Source File

SOURCE=.\Wordlist.h
# End Source File
# Begin Source File

SOURCE=.\WordSettings.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\alpha2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\arrow_up.bmp
# End Source File
# Begin Source File

SOURCE=.\res\arrowDown.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\res\merge.bmp
# End Source File
# Begin Source File

SOURCE=.\res\merge.gif
# End Source File
# Begin Source File

SOURCE=.\res\merge.jpg
# End Source File
# Begin Source File

SOURCE=.\res\SettingTree.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stop.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\topic.bmp
# End Source File
# Begin Source File

SOURCE=".\res\Word Aid 2.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Word Aid 2.rc2"
# End Source File
# Begin Source File

SOURCE=.\res\wordaid.ico
# End Source File
# Begin Source File

SOURCE=.\res\words.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=".\bug fixes.txt"
# End Source File
# Begin Source File

SOURCE=".\Build Notes.txt"
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\vssver.scc
# End Source File
# End Target
# End Project
