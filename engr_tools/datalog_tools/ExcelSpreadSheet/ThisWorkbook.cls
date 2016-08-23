VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "ThisWorkbook"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Option Explicit
Option Compare Text

Private Sub Workbook_Open()
    AddinCheck
    VersionCheck
   
    ' Regenerate MD Details formulas
    Application.ScreenUpdating = False
    Worksheets("MD Details").Visible = True
    Worksheets("MD Details").Select
    Worksheets("MD Details").Unprotect
    
    Range("C16").Select
    Range(Selection, Selection.End(xlToRight)).Select
    Selection.Copy
    Range("C17:CA1015").Select
    Selection.PasteSpecial Paste:=xlPasteFormulas, Operation:=xlNone, _
        SkipBlanks:=False, Transpose:=False
    Application.CutCopyMode = False
    Range("A1").Select
    Worksheets("MD Details").Protect
    Worksheets("MD Details").Visible = False
    
    ' Clean up and reset single donor sheet
    Worksheets("SD_Details").Range("A30:CF2000").Delete xlShiftUp
    Worksheets("SD_Details").Range("T2") = 1
    
    ' Reset to IFU
    Sheets("IFU").Select
    Range("A1").Select
    Application.ScreenUpdating = True
    
    ' Check International Settings
    If Application.International(xlThousandsSeparator) <> "," Then
        MsgBox "Thousands separator: Please change your Regional and Language Options to 'English (United States)' and re-open this tool."
    End If
    If Application.International(xlDecimalSeparator) <> "." Then
        MsgBox "Decimal separotor:Please change your Regional and Language Options to 'English (United States)' and re-open this tool."
    End If
    
    'Check that it's not running off of the network
    Dim mypath As String
    Dim netpath As String
    
    netpath = "\\bctquad3\home\Excel Tool Chest\B Field Support Tools\Trima"
    mypath = ActiveWorkbook.Path
    'MsgBox mypath
    If mypath = netpath Then
        MsgBox "DO NOT run tool from network.  Please save to your computer."
    End If
    
End Sub

Private Sub Workbook_BeforeClose(Cancel As Boolean)
    CleanupConfigs
    
    ' Clear formulas on MD Details to reduce file size
    Application.ScreenUpdating = False
    Worksheets("MD Details").Visible = True
    Worksheets("MD Details").Select
    Worksheets("MD Details").Unprotect
    
    Range("C17").Select
    Range(Selection, Selection.End(xlToRight)).Select
    Range(Selection, Selection.End(xlDown)).Select
    Selection.ClearContents
    
    Worksheets("MD Details").Protect
    Worksheets("MD Details").Visible = False
    Sheets("IFU").Select
    Application.ScreenUpdating = True
    'ActiveWorkbook.Save
End Sub
Private Sub Workbook_SheetActivate(ByVal sh As Object)
    CleanUp
End Sub
