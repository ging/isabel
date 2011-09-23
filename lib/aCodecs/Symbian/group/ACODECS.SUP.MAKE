
PATH=C:\Symbian\8.0a\S60_2nd_FP2\epoc32\tools\;C:\Symbian\8.0a\S60_2nd_FP2\epoc32\gcc\bin\;C:\Perl\bin\;C:\Archivos de programa\Archivos comunes\Symbian\Tools;C:\Archivos de programa\Abria Merlin\Perl\bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\Archivos de programa\Mbone;;C:\Archivos de programa\Microsoft Visual Studio .NET 2003\Common7\IDE;C:\Archivos de programa\Microsoft Visual Studio .NET 2003\VC7\BIN;C:\Archivos de programa\Microsoft Visual Studio .NET 2003\Common7\Tools;C:\Archivos de programa\Microsoft Visual Studio .NET 2003\Common7\Tools\bin\prerelease;C:\Archivos de programa\Microsoft Visual Studio .NET 2003\Common7\Tools\bin;C:\Archivos de programa\Microsoft Visual Studio .NET 2003\SDK\v1.1\bin;C:\WINDOWS\Microsoft.NET\Framework\v1.1.4322;C:\Perl\bin\;C:\Archivos de programa\Archivos comunes\Symbian\Tools;C:\Archivos de programa\Abria Merlin\Perl\bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;;C:\Archivos de programa\Java\j2re1.4.1_02\bin;C:\Archivos de programa\Microsoft Visual Studio .NET 2003\Vc7\bin;

# EPOC DEFINITIONS

EPOCBLD = \Symbian\8.0a\S60_2nd_FP2\EPOC32\BUILD\ISABEL\LIB\ACODECS\SYMBIAN\GROUP\ACODECS\WINS\ #
EPOCTRG = \Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\ #
EPOCLIB = \Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\ #
EPOCLINK = \Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\ #
EPOCSTATLINK = \Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\ #

EPOCBLDUDEB = $(EPOCBLD)UDEB
EPOCTRGUDEB = $(EPOCTRG)UDEB
EPOCLIBUDEB = $(EPOCLIB)UDEB
EPOCLINKUDEB = $(EPOCLINK)UDEB
EPOCSTATLINKUDEB = $(EPOCSTATLINK)UDEB

EPOCBLDUREL = $(EPOCBLD)UREL
EPOCTRGUREL = $(EPOCTRG)UREL
EPOCLIBUREL = $(EPOCLIB)UDEB
EPOCLINKUREL = $(EPOCLINK)UDEB
EPOCSTATLINKUREL = $(EPOCSTATLINK)UREL


TRGDIR = .\

DATADIR = Z\SYSTEM\DATA

LIBRARY :
	@echo Not attempting to create "$(EPOCLIB)UDEB\ACODECS.LIB"
	@echo from frozen .DEF file, since EXPORTUNFROZEN specified.


# REAL TARGET - IMPORT LIBRARY

"$(EPOCLIB)UDEB\ACODECS.LIB" : "\ISABEL\LIB\ACODECS\SYMBIAN\BWINS\ACODECSU.DEF" MAKEWORKLIBRARY
	lib.exe /nologo /machine:i386 /nodefaultlib /name:"ACODECS.DLL" /def:"\ISABEL\LIB\ACODECS\SYMBIAN\BWINS\ACODECSU.DEF" /out:"$(EPOCLIB)UDEB\ACODECS.LIB"
	del "$(EPOCLIB)UDEB\ACODECS.exp"


MAKEWORKLIBRARY : "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB"

"\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB" :
	@perl -S emkdir.pl "\Symbian\8.0a\S60_2nd_FP2\EPOC32\RELEASE\WINS\UDEB"


# BUILD - UDEB

LIBS= \
	"$(EPOCLINKUDEB)\EUSER.LIB" \
	"$(EPOCLINKUDEB)\ESTLIB.LIB" \
	"$(EPOCLINKUDEB)\openfix.lib"

LINK_OBJS= \
	"$(EPOCBLDUDEB)\ACODECS.UID.obj" \
	"$(EPOCBLDUDEB)\ACODECSDLLMAIN.obj" \
	"$(EPOCBLDUDEB)\ADD.obj" \
	"$(EPOCBLDUDEB)\CODE.obj" \
	"$(EPOCBLDUDEB)\CODECS.obj" \
	"$(EPOCBLDUDEB)\DECODE.obj" \
	"$(EPOCBLDUDEB)\GSM.obj" \
	"$(EPOCBLDUDEB)\GSM_CREATE.obj" \
	"$(EPOCBLDUDEB)\GSM_DECODE.obj" \
	"$(EPOCBLDUDEB)\GSM_DESTROY.obj" \
	"$(EPOCBLDUDEB)\GSM_ENCODE.obj" \
	"$(EPOCBLDUDEB)\GSM_OPTION.obj" \
	"$(EPOCBLDUDEB)\LONG_TERM.obj" \
	"$(EPOCBLDUDEB)\LPC.obj" \
	"$(EPOCBLDUDEB)\PREPROCESS.obj" \
	"$(EPOCBLDUDEB)\RPE.obj" \
	"$(EPOCBLDUDEB)\SHORT_TERM.obj" \
	"$(EPOCBLDUDEB)\TABLE.obj"

STAGE1_LINK_FLAGS="$(EPOCSTATLINKUDEB)\EDLL.LIB" \
 $(LIBS) /nologo /entry:"_E32Dll" /subsystem:windows /dll /debug \
 /incremental:no /machine:IX86 /nodefaultlib /include:"?_E32Dll@@YGHPAXI0@Z" /out:"$(EPOCBLDUDEB)\ACODECS.DLL" /WARN:3

PRELINKUDEB : $(LINK_OBJS) "$(EPOCSTATLINKUDEB)\EDLL.LIB" $(LIBS)
	link.exe @<<
		$(STAGE1_LINK_FLAGS) $(LINK_OBJS)
<<
	del "$(EPOCBLDUDEB)\ACODECS.DLL"
	del "$(EPOCBLDUDEB)\ACODECS.exp"
	dumpbin /exports /out:"$(EPOCBLDUDEB)\ACODECS.inf" "$(EPOCBLDUDEB)\ACODECS.lib"
	del "$(EPOCBLDUDEB)\ACODECS.lib"
	perl -S makedef.pl -Inffile "$(EPOCBLDUDEB)\ACODECS.inf" "$(EPOCBLD)ACODECS.def" 
	del "$(EPOCBLDUDEB)\ACODECS.inf"
	lib.exe  /nologo /machine:i386 /nodefaultlib /name:"ACODECS.DLL" /def:"$(EPOCBLD)ACODECS.def" /out:"$(EPOCBLDUDEB)\ACODECS.lib"
	copy "$(EPOCBLDUDEB)\ACODECS.lib" "$(EPOCLIB)UDEB\ACODECS.LIB"
	del "$(EPOCBLDUDEB)\ACODECS.lib"
	@echo First-stage link successful


STOPLINKUDEB : DELEXPOBJUDEB
	@echo Stopped the build by removing the export object,
	@echo if present, because the pre-link stage failed


POSTBUILDUDEB : DELEXPOBJUDEB


DELEXPOBJUDEB :
	if exist "$(EPOCBLDUDEB)\ACODECS.exp" del "$(EPOCBLDUDEB)\ACODECS.exp"



# BUILD - UREL

LIBS= \
	"$(EPOCLINKUREL)\EUSER.LIB" \
	"$(EPOCLINKUREL)\ESTLIB.LIB" \
	"$(EPOCLINKUREL)\openfix.lib"

LINK_OBJS= \
	"$(EPOCBLDUREL)\ACODECS.UID.obj" \
	"$(EPOCBLDUREL)\ACODECSDLLMAIN.obj" \
	"$(EPOCBLDUREL)\ADD.obj" \
	"$(EPOCBLDUREL)\CODE.obj" \
	"$(EPOCBLDUREL)\CODECS.obj" \
	"$(EPOCBLDUREL)\DECODE.obj" \
	"$(EPOCBLDUREL)\GSM.obj" \
	"$(EPOCBLDUREL)\GSM_CREATE.obj" \
	"$(EPOCBLDUREL)\GSM_DECODE.obj" \
	"$(EPOCBLDUREL)\GSM_DESTROY.obj" \
	"$(EPOCBLDUREL)\GSM_ENCODE.obj" \
	"$(EPOCBLDUREL)\GSM_OPTION.obj" \
	"$(EPOCBLDUREL)\LONG_TERM.obj" \
	"$(EPOCBLDUREL)\LPC.obj" \
	"$(EPOCBLDUREL)\PREPROCESS.obj" \
	"$(EPOCBLDUREL)\RPE.obj" \
	"$(EPOCBLDUREL)\SHORT_TERM.obj" \
	"$(EPOCBLDUREL)\TABLE.obj"

STAGE1_LINK_FLAGS="$(EPOCSTATLINKUREL)\EDLL.LIB" \
 $(LIBS) /nologo /entry:"_E32Dll" /subsystem:windows /dll \
 /incremental:no /machine:IX86 /nodefaultlib /include:"?_E32Dll@@YGHPAXI0@Z" /out:"$(EPOCBLDUREL)\ACODECS.DLL" /WARN:3

PRELINKUREL : $(LINK_OBJS) "$(EPOCSTATLINKUREL)\EDLL.LIB" $(LIBS)
	link.exe @<<
		$(STAGE1_LINK_FLAGS) $(LINK_OBJS)
<<
	del "$(EPOCBLDUREL)\ACODECS.DLL"
	del "$(EPOCBLDUREL)\ACODECS.exp"
	dumpbin /exports /out:"$(EPOCBLDUREL)\ACODECS.inf" "$(EPOCBLDUREL)\ACODECS.lib"
	del "$(EPOCBLDUREL)\ACODECS.lib"
	perl -S makedef.pl -Inffile "$(EPOCBLDUREL)\ACODECS.inf" "$(EPOCBLD)ACODECS.def" 
	del "$(EPOCBLDUREL)\ACODECS.inf"
	lib.exe  /nologo /machine:i386 /nodefaultlib /name:"ACODECS.DLL" /def:"$(EPOCBLD)ACODECS.def" /out:"$(EPOCBLDUREL)\ACODECS.lib"
	copy "$(EPOCBLDUREL)\ACODECS.lib" "$(EPOCLIB)UDEB\ACODECS.LIB"
	del "$(EPOCBLDUREL)\ACODECS.lib"
	@echo First-stage link successful


STOPLINKUREL : DELEXPOBJUREL
	@echo Stopped the build by removing the export object,
	@echo if present, because the pre-link stage failed


POSTBUILDUREL : DELEXPOBJUREL


DELEXPOBJUREL :
	if exist "$(EPOCBLDUREL)\ACODECS.exp" del "$(EPOCBLDUREL)\ACODECS.exp"




RECREATEWORKSPACE :
	cd \ISABEL\LIB\ACODECS\SYMBIAN\GROUP
	perl -S makmake.pl -D \ISABEL\LIB\ACODECS\SYMBIAN\GROUP\ACODECS.MMP VC7

