/*
 *
 * $Header$
 *
 *	Trima boot-mode functions
 *
 */

#ifndef _BOOTMODE_INCLUDE
#define _BOOTMODE_INCLUDE

/*
 *	Make sure this file uses only constructs valid in both C and C++.  Although
 * it is used mostly from .cpp files, the operating system image build
 * uses it from a .c file.
 */

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

extern BOOL trimaSingleStepBootMode;
extern BOOL trimaNormalBootMode;
extern BOOL trimaKeyboardAttached;
extern BOOL safetyBootInProgress;
extern BOOL safetyBootFailed;
extern BOOL safetyAlive;


/*
 *	Get boot mode information
 */
int bootSingleStepEnabled (void);
int bootNormalModeEnabled (void);
int bootServiceModeEnabled (void);
int bootAltModeEnabled (void);
int bootKeyboardAttached (void);


/*
 *	Support functions for service/single step modes
 */
int bootPauseButtonPressed (void);
int bootStopButtonPressed (void);

/*
 *  Boot Progress Info
 */
int bootSafetyBootFailed (void);
int bootSafetyIsAlive (void);



typedef enum
{
   SingleStep_Continue,
   SingleStep_Abort
} TRIMAOSSingleStepStatus;

TRIMAOSSingleStepStatus bootPromptSingleStep (const char* msg);


#ifdef __cplusplus
}  // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _BOOTMODE_INCLUDE */

/* FORMAT HASH 8332313a18d61061042b81ea87a8c8c7 */
