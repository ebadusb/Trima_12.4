echo off
set BUILD_DIR=..\..\..
set CYGWIN=nodosfilewarning

IF EXIST BooleanFeatureInit.cpp attrib -R BooleanFeatureInit.cpp
IF EXIST ValueFeatureInit.cpp attrib -R ValueFeatureInit.cpp

IF NOT DEFINED BUILD_MACHINE SET BUILD_MACHINE=NO

IF %BUILD_MACHINE% EQU YES GOTO MACHINE

GOTO GENERATE

:MACHINE
echo setting machine variables
set WIND_HOST_TYPE=x86-win32
set WIND_BASE=D:\tornado2.2
set PATH=%WIND_BASE%\host\%WIND_HOST_TYPE%\bin;%PATH%

:GENERATE
echo generating files
call sed -n -f %BUILD_DIR%\cds\build_bool_feature_enum.sed %BUILD_DIR%\cds\bool_features.txt > BooleanFeatures.h

call sed -n -f %BUILD_DIR%\cds\build_bool_feature_init.sed  %BUILD_DIR%\cds\bool_features.txt > BooleanFeatureInit.cpp

call sed -n -f %BUILD_DIR%\cds\build_value_feature_enum.sed %BUILD_DIR%\cds\value_features.txt > ValueFeatures.h

call sed -n -f %BUILD_DIR%\cds\build_value_feature_init.sed %BUILD_DIR%\cds\value_features.txt > ValueFeatureInit.cpp
