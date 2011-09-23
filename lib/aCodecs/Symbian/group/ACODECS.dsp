# Microsoft Developer Studio Project File - Name="ACODECS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ACODECS - Win32 Uni Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ACODECS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ACODECS.mak" CFG="ACODECS - Win32 Uni Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ACODECS - Win32 Uni Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ACODECS - Win32 Uni Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ACODECS - Win32 Uni Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Win32_Un"
# PROP BASE Intermediate_Dir ".\Win32_Un"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UREL"
# PROP Intermediate_Dir "\Symbian\8.0a\S60_2nd_FP2\EPOC32\BUILD\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\GROUP\ACODECS\WINS\UREL"
# ADD CPP /nologo /Zp4 /MD /W4 /O1 /Op /X /I "\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\INC" /I "\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE" /I "\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\LIBC" /D "__SYMBIAN32__" /D "__VC32__" /D "__WINS__" /D "__DLL__" /D "NDEBUG" /D "_UNICODE" /GF /c
# ADD MTL /nologo /mktyplib203 /D /win32
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UREL\EDLL.LIB" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\euser.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\estlib.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\openfix.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\BUILD\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\GROUP\ACODECS\WINS\UREL\ACODECS.exp" /nologo /entry:"_E32Dll" /subsystem:windows /dll /machine:IX86 /nodefaultlib /include:"?_E32Dll@@YGHPAXI0@Z" /WARN:3
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=echo Doing first-stage link by name 	nmake -nologo -f "\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\GROUP\ACODECS.SUP.MAKE" PRELINKUREL 	if errorlevel 1 nmake -nologo -f "\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\GROUP\ACODECS.SUP.MAKE" STOPLINKUREL
PostBuild_Cmds=nmake -nologo -f "\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\GROUP\ACODECS.SUP.MAKE" POSTBUILDUREL
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ACODECS - Win32 Uni Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Win32_U0"
# PROP BASE Intermediate_Dir ".\Win32_U0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB"
# PROP Intermediate_Dir "\Symbian\8.0a\S60_2nd_FP2\EPOC32\BUILD\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\GROUP\ACODECS\WINS\UDEB"
# PROP Ignore_Export_Lib 0
# ADD CPP /nologo /Zp4 /MDd /W4 /Zi /Od /X /I "\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\INC" /I "\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE" /I "\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\LIBC" /D "__SYMBIAN32__" /D "__VC32__" /D "__WINS__" /D "__DLL__" /D "_DEBUG" /D "_UNICODE" /FR /Fd"\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\ACODECS.PDB" /GF /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\EDLL.LIB" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\euser.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\estlib.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB\openfix.lib" "\Symbian\8.0a\S60_2nd_FP2\EPOC32\BUILD\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\GROUP\ACODECS\WINS\UDEB\ACODECS.exp" /nologo /entry:"_E32Dll" /subsystem:windows /dll /debug /machine:IX86 /nodefaultlib /include:"?_E32Dll@@YGHPAXI0@Z" /WARN:3
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=echo Doing first-stage link by name 	nmake -nologo -f "\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\GROUP\ACODECS.SUP.MAKE" PRELINKUDEB 	if errorlevel 1 nmake -nologo -f "\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\GROUP\ACODECS.SUP.MAKE" STOPLINKUDEB
PostBuild_Cmds=nmake -nologo -f "\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\GROUP\ACODECS.SUP.MAKE" POSTBUILDUDEB
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ACODECS - Win32 Uni Release"
# Name "ACODECS - Win32 Uni Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\GROUP\Acodecs.uid.cpp
DEP_CPP_ACODE=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	
# End Source File
# Begin Source File

SOURCE=\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\SRC\Acodecsdllmain.cpp
DEP_CPP_ACODEC=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\INC\Acodecs.pan"\
	
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=\REPOSITORIO\ISABEL\LIB\ACODECS\SYMBIAN\INC\Acodecs.pan
# End Source File
# End Group
# Begin Group "aCodecs"

# PROP Default_Filter ""
# Begin Group "codecs"

# PROP Default_Filter ""
# Begin Group "gsm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\add.cxx
DEP_CPP_ADD_C=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\code.cxx
DEP_CPP_CODE_=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\cx_gsm.h
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\decode.cxx
DEP_CPP_DECOD=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\gsm.cxx
DEP_CPP_GSM_C=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\gsm.h
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\gsm_create.cxx
DEP_CPP_GSM_CR=\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\gsm_decode.cxx
DEP_CPP_GSM_D=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\gsm_destroy.cxx
DEP_CPP_GSM_DE=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\gsm_encode.cxx
DEP_CPP_GSM_E=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\gsm_option.cxx
DEP_CPP_GSM_O=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\long_term.cxx
DEP_CPP_LONG_=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\lpc.cxx
DEP_CPP_LPC_C=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\preprocess.cxx
DEP_CPP_PREPR=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\private.h
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\rpe.cxx
DEP_CPP_RPE_C=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\short_term.cxx
DEP_CPP_SHORT=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs\gsm\table.cxx
DEP_CPP_TABLE=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=..\..\lib\codec.cxx
DEP_CPP_CODEC=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\include\codec.h
# End Source File
# Begin Source File

SOURCE=..\..\lib\codecs.cxx
DEP_CPP_CODECS=\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32base.inl"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32def.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des16.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32des8.h"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\E32STD.H"\
	"..\..\..\..\..\..\Symbian\8.0a\S60_2nd_FP2\EPOC32\INCLUDE\e32std.inl"\
	"..\..\include\codecs.h"\
	"..\..\lib\codecs\g711\g711.h"\
	"..\..\lib\codecs\g711\g711Imp.h"\
	"..\..\lib\codecs\g721\g721.h"\
	"..\..\lib\codecs\g722\g722.h"\
	"..\..\lib\codecs\g726\g726.h"\
	"..\..\lib\codecs\g726\g726_xx.h"\
	"..\..\lib\codecs\g726\g72x.h"\
	"..\..\lib\codecs\g728\g728.h"\
	"..\..\lib\codecs\gsm\cx_gsm.h"\
	"..\..\lib\codecs\gsm\gsm.h"\
	"..\..\lib\codecs\gsm\private.h"\
	"..\..\lib\codecs\none\none.h"\
	{$(INCLUDE)}"codec.h"\
	
# End Source File
# Begin Source File

SOURCE=..\..\include\codecs.h
# End Source File
# End Group
# End Target
# End Project
