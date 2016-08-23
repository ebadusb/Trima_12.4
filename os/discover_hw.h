/*
 * $Header: $

 * $Log: $
 *
 */

#ifndef _DISCOVER_HW_INCLUDE
#define _DISCOVER_HW_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

unsigned char isVersalogic (void);       /* 0-Not Versalogic 1-Versalogic */
unsigned char isVersalogicVSBC6 (void);  /* 0-Not VSBC-6 1-VSBC-6*/
unsigned char isVersalogicPython (void); /* 0-Not Python 1-Python */

unsigned char isCT655XXGraphics (void); /* 0-Not Chip & Tech. 1-Chips & Tech. */
unsigned char isGeodeLXGraphics (void); /* 0-Not Geode LX Chip 1-Geode LX Chip */

unsigned int getGraphicsVendorID (void);

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _DISCOVER_HW_INCLUDE */

/* FORMAT HASH 6f5414dba08a12a568a35b6359e712c0 */
