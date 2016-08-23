/*******************************************************************************
 *
 * Copyright (c) 2011 CaridianBCT, Inc. All rights reserved.
 *
 * TITLE:      smokeTest.cpp
 *
 * AUTHOR:     Robert Watkins
 *
 ******************************************************************************/

// vxworks includes
#include <vxWorks.h>
#include <usrLib.h>
#include <taskLib.h>                // Uses to access the taskSpawn function
#include <routeLib.h>
#include <semLib.h>                 // Add Semaphores

// standard includes
#include <stdio.h>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>                  // Used to access stringstream

#include <trimamessages.h>			// Used to access the message system
#include <trima_tasks.h>			// Used to access trimaStartShutdown( ) and TTE_NextProcedure
#include <callback.h>				// Access to CallbackBase()::CallbaseBase( )
#include <message.h>
#include <messagesystem.h>			// The CaridianBCT Message system used in Trima

#include "screenTouch.hpp"          // Used to access the touch function
#include "delay.hpp"                // Used to access the waitForScreen function

#include <bitmap_info.h>            // #define statements for x and y coordinates for most of the gui buttons
#include <set_listbox.hpp>          // #define statement for __HORIZONTAL_MARGIN__
#include <setid.cpp>                // #define statements for BITMAP_BUTTON_USE_SET_SELECTION_X and BITMAP_BUTTON_USE_SET_SELECTION_Y

#include <datalog.h>                // Used to access the Datalog print statements
#include <datalog_levels.h>         // Used to access the "log_level_datalog_info" datalog level

#include <alarmEnums.h>             // Used to access GUI_Alarm_struct
#include <alarm.hpp>                // Used to access BITMAP_BUTTON_GENERIC_X and BITMAP_BUTTON_CONTINUE_Y

#include <spoofclient.hpp>          // Used to access the Spoof_Client class
//#include <procdata.h>             // Used to access the ProcData class
#include <procstate.h>              // Used to access the ProcState class

// For BloodPrimeReturn Substate
#include <procvolumes_cds.h>		// Used to access the ProcVolumes_CDS class
#include <datastore.h>				// Used to access ROLE_SPOOFER
#include <cobeconfig.h>				// Used to access the ProcState class

//#include <fastdata.h>				// Used to access FASTDATALENGTH


/*
// Robert Watkins: marked for death
extern "C" STATUS	shellInit(int, int);
extern "C" STATUS	shellParserControl(...); 
extern "C" STATUS 	telnetdParserSet(FUNCPTR pParserCtrlRtn);
extern "C" STATUS 	telnetdInit(int numClients, BOOL staticFlag);
extern "C" STATUS 	telnetdStart(int port);
*/

extern int shellTaskPriority;

#ifdef __cplusplus
extern "C" int spoof_server( const char* args );
#endif

SEM_ID semSmokeTest;  // THE SEMAPHORE

/*! \var Message<GUI_Alarm_struct> _GUIAlarmStructResponseMessage
 * The Message telling us that an alarm screen has popped up.
 */
Message<GUI_Alarm_struct> _GUIAlarmStructResponseMessage;

Message<ProcState> _ProcStateResponseMessage;


/*! \struct donorInfo
 * Holds the donor info that we read in from donorInfo.dat.
 */
struct donorInfo
{
	char gender;
	int heightFeet;
	int heightInches;
	int weight;
	int hematocrit;
	int plateletPrecount;
};

int acPrimeInletCounter;
int bloodPrimeReturnCounter;
int PrimeChannelCounter;
int PrimeAirout2Counter;

using namespace std;

/*! \fn string concatTwoStrings(string& str1, string& str2)
 * Concatenates two strings together.
 */
string concatTwoStrings(string& str1, string& str2);

/*! \fn void decrementNumberOfProceduresLeft()
 * Decrements 'Number of Procedures' by 1 in donorInfo.dat. This function assumes that 
 * 'Number of Procedures = %d; is the last line of donorInfo.dat. It also 
 * assumes that a ';' is at the end of the line.
 */
void decrementNumberOfProceduresLeft();

/*! \fn void deleteLastWordAndWhitespaceFromString(string& str)
 * Deletes the last word and any whitespace after the last word from str.
 * If str is the empty string than the str remains the empty string.
 */
void deleteLastWordAndWhitespaceFromString(string& str);

/*! \fn void handleApsAlarmMessage()
 * Recieves a GUI_Alarm_struct message. If the alarm was either APS_TOO_LOW or APS_TOO_HIGH_ALERT
 * then the continue button on the alarm screen is pushed.
 */
void handleApsAlarmMessage();

/*! \fn string itostring(int i)
 * Converts an int to a string
 */

void handleProcStateMessage();

string itostring(int i);

/*! \fn void messageHandler(void* messageName)
 * Recieves a message and prints out the name of the message recieved to the Datalog.
 */
void messageHandler(void* messageName);

/*! \fn void readDatFile(donorInfo& donorInfoStruct)
 * Reads in the donor info from donorInfo.dat into donorInfoStruct.
 * This function assumes that each line in donorInfo.dat is in the format:
 * %NAME% = %VALUE%;
 */
void readDatFile(donorInfo& donorInfoStruct);

/*! \fn string readEntireFileToString(ifstream& readEntireFileStream)
 * Reads an entire text file into one string.
 */
string readEntireFileToString(ifstream& readEntireFileStream);

/*! \fn int readNumberOfProceduresFromFile()
 * Reads the number of procedures from donorInfo.dat. This function assumes that 
 * 'Number of Procedures = %d; is the last line of donorInfo.dat. Also, it is 
 * assumed that a ';' is at the end of the line.
 */
int readNumberOfProceduresFromFile();

/*! \fn string removeSemicolonFromEndOfString(string stringWithSemicolon)
 * Removes the semicolon at the end of stringWithSemicolon.
 */
string removeSemicolonFromEndOfString(string stringWithSemicolon);

/*! \fn void smokeTest()
 * Reads in the donor info from donorInfo.dat and pushes the donor info buttons
 * on the screen. Next, the buttons up until the end of a procedure are pushed.
 * The process is repeated how ever many times was specified in donorInfo.dat.
 *
 * Robert Watkins: smokeTest() must be ran for a procedure with no plasma.
 * NOTE TO SELF: This is required because of the plasma screen that appears 
 * during postrun.
 */
void smokeTest();

/*! \fn int spoof_server( const char* args )
 * Initializes Telnet, a semaphore call semSmokeTest, the message system, and the individual messages. 
 * Also, a seperate task is spawned for smokeTest().
 */
int spoof_server( const char* args );

//void spoofAccessPressure();
//void spoofRedGreenValues();

/*! \fn int stringtoi(string str)
 * Converts a string type to an int.
 */
int stringtoi(string str);





string concatTwoStrings(string& str1, string& str2)
{
    string return_string;
    
    // Make a string of heightFeet and heightInches concatenated together
	return_string = str1;
	return_string += str2;
	
	return return_string;
}

int stringtoi(string str)
{
	return atoi(str.c_str());
}

string itostring(int i)
{
    string str;
    stringstream out;
    out << i;
    str = out.str();
    return str;
}

string readEntireFileToString(ifstream& readEntireFileStream)
{
    string returnString;
    
    while (!readEntireFileStream.eof())
    {
        returnString.push_back(readEntireFileStream.get());
    }

    return returnString;
}

void deleteLastWordAndWhitespaceFromString(string& str)
{   
    for (int i = str.length() - 1; i >= 0; i--)
    {
        if ( isspace(str[i]) )
        {
            str.erase(i, 1);
        }
        else
        {
            break;
        }
    }
    
    for (int i = str.length() - 1; i >= 0; i--)
    {
        if ( !isspace(str[i]) )
        {
            str.erase(i, 1);
        }
        else
        {
            break;
        }
    }
}

string removeSemicolonFromEndOfString(string stringWithSemicolon)
{
    stringWithSemicolon.erase(stringWithSemicolon.length() - 1, 1);

    return stringWithSemicolon;
}

void readDatFile(donorInfo& donorInfoStruct)
{
	string gender;
    string heightFeet;
    string heightInches;
    string weight;
	// Robert Watkins: CaridianBCT doesn't use blood type for testing.
    //string bloodType;
    string hematocrit;
    string plateletPrecount;

	string newWord;
	
	ifstream myFile;  
    myFile.open("/machine/Tools/donorInfo.dat");
	if (myFile.fail())
	{
		DataLog (log_level_datalog_info) << "Input file opening failed" << endmsg;
		return;
	}
	

    myFile >> newWord;
    myFile >> newWord;
    myFile >> newWord; 
    gender = newWord;
	gender = removeSemicolonFromEndOfString(gender);
	donorInfoStruct.gender = gender[0];
	
    myFile >> newWord;
    myFile >> newWord;
    myFile >> newWord;  
    myFile >> newWord; 
    heightFeet = newWord;
	heightFeet = removeSemicolonFromEndOfString(heightFeet);
	donorInfoStruct.heightFeet = stringtoi(heightFeet);
	
    myFile >> newWord;
    myFile >> newWord;
    myFile >> newWord; 
    myFile >> newWord;  
    heightInches = newWord;
	heightInches = removeSemicolonFromEndOfString(heightInches);
	donorInfoStruct.heightInches = stringtoi(heightInches);

    myFile >> newWord;
    myFile >> newWord;
    myFile >> newWord;    
    weight = newWord;
	weight = removeSemicolonFromEndOfString(weight);
	donorInfoStruct.weight = stringtoi(weight);
    
    myFile >> newWord;
    myFile >> newWord;
    myFile >> newWord;  
    myFile >> newWord; 
	// Robert Watkins: CaridianBCT doesn't use blood type for testing.
    //bloodType = newWord;
	//bloodType = removeSemicolonFromEndOfString(bloodType);
   
    myFile >> newWord;
    myFile >> newWord;
    myFile >> newWord; 
    hematocrit = newWord;
	hematocrit = removeSemicolonFromEndOfString(hematocrit);
	donorInfoStruct.hematocrit = stringtoi(hematocrit);
    
    myFile >> newWord;
    myFile >> newWord;
    myFile >> newWord;
    myFile >> newWord; 
    plateletPrecount = newWord;
	plateletPrecount = removeSemicolonFromEndOfString(plateletPrecount);
	donorInfoStruct.plateletPrecount = stringtoi(plateletPrecount);


    myFile.close();
}

void decrementNumberOfProceduresLeft()
{
	int counterMinusOne = 0;
	string newWord;
	string decrementedWord;
	string entireFile;
    ifstream myFileInput;
    ifstream readEntireFileStream;
    ofstream myFileOutput;



    myFileInput.open("/machine/Tools/donorInfo.dat");
	if (myFileInput.fail())
	{
		DataLog (log_level_datalog_info) << "Input file opening failed" << endmsg;
		return;
	}

	while (myFileInput.peek() != EOF)
	{
		myFileInput >> newWord;
	}

    myFileInput.close();

	newWord = removeSemicolonFromEndOfString(newWord);
	counterMinusOne = stringtoi(newWord) - 1;
	decrementedWord = itostring(counterMinusOne);
    
    
    readEntireFileStream.open("/machine/Tools/donorInfo.dat");
	if (readEntireFileStream.fail())
	{
		DataLog (log_level_datalog_info) << "Input file opening failed" << endmsg;
		return;
	}

    // Put the entire file into 1 string
    entireFile = readEntireFileToString(readEntireFileStream);

    readEntireFileStream.close();

    entireFile.erase(entireFile.length() - 1, 1); // Get rid of the EOF at the end    
    deleteLastWordAndWhitespaceFromString(entireFile);
    decrementedWord += ";";
    entireFile += decrementedWord;
    
    
    myFileOutput.open("/machine/Tools/donorInfo.dat");
    if (myFileOutput.fail())
	{
		DataLog (log_level_datalog_info) << "Input file opening failed" << endmsg;
		return;
	}

    myFileOutput << entireFile;
    
    myFileOutput.close();
}

int readNumberOfProceduresFromFile()
{
	 string newWord;
	 int numberOfProcedures = 0;

	 ifstream myFileInput;
	 myFileInput.open("/machine/Tools/donorInfo.dat");
	 if (myFileInput.fail())
	 {
	     DataLog (log_level_datalog_info) << "Input file opening failed" << endmsg;
		 return -1;
	 }


	 while (myFileInput.peek() != EOF)
	 {
		 myFileInput >> newWord;
	 }

	 newWord = removeSemicolonFromEndOfString(newWord);
	 numberOfProcedures = stringtoi(newWord);

	 myFileInput.close();
	 return numberOfProcedures;
}

void smokeTest()
{	 
	donorInfo donorInfoStruct;
	string height;
	int numberOfProcedures = 0;
	acPrimeInletCounter = 0;
	bloodPrimeReturnCounter = 0;
	PrimeChannelCounter = 0;
	PrimeAirout2Counter = 0;


	SpoofClient my_client(1); // Initialize a Spoofer Client
	//ProcData pd;
	ProcVolumes_CDS procVolume(ROLE_SPOOFER);


	// Hold here waiting for the boot up to finish
	//semTake(semSmokeTest, WAIT_FOREVER);

	// Get donor info from the .dat file
	readDatFile(donorInfoStruct);	

	// Get the number of procedures left from the .dat file
	numberOfProcedures = readNumberOfProceduresFromFile();

	// This outer if statement is to take care of the case of starting with
	// Number of Procedures = 0 the first time you ftp the .dat file to a Trima.
	if (numberOfProcedures > 0)
	{
		// Start Pushing the GUI buttons
		Delay::wrapWaitForScreen(GUI_SCREEN_SYSDONOR);
		ScreenTouch::wrapTouch(BUTNINFO_X + 5, BUTNINFO_Y + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_DONVITAL);
		if (donorInfoStruct.gender == 'm')
		{
			ScreenTouch::wrapTouch(BUTN_DONOR_GENDER_X + 5, BUTN_DONOR_GENDER_Y + 5);
		}
		else if (donorInfoStruct.gender == 'f')
		{
			ScreenTouch::wrapTouch(BUTN_DONOR_GENDER_X + 5, BUTN_DONOR_GENDER_Y + 5);
			ScreenTouch::wrapTouch(BUTN_DONOR_GENDER_X + 5, BUTN_DONOR_GENDER_Y + 5);
		}
		ScreenTouch::wrapTouch(BUTN_DONOR_HEIGHT_X + 5, BUTN_DONOR_HEIGHT_Y + 5);
	
		Delay::wrapWaitForScreen(GUI_SCREEN_KEYPAD);
		height = itostring(donorInfoStruct.heightFeet);
		height += itostring(donorInfoStruct.heightInches);
		ScreenTouch::wrapKeypadSequence( height.append("E"), GUI_BUTTON_DONOR_HEIGHT);

		Delay::wrapWaitForScreen(GUI_SCREEN_DONVITAL);
		ScreenTouch::wrapTouch(BUTN_DONOR_WEIGHT_X + 5, BUTN_DONOR_WEIGHT_Y + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_KEYPAD);
		ScreenTouch::wrapKeypadSequence(itostring(donorInfoStruct.weight).append("E"), GUI_BUTTON_DONOR_WEIGHT);

		Delay::wrapWaitForScreen(GUI_SCREEN_DONVITAL);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);
		
		// Robert Watkins: CaridianBCT doesn't use blood type for testing.
	/*
		Delay::wrapWaitForScreen(GUI_SCREEN_DONINFO);
		ScreenTouch::wrapTouch(BUTN_DONOR_BLOOD_TYPE_X + 5, BUTN_DONOR_BLOOD_TYPE_Y + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_KEYPAD);
		ScreenTouch::wrapKeypadSequence(bloodType.c_str(), GUI_BUTTON_DONOR_BLOOD_TYPE);
	*/
		Delay::wrapWaitForScreen(GUI_SCREEN_DONINFO);
		ScreenTouch::wrapTouch(BUTN_DONOR_HEMATOCRIT_X + 5, BUTN_DONOR_HEMATOCRIT_Y + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_KEYPAD);
		ScreenTouch::wrapKeypadSequence(itostring(donorInfoStruct.hematocrit).append("E"), GUI_BUTTON_DONOR_HEMATOCRIT);

		Delay::wrapWaitForScreen(GUI_SCREEN_DONINFO);
		ScreenTouch::wrapTouch(BUTN_DONOR_PLATELET_PRECOUNT_X + 5, BUTN_DONOR_PLATELET_PRECOUNT_Y + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_KEYPAD);
		ScreenTouch::wrapKeypadSequence(itostring(donorInfoStruct.plateletPrecount).append("E"), GUI_BUTTON_DONOR_PLATELET_PRECOUNT);

		Delay::wrapWaitForScreen(GUI_SCREEN_DONINFO);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_PREDICT);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);


		// After two-button screen with a checkmark (Push Load System next).
		Delay::wrapWaitForScreen(GUI_SCREEN_SYSDONOR);
		ScreenTouch::wrapTouch(BUTNSETUP_X + 5, BUTNSETUP_Y + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_SYSCASSETTE);
		ScreenTouch::wrapTouch(380 + 5, 170 + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_SET_LISTBOX);
		ScreenTouch::wrapTouch(380 + 45 + 5, 170 + 50);

		Delay::wrapWaitForScreen(GUI_SCREEN_SYSCASSETTE);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);
	
		Delay::wrapWaitForScreen(GUI_SCREEN_SYSDISP);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);




		// Start hooking stuff up
		Delay::wrapWaitForScreen(GUI_SCREEN_SYSDISP);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);
	
		
	    my_client.spoofData("CHwStates", "red", "1000");
		my_client.spoofData("CHwStates", "green", "1000");
        my_client.spoofActivate();
		
		// Hold here waiting for the air to finish being put in
		semTake(semSmokeTest, WAIT_FOREVER);

		//cout << "pd.Status().APS(): " << pd.Status().APS() << endl;

		Delay::seconds(15);
		Delay::wrapWaitForScreen(GUI_SCREEN_SYSCLAMP);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);
		
		// Hold here waiting for the CheckSampleBag substate
		semTake(semSmokeTest, WAIT_FOREVER);
		my_client.spoofData( "CHwStates", "accessPressure", 1, 3 * 100 );
		//cout << "pd.Status().APS(): " << pd.Status().APS() << endl;

		// Hold here waiting for the PressInletLine substate
		semTake(semSmokeTest, WAIT_FOREVER);
		my_client.spoofData( "CHwStates", "accessPressure", 1, 3 * 650 );

		// Hold here waiting for the NegativePressTest substate
		semTake(semSmokeTest, WAIT_FOREVER);
		my_client.spoofData( "CHwStates", "accessPressure", 1, 3 * -450 );

		// Hold here waiting for the Neg2NegPressRelief substate
		semTake(semSmokeTest, WAIT_FOREVER);
		my_client.spoofData( "CHwStates", "accessPressure", 1, 3 * -20 );

		// Hold here waiting for the Pos2NegPressRelief substate
		semTake(semSmokeTest, WAIT_FOREVER);
		my_client.spoofData( "CHwStates", "accessPressure", 1, 3 * -100 );

		// Hold here waiting for the Test to complete
		semTake(semSmokeTest, WAIT_FOREVER);

		//my_client.spoofEnd();


		Delay::seconds(20);
		Delay::wrapWaitForScreen(GUI_SCREEN_SYSACATT);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);
	

		// Hold here waiting for the ACPrimeInlet substate
		semTake(semSmokeTest, WAIT_FOREVER);
		my_client.spoofData( "CHwStates", "acDetector", "CHW_AC_DETECTOR_FLUID" );


		// Hold here waiting for the AC prime to complete (ACPrimeCompletedMsg)
		semTake(semSmokeTest, WAIT_FOREVER);

		Delay::wrapWaitForScreen(GUI_SCREEN_SYSDONOR);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_CONFIRM_DON_INFO);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);

		Delay::seconds(15);
		Delay::wrapWaitForScreen(GUI_SCREEN_DONCONN);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);
	

		// Hold here waiting for the BloodPrimeReturn substate
		semTake(semSmokeTest, WAIT_FOREVER);
		while (procVolume.Vreturn.Get() >= CobeConfig::data().LowLevelTooSoonLimit - 5.0f) 
		{
			taskDelay(10);    // This is need because with no taskDelay the smokeTest task hogs the processor
			                  // and we get a watchdog error in the dlog (Trima machine crashes and reboots as well).
		}
		my_client.spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_MIDDLE" );

		// Hold here waiting for the PrimeChannel substate
		semTake(semSmokeTest, WAIT_FOREVER);
		my_client.spoofData( "RW_FastData", "raw", "1000" ); // NOTE: Because of the code added in engr_tools/spoofer/rwFastData.hpp
															 // the spoofData() here does not care about the 2nd ("raw") and 3rd "1000" arguments.
		//my_client.spoofData( "CHwStates", "accessPressure", 1, 3 * 10 );


		while ( procVolume.VinTotal.Get() - procVolume.Vreturn.Get() <= CobeConfig::data().PltPlsRBCFirstCycleTooSoon)
		{
			taskDelay(10);    // This is need because with no taskDelay the smokeTest task hogs the processor
			                  // and we get a watchdog error in the dlog (Trima machine crashes and reboots as well).
		}
		my_client.spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_HIGH" );
		//my_client.spoofData( "CHwStates", "event", "CHW_RESERVOIR_EVENT" );
		//my_client.spoofData( "SHwStates", "returnPumpDirChgTime", "0" );

		// Hold here waiting for the PrimeAirout2 substate
		semTake(semSmokeTest, WAIT_FOREVER);
		//my_client.spoofData( "SHwStates", "returnPumpDirChgTime", "0" );
		//my_client.spoofData( "SHwStates", "event", "SHW_NORMAL_UPDATE" );

		// Hold here waiting for the run and blood rinseback to complete
		semTake(semSmokeTest, WAIT_FOREVER);

		my_client.spoofEnd();

		Delay::seconds(5);
		Delay::wrapWaitForScreen(GUI_SCREEN_RUNPROC);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);

		Delay::seconds(30);
		Delay::wrapWaitForScreen(GUI_SCREEN_DISCONN);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_DISCONN);
		ScreenTouch::wrapTouch(BUTNCONF_X + 5, BUTNCONF_Y + 5);

		Delay::seconds(15);
		Delay::wrapWaitForScreen(GUI_SCREEN_SEAL_AND_DETACH);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);

		// Hold here waiting for the cassette to raise
		semTake(semSmokeTest, WAIT_FOREVER);

		Delay::seconds(15);
		Delay::wrapWaitForScreen(GUI_SCREEN_SEAL_AND_DETACH);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_RUNSUMM1);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_RUNSUM_PLT);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);

		// Robert Watkins:  This screen only appears in a run with plasma.
		// In a run without plasma this screen does not appear.
		//Delay::wrapWaitForScreen(GUI_SCREEN_RUNSUM_PLASMA);
		//ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);

		Delay::wrapWaitForScreen(GUI_SCREEN_RUNSUMM2);
		ScreenTouch::wrapTouch(BUTNCONT_X + 5, BUTNCONT_Y + 5);



		// Decrement the number of procedures variable in the .dat file
		decrementNumberOfProceduresLeft();

		// Get the number of procedures left from the .dat file
		numberOfProcedures = readNumberOfProceduresFromFile();
		DataLog (log_level_datalog_info) << "Number of procedures left: " << numberOfProcedures << endmsg;

		if (numberOfProcedures > 0)
		{
			// Button for confirm another procedure or to cancel
			Delay::wrapWaitForScreen(GUI_SCREEN_REBOOT_CONFIRM_BOX);
			ScreenTouch::wrapTouch(180 + 23 + 5, 167 + 128 + 5);
		}
	}
}

void messageHandler(void* messageName)
{
	semGive(semSmokeTest);
	DataLog (log_level_datalog_info) << (char*) messageName << " " << "Message Recieved" << endmsg;
}

void handleApsAlarmMessage()
{
	//DataLog (log_level_spoofer_debug) << "ALARM NAME: " << _GUIAlarmStructResponseMessage.getData().alarm_name << endmsg;
	if ((_GUIAlarmStructResponseMessage.getData().alarm_name == APS_TOO_LOW) || (_GUIAlarmStructResponseMessage.getData().alarm_name == APS_TOO_HIGH_ALERT) )
	{
		DataLog (log_level_datalog_info) << "GUI_Alarm_struct Message Recieved" << endmsg;

		// Touch the continue button on the APS Alarm screen
		// BITMAP_BUTTON_GENERIC_X and BITMAP_BUTTON_CONTINUE_Y are defined in alarm.hpp
		Delay::wrapWaitForScreen(GUI_SCREEN_ALARM);
		ScreenTouch::wrapTouch(BITMAP_BUTTON_GENERIC_X + 5, BITMAP_BUTTON_CONTINUE_Y + 5);
	}
}

void handleProcStateMessage()
{
	DataLog (log_level_datalog_info) << "ProcState Message Recieved: Entering " << _ProcStateResponseMessage.getData().stateName() << " substate." << endmsg;

	if ( strcmp(_ProcStateResponseMessage.getData().stateName(), "CheckSampleBag") == 0)
	{
		DataLog (log_level_datalog_info) << "Inside CheckSampleBag" << endmsg;
		semGive(semSmokeTest);	
	}
	else if ( strcmp(_ProcStateResponseMessage.getData().stateName(), "PressInletLine") == 0)
	{
		DataLog (log_level_datalog_info) << "Inside PressInletLine" << endmsg;
		semGive(semSmokeTest);	
	}
	else if ( strcmp(_ProcStateResponseMessage.getData().stateName(), "NegativePressTest") == 0)
	{
		DataLog (log_level_datalog_info) << "Inside NegativePressTest" << endmsg;
		semGive(semSmokeTest);	
	}
	else if ( strcmp(_ProcStateResponseMessage.getData().stateName(), "Neg2NegPressRelief") == 0)
	{
		DataLog (log_level_datalog_info) << "Inside Neg2NegPressRelief" << endmsg;
		semGive(semSmokeTest);	
	}
	else if ( strcmp(_ProcStateResponseMessage.getData().stateName(), "Pos2NegPressRelief") == 0)
	{
		DataLog (log_level_datalog_info) << "Inside Pos2NegPressRelief" << endmsg;
		semGive(semSmokeTest);	
	}
	else if ( strcmp(_ProcStateResponseMessage.getData().stateName(), "ACPrimeInlet") == 0 && acPrimeInletCounter < 1)
	{
		acPrimeInletCounter++;

		DataLog (log_level_datalog_info) << "Inside ACPrimeInlet" << endmsg;
		semGive(semSmokeTest);	
	}
	else if ( strcmp(_ProcStateResponseMessage.getData().stateName(), "BloodPrimeReturn") == 0 && bloodPrimeReturnCounter < 1)
	{
		bloodPrimeReturnCounter++;

		DataLog (log_level_datalog_info) << "Inside BloodPrimeReturn" << endmsg;
		semGive(semSmokeTest);	
	}
	else if ( strcmp(_ProcStateResponseMessage.getData().stateName(), "PrimeChannel") == 0 && PrimeChannelCounter < 1)
	{
		PrimeChannelCounter++;

		DataLog (log_level_datalog_info) << "Inside PrimeChannel" << endmsg;
		semGive(semSmokeTest);	
	}
	else if ( strcmp(_ProcStateResponseMessage.getData().stateName(), "PrimeAirout2") == 0 && PrimeAirout2Counter < 1)
	{
		PrimeAirout2Counter++;

		DataLog (log_level_datalog_info) << "Inside PrimeAirout2" << endmsg;
		semGive(semSmokeTest);	
	}
}
/*
void handleProcStateMessage()
{
	int taskIdTwo;
	
	DataLog (log_level_datalog_info) << "ProcState Message Recieved: Entering " << _ProcStateResponseMessage.getData().stateName() << " substate." << endmsg;

	if ( strcmp(_ProcStateResponseMessage.getData().stateName(), "LowerCassette") == 0)
	{
		DataLog (log_level_datalog_info) << "Inside LowerCassette" << endmsg;

		// Spawn new task and go to smokeTest() function
		taskIdTwo = taskSpawn("spoofRedGreenValues", 30, VX_FP_TASK, 30000, (FUNCPTR)spoofRedGreenValues, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	}
	else if ( strcmp(_ProcStateResponseMessage.getData().stateName(), "CheckSampleBag") == 0)
	{
		DataLog (log_level_datalog_info) << "Inside CheckSampleBag" << endmsg;

		// Spawn new task and go to smokeTest() function
		taskIdTwo = taskSpawn("spoofAccessPressure", 30, VX_FP_TASK, 30000, (FUNCPTR)spoofAccessPressure, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	}
}

void spoofRedGreenValues()
{
	DataLog (log_level_datalog_info) << "spoofRedGreenValues: 1" << endmsg;
	SpoofClient my_client(1); // Initialize a Spoofer Client

	DataLog (log_level_datalog_info) << "spoofRedGreenValues: 2" << endmsg;
	my_client.spoofData("CHwStates", "red", "1000");
	DataLog (log_level_datalog_info) << "spoofRedGreenValues: 3" << endmsg;
	my_client.spoofData("CHwStates", "green", "1000");
	DataLog (log_level_datalog_info) << "spoofRedGreenValues: 4" << endmsg;
	my_client.spoofActivate();
	DataLog (log_level_datalog_info) << "spoofRedGreenValues: 5" << endmsg;
}

void spoofAccessPressure()
{
	DataLog (log_level_datalog_info) << "spoofAccessPressure: 1" << endmsg;
	SpoofClient my_client(1); // Initialize a Spoofer Client

	DataLog (log_level_datalog_info) << "spoofAccessPressure: 2" << endmsg;
	my_client.spoofData("CHwStates", "accessPressure", 1, 100 * 3);
	DataLog (log_level_datalog_info) << "spoofAccessPressure: 3" << endmsg;
}
*/
int rw_init( const char* args )
{
	int taskIdOne;
	/*
	// Robert Watkins: marked for death
	if ((taskNameToId ("tShell")) == ERROR)	// Shell not started yet.
	{
	    shellTaskPriority = 100;
	    shellInit(60000, TRUE);
	}

	telnetdParserSet(shellParserControl);	
	telnetdInit(1, 1);	
	telnetdStart(23);
	// Robert Watkins: marked for death
	*/

	log_level_spoofer_debug.logOutput(DataLog_LogEnabled);

	DataLog (log_level_datalog_info) << "spoof_main( " << args << " )" << endmsg;

	// Create the Semaphore
	semSmokeTest = semBCreate(SEM_Q_FIFO, SEM_EMPTY); // Semaphore is already taken at this point (semSmokeTest = 0)

	
	//
	// Set up the messages
	//

	// Initialize message system
	MessageSystem _MessageSystem;
	_MessageSystem.initBlocking( "Initializing message system", 30 );
	
	/*
	ConfirmEnableLoadSystemMsg _ConfirmEnableLoadSystemResponseMessage;

	// Wait for the boot up to finish
	_ConfirmEnableLoadSystemResponseMessage.init( 
		CallbackBase( (CallbackBase::FncPtrData) &messageHandler, (void *) "ConfirmEnableLoadSystemMsg" ),
		MessageBase::SNDRCV_RECEIVE_ONLY
		);
	*/	

	// Wait to hit continue until the air has finished being put in
	DisposableLoweredMsg _DisposableLoweredResponseMessage;

	_DisposableLoweredResponseMessage.init( 
		CallbackBase( (CallbackBase::FncPtrData) &messageHandler, (void *) "DisposableLoweredMsg" ),
		MessageBase::SNDRCV_RECEIVE_ONLY
		);

	// Wait to hit continue until the Test has been completed
	TestCompletedMsg _TestCompletedResponseMessage;

	_TestCompletedResponseMessage.init( 
		CallbackBase( (CallbackBase::FncPtrData) &messageHandler, (void *) "TestCompletedMsg" ),
		MessageBase::SNDRCV_RECEIVE_ONLY
		);

	// Wait to hit continue until the AC prime has completed
	ACPrimeCompletedMsg _ACPrimeCompletedResponseMessage;

	_ACPrimeCompletedResponseMessage.init( 
		CallbackBase( (CallbackBase::FncPtrData) &messageHandler, (void *) "ACPrimeCompletedMsg" ),
		MessageBase::SNDRCV_RECEIVE_ONLY
		);

	// Wait to hit continue until the run is finished and blood rinseback has been completed
	RunCompleteMsg _RunCompleteResponseMessage;

	_RunCompleteResponseMessage.init( 
		CallbackBase( (CallbackBase::FncPtrData) &messageHandler, (void *) "RunCompleteMsg" ),
		MessageBase::SNDRCV_RECEIVE_ONLY
		);

	TellProcToTransitionFromDisconnect _TellProcToTransitionFromDisconnectResponseMessage;

	_TellProcToTransitionFromDisconnectResponseMessage.init( 
		CallbackBase( (CallbackBase::FncPtrData) &messageHandler, (void *) "TellProcToTransitionFromDisconnect" ),
		MessageBase::SNDRCV_RECEIVE_ONLY
		);
	
	// Initialize the APS Alarm message
	_GUIAlarmStructResponseMessage.init( 
		CallbackBase( (CallbackBase::FncPtrVoid) &handleApsAlarmMessage ),
		MessageBase::SNDRCV_RECEIVE_ONLY
		);
	
	_ProcStateResponseMessage.init( 
		CallbackBase( (CallbackBase::FncPtrVoid) &handleProcStateMessage ),
		MessageBase::SNDRCV_RECEIVE_ONLY
		);


	// Spawn new task and go to smokeTest() function
	taskIdOne = taskSpawn("smokeTest", 30, VX_FP_TASK, 30000, (FUNCPTR)smokeTest, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	// Initialize screen touch
	ScreenTouch::wrapInit();

	_MessageSystem.dispatchMessages( );
	
	DataLog (log_level_datalog_info) << "normal exit" << endmsg;
    return 0;
}

