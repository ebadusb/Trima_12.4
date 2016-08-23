/*
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/os/rcs/sys_info.h 1.1 2007/11/07 22:38:44Z dslausb Exp $
 *
 *	Taos system information
 *
 * $Log: sys_info.h $
 * Revision 1.1  2007/11/07 22:38:44Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:34:17Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:30:37Z  dslausb
 * Initial revision
 * Revision 1.2  2007/08/28 16:04:38Z  rhecusb
 * sysCPUSpeed() function does not currently work with vxWorks version 5.5.1. Will need to revisit this.
 * Revision 1.1  2004/03/04 17:55:16Z  jl11312
 * Initial revision
 *
 */

#ifndef _SYS_INFO_INCLUDE
#define _SYS_INFO_INCLUDE

/*
 *	Make sure this file uses only constructs valid in both C and C++.  Although
 * it is used mostly from .cpp files, the operating system image build
 * uses it from a .c file.
 */

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/*
 * Get system information
 */

/***** RJH_08_28_07: NOTE: sysCPUSpeed() does not work with vxWorks 5.5.1 for now *****/
// unsigned long sysCPUSpeed(void);
//
const char* sysProjectRevision (void);

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _SYS_INFO_INCLUDE */

/* FORMAT HASH 9740267ecc4059f25c85d969d505adf9 */
