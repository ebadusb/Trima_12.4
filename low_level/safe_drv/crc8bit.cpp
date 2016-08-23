/*
 *
 * $Header: $
 * $Log: $
 *
 * TITLE:      crc8bit.cpp, a CRC generator for 8 bit data.  used to
 *             check the CRC on the basin data message received from the
 *             basin data microprocessor
 *
 *             Taken from Dallas Semiconductor Website and converted to C++
 *
 * ABSTRACT:   Generates a CRC for 8 bit data.
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 * PUBLIC FUNCTIONS:
 *
 *
 */

#include "crc8bit.hpp"

crc8Bit::crc8Bit()
{
   _crc = 0x0;
}

crc8Bit::~crc8Bit()
{}

void crc8Bit::calculateCRC (char* data, int length)
{
   char* dataPtr;
   dataPtr = data;
   for (int i = 0; i<length; i++)
   {
      calcCRCbyte((char)(*dataPtr));
      dataPtr++;
   }
}

void crc8Bit::calcCRCbit (char input)
{
   if ((_crc & 0x80) != 0)
   {
      _crc <<= 1;
      if (input == 0) _crc ^= 0x31;
   }
   else
   {
      _crc <<= 1;
      if (input != 0) _crc ^= 0x31;
   }
}

void crc8Bit::calcCRCbyte (char input)
{
   int n;
   for (n = 0; n < 8; ++n)
   {
      calcCRCbit((char)(input & 0x01));
      input >>= 1;
   }
}

unsigned char crc8Bit::getCRC ()
{
   return(_crc);
}

void crc8Bit::clearCRC ()
{
   _crc = 0x0;
}

/* FORMAT HASH 24f6e30fbf5e4f406224353d48eda029 */
