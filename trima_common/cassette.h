/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cassette.h
 *             Focussed system cassette functions.
 *
 * ABSTRACT:   This file contains the encapsulation for the cassette.
 *
 */

#ifndef CASSETTE_H
#define CASSETTE_H

#include "datalog.h"
#include "catalogchecker.h"


//
//
//   The cassette class encapsulates the concept of a physical cassette.
//
class cassette
{
public:

   // NOT_LOADED is specified as zero to match the initial value
   // of the cassette type in the common datastore.
   // NOTE: make sure PLTPLSRBC is the first specific-type of cassette type and
   // END_OF_LIST is always last!
   enum cassetteType { NOT_LOADED=0, UNKNOWN, PLTPLSRBC, RBCPLS, END_OF_LIST };

   //
   //
   //  Constructor, destructor
   //
   cassette();
   virtual ~cassette();

   //
   //
   //  You can ask me if I am compatible with a given procedure.  I return
   //  nonzero if I am compatible and zero if I am incompatible.  If my type
   //  is UNKNOWN I am always incompatible; if my type is NOT_LOADED then
   //  I am always compatible. Note that the rbc parameter is DOSE, not
   //  collect volume; platelet and plasma volumes refer to the actual bag
   //  volumes.
   //
   virtual const int compatible (cassetteType type,
                                 const float plateletVolume,
                                 const float plasmaVolume,
                                 const float rbcDose) const;

   //
   //
   //  You can ask me to recommend a cassette type for a procedure.
   //  If I cannot find a valid cassette type I return UNKNOWN.
   //
   virtual const cassetteType recommendedType (const float plateletVolume,
                                               const float plasmaVolume,
                                               const float rbcDose) const;

   //
   //   You can ask me for the maximum amount of plasma that can be collected
   //   given the RBC dosage and platelet target.  If I am given a set type
   //   other than UNKNOWN, then I only recommend for that cassette type.  If
   //   UNKOWN cassette type is given then I examine all sets and return the
   //   maximum.
   //
   const float maxPlasma (cassetteType type,
                          const float plateletVolume,
                          const float rbcDose) const;

   //
   //   You can also ask me for the maximum RBC dose that can be collected
   //   on a given cassette.
   //
   const float maxRbcDose (cassetteType type) const;

protected:

   //
   //   Worker Bee functions
   //
   void volumes (cassetteType type,
                 float& platelet,
                 float& plasma,
                 float& rbcs) const;

};


DataLog_Stream& operator << (DataLog_Stream& os, cassette::cassetteType value);




class CatalogNumber
{
public:
   typedef enum { BLACK_STAMP = 0, WHITE_STAMP = 1 } STAMP_COLOR;
   enum { REF_NUM_SIZE        = 16 };

   CatalogNumber(const char* refNum, unsigned int adminCode, const char* barcodeString);


   // This one should only be used for loading a cassette from CDS.
   // It should be used during PFR, at which point we should no longer
   // care about the barcode or the REF number, only the func bits
   // (which is good, because that's all CDS knows about.
   CatalogNumber(unsigned int funcBits);

   const char*  RefNum () const { return _refNum; }
   const char*  BarcodeNum () const { return _barcode; }
   unsigned int AdminCode () const { return _adminCode; }
   unsigned int FunctBits () const { return _functBits; }

   bool isPlt () const { return CatalogChecker::checkBit(_functBits, CatalogChecker::PLATELET_BAG); }
   bool isPls () const { return CatalogChecker::checkBit(_functBits, CatalogChecker::PLASMA_BAG); }
   bool isRbc () const { return CatalogChecker::checkBit(_functBits, CatalogChecker::RBC_BAG) || CatalogChecker::checkBit(_functBits, CatalogChecker::DRBC_BAG); }

   bool isPas () const { return CatalogChecker::checkBit(_functBits, CatalogChecker::MSS_PLT); }
   bool isRas () const { return CatalogChecker::checkBit(_functBits, CatalogChecker::MSS_RBC); }
   bool isWhiteStamp () const { return CatalogChecker::checkBit(_functBits, CatalogChecker::STAMP) == CatalogNumber::WHITE_STAMP; }
   bool isBlackStamp () const { return CatalogChecker::checkBit(_functBits, CatalogChecker::STAMP) == CatalogNumber::BLACK_STAMP; }
   bool isDrbc () const { return CatalogChecker::checkBit(_functBits, CatalogChecker::DRBC_BAG); }
   bool isNoSalineBolus () const { return CatalogChecker::checkBit(_functBits, CatalogChecker::NO_SALINE_BOLUS); }

private:
   unsigned int _adminCode, _functBits;
   char         _refNum[REF_NUM_SIZE];
   char         _barcode[TUBING_SET_BARCODE_SIZE];

   CatalogNumber(); // default constructor disallowed (private)
};


#endif

/* FORMAT HASH 2968563ab267ad62654025dbefd8453a */
