Public Declare Function Optimizer_64_LoadOptimizerFromLog Lib "ExcelPredict.6.4.xll" (ByVal datalog As String, ByVal cobeconfig As String) As Long
Public Declare Function Optimizer_64_LoadOptimizerFromConfig Lib "ExcelPredict.6.4.xll" (ByVal userconfig As String, ByVal cobeconfig As String) As Long
Public Declare Function Optimizer_64_LoadOptimizerFromConfigs Lib "ExcelPredict.6.4.xll" (ByVal userconfig As String, ByVal cobeconfig As String, ByVal softwareconfig As String) As Long
Public Declare Function Optimizer_64_UpdateConditions Lib "ExcelPredict.6.4.xll" () As Long
Public Declare Function Optimizer_64_UpdateConfigurations Lib "ExcelPredict.6.4.xll" () As Long
Public Declare Function Optimizer_64_OptimizeProduct Lib "ExcelPredict.6.4.xll" (ByVal product As Long) As Long
Public Declare Function SoftwareOptions_64_SetOption Lib "ExcelPredict.6.4.xll" (ByVal opt As Long, ByVal enabled As Long) As Long
Public Declare Function SoftwareOptions_64_GetOption Lib "ExcelPredict.6.4.xll" () As Long

