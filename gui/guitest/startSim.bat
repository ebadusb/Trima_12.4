@echo off
rem
rem startSim.bat - Start Trima Simulator
rem
rem $Header: E:/BCT_Development/Trima5.R/Trima/gui/guitest/rcs/startSim.bat 1.4 2008/12/16 19:20:36Z dslausb Exp $
rem

if not defined SIMDIR goto find_trima
goto trima_ok

:trima_ok
echo Trima Directory: %SIMDIR%
echo Attempting to start the simulator.
date/t
time/t
rem 
rem Start the simulator
rem
start %SIMDIR%\source\vxworks_sim\vxworks.exe /r268435456
echo Simulator started.
sleep 5
echo Attempting to connect telnet shell
cmd /k telnet 90.0.0.1
goto end_of_script

:find_trima
echo Finding Trima
if exist __SIMDIR__\gui\guitest\simguiTelnet goto trima_original_build_loc
if exist c:\trima\sim\source\gui\guitest\simguiTelnet goto trima_on_c_loc_1
if exist c:\trima\source\gui\guitest\simguiTelnet goto trima_on_c_loc_2
if exist d:\trima\sim\source\gui\guitest\simguiTelnet goto trima_on_d_loc_1
if exist d:\trima\source\gui\guitest\simguiTelnet goto trima_on_d_loc_2
goto no_trima

:trima_original_build_loc
echo Checking original build location
set SIMDIR=__SIMDIR__
goto trima_ok

:trima_on_c_loc_1
echo Checking location 1
set SIMDIR=c:\trima\sim
goto trima_ok

:trima_on_c_loc_2
echo Checking location 2
set SIMDIR=c:\trima
goto trima_ok

:trima_on_d_loc_1
echo Checking location 3
set SIMDIR=d:\trima\sim
goto trima_ok

:trima_on_d_loc_2
echo Checking location 4
set SIMDIR=d:\trima
goto trima_ok

:no_trima
echo Unable to locate the Trima installation directory.
goto end_of_script

:end_of_script
echo Script complete
