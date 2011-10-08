# Microsoft Developer Studio Project File - Name="SLibExc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SLibExc - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SLibExc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SLibExc.mak" CFG="SLibExc - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SLibExc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SLibExc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SLibExc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /D "_LIB" /D "WIN32" /D "NDEBUG" /D "__RELEASE_CFG" /D "__CRYPT" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SLibExc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "SLibExc - Win32 Release"
# Name "SLibExc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SLAutoPtr.cpp
# End Source File
# Begin Source File

SOURCE=.\SLCfgReg.cpp
# End Source File
# Begin Source File

SOURCE=.\SLCfgStr.cpp
# End Source File
# Begin Source File

SOURCE=.\SLCont.cpp
# End Source File
# Begin Source File

SOURCE=.\SLDbg.cpp
# End Source File
# Begin Source File

SOURCE=.\SLDlgMem.cpp
# End Source File
# Begin Source File

SOURCE=.\SLFileMap.cpp
# End Source File
# Begin Source File

SOURCE=.\SLFileName.cpp
# End Source File
# Begin Source File

SOURCE=.\SLMemMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SLPrc.cpp
# End Source File
# Begin Source File

SOURCE=.\SLResMem.cpp
# End Source File
# Begin Source File

SOURCE=.\SLString.cpp
# End Source File
# Begin Source File

SOURCE=.\SLThunk.cpp
# End Source File
# Begin Source File

SOURCE=.\SLVerInfo.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SLAutoPtr.h
# End Source File
# Begin Source File

SOURCE=.\SLCfgReg.h
# End Source File
# Begin Source File

SOURCE=.\SLCfgStr.h
# End Source File
# Begin Source File

SOURCE=.\SLCont.h
# End Source File
# Begin Source File

SOURCE=.\SLDbg.h
# End Source File
# Begin Source File

SOURCE=.\SLDlgMem.h
# End Source File
# Begin Source File

SOURCE=.\SLFileMap.h
# End Source File
# Begin Source File

SOURCE=.\SLFileName.h
# End Source File
# Begin Source File

SOURCE=.\SLMemMgr.h
# End Source File
# Begin Source File

SOURCE=.\SLPrc.h
# End Source File
# Begin Source File

SOURCE=.\SLResMem.h
# End Source File
# Begin Source File

SOURCE=.\SLString.h
# End Source File
# Begin Source File

SOURCE=.\SLThunk.h
# End Source File
# Begin Source File

SOURCE=.\SLUtil.h
# End Source File
# Begin Source File

SOURCE=.\SLVerInfo.h
# End Source File
# End Group
# End Target
# End Project
