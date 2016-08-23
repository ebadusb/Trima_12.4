set BUILD_DIR=..\..\..
set CYGWIN=nodosfilewarning

IF EXIST BooleanFeatureInit.cpp attrib -R BooleanFeatureInit.cpp
IF EXIST ValueFeatureInit.cpp attrib -R ValueFeatureInit.cpp

call sed -n -f %BUILD_DIR%\cds\build_bool_feature_enum.sed %BUILD_DIR%\cds\bool_features.txt > BooleanFeature.h

call sed -n -f %BUILD_DIR%\cds\build_bool_feature_init.sed  %BUILD_DIR%\cds\bool_features.txt > BooleanFeatureInit.cpp

call sed -n -f %BUILD_DIR%\cds\build_value_feature_enum.sed %BUILD_DIR%\cds\value_features.txt > ValueFeatures.h

call sed -n -f %BUILD_DIR%\cds\build_value_feature_init.sed %BUILD_DIR%\cds\value_features.txt > ValueFeatureInit.cpp