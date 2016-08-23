Attribute VB_Name = "SingleDonor"
Option Explicit

Sub SingleDonorTable()
' Runs predict code and outputs on Details sheet
 ' Most of this code was borrowed from UpdatePredictTable()
'On Error GoTo SDErr

Dim filenameUserConfig As String
Dim filenameCobeConfig As String
Dim filenamesw As String
Dim SWVer As Single
Dim productindex As Integer
Dim productValue As Integer
Dim productLimit As Integer
Dim i As Integer
Dim ii As Integer
Dim j As Integer
Dim startTime As Single
Dim stopTime As Single
Dim elapsedTime As Single
Dim x, y As Integer
Dim toolname As String
Dim sw
Dim SW51 As C51
Dim SW52 As C52
Dim SW60 As C60
Dim SW61 As C61
Dim SW63 As C63
Dim SWPAS As CPAS
Dim PQ As Integer
Dim CI As Integer
Dim ResultNum As Integer
Dim Donors As Variant
Dim DonorsRange As Range
Dim AOutput() As Variant
Dim PQArray() As Integer
Dim CSFArray() As Variant
Dim RpmArray() As Variant
Dim MyRange As Range
Dim PQRange As Range
Dim CSFRange As Range
Dim RpmRange As Range
Dim Ytarg As Single
Dim RBCtarg As Single
Dim PLStarg As Single
Dim Tpir As Single
Dim Tepca As Single
Dim Tepc As Single
Dim Tmin As Single
Dim Tppp As Single
Dim Tpca As Single
Dim Status As Single
Dim DonorNum As Integer
Dim Fm As Single
Dim bOver As Range

Dim netpath As String
   
netpath = "\\bctquad3\home\TempTrimaTools\"

' Setup
Application.EnableCancelKey = XlEnableCancelKey.xlDisabled
toolname = ActiveWorkbook.Name
Application.ScreenUpdating = False
startTime = Time
Workbooks(toolname).Activate
Sheets("Configs & Procedures").Select
SWVer = Range("H11").value
'filenameUserConfig = ActiveWorkbook.Path & "\config.dat"
filenameUserConfig = netpath & SWVer & "config\config.dat"
'filenameCobeConfig = ActiveWorkbook.Path & "\cobe_config.dat"
filenameCobeConfig = netpath & SWVer & "config\cobe_config.dat"
'filenamesw = ActiveWorkbook.Path & "\sw.dat"
filenamesw = netpath & SWVer & "config\sw.dat"
productindex = 0
productValue = 0
Tmin = 0.5      ' Minimum time for a real substate, min.
If Range("FmSetting") = "YES" Then ' Set Fm for Postcount algorithm calc
    Fm = 0.8
Else
    Fm = 1#
End If

' Delete old data
Worksheets("SD_Details").Range("I10:BW2000").ClearContents
Worksheets("SD_Details").Range("H30:H2000").ClearContents

' Generate config files
If GenerateConfig() <> 1 Then
    MsgBox "Config generation crashed."
    Exit Sub
End If
'On Error GoTo SDErr
Workbooks(toolname).Activate

' Set Software version based on user input
    ' Add new software versions to this check
If SWVer = 5.1 Then
    Set SW51 = New C51
    Set sw = SW51
ElseIf SWVer = 5.2 Then
    Set SW52 = New C52
    Set sw = SW52
ElseIf SWVer = 6# Then
    Set SW60 = New C60
    Set sw = SW60
ElseIf SWVer = 6.1 Then
    Set SW61 = New C61
    Set sw = SW61
ElseIf SWVer = 6.3 Then
    Set SW63 = New C63
    Set sw = SW63
ElseIf SWVer = 1 Then
    MsgBox "Use ConfigLoop sheet instead."
ElseIf SWVer = 2 Then
    Set SWPAS = New CPAS
    Set sw = SWPAS
End If

With sw
    ' Create optimizer
    If .LoadOptimizerFromConfigs(filenameUserConfig, filenameCobeConfig, filenamesw) <> 1 Then
        MsgBox "Error loading Config.dat, CobeConfig.dat, or sw.dat."
        Sheets("Directory").Select
        Exit Sub
    End If
    
    productLimit = Worksheets("DS Analysis").Range("BC64") + 1
'    If SWVer > 5.99 Then
'        productLimit = 21
'    Else
'        productLimit = 16
'    End If
    
    'Determine configured procedures
    y = Worksheets("DS Analysis").Range("BC64")
    
    ' ReDim arrays to fit # of entries
    ResultNum = 56 'Number of results for the array
    ReDim AOutput(y, 1 To ResultNum)
    ReDim CSFArray(y, 7)
    
    
    ' Transfer multiple donors to SD_Details sheet for proc info for all procs
    Sheets("SD_Details").Visible = True
    Sheets("SD_Details").Select
    DonorNum = Range("T2").value
    Cells(4, 2) = "ACCEL VERSION " & Format(SWVer, "0.0")
    
    ' Loop through requested donors
    ReDim AOutput(y, 1 To ResultNum)
    ReDim CSFArray(y, 7)
    ReDim RpmArray(y, 7)
   For ii = 0 To DonorNum - 1
    If DonorNum > 1 And ii > 0 Then
            Cells(10 + 20 * ii, 3) = Worksheets("DS Analysis").Cells(9 + ii, 28)
            Cells(10 + 20 * ii, 4) = Worksheets("DS Analysis").Cells(9 + ii, 29)
            Cells(10 + 20 * ii, 5) = Worksheets("DS Analysis").Cells(9 + ii, 30)
            Cells(10 + 20 * ii, 6) = Worksheets("DS Analysis").Cells(9 + ii, 31)
            Cells(10 + 20 * ii, 7) = Worksheets("DS Analysis").Cells(9 + ii, 32)
    End If
    
    ' Load donors into Array
    Sheets("SD_Details").Select
    Set DonorsRange = Range(Cells(10 + 20 * ii, 3), Cells(10 + 20 * ii, 7))
    Donors = DonorsRange.Resize(1, 5)
    ' Check for incomplete donor parameters
    If Donors(1, 1) = "" Or Donors(1, 2) = "" Or Donors(1, 3) = "" Then
        MsgBox "Incomplete Donor Parameters"
        Sheets("SD_Details").Visible = False
        ActiveWindow.ScrollWorkbookTabs Position:=xlFirst
        GoTo SDExit
    End If
    .tbv CSng(Donors(1, 1))
    .precount CSng(Donors(1, 2))
    .hct CSng(Donors(1, 3))
    .weight CSng(Donors(1, 4))
    .height CSng(Donors(1, 5))
    .blood_type 510
    .female Range("DonorSex") '1=female
    
    ' Update optimizer
    If .UpdateOptimizerConditions <> 1 Then
        MsgBox "Error updating donor"
        GoTo SDExit
    End If
    
    productindex = 0
    productValue = 0
    
    For i = 0 To y - 1
        productindex = productindex + 1
        ' update config
        Dim cc As Integer
        Set bOver = Worksheets("ConfigLoop").Range("bOverride")
        If bOver = "Yes" Then
            'clear old product and proc values
            For cc = 1 To 20
                If cc < 11 Then .Set_key_plt_yield cc, 0
                If cc < 11 Then .Set_key_plt_volume cc, 0
                If cc < 7 Then .Set_key_pls_volume cc, 0
                If cc < 4 Then .Set_key_rbc_dose cc, 0
                .Set_key_platelet cc, 0
                .Set_key_plasma cc, 0
                .Set_key_rbc cc, 0
            Next cc
            ' set proc 1 to first product values
            .Set_key_platelet 0, 1
            .Set_key_plasma 0, 1
            .Set_key_rbc 0, 1
            productindex = 1
            ' set new configs from list
            .Set_key_plt_yield 0, Worksheets("ConfigLoop").Range("C" & 5 + i)
            .Set_key_plt_volume 0, Worksheets("ConfigLoop").Range("D" & 5 + i)
            .Set_key_pls_volume 0, Worksheets("ConfigLoop").Range("F" & 5 + i)
            .Set_key_rbc_dose 0, Worksheets("ConfigLoop").Range("H" & 5 + i)
            
            .UpdateConfigurations
            .OptimizeProduct (1)
        Else
            .OptimizeProduct (productindex)
        End If
        
        
        ' Build an array of results and write the sheet once, much faster
         ' Set variables for IF statements
        Ytarg = .getDesiredYield(productindex)
        RBCtarg = .getProcRBCVolume(productindex)
        PLStarg = .getPredVp(productindex)
        Tppp = .S_T(productindex, 2)
        Tpca = .S_T(productindex, 5)
        Tpir = .S_T(productindex, 8)
        Tepca = .S_T(productindex, 9)
        Tepc = .S_T(productindex, 10)
        Status = .getStatus(productindex)
        
        'Substates:
        '2= PrePlateletPlasma
        '3=
        '4=
        '5= PCA
        '6= MidRun
        '7= PIRPlasma (rare)
        '8= PIRNoPlasma
        '9= EPCA
        '10= EPC
       
        If Ytarg > 0 Then AOutput(i, 1) = Ytarg
        If Ytarg > 0 And .getPredVc(productindex) > 0 Then AOutput(i, 2) = .getPredVc(productindex)
        If PLStarg > 1 And PLStarg < 5000 Then AOutput(i, 3) = PLStarg
        If RBCtarg > 0 Then AOutput(i, 4) = RBCtarg
       If Status = 0 Then
        AOutput(i, 5) = .getPredTp(productindex)
        If Ytarg > 0 Then AOutput(i, 6) = .getTc(productindex)
        If Ytarg > 0 Then AOutput(i, 7) = .getQinc(productindex)
        If Ytarg > 0 Then AOutput(i, 8) = .getQcmr(productindex)
        If SWVer > 5.3 Then
            AOutput(i, 9) = .getMaxQin(productindex)
        Else
            AOutput(i, 9) = ""
        End If
        If PLStarg > 0 And Ytarg > 0 Then AOutput(i, 10) = .S_Qrinst(productindex, 5)       '**PP R flow
        If Ytarg > 0 Then AOutput(i, 11) = .S_Qrinst(productindex, 6)       '**MR R flow
        If Ytarg > 0 Then
        If PLStarg > 0 And AOutput(i, 10) > 0 Then AOutput(i, 12) = .S_Qininstr(productindex, 5) / .S_Qininstd(productindex, 5) '**Qin R/D PP
        If AOutput(i, 11) > 0 Then AOutput(i, 13) = .S_Qininstr(productindex, 6) / .S_Qininstd(productindex, 6) '**Qin R/D MR
        AOutput(i, 14) = .VinTarget(productindex)        '**Vintarg 'Same as VinTarget
        End If
        AOutput(i, 15) = .S_EndVin(productindex, .getNumOfSubstates(productindex) - 1)    '**Vin
        AOutput(i, 16) = .getPredVac(productindex)       '**Vac
        If Ytarg > 0 Then AOutput(i, 17) = .getProcPlateletProdCon(productindex) '**Cplt
        If Ytarg > 0 Then AOutput(i, 18) = Donors(1, 2) - (Fm * .getDesiredYield(productindex) * 100000000000# / (Donors(1, 1) * 1000000#)) '**Cpost:(_PltPre-(Fm * _PltYield/(_TBV*1E6f)))
        AOutput(i, 19) = ((Donors(1, 1) * Donors(1, 3)) - (.getPredRBCDose(productindex) + .getResidualRBCVolume(productindex) + (20 * Donors(1, 3)))) / Donors(1, 1) '**Hpost
        'Substate Times:
        If PLStarg > 0 And Ytarg > 0 And Tppp > Tmin Then AOutput(i, 20) = Tppp '**PP/Pls
        If PLStarg > 0 And Ytarg > 0 Then AOutput(i, 21) = .S_T(productindex, 4)   '**PP
        If PLStarg > 0 And Ytarg > 0 And Tpca > Tmin Then AOutput(i, 22) = .S_T(productindex, 5) '**PCA
        If Ytarg > 0 Then AOutput(i, 23) = .S_T(productindex, 6)           '**MR
        If Tpir > Tmin And Ytarg > 0 Then AOutput(i, 24) = Tpir        '**PIR
        If Tepca > Tmin And Ytarg > 0 Then AOutput(i, 25) = Tepca         '**EPCA
        If Tepc > Tmin And Ytarg > 0 Then AOutput(i, 26) = Tepc          '**EPC
        If RBCtarg > 0 Then AOutput(i, 27) = .S_T(productindex, .RBCState(productindex))        'RBC
        If Ytarg > 0 Then AOutput(i, 28) = .totalPurges(productindex)      '**Number of purges
        If Ytarg > 0 Then AOutput(i, 29) = .getYdpur(productindex)         '**Y loss Purge
        If Ytarg > 0 Then AOutput(i, 30) = .getYdch(productindex)          '**Y loss LRS
        If Ytarg > 0 Then AOutput(i, 31) = .YdchPIR(productindex)          '**Y loss No PIR
        If Ytarg > 0 Then
         'If SWVer <> 6.1 Then
          If Tppp > Tmin And PLStarg > 0 Then AOutput(i, 32) = .S_Qininstd(productindex, 2)         '**PP/Pls
         'Else
         ' If Tppp > Tmin And PLStarg > 0 Then AOutput(i, 32) = .getPrePltPlasmaEndQin(productindex)         '**PP/Pls
         'End If
         If PLStarg > 0 Then AOutput(i, 33) = .S_Qininstd(productindex, 4)           '**PP
         If Tpca > Tmin And PLStarg > 0 Then AOutput(i, 34) = .S_Qininstd(productindex, 5)         '**PCA
         AOutput(i, 35) = .S_Qininstd(productindex, 6)                                '**MR
         If Tpir > Tmin Then AOutput(i, 36) = .S_Qininstd(productindex, 8)            '**PIR
         If Tepca > Tmin Then AOutput(i, 37) = .S_Qininstd(productindex, 9)           '**PCAE
         If Tepc > Tmin Then AOutput(i, 38) = .S_Qininstd(productindex, 10)           '**PCE
        End If
        If RBCtarg > 0 Then AOutput(i, 39) = .S_Qininstd(productindex, .RBCState(productindex))          '**RBC
        If Ytarg > 0 Then
         If Tppp > Tmin And PLStarg > 0 Then AOutput(i, 40) = .S_Qacinst(productindex, 2)           '**PP/Pls
         If PLStarg > 0 Then AOutput(i, 41) = .S_Qacinst(productindex, 4)           '**PP
         If Tpca > Tmin And PLStarg > 0 Then AOutput(i, 42) = .S_Qacinst(productindex, 5)           '**PCA
        AOutput(i, 43) = .S_Qacinst(productindex, 6)                                '**MR
         If Tpir > Tmin Then AOutput(i, 44) = .S_Qacinst(productindex, 8)           '**PIR
         If Tepca > Tmin Then AOutput(i, 45) = .S_Qacinst(productindex, 9)          '**PCAE
         If Tepc > Tmin Then AOutput(i, 46) = .S_Qacinst(productindex, 10)          '**PCE
        End If
        If RBCtarg > 0 Then AOutput(i, 47) = .S_Qacinst(productindex, .RBCState(productindex))          '**RBC
        If Ytarg > 0 Then
         If Tppp > Tmin And PLStarg > 0 Then AOutput(i, 48) = .S_Qch(productindex, 2)       '**PP/Pls
         If PLStarg > 0 Then AOutput(i, 49) = .S_Qch(productindex, 4)                       '**PP
         If Tpca > Tmin And PLStarg > 0 Then AOutput(i, 50) = .S_Qch(productindex, 5)       '**PCA
         AOutput(i, 51) = .S_Qch(productindex, 6)                                           '**MR
         If Tpir > Tmin Then AOutput(i, 52) = .S_Qch(productindex, 8)                       '**PIR
         If Tepca > Tmin Then AOutput(i, 53) = .S_Qch(productindex, 9)                      '**EPCA
         If Tepc > Tmin Then AOutput(i, 54) = .S_Qch(productindex, 10)  '**EPC
         AOutput(i, 56) = CInt(.get_Vinpur(productindex))
        End If
       End If
         AOutput(i, 55) = Status
         
         'Generate 6.1 CSF array
'        If SWVer = 6.1 And Status = 0 Then
'         CSFArray(i, 0) = .Csf(productindex, 2)
'         CSFArray(i, 1) = .Csf(productindex, 4)
'         CSFArray(i, 2) = .Csf(productindex, 5)
'         CSFArray(i, 3) = .Csf(productindex, 6)
'         CSFArray(i, 4) = .Csf(productindex, 8)
'         CSFArray(i, 5) = .Csf(productindex, 9)
'         CSFArray(i, 6) = .Csf(productindex, 10)
'         If RBCtarg > 0 Then CSFArray(i, 7) = .Csf(productindex, .RBCState(productindex))
'        End If
        'Generate 6.1 Rpm array
'        If SWVer = 6.1 And Status = 0 Then
'         RpmArray(i, 0) = .Rpm(productindex, 2)
'         RpmArray(i, 1) = .Rpm(productindex, 4)
'         RpmArray(i, 2) = .Rpm(productindex, 5)
'         RpmArray(i, 3) = .Rpm(productindex, 6)
'         RpmArray(i, 4) = .Rpm(productindex, 8)
'         RpmArray(i, 5) = .Rpm(productindex, 9)
'         RpmArray(i, 6) = .Rpm(productindex, 10)
'         If RBCtarg > 0 Then RpmArray(i, 7) = .Rpm(productindex, .RBCState(productindex))
'        End If
        
    Next i

    ' Write arrays to SD_Details sheet
    Sheets("SD_Details").Select
    CI = 63                     'Column Offset
    Set MyRange = Range(Cells(10 + 20 * ii, 9), Cells(y + 9 + 20 * ii, 9 + ResultNum - 1))
    MyRange.value = AOutput
    If SWVer = 6.1 Then
        Set CSFRange = Range(Cells(10 + 20 * ii, 5 + CI), Cells(29 + 20 * ii, 12 + CI))
        CSFRange.value = CSFArray
        Set RpmRange = Range(Cells(10 + 20 * ii, 9 + 5 + CI), Cells(29 + 20 * ii, 9 + 12 + CI))
        RpmRange.value = RpmArray
    End If
   Next ii
End With

'Copy the format so it's easy to read
Dim Paster As String
For i = 1 To DonorNum - 1
    Paster = "Paste 1: "
    Range("C10:CF29").Select
    Selection.Copy
    Range("C" & 10 + 20 * i).Select
    Selection.PasteSpecial Paste:=xlPasteFormats, Operation:=xlNone, _
        SkipBlanks:=False, Transpose:=False
    Paster = "Paste 2: "
    Range("H10:H29").Copy
    ActiveSheet.Paste Destination:=Range("H" & 10 + 20 * i & ":H" & 19 + 20 * i)
Next i

Application.CutCopyMode = False
Range("A1").Select
Sheets("SD_Details").Visible = False
ActiveWindow.ScrollWorkbookTabs Position:=xlFirst
stopTime = Time
elapsedTime = Round((stopTime - startTime) * 24 * 60 * 60, 0)
If Cells(2, 17) = "ON" Then MsgBox "Elapsed time: " & elapsedTime & " sec"
Application.ScreenUpdating = True

InputChange_Single = False
SetSimStatus

SDExit:
    Sheets("Directory").Select
    Exit Sub
SDErr:
    MsgBox Paster & Err.Description
    Resume SDExit
    
End Sub


