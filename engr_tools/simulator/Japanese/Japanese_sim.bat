@echo off
REM ----------------------------------------------------------
REM  This is the batch file to run the full routine simulation
REM  of trima.
REM ----------------------------------------------------------

IF NOT EXIST D:\NewBuild\Config GOTO BUILD
echo New Build exists. Applying new build...

xcopy d:\newbuild\config\strings\*.info C:\sim\trima\6_3\config\strings\ /Y /R

REM move the old build
move /Y d:\simulator\trima6.3\config d:\oldbuild\config
move /Y d:\simulator\trima6.3\machine d:\oldbuild\machine
move /Y d:\simulator\trima6.3\source d:\oldbuild\source
move /Y d:\simulator\trima6.3\trima d:\oldbuild\trima
move /Y d:\simulator\trima6.3\vxboot d:\oldbuild\vxboot

REM move the new build
move /Y d:\newbuild\config d:\simulator\trima6.3\config
move /Y d:\newbuild\machine d:\simulator\trima6.3\machine
move /Y d:\newbuild\source d:\simulator\trima6.3\source
move /Y d:\newbuild\trima d:\simulator\trima6.3\trima
move /Y d:\newbuild\vxboot d:\simulator\trima6.3\vxboot

:BUILD

del C:\sim\trima\6_3\machine\snap\*.* /Q /F

echo copy strings info
REM copy the strings
xcopy d:\strings\*.info C:\sim\trima\6_3\config\strings\ /Y /R

echo set configuration
REM copy the config
xcopy d:\simulator\trima6.3\japanese\config.dat C:\sim\trima\6_3\config\config.dat /Y /R

echo run simulator
d:\simulator\trima6.3\SimulatorStarter 90.0.0.1 23 d:\simulator\trima6.3\startSimNoTelnet.bat d:\simulator\trima6.3\simguitelnet 110 15