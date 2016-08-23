

#include "crcgen.h"
#include <iostream>

using namespace std;

int main (int argc, char*  argv[])
{
       char* str = "select_screen_display_platelet_volume=0";
      unsigned long  crc = 0;
      crcgen32(&crc, (unsigned char *)str, strlen(str));
      //file_crcgen32 (argv[1],&crc);

      cout << "CRC is -> " ;
      cout <<  hex  ;
      cout << crc << endl;
      return 0;
   
}