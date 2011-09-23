# Microsoft Developer Studio Project File - Name="ACODECSTEST" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ACODECSTEST - Win32 Uni Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ACODECSTEST.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ACODECSTEST.mak" CFG="ACODECSTEST - Win32 Uni Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ACODECSTEST - Win32 Uni Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ACODECSTEST - Win32 Uni Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ACODECSTEST - Win32 Uni Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Win32_U0"
# PROP BASE Intermediate_Dir ".\Win32_U0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB"
# PROP Intermediate_Dir "\Symbian\8.0a\S60_2nd_FP2\EPOC32\BUILD\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST\WINS\UDEB"
# ADD CPP /nologo /Zp4 /GF /MDd /W4 /Zi /Od /X /I "\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC" /I "\REPOSI~1\ISABEL\LIB\ACODECS\INCLUDE" /I "\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\INC" /I "\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE" /D "__SYMBIAN32__" /D "__VC32__" /D "__WINS__" /D "__DLL__" /D "_DEBUG" /D "_UNICODE" /FR /Fd"\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\Z\SYSTEM\APPS\ACODECSTEST\ACODECSTEST.PDB" /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\EDLL.LIB" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\euser.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\apparc.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\eikcore.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\cone.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\acodecs.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\openfix.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\BUILD\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST\WINS\UDEB\ACODECSTEST.exp" /nologo /entry:"_E32Dll" /subsystem:windows /dll /pdb:"\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\Z\SYSTEM\APPS\ACODECSTEST\ACODECSTEST.pdb" /debug /machine:IX86 /nodefaultlib /include:"?_E32Dll@@YGHPAXI0@Z" /out:"\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\Z\SYSTEM\APPS\ACODECSTEST\ACODECSTEST.APP" /WARN:3
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE=$(InputPath)
PreLink_Cmds=echo Doing first-stage link by name\
	nmake -nologo -f "\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST.SUP.MAKE" PRELINKUDEB\
	if errorlevel 1 nmake -nologo -f "\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST.SUP.MAKE" STOPLINKUDEB 
PostBuild_Cmds=nmake -nologo -f "\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST.SUP.MAKE" POSTBUILDUDEB
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ACODECSTEST - Win32 Uni Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Win32_Un"
# PROP BASE Intermediate_Dir ".\Win32_Un"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UREL"
# PROP Intermediate_Dir "\Symbian\8.0a\S60_2nd_FP2\EPOC32\BUILD\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST\WINS\UREL"
# ADD CPP /nologo /Zp4 /GF /MD /W4 /O1 /Op /X /I "\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC" /I "\REPOSI~1\ISABEL\LIB\ACODECS\INCLUDE" /I "\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\INC" /I "\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE" /D "__SYMBIAN32__" /D "__VC32__" /D "__WINS__" /D "__DLL__" /D "NDEBUG" /D "_UNICODE" /c
# ADD MTL /nologo /mktyplib203 /D /win32
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UREL\EDLL.LIB" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\euser.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\apparc.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\eikcore.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\cone.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\acodecs.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\openfix.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\BUILD\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST\WINS\UREL\ACODECSTEST.exp" /nologo /entry:"_E32Dll" /subsystem:windows /dll /machine:IX86 /nodefaultlib /include:"?_E32Dll@@YGHPAXI0@Z" /out:"\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UREL\Z\SYSTEM\APPS\ACODECSTEST\ACODECSTEST.APP" /WARN:3
# Begin Special Build Tool
SOURCE=$(InputPath)
PreLink_Cmds=echo Doing first-stage link by name\
	nmake -nologo -f "\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST.SUP.MAKE" PRELINKUREL\
	if errorlevel 1 nmake -nologo -f "\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST.SUP.MAKE" STOPLINKUREL 
PostBuild_Cmds=nmake -nologo -f "\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST.SUP.MAKE" POSTBUILDUREL
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ACODECSTEST - Win32 Uni Debug"
# Name "ACODECSTEST - Win32 Uni Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\Acodecstest.rss
USERDEP__ACODECSTEST=

!IF  "$(CFG)" == "ACODECSTEST - Win32 Uni Debug"

# PROP Intermediate_Dir "\Symbian\8.0a\S60_2nd_FP2\EPOC32\BUILD\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST\WINS"
# Begin Custom Build - Building resources from Acodecstest.rss
InputPath=\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\Acodecstest.rss

BuildCmds= \
	nmake -nologo -f "\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST.SUP.MAKE"\
  "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\Z\SYSTEM\APPS\ACODECSTEST\ACODECSTEST.r"

"\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\Z\SYSTEM\APPS\ACODECSTEST\ACODECSTEST.rSC.dummy" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ACODECSTEST - Win32 Uni Release"

# PROP Intermediate_Dir "\Symbian\8.0a\S60_2nd_FP2\EPOC32\BUILD\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST\WINS"
# Begin Custom Build - Building resources from Acodecstest.rss
InputPath=\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\Acodecstest.rss

BuildCmds= \
	nmake -nologo -f "\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\ACODECSTEST.SUP.MAKE"\
  "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UREL\Z\SYSTEM\APPS\ACODECSTEST\ACODECSTEST.r"

"\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UREL\Z\SYSTEM\APPS\ACODECSTEST\ACODECSTEST.rSC.dummy" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\Acodecstest.cpp
# End Source File
# Begin Source File

SOURCE=\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\Acodecstest.uid.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=\REPOSI~1\ISABEL\LIB\ACODECS\SYMBIAN\TSRC\Acodecstest.h
# End Source File
# End Group
# End Target
# End Project
