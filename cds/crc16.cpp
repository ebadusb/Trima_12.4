#include "crc16.h"

#define POLYNOMIAL ((unsigned short)0x8005)
#define RESET_VALUE ((unsigned short)0)
#define CRC_XOR_OUTPUT ((unsigned short)0)

// unsigned short Crc16::_crc;

Crc16::Crc16(void)
   : _crc(RESET_VALUE)
{}


Crc16::~Crc16(void)
{}

void Crc16::nextByte (unsigned char b)
{
   int i;
   const int nSize = 8;

   _crc ^= ( static_cast<unsigned short>(b) << nSize);

   for (i = 0; i< nSize; i++)
   {
      if ( _crc & 0x8000 )
      {
         _crc = (_crc << 1) ^ POLYNOMIAL;
      }
      else
      {
         _crc <<= 1;
      }
   }
}

unsigned short Crc16::doCRC16 (unsigned char* data, int length)
{
   _crc = RESET_VALUE;

   while (length--)
   {
      Crc16::nextByte(*(data++) );
   }

   return CRC_XOR_OUTPUT ^ _crc;
}

/* FORMAT HASH 11cf39de508a46848f0645f4db08867e */
