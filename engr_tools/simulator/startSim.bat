@echo off
rem
rem startSim.bat - Start Trima Simulator
rem
rem $Header: E:/BCT_Development/Trima5.R/Trima/gui/guitest/rcs/startSim.bat 1.4 2008/12/16 19:20:36Z dslausb Exp $
rem

call PreProcess.bat

start c:\sim\trima\main\source\vxworks_sim\vxworks.exe /r268435456
sleep 5
echo Attempting to connect telnet shell
cmd /k telnet 90.0.0.1
goto end_of_script

