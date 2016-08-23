@echo off
REM ----------------------------------------------------------
REM  This is the batch file to run the full routine simulation
REM  of trima.
REM ----------------------------------------------------------

copy c:\sim\trima\main\config\cal.dat c:\sim\trima\main\config\cal.dat.bk 
copy c:\sim\trima\main\config\cassette.dat c:\sim\trima\main\config\cassette.dat.bk 
copy c:\sim\trima\main\config\cobe_config.dat c:\sim\trima\main\config\cobe_config.dat.bk 
copy c:\sim\trima\main\config\config.dat c:\sim\trima\main\config\config.dat.bk 
copy c:\sim\trima\main\config\hw.dat c:\sim\trima\main\config\hw.dat.bk 
copy c:\sim\trima\main\config\meter.dat c:\sim\trima\main\config\meter.dat.bk 
copy c:\sim\trima\main\config\rbc.dat c:\sim\trima\main\config\rbc.dat.bk 
copy c:\sim\trima\main\config\setconfig.dat c:\sim\trima\main\config\setconfig.dat.bk 
copy c:\sim\trima\main\config\sounds.dat c:\sim\trima\main\config\sounds.dat.bk 
copy c:\sim\trima\main\config\sounds_j.dat c:\sim\trima\main\config\sounds_j.dat.bk 
copy c:\sim\trima\main\config\touch_screen.dat c:\sim\trima\main\config\touch_screen.dat.bk 
copy c:\sim\trima\main\config\trap_default.dat c:\sim\trima\main\config\trap_default.dat.bk 
copy c:\sim\trima\main\config\trap_override.dat c:\sim\trima\main\config\trap_override.dat.bk 

copy c:\sim\trima\main\config\crc\cassette.crc c:\sim\trima\main\config\crc\cassette.crc.bk 
copy c:\sim\trima\main\config\crc\setconfig.crc c:\sim\trima\main\config\crc\setconfig.crc.bk 

mkdir c:\sim\trima\main\machine\tmp
