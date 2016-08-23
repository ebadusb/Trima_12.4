/*****************************************************************************\
**
** ExcelPredictBuildProcess.h
**
** Provides the implementation for the automatic code generation.
**
\*****************************************************************************/

#ifndef EXCELPREDICTBUILDPROCESS_H
#define EXCELPREDICTBUILDPROCESS_H

/* Version: The version of the tool. *****************************************/

#define VERSION_FUNCTION "64"
#define VERSION_FILENAME "6.4"

/* Files: Files that will be modified. ***************************************/

static const char *FILE_PREDICT_SOURDE = "ExcelPredict.h";
static const char *FILE_DEFINITION_TEMPLATE = "ExcelAddin.template.def";
static const char *FILE_DEFINITION_GENERATED = "ExcelAddin.def";
static const char *FILE_SOURCE_TEMPLATE = "ExcelPredict.template.cpp";
static const char *FILE_SOURCE_PRODUCT = "ExcelPredict.cpp";
static const char *FILE_DESCRIPTION_TEMPLATE = "ExcelAddin.template.h";
static const char *FILE_DESCRIPTION_PRODUCT = "ExcelAddin.h";
static const char *FILE_MODULE_TEMPLATE = "ExcelPredict.template.bas";
static const char *FILE_MODULE_PRODUCT = "ExcelPredict."VERSION_FILENAME".bas";

/* Sections: The sections to parse. ******************************************/

static const char *PROCEDURE_FUNCTIONS_BEGIN = "PROCEDURE_FUNCTIONS_BEGIN";
static const char *PROCEDURE_FUNCTIONS_END = "PROCEDURE_FUNCTIONS_END";
static const char *CONFIG_FUNCTIONS_BEGIN = "CONFIG_FUNCTIONS_BEGIN";
static const char *CONFIG_FUNCTIONS_END = "CONFIG_FUNCTIONS_END";
static const char *CONFIGARRAY_FUNCTIONS_BEGIN = "CONFIGARRAY_FUNCTIONS_BEGIN";
static const char *CONFIGARRAY_FUNCTIONS_END = "CONFIGARRAY_FUNCTIONS_END";
static const char *SUBSTATE_FUNCTIONS_BEGIN = "SUBSTATE_FUNCTIONS_BEGIN";
static const char *SUBSTATE_FUNCTIONS_END = "SUBSTATE_FUNCTIONS_END";
static const char *CONDITION_FUNCTIONS_BEGIN = "CONDITION_FUNCTIONS_BEGIN";
static const char *CONDITION_FUNCTIONS_END = "CONDITION_FUNCTIONS_END";

/* Functions: The prefix for the functions. **********************************/

static const char *FUNCTION_PROCEDURE_GET_PREFIX = "Predict_" VERSION_FUNCTION "_Get_";
static const char *FUNCTION_CONFIG_GET_PREFIX = "Config_" VERSION_FUNCTION "_Get_";
static const char *FUNCTION_CONFIG_SET_PREFIX = "Config_" VERSION_FUNCTION "_Set_";
static const char *FUNCTION_SUBSTATE_GET_PREFIX = "Substate_" VERSION_FUNCTION "_Get_";
static const char *FUNCTION_CONDITION_GET_PREFIX = "Condition_" VERSION_FUNCTION "_Get_";
static const char *FUNCTION_CONDITION_SET_PREFIX = "Condition_" VERSION_FUNCTION "_Set_";

/* Functions: The alias for the functions. ***********************************/

static const char *FUNCTION_PROCEDURE_GET_ALIAS = "Predict_Get_";
static const char *FUNCTION_CONFIG_GET_ALIAS = "Config_Get_";
static const char *FUNCTION_CONFIG_SET_ALIAS = "Config_Set_";
static const char *FUNCTION_SUBSTATE_GET_ALIAS = "Substate_Get_";
static const char *FUNCTION_CONDITION_GET_ALIAS = "Condition_Get_";
static const char *FUNCTION_CONDITION_SET_ALIAS = "Condition_Set_";

/* Implementation: The implementaion of the functions to generate. ***********/

static const char* FUNCTION_PROCEDURE_GET_TEMPLATE = 
	"DLLEXPORT <type> WINAPI <function>(int product) \n"
	"{ \n"
	"	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); \n"
	"	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"	return accessor.<name>.Get(optimizer, product); \n"
	"} \n"; 

static const char* FUNCTION_CONFIG_GET_TEMPLATE = 
	"DLLEXPORT <type> WINAPI <function>(void) \n"
	"{ \n"
	"	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); \n"
	"	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"	return accessor.<name>.Get(optimizer); \n"
	"} \n"; 

static const char* FUNCTION_CONFIG_SET_TEMPLATE = 
	"DLLEXPORT <type> WINAPI <function>(<type> value) \n"
	"{ \n"
	"	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); \n"
	"	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"	return accessor.<name>.Set(optimizer, value); \n"
	"} \n"; 

static const char* FUNCTION_CONFIGARRAY_GET_TEMPLATE = 
	"DLLEXPORT <type> WINAPI <function>(int index) \n"
	"{ \n"
	"	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); \n"
	"	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"	return accessor.<name>.Get(optimizer, index); \n"
	"} \n"; 

static const char* FUNCTION_CONFIGARRAY_SET_TEMPLATE = 
	"DLLEXPORT <type> WINAPI <function>(int index, <type> value) \n"
	"{ \n"
	"	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); \n"
	"	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"	return accessor.<name>.Set(optimizer, index, value); \n"
	"} \n"; 

static const char* FUNCTION_SUBSTATE_GET_TEMPLATE = 
	"DLLEXPORT <type> WINAPI <function>(int product, int index) \n"
	"{ \n"
	"	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); \n"
	"	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"	return accessor.<name>.Get(optimizer, product, index); \n"
	"} \n"; 

static const char* FUNCTION_CONDITION_GET_TEMPLATE = 
	"DLLEXPORT <type> WINAPI <function>(void) \n"
	"{ \n"
	"	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); \n"
	"	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"	return accessor.<name>.Get(conditions); \n"
	"} \n"; 

static const char* FUNCTION_CONDITION_SET_TEMPLATE = 
	"DLLEXPORT <type> WINAPI <function>(<type> value) \n"
	"{ \n"
	"	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); \n"
	"	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"	return accessor.<name>.Set(conditions, value); \n"
	"} \n"; 

/* Excel Registration: The registration format of the functions. *************/

static const char* FUNCTION_PROCEDURE_GET_DECLARE[10] = {
	"<function>",
	"<type>J",
	"<function>",
	"Product",
	"2",
	"Predict " VERSION_FILENAME,
	"",
	"",
	"",
	""
};

static const char* FUNCTION_CONFIG_GET_DECLARE[10] = {
	"<function>",
	"<type>",
	"<function>",
	"",
	"2",
	"Predict " VERSION_FILENAME,
	"",
	"",
	"",
	""
};

static const char* FUNCTION_CONFIG_SET_DECLARE[10] = {
	"<function>",
	"J<type>",
	"<function>",
	"Value",
	"2",
	"Predict " VERSION_FILENAME,
	"",
	"",
	"",
	""
};

static const char* FUNCTION_CONFIGARRAY_GET_DECLARE[10] = {
	"<function>",
	"<type>J",
	"<function>",
	"Index",
	"2",
	"Predict " VERSION_FILENAME,
	"",
	"",
	"",
	""
};

static const char* FUNCTION_CONFIGARRAY_SET_DECLARE[10] = {
	"<function>",
	"JJ<type>",
	"<function>",
	"Index,Value",
	"2",
	"Predict " VERSION_FILENAME,
	"",
	"",
	"",
	""
};

static const char* FUNCTION_SUBSTATE_GET_DECLARE[10] = {
	"<function>",
	"<type>JJ",
	"<function>",
	"Product, Index",
	"2",
	"Predict " VERSION_FILENAME,
	"",
	"",
	"",
	""
};

static const char* FUNCTION_CONDITION_GET_DECLARE[10] = {
	"<function>",
	"<type>",
	"<function>",
	"",
	"2",
	"Predict " VERSION_FILENAME,
	"",
	"",
	"",
	""
};

static const char* FUNCTION_CONDITION_SET_DECLARE[10] = {
	"<function>",
	"J<type>",
	"<function>",
	"Value",
	"2",
	"Predict " VERSION_FILENAME,
	"",
	"",
	"",
	""
};

/* VBA Registration: The VBD registration format of the functions. ***********/

static const char* FUNCTION_PROCEDURE_GET_MODULE = 
	"Public Declare Function <function> Lib \"ExcelPredict." VERSION_FILENAME ".xll\" (ByVal product As Long) As <type>";
static const char* FUNCTION_CONFIG_GET_MODULE = 
	"Public Declare Function <function> Lib \"ExcelPredict." VERSION_FILENAME ".xll\" () As <type>";
static const char* FUNCTION_CONFIG_SET_MODULE = 
	"Public Declare Function <function> Lib \"ExcelPredict." VERSION_FILENAME ".xll\" (ByVal value As <type>) As <type>";
static const char* FUNCTION_CONFIGARRAY_GET_MODULE = 
	"Public Declare Function <function> Lib \"ExcelPredict." VERSION_FILENAME ".xll\" (ByVal index As Long) As <type>";
static const char* FUNCTION_CONFIGARRAY_SET_MODULE = 
	"Public Declare Function <function> Lib \"ExcelPredict." VERSION_FILENAME ".xll\" (ByVal index As Long, ByVal value As <type>) As <type>";
static const char* FUNCTION_SUBSTATE_GET_MODULE = 
	"Public Declare Function <function> Lib \"ExcelPredict." VERSION_FILENAME ".xll\" (ByVal product As Long, ByVal index As Long) As <type>";
static const char* FUNCTION_CONDITION_GET_MODULE = 
	"Public Declare Function <function> Lib \"ExcelPredict." VERSION_FILENAME ".xll\" () As <type>";
static const char* FUNCTION_CONDITION_SET_MODULE = 
	"Public Declare Function <function> Lib \"ExcelPredict." VERSION_FILENAME ".xll\" (ByVal value As <type>) As <type>";

#endif // EXCELPREDICTBUILDPROCESS_H