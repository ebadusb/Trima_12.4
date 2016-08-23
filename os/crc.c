/*
 * $Header: E:/BCT_Development/Trima5.R/Trima/os/rcs/crc.c 1.1 2007/11/07 22:38:37Z dslausb Exp $
 *
 *	Fast CRC routine
 *
 * $Log: crc.c $
 * Revision 1.1  2007/11/07 22:38:37Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:34:09Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:28:35Z  dslausb
 * Initial revision
 * Revision 1.2  2007/06/15 20:24:00Z  dslausb
 * Tornado's C compiler doesn't like line comments.
 * Revision 1.1  2007/05/22 18:20:08Z  dslausb
 * Initial revision
 * Revision 1.1  2005/01/17 18:24:12Z  ms10234
 * Initial revision
 * Revision 1.2  2004/05/03 18:28:17Z  jl11312
 * - corrected input, output list
 * - added work-around for bug in compiler handling inline assembly to load crctable address
 * Revision 1.1  2004/03/04 17:55:41Z  jl11312
 * Initial revision
 *
 */

#include <vxworks.h>
#include "crcgen.h"
#include "crc_table.h"

/*/ SPECIFICATION: crcgen32()
//   compute 32 bit CRC over a data buffer.
//   overwrites initial crc with the new crc.
// ERROR HANDLING:  returns -1 if arg error, else returns 0
/*/
int crcgen32 (unsigned long* pcrc, const unsigned char* pdata, long length)
{
   int            result = -1;
   unsigned long* table  = crctable;

   __asm__ volatile
   (
      "movl %0, %%ebx\n"            /*/ ebx = pcrc */
      "orl %%ebx, %%ebx\n"
      "jz crcgen32_done\n"
      "movl (%%ebx), %%ebx\n"       /*/ ebx = *pcrc */

      "movl %2, %%esi\n"            /*/ esi = pdata */
      "orl %%esi, %%esi\n"
      "jz crcgen32_done\n"

      "movl $0, %1\n"               /*/ result = 0 */
      "movl %3, %%ecx\n"            /*/ ecx = length */
      "movl %4, %%edi\n"            /*/ edi = crctable */

      "crcgen32_loop:\n"
      "decl %%ecx\n"                    /*/ while (length--) */
      "jl crcgen32_done\n"

      "xorl %%eax, %%eax\n"         /*/ eax = *pdata++ */
      "movb (%%esi), %%al\n"
      "incl %%esi\n"

      "movl %%ebx, %%edx\n"         /*/ eax = crctable[(*pcrc ^ *pdata) & 0xff] */
      "xorb %%dl, %%al\n"
      "movl (%%edi,%%eax,4), %%eax\n"

      "shrl $8, %%edx\n"            /*/ edx = *pcrc >> 8 */

      "xorl %%eax, %%edx\n"         /*/ ebx = crctable[(*pcrc ^ *pdata) & 0xff] ^ (*pcrc >> 8) */
      "movl %%edx, %%ebx\n"
      "jmp crcgen32_loop\n"

      "crcgen32_done:\n"
      "movl %0, %%eax\n"
      "movl %%ebx, (%%eax)"         /*/ *pcrc = ebx */

      : "=m" (pcrc), "=m" (result)
      : "m" (pdata), "m" (length), "m" (table)
      : "eax", "ebx", "ecx", "edx", "edi", "esi"
   );

   return result;
}

/* FORMAT HASH 6e535e7b06ec0e2b1057d93c5f9900ee */
