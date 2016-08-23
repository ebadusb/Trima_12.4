/*
 *
 * $Header$
 *
 *  Inter-processor communications provided directly by the OS layer
 *
 */

#ifndef _OS_COMM_INCLUDE
#define _OS_COMM_INCLUDE

/*
 *  Make sure this file uses only constructs valid in both C and C++.  Although
 * it is used mostly from .cpp files, the operating system image build
 * uses it from a .c file.
 */

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/*
 *  Support functions for OS communications
 */
typedef enum
{
   OSComm_Reset             = 0xa55af001, /* request hardware reset (through watchdogs) */
   OSComm_NextProcedure     = 0xa55af002, /* request shutdown for next procedure */
   OSComm_Fatal             = 0xa55af003, /* request shutdown for fatal error */
   OSComm_SwitchToService   = 0xa55af004, /* request service mode */
   OSComm_WriteSafetyPFData = 0xa55af005, /* write safety power fail data to disk (only supported when sent from safety to control) */
   OSComm_ClearSafetyPFData = 0xa55af006, /* clear safety power fail data from disk (only supported when sent from safety to control) */
   OSComm_RequestTimeData   = 0xa55af007, /* write safety power fail data to disk (only supported when sent from safety to control) */
   OSComm_SendTimeData      = 0xa55af008, /* clear safety power fail data from disk (only supported when sent from safety to control) */
   OSComm_SafetyAlive       = 0xa55af009  /* Safety is Alive notification sent from Safety to Control */

} OSCommData;

typedef void OSCommCallback (OSCommData data);

void osCommSetCallback (OSCommCallback* callback);
void osCommSend (OSCommData data);

#ifdef __cplusplus
}  // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _OS_COMM_INCLUDE */

/* FORMAT HASH 23b94e589c1fb0eb9c9dda284a250cc6 */
