@rem
@rem Root of Visual Developer Studio Common files.
@set VSCommonDir=C:\ARCHIV~1\MICROS~2\Common

@rem
@rem Root of Visual Developer Studio installed files.
@rem
@set MSDevDir=C:\ARCHIV~1\MICROS~2\Common\msdev98

@rem
@rem Root of Visual C++ installed files.
@rem
@set MSVCDir=C:\ARCHIV~1\MICROS~2\VC98

@rem
@rem VcOsDir is used to help create either a Windows 95 or Windows NT specific path.
@rem
@set VcOsDir=WIN95
@if "%OS%" == "Windows_NT" set VcOsDir=WINNT

@rem
@rem

@if "%OS%" == "Windows_NT" set PATH=%MSDevDir%\BIN;%MSVCDir%\BIN;%VSCommonDir%\TOOLS\%VcOsDir%;%VSCommonDir%\TOOLS;%PATH%
@if "%OS%" == "" set PATH="%MSDevDir%\BIN";"%MSVCDir%\BIN";"%VSCommonDir%\TOOLS\%VcOsDir%";"%VSCommonDir%\TOOLS";"%windir%\SYSTEM";"%PATH%"
@set INCLUDE=%MSVCDir%\ATL\INCLUDE;%MSVCDir%\INCLUDE;%MSVCDir%\MFC\INCLUDE;C:\DX90SDK\Include;C:\DOCUMENTS AND SETTINGS\ISABEL\MIS DOCUMENTOS\VICEN\WINDOWSICF2\INCLUDE;C:\IPV6KIT\INC;C:\DX90SDK\SAMPLES\C++\DIRECTSHOW\BASECLASSES;C:\DOCUMENTS AND SETTINGS\ISABEL\MIS DOCUMENTOS\VICEN\RTP\INCLUDE;C:\DX90SDK\SAMPLES\C++\DIRECTSHOW\COMMON;%INCLUDE%
@set LIB=%MSVCDir%\LIB;%MSVCDir%\MFC\LIB;%LIB%

@set VcOsDir=
@set VSCommonDir=


@bldmake bldfiles && makmake aCodecs.mmp vc6