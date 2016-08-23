echo off
echo changing file attributes
ATTRIB -R /S /D

rem --------------------------------------
rem  run the predict with out.xml as ouput
rem --------------------------------------
echo running unit test
call PredictUnitTest.exe out.xml

rem -------------------------------------
rem  transform the xml to html
rem -------------------------------------
echo transforming output
call msxsl out.xml ResultTransform.xslt -o unitTestResult.html

rem -------------------------------------
rem  show the html file
rem -------------------------------------
echo showing test result
call unitTestResult.html