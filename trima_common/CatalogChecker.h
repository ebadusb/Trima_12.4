/*
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      CatalogChecker.h
 *
 * ABSTRACT:   This file contains the functions for checking the catalog number
 *             and admin code. It also defines the function bits associated wit
 *             a tubing set cassette.
 */

#ifndef CatalogChecker_H
#define CatalogChecker_H



const int TUBING_SET_BARCODE_SIZE = 80;

//
// Message definitions for Vista -> GUI catalog list
//
typedef struct
{
   int  catalogNumber;
   int  tubingSetCode;
   char barcode[TUBING_SET_BARCODE_SIZE];      // null terminated
} VistaCatalogItem;


const int MAX_CATALOG_ITEMS = 20;

typedef struct
{
   int              numberOfItems;
   int              permanent;
   VistaCatalogItem catalogList[MAX_CATALOG_ITEMS];
} VistaCatalogList;


class CatalogChecker
{
public:
   CatalogChecker(unsigned int catalogNumber, unsigned int adminCode);
   virtual ~CatalogChecker() { }

   // true if the catalog number and admin code are "mates"
   bool isValid ();
   bool isBarcodeValid (const char* barcode);

   // returns the function codes for a valid catalog & admin code
   // returns 0 if called for an invalid catalogNumber/adminCode pair.
   unsigned int functionCodes ();

   typedef enum
   {
      DRBC_BAG        = 0,
      RBC_BAG         = 1,
      PLASMA_BAG      = 2,
      PLATELET_BAG    = 3,
      MSS_PLT         = 4,
      NO_SALINE_BOLUS = 5,  // 1 = black stamp kit with no replacement fluid line
      MSS_RBC         = 6,
      STAMP           = 7,  // 1 = white; 0 = black
      spare_1         = 8,  // available for use
      spare_2         = 9,  // available for use
      AC_LUER_CONNECT = 10, // 1 = AC bag has luer connector; 0 = spike
      spare_3         = 11, // available for use
      spare_4         = 12, // available for use
      spare_5         = 13, // available for use
      spare_6         = 14, // available for use
      spare_7         = 15, // available for use
      MAX_NUM_FUNC_BITS     // Maximum number of function bits supported
   } BIT_TO_CHECK;          // which bit in the 10 bit array do you want to check?

   // CHECK THE BITS FOR THE MEMBER CATALOG NUMBER
   bool checkBit (BIT_TO_CHECK bitToCheck);

   // CHECK THE BITS FOR THE ANY 10 BIT FUNCTION CODE
   static bool checkBit (unsigned int funcBits, BIT_TO_CHECK bitToCheck);

   // CREATE AN TUBING SET CODE FOR A GIVEN CATALOG AND FUNCTION BIT NUMBER
   // Not needed on Trima. Intended for off-line TubingCode utility.
   unsigned int createAdminCode (unsigned int catalogNumber, unsigned int functionBits);

   static const unsigned int ORIGINAL_MAX_BITS;
   static const unsigned int ORIGINAL_BIT_MASK;
   static const unsigned int EXTENDED_BIT_MASK;

private:
   unsigned short doCRC16 (unsigned char* data, size_t length);
   void           nextByte (unsigned char b);

   unsigned short _crc;
   unsigned int   _catalogNumber;
   unsigned int   _adminCode;
   unsigned int   _funcBits;
   bool           _isValid;
};

#endif

/* FORMAT HASH 83ed867636289e99c2e57007a9224398 */
