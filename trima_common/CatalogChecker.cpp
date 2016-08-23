/**
 * Copyright (C) 2006-2013 Terumo BCT, Inc.  All rights reserved.
 *
 * @file   CatalogChecker.cpp
 *
 * @brief  Implements the CatalogChecker class
 *
 * In Oct 2013, CatalogChecker was modified to extend the number of function
 * bits from 10 to 16. If necessary, one could easily extend it to 32 bits
 * provided the interface is extended to accommodate a uint64_t admin code.
 * Doing this also requires changes to:
 *
 * - Tubing Set keypad needs a 12-digit code (it was extended from 8 to 10)
 * - Vista interface updates for a uint64_t admin code vs. uint32_t
 * - Updates to any other internal clients for uint64_t vs. uint32_t
 * - Updates to .../engr_tools/TubingCode utility for generating codes
 */

#ifdef _WIN32
#include <stdint.h>
#else
#include <vxworks.h>
#endif

#include <stdio.h>
#include <string.h>

#include "CatalogChecker.h"

// constants used by the CRC calculator
static const unsigned short CRC_POLYNOMIAL  = 0x8005;
static const unsigned short CRC_XOR_OUTPUT  = 0;
static const unsigned short CRC_RESET_VALUE = 0;

// bit masks to determine if the admin code is using extended function bits
const unsigned int CatalogChecker::ORIGINAL_MAX_BITS = 10;
const unsigned int CatalogChecker::ORIGINAL_BIT_MASK = ((1 << ORIGINAL_MAX_BITS) - 1);
const unsigned int CatalogChecker::EXTENDED_BIT_MASK = ~ORIGINAL_BIT_MASK;

CatalogChecker::CatalogChecker(unsigned int catalogNumber, unsigned int adminCode)
   : _crc(CRC_RESET_VALUE),
     _catalogNumber(catalogNumber),
     _adminCode(adminCode),
     _funcBits(adminCode >> 16)
{
   // the CRC values to compare
   unsigned short crcFromAdminCode = static_cast<unsigned short>(_adminCode);
   unsigned short calculatedCrc    = 0;

   // does this kit use the extended function bits?
   const bool isExtended = static_cast<bool>(_funcBits & EXTENDED_BIT_MASK);

   if (isExtended)
   {
      // currently, only 16 function bits are defined, but here we reserve
      // 32 bits for future expansion of catalog and/or function bits
      uint64_t dataToCRC = (static_cast<uint64_t>(_catalogNumber) << 32) | _funcBits;
      calculatedCrc = doCRC16(reinterpret_cast<unsigned char*>(&dataToCRC), sizeof(dataToCRC));
   }
   else
   {
      // here, we use the original function bit scheme
      uint32_t dataToCRC = (_catalogNumber << ORIGINAL_MAX_BITS) | _funcBits;
      calculatedCrc = doCRC16((unsigned char*)(&dataToCRC), sizeof(dataToCRC));
   }

   _isValid = (calculatedCrc == crcFromAdminCode);

  #ifndef _WIN32
   // On Trima, cassette is totally worthless if catalog/admin pair is bad,
   // so clear the function bits out to an invalid value.
   if (!_isValid) _funcBits = 0;
  #endif // _WIN32
}

bool CatalogChecker::isValid ()
{
   return _isValid;
}


bool CatalogChecker::isBarcodeValid (const char* barcode)
{
   char catalogNumber[20];

   catalogNumber[0] = 0;

   sprintf(catalogNumber, "%d", _catalogNumber);

   if (strstr(barcode, catalogNumber) != 0)
      return true;
   else
      return false;
}


unsigned int CatalogChecker::functionCodes ()
{
   return _funcBits;
}


bool CatalogChecker::checkBit (BIT_TO_CHECK bitToCheck)
{
   return checkBit(_funcBits, bitToCheck);
}


bool CatalogChecker::checkBit (unsigned int funcBits, BIT_TO_CHECK bitToCheck)
{
   // Shift off bitToCheck digits and mask with 1.
   return static_cast<bool>((funcBits >> static_cast<unsigned>(bitToCheck)) & 1);
}

#ifdef _WIN32
//
// This is N/A on Trima side. Used for creating Admin codes via TubingCode
// generator utility (.../engr_tools/TubingCode)
//
unsigned int CatalogChecker::createAdminCode (unsigned int catalogNumber, unsigned int functionBits)
{
   // check that the max number of supported bits is not exceeded
   if ( functionBits >= (1 << MAX_NUM_FUNC_BITS) )
   {
      printf("%s(): functionBits=%#x is invalid! MAX_NUM_FUNC_BITS=%d",
             __FUNCTION__, functionBits, MAX_NUM_FUNC_BITS);
      return 0;
   }

   unsigned int calculatedCrc = 0;

   // does this kit use the extended function bits?
   const bool isExtended = (bool)(functionBits & EXTENDED_BIT_MASK);

   if (isExtended)
   {
      // currently, only 16 function bits are defined, but here we reserve
      // 32 bits for future expansion of catalog and/or function bits
      uint64_t dataToCRC = ((uint64_t)catalogNumber << 32) | functionBits;
      calculatedCrc = doCRC16((unsigned char*)&dataToCRC, sizeof(dataToCRC));
   }
   else
   {
      // here, we use the original function bit scheme
      uint32_t dataToCRC = (catalogNumber << ORIGINAL_MAX_BITS) | functionBits;
      calculatedCrc = doCRC16((unsigned char*)&dataToCRC, sizeof(dataToCRC));
   }

   unsigned int adminCode = (functionBits << 16) | calculatedCrc;

   return adminCode;
}
#endif // _WIN32

////////////////////////////////////////////////////////////////////////////////
// This is the same CRC calculator as .../cds/crc16.{h,cpp}. I considered
// replacing this internal impl with it, but decided against it since the
// WIN32 Visual C++ TubingCode generator also uses this implementation.
// And also to ensure backward compatibility with admin codes from the past.

unsigned short CatalogChecker::doCRC16 (unsigned char* data, size_t length)
{
   _crc = CRC_RESET_VALUE;

   while (length--)
      this->nextByte(*(data++) );

   return CRC_XOR_OUTPUT ^ _crc;
}

void CatalogChecker::nextByte (unsigned char b)
{
   int i;
   const int nSize = 8;

   _crc ^= ( static_cast<unsigned short>(b) << nSize);

   for (i = 0; i<nSize; ++i)
   {
      if (_crc & 0x8000)
      {
         _crc = (_crc << 1) ^ CRC_POLYNOMIAL;
      }
      else
      {
         _crc <<= 1;
      }
   }
}

/* FORMAT HASH 6fabcf5fc59228d5b216dc3d6c297143 */
