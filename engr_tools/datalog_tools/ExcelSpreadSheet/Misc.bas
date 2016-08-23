Attribute VB_Name = "Misc"
Option Explicit
Public InputChange_Multi As Boolean
Public InputChange_Single As Boolean

Sub TransferDonors()
Attribute TransferDonors.VB_ProcData.VB_Invoke_Func = " \n14"
'Copy either statistical or fixed donor sample to Details sheet
Dim DonorRange As Range
Dim DonorType As String

DonorType = Sheets("Donor Sample").Cells(2, 5)
Sheets("DS Analysis").Visible = True

    ' Pick range based on user input
    If DonorType = "Statistical" Then
        Sheets("DS Analysis").Select
        Set DonorRange = Range("N9:R9")
    ElseIf DonorType = "Direct Input" Then
        Sheets("DS Analysis").Select
        Set DonorRange = Range("AB9:AF9")
    Else
        MsgBox "Donor Sample selection problem."
        Exit Sub
    End If
    
    ' Copy donors to Details sheet
    DonorRange.Select
    Range(Selection, Selection.End(xlDown)).Select
    Selection.Copy
    Sheets("Details").Select
    Range("D10").Select
    Selection.PasteSpecial Paste:=xlPasteValues, Operation:=xlNone, SkipBlanks _
        :=False, Transpose:=False
    Range("A1").Select

Sheets("DS Analysis").Visible = False
End Sub
Sub DonorsStat()
'Copy Statistical DS to Fixed DS Without Activation(Donor Sample Sheet)
    ' Generate ABO Types
    ABOGen ("Stat")
    
    Application.ScreenUpdating = False
    ActiveWorkbook.Unprotect
    Sheets("DS Analysis").Visible = True
    Sheets("DS Analysis").Select
    
    ' Copy without activation.
    Application.Calculation = xlCalculationManual
    
    Range("G9:K1008").Select
    Selection.Copy
    Range("O9").Select
    Selection.PasteSpecial Paste:=xlValues, Operation:=xlNone, _
        SkipBlanks:=False, Transpose:=False
    Application.CutCopyMode = False
    
    ' Copy Stats to Last Loaded Inputs
    Range("C7:C18").Select
    Selection.Copy
    Range("C44").Select
    Selection.PasteSpecial Paste:=xlPasteValues, Operation:=xlNone, SkipBlanks _
        :=False, Transpose:=False
    Application.CutCopyMode = False
    
    Range("A1").Select
    Sheets("DS Analysis").Visible = False
    Sheets("Donor Sample").Select
    Range("A1").Select
    
    'ActiveWorkbook.Protect
    ActiveWindow.ScrollWorkbookTabs Position:=xlFirst
    Application.Calculation = xlCalculationAutomatic
End Sub

Sub ToDirectory()
    ProtectIt
    Sheets("MD Summary").Visible = False
    Sheets("MD Details").Visible = False
    Sheets("MD DQP Stats").Visible = False
    Sheets("SD Details").Visible = False
    Sheets("SD_Details").Visible = False
    Sheets("UserConfig").Visible = False
    Sheets("Details").Visible = False
    
    Sheets("Directory").Select
    Range("A1").Select
End Sub
Sub ToCandP()
    Sheets("Configs & Procedures").Select
    Range("A1").Select
End Sub
Sub ToDS()
    Sheets("Donor Sample").Select
    Range("A1").Select
End Sub
Sub ToMDSum()
    Sheets("MD Summary").Visible = True
    Sheets("MD Summary").Select
    Range("A1").Select
End Sub
Sub ToMDDet()
    Sheets("MD Details").Visible = True
    Sheets("MD Details").Select
    Range("A1").Select
End Sub
Sub ToDQP()
    Sheets("MD DQP Stats").Visible = True
    Sheets("MD DQP Stats").Select
    Range("A1").Select
End Sub
Sub ToSDDet()
    Sheets("SD Details").Visible = True
    Sheets("SD Details").Select
    Range("A1").Select
End Sub
Sub ToUConfig()
    Sheets("UserConfig").Visible = True
    Sheets("UserConfig").Select
    Range("A1").Select
End Sub
Sub UpdateResults()
' Runs Multi or Single simulation based on user selection
Dim SD As String

SD = Worksheets("Donor Sample").Range("E2").value
If SD = "Single Donor" Then
    SingleDonorTable
ElseIf SD = "Direct Input" Then
    UpdatePredictTable
ElseIf SD = "Statistical" Then
    UpdatePredictTable
Else
    MsgBox "Simulation Error"
    Exit Sub
End If

End Sub

Sub SetSimStatus()
Worksheets("Directory").Select
    If InputChange_Multi = True Then
        Range("H28") = "Multi-Donor Inputs have changed."
    Else
        Range("H28") = "Multi-Donor Inputs are current."
    End If

    If InputChange_Single = True Then
        Range("K28") = "Single-Donor Inputs have changed."
    Else
        Range("K28") = "Single-Donor Inputs are current."
    End If
Range("A1").Select
End Sub

Sub CleanUp()
    ActiveSheet.UsedRange ' resize sheet size to minimum
End Sub

Sub ProtectIt()
    ActiveSheet.UsedRange ' resize sheet size to minimum
    ActiveSheet.Protect DrawingObjects:=True, Contents:=True, Scenarios:=True
End Sub

Sub AddinCheck() 'Loop through number of installed addins, if it doesn't find it or out of date, Install it.
On Error GoTo ErrUpdate
Dim ad
Dim Addin51 As AddIn
Dim Addin52 As AddIn
Dim Addin60 As AddIn
Dim Addin61 As AddIn
Dim appPath As String
Dim response As Integer

'Where the [hidden] add-in files are kept on the network
appPath = "\\bctquad3\home\Excel Tool Chest\B  Field Support Tools\Trima\"

'Copy addins if network is available
Dim copyto As String
Dim Trima_addin As String
Dim fso As New FileSystemObject
Dim i As Integer
Dim AddVer As Variant 'Array of local filenames from spreadsheet

copyto = CStr(Environ("USERPROFILE") & "\Application Data\Microsoft\AddIns\")
If DirectoryExists(copyto) = False Then MsgBox "Are add-ins in a custom directory? Can't get new versions from network."
If DirectoryExists(appPath) = False Then
Else

    'Ask to update add-ins
'    response = MsgBox("Check for Trima software updates?", vbYesNo, "RECOMMENDED!")
'    If response = 7 Then
'        MsgBox "Simulations may not function properly"
'        Exit Sub
'    ElseIf response = 6 Then
'    End If

    Trima_addin = Dir(appPath, vbHidden)
    Do While Trima_addin <> ""
        If Trima_addin Like "*.xll" Then
            For Each ad In Application.Addins
                If ad.Name = Trima_addin Then
                    If AddinsCurrent(Trima_addin, appPath) <> 0 Then ' skip this if add-ins are current
                        If ad.Installed = True Then ad.Installed = False
                        Application.ExecuteExcel4Macro "UNREGISTER(""" & Trima_addin & """)"
                        fso.CopyFile appPath & Trima_addin, copyto, True
                        Application.RegisterXLL copyto & Trima_addin
                        ' Write new file mod date to spreadsheet
                        AddVer = Worksheets("IFU").Range("Z4:AA13").value
                        For i = 1 To 10
                            If Trima_addin = AddVer(i, 1) Then
                                Worksheets("IFU").Range("AA" & 3 + i & "").value = GetFileInfo(copyto & Trima_addin)
                                Exit For
                            End If
                        Next
                        Exit For
                    End If
                End If
                If Trima_addin = ad.Name Then Exit For
            Next
        End If
        Trima_addin = Dir
    Loop
End If

ResumeUpdate:
On Error GoTo ErrAdd
' Better way to do this:
'Dim filenames, fn As Variant
'filenames = Array("ExcelPredict.5.1.xll", "ExcelPredict.5.2.xll", "ExcelPredict.6.0.xll", "ExcelPredict.6.1.xll")
'
'For Each fn In filenames
'    ...
'    'Debug.Print fn
'Next

    'Check and add 5.1:
    For Each ad In Application.Addins
        If ad.Name = "ExcelPredict.5.1.xll" Then
            ad.Installed = True
            GoTo v51_Done
        End If
    Next
    Set Addin51 = Application.Addins.Add(filename:=copyto & "ExcelPredict.5.1.xll", CopyFile:=True)
    Addins("ExcelPredict.5.1").Installed = True
v51_Done:
    
    'Check and add 5.2:
    For Each ad In Application.Addins
        If ad.Name = "ExcelPredict.5.2.xll" Then
            ad.Installed = True
            GoTo v52_Done
        End If
    Next
    Set Addin52 = Application.Addins.Add(filename:=copyto & "ExcelPredict.5.2.xll", CopyFile:=True)
    Addins("ExcelPredict.5.2").Installed = True
v52_Done:
    
    'Check and add 6.0:
    For Each ad In Application.Addins
        If ad.Name = "ExcelPredict.6.0.xll" Then
            ad.Installed = True
            GoTo v60_Done
        End If
    Next
    Set Addin60 = Application.Addins.Add(filename:=copyto & "ExcelPredict.6.0.xll", CopyFile:=True)
    Addins("ExcelPredict.6.0").Installed = True
v60_Done:
    
    'Check and add 6.1:
    For Each ad In Application.Addins
        If ad.Name = "ExcelPredict.6.1.xll" Then
            ad.Installed = True
            GoTo v61_Done
        End If
    Next
    Set Addin61 = Application.Addins.Add(filename:=copyto & "ExcelPredict.6.1.xll", CopyFile:=True)
    Addins("ExcelPredict.6.1").Installed = True
v61_Done:

ExitAdd:
    Exit Sub
ErrAdd:
    MsgBox "Add-in errors.  You must have BCT network access the first time you open this tool."
    Resume ExitAdd
ErrUpdate:
    MsgBox "Add-in update errors."
    Resume ResumeUpdate
End Sub

Sub VersionCheck()
On Error GoTo ErrorVC
Dim appPath As String
Dim toolname As String
Dim version_net As String
Dim version_loc As String
Dim DE As Boolean ' Directory Exists
Dim OE As Integer ' Open Explorer?

toolname = "[TAPT]" ' Replace with tool running this code
version_loc = Range("version_loc") ' typ. on IFU sheet
version_net = "Error"
appPath = "\\bctquad3\home\Excel Tool Chest\B  Field Support Tools\Trima\"
DE = False
OE = 7

' Check access to network folder and get network version
DE = DirectoryExists(appPath)
If DE = True Then
    version_net = INI_FileVersion(toolname)
Else
    GoTo ExitVC
End If

' Compare versions
If StrComp(version_loc, version_net) <> 0 Then
    If version_net = "Error" Then
        GoTo ErrorVC
    Else
        OE = MsgBox("Tool update available. You should download a new version from the Tool Chest" & _
        vbCrLf & "Would you like to open Explorer?", vbYesNo, "RECOMMENDED")
        If DE = True And OE = 6 Then Shell "Explorer.exe /n,/e," & appPath, vbNormalFocus
    End If
End If
Debug.Print version_loc, version_net
ExitVC:
    Exit Sub
ErrorVC:
    MsgBox "Error reading tool version from the network."
    GoTo ExitVC
End Sub
Function INI_FileVersion(toolname As String) As String
On Error GoTo ErrorINIF
Dim myIniFile As String
Dim iniBuff As Integer
Dim rawLineInput As String
Dim toolversion As String
Dim toolA() As String

'set up and open .ini file
myIniFile = "\\bctquad3\home\Excel Tool Chest\A  Tool Index And Status\ETC_VersionTracker.ini"
iniBuff = FreeFile() ' get available file buffer number
Open myIniFile For Input As #iniBuff

Do While Not (EOF(iniBuff))
    Line Input #iniBuff, rawLineInput ' get line from file
    If Left(rawLineInput, 1) = "[" Then ' check for section name
        If rawLineInput = toolname Then GoTo FoundFile
    End If
Loop
GoTo ErrorINIF ' Didn't find toolname, exiting fxn

FoundFile:
Line Input #iniBuff, toolversion ' moves to next line in ini file where version should be
If Not (toolversion Like "version=*") Then GoTo ErrorINIF
toolA = Split(toolversion, "=")
toolversion = Trim(toolA(1))
INI_FileVersion = toolversion

ExitINIF:
    Exit Function
ErrorINIF:
    INI_FileVersion = "Error"
    Resume ExitINIF
End Function
Sub ABOGen(DonorSample As String)
Dim i As Integer
Dim col As Integer

Application.ScreenUpdating = False
Application.Calculation = xlCalculationManual
Worksheets("DS Analysis").Visible = True
Worksheets("DS Analysis").Select

If DonorSample = "Direct" Then
    col = 35
ElseIf DonorSample = "Stat" Then
    col = 21
End If

For i = 9 To 1008
        Cells(i, col) = Rnd()
Next i
Application.Calculation = xlCalculationAutomatic
Worksheets("DS Analysis").Visible = False
Worksheets("Donor Sample").Select
Application.ScreenUpdating = True
End Sub

Public Function DirectoryExists(strDirPath As String) As Boolean
On Error Resume Next
DirectoryExists = False

If Len(Dir(strDirPath, vbDirectory)) = 0 Then
    DirectoryExists = False
Else
    DirectoryExists = True
End If

End Function

Sub MakeStatic()
' Makes static Sheet, JP 5/21/2010
Dim shName As String
Dim shNameNew As String
Dim sh As Variant

    shName = ActiveSheet.Name
    shNameNew = "Static" & shName
    
    For Each sh In Sheets
        If sh.Name = shNameNew Then _
        shNameNew = shNameNew & CStr(Sheets.Count) ' avoid dup names
    Next
    
    Sheets.Add After:=Sheets(Sheets.Count)
    Sheets(Sheets.Count).Select
    Sheets(Sheets.Count).Name = shNameNew
    ActiveWindow.Zoom = 75
    Sheets(shName).Select
    Cells.Select
    Selection.Copy
    Sheets(shNameNew).Select
    Selection.PasteSpecial Paste:=xlPasteValues, Operation:=xlNone, SkipBlanks:=False, Transpose:=False
    Selection.PasteSpecial Paste:=xlPasteFormats, Operation:=xlNone, SkipBlanks:=False, Transpose:=False
End Sub

Function GetFileInfo(filename As String) As String
Dim fs
    Set fs = CreateObject("scripting.filesystemobject").GetFile(filename)
    GetFileInfo = fs.DateLastModified
    Set fs = Nothing
End Function

Function AddinsCurrent(Trima_addin As String, appPath As String) As Integer
On Error GoTo ErrAC
Dim ModDate_net As String
Dim ModDate_loc As String
Dim i As Integer
Dim Addins As Variant 'Array of local filenames from spreadsheet

Addins = Worksheets("IFU").Range("Z4:AB13").value
For i = 1 To 10
    If Trima_addin = Addins(i, 1) Then ' find correct addin
        If Addins(i, 3) = "N" Then ' check for override
            ModDate_loc = Addins(i, 2)
            Debug.Print Addins(i, 1), Addins(i, 2), Addins(i, 3)
            GoTo FoundMatch
        Else
            AddinsCurrent = 0
            GoTo ExitAC
        End If
    End If
Next

GoTo ErrAC '*** Handle no match found!!!
FoundMatch:
ModDate_net = GetFileInfo(appPath & Trima_addin)
AddinsCurrent = StrComp(ModDate_net, ModDate_loc)
'Debug.Print Trima_addin, ModDate_loc, ModDate_net

ExitAC:
Exit Function
ErrAC:
AddinsCurrent = 1
Resume ExitAC
End Function
