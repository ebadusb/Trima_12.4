/*******************************************************************************
 *
 * Copyright (c) 2003 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      cassette_mgr.hpp
 *             Cassette Manager
 *
 * AUTHOR:     Dan Slauson
 *
 * ABSTRACT:   This file provides utility functions for the addition of
 *		cassette to the cassette catalog.
 *
 *		There's two classes.  The first, CassetteContainer, contains the actual
 *		functionality parameters of the cassette.  The other, CassetteMgr,
 *		is the one that is in charge of reading and writing the file and managing
 *		data structures full of CassetteContainers.
 *
 *		There are two data structures full of CassetteContainer pointers.  The
 *		first is a map that corresponds to the master list in cassette.dat.
 *		The second is a vector that corresponds to the admin list in setconfig.dat.
 *
 ******************************************************************************/

#ifndef __CASSETTE_MGR_HPP__
#define __CASSETTE_MGR_HPP__



#include "message.h"                  // Include message system stuff
#include "trimamessages.h"            // Trima Int messages

#include "bio_signatures.h"           // Include Bio Sig message defns
#include "alarms.h"                   // Get alarm defns
#include "gui_cds.h"                  // Get log item sequence number
#include "safproc.h"                  // Get Donor Vitals Msg
#include "guistring.h"                // To make text for cfr_list.
#include "pfrmsg.h"                   // Gets PFR Status msg
#include "scan_structure.h"
#include "filehelp.h"
#include "CatalogChecker.h"   // Check CRC on catalog numbers.
#include "predict_msgs.h"

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////  CASSETTEMGR CLASS//////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
class CassetteMgr
{
public:
   enum ADD_RETURN_STATUS
   {
      CASSETTE_ADD_SUCCESS,
      CASSETTE_ADD_INVALID,
      CASSETTE_ADD_INVALID_BARCODE
   };

   // CONSTRUCTOR AND DESTRUCTOR
   CassetteMgr();
   virtual ~CassetteMgr();

   // THE NUMBER OF SETS IN THE ADMIN LIST
   int numSets ();

   // HAVE WE REACHED OR EXCEEDED THE LIMIT?
   bool canFitCassetteIn (const guistring& refNum) const;

   // INITIALIZE THIS CLASS
   void Initialize ();

   // ADD AND DELETE CASSETTES FROM/TO THE ADMIN LIST
   ADD_RETURN_STATUS AddNewCassette (const char* catalogNumber, const char* adminCode, const char* barCode);
   bool              ConfirmDeleteOne ();
   bool              DeleteAllCassettes ();

   // FINDS AND STORES A CASSETTE BY ITS REF NUMBER
   bool LookupCassette (const char* cassetteIdString);

   // FINDS AND STORES A CASSETTE BY ITS BARCODE
   void lookupBarcode (const SCAN_STRUCTURE& scanStruct);

   // SET THE CURRENTLY SELECTED CASSETTE AS THE LATEST CASSETTE
   void setScannedAsCurrent ();

   // DO WE CURRENTLY HAVE A CASSETTE SELECTED OR SCANNED?
   bool HasCurrentCassetteSet ();
   bool HasCurrentScannedCassetteSet ();

   // CLEAR THE CURRENTLY SELECTED OR SCANNED CASSETTE
   void clearCurrent ();
   void clearScannedCassette ();

   // GET THE REF NUM STRING FOR THE CURRENTLY SELECTED CASSETTE
   guistring getCurrentStringID ();

   void sendFunctBits ();

   // RETURN THE SECRET SET CODE ASSOCIATED WITH THIS REF NUM
   int findAdminNum (const char* refNum);
   // RETURN THE BARCODE STRING ASSOCIATED WITH THIS REF NUM
   guistring findBarcode (const char* refNum);


   // Use this function if we want to check for a single option after filtering
   // cassettes that are inappropriate for the selected procedure.
   bool checkForSingleOption ();

   // CHECK TO SEE IF THE SCANNED CASSETTE IS COMPATIBLE WITH THE PROCEDURE PARAMETERS PASSED
   bool checkCurrentForCompatability ();

   // SET, GET, AND CLEAR THE VALUE OF THE CASSETTE PENDING DELETION
   void             setCassetteToDelete (const char* deleteSet);
   const guistring& getCassetteToDelete () const;
   void             clearCassetteToDelete ();

   // HANDLE THE REF NUMBER LIST MESSAGE FROM VISTA
   void handleVistaRefNumListMsg ();

   bool currentSetIsPas ();
   bool currentSetIsRas ();
   bool currentSetIsDrbc ();
   bool currentSetIsWhiteStamp ();
   bool currentSetIsPlasmaOnly ();
   bool currentSetHasAcLuer ();

private:
   // Copy and assignment are disallowed/not implemented
   CassetteMgr(const CassetteMgr&);
   CassetteMgr& operator = (const CassetteMgr&);

   // MESSAGE FROM VISTA
   Message<VistaCatalogList> _vistaRefNumListMsg;

   // PFR cassette function bits message
   Proc2GuiPfrSetFuncBits _pfrCassetteFuncBitsMsg;

   // THE CURRENTLY CHOSEN CASSETTE
   CatalogNumber* _currentCassette;

   // THE MOST RECENTLY SCANNED CASSETTE
   CatalogNumber* _scannedCassette;

   // STORES THE CASSETTE TO DELETE BETWEEN "DELETE" AND "CONFIRM" ON MACH SCREEN.
   guistring _cassetteToDelete;

   // THE LISTS WERE INITIALIZED OK.
   bool _initMasterOk, _initAdminOk;

   void gotPfrFuncBits ();

   void verifyCurrentSet ();

   // GET THE FUNCTION BITS FOR THE CURRENTLY SELECTED CASSETTE
   unsigned int getFunctBits ();
   // GET THE FUNCTION BITS FOR THE CASSETTE ASSOCIATED WITH THIS REF NUM
   unsigned int getFunctBits (char* cassetteIdString);

    #if (CPU==SIMNT)
   void simulatorLoadDefaultSet ();
    #endif

   void setCurrentSet (CatalogNumber* newCurrent);

};

#endif

/* FORMAT HASH d3d4df232aea33e6659b401489a847b5 */
