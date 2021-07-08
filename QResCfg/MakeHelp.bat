@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by qres.HPJ. >"hlp\qres.hm"
echo. >>"hlp\qres.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\qres.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\qres.hm"
echo. >>"hlp\qres.hm"
echo // Prompts (IDP_*) >>"hlp\qres.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\qres.hm"
echo. >>"hlp\qres.hm"
echo // Resources (IDR_*) >>"hlp\qres.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\qres.hm"
echo. >>"hlp\qres.hm"
echo // Dialogs (IDD_*) >>"hlp\qres.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\qres.hm"
echo. >>"hlp\qres.hm"
echo // Frame Controls (IDW_*) >>"hlp\qres.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\qres.hm"
REM -- Make help for Project qres


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\qres.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\qres.hlp" goto :Error
if not exist "hlp\qres.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\qres.hlp" Debug
if exist Debug\nul copy "hlp\qres.cnt" Debug
if exist Release\nul copy "hlp\qres.hlp" Release
if exist Release\nul copy "hlp\qres.cnt" Release
echo.
goto :done

:Error
echo hlp\qres.hpj(1) : error: Problem encountered creating help file

:done
echo.
