/*
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/os/rcs/load_module.h 1.1 2007/11/07 22:38:40Z dslausb Exp $
 * $Log: load_module.h $
 * Revision 1.1  2007/11/07 22:38:40Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:34:12Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:29:02Z  dslausb
 * Initial revision
 * Revision 1.1  2007/05/22 18:20:12Z  dslausb
 * Initial revision
 * Revision 1.1  2005/01/10 17:38:44Z  ms10234
 * Initial revision
 *
 */

#ifndef _LOAD_MODULE_INCLUDE
#define _LOAD_MODULE_INCLUDE

#include <moduleLib.h>

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

typedef struct
{
   const char*   fileName;
   MODULE_ID     modId;
   unsigned long textAddr;
   unsigned long dataAddr;
   unsigned long bssAddr;
} LoadModuleInfo;

STATUS loadModuleFromFile (const char* fileName, LoadModuleInfo* info);
STATUS loadModuleFromFileDescriptor (int fileDescriptor, LoadModuleInfo* info);

#ifdef __cplusplus
}  // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _LOAD_MODULE_INCLUDE */

/* FORMAT HASH 9462b71ea4d1170602ede64c62c10774 */
