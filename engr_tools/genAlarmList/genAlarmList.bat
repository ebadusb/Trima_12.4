@echo off

REM This batch file creates an XML and CSV file containing the alarm information for
REM Trima, including the language strings available in the %LANGDIR% folder
REM defined in the environment (as of 6.3 and later).
REM
REM It assumes that the gawk program (perhaps from cygwin) is available on the
REM path, and that the awk programs utilized are located in the 
REM engr_tools/genAlarmList directory, along with this script.

REM Make a list of the language files as input to the xml generator. Since '??'
REM seems to be treated as '*' in some shells, special handling is needed via
REM awk to restrict the results to stringXX.cpp.

dir /b %LANGDIR%\string*.info > strfiles.tmp

REM Run the xml/csv generator program.
REM
REM Example of how to explicitly set the CSV/XML output files:
REM
REM gawk -v xmlFile=alarmList.xml -v csvFile=alarmList.csv -f alarmReport.awk -f common.awk strfiles.tmp
REM

gawk -f alarmReport.awk -f common.awk strfiles.tmp > alarmList.xml
