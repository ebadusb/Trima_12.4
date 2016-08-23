Files:
	Trima Accel Prediction Tool_6.3.xlms - this is the original file from Marlene renamed with 6.3 suffix
	C61.cs - class file exported from xlms file for 6.1 predict. This will be the template for 6.3
	Analyze.bas - exported from xlms file
	configs.bas - exported from xlms file
	Misc.bas - exported from xlms file
	SingleDonor.bas - exported from xlms file
	ThisWorkbook.cls - exported from xlms file

export process:
	open xlms file
	select developer
	select visual basic
	navigate to object browser
	select a file, right click, export
	
Deployment

	ThisWorkbook    
	netpath = "\\bctquad3\home\Excel Tool Chest\B Field Support Tools\Trima"
	
	Misc
	appPath = "\\bctquad3\home\Excel Tool Chest\B  Field Support Tools\Trima\"
	appPath = "\\bctquad3\home\Excel Tool Chest\B  Field Support Tools\Trima\"
	myIniFile = "\\bctquad3\home\Excel Tool Chest\A  Tool Index And Status\ETC_VersionTracker.ini"
	
	New
	netpath = "\\bctquad3\home\TempTrimaTools\"

Password
	Under Review | Unprotect Sheet
	UHURU

Dropdown list:
	To modify:
		Unprotect sheet
		under Data | Data Validation | Settings
			Change Source


Note:
	ConfigLoop must always be un-hidden, otherewise it will not work. 




11/13/2012 - Single Donor Flow:

Dependent on Sheets: 
	1) Configs & Procedures
		
	2) ConfigLoop
		Range: P6, Static Config

	3) DS Analysis
		Range BC64, No of configured procedures


		AOutput(7, 1 to 56) - initialize AOutput arrays of 7 by 56

	4) SD_Details
		Range T2, No of Donors
