@echo off
rem
rem
rem ISABEL: A group collaboration tool for the Internet
rem Copyright (C) 2009 Agora System S.A.
rem 
rem This file is part of Isabel.
rem 
rem Isabel is free software: you can redistribute it and/or modify
rem it under the terms of the Affero GNU General Public License as published by
rem the Free Software Foundation, either version 3 of the License, or
rem (at your option) any later version.
rem 
rem Isabel is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem Affero GNU General Public License for more details.
rem 
rem You should have received a copy of the Affero GNU General Public License
rem along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
rem

rem
rem Root of Visual Developer Studio Common files.
set VSCommonDir=C:\ARCHIV~1\MICROS~2\Common

rem
rem Root of Visual Developer Studio installed files.
rem
set MSDevDir=C:\ARCHIV~1\MICROS~2\Common\msdev98

rem
rem Root of Visual C++ installed files.
rem
set MSVCDir=C:\ARCHIV~1\MICROS~2\VC98

rem
rem VcOsDir is used to help create either a Windows 95 or Windows NT specific path.
rem
set VcOsDir=WIN95
if "%OS%" == "Windows_NT" set VcOsDir=WINNT

rem
echo Setting environment for using Microsoft Visual C++ tools.
rem

if "%OS%" == "Windows_NT" set PATH=%MSDevDir%\BIN;%MSVCDir%\BIN;%VSCommonDir%\TOOLS\%VcOsDir%;%VSCommonDir%\TOOLS;%PATH%
if "%OS%" == "" set PATH="%MSDevDir%\BIN";"%MSVCDir%\BIN";"%VSCommonDir%\TOOLS\%VcOsDir%";"%VSCommonDir%\TOOLS";"%windir%\SYSTEM";"%PATH%"
set INCLUDE=%MSVCDir%\ATL\INCLUDE;%MSVCDir%\INCLUDE;%MSVCDir%\MFC\INCLUDE;..\include;%INCLUDE%
set LIB=%MSVCDir%\LIB;%MSVCDir%\MFC\LIB;%LIB%

set VcOsDir=
set VSCommonDir=

if "%1" == "clean" goto CLEAN
if "%1" == "distrib" goto DISTRIB

 
:COMPILE
echo ============================
echo == Making ICF2.DLL...     ==
echo ============================
nmake icf2_dll.mak
goto END

:DISTRIB
echo ============================
echo == Copying ICF2.DLL...    ==
echo ============================
goto END

:CLEAN
echo =======================================
echo == Cleaning ICF2 object files...     ==
echo =======================================
del /F /Q release
rmdir release

:END
