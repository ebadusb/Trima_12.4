#include "datastore.h"
#include "filenames.h"
#define PFSAVE_FILE "/machine/savedata/pfsave.dat"


class DS_1 : public SingleWriteDataStore
{
public:
   BaseElement<int> BE1;
   BaseElement<int> BE2;
   BaseElement<int> BE3;

   ~DS_1() { } 

   DS_1 (Role role) :
      SingleWriteDataStore ("DS_1", role)
   {
      BE1.Register (this, PFR_RECOVER, 99);
      BE2.Register (this, NO_PFR,      99);
      BE3.Register (this, PFR_RECOVER, 99);
   }

private:
   DS_1();
};


class DS_2 : public SingleWriteDataStore
{
public:
   BaseElement<int> BE1;
   BaseElement<int> BE2;
   BaseElement<int> BE3;

   ~DS_2() { } 

   DS_2 (Role role) :
      SingleWriteDataStore ("DS_2", role)
   {
      BE1.Register (this, PFR_RECOVER, 99);
      BE2.Register (this, NO_PFR,      99);
      BE3.Register (this, PFR_RECOVER, 99);
   }

private:
   DS_2();
};


class DS_3 : public SingleWriteDataStore
{
public:
   BaseElement<int> BE1;

   ~DS_3() { } 

   DS_3 (Role role) :
      SingleWriteDataStore ("DS_3", role)
   {
      BE1.Register (this, NO_PFR, 99);
   }

private:
   DS_3();
};


class DS_4 : public SingleWriteDataStore
{
public:
   BaseElement<int> BE1;
   BaseElement<int> BE2;
   BaseElement<int> BE3;

   ~DS_4() { } 

   DS_4 (Role role) :
      SingleWriteDataStore ("DS_4", role)
   {
      BE1.Register (this, PFR_RECOVER, 99);
      BE2.Register (this, NO_PFR,      99);
      BE3.Register (this, PFR_RECOVER, 99);
   }

private:
   DS_4();
};



class DS_5 : public SingleWriteDataStore
{
public:
   BaseElement<int> BE1;
   BaseElement<int> BE2;
   BaseElement<int> BE3;

   ~DS_5() { } 

   DS_5 (Role role) :
      SingleWriteDataStore ("DS_5", role)
   {
      BE1.Register (this, PFR_RECOVER, 99);
      BE2.Register (this, NO_PFR,      99);
      BE3.Register (this, PFR_RECOVER, 99);
   }

private:
   DS_5();
};



extern "C" void cds_pfr_test_1()
{
   datalog_SetLevelOutputOptions(log_handle_cds_debug, DataLog_LogEnabled, DataLog_ConsoleEnabled );
   cout << "Creating Datastores" << endl;

   DS_1 _cds1(ROLE_RO);
   DS_2 _cds2(ROLE_RW);
   DS_3 _cds3(ROLE_RO);
   DS_4 _cds4(ROLE_RW);
   DS_5 _cds5(ROLE_RO);

   _cds2.BE1 = 1;
   _cds2.BE2 = 2;
   _cds2.BE3 = 3;

   _cds4.BE1 = 4;
   _cds4.BE2 = 5;
   _cds4.BE3 = 6;


   cout << "Saving the datstores" << endl;
   cout << "CDS 1." << endl;
   cout << "       BE1 is " << _cds1.BE1 << endl;
   cout << "       BE2 is " << _cds1.BE2 << endl;
   cout << "       BE3 is " << _cds1.BE3 << endl;
   cout << "CDS 2." << endl;
   cout << "       BE1 is " << _cds2.BE1 << endl;
   cout << "       BE2 is " << _cds2.BE2 << endl;
   cout << "       BE3 is " << _cds2.BE3 << endl;
   cout << "CDS 3." << endl;
   cout << "       BE1 is " << _cds3.BE1 << endl;
   cout << "CDS 4." << endl;
   cout << "       BE1 is " << _cds4.BE1 << endl;
   cout << "       BE2 is " << _cds4.BE2 << endl;
   cout << "       BE3 is " << _cds4.BE3 << endl;
   cout << "CDS 5." << endl;
   cout << "       BE1 is " << _cds5.BE1 << endl;
   cout << "       BE2 is " << _cds5.BE2 << endl;
   cout << "       BE3 is " << _cds5.BE3 << endl;

   // Save the CDS
   ofstream saveFile;
   saveFile.open(PFSAVE_FILE);
   DataStore::SetAllPfData();
   saveFile.write( DataStore::PfMemoryBlock(), DataStore::PfCompleteLength() );
   saveFile.close();

   // cout.write( DataStore::PfMemoryBlock(), DataStore::PfCompleteLength() );
   // cout << endl;

}



extern "C" void cds_pfr_test_2()
{
   datalog_SetLevelOutputOptions(log_handle_cds_debug, DataLog_LogEnabled, DataLog_ConsoleEnabled );
   cout << "Restoring Datastores" << endl;

   DS_1 _cds1(ROLE_RO);
   DS_2 _cds2(ROLE_RW);
   DS_3 _cds3(ROLE_RO);
   DS_4 _cds4(ROLE_RW);
   DS_5 _cds5(ROLE_RO);

   // Restore the CDS
   ifstream restoreFile;
   restoreFile.open(PFSAVE_FILE);
   restoreFile.read( DataStore::PfMemoryBlock(), DataStore::PfCompleteLength() );
   restoreFile.close();

   // cout.write( DataStore::PfMemoryBlock(), DataStore::PfCompleteLength() );
   // cout << endl;

   DataStore::RetrieveAllPfData();
   DataStore::RestoreAllPfData();

   if ( (_cds1.BE1 == 99) &&
        (_cds1.BE2 == 99) &&
        (_cds1.BE3 == 99) )
   {
      cout << "CDS 1 Passed." << endl;
      cout << "BE1 is " << _cds1.BE1 << endl;
      cout << "BE2 is " << _cds1.BE2 << endl;
      cout << "BE3 is " << _cds1.BE3 << endl;
   }
   else
   {
      cout << "CDS 1 Failed." << endl;
      cout << "BE1 is " << _cds1.BE1 << endl;
      cout << "BE2 is " << _cds1.BE2 << endl;
      cout << "BE3 is " << _cds1.BE3 << endl;
   }

   if ( (_cds2.BE1 == 1) &&
        (_cds2.BE2 == 99) &&
        (_cds2.BE3 == 3) )
   {
      cout << "CDS 2 Passed." << endl;
      cout << "BE1 is " << _cds2.BE1 << endl;
      cout << "BE2 is " << _cds2.BE2 << endl;
      cout << "BE3 is " << _cds2.BE3 << endl;
   }
   else
   {
      cout << "CDS 2 Failed." << endl;
      cout << "BE1 is " << _cds2.BE1 << endl;
      cout << "BE2 is " << _cds2.BE2 << endl;
      cout << "BE3 is " << _cds2.BE3 << endl;
   }

   if (_cds3.BE1 == 99) 
   {
      cout << "CDS 3 Passed." << endl;
      cout << "BE1 is " << _cds3.BE1 << endl;
   }
   else
   {
      cout << "CDS 3 Failed." << endl;
      cout << "BE1 is " << _cds3.BE1 << endl;
   }

   if ( (_cds4.BE1 == 4) &&
        (_cds4.BE2 == 99) &&
        (_cds4.BE3 == 6) )
   {
      cout << "CDS 4 Passed." << endl;
      cout << "BE1 is " << _cds4.BE1 << endl;
      cout << "BE2 is " << _cds4.BE2 << endl;
      cout << "BE3 is " << _cds4.BE3 << endl;
   }
   else
   {
      cout << "CDS 4 Failed." << endl;
      cout << "BE1 is " << _cds4.BE1 << endl;
      cout << "BE2 is " << _cds4.BE2 << endl;
      cout << "BE3 is " << _cds4.BE3 << endl;
   }

   if ( (_cds5.BE1 == 99) &&
        (_cds5.BE2 == 99) &&
        (_cds5.BE3 == 99) )
   {
      cout << "CDS 5 Passed." << endl;
      cout << "BE1 is " << _cds5.BE1 << endl;
      cout << "BE2 is " << _cds5.BE2 << endl;
      cout << "BE3 is " << _cds5.BE3 << endl;
   }
   else
   {
      cout << "CDS 5 Failed." << endl;
      cout << "BE1 is " << _cds5.BE1 << endl;
      cout << "BE2 is " << _cds5.BE2 << endl;
      cout << "BE3 is " << _cds5.BE3 << endl;
   }
}
