/*
 * copyright 1995, 1996 Cobe BCT Lakewood, Colorado
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/engr_tools/datalog_tools/Trima/include/common/rcs/msgcrc.h 1.1 2009/01/29 17:57:21Z jsylusb Exp $
 * $Log: msgcrc.h $
 * Revision 1.1  2009/01/29 17:57:21Z  jsylusb
 * Initial revision
 * Revision 1.1  2008/08/02 17:30:16Z  ssunusb
 * Initial revision
 * Revision 1.1  2005/12/16 21:49:59Z  rm70006
 * Initial revision
 * Revision 1.2  2002/07/03 20:10:56Z  ms10234
 * Completed some code cleanup and minor fixes
 * Revision 1.1  2002/05/01 14:44:09  ms10234
 * Initial revision
 * Revision 1.1  1999/05/24 23:26:21  TD10216
 * Initial revision
 * Revision 1.2  1996/07/24 19:49:21  SS03309
 * fix MKS 
 * Revision 1.2  1996/07/22 14:42:52  SS03309
 * Revision 1.5  1996/06/27 16:28:18  SS03309
 * Added header/log comments
 *
 * crc.h
 *
 * Compute 32 bit CRC on buffers and focus messages
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS2.DOC
 * Select OMT:       I:\router
 * Test:             I:\ieee1498\STP2.DOC
 *
 * EXAMPLE:
 */

#ifndef CRC_H                    
#define CRC_H

/*
// compute 32bit CRC on block
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
// SPECIFICATION:    compute 32 bit CRC
//                   Parameter:
//                   blk_adr - pointer to block
//                   blk_len - length of block
//
// ERROR HANDLING:   none
*/

unsigned long msgcrc32( unsigned char* blk_adr, unsigned long blk_len);

/*
// SPECIFICATION:    add to existing 32 bit CRC
//                   Parameter:
//                   blk_adr - pointer to block
//                   blk_len - length of block
//                   crc - existing 32 bit CRC
//
// ERROR HANDLING:   none
*/

unsigned long addToMsgCrc32( unsigned char* blk_adr,
                          unsigned long blk_len,
                          unsigned long crc);

#ifdef __cplusplus
};
#endif

#endif
