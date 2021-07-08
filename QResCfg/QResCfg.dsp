# Microsoft Developer Studio Project File - Name="QResCfg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=QResCfg - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "QResCfg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "QResCfg.mak" CFG="QResCfg - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "QResCfg - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "QResCfg - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/QRES/QResCfg", CAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "QResCfg - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /O1 /I "..\qreslib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 "..\qreslib\Release\qreslib.lib" ole32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBC" /IGNORE:4089
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "QResCfg - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\qreslib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 "..\qreslib\Debug\qreslib.lib" ole32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCD"

!ENDIF 

# Begin Target

# Name "QResCfg - Win32 Release"
# Name "QResCfg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\AdvDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\colormode.cpp
# End Source File
# Begin Source File

SOURCE=.\QResCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\QResCfg.rc
# End Source File
# Begin Source File

SOURCE=.\QResPropertyPage1.cpp
# End Source File
# Begin Source File

SOURCE=.\QResPropertyPage2.cpp
# End Source File
# Begin Source File

SOURCE=.\QResPropertyPage3.cpp
# End Source File
# Begin Source File

SOURCE=.\QResPropertyPage4.cpp
# End Source File
# Begin Source File

SOURCE=.\QResPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\Shortcut.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AdvDlg.h
# End Source File
# Begin Source File

SOURCE=.\colormode.h
# End Source File
# Begin Source File

SOURCE=.\QResCfg.h
# End Source File
# Begin Source File

SOURCE=.\QResPropertyPage.h
# End Source File
# Begin Source File

SOURCE=.\QResPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\Shortcut.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\confirm.BMP
# End Source File
# Begin Source File

SOURCE=.\res\QResCfg.ico
# End Source File
# Begin Source File

SOURCE=.\res\QResCfg.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SCREEN1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SETTINGS.bmp
# End Source File
# End Group
# End Target
# End Project
# Section QResCfg : {1E1CAF61-68DA-11D0-B7F2-444553540000}
# 	0:8:dirpkr.h:dirpkr.h
# 	0:10:dirpkr.cpp:dirpkr.cpp
# 	2:17:CLASS: CMyFileDlg:CMyFileDlg
# 	2:8:dirpkr.h:dirpkr.h
# 	2:19:Application Include:QResCfg.h
# 	2:10:dirpkr.cpp:dirpkr.cpp
# End Section
