Attribute VB_Name = "configs"
Option Explicit

Public Function GenerateConfig()
Dim mypath As String
Dim Config As String
Dim cobeconfig As String
Dim ConfigSht As String
Dim CobeConfigSht As String
Dim SWVer As Single
Dim swdat As String
Dim swSht As String

'Skip Config generation
If Worksheets("ConfigLoop").Range("P6").value = "Yes" Then GoTo EOConfig

' Set path and filenames
mypath = ActiveWorkbook.Path & "\"
Config = "config.dat"
cobeconfig = "cobe_config.dat"
swdat = "sw.dat"
SWVer = Worksheets("Configs & Procedures").Range("H11").value
swSht = "sw"

'Choose the correct versions, Add new versions here
If SWVer = 5.1 Then
    ConfigSht = "config51"
    CobeConfigSht = "cc51"
ElseIf SWVer = 5.2 Then
    ConfigSht = "config52"
    CobeConfigSht = "cc52"
ElseIf SWVer = 6# Then
    ConfigSht = "config60"
    CobeConfigSht = "cc60"
ElseIf SWVer = 6.1 Then
    ConfigSht = "config61"
    CobeConfigSht = "cc61"
    swSht = "sw61"
'ElseIf SWVer = 1 Then
'    ConfigSht = "configAMAPP"
'    CobeConfigSht = "cc51"
ElseIf SWVer = 2 Then
    ConfigSht = "configPAS"
    CobeConfigSht = "ccPAS"
    swSht = "swPAS"
Else
    MsgBox "Software version error: " & SWVer
    GenerateConfig = 0
    Exit Function
End If

' Unhide the config sheets
Worksheets(ConfigSht).Visible = True
Worksheets(CobeConfigSht).Visible = True
Worksheets(swSht).Visible = True

' Generate User Config
Worksheets(ConfigSht).Copy
On Error Resume Next
Kill mypath & Config
Worksheets(ConfigSht).SaveAs filename:=mypath & Config, FileFormat:=xlTextPrinter
Workbooks(Config).Close SaveChanges:=False

' Generate Cobe Config
Worksheets(CobeConfigSht).Copy
On Error Resume Next
Kill mypath & cobeconfig
Worksheets(CobeConfigSht).SaveAs filename:=mypath & cobeconfig, FileFormat:=xlTextPrinter
Workbooks(cobeconfig).Close SaveChanges:=False
    
' Generate sw.dat; only used for some versions
Worksheets(swSht).Copy
On Error Resume Next
Kill mypath & swdat
Worksheets(swSht).SaveAs filename:=mypath & swdat, FileFormat:=xlTextPrinter 'xlCSV
Workbooks(swdat).Close SaveChanges:=False
    
' Hide the config sheets
Worksheets(ConfigSht).Visible = False
Worksheets(CobeConfigSht).Visible = False
Worksheets(swSht).Visible = False

EOConfig:
GenerateConfig = 1
End Function

Sub CleanupConfigs()
' Delete the config files
Dim mypath As String
Dim Config As String
Dim cobeconfig As String

    mypath = ActiveWorkbook.Path & "\"
    Config = "config.dat"
    cobeconfig = "cobe_config.dat"
    On Error Resume Next
    Kill mypath & Config
    Kill mypath & cobeconfig
    Kill mypath & "sw.dat"
End Sub
