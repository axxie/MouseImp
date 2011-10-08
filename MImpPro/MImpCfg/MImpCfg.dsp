# Microsoft Developer Studio Project File - Name="MImpCfg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MImpCfg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MImpCfg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MImpCfg.mak" CFG="MImpCfg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MImpCfg - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MImpCfg - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MImpCfg - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /D "_MBCS" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "__RELEASE_CFG" /Yu"stdafx.h" /FD /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "__CRYPT"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 DELAYIMP.LIB /nologo /subsystem:windows /machine:I386 /out:"..//Release/MIProCfg.exe" /DELAYLOAD:WINSPOOL.DRV /DELAYLOAD:version.dll /DELAYLOAD:SHELL32.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MImpCfg - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL" /d "__CRYPT"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"..//Debug/MIProCfg.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "MImpCfg - Win32 Release"
# Name "MImpCfg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MImpCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\MImpCfg.h
# End Source File
# Begin Source File

SOURCE=.\MImpCfgMainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MImpCfgMainDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewsShowDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\NewsShowDialog.h
# End Source File
# Begin Source File

SOURCE=.\QuickTourDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickTourDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\arrows.bmp
# End Source File
# Begin Source File

SOURCE=.\res\check_bmp.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hand_cur.cur
# End Source File
# Begin Source File

SOURCE=.\res\logo.png
# End Source File
# Begin Source File

SOURCE=.\res\logo1.png
# End Source File
# Begin Source File

SOURCE=.\res\logo2.png
# End Source File
# Begin Source File

SOURCE=.\res\logo3.png
# End Source File
# Begin Source File

SOURCE=.\res\logo4.png
# End Source File
# Begin Source File

SOURCE=.\res\logo5.png
# End Source File
# Begin Source File

SOURCE=.\res\MImpCfg.ico
# End Source File
# Begin Source File

SOURCE=.\MImpCfg.rc
# End Source File
# Begin Source File

SOURCE=.\res\MImpCfg.rc2
# End Source File
# Begin Source File

SOURCE=.\res\patt_type.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Quick0.png
# End Source File
# Begin Source File

SOURCE=.\res\Quick1.png
# End Source File
# Begin Source File

SOURCE=.\res\Quick1r.png
# End Source File
# Begin Source File

SOURCE=.\res\Quick2.png
# End Source File
# Begin Source File

SOURCE=.\res\Quick2r.png
# End Source File
# Begin Source File

SOURCE=.\res\Quick3.png
# End Source File
# Begin Source File

SOURCE=.\res\Quick3r.png
# End Source File
# Begin Source File

SOURCE=.\res\Quick4.png
# End Source File
# Begin Source File

SOURCE=.\res\Quick4r.png
# End Source File
# Begin Source File

SOURCE=.\res\Quick5.png
# End Source File
# Begin Source File

SOURCE=.\res\Quick5r.png
# End Source File
# Begin Source File

SOURCE=.\res\Quick6.png
# End Source File
# Begin Source File

SOURCE=.\res\Quick6r.png
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\res\title_logo.png
# End Source File
# Begin Source File

SOURCE=.\res\TVLogo.BMP
# End Source File
# Begin Source File

SOURCE=.\res\wnd_clear.ico
# End Source File
# Begin Source File

SOURCE=.\res\wnd_cur.ico
# End Source File
# Begin Source File

SOURCE=.\res\wnd_find.cur
# End Source File
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Group "HyperUi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BmpStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\BmpStatic.h
# End Source File
# Begin Source File

SOURCE=.\ClickStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\ClickStatic.h
# End Source File
# Begin Source File

SOURCE=.\hyperlink.cpp
# End Source File
# Begin Source File

SOURCE=.\hyperlink.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AppConst.cpp
# End Source File
# Begin Source File

SOURCE=.\AppConst.h
# End Source File
# Begin Source File

SOURCE=.\MyListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\MyListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\RegShowWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\RegShowWnd.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Pages"

# PROP Default_Filter ""
# Begin Group "WndPattern"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AutoHidePatterns.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoHidePatterns.h
# End Source File
# Begin Source File

SOURCE=.\SelWndPatternDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelWndPatternDlg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AboutPage.cpp
# End Source File
# Begin Source File

SOURCE=.\AboutPage.h
# End Source File
# Begin Source File

SOURCE=.\ChildDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildDlg.h
# End Source File
# Begin Source File

SOURCE=.\General.cpp
# End Source File
# Begin Source File

SOURCE=.\General.h
# End Source File
# Begin Source File

SOURCE=.\OdometerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OdometerDlg.h
# End Source File
# Begin Source File

SOURCE=.\ScrollPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ScrollPage.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\MICrptInfo.h
# End Source File
# Begin Source File

SOURCE=..\MICrptInfoImpl.h
# End Source File
# Begin Source File

SOURCE=..\MIGlobal.h
# End Source File
# End Target
# End Project
