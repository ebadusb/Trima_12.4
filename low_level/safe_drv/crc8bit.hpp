/*
 *
 * $Header: $
 * $Log: $
 *
 * TITLE:      crc8bit.hpp, a CRC generator for 8 bit data.  used to
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

#ifndef CRC8BIT_HPP
#define CRC8BIT_HPP

/* Call clrCRC() first. It will initialize the routine */

/* If you wish to process bitwise, call calcCRCbyte() for each bit */
/* that is to be processed. Pass it a char that is 0 if the bit is */
/* zero or not zero if the bit is one */

/* If you wish to process bytewise, call calcCRCbyte() for each byte */
/* that is to be processed. Pass it a char that is the byte. It will */
/* return the current CRC value. */

/* Call getCRC() after all of the bits or bytes are processed, including */
/* the received crc. If zero is returned, the test passes. */


class crc8Bit
{
public: crc8Bit::crc8Bit();
   crc8Bit::~crc8Bit();
   void          calculateCRC (char* data, int length);
   unsigned char getCRC ();
   void          clearCRC ();
private:
   void calcCRCbit (char input);
   void calcCRCbyte (char input);
   unsigned char _crc;
};


#endif // CRC8BIT_HPP

/* FORMAT HASH 0c10c1b967ce1b0452cd601559a96db9 */
