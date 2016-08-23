@echo off
rem
rem create_trima.bat - Top-level build script for Trima under vxWorks
rem

set EXITVAL=0
set beg=%date% %time%

if not defined CPU set CPU=I80486
if not exist .\makefile.vxc goto not_trima_sandbox
if not exist .\create_trima.bat goto not_trima_sandbox
if not exist .\focgui goto not_trima_sandbox
if not exist .\gui goto not_trima_sandbox
if not exist .\low_level goto not_trima_sandbox
if not exist .\proc goto not_trima_sandbox
if not exist .\safety goto not_trima_sandbox
if not exist .\softcrc goto not_trima_sandbox
if not exist .\tcpip goto not_trima_sandbox
if not exist .\engr_tools goto not_trima_sandbox
if not exist .\trima_common goto not_trima_sandbox

if not defined WIND_HOST_TYPE goto find_tornado
if not defined WIND_BASE goto find_tornado
goto tornado_ok

:tornado_ok
echo +++ Start of Trima build: %beg%

rem
rem Check for Parasoft static analysis options.
rem If option found, remove it from the mkargs that get passed to make
rem 
set mkargs=%*
if "%1" == "/analysis"   set mkargs=%mkargs:/analysis=%   && goto find_parasoft
if "%1" == "/-analysis"  set mkargs=%mkargs:/-analysis=%  && goto build_without_parasoft
if "%1" == "/noanalysis" set mkargs=%mkargs:/noanalysis=% && goto build_without_parasoft
rem
rem Until we can test this with CCTray, build without Parasoft
rem
goto build_without_parasoft

:find_parasoft
rem 
rem See if the Parasoft static analysis tool is present.  If so, build using it
rem - Start with the directory For Eclipse-based installation
rem
set PARASOFT_BIN="%PROGRAMFILES%\Parasoft\C++test for Eclipse\9.5"
if not exist %PARASOFT_BIN%\cpptestscan.exe (set PARASOFT_BIN="%PROGRAMFILES%\Parasoft\C++test\9.5")
if not exist %PARASOFT_BIN%\cpptestscan.exe (set PARASOFT_BIN="%PROGRAMFILES(x86)%\Parasoft\C++test\9.5")
if not exist %PARASOFT_BIN%\cpptestscan.exe (set PARASOFT_BIN="%ProgramW6432%\Parasoft\C++test\9.5")
if not exist %PARASOFT_BIN%\cpptestscan.exe (goto build_without_parasoft)
rem Remove the quotation marks and prepend to PATH
set PARASOFT_BIN=%PARASOFT_BIN:"=%
set ORIG_PATH=%PATH%
set PATH=%PARASOFT_BIN%;%PATH%
set CPPTESTSCAN=%PARASOFT_BIN%\cpptestscan.exe

rem 
rem Build with Parasoft static analysis tool
rem
:build_with_parasoft
echo +++ Building with Parasoft: %CPPTESTSCAN%
make -f makefile.vx %mkargs% DO_CPPTEST=TRUE
rem Restore the original PATH
set PATH=%ORIG_PATH%
goto check_error_level

rem 
rem Build normally (no static analysis)
rem
:build_without_parasoft
echo +++ Building without Parasoft
make -f makefile.vx %mkargs%
goto check_error_level

:check_error_level
set EXITVAL=%ERRORLEVEL%
if %ERRORLEVEL% NEQ 0 echo +++ make1: ERRORLEVEL=%ERRORLEVEL% && goto end_of_script

rem
rem Build the trima current_build archive
rem
make -f makefile.vx archive %mkargs%
set EXITVAL=%ERRORLEVEL%
if %ERRORLEVEL% NEQ 0 echo +++ make2: ERRORLEVEL=%ERRORLEVEL% && goto end_of_script

rem
rem Mark the latest build, but note that a failure here will not cause an exit failure
rem
make -f makefile.vx %mkargs% latest_trima_build_dir.mk
goto end_of_script

:find_tornado
if exist c:\tornado2.2\host\x86-win32\bin\torVars.bat goto tornado_on_c
if exist d:\tornado2.2\host\x86-win32\bin\torVars.bat goto tornado_on_d
goto no_tornado

:tornado_on_c
call c:\tornado2.2\host\x86-win32\bin\torVars.bat
goto tornado_ok

:tornado_on_d
call d:\tornado2.2\host\x86-win32\bin\torVars.bat
goto tornado_ok

:not_trima_sandbox
echo create_trima must be run from the base trima sandbox directory
set EXITVAL=100
goto end_of_script

:no_tornado
echo Unable to locate the Tornado installation directory.
echo The file torVars.bat from the Tornado host tools must be run to
echo properly set the build environment
set EXITVAL=101
goto end_of_script

:end_of_script
if %EXITVAL% NEQ 0 echo +++ EXITVAL=%EXITVAL%
set end=%date% %time%
echo +++ Beg: %beg%
echo +++ End: %end%

exit /B %EXITVAL%

