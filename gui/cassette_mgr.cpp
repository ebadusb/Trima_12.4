/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cassette_mgr.cpp
 *             Cassette Manager
 *
 * AUTHOR:     Dan Slauson
 *
 * ABSTRACT:   This file provides utility functions for the addition of
 *		cassette to the cassette catalog.
 *
 *		There's two classes.  The first, CatalogNumber, contains the actual
 *		functionality parameters of the cassette.  The other, CassetteMgr,
 *		is the one that is in charge of reading and writing the file and managing
 *		data structures full of CatalogNumber.
 *
 *		There are two data structures full of CatalogNumber pointers.  The
 *		first is a map that corresponds to the master list in cassette.dat.
 *		The second is a vector that corresponds to the admin list in setconfig.dat.
 *
 ******************************************************************************/

#include <vxWorks.h>
#include <time.h>

#include "cassette_mgr.hpp"

#include "cassette_dat.h"     // Cassette data file I/O
#include "trima_datalog.h"    // Datalog stuff
#include "cfg_ids.h"          // Get tracking values for audit level
#include "guiglobs.hpp"       // Gui globals
#include "display_text.h"     // To find alarm strings.
#include "text_utils.h"       // Gui Text Utilities
#include "cassette.h"



#define _AUTOLOAD_CASSETTE_ "80470" // "80529"

//
// Default constructor
//
CassetteMgr::CassetteMgr()
   : _vistaRefNumListMsg(),
     _pfrCassetteFuncBitsMsg(),
     _currentCassette(NULL),
     _scannedCassette(NULL),
     _cassetteToDelete(),
     _initMasterOk(false),
     _initAdminOk(false)
{ }

void CassetteMgr::handleVistaRefNumListMsg ()
{
   const VistaCatalogList msgList = _vistaRefNumListMsg.getData();

   DataLog (log_level_gui_info) << "Received REF Number List Message from Vista." << endmsg;

   DeleteAllCassettes();

   for (int i = 0; i < msgList.numberOfItems; i++)
   {
      const int      barcodeSize = strlen(msgList.catalogList[i].barcode);
      CatalogChecker catalogChecker(msgList.catalogList[i].catalogNumber, msgList.catalogList[i].tubingSetCode);

      if ((barcodeSize <= 0) ||
          (barcodeSize > TUBING_SET_BARCODE_SIZE) ||
          (!catalogChecker.isValid())
          )
      {
         // Log event for later debugging.
         DataLog(log_level_gui_error) << "Catalog entry " << i << " is invalid.  No more will be added." << endmsg;
         return;
      }

      char refNumString[CatalogNumber::REF_NUM_SIZE];
      sprintf(refNumString, "%d", msgList.catalogList[i].catalogNumber);

      AdminCassetteDat::addCassette(refNumString,
                                    msgList.catalogList[i].tubingSetCode,
                                    msgList.catalogList[i].barcode);
   }

   // persist the change if desired
   if (msgList.permanent)
   {
      AdminCassetteDat::updateCassetteFile();
   }
}



//
// Default destructor
//
CassetteMgr::~CassetteMgr()
{
   delete _currentCassette;
   setCurrentSet(NULL);
   delete _scannedCassette;
   _scannedCassette = NULL;
}

//
// Initialize
//
void CassetteMgr::Initialize ()
{
   _vistaRefNumListMsg.init(Callback<CassetteMgr>(this, &CassetteMgr::handleVistaRefNumListMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
   _pfrCassetteFuncBitsMsg.init(Callback<CassetteMgr>(this, &CassetteMgr::gotPfrFuncBits), MessageBase::SNDRCV_RECEIVE_ONLY);

   _initMasterOk = MasterCassetteDat::fileOK();
   _initAdminOk  = AdminCassetteDat::fileOK();
}



//
// AddNewCassette
//
CassetteMgr::ADD_RETURN_STATUS CassetteMgr::AddNewCassette (const char* catalogNumber, const char* adminCode, const char* barCode)
{
   DataLog (log_level_gui_info) << "Starting AddNewCassette("
                                << catalogNumber << ", " << adminCode << ", " << barCode << ")." << endmsg;

   if (!_initAdminOk)
   {
      DataLog (log_level_gui_error) << "Admin list did not initialize OK.  Adding a new cassette is out of the question." << endmsg;
      return CASSETTE_ADD_INVALID;
   }

   CatalogChecker catalogChecker((unsigned int)atoi(catalogNumber),
                                 (unsigned int)strtoul(adminCode, NULL, 10));

   if (!catalogChecker.isValid())
   {
      DataLog (log_level_gui_info) << "Entered cassette is not valid. Catalog Number: "
                                   << catalogNumber << ", Admin Code: " << adminCode << endmsg;
      return CASSETTE_ADD_INVALID;
   }

   // If a barcode was passed, and it does not contain the REF number as a substring,
   // then we should reject it, because that's not cool with us. (See IT 7754)
   if ((strcmp(barCode, "") != 0) &&
       (strstr(barCode, catalogNumber) == NULL))
   {
      DataLog (log_level_gui_info) << "Scanned barcode does not match cassette number." << endmsg;
      return CASSETTE_ADD_INVALID_BARCODE;
   }

   DataLog (log_level_gui_info) << "Valid cassette: (ID:" << catalogNumber
                                << ", Stamp:" << (int)catalogChecker.checkBit(CatalogChecker::STAMP)
                                << ", PAS:" << (int)catalogChecker.checkBit(CatalogChecker::MSS_PLT)
                                << ", RAS:" << (int)catalogChecker.checkBit(CatalogChecker::MSS_RBC)
                                << ")" << endmsg;

   // See if it already exists, and if so, replace it.
   for (CASSETTE_VECTOR_ITERATOR it = AdminCassetteDat::begin(); it != AdminCassetteDat::end(); it++)
   {
      const char* refNumStr = (*it)->RefNum();

      if (strcmp(refNumStr, catalogNumber) == 0)
      {
         AdminCassetteDat::erase(it);
         AdminCassetteDat::addCassette(catalogNumber,
                                       (unsigned int)strtoul(adminCode, NULL, 10),
                                       barCode);

         DataLog (log_level_gui_info) << "Replaced existing catalog number in admin catalog file" << endmsg;
         return CASSETTE_ADD_SUCCESS;
      }
   }

   // If the vector is not already up to the limit, slap it on the end.
   if (AdminCassetteDat::size() >= MAX_CATALOG_ITEMS)
   {
      DataLog (log_level_gui_error) << "Admin cassette file has reached limit of "
                                    << MAX_CATALOG_ITEMS << " cassettes.  Unable to add this one." << endmsg;
      return CASSETTE_ADD_INVALID;
   }

   AdminCassetteDat::addCassette(catalogNumber,
                                 (unsigned int)strtoul(adminCode, NULL, 10),
                                 barCode);

   DataLog(log_level_gui_info) << "New catalog successfully added." << endmsg;

   // Now, update the pin file so we are in sync.
   return CASSETTE_ADD_SUCCESS;
}


bool CassetteMgr::LookupCassette (const char* cassetteIdString)
{
   DataLog (log_level_gui_info) << "Entering LookupCassette(" << cassetteIdString << ")" << endmsg;

   clearCurrent();

   if (_initAdminOk)
   {
      // Look through the small list first.
      for (CASSETTE_VECTOR_ITERATOR it = AdminCassetteDat::begin(); it != AdminCassetteDat::end(); it++)
      {
         const char* refNum = (*it)->RefNum();

         if (strcmp(refNum, cassetteIdString) == 0)
         {
            setCurrentSet(*it);

            DataLog(log_level_gui_info) << "Found matching cassette for \"" << cassetteIdString
                                        << "\": (Stamp:" << (((*it)->isWhiteStamp()) ? "White, " : "Black, ")
                                        << "isPas:" << (int)(*it)->isPas()
                                        << ", isRas:" << (int)(*it)->isRas()
                                        << ", isDrbc:" << (int)(*it)->isDrbc()
                                        << ") in admin list.  Setting as current." << endmsg;

            return true;
         }
      }

      DataLog (log_level_gui_info) << "Admin list does not contain the cassette. Searching master list." << endmsg;
   }
   else
      DataLog (log_level_gui_info) << "Admin list failed to initialize.  Not searched." << endmsg;

   if (!_initMasterOk)
   {
      DataLog (log_level_gui_info) << "Master list failed to initialize.  Not searched." << endmsg;
      return false;
   }

   // If we get to this point, the small list didn't have our cassette,
   // so search through the big list.
   CASSETTE_MAP_ITERATOR foundCassette = MasterCassetteDat::find(cassetteIdString);

   if (foundCassette == MasterCassetteDat::end())
   {
      DataLog (log_level_gui_info) << "Unable to find cassette entered." << endmsg;
      return false;
   }
   else
   {
      DataLog(log_level_gui_info) << "Found matching cassette for \"" << cassetteIdString
                                  << "\": (Stamp:" << ((foundCassette->second->isWhiteStamp()) ? "White, " : "Black, ")
                                  << "isPas:" << (int)foundCassette->second->isPas()
                                  << ", isRas:" << (int)foundCassette->second->isRas()
                                  << ", isDrbc:" << (int)foundCassette->second->isDrbc()
                                  << "). Setting as current." << endmsg;

      setCurrentSet(foundCassette->second);
      return true;
   }

   return MasterCassetteDat::find(cassetteIdString) != MasterCassetteDat::end();
}



bool CassetteMgr::ConfirmDeleteOne ()
{
   if (!_initAdminOk)
   {
      DataLog (log_level_gui_info) << "Admin list did not initialize OK.  Aborting deletion." << endmsg;
      return false;
   }

   if (strcmp(_cassetteToDelete, "") == 0)
   {
      DataLog (log_level_gui_error) << "Error: no cassette string stored." << endmsg;
      return false;
   }

   // Look through the small list first.
   for (CASSETTE_VECTOR_ITERATOR it = AdminCassetteDat::begin(); it != AdminCassetteDat::end(); it++)
   {
      const char* refNum = (*it)->RefNum();
      if (strcmp(refNum, _cassetteToDelete) == 0)
      {
         CatalogNumber* deleteMe = *it;
         AdminCassetteDat::erase(it);
         delete deleteMe;

         DataLog (log_level_gui_info) << "Cassette " << _cassetteToDelete << " deletion successful." << endmsg;
         clearCassetteToDelete();
         return true;
      }
   }

   DataLog (log_level_gui_error) << "Error: Cassette deletion unsuccessful." << endmsg;
   clearCassetteToDelete();
   return false;
}

bool CassetteMgr::DeleteAllCassettes ()
{
   if (!_initAdminOk)
   {
      DataLog (log_level_gui_info) << "Admin list did not initialize OK.  Aborting deletion." << endmsg;
      return false;
   }

   AdminCassetteDat::clear();

   if (AdminCassetteDat::size() == 0)
   {
      DataLog (log_level_gui_info) << "Delete all cassettes successful." << endmsg;
      return true;
   }

   DataLog (log_level_gui_error) << "Error: Delete all cassettes unsuccessful." << endmsg;
   return false;
}



bool CassetteMgr::HasCurrentCassetteSet ()
{
   return _currentCassette != NULL;
}



bool CassetteMgr::HasCurrentScannedCassetteSet ()
{
   return _scannedCassette != NULL;
}



guistring CassetteMgr::getCurrentStringID ()
{
   if (_currentCassette == NULL)
      return "";

   return _currentCassette->RefNum();
}



void CassetteMgr::setScannedAsCurrent ()
{
   DataLog (log_level_gui_info) << "Setting current cassette as most recently scanned cassette: "
                                << _scannedCassette->RefNum() << endmsg;
   setCurrentSet(_scannedCassette);
}



// Return the function bits for the currently selected cassette
unsigned int CassetteMgr::getFunctBits ()
{
#if (CPU==SIMNT)
   if (_currentCassette == NULL)
      simulatorLoadDefaultSet();

   return _currentCassette->FunctBits();
#else

   if (_currentCassette == NULL || _currentCassette->FunctBits() <= 0)
      return guiglobs::cdsData.run.CassetteFunctionBits.Get();
   else
      return _currentCassette->FunctBits();

#endif // #if (CPU==SIMNT)
}



void CassetteMgr::sendFunctBits ()
{
   if (_currentCassette == NULL)
   {
      DataLog (log_level_gui_error) << "ERROR: No function bits to send to proc." << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "Unable to send function bits to proc.");
   }
   else
   {
      CASSETTE_MESSAGE cassetteMsg;

      cassetteMsg.functBits = _currentCassette->FunctBits();
      cassetteMsg.ref       = atoi(_currentCassette->RefNum());

      DataLog(log_level_gui_info) << "Sending cassette (REF=" << cassetteMsg.ref
                                  << "; BITS=" << cassetteMsg.functBits << ")" <<  endmsg;

      Message<CASSETTE_MESSAGE> sendCassette(MessageBase::SEND_LOCAL);
      sendCassette.send(cassetteMsg);
   }
}




// Return the function bits for cassette that matches the parameter string
// Return 0 if not found.
unsigned int CassetteMgr::getFunctBits (char* cassetteIdString)
{
   if (_initAdminOk)
   {
      // Look through the small list first.
      for (CASSETTE_VECTOR_ITERATOR it = AdminCassetteDat::begin(); it != AdminCassetteDat::end(); it++)
      {
         if (strcmp((*it)->RefNum(), cassetteIdString) == 0)
         {
            return (*it)->FunctBits();
         }
      }

      DataLog (log_level_gui_info) << "Cassette not found in admin list.  Looking through master list." << endmsg;
   }
   else
      DataLog (log_level_gui_info) << "Admin list failed to initialize properly.  Skipping and looking through master list." << endmsg;

   if (!_initMasterOk)
   {
      DataLog (log_level_gui_info) << "Master list did not initialize properly.  Aborting FuncBits search." << endmsg;
      return 0;
   }
   // If you can't find them in the small list, look through the big list.
   CASSETTE_MAP_ITERATOR foundCassette = MasterCassetteDat::find(cassetteIdString);

   if (foundCassette == MasterCassetteDat::end())
   {
      DataLog (log_level_gui_info) << "Unable to locate cassette associated with that string." << endmsg;
      return 0;
   }
   else
      return foundCassette->second->FunctBits();
}



void CassetteMgr::clearCurrent ()
{
   setCurrentSet(NULL);
}



void CassetteMgr::clearScannedCassette ()
{
   _scannedCassette = NULL;
}



int CassetteMgr::numSets ()
{
   return AdminCassetteDat::size();
}



// Look through the master list for an admin number associated with
// this REF number.  If there is one, return it.
int CassetteMgr::findAdminNum (const char* refNum)
{
   if (!_initMasterOk)
   {
      DataLog (log_level_gui_info) << "Master list did not initialize OK.  Aborting search." << endmsg;
      return 0;
   }

   CASSETTE_MAP_ITERATOR cassette = MasterCassetteDat::find(refNum);

   if (cassette != MasterCassetteDat::end())
      return cassette->second->AdminCode();
   else
      return 0;
}



// Look through the master list for a barcode associated with
// this REF number.  If there is one, return it.
guistring CassetteMgr::findBarcode (const char* refNum)
{
   guistring blankString("");

   if (!_initMasterOk)
   {
      DataLog (log_level_gui_info) << "Master list did not initialize OK.  Aborting search." << endmsg;
      return blankString;
   }

   CASSETTE_MAP_ITERATOR cassette = MasterCassetteDat::find(refNum);

   if (cassette != MasterCassetteDat::end())
   {
      return guistring(cassette->second->BarcodeNum());
   }
   else
      return blankString;
}



// If you can find a cassette associated with this barcode,
// then stash it for later use.
void CassetteMgr::lookupBarcode (const SCAN_STRUCTURE& scanStruct)
{
   _scannedCassette = NULL;
   char scanString[MAX_SCAN_LENGTH];
   strcpy(scanString, scanStruct.scan_string);

   // Add terminating character.
   scanString[scanStruct.scan_strlen] = '\0';

   DataLog (log_level_gui_info) << "lookupBarcode() beginning search for \""
                                << scanString << "\"" << endmsg;

   if (_initAdminOk)
   {
      // Check the short list (the admin list) first.
      for (CASSETTE_VECTOR_ITERATOR it = AdminCassetteDat::begin(); it != AdminCassetteDat::end(); it++)
      {
         if ((strcmp((*it)->BarcodeNum(), "0") != 0) &&
             (strstr(scanString, (*it)->BarcodeNum()) != NULL))
         {
            DataLog (log_level_gui_info) << "New most recent scanned REF num: " << (*it)->RefNum() << endmsg;
            _scannedCassette = *it;
            return;
         }
      }

      DataLog (log_level_gui_info) << "Barcode not found in admin list.  Searching master list." << endmsg;
   }
   else DataLog (log_level_gui_info) << "Admin list failed to initialize properly.  Skipping and searching master list for barcode" << endmsg;

   if (!_initMasterOk)
   {
      DataLog (log_level_gui_info) << "Master list did not initialize OK.  Aborting search." << endmsg;
      return;
   }

   // If you don't find the barcode in the admin list, start looking through the
   // master list.
   for (CASSETTE_MAP_ITERATOR it = MasterCassetteDat::begin(); it != MasterCassetteDat::end(); it++)
   {
      if ((strcmp(it->second->BarcodeNum(), "0") != 0) &&
          (strstr(scanString, it->second->BarcodeNum()) != NULL))
      {
         DataLog (log_level_gui_info) << "New most recent scanned REF num: " << it->first << endmsg;
         _scannedCassette = it->second;
         return;
      }
   }

   // If you get here, it was not found in either list.
   DataLog (log_level_gui_info) << "Unable to associate barcode ("
                                << scanString << ") with a REF number." << endmsg;
}



// Use this function if you want to check for only one cassette in the admin list (independent of selected procedure)
bool CassetteMgr::checkForSingleOption ()
{
   if (!_initAdminOk)
   {
      DataLog (log_level_gui_info) << "Admin list did not initialize OK.  Aborting check for single option." << endmsg;
      return false;
   }

   if (AdminCassetteDat::size() == 1)
   {
      setCurrentSet(*AdminCassetteDat::begin());

      const char* refNum = _currentCassette->RefNum();

      DataLog (log_level_gui_info) << "Admin cassettes list contains only a single cassette: "
                                   << refNum << ". Setting as current" << endmsg;

      return true;
   } // IT 11937 ->  allow the code below to execute by removing the else here.



   // If we got here, then there is some kind of restriction on
   // the cassette we can use.  We need to look at all of 'em
   // to figure out how many qualify.
   int            validCount          = 0;
   CatalogNumber* placeHolderCassette = NULL;

   for (CASSETTE_VECTOR_ITERATOR it = AdminCassetteDat::begin(); it != AdminCassetteDat::end(); it++)
   {
      if (guiglobs::cdsData.procedures.kitIsCompatibleWithCurrentProcedure(*it))
      {
         const char* refNum = (*it)->RefNum();

         DataLog (log_level_gui_info) << "Cassette " << refNum << " is OK." << endmsg;
         validCount++;
         placeHolderCassette = *it;
      }
   }

   if (validCount == 1)
   {
      DataLog (log_level_gui_info) << "only one Ref# qualified " << "." << endmsg;
      setCurrentSet(placeHolderCassette);
      return true;
   }
   else
   {
      DataLog (log_level_gui_info) << "too many Ref# qualified to auto-pop. valid sets =  " << validCount << " ." << endmsg;
      return false;

   }
}



bool CassetteMgr::checkCurrentForCompatability ()
{
   // If the current cassette is empty, attempt to
   // load one from CDS.  If that doesn't work, return 0.
#if (CPU==SIMNT)
   if (_currentCassette == NULL)
      simulatorLoadDefaultSet();
#endif

   if (_currentCassette == NULL)
   {
      DataLog (log_level_gui_info) << "No current set is available." << endmsg;
      return false;
   }

   if (guiglobs::cdsData.procedures.kitIsCompatibleWithCurrentProcedure(_currentCassette))
   {
      DataLog (log_level_gui_info) << "Current cassette is OK." << endmsg;
      return true;
   }
   else
      return false;
}


void CassetteMgr::setCassetteToDelete (const char* deleteSet)
{
   sprintf(_cassetteToDelete, deleteSet);
}

const guistring& CassetteMgr::getCassetteToDelete () const
{
   return _cassetteToDelete;
}

void CassetteMgr::clearCassetteToDelete ()
{
   sprintf(_cassetteToDelete, "");
}

bool CassetteMgr::canFitCassetteIn (const guistring& refNum) const
{
   if (!_initAdminOk)
   {
      DataLog (log_level_gui_info) << "Admin list did not initialize OK.  Returning false." << endmsg;
      return false;
   }

   if (AdminCassetteDat::size() >= MAX_CATALOG_ITEMS)
   {
      // If we're here, we've already got too many cassettes.
      // If we can overwrite an existing one, though, then
      // we're still OK.
      for (CASSETTE_VECTOR_ITERATOR it = AdminCassetteDat::begin();
           it != AdminCassetteDat::end();
           it++)
      {
         if (strcmp(refNum, (*it)->RefNum()) == 0)
            return true;
      }

      return false;
   }

   return true;
}

#if (CPU==SIMNT)
void CassetteMgr::simulatorLoadDefaultSet ()
{
   if (checkForSingleOption()) DataLog (log_level_gui_info) << "Only one admin cassette available.  Loading that." << endmsg;
   else
   {
      // If this is the simulator, select cassette.
      DataLog (log_level_gui_info) << "Simulator autoloading cassette number "
                                   << _AUTOLOAD_CASSETTE_ << "." << endmsg;
      LookupCassette(_AUTOLOAD_CASSETTE_);
   }
}
#endif

// This gets called when proc sends over some PFR function bits.
// In general, we're calling them directly from CDS now, anyway, but we'll
// still listen and store 'em.
void CassetteMgr::gotPfrFuncBits ()
{
   const int cdsFuncBits = _pfrCassetteFuncBitsMsg.getData();

   if (cdsFuncBits == 0)
   {
      DataLog (log_level_gui_info) << "CDS contains no cassette data." << endmsg;
   }
   else
   {
      DataLog (log_level_gui_info) << "Cassette manager received PFR cassette function bits ("
                                   << cdsFuncBits << ")" << endmsg;

      // In this isolated case, since we don't know the REF, barcode, etc,
      // we can't directly load a set from the existing lists, so just create
      // a new one from just the function bits.
      static CatalogNumber* thePfrCassette = NULL;
      if (thePfrCassette != NULL) delete thePfrCassette;
      thePfrCassette = new CatalogNumber(cdsFuncBits);

      setCurrentSet(thePfrCassette);
   }
}

bool CassetteMgr::currentSetIsPas ()
{
   #if CPU==SIMNT
   return true;
   #endif /* if CPU==SIMNT */

   unsigned int functBits = getFunctBits();

   if (functBits == 0)
   {
      DataLog (log_level_gui_error) << "ERROR: no current cassette data. Returning false." << endmsg;
      return false;
   }
   else return CatalogChecker::checkBit(functBits, CatalogChecker::MSS_PLT);
}


bool CassetteMgr::currentSetIsPlasmaOnly ()
{
   unsigned int functBits = getFunctBits();

   if (functBits == 0)
   {
      DataLog (log_level_gui_error) << "ERROR: no current cassette data. Returning false." << endmsg;
      return false;
   }
   // This is a plasma-only kit if we have a plasma bag, but no RBC or PLT bags.
   else
      return (CatalogChecker::checkBit(functBits, CatalogChecker::PLASMA_BAG) &&
              (!CatalogChecker::checkBit(functBits, CatalogChecker::DRBC_BAG)) &&
              (!CatalogChecker::checkBit(functBits, CatalogChecker::RBC_BAG)) &&
              (!CatalogChecker::checkBit(functBits, CatalogChecker::PLATELET_BAG)));
}



bool CassetteMgr::currentSetIsRas ()
{
   #if CPU==SIMNT
   return true;
   #endif /* if CPU==SIMNT */

   unsigned int functBits = getFunctBits();

   if (functBits == 0)
   {
      DataLog (log_level_gui_error) << "ERROR: no current cassette data. Returning false." << endmsg;
      return false;
   }
   else return CatalogChecker::checkBit(functBits, CatalogChecker::MSS_RBC);
}

bool CassetteMgr::currentSetIsDrbc ()
{
   #if CPU==SIMNT
   return true;
   #endif /* if CPU==SIMNT */

   unsigned int functBits = getFunctBits();

   if (functBits == 0)
   {
      DataLog (log_level_gui_error) << "ERROR: no current cassette data. Returning false." << endmsg;
      return false;
   }
   else return CatalogChecker::checkBit(functBits, CatalogChecker::DRBC_BAG);
}

bool CassetteMgr::currentSetIsWhiteStamp ()
{
   unsigned int functBits = getFunctBits();

   if (functBits == 0)
   {
      DataLog (log_level_gui_error) << "ERROR: no current cassette data. Returning false." << endmsg;
      return false;
   }
   else
      return (CatalogChecker::checkBit(functBits, CatalogChecker::STAMP) == CatalogNumber::WHITE_STAMP);
}

bool CassetteMgr::currentSetHasAcLuer ()
{
   unsigned int functBits = getFunctBits();

   if (functBits == 0)
   {
      DataLog (log_level_gui_error) << "ERROR: no current cassette data. Returning false." << endmsg;
   }
   return CatalogChecker::checkBit(functBits, CatalogChecker::AC_LUER_CONNECT);
}

void CassetteMgr::setCurrentSet (CatalogNumber* newCurrent)
{
   _currentCassette = newCurrent;

   if (!guiglobs::setTextDataItem("currentlySelectedRef",
                                  (_currentCassette ? _currentCassette->RefNum() : "0")))
   {
      _FATAL_ERROR(__FILE__, __LINE__, "Unable to access cassette variable");
   }
}

/* FORMAT HASH 348cb2f9b34aeea0a9f191d001b14a47 */
