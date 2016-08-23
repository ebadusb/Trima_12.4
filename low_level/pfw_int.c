/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      Power Fail Handling
 *
 */

#include <vxWorks.h>
#include <intLib.h>
#include <iv.h>
#include <sysLib.h>

#include "common_hw_private.h"
#include "pfw_int.h"
#include "trima_datalog.h"
#include "trima_tasks.h"
#include "int_util.h"

typedef enum
{
   PFW_IDLE,                  /* no power-fail handling in progress */
   PFW_POWER_DISABLED,        /* 24/64 V power disabled, waiting to determine if shutdown should occur */
   PFW_POWER_RESTORED,        /* 24/64 V power re-enabled, waiting for PFW de-asserted */
   PFW_SHUTDOWN               /* power was not restored, shutting down system */
} PFW_STATE;

/*
 * struct to hold data for power fail handler
 */
typedef struct
{
   unsigned char* powerFailFlag;             /* 0=normal, 1=power fail */
   PFW_STATE      pfwState;                  /* see PFW_STATE enumeration */

   unsigned char  pfwDetected;              /* non-zero if PFW detected */

   unsigned short prevPFStatus;              /* power supply status from previous cycle */
   unsigned short currPFStatus;              /* current (deglitched) power supply status */

   unsigned char  shutdownInProgress;        /* non-zero if power fail shutdown in progress */
} PFWData;

static PFWData data;

/*
 * SPECIFICATION:    Attaches power fail handler
 */
void pfw_attach (unsigned char* powerFailFlag)
{
   data.powerFailFlag      = powerFailFlag;
   *(data.powerFailFlag)   = 0;

   data.pfwState           = PFW_IDLE;
   data.pfwDetected        = 0;
   data.prevPFStatus       = 0xffff;
   data.currPFStatus       = 0xffff;
   data.shutdownInProgress = 0;
}

/*
 * SPECIFICATION:    Shutdown if power fail still active after delay
 *
 * ERROR HANDLING:   none
 */
void pfw_timer (void)
{
   char pfStatus;

   if ( data.shutdownInProgress )
   {
      /*
         * Skip if we have already notified the system of the power fail shutdown.
       */
      return;
   }

   /*
    * We must see two timer updates with the same power fail data before accepting it.
    * This is to handle possible timing glitches in the transitions of the PFW and PLD
    * lines.
    */
   pfStatus = hw_powerFailGetStatus();
   if (pfStatus == data.prevPFStatus)
   {
      if (data.currPFStatus != pfStatus)
      {
         datalog_PrintToDefault(__FILE__, __LINE__,
                                "Power supply status change from 0x%X to 0x%X",
                                (int)data.currPFStatus, (int)pfStatus);
         data.currPFStatus = pfStatus;
      }
   }
   else
   {
      data.prevPFStatus = pfStatus;
   }

   if ( (pfStatus & hw_powerFailWarning) == hw_powerFailWarning &&
        !data.pfwDetected )
   {
      /*
       * Power fail warning signal received from power supply
       */
      *(data.powerFailFlag) = 1;
      data.pfwDetected      = 1;

      /*
       * force the system to the PFW state so that we avoid transition glitches
       */
      data.currPFStatus |= hw_powerFailWarning;
      datalog_PrintToDefault(__FILE__, __LINE__, "PFW detected - status = 0x%X", pfStatus);
   }

   /*
    * Once the PFW is detected, we need to wait until
    * we have determined if the system is shutting down.
    */
   if (data.pfwDetected)
   {
      if ((data.currPFStatus & (hw_powerFailWarning | hw_powerFailLineDrop)) == (hw_powerFailWarning | hw_powerFailLineDrop))
      {
         /*
          * Power not coming back - force data save and shutdown
          */
         data.shutdownInProgress = 1;
         datalog_PrintToDefault(__FILE__, __LINE__, "Starting power fail shutdown sequence, pfwState=%d", (int)data.pfwState);
         data.pfwState           = PFW_SHUTDOWN;
      }
      else if ((data.currPFStatus & (hw_powerFailWarning | hw_powerFailLineDrop)) == (hw_powerFailLineDrop))
      {
         /*
          * Power has been restored - restore power register
          */
         if ( data.pfwState != PFW_POWER_RESTORED )
         {
            hw_powerFailRestore();

            *(data.powerFailFlag) = 0;
            datalog_PrintToDefault(__FILE__, __LINE__, "Restoring power, pfwState=%d", (int)data.pfwState);
            data.pfwState         = PFW_POWER_RESTORED;
         }
      }
      else if ((data.currPFStatus & (hw_powerFailWarning | hw_powerFailLineDrop)) == (hw_powerFailWarning))
      {
         if ( data.pfwState != PFW_POWER_DISABLED )
         {
            hw_powerFailSave();
            datalog_PrintToDefault(__FILE__, __LINE__, "AC interrupted - disabling 24V/64V outputs, pfwState=%d", (int)data.pfwState);
            data.pfwState = PFW_POWER_DISABLED;
         }
      }
      else if ((data.currPFStatus & (hw_powerFailWarning | hw_powerFailLineDrop)) == 0)
      {
         if ( data.pfwState != PFW_POWER_RESTORED )
         {
            datalog_PrintToDefault(__FILE__, __LINE__, "Incorrect power supply transition to AC nominal, pfwState=%d", (int)data.pfwState);

            hw_powerFailRestore();
            *(data.powerFailFlag) = 0;
         }

         data.pfwDetected = 0;
         datalog_PrintToDefault(__FILE__, __LINE__, "AC nominal, pfwState=%d", (int)data.pfwState);
         data.pfwState    = PFW_IDLE;
      }
   }

   if (data.shutdownInProgress)
   {
      hw_powerFailSave();
      trimaStartShutdown(TTE_PowerFail);
   }
}

/* FORMAT HASH fc26aff0a4d3aaecf1a235c6874d1f25 */
