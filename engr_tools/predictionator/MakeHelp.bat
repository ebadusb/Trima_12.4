@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by AP2 PREDICTIONATER.HPJ. >"hlp\AP2 predictionater.hm"
echo. >>"hlp\AP2 predictionater.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\AP2 predictionater.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\AP2 predictionater.hm"
echo. >>"hlp\AP2 predictionater.hm"
echo // Prompts (IDP_*) >>"hlp\AP2 predictionater.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\AP2 predictionater.hm"
echo. >>"hlp\AP2 predictionater.hm"
echo // Resources (IDR_*) >>"hlp\AP2 predictionater.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\AP2 predictionater.hm"
echo. >>"hlp\AP2 predictionater.hm"
echo // Dialogs (IDD_*) >>"hlp\AP2 predictionater.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\AP2 predictionater.hm"
echo. >>"hlp\AP2 predictionater.hm"
echo // Frame Controls (IDW_*) >>"hlp\AP2 predictionater.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\AP2 predictionater.hm"
REM -- Make help for Project AP2 PREDICTIONATER


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\AP2 predictionater.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\AP2 predictionater.hlp" goto :Error
if not exist "hlp\AP2 predictionater.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\AP2 predictionater.hlp" Debug\predictionater.hlp
if exist Debug\nul copy "hlp\AP2 predictionater.cnt" Debug
if exist Release\nul copy "hlp\AP2 predictionater.hlp" Release
if exist Release\nul copy "hlp\AP2 predictionater.cnt" Release
echo.
goto :done

:Error
echo hlp\AP2 predictionater.hpj(1) : error: Problem encountered creating help file

:done
echo.
