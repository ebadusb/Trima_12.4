@echo off
REM ----------------------------------------------------------
REM  This is the batch file to run the full routine simulation
REM  of trima.
REM ----------------------------------------------------------

call PreProcess.bat


SimulatorStarter 90.0.0.1 23 startSimNoTelnet.bat auto 170 15
