Attribute VB_Name = "Analyze"
Option Explicit
Public iterA() As Variant
Public iterProcs() As Single

Sub UpdatePredictTable()
On Error GoTo MDErr
' Runs predict code and outputs on Details sheet
Dim filenameUserConfig As String
Dim filenameCobeConfig As String
Dim filenamesw As String
Dim SWVer As Single
Dim productindex As Integer
Dim productValue As Integer
Dim productLimit As Integer
Dim i, j As Integer
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
Dim SWPAS As CPAS
Dim PQ As Integer
Dim CI As Integer
Dim ResultNum As Integer
Dim Donors As Variant
Dim DonorsRange As Range
Dim ABOType As Variant
Dim ABORange As Range
Dim DonorSex As Variant
Dim DSRange As Range
Dim AOutput() As Variant
Dim PQArray() As Integer
Dim MyRange As Range
Dim PQRange As Range
Dim Ytarg As Single
Dim RBCtarg As Single
Dim PLStarg As Single
Dim Tpir As Single
Dim Tepca As Single
Dim Tepc As Single
Dim Tmin As Single
Dim Tppp As Single
Dim Tpca As Single
Dim FirstGood As Integer
Dim Fm As Single
Dim bOver As Range
Dim bIter As Range
Dim tbvflag As Boolean
Dim tbvenforce As Boolean
Dim tbvlimit As Integer
Dim MOPlsflag As Boolean
Dim MOPlsenforce As Integer
Dim MOPltflag As Boolean
Dim MOPltenforce As Integer
Dim MOflag As Boolean
Dim filterloss As Integer

' Setup
Application.EnableCancelKey = XlEnableCancelKey.xlDisabled
toolname = ActiveWorkbook.Name
Application.ScreenUpdating = False
startTime = Time
Workbooks(toolname).Activate
Sheets("Configs & Procedures").Select
SWVer = Range("H11").value
filenameUserConfig = ActiveWorkbook.Path & "\config.dat"
filenameCobeConfig = ActiveWorkbook.Path & "\cobe_config.dat"
filenamesw = ActiveWorkbook.Path & "\sw.dat"
productindex = 0
productValue = 0
Tmin = 0.5      ' Minimum time for a real substate, min.
ClearData 10000 ' Delete old data
TransferDonors  ' Transfer donors to Details sheet
If Range("FmSetting") = "YES" Then ' Set Fm for Postcount algorithm calc
    Fm = 0.8
Else
    Fm = 1#
End If

' Setup tbv limit and male/AB female only options for <v6.0
tbvflag = False
tbvenforce = True
If Range("tbvswitch") = "NO" Then tbvenforce = False
tbvlimit = Range("tbvlimit") * 1000

MOPlsflag = False
MOPlsenforce = 0
If Range("MOPlsSwitch") = "YES" Then MOPlsenforce = 1
If Range("MOPlsSwitch") = "Male/AB Female" Then MOPlsenforce = 2

MOPltflag = False
MOPltenforce = 0
If Range("MOPltSwitch") = "YES" Then MOPltenforce = 1
If Range("MOPltSwitch") = "Male/AB Female" Then MOPltenforce = 2

' Check if stat. donor sample has changed
If Range("StatDiff") > 0 And Range("DBO") = 1 Then
    If MsgBox("Statistical Donor input has changed. " & vbCrLf & _
    "Don 't forget to press the button to update the sample." & vbCrLf & _
    "Cancel to EXIT, OK to Continue", _
    vbOKCancel, "Statistical Donor Sample") = 2 Then GoTo MDExit
End If

' Generate config files
If GenerateConfig() <> 1 Then
    MsgBox "Config generation crashed."
    Exit Sub
End If
Workbooks(toolname).Activate

' Set Software version based on user input
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
ElseIf SWVer = 1 Then
    MsgBox "Use ConfigLoop sheet instead."
ElseIf SWVer = 2 Then
    Set SWPAS = New CPAS
    Set sw = SWPAS
End If

On Error GoTo MDErr
Sheets("Details").Select
Cells(3, 2) = "ACCEL VERSION " & Format(SWVer, "0.0")

With sw
    ' Create optimizer
    If .LoadOptimizerFromConfigs(filenameUserConfig, filenameCobeConfig, filenamesw) <> 1 Then
        MsgBox "Error loading Config.dat, CobeConfig.dat, or sw.dat."
        Sheets("Directory").Select
        Exit Sub
    End If

    ' Count the # of donors
    y = Range("DonorCount").value

    'Determine configured procedures
    productLimit = Worksheets("DS Analysis").Range("BC64")
    
    ' ReDim arrays to fit # of entries
    ResultNum = 54 'Number of results for the array
    ReDim AOutput(y, 1 To ResultNum)
    ReDim PQArray(y, 1 To productLimit) As Integer
    
    'Load donors into Array
    Set DonorsRange = Range(Cells(10, 4), Cells(10, 8))
    Donors = DonorsRange.Resize(y, 5)
        ' Load blood type into Array: Ran out of room when BT was added
        If Range("DBO") = 1 Then
            Set ABORange = Worksheets("DS Analysis").Range("ABOStat")
        ElseIf Range("DBO") = 2 Then
            Set ABORange = Worksheets("DS Analysis").Range("ABODirect")
        End If
        ABOType = ABORange.Resize(y, 1)
        
        ' Load DonorSex
        Set DSRange = Worksheets("DS Analysis").Range("DonorSex")
        DonorSex = DSRange.Resize(y, 1)
        If Range("DBO") = 2 Then
        ElseIf Range("DBO") = 1 Then 'statistical
            For i = 1 To y
                DonorSex(i, 1) = 0 ' set all to male
            Next i
        End If

    'Loop through all the donors
    For i = 0 To y - 1
        ' Set donor information
        .tbv CSng(Donors(i + 1, 1))
            'Flag low tbv donors
            tbvflag = False
            If CSng(Donors(i + 1, 1)) < tbvlimit And SWVer < 6 _
            And tbvenforce = True Then
                tbvflag = True
            End If
        .precount CSng(Donors(i + 1, 2))
        .hct CSng(Donors(i + 1, 3))
        .weight CSng(Donors(i + 1, 4))
        .height CSng(Donors(i + 1, 5))
        .blood_type CSng(ABOType(i + 1, 1))
        .female CSng(DonorSex(i + 1, 1)) '1=female
            ' Flag female donors
            MOflag = False
            If CSng(DonorSex(i + 1, 1)) = 1 And SWVer < 6 Then MOflag = True
            If CSng(DonorSex(i + 1, 1)) = 1 And SWVer < 6 _
            And (CSng(ABOType(i + 1, 1)) = 16 _
            Or CSng(ABOType(i + 1, 1)) = 8) Then MOflag = False
        
        ' Update optimizer
        If .UpdateOptimizerConditions <> 1 Then
            MsgBox "Error updating donor"
            GoTo MDExit
        End If
        
        ' Update config
        Dim cc As Integer
        Set bOver = Worksheets("ConfigLoop").Range("bOverride")
        Set bIter = Worksheets("ConfigLoop").Range("bIterate")
        If bOver = "Yes" Then
            ' clear old product and proc values
            If i = 0 Then
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
            End If
          If bIter = "No" Then
            For PQ = 1 To productLimit 'loop through priority list to find top qualified
                    '?loop to load more than one at a time?
                .Set_key_plt_yield 0, Worksheets("ConfigLoop").Range("C" & 5 + PQ)
                .Set_key_plt_volume 0, Worksheets("ConfigLoop").Range("D" & 5 + PQ)
                .Set_key_pls_volume 0, Worksheets("ConfigLoop").Range("F" & 5 + PQ)
                .Set_key_rbc_dose 0, Worksheets("ConfigLoop").Range("H" & 5 + PQ)
                .UpdateConfigurations
                .OptimizeProduct (1)
                If .getStatus(1) = 0 Then GoTo GotOpti
            Next PQ
          ElseIf bIter = "Yes" Then ' loop through interative steps to find top qualified
            On Error GoTo 0
            productLimit = Worksheets("ConfigLoop").Range("bIterCount")
            If i = 0 Then BuildIterProcs 'build array of procs
            Worksheets("ConfigLoop").Range("J48:N" & productLimit + 47) = iterProcs
            ' loop through procs
            For PQ = 0 To productLimit - 1 'loop through priority list to find top qualified
                    '?loop to load more than one at a time?
                .Set_key_plt_yield 0, iterProcs(PQ, 1)
                .Set_key_plt_volume 0, iterProcs(PQ, 2)
                .Set_key_pls_volume 0, Application.WorksheetFunction.Lookup(iterProcs(PQ, 3), Worksheets("ConfigLoop").Range("J6:M26"))
                .Set_key_rbc_dose 0, Application.WorksheetFunction.Lookup(iterProcs(PQ, 4), Worksheets("ConfigLoop").Range("J6:N26"))
                .UpdateConfigurations
                .OptimizeProduct (1)
                If .getStatus(1) = 0 Then GoTo GotOpti
            Next PQ
          End If
        Else
            'GoTo Override  'Uncomment to use existing qualified procedure list
            ' Write array of Procedure Qualification reasons
            FirstGood = 0
            For PQ = 1 To productLimit
                .OptimizeProduct (PQ)
                PQArray(i, PQ) = .getStatus(PQ)
                If PQArray(i, PQ) = 0 And FirstGood = 0 Then ' Catch the first optimized product
                    productindex = PQ
                    FirstGood = 1
                End If
            Next PQ
            'Reset dRBC status for donors <TBV limit
            If tbvflag = True Then
                For PQ = 1 To productLimit
                    If .getProcRBCVolume(PQ) > 320 And PQArray(i, PQ) = 0 Then PQArray(i, PQ) = 30
                Next PQ
            End If
'            Force prequalified proc for direct comparison
'Override:  'Used to force an existing qualification list in range "QPRange"
'            On Error GoTo 0
'            Dim QPR As Range
'            Set QPR = Range("QPRange")
'            productindex = QPR(i + 1, 1)
'            .OptimizeProduct (productindex)
'            GoTo GotOpti
        End If
        If FirstGood = 0 Then GoTo SkipDonor ' Skip if no proc qualified
GotOpti:
        ' Build an array of results and write the sheet once, =much faster
         ' Set variables for IF statements
        Ytarg = .getDesiredYield(productindex)
        RBCtarg = .getProcRBCVolume(productindex)
            ' Remove filter loss of 10mL each from RBC target
            If RBCtarg > 0 And Range("PTFswitch") = "YES" And SWVer > 5.99 Then
                If RBCtarg > 320 Then
                    filterloss = 20
                Else
                    filterloss = 10
                End If
                RBCtarg = (RBCtarg * 0.8 - filterloss) / 0.8
            End If
        PLStarg = .getPredVp(productindex)
        Tppp = .S_T(productindex, 2)
        Tpca = .S_T(productindex, 5)
        Tpir = .S_T(productindex, 8)
        Tepca = .S_T(productindex, 9)
        Tepc = .S_T(productindex, 10)
        
        'Bailout dRBC donors <limit and find next qualified procedure
        If tbvflag = True And PQArray(i, productindex) <> 0 Then
            'GoTo SkipDonor '*no procs after dRBC will be simulated!
            For PQ = productindex To productLimit
                If PQArray(i, PQ) = 0 Then
                    productindex = PQ
                    GoTo GotOpti
                Else
                    GoTo SkipDonor 'skips if dRBC is last/only qualified
                End If
            Next PQ
        End If
        'Bailout Female Platelet/Plasma donors
        If MOflag = True And MOPlsenforce <> 0 And PLStarg > 1 Then
            For PQ = productindex + 1 To productLimit
                If PQArray(i, PQ) = 0 Then
                    productindex = PQ
                    GoTo GotOpti
                End If
            Next PQ
            GoTo SkipDonor 'skips if last/only qualified
        End If
        
        If MOflag = True And MOPltenforce <> 0 And Ytarg > 1 Then
            For PQ = productindex + 1 To productLimit
                If PQArray(i, PQ) = 0 Then
                    productindex = PQ
                    GoTo GotOpti
                End If
            Next PQ
            GoTo SkipDonor 'skips if last/only qualified
        End If
        
        'Substate enum:
        '2= PrePlateletPlasma   '3=         '4=
        '5= PCA                 '6= MidRun  '7= PIRPlasma (rare)
        '8= PIRNoPlasma         '9= EPCA    '10= EPC
        
        Sheets("Details").Visible = True
        Sheets("Details").Select
        Cells(i + 10, 3) = productindex ' Displays qualified procedure
        If Ytarg > 0 Then AOutput(i, 1) = Ytarg
        If Ytarg > 0 Then AOutput(i, 2) = .getPredVc(productindex)
        If PLStarg > 1 Then AOutput(i, 3) = PLStarg
        If RBCtarg > 0 Then AOutput(i, 4) = RBCtarg
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
        If AOutput(i, 11) > 0.001 Then AOutput(i, 13) = .S_Qininstr(productindex, 6) / .S_Qininstd(productindex, 6) '**Qin R/D MR
        AOutput(i, 14) = .VinTarget(productindex)        '**Vintarg 'Same as VinTarget
        End If
        AOutput(i, 15) = .S_EndVin(productindex, .getNumOfSubstates(productindex) - 1)    '**Vin
        AOutput(i, 16) = .getPredVac(productindex)       '**Vac
        If Ytarg > 0 Then AOutput(i, 17) = .getProcPlateletProdCon(productindex) '**Cplt
        If Ytarg > 0 Then AOutput(i, 18) = Cells(i + 10, 5) - (Fm * .getDesiredYield(productindex) * 100000000000# / (Cells(i + 10, 4) * 1000000#)) '**Cpost:(_PltPre-(_PltYield/(_TBV*1E6f)))
        AOutput(i, 19) = ((Cells(i + 10, 4) * Cells(i + 10, 6)) - (.getPredRBCDose(productindex) + .getResidualRBCVolume(productindex) + (20 * Cells(i + 10, 6)))) / Cells(i + 10, 4) '**Hpost
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
        If Ytarg > 0 And SWVer <> 6.1 Then AOutput(i, 31) = .YdchPIR(productindex)        '**Y loss No PIR
        If Ytarg > 0 Then
         If Tppp > Tmin And PLStarg > 0 Then AOutput(i, 32) = .S_Qininstd(productindex, 2)         '**PP/Pls
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
         If Tepc > Tmin Then AOutput(i, 54) = .S_Qch(productindex, 10)                       '**EPC
        End If
SkipDonor:
    Next i
End With

' Write arrays to Details sheet
Sheets("Details").Visible = True
Sheets("Details").Select
CI = 63 'Column Offset
Set MyRange = Range(Cells(10, 9), Cells(y + 9, 9 + ResultNum - 1))
MyRange.value = AOutput
Set PQRange = Range(Cells(10, CI), Cells(y + 9, CI + productLimit - 1))
PQRange.value = PQArray

ActiveWindow.ScrollWorkbookTabs Position:=xlFirst
Sheets("Details").Visible = False
Application.ScreenUpdating = True
stopTime = Time
elapsedTime = Round((stopTime - startTime) * 24 * 60 * 60, 0)
If Cells(2, 17) = "ON" Then MsgBox "Elapsed time: " & elapsedTime & " sec"

InputChange_Multi = False
SetSimStatus

MDExit:
    Sheets("Details").Visible = False
    Sheets("Directory").Select
    Exit Sub
MDErr:
    MsgBox Err.Description
    Resume MDExit
End Sub

Public Sub BuildIterProcs()
On Error GoTo BIPErr
Dim cPlt As Single
Dim maxPlt As Single
Dim minPlt As Single
Dim stepPlt As Single
Dim cntPlt As Integer
Dim maxPls As Integer
Dim minPls As Integer
Dim stepPls As Integer
Dim cntPls As Integer
Dim maxRBC As Integer
Dim minRBC As Integer
Dim stepRBC As Integer
Dim cntRBC As Integer
Dim prPlt As Integer
Dim prPls As Integer
Dim prRBC As Integer
Dim itercnt As Integer
Dim ii As Integer
Dim jj As Integer
Dim kk As Integer
Dim Yt As Single
Dim dbs As Variant
Dim iii As Integer

' set variables
itercnt = 0
cPlt = Worksheets("ConfigLoop").Range("cPlt")
maxPlt = Worksheets("ConfigLoop").Range("maxPlt")
minPlt = Worksheets("ConfigLoop").Range("minPlt")
stepPlt = Worksheets("ConfigLoop").Range("stepPlt")
cntPlt = Worksheets("ConfigLoop").Range("cntPlt") 'Abs(maxPlt - minPlt) / stepPlt + 1
maxPls = Worksheets("ConfigLoop").Range("maxPls")
minPls = Worksheets("ConfigLoop").Range("minPls")
stepPls = Worksheets("ConfigLoop").Range("stepPls")
cntPls = Worksheets("ConfigLoop").Range("cntPls") 'Abs((maxPls - minPls) / stepPls) + 1
maxRBC = Worksheets("ConfigLoop").Range("maxRBC")
minRBC = Worksheets("ConfigLoop").Range("minRBC")
stepRBC = Worksheets("ConfigLoop").Range("stepRBC")
cntRBC = Worksheets("ConfigLoop").Range("cntRBC") 'Abs((maxRBC - minRBC) / stepRBC) + 1
prPlt = Worksheets("ConfigLoop").Range("prPlt")
prPls = Worksheets("ConfigLoop").Range("prPls")
prRBC = Worksheets("ConfigLoop").Range("prRBC")

' set variable loop limits (for variable priority)
'If prPlt = 1 Then A = cntPlt
'If prPls = 1 Then A = cntPls
'If prRBC = 1 Then A = cntRBC
'If prPlt = 2 Then B = cntPlt
'If prPls = 2 Then B = cntPls
'If prRBC = 2 Then B = cntRBC
'If prPlt = 3 Then C = cntPlt
'If prPls = 3 Then C = cntPls
'If prRBC = 3 Then C = cntRBC

' Apply platelet deadband ranges and build Ytarg list
Dim Ytarray() As Single
ReDim Ytarray(cntPlt)
dbs = Worksheets("ConfigLoop").Range("dbs")

ii = 0
Yt = dbs(3, 2)
Do While Yt > dbs(3, 1) + stepPlt - 0.1
    Ytarray(ii) = dbs(3, 2) - ii * stepPlt
    Yt = Ytarray(ii)
    Debug.Print ii, Ytarray(ii)
    ii = ii + 1
Loop
iii = 0
Do While Yt > dbs(2, 1) + stepPlt - 0.1
    Ytarray(ii) = dbs(2, 2) - iii * stepPlt
    Yt = Ytarray(ii)
    Debug.Print ii, Ytarray(ii)
    ii = ii + 1
    iii = iii + 1
Loop
iii = 0
Do While Yt > dbs(1, 1) + stepPlt - 0.1
    Ytarray(ii) = dbs(1, 2) - iii * stepPlt
    Yt = Ytarray(ii)
    Debug.Print ii, Ytarray(ii)
    ii = ii + 1
    iii = iii + 1
Loop
cntPlt = ii 'reset plt count after deadbands

' build proc array
ReDim iterA(4, cntPlt, cntPls, cntRBC) As Variant
ReDim iterProcs(Worksheets("ConfigLoop").Range("bIterCount"), 5) As Single
If prPls < prRBC Then
' Plasma>RBC priority
For ii = 0 To cntPlt
    For jj = 0 To cntPls - 1
        For kk = 0 To cntRBC - 1
            iterA(0, ii, jj, kk) = Ytarray(ii) 'Yt
            iterA(1, ii, jj, kk) = Ytarray(ii) / cPlt * 100000# 'PLT vol
            iterA(2, ii, jj, kk) = maxPls - jj * stepPls 'PLS
            iterA(3, ii, jj, kk) = maxRBC - kk * stepRBC 'RBC
            iterProcs(itercnt, 0) = itercnt
            iterProcs(itercnt, 1) = iterA(0, ii, jj, kk)
            iterProcs(itercnt, 2) = iterA(1, ii, jj, kk)
            iterProcs(itercnt, 3) = iterA(2, ii, jj, kk)
            iterProcs(itercnt, 4) = iterA(3, ii, jj, kk)
            itercnt = itercnt + 1
        Next kk
    Next jj
Next ii
ElseIf prPls > prRBC Then
' RBC>Plasma priority
For ii = 0 To cntPlt
    For jj = 0 To cntRBC - 1
        For kk = 0 To cntPls - 1
            iterA(0, ii, kk, jj) = Ytarray(ii) 'Yt
            iterA(1, ii, kk, jj) = Ytarray(ii) / cPlt * 100000# 'PLT vol
            iterA(2, ii, kk, jj) = maxPls - kk * stepPls 'PLS
            iterA(3, ii, kk, jj) = maxRBC - jj * stepRBC 'RBC
            iterProcs(itercnt, 0) = itercnt
            iterProcs(itercnt, 1) = iterA(0, ii, kk, jj)
            iterProcs(itercnt, 2) = iterA(1, ii, kk, jj)
            iterProcs(itercnt, 3) = iterA(2, ii, kk, jj)
            iterProcs(itercnt, 4) = iterA(3, ii, kk, jj)
            itercnt = itercnt + 1
        Next kk
    Next jj
Next ii
ElseIf prPls = 0 And prRBC = 0 Then
'Don't collect plasma or rbcs
For ii = 0 To cntPlt
    jj = 0
    kk = 0
            iterA(0, ii, kk, jj) = Ytarray(ii) 'Yt
            iterA(1, ii, kk, jj) = Ytarray(ii) / cPlt * 100000# 'PLT vol
            iterA(2, ii, kk, jj) = 0 'PLS
            iterA(3, ii, kk, jj) = 0 'RBC
            iterProcs(itercnt, 0) = itercnt
            iterProcs(itercnt, 1) = iterA(0, ii, kk, jj)
            iterProcs(itercnt, 2) = iterA(1, ii, kk, jj)
            iterProcs(itercnt, 3) = iterA(2, ii, kk, jj)
            iterProcs(itercnt, 4) = iterA(3, ii, kk, jj)
            itercnt = itercnt + 1
Next ii
End If

BIPExit:
    Sheets("Details").Visible = False
    Sheets("Directory").Select
    Exit Sub
BIPErr:
    MsgBox "Error generating priority list. " & Err.Description
    Resume BIPExit
End Sub

Public Sub ClearData(maxrow As Integer)
' Clears range of calculated data
    Sheets("Details").Visible = True
    Sheets("Details").Select
    Range("C10:CD" & maxrow & "").Select
    Selection.ClearContents
    Range("A1").Select
End Sub

