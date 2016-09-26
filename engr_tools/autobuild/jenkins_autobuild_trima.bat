@echo off
rem ----------------------------------------------------------------------------
rem - The following variables must be updated for the branch.
rem - Note: the following variables must be provided by the build environment
rem -
rem -    PROJECT_BRANCH  : The branch name (e.g., Trima_6.0.6)
rem -    PROJECT_VERSION : The branch major.minor version (e.g., 8.11)
rem -    PROJECT_LANGDIR : The language directory (8.8)
rem -    NEWVERSION      : The desired revision number (e.g., 8.11.00123)
rem -    WORKSPACE       : The P4 workspace containing the autobuild tools
rem -    P4USER          : The Perforce user name
rem ----------------------------------------------------------------------------
if not defined PROJECT_BRANCH   goto abort
if not defined PROJECT_VERSION  goto abort
if not defined DO_COPY_COMMON   set DO_COPY_COMMON=false
if not defined DEV_TEST         set DEV_TEST=false

set PROJECT_DEPOT=//depot/main/embedded/Trima
set COMMON_DEPOT=//depot/main/embedded/Common

set p4ClientName=Trima_QA_%PROJECT_BRANCH%

:: BCT_HOME gets mapped to K: drive 
set BCT_HOME=\\bctquad3\home
set buildDirTop=K:\BCT_Development\AutomatedBuilds\Trima\Trima-%PROJECT_VERSION%
set rmsgDirNet=K:\BCT_Development\reserved_messages_perforce_builds

set sandboxDir=%WORKSPACE%\sandbox
set residue=%sandboxDir%\xxx_residue
set perforce_tools=.
set batchSub=%perforce_tools%\batch_substitute.bat

rem -------------------- Environment variables used by makefiles
set RMSGDIR=
set COMDIR=%sandboxDir%\Common
set INSTDIR=%sandboxDir%\Trima\Install
set MAINDIR=%sandboxDir%\Trima\%PROJECT_BRANCH%
set LANGDIR=%sandboxDir%\Trima\Trima_Languages\%PROJECT_LANGDIR%
set CPU=I80486
set SMARTIFACTS=true

rem ============================================================================
rem
rem   ******** No modifications below this banner ********
rem
rem ============================================================================

rem ----------------------------------------------------------------------------
rem For testing, force the label to be DevTest
rem ----------------------------------------------------------------------------
if "%DEV_TEST%" == "true" (
   set NEWVERSION=%PROJECT_VERSION%.DevTest
   set p4ClientName=Trima_DEV_%PROJECT_BRANCH%
)

set buildDir=%buildDirTop%\Trima-%NEWVERSION%

:: BUILD_INFO_NETWORKDIR is used by makefiles to generate build_info file.
:: Ideally, it won't use a drive map, but set a default if its undefined.
if not defined BUILD_INFO_NETWORKDIR set BUILD_INFO_NETWORKDIR=%buildDir%

echo.
echo.
echo ===============================================================
echo ---------------------------------------------------------------
echo ----------                                      ---------------
echo ----------       Jenkins Autobuild Script       ---------------
echo ----------                                      ---------------
echo ---------------------------------------------------------------
echo ===============================================================
echo.
echo.

if "%DEV_TEST%" == "true" echo +++ NOTE: DEV_TEST=true forces NEWVERSION=%NEWVERSION%

set exitVal=
set startDate=%date%
set startTime=%time%

rem ----------------------------------------------------------------------------
rem
rem        Login to Perforce is not done here; instead it's assumed that the parent
rem        Jenkins job has logged in already. So here we just check that P4USER
rem        is defined. If not, it *might* indicate this script isn't being run
rem        from a Jenkins job.
rem
rem ----------------------------------------------------------------------------

set user_name=%P4USER%

if not defined P4USER (
   echo Error 100: Perforce user P4USER is not defined!
   set /a exitVal=100
   goto end_of_script
)

:: Show the status of current Perforce user
echo Perforce is logged in as user %user_name%
p4 login -s

rem ----------------------------------------------------------------------------
rem        Clean up directories from previous build
rem ----------------------------------------------------------------------------

echo.
echo +++ Cleaning up directories from previous build. Please wait ...

if exist %sandboxDir% ( 
   rmdir /S /Q %sandboxDir%
)

mkdir %sandboxDir%

echo +++ Done cleaning.
echo.
echo.

if not exist %sandboxDir% (
   echo Error 101: Unable to create sandbox directory "%sandboxDir%".
   set /a exitVal=101
   goto end_of_script
)

mkdir %residue%

echo +++ Building %NEWVERSION%
set build=%NEWVERSION%

rem ----------------------------------------------------------------------------
rem        Check if the build number is empty
rem ----------------------------------------------------------------------------

if "%build%" == "" ( 
   echo Error 102: Invalid build number: "%build%"
   set /a exitVal=102
   goto end_of_script
)

if "%DEV_TEST%" == "true" goto CheckLabelInUse 

rem ----------------------------------------------------------------------------
rem         Check if the build number contains numbers
rem ----------------------------------------------------------------------------

set buildInput=%residue%\buildInput
set containsNo=%residue%\containsNo

echo %build% > %buildInput%
findstr [0-9] %buildInput% > %containsNo%

for %%A in (%containsNo%) do ( 
    if %%~zA==0 (
        echo Error 103: Invalid Build number: "%build%"
	set /a exitVal=103
        goto end_of_script
    ) else (
        echo +++ Valid Build number.
    )
)

rem ----------------------------------------------------------------------------
rem
rem        Check if the label is currently in use
rem
rem ----------------------------------------------------------------------------
:CheckLabelInUse

:: Make sure the K: drive is available
if not exist K:\ net use K: %BCT_HOME%

set current_build_label=Trima_build-%build%
set current_build_info=%buildDirTop%\current_build.info

echo +++ Prepping to generate build label "%current_build_label%"

set filemask=%residue%\p4labels.tmp
p4 labels -e Trima_build-%PROJECT_VERSION%* > %filemask%

findstr /l /c:%current_build_label% %filemask%

if %ERRORLEVEL% == 1 (
   echo +++ Build label %current_build_label% does not exist.
   goto CheckCurrentLabel
)

if "%DEV_TEST%" == "true" (
   echo +++ Build label %current_build_label% already exists and will be updated 
   goto CheckCurrentLabel
) else (
   echo Error 104: Build label %current_build_label% exists in Perforce. Please specify a new one
   set /a exitVal=104
   goto end_of_script
)

:CheckCurrentLabel
set prev_build_label=
if NOT EXIST %current_build_info% goto CreateSandbox

for /F %%a in (%current_build_info%) do set prev_build_label=%%a
echo +++ Prev build is "%prev_build_label%"

:CreateSandbox

rem ----------------------------------------------------------------------------
rem Display the build number.
rem ----------------------------------------------------------------------------

echo +++ Build Number: %build%

rem ----------------------------------------------------------------------------
rem Mark date and time at the start of the build
rem ----------------------------------------------------------------------------

set buildStartDate=%date%
set buildStartTime=%time%

rem ----------------------------------------------------------------------------
rem
rem        Use latest rmsg if it's not been frozen
rem
rem ----------------------------------------------------------------------------

set perforceLog=%sandboxDir%\perforce.log
set changesLog=%sandboxDir%\changes.log

set rmsgBuildLocation=
if not exist %buildDirTop%\latest_rmsg_build_dir.mk goto LatestRmsg

:FrozenRmsg
for /F %%a in (%buildDirTop%\latest_rmsg_build_dir.mk) do set rmsgBuildLocation=%%a
echo +++ Using frozen rmsg build
goto CopyRmsg 

:LatestRmsg
for /F %%a in (%rmsgDirNet%\latest_rmsg_build_dir.mk) do set rmsgBuildLocation=%%a
echo +++ Using latest rmsg build

:CopyRmsg
echo +++ Copying rmsg build from %rmsgDirNet%\%rmsgBuildLocation% ...
for /f "tokens=* delims=.\ " %%a in ("%rmsgBuildLocation%") do set rmsgBuildLocation=%%a
set localRmsgDir=%sandboxDir%\rmsg_%rmsgBuildLocation%
if exist %localRmsgDir% rmdir /S /Q %localRmsgDir%
mkdir %localRmsgDir%
echo +++ Copying rmsg build from %rmsgDirNet%\%rmsgBuildLocation% to %localRmsgDir% ... 2>&1 | tee -a %perforceLog%
xcopy %rmsgDirNet%\%rmsgBuildLocation% %localRmsgDir% /E /V /F >> %perforceLog% 2>&1    
echo %localRmsgDir% > %sandboxDir%\latest_rmsg_build_dir.mk

set RMSGDIR=%localRmsgDir%

rem ----------------------------------------------------------------------------
rem
rem         Create Perforce sandbox with latest files from depot
rem
rem ----------------------------------------------------------------------------
echo +++ Creating Perforce view ...

set myview=%residue%\p4view.tmp
call %batchSub% XXXX %p4ClientName% %perforce_tools%\trima_workspace_template.tmp > %myview%.1
call %batchSub% WWWW %sandboxDir% %myview%.1 > %myview%.2
call %batchSub% UUUU %user_name% %myview%.2 > %myview%

echo +++ Synching client %p4ClientName% using view in %myview% 2>&1 | tee -a %perforceLog%

:: Setup and sync to the client. Here we use 'set P4CLIENT' vs. 'p4 set P4CLIENT' to
:: resolve problem associated with concurrent Jenkins jobs using the p4 command-line API.
:: 
p4 client -i < %myview%
set P4CLIENT=%p4ClientName%
p4 sync -f %PROJECT_DEPOT%/... >> %perforceLog% 2>&1

rem ----------------------------------------------------------------------------
rem
rem         Sync Common to a particular label.
rem
rem ----------------------------------------------------------------------------

set commonLabel=
if exist %MAINDIR%\common_version (
   for /F "usebackq delims==" %%a in (`findstr /I /B common_ %MAINDIR%\common_version`) do set commonLabel=%%a
   goto GetCommon
) else (
   echo Error 105: File %MAINDIR%\common_version not found. Cannot determine common label
   set /a exitVal=105
   goto RemoveClient
)

:GetCommon
echo +++ Synching with Common label %commonLabel% 2>&1 | tee -a %perforceLog%
mkdir %COMDIR%
p4 sync -f %COMMON_DEPOT%/...@%commonLabel% >> %perforceLog% 2>&1    
echo %COMDIR% > %sandboxDir%\latest_common_build_dir.mk

rem ----------------------------------------------------------------------------
rem
rem         Create build label using label template
rem
rem ----------------------------------------------------------------------------
:CreateLabel

echo +++ Creating Perforce label: %current_build_label%

del /F /Q %MAINDIR%\projectrevision
call %batchSub% BB %build% %perforce_tools%\projectrevision.tmp > %MAINDIR%\projectrevision

set labeltmp=%residue%\label.tmp
call %batchSub% LLLL %current_build_label% %perforce_tools%\trima_label_template.tmp > %labeltmp%.1
call %batchSub% UUUU %user_name% %labeltmp%.1 > %labeltmp%.2
call %batchSub% BBB %build% %labeltmp%.2 > %labeltmp%.3
call %batchSub% CCC %commonLabel% %labeltmp%.3 > %labeltmp%.4
call %batchSub% DDD "%date% %time%" %labeltmp%.4 > %labeltmp%

echo +++ Creating Perforce label %current_build_label% from %labeltmp% 2>&1 | tee -a %perforceLog%
p4 label -i < %labeltmp%
p4 label -o %current_build_label% >> %perforceLog% 2>&1
echo +++ Tagging %current_build_label% ... 2>&1 | tee -a %perforceLog%
p4 tag -l %current_build_label% //%p4ClientName%/... >> %perforceLog% 2>&1

rem ----------------------------------------------------------------------------
rem
rem         Log changes since prev build
rem
rem ----------------------------------------------------------------------------

if defined prev_build_label (
   echo +++ Logging changes since %prev_build_label% to changes.log | tee -a %perforceLog%
   echo ++++ Changes between %current_build_label% and %prev_build_label% > %changesLog%
   p4 diff2 -q %PROJECT_DEPOT%/...@%current_build_label% %PROJECT_DEPOT%/...@%prev_build_label% >> %changesLog% 2>&1
)

rem ----------------------------------------------------------------------------
rem
rem    Done with Perforce, so remove the client sooner than later. Mitigates a
rem    problem observed with concurrent Jenkins jobs that setup a client using
rem    the P4 command-line API.
rem
rem    It is crucial to delete the client. If you don't, at the next build files
rem    from prev build will be deleted from build dir. We still have labels but
rem    we lose copy of build.
rem
rem ----------------------------------------------------------------------------
:RemoveClient
echo +++ Removing P4 Client %p4ClientName%  ...
p4 workspace -d %p4ClientName%
if defined exitVal goto end_of_script


if not "%DEV_TEST%" == "true" echo %current_build_label% > %current_build_info%

rem ----------------------------------------------------------------------------
rem
rem        Build the project.
rem
rem ----------------------------------------------------------------------------
:BuildIt

set buildLog=%sandboxDir%\build.log

echo +++ Building Trima project ... | tee -a %buildLog%

pushd %sandboxDir%

rem ----------------------------------------------------------------------------
rem
rem         Start the build 
rem
rem ----------------------------------------------------------------------------
rem attrib -r *.* /s /d

cd %COMDIR%
call create_common.bat %JOBS% 2>&1 | tee -a %buildLog%

cd %MAINDIR%
call create_trima.bat 2>&1 | tee -a %buildLog%

set buildEndDate=%date%
set buildEndTime=%time%
set buildStatus=complete

if %ERRORLEVEL% NEQ 0 set buildStatus=busted
if not exist %MAINDIR%\current_build\trima_ftp set buildStatus=busted

if %buildStatus% == busted (
   echo Error %ERRORLEVEL%: Failure building the project.
   echo +++ Marking build as busted
   set buildDir=%buildDir%_busted
   set /a exitVal=%ERRORLEVEL%
)

rem ----------------------------------------------------------------------------
rem
rem         Copy locally built sandbox to %buildDir%
rem
rem ----------------------------------------------------------------------------
:CopyBuild

rem if exist %residue% rmdir /S /Q %residue%

set copyLog=%sandboxDir%\copy.log
set buildDirCPU=%buildDir%\%CPU%

:: Make sure the K: drive is *still* available
if not exist K:\ net use K: %BCT_HOME%

echo +++ Build %buildStatus%. Copying artifacts to %buildDirCPU% 2>&1 | tee -a %buildLog%

popd

if exist %buildDir% (
   rmdir /S /Q %buildDir%
)
mkdir %buildDirCPU%

if %buildStatus% == busted goto Summary

rem ------ Split the artifacts up (Trima specific):
rem
rem Trima-XX.yyyy
rem - Alarms
rem - %CPU%
rem   - current_build
rem   - engr_tools
rem   - Source
rem   - Source_Install
rem   - Source_Languages
rem   - Source_%commonLabel%    [optional]

set excludeDirs=%sandboxDir%\exclude.txt
echo \current_build\ > %excludeDirs%
echo \engr_tools\ >> %excludeDirs%
echo \%CPU%\ >> %excludeDirs%
echo \%SIMNT%\ >> %excludeDirs%
echo \.dfile\ >> %excludeDirs%

@echo on
@echo +++ Copy current build ... | tee -a %copyLog%
xcopy %MAINDIR%\current_build %buildDirCPU%\current_build /S /I /F /R /Y 2>&1 | tee -a %copyLog%
:: if exist %buildDirTop%\updateTrima_55 copy %buildDirTop%\updateTrima_55 %buildDirCPU%\current_build\trima_ftp 2>&1 | tee -a %copyLog%

@echo +++ Copy engr_tools and service ... | tee -a %copyLog%
xcopy %MAINDIR%_%CPU%\engr_tools\bin\*.out %buildDirCPU%\engr_tools /S /I /F /R /Y 2>&1 | tee -a %copyLog%
xcopy %MAINDIR%_%CPU%\service\bin %buildDirCPU%\Source\service\bin /S /I /F /R /Y 2>&1 | tee -a %copyLog%

@echo +++ Copy Alarms info files ... | tee -a %copyLog%
xcopy %MAINDIR%\engr_tools\genAlarmList\*.xml %buildDir%\Alarms /I /F /R /Y 2>&1 | tee -a %copyLog%
xcopy %MAINDIR%\engr_tools\genAlarmList\*.csv %buildDir%\Alarms /I /F /R /Y 2>&1 | tee -a %copyLog%

@echo +++ Copy Trima source files ... | tee -a %copyLog%
xcopy %MAINDIR% %buildDirCPU%\Source /EXCLUDE:%excludeDirs% /S /I /F /R /Y 2>&1 | tee -a %copyLog%
xcopy %INSTDIR% %buildDirCPU%\Source_Install /EXCLUDE:%excludeDirs% /S /I /F /R /Y 2>&1 | tee -a %copyLog%
xcopy %LANGDIR% %buildDirCPU%\Source_Languages /S /I /F /R /Y 2>&1 | tee -a %copyLog%

if "%DO_COPY_COMMON%" == "false" goto CopyDone 
@echo +++ Copy Common source files ... | tee -a %copyLog%
xcopy %COMDIR%  %buildDirCPU%\Source_%commonLabel% /EXCLUDE:%excludeDirs% /S /I /F /R /Y 2>&1 | tee -a %copyLog%

:CopyDone
@echo off

if %ERRORLEVEL% NEQ 0 (
    echo Error %ERRORLEVEL%: Copying artifacts to Build directory
    set /a exitVal=%ERRORLEVEL%
    rem goto RemoveClient
)

rem ----------------------------------------------------------------------------
rem	Summarize and copy logs
rem ----------------------------------------------------------------------------
:Summary

echo. | tee -a %buildLog%
echo BUILD_SUMMARY_BEG | tee -a %buildLog%
echo +++ Trima Label     : %current_build_label% 2>&1 | tee -a %buildLog%
echo +++ Common Label    : %commonLabel% 2>&1 | tee -a %buildLog%
echo +++ Sandbox Start   : %startDate%, %startTime% 2>&1 | tee -a %buildLog%
echo +++ Build Start     : %buildStartDate%, %buildStartTime% 2>&1 | tee -a %buildLog%
echo +++ Build End       : %buildEndDate%, %buildEndTime% 2>&1 | tee -a %buildLog%
echo +++ Copy Complete   : %date%, %time% 2>&1 | tee -a %buildLog%
echo +++ Build Status    : %buildStatus% 2>&1 | tee -a %buildLog%
echo BUILD_SUMMARY_END | tee -a %buildLog%

@echo on
@echo Copy logs: xcopy %sandboxDir%\*.log %buildDir%\_BuildLogs /I /F /R /Y
xcopy %sandboxDir%\*.log %buildDir%\_BuildLogs /I /F /R /Y
@echo off

rem ----- Make read-only
attrib +R %buildDir%\* /s /d

:end_of_script
popd

echo +++ Build done. Status: %buildStatus%

if not defined exitVal set /a exitVal=%ERRORLEVEL%
exit /B %exitVal%
