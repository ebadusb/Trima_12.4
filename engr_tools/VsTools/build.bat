@echo off
cd ..\..\..
cd %1
chdir
@echo %*
rem chdir
rem SET MAINDIR
rem SET RMSGDIR
SET CPU
make -f makefile.vx %2=%3 %4=%5
