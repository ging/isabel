-----------------------------------------------------------------------------

    readme.txt - aCodecs dll
    
-----------------------------------------------------------------------------

This project implements aCodecs Dynamic Link Library (acodecs.dll).
Installation package file (acodecs.sis) is also provided to install
the DLL in emulator (in \System\Libs directory).

aCodecs.dll implements CaCodecs object, API is defined in aCodecs.h file.

SIS-file installs also aCodecsTest test application in emulator.
aCodecsTest can be used to test the DLL functions. 


Compile project using following commands in DOS-prompt:

    bldmake bldfiles
    abld build wins(cw) udeb
    abld build thumb urel


Freeze DLL API, regenerate makefiles and recompile:

    abld freeze wins(cw)
    abld freeze thumb
    bldmake bldfiles
    abld build wins(cw) udeb
    abld build thumb urel

When the DLL API is frozen, exported functions are saved in DEF-files:
\aCodecs\BWINS\ACODECSU.DEF for WINS, and \aCodecs\MARM\ACODECSU.DEF
for ARMI and THUMB platforms. When the DLL is ready and released, save
the DEF-files with other DLL source files. DEF-files freeze the API,
so that future changes in the DLL implementation does not change the
API and hence break applications using aCodecs DLL.


Compile test application (see also ..\TSrc\readme.txt):

    abld test build wins(cw) udeb
    abld test build thumb urel


Create installation package (SIS-file):

    makesis acodecs.pkg



