#ifndef CRC16_H
#define CRC16_H

class Crc16
{
public:
   Crc16(void);
   ~Crc16(void);

   unsigned short doCRC16 (unsigned char* data, int length);
private:
   void nextByte (unsigned char b);

   unsigned short _crc;

};

#endif

/* FORMAT HASH a17929e9c08e3ee6b0ff0a9f9098aed2 */
