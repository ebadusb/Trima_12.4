/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cds_test.cpp
 *             Common Data Store Unit Test Driver
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This test code exercizes the Data Store unit.
 *
 * HISTORY:    $Log: cds_test.cpp $
 * HISTORY:    Revision 1.12  2003/06/19 18:51:16Z  ms10234
 * HISTORY:    5829 - changes for pfr
 * HISTORY:    Revision 1.11  2003/04/08 14:19:30Z  ms10234
 * HISTORY:    5818 - Changed log levels to be one of the standard types
 * HISTORY:    Revision 1.10  2003/01/10 18:21:24Z  rm70006
 * HISTORY:    updated test code.
 * HISTORY:    Revision 1.9  2002/10/29 16:47:47Z  rm70006
 * HISTORY:    Add more tests.
 * HISTORY:    Revision 1.8  2002/10/21 17:42:02Z  rm70006
 * HISTORY:    Update unit tests.
 * HISTORY:    Revision 1.7  2002/09/25 16:07:27Z  rm70006
 * HISTORY:    Updated test
 * HISTORY:    Revision 1.6  2002/09/13 20:10:17Z  rm70006
 * HISTORY:    Updated unit test.
 * HISTORY:    Revision 1.5  2002/06/24 19:50:00Z  rm70006
 * HISTORY:    Finished unit test
 * HISTORY:    Revision 1.4  2002/06/17 18:46:39Z  rm70006
 * HISTORY:    Changed cds test class to match changes made to datastore class.
 * HISTORY:    Revision 1.3  2002/06/04 18:52:05Z  rm70006
 * HISTORY:    Changed to only have 1 get
 * HISTORY:    Revision 1.2  2002/06/04 17:19:52Z  rm70006
 * HISTORY:    Updated test code.
*******************************************************************************/


#include <iostream.h>


#include "semLib.h"

#include "cds.h"
#include "filenames.h"
#define PFSAVE_FILE "/machine/savedata/pfsave.dat"

SEM_ID readerSem;
SEM_ID writerSem;


void Prompt (char *msg)
{
   char ch;

   cout << msg << endl;
   cout << "?";
   cin >> ch;
   cout << endl;
}


const int spoof_int = 99;


const int & SpoofInt(const ElementType &e)
{
   cout << "\tSpoofIntRef Called" << endl;
   return spoof_int;
}




void DSTest()
{
   // Initialization
   readerSem = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE); 
   writerSem = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE); 
   taskPrioritySet(taskIdSelf(), 10);
   
   semTake(readerSem, -1);
   semTake(writerSem, -1);

   cout << "Data Store Tester" << endl;

   cout << "Start DSReader now" << endl;
   cout << "Start DSWriter now" << endl;

   cout << "Test 1." << endl;
   cout << "This test will create 2 CDS instances.  One RO and one RW." << endl;
   cout << "The RO instance will validate that the RW instance properly " << endl;
   cout << "initializes them." << endl << endl;

   Prompt("Press a key to begin the Test");

   // Start the DS Writer to create and initialize the CDS.
   semGive(writerSem);
   semTake(writerSem, -1);

   Prompt("Press a key to begin the Test");
   semGive(readerSem);
   semTake(readerSem, -1);

   cout << "Verify that the Reader passes the tests." << endl << endl;
   cout << "Test 1 Complete" << endl << endl;

   cout << "Test 2." << endl;
   cout << "This test will verify that set and get work on reader and writer" << endl << endl;
   Prompt("Press a key to begin the Test");

   // Unblock the writer 
   semGive(writerSem);
   semTake(writerSem, -1);

   Prompt("Press a key to continue the Test");

   // Unblock the reader
   semGive(readerSem);
   semTake(readerSem, -1);

   Prompt("Press a key to continue the Test (bounds set should fail)");
   
   // Unblock the writer 
   semGive(writerSem);
   semTake(writerSem, -1);

   cout << "Test 2 Complete" << endl << endl;

   cout << "Test 3." << endl;
   cout << "This test will verify that the semaphore locking works on the CDS" << endl << endl;
   Prompt("Press a key to begin the Test");

   // Unblock both reader and writer
   semGive(readerSem);
   semGive(writerSem);
   
   // Take the semaphores back 
   semTake(readerSem, -1);
   semTake(writerSem, -1);

   cout << "Test 3 Complete" << endl << endl;

   // PFR Save/Restore test
   cout << "Test 4." << endl;
   cout << "This test will verify that the PFR file can be written to and read back." << endl << endl;
   Prompt("Press a key to begin the Test");

   // Save the CDS
   ofstream saveFile;
   saveFile.open(PFSAVE_FILE);
   DataStore::SetAllPfData();
   saveFile.write( DataStore::PfMemoryBlock(), DataStore::PfCompleteLength() );
   saveFile.close();   

   Prompt("Press a key to continue the Test(overwrite cds)");
   
   // Unblock the writer 
   semGive(writerSem);
   semTake(writerSem, -1);

   Prompt("Press a key to continue the Test(test the restored values)");

   // Restore the CDS
   ifstream restoreFile;
   restoreFile.open(PFSAVE_FILE);
   restoreFile.read( DataStore::PfMemoryBlock(), DataStore::PfCompleteLength() );
   DataStore::RetrieveAllPfData();
   DataStore::RestoreAllPfData();
   restoreFile.close();

   // Unblock the reader
   semGive(readerSem);
   semTake(readerSem, -1);

   cout << "Test 4 Complete" << endl << endl;

   // Spoofer test
   cout << "Test 5." << endl;
   cout << "This test will verify that the Spoofer Role works correctly." << endl << endl;
   Prompt("Press a key to begin the Test");

   cout << "Reader should display original value" << endl;

   // Unblock the reader to display the original value.
   semGive(readerSem);
   semTake(readerSem, -1);

   CDS _spoofCDS(ROLE_SPOOFER);

   cout << "Spoofer created." << endl;
   cout << "Spoofer sees value " << _spoofCDS.BE1.Get() 
        << " (should be the orignal value)" << endl << endl;
   
   Prompt("Press a key to continue");

   //_spoofCDS.BE1.SetSpoof(SpoofInt);
   cout << "Spoofed BE1" << endl;

   // Unblock the reader 
   semGive(readerSem);
   semTake(readerSem, -1);

   cout << "Reader should display spoofed value of " << spoof_int << endl;
   cout << "Spoofer should display real value: " << _spoofCDS.BE1.Get() << endl << endl;

   Prompt("Press a key to continue");

   cout << "Unregistering Spoofer" << endl;
   _spoofCDS.BE1.ClearSpoof();

   cout << "Reader should display correct value again" << endl;

   // Unblock the reader 
   semGive(readerSem);
   semTake(readerSem, -1);

   Prompt("Press a key to continue");
   
   cout << "Deallocating Spoofer" << endl;

   Prompt("Press a key to continue");

   cout << "Reader should display same value" << endl;
   
   _spoofCDS.~CDS();

   // Unblock the reader 
   semGive(readerSem);
   semTake(readerSem, -1);

   cout << "Test 5 Complete" << endl << endl;

   // Multiple Writer Test
   cout << "Test 6." << endl;
   cout << "This test will verify that multiple writers are not allowed." << endl << endl;
   cout << "Start DSWriteInOut" << endl;
   cout << "Verify that DSWriteInOut task asserts a failure and exits" << endl;
   Prompt("Press a key to continue");
   
   cout << "Test 6 Complete" << endl << endl;

   // Unexpected termination test
   cout << "Test 7." << endl;
   cout << "This test will verify that an unexpected termination by one cds user doesn't affect others." << endl << endl;
   cout << "DSWriter will write a value and exit, the reader will verify that the CDS returns an error" << endl;
   Prompt("Press a key to begin the test");
   
   // Unblock the writer 
   semGive(writerSem);
   //semTake(writerSem, -1);

   cout << "Verify that the Writer has exited...." << endl;
   Prompt("Press a key to continue");

   cout << "Verify that the reader returns an error" << endl;   
   
   // Unblock the reader 
   semGive(readerSem);
   //semTake(readerSem, -1);

   cout << "Test 7 Complete" << endl << endl;

   // multi-threaded read/write test

   // End of Unit Test.
   cout << "Unit Test Complete" << endl;
   //semGive(writerSem);
   //semGive(readerSem);

   semDelete(writerSem);
   semDelete(readerSem);
}



void DSReader ()
{
   // Initialization
   taskPrioritySet(taskIdSelf(), 20);

   cout << "Data Store Reader" << endl << endl;

   cout << "Test 1." << endl;
   
   // Wait for test sync
   semTake(readerSem, -1);
   semGive(readerSem);

   //
   // Test 1
   //
   CDS _cds(ROLE_RO);

   //_cds.Lock();

   // All BE's (with simple types) should be initialized
   if ( (_cds.BE1.Get() != 1)    ||
        (_cds.BE2.Get() != 2.0f) ||
        (_cds.BE3.Get() != 3.0)  ||
        (_cds.BE4.Get() != THIS) )
   {
      cout << "\tInit Test Failed." << endl;
      cout << "\tBE1 is " << _cds.BE1.Get() << endl;
      cout << "\tBE2 is " << _cds.BE2.Get() << endl;
      cout << "\tBE3 is " << _cds.BE3.Get() << endl;
      cout << "\tBE4 is " << _cds.BE4.Get() << endl;
      
   }
   else
   {
      cout << "\tInit Test Passed for BaseElements." << endl;
   }

   // Check RE's against default values
   if ( (_cds.RE1.Get() != 8)   ||
        (_cds.RE2.Get() != 1.0) ||
        (_cds.RE3.Get() != 2.5) ||
        (_cds.RE4.Get() != THAT) )
   {
      cout << "\tInit Test Failed." << endl;
      cout << "\tRE1 is " << _cds.RE1.Get() << endl;
      cout << "\tRE2 is " << _cds.RE2.Get() << endl;
      cout << "\tRE3 is " << _cds.RE3.Get() << endl;
      cout << "\tRE4 is " << _cds.RE4.Get() << endl;
   }
   else
   {
      cout << "\tInit Test Passed for RangedElements." << endl;
   }

   //_cds.Unlock();

   cout << "\tTest 1 Complete." << endl << endl;

   //
   // Test 2
   //
   cout << "Test 2." << endl;

   // Wait for test sync
   semTake(readerSem, -1);
   semGive(readerSem);

   cout << "BaseElement Tests" << endl;

   // Verify that the BE's are all set correctly.   
   if ( (_cds.BE1.Get() != 2)   ||
        (_cds.BE2.Get() != 3.0) ||
        (_cds.BE3.Get() != 4.0) ||
        (_cds.BE4.Get() != THEOTHER) )
   {
      cout << "\tSet/Get Test Failed." << endl;
      cout << "\tBE1 is " << _cds.BE1.Get() << endl;
      cout << "\tBE2 is " << _cds.BE2.Get() << endl;
      cout << "\tBE3 is " << _cds.BE3.Get() << endl;
      cout << "\tBE4 is " << _cds.BE4.Get() << endl;
   }
   else
   {
      cout << "\tSet/Get Test Passed for BaseElements." << endl;
   }

   // Test Struct inits.
   CharArrayType charArray = _cds.BE5.Get();

   for (int i = 0; i < 80; i++)
   {
      if (charArray[i] != '>')
      {
         cout << "\tSet/Get Test Failed for String.  Value [" << i << "] is: "
              << charArray[i] << endl;
         cout << "Sting value is: " << charArray << endl;
         break;
      }
   }

   MessageType msg = _cds.BE6.Get();
 
   if ( (msg.x != 1) || (msg.y != 2) || (msg.z != 3) )
   {
      cout << "\tSet/Get Test Failed for MessageType.  Value is: "; 
      cout << msg << endl;
   }
   else
   {
      cout << "\tMessageType Test Passed" << endl;
   }
   
   MessageArrayType msgArray = _cds.BE7.Get();

   for (int i = 0; i < 50; i++)
   {
      if ( (msgArray[i].x != 1) || (msgArray[i].y != 2) || (msgArray[i].z != 3) )
      {
         cout << "\tSet/Get Test Failed for MessageType.  Value [" << i << "] is: "
              << msg << endl; 
      }
   }
   
   // Check RE's
   if ( (_cds.RE1.Get() != 5)    ||
        (_cds.RE2.Get() != 6.0f) ||
        (_cds.RE3.Get() != 3.14) ||
        (_cds.RE4.Get() != THIS) )
   {
      cout << "\tSet/Get Test Failed." << endl;
      cout << "\tRE1 is " << _cds.RE1.Get() << endl;
      cout << "\tRE2 is " << _cds.RE2.Get() << endl;
      cout << "\tRE3 is " << _cds.RE3.Get() << endl;
      cout << "\tRE4 is " << _cds.RE4.Get() << endl;
   }
   else
   {
      cout << "\tSet/Get Test Passed for RangedElements." << endl;
   }

   cout << "\tTest 2 Complete." << endl << endl;

   //
   // Test 3
   //
   cout << "Test 3." << endl;

   // Sync with the test exec.   
   semTake(readerSem, -1);
   semGive(readerSem);

   cout << "\tVerify that the values printed change as the writer updates" << endl;
   
   for (int i = 0; i < 20; i++)
   {
      //_cds.Lock();
      cout << "\tRE1 " << _cds.RE1.Get() << endl;
      //_cds.Unlock();
      taskDelay(3);
   }

   cout << "\tTest 3 Complete." << endl << endl;

   //
   // Test 4
   //
   cout << "Test 4." << endl;
   
   // Sync with the test exec.   
   semTake(readerSem, -1);
   semGive(readerSem);

   cout << "\tVerify that the CDS was restored" << endl;


   // Verify that the BE's are all set correctly.   
   if ( (_cds.BE1.Get() != 2)   ||
        (_cds.BE2.Get() != 3.0) ||
        (_cds.BE3.Get() != 4.0) ||
        (_cds.BE4.Get() != THEOTHER) )
   {
      cout << "\tSet/Get Test Failed for BaseElements." << endl;
      cout << "\tBE1 is " << _cds.BE1.Get() << endl;
      cout << "\tBE2 is " << _cds.BE2.Get() << endl;
      cout << "\tBE3 is " << _cds.BE3.Get() << endl;
      cout << "\tBE4 is " << _cds.BE4.Get() << endl;
   }
   else
   {
      cout << "\tSet/Get Test Passed for BaseElements." << endl;
   }

   // Test Struct inits.
   charArray = _cds.BE5.Get();

   for (int i = 0; i < 80; i++)
   {
      if (charArray[i] != '>')
      {
         cout << "\tSet/Get Test Failed for charArray.  Value [" << i << "] is: "
              << charArray[i] << endl;

         break;
      }
   }

   msg = _cds.BE6.Get();
 
   if ( (msg.x != 1) || (msg.y != 2) || (msg.z != 3) )
   {
      cout << "\tSet/Get Test Failed for MessageType.  Value is: "; 
      cout << msg << endl;
   }


   msgArray = _cds.BE7.Get();

   for (int i = 0; i < 50; i++)
   {
      if ( (msgArray[i].x != 1) || (msgArray[i].y != 2) || (msgArray[i].z != 3) )
      {
         cout << "\tSet/Get Test Failed for MessageType.  Value [" << i << "] is: "
              << msg << endl; 
      }
   }

   // Check RE's
   if ( (_cds.RE1.Get() != 24)   ||
        (_cds.RE2.Get() != 6.0f) ||
        (_cds.RE3.Get() != 3.0)  ||
        (_cds.RE4.Get() != THAT) )
   {
      cout << "\tSet/Get Test Failed for RangedElements." << endl;
      cout << "\tRE1 is " << _cds.RE1.Get() << endl;
      cout << "\tRE2 is " << _cds.RE2.Get() << endl;
      cout << "\tRE3 is " << _cds.RE3.Get() << endl;
      cout << "\tRE4 is " << _cds.RE4.Get() << endl;
   }
   else
   {
      cout << "\tSet/Get Test Passed for RangedElements." << endl;
   }

   cout << "\tTest 4 Complete." << endl << endl;

   //
   // Test 5
   //
   cout << "Test 5." << endl;
   
   //
   // Sync with the test exec.   
   semTake(readerSem, -1);
   semGive(readerSem);

   cout << "\tBE1 (original value) is " << _cds.BE1.Get() << endl << endl;

   //
   // Sync with the test exec.   
   semTake(readerSem, -1);
   semGive(readerSem);

   cout << "\tBE1 (spoofed value) is " << _cds.BE1.Get() << endl << endl;

   //
   // Sync with the test exec.   
   semTake(readerSem, -1);
   semGive(readerSem);

   cout << "\tBE1 (original value) is " << _cds.BE1.Get() << endl << endl;

   //
   // Sync with the test exec.   
   semTake(readerSem, -1);
   semGive(readerSem);

   cout << "\tBE1 (original value) is " << _cds.BE1.Get() << endl << endl;

   cout << "\tTest 5 Complete." << endl << endl;

   //
   // Test 7
   //
   cout << "Test 7." << endl;
   
   //
   // Sync with the test exec.   
   semTake(readerSem, -1);
   semGive(readerSem);

   cout << "\tBE1 value is: " << _cds.BE1.Get() << endl;

   cout << "\tTest 7 Complete." << endl << endl;

   cout << "Exiting...." << endl;

   cout << "Test Complete" << endl;
   
}





void DSWriter ()
{
   // Initialization
   taskPrioritySet(taskIdSelf(), 20);

   cout << "Data Store Writer" << endl << endl;

   cout << "Test 1." << endl;
   
   // Sync with the test exec.   
   semTake(writerSem, -1);
   semGive(writerSem);

   // Run Initialize Test
   // 1.  Create CDS instance with ROLE_RW
   // 2.  Wait for Read test to check init values.
   // 3.  End.
   CDS _cds(ROLE_RW);

   cout << "\tCDS created and initialized.  Run the Reader to verify the CDS" << endl;

   cout << "\tTest 1 Complete." << endl << endl;
   
   //
   // Test 2
   //
   cout << "Test 2" << endl;

   // Sync with the test exec.   
   semTake(writerSem, -1);
   semGive(writerSem);

   EnumType    Enum = THEOTHER;
   MessageType msg  = {1, 2.0f, 3.0};

   MessageArrayType msgArray;
   CharArrayType charArray;

   for (int i = 0; i < 80; i++)
   {
      charArray[i] = '>';
   }

   for (int i = 0; i < 50; i++)
   {
      msgArray[i] = msg;
   }

   //_cds.Lock();
   _cds.BE1.Set(2);
   _cds.BE2.Set(3.0f);
   _cds.BE3.Set(4.0f);
   _cds.BE4.Set(Enum);
   _cds.BE5.Set(charArray);
   _cds.BE6.Set(msg);
   _cds.BE7.Set(msgArray);
   cout << "\tBE Set" << endl;

   _cds.RE1.Set(5);
   _cds.RE2.Set(6.0f);
   _cds.RE3.Set(3.14);
   _cds.RE4.Set(THIS);
   cout << "\tRE Initialized" << endl;
   //_cds.Unlock();

   // Sync with the test exec.   
   semTake(writerSem, -1);
   semGive(writerSem);

   // Test Lower/Upper Bounds on Set for ranged element.
   if ( (_cds.RE1.Set(0)   != false) ||
        (_cds.RE1.Set(100) != false) ||
        (_cds.RE1.Get()    != 5)
      )
      cout << "\tRangeElement Bounds Check Test Failed!" << endl;
   else
      cout << "\tRangeElement Bounds Check Test Passed" << endl;

   cout << "\tTest 2 Complete" << endl << endl;

   //
   // Test 3
   //
   cout << "Test 3" << endl;
   
   // Sync with the test exec.   
   semTake(writerSem, -1);
   semGive(writerSem);
   
   for (int i = 5; i < 25; i++)
   {
      //_cds.Lock();
      _cds.RE1.Set(i);
      cout << "\tSetting RE1 to " << i << endl;
      //_cds.Unlock();
      taskDelay(2);
   }

   cout << "\tTest 3 Complete" << endl << endl;

   //
   // Test 4
   //
   cout << "Test 4" << endl;

   // Sync with the test exec.   
   semTake(writerSem, -1);
   semGive(writerSem);

   for (int i = 0; i < 80; i++)
   {
      charArray[i] = '<';
   }
   
   msg.x = 0;
   msg.y = 0;
   msg.z = 0;
   
   for (int i = 0; i < 50; i++)
   {
      msgArray[i] = msg;
   }

   // Overwrite current memory contents
   //_cds.Lock();
   _cds.BE1.Set(0);
   _cds.BE2.Set(0.0f);
   _cds.BE3.Set(0.0f);
   _cds.BE4.Set(THIS);
   _cds.BE5.Set(charArray);
   _cds.BE6.Set(msg);
   _cds.BE7.Set(msgArray);
   cout << "\tBE Set" << endl;

   _cds.RE1.Set(50);
   _cds.RE2.Set(200.0f);
   _cds.RE3.Set(3.00);
   _cds.RE4.Set(THAT);
   cout << "\tRE Set" << endl;
   //_cds.Unlock();

   cout << "\tTest 4 Complete" << endl << endl;

   //
   // Test 7
   //
   cout << "Test 7" << endl;

   // Sync with the test exec.   
   semTake(writerSem, -1);
   semGive(writerSem);

   //_cds.Lock();
   _cds.BE1.Set(20);
   
   cout << "\tBE1 Modified" << endl;
   cout << "\tTest 7 Complete" << endl << endl;
   
   cout << "\tExiting...." << endl;
   cout << "Test Complete" << endl;
}



void DSWriteInOut()
{
   cout << "DSWriteInOut" << endl;

   cout << "Creating CDS Writer" << endl;
   
   CDS _cds(ROLE_RW);
   
   cout << "Exiting ..." << endl;
}



void DSReadInOut()
{
   cout << "DSReadInOut" << endl;

   cout << "Creating CDS Reader" << endl;

   CDS _cds(ROLE_RO);

   cout << "Exiting ... " << endl;
}


class BDS : public SingleWriteDataStore
{
public:
   BaseElement<int> BE1;

   ~BDS() { } 

   BDS (Role role) :
      SingleWriteDataStore ("CDS", role)
   {
      BE1.Register (this, PFR_RECOVER, 1);
   }

private:
   BDS();
};



extern "C" void DSButzkeTest()
{
   cout << "Creating a MultiWrite CDS" << endl;
   CDS *cds = new CDS(ROLE_RW);

   cout << "Deleting a MultiWrite CDS" << endl;
   delete cds; // remove role

   cout << "Creating a CDS again" << endl;
   cds = new CDS(ROLE_RW);
   CDS *cds1 = new CDS(ROLE_RW);

   cout << "test passed" << endl;


   cout << "Creating a SingleWrite CDS" << endl;
   BDS *bds_ro1 = new BDS(ROLE_RO);
   BDS *bds_ro2 = new BDS(ROLE_RO);
   BDS *bds_rw1 = new BDS(ROLE_RW);
   BDS *bds_ro3 = new BDS(ROLE_RO);
   BDS *bds_ro4 = new BDS(ROLE_RO);


   cout << "Deleting a SingleWrite CDS" << endl;
   delete bds_rw1;
   delete bds_ro1;
   delete bds_ro2;

   cout << "Creating a SingleWrite CDS again" << endl;
   bds_rw1 = new BDS(ROLE_RW);
   bds_ro1 = new BDS(ROLE_RO);
   bds_ro2 = new BDS(ROLE_RO);

   cout << "test passed" << endl;
   
   cout << "Testing to make sure you can't have 2 writers declared.  Fatal error should be generated." << endl;

   BDS *bds1 = new BDS(ROLE_RW);
}



extern "C" void new_get_test()
{
   CDS _cds(ROLE_RW);

   int i;
   
   for (int j = 0; j < 10; j++)
   {
      //_cds.BE1.Get(&i);
      i = _cds.BE1;

      cout << "i is " << i << endl;

      //_cds.BE1.Set(++i);
      _cds.BE1 = ++i;
   }
}




class DWDS : public DynamicSingleWriteDataStore
{
public:
   BaseElement<int> BE1;

   ~DWDS() { } 

   DWDS (Role role) :
      DynamicSingleWriteDataStore ("DWDS", role)
   {
      BE1.Register (this, PFR_RECOVER, 1);
   }

private:
   DWDS();
};



extern "C" void dynamic_cds_test ()
{
   DWDS _cds1(ROLE_RO);

   // Make sure get works
   cout << "BE1 is " << _cds1.BE1 << endl;

   // Make sure set role works 
   _cds1.SetWrite();

   // Make sure set works
   _cds1.BE1 = 5;

   cout << "BE1 is " << _cds1.BE1 << endl;

   //_cds1.SetRead();

   // Check write controls.
   DWDS _cds2(ROLE_RO);

   _cds2.BE1 = 5;

   //_cds2.SetWrite();

   //_cds1.SetWrite();
}


class SmallCDS : public SingleWriteDataStore
{
public:
   BaseElement<int> BE1;

   ~SmallCDS() { } 

   SmallCDS (Role role) :
      SingleWriteDataStore ("SmallCDS", role)
   {
      BE1.Register (this, PFR_RECOVER, 1);
   }

private:
   SmallCDS();
};


class MediumCDS : public SingleWriteDataStore
{
public:
   BaseElement<int> BE1;
   BaseElement<int> BE2;
   BaseElement<int> BE3;
   BaseElement<int> BE4;
   BaseElement<int> BE5;
   BaseElement<int> BE6;
   BaseElement<int> BE7;
   BaseElement<int> BE8;
   BaseElement<int> BE9;
   BaseElement<int> BE10;

   ~MediumCDS() { } 

   MediumCDS (Role role) :
      SingleWriteDataStore ("MediumCDS", role)
   {
      BE1.Register (this, PFR_RECOVER, 1);
      BE2.Register (this, PFR_RECOVER, 1);
      BE3.Register (this, PFR_RECOVER, 1);
      BE4.Register (this, PFR_RECOVER, 1);
      BE5.Register (this, PFR_RECOVER, 1);
      BE6.Register (this, PFR_RECOVER, 1);
      BE7.Register (this, PFR_RECOVER, 1);
      BE8.Register (this, PFR_RECOVER, 1);
      BE9.Register (this, PFR_RECOVER, 1);
      BE10.Register (this, PFR_RECOVER, 1);
   }

private:
   MediumCDS();
};


class LargeCDS : public SingleWriteDataStore
{
public:
   BaseElement<int> BE1;
   BaseElement<int> BE2;
   BaseElement<int> BE3;
   BaseElement<int> BE4;
   BaseElement<int> BE5;
   BaseElement<int> BE6;
   BaseElement<int> BE7;
   BaseElement<int> BE8;
   BaseElement<int> BE9;
   BaseElement<int> BE10;

   BaseElement<int> CE1;
   BaseElement<int> CE2;
   BaseElement<int> CE3;
   BaseElement<int> CE4;
   BaseElement<int> CE5;
   BaseElement<int> CE6;
   BaseElement<int> CE7;
   BaseElement<int> CE8;
   BaseElement<int> CE9;
   BaseElement<int> CE10;

   BaseElement<int> DE1;
   BaseElement<int> DE2;
   BaseElement<int> DE3;
   BaseElement<int> DE4;
   BaseElement<int> DE5;
   BaseElement<int> DE6;
   BaseElement<int> DE7;
   BaseElement<int> DE8;
   BaseElement<int> DE9;
   BaseElement<int> DE10;

   BaseElement<int> EE1;
   BaseElement<int> EE2;
   BaseElement<int> EE3;
   BaseElement<int> EE4;
   BaseElement<int> EE5;
   BaseElement<int> EE6;
   BaseElement<int> EE7;
   BaseElement<int> EE8;
   BaseElement<int> EE9;
   BaseElement<int> EE10;

   BaseElement<int> FE1;
   BaseElement<int> FE2;
   BaseElement<int> FE3;
   BaseElement<int> FE4;
   BaseElement<int> FE5;
   BaseElement<int> FE6;
   BaseElement<int> FE7;
   BaseElement<int> FE8;
   BaseElement<int> FE9;
   BaseElement<int> FE10;

   BaseElement<int> GE1;
   BaseElement<int> GE2;
   BaseElement<int> GE3;
   BaseElement<int> GE4;
   BaseElement<int> GE5;
   BaseElement<int> GE6;
   BaseElement<int> GE7;
   BaseElement<int> GE8;
   BaseElement<int> GE9;
   BaseElement<int> GE10;

   BaseElement<int> HE1;
   BaseElement<int> HE2;
   BaseElement<int> HE3;
   BaseElement<int> HE4;
   BaseElement<int> HE5;
   BaseElement<int> HE6;
   BaseElement<int> HE7;
   BaseElement<int> HE8;
   BaseElement<int> HE9;
   BaseElement<int> HE10;

   BaseElement<int> IE1;
   BaseElement<int> IE2;
   BaseElement<int> IE3;
   BaseElement<int> IE4;
   BaseElement<int> IE5;
   BaseElement<int> IE6;
   BaseElement<int> IE7;
   BaseElement<int> IE8;
   BaseElement<int> IE9;
   BaseElement<int> IE10;

   BaseElement<int> JE1;
   BaseElement<int> JE2;
   BaseElement<int> JE3;
   BaseElement<int> JE4;
   BaseElement<int> JE5;
   BaseElement<int> JE6;
   BaseElement<int> JE7;
   BaseElement<int> JE8;
   BaseElement<int> JE9;
   BaseElement<int> JE10;

   BaseElement<int> KE1;
   BaseElement<int> KE2;
   BaseElement<int> KE3;
   BaseElement<int> KE4;
   BaseElement<int> KE5;
   BaseElement<int> KE6;
   BaseElement<int> KE7;
   BaseElement<int> KE8;
   BaseElement<int> KE9;
   BaseElement<int> KE10;

   ~LargeCDS() { } 

   LargeCDS (Role role) :
      SingleWriteDataStore ("LargeCDS", role)
   {
      BE1.Register (this, PFR_RECOVER, 1);
      BE2.Register (this, PFR_RECOVER, 1);
      BE3.Register (this, PFR_RECOVER, 1);
      BE4.Register (this, PFR_RECOVER, 1);
      BE5.Register (this, PFR_RECOVER, 1);
      BE6.Register (this, PFR_RECOVER, 1);
      BE7.Register (this, PFR_RECOVER, 1);
      BE8.Register (this, PFR_RECOVER, 1);
      BE9.Register (this, PFR_RECOVER, 1);
      BE10.Register (this, PFR_RECOVER, 1);
   
      CE1.Register (this, PFR_RECOVER, 1);
      CE2.Register (this, PFR_RECOVER, 1);
      CE3.Register (this, PFR_RECOVER, 1);
      CE4.Register (this, PFR_RECOVER, 1);
      CE5.Register (this, PFR_RECOVER, 1);
      CE6.Register (this, PFR_RECOVER, 1);
      CE7.Register (this, PFR_RECOVER, 1);
      CE8.Register (this, PFR_RECOVER, 1);
      CE9.Register (this, PFR_RECOVER, 1);
      CE10.Register (this, PFR_RECOVER, 1);
   
      DE1.Register (this, PFR_RECOVER, 1);
      DE2.Register (this, PFR_RECOVER, 1);
      DE3.Register (this, PFR_RECOVER, 1);
      DE4.Register (this, PFR_RECOVER, 1);
      DE5.Register (this, PFR_RECOVER, 1);
      DE6.Register (this, PFR_RECOVER, 1);
      DE7.Register (this, PFR_RECOVER, 1);
      DE8.Register (this, PFR_RECOVER, 1);
      DE9.Register (this, PFR_RECOVER, 1);
      DE10.Register (this, PFR_RECOVER, 1);
   
      EE1.Register (this, PFR_RECOVER, 1);
      EE2.Register (this, PFR_RECOVER, 1);
      EE3.Register (this, PFR_RECOVER, 1);
      EE4.Register (this, PFR_RECOVER, 1);
      EE5.Register (this, PFR_RECOVER, 1);
      EE6.Register (this, PFR_RECOVER, 1);
      EE7.Register (this, PFR_RECOVER, 1);
      EE8.Register (this, PFR_RECOVER, 1);
      EE9.Register (this, PFR_RECOVER, 1);
      EE10.Register (this, PFR_RECOVER, 1);
   
      FE1.Register (this, PFR_RECOVER, 1);
      FE2.Register (this, PFR_RECOVER, 1);
      FE3.Register (this, PFR_RECOVER, 1);
      FE4.Register (this, PFR_RECOVER, 1);
      FE5.Register (this, PFR_RECOVER, 1);
      FE6.Register (this, PFR_RECOVER, 1);
      FE7.Register (this, PFR_RECOVER, 1);
      FE8.Register (this, PFR_RECOVER, 1);
      FE9.Register (this, PFR_RECOVER, 1);
      FE10.Register (this, PFR_RECOVER, 1);
   
      GE1.Register (this, PFR_RECOVER, 1);
      GE2.Register (this, PFR_RECOVER, 1);
      GE3.Register (this, PFR_RECOVER, 1);
      GE4.Register (this, PFR_RECOVER, 1);
      GE5.Register (this, PFR_RECOVER, 1);
      GE6.Register (this, PFR_RECOVER, 1);
      GE7.Register (this, PFR_RECOVER, 1);
      GE8.Register (this, PFR_RECOVER, 1);
      GE9.Register (this, PFR_RECOVER, 1);
      GE10.Register (this, PFR_RECOVER, 1);
   
      HE1.Register (this, PFR_RECOVER, 1);
      HE2.Register (this, PFR_RECOVER, 1);
      HE3.Register (this, PFR_RECOVER, 1);
      HE4.Register (this, PFR_RECOVER, 1);
      HE5.Register (this, PFR_RECOVER, 1);
      HE6.Register (this, PFR_RECOVER, 1);
      HE7.Register (this, PFR_RECOVER, 1);
      HE8.Register (this, PFR_RECOVER, 1);
      HE9.Register (this, PFR_RECOVER, 1);
      HE10.Register (this, PFR_RECOVER, 1);
   
      IE1.Register (this, PFR_RECOVER, 1);
      IE2.Register (this, PFR_RECOVER, 1);
      IE3.Register (this, PFR_RECOVER, 1);
      IE4.Register (this, PFR_RECOVER, 1);
      IE5.Register (this, PFR_RECOVER, 1);
      IE6.Register (this, PFR_RECOVER, 1);
      IE7.Register (this, PFR_RECOVER, 1);
      IE8.Register (this, PFR_RECOVER, 1);
      IE9.Register (this, PFR_RECOVER, 1);
      IE10.Register (this, PFR_RECOVER, 1);
   
      JE1.Register (this, PFR_RECOVER, 1);
      JE2.Register (this, PFR_RECOVER, 1);
      JE3.Register (this, PFR_RECOVER, 1);
      JE4.Register (this, PFR_RECOVER, 1);
      JE5.Register (this, PFR_RECOVER, 1);
      JE6.Register (this, PFR_RECOVER, 1);
      JE7.Register (this, PFR_RECOVER, 1);
      JE8.Register (this, PFR_RECOVER, 1);
      JE9.Register (this, PFR_RECOVER, 1);
      JE10.Register (this, PFR_RECOVER, 1);
   
      KE1.Register (this, PFR_RECOVER, 1);
      KE2.Register (this, PFR_RECOVER, 1);
      KE3.Register (this, PFR_RECOVER, 1);
      KE4.Register (this, PFR_RECOVER, 1);
      KE5.Register (this, PFR_RECOVER, 1);
      KE6.Register (this, PFR_RECOVER, 1);
      KE7.Register (this, PFR_RECOVER, 1);
      KE8.Register (this, PFR_RECOVER, 1);
      KE9.Register (this, PFR_RECOVER, 1);
      KE10.Register (this, PFR_RECOVER, 1);
   
   }

private:
   LargeCDS();
};





extern "C" void mark_test()
{
   cout << "Small CDS" << endl;
   
   SmallCDS _smallCDS_1(ROLE_RO);
   SmallCDS _smallCDS_2(ROLE_RO);

   cout << "Medium CDS" << endl;

   MediumCDS _mediumCDS_1(ROLE_RO);
   MediumCDS _mediumCDS_2(ROLE_RO);
   
   cout << "Large CDS" << endl;

   LargeCDS _largeCDS_1(ROLE_RO);
   LargeCDS _largeCDS_2(ROLE_RO);
}
