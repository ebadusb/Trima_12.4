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

#define VERSION_FUNCTION "52"
#define VERSION_FILENAME "5.2"

/* Files: Files that will be modified. ***************************************/

static const char *FILE_PREDICT_SOURCE = "TrimaPredict.h";
static const char *FILE_SOURCE_TEMPLATE = "ManagedPredict.template.h";
static const char *FILE_SOURCE_PRODUCT = "ManagedPredict.h";

/* Sections: The sections to parse. ******************************************/

static const char *PREDICT_FUNCTIONS_BEGIN = "PREDICT_FUNCTIONS_BEGIN";
static const char *PREDICT_FUNCTIONS_END = "PREDICT_FUNCTIONS_END";
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

static const char *FUNCTION_PREDICT_GET_PREFIX = "Predict_Get_";
static const char *FUNCTION_PROCEDURE_GET_PREFIX = "Procedure_Get_";
static const char *FUNCTION_CONFIG_GET_PREFIX = "Config_Get_";
static const char *FUNCTION_CONFIG_SET_PREFIX = "Config_Set_";
static const char *FUNCTION_SUBSTATE_GET_PREFIX = "Substate_Get_";
static const char *FUNCTION_CONDITION_GET_PREFIX = "Condition_Get_";
static const char *FUNCTION_CONDITION_SET_PREFIX = "Condition_Set_";

/* Functions: The alias for the functions. ***********************************/

static const char *FUNCTION_PROCEDURE_GET_ALIAS = "Predict_Get_";
static const char *FUNCTION_CONFIG_GET_ALIAS = "Config_Get_";
static const char *FUNCTION_CONFIG_SET_ALIAS = "Config_Set_";
static const char *FUNCTION_SUBSTATE_GET_ALIAS = "Substate_Get_";
static const char *FUNCTION_CONDITION_GET_ALIAS = "Condition_Get_";
static const char *FUNCTION_CONDITION_SET_ALIAS = "Condition_Set_";

/* Implementation: The implementaion of the functions to generate. ***********/

static const char* FUNCTION_PROCEDURE_GET_TEMPLATE = 
	"\tstatic <type> <function>(int product) \n"
	"\t{ \n"
	"\t	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); \n"
	"\t	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"\t	return accessor.<name>.Get(optimizer, product); \n"
	"\t} \n"; 

static const char* FUNCTION_CONFIG_GET_TEMPLATE = 
	"\tstatic <type> <function>(void) \n"
	"\t{ \n"
	"\t	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); \n"
	"\t	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"\t	return accessor.<name>.Get(optimizer); \n"
	"\t} \n"; 

static const char* FUNCTION_CONFIG_SET_TEMPLATE = 
	"\tstatic <type> <function>(<type> value) \n"
	"\t{ \n"
	"\t	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); \n"
	"\t	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"\t	return accessor.<name>.Set(optimizer, value); \n"
	"\t} \n"; 

static const char* FUNCTION_CONFIGARRAY_GET_TEMPLATE = 
	"\tstatic <type> <function>(int index) \n"
	"\t{ \n"
	"\t	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); \n"
	"\t	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"\t	return accessor.<name>.Get(optimizer, index); \n"
	"\t} \n"; 

static const char* FUNCTION_CONFIGARRAY_SET_TEMPLATE = 
	"\tstatic <type> <function>(int index, <type> value) \n"
	"\t{ \n"
	"\t	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); \n"
	"\t	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"\t	return accessor.<name>.Set(optimizer, index, value); \n"
	"\t} \n"; 

static const char* FUNCTION_SUBSTATE_GET_TEMPLATE = 
	"\tstatic <type> <function>(int product, int index) \n"
	"\t{ \n"
	"\t	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); \n"
	"\t	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"\t	return accessor.<name>.Get(optimizer, product, index); \n"
	"\t} \n"; 

static const char* FUNCTION_CONDITION_GET_TEMPLATE = 
	"\tstatic <type> <function>(void) \n"
	"\t{ \n"
	"\t	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); \n"
	"\t	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"\t	return accessor.<name>.Get(conditions); \n"
	"\t} \n"; 

static const char* FUNCTION_CONDITION_SET_TEMPLATE = 
	"\tstatic <type> <function>(<type> value) \n"
	"\t{ \n"
	"\t	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); \n"
	"\t	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); \n"
	"\t	return accessor.<name>.Set(conditions, value); \n"
	"\t} \n"; 

#endif // EXCELPREDICTBUILDPROCESS_H