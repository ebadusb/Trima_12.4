/*
 *
 * $Header$
 *
 *  boot-mode functions
 *
*/


#include <vxWorks.h>
#include <envLib.h>
#include <sysLib.h>

/* Common */
#include "cca_pci_intf.h"

#include "boot_mode.h"
#include "node_data.h"

int bootSingleStepEnabled (void)
{
   return(trimaSingleStepBootMode);
}

int bootNormalModeEnabled (void)
{
   return(trimaNormalBootMode && !getenv("SERVICE_MODE"));
}


int bootServiceModeEnabled (void)
{
   return(trimaNormalBootMode && getenv("SERVICE_MODE"));
}

int bootAltModeEnabled (void)
{
   return(!trimaNormalBootMode);
}

int bootKeyboardAttached (void)
{
   return(trimaKeyboardAttached);
}

int bootSafetyBootFailed (void)
{
   return(safetyBootFailed);
}

int bootSafetyIsAlive (void)
{
   return(safetyAlive);
}


#ifdef CONTROL_BUILD

/* Common */
#include "cca_pci_control.h"

#define CCA_INDX    0   /* PCI resource index for Control CCA */
#define BAR_INDX    0   /* PCI BAR index for Control CCA */

enum { PCI_IN_BUTTONS           = CCA_IO_PORT(CCA_INDX, BAR_INDX, CTL3_INP_BUTTONS) };
enum { PCI_NOT_STOP_SWITCH_BIT  = CTL3_NOT_STOP_SWITCH_BIT };
enum { PCI_NOT_PAUSE_SWITCH_BIT = CTL3_NOT_PAUSE_SWITCH_BIT };

enum { ISA_BASE                 = 0x0B00 };    /* lower I/O base address for control */
enum { ISA_IN_BUTTONS           = ISA_BASE+0x14 };
enum { ISA_NOT_STOP_SWITCH_BIT  = 0x02 };      /* stop switch bit for control */
enum { ISA_NOT_PAUSE_SWITCH_BIT = 0x01 };      /* pause switch bit for control */

#endif /* CONTROL_BUILD */

#ifdef SAFETY_BUILD

/* Common */
#include "cca_pci_safety.h"

#define CCA_INDX    0   /* PCI resource index for Safety CCA */
#define BAR_INDX    0   /* PCI BAR index for Safety CCA */

enum { PCI_IN_BUTTONS           = CCA_IO_PORT(CCA_INDX, BAR_INDX, SAFP_INP_SWITCHES) };
enum { PCI_NOT_STOP_SWITCH_BIT  = SAFP_NOT_STOP_SWITCH_BIT };
enum { PCI_NOT_PAUSE_SWITCH_BIT = SAFP_NOT_PAUSE_SWITCH_BIT };

enum { ISA_BASE                 = 0x0700 };    /* I/O base address for safety */
enum { ISA_IN_BUTTONS           = ISA_BASE+0x06 };
enum { ISA_NOT_STOP_SWITCH_BIT  = 0x08 };      /* stop switch bit for safety */
enum { ISA_NOT_PAUSE_SWITCH_BIT = 0x04 };      /* pause switch bit for safety */

#endif /* SAFETY_BUILD */

int bootPauseButtonPressed (void)
{
   int isPressed = 0;

   /* Note that this button switch is active low */
   if (ccaPciResourcesAvailable())
   {
      UCHAR value = ccaInByte(PCI_IN_BUTTONS);
      isPressed = (value & PCI_NOT_PAUSE_SWITCH_BIT) ? 0 : 1;
   }
   else
   {
      UCHAR value = sysInByte(ISA_IN_BUTTONS);
      isPressed = (value & ISA_NOT_PAUSE_SWITCH_BIT) ? 0 : 1;
   }

   return isPressed;
}

int bootStopButtonPressed (void)
{
   int isPressed = 0;

   /* Note that this button switch is active low */
   if (ccaPciResourcesAvailable())
   {
      UCHAR value = ccaInByte(PCI_IN_BUTTONS);
      isPressed = (value & PCI_NOT_STOP_SWITCH_BIT) ? 0 : 1;
   }
   else
   {
      UCHAR value = sysInByte(ISA_IN_BUTTONS);
      isPressed = (value & ISA_NOT_STOP_SWITCH_BIT) ? 0 : 1;
   }

   return isPressed;
}

TRIMAOSSingleStepStatus bootPromptSingleStep (const char* msg)
{
   if (trimaSingleStepBootMode)
   {
      printf("press and release pause button to %s\r", msg);
      while (!bootPauseButtonPressed())
      {
         taskDelay(1);
      }

      if (bootStopButtonPressed())
      {
         return SingleStep_Abort;
      }

      taskDelay(2);
      while (bootPauseButtonPressed())
      {
         taskDelay(1);
      }

      printf("\n");
   }
   return SingleStep_Continue;
}

/* FORMAT HASH ebbe789149a462d8d41b2603a988127d */
