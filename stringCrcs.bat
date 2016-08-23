@echo off
rem
rem stringCrcs.bat - Generate CRCs for all string files
rem
rem $Header: E:/BCT_Development/Trima5.R/Trima/rcs/stringCrcs.bat 1.1 2009/06/22 21:18:04Z dslausb Exp dslausb $
rem
rem $Log: stringCrcs.bat $
rem Revision 1.1  2009/06/22 21:18:04Z  dslausb
rem Initial revision
rem

if not defined WIND_HOST_TYPE goto find_tornado
if not defined WIND_BASE goto find_tornado
goto tornado_ok

:tornado_ok
echo Get CRCs for string.info files
date/t
time/t
rem 
rem Build the trima software
rem
cd engr_tools\guiTools\stringCrcGen
make -f genAllStringCrcs.vx %*
cd ..\..\..
goto end_of_script

:find_tornado
if exist c:\dan_batch\torvars2.2.bat goto danTornado
if exist c:\tornado2.2\host\x86-win32\bin\torVars.bat goto tornado_on_c
if exist d:\tornado2.2\host\x86-win32\bin\torVars.bat goto tornado_on_d
goto no_tornado

:danTornado
call C:\dan_batch\torvars2.2.bat
goto tornado_ok

:tornado_on_c
call c:\tornado2.2\host\x86-win32\bin\torVars.bat
goto tornado_ok

:tornado_on_d
call d:\tornado2.2\host\x86-win32\bin\torVars.bat
goto tornado_ok

:not_trima_sandbox
echo create_trima must be run from the base trima sandbox directory
goto end_of_script

:no_tornado
echo Unable to locate the Tornado installation directory.
echo The file torVars.bat from the Tornado host tools must be run to
echo properly set the build environment
goto end_of_script

:end_of_script

