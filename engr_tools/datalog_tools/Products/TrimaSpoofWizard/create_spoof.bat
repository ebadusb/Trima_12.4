@echo off
rem Command line build environments
set WIND_HOST_TYPE=x86-win32
set WIND_BASE=D:\Tornado2.2
set PATH=%WIND_BASE%\host\%WIND_HOST_TYPE%\bin;%PATH%

make -f D:\Source\Trima\engr_tools\spoofer\custom\makefile.vx MAINDIR=D:\Source\Trima COMDIR=Z:\common_builds\common_build_6.260 SPOOF_NAME=%1 SPOOF_SRC=%2 SPOOF_DST=%2