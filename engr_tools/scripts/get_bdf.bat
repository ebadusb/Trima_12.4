@echo off
REM DOS wrapper for the Bash shell script.
REM
REM Usage: get_bdf [-h] [dest_trima_dir] [trima_build_num]
REM

set MYPWD=%CD%
set PROG=%MYPWD%\engr_tools\scripts\get_bdf

if not defined WIND_BASE goto find_tornado
if not defined WIND_HOST_TYPE goto find_tornado

:find_common

cd ..\..
set MAIN=%CD%

if defined COMDIR goto verify_comdir

if exist Common (
   echo Setting COMDIR to %MAIN%\Common
   set COMDIR=%MAIN%\Common
   goto verify_comdir
)

if exist latest_common_build_dir.mk (
   echo Getting COMDIR from latest_common_build_dir.mk
   for /F %%I in ('type latest_common_build_dir.mk') do set COMDIR=%%~fI
   goto verify_comdir   
)

:verify_comdir

if not exist %COMDIR% (
   echo Directory does not exist: COMDIR=%COMDIR%
   echo Please set COMDIR to the location of your Commmon directory
   exit /b 1
)

cd %COMDIR%
set COMDIR=%CD%
echo Using COMDIR=%COMDIR%

REM -------------------- If necessary, create tmp dir to silence sh

if not exist %WIND_BASE%\host\%WIND_HOST_TYPE% goto no_tornado
set TMPDIR=%WIND_BASE%\host\%WIND_HOST_TYPE%\tmp
if not exist %TMPDIR% mkdir %TMPDIR%
set PATH=%WIND_BASE%\host\%WIND_HOST_TYPE%\bin;%PATH%
set CYGWIN=nodosfilewarning

cd %MYPWD%

REM -------------------- Convert to Unix style path and pass it to Bash shell
set XPROG=%PROG:\=/%
echo Running: %XPROG% %*
sh -c '%XPROG% %*'

:the_end
exit /b 0

:find_tornado
set TORVARS=\Tornado2.2\host\x86-win32\bin\torVars.bat
if exist C:%TORVARS% set TV=C:%TORVARS%
if exist D:%TORVARS% set TV=D:%TORVARS%
if not exist %TV% goto no_tornado
call %TV%
goto find_common

:no_tornado
echo Unable to locate the Tornado installation directory.
echo The file torVars.bat from Tornado host tools must be
echo run to properly set the build environment.
exit /b 1

