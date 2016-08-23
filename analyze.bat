@echo off
REM
REM Runs Parasoft Static Analysis on my local sandbox.
REM Generates HTML report: cpptest_out/report.html
REM
REM USAGE: analyze [/v] [/o | /f infile | chglist ]
REM
REM where:
REM
REM   /v        : verbose output; details written to rep_stdout.txt
REM   /o        : runs on all 'opened' (checked out) files in current directory
REM   /f infile : infile contains the list of files to analyze, one per line
REM   chglist   : Perforce number for pending or submitted change list, OR
REM               a value of 'default' uses the default pending change list.
REM
REM The default behavior is: default
REM
REM Requires:
REM - P4CLIENT is set to the correct Perforce client (a.k.a. workspace)
REM - PATH is set to find Parasoft's cpptestcli.exe
REM - Script is run from the top of source tree (e.g., .../Trima/Main)
REM - Your Eclipse workspace is rooted at: ..\..

set CPPTESTCMD=cpptestcli.exe
set CPPTEST_WORKSPACE=..\..
set VERBOSE=false

set REPORT_DIR=cpptest_out
set REPORT_HTML=%REPORT_DIR%\report.html
set STDOUT=%REPORT_DIR%\rep_stdout.txt
set TEST_FILES=%REPORT_DIR%\files_tested.lst

set MYSETTINGS=.psoft_cli_settings
set CONFIG="team://Terumo BCT Embedded - Phase 1"
set CONFIG="team://Trima-Phase1a"
set MYBDF=ctscan.bdf

REM This gets set to 0 if no violations were found
set /a EXITVAL=2

if not defined P4CLIENT (
   echo Environment variable not set: P4CLIENT
   goto theEnd
)

if not exist %MYSETTINGS% (
   echo Parasoft settings not found: %MYSETTINGS%
   goto theEnd
)

REM Sanity check on the BDF file:
for /F %%i in ("%MYBDF%") do set size=%%~zi
if /I "%size%" LEQ "100000" (
   echo Your BDF file, %MYBDF%, is missing/dubious! You need to EITHER:
   echo - Clean your sandbox and run: create_trima /analysis
   echo - OR run from DOS:  .\engr_tools\scripts\get_bdf.bat
   echo   OR run from Bash: ./engr_tools/scripts/get_bdf
   goto theEnd
)

REM Purge any existing report output; otherwise, make sure REPORT_DIR exists
if exist %REPORT_DIR% (del %REPORT_DIR%\rep*) else (mkdir %REPORT_DIR%)

REM Get the basename of current directory
for /F %%i in ("%CD%") do @set BASE=%%~ni

REM Parse command line options:
REM Default uses currently opened (checked out) files
if "%1" == "/v" set VERBOSE=true && shift
if "%1" == "" set CHGLIST=default && goto do_chglist_default
if "%1" == "default" set CHGLIST=default && goto do_chglist_default
if "%1" == "/o" set CHGLIST=opened && goto do_opened
if "%1" == "/f" set INFILE=%2 && goto do_infile
set CHGLIST=%1 && goto do_chglist_num

:do_chglist_default
echo Analyzing files from changelist: default
p4 opened | grep "default change" | egrep "\.[ch]pp#|\.[ch]#" | sed 's?.*/%BASE%?**?;s/#.*//' > %TEST_FILES%
goto okay

:do_opened
echo Analyzing checked out files per: p4 opened
p4 opened | egrep "\.[ch]pp#|\.[ch]#" | sed 's?.*/%BASE%?**?;s/#.*//' > %TEST_FILES%
goto okay

:do_infile
echo Analyzing files listed in: %INFILE%
if not exist %INFILE% echo /f INFILE does not exist: %INFILE% && goto theEnd
REM Convert slashes to / and strip any depot revision specifiers
egrep "\.[ch]pp|\.[ch]" %INFILE% | sed 's?[/\\\]?/?g';s?.*/%BASE%?**?;s?#.*??' > %TEST_FILES%
goto okay

:do_chglist_num
echo Analyzing files from changelist: %CHGLIST%
p4 describe -s %CHGLIST% | egrep "\.[ch]pp#|\.[ch]#" | sed 's?.*/%BASE%?**?;s/#.*//' > %TEST_FILES%
goto okay

:okay

for /F %%i in ("%TEST_FILES%") do set size=%%~zi
if %size% == 0 echo No C/C++ files found to analyze!? && goto theEnd

echo ================================================== Files to test:
echo %CD%
cat %TEST_FILES%

echo ================================================== Running Parasoft ...
if %VERBOSE% == true (
   @echo on
   %CPPTESTCMD% -J-Xmx768m -J-Dcom.parasoft.xtest.same.vm.reports.generation=on -data %CPPTEST_WORKSPACE% -import .project -localsettings %MYSETTINGS% -config %CONFIG% -include %TEST_FILES% -report %REPORT_DIR% | tee %STDOUT%
   @echo off
) else (
   %CPPTESTCMD% -J-Xmx768m -J-Dcom.parasoft.xtest.same.vm.reports.generation=on -data %CPPTEST_WORKSPACE% -import .project -localsettings %MYSETTINGS% -config %CONFIG% -include %TEST_FILES% -report %REPORT_DIR% > %STDOUT%
)

echo ================================================== Summary:
awk '/Total Files/,/Number of Rules Violated/{ if ($1 !~ /^>+/) print}' %STDOUT%

for /F %%a in ('type %STDOUT%^|find /c "Violations Found: 0"') do @set /a NOERRS=%%a
if %NOERRS% == 1 (set EXITVAL=0) else (set EXITVAL=1)

echo ================================================== Done:
echo For details, see report: %CD%\%REPORT_HTML%
echo Opening report in default browser ...
start %REPORT_HTML%
echo Done.

:theEnd

exit /b %EXITVAL%
