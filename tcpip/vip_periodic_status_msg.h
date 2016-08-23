/*******************************************************************************
 *
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      vip_periodic_status_msg
 *             Vista Interface
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file sends out the Vista Periodic Status Message
 *
 * HISTORY:    $Log: vip_periodic_status_msg.h $
 * HISTORY:    Revision 1.13  2009/02/26 22:57:11Z  jsylusb
 * HISTORY:    Updated the vista interface.
 * HISTORY:    Revision 1.12  2007/08/28 21:38:28Z  spriusb
 * HISTORY:    Fix for auto-split: changing name of REF message to be more generic (no longer VistaRefNumChosenMsg)
 * HISTORY:    Revision 1.11  2006/09/15 14:54:31Z  rm70006
 * HISTORY:    IT 7722.  Added cassette ref number to status message.
 * HISTORY:    Revision 1.10  2003/10/23 18:41:01Z  rm70006
 * HISTORY:    IT 6550.  Fix bugs in handling invalid pins.
 * HISTORY:    Revision 1.9  2003/10/21 15:46:51Z  rm70006
 * HISTORY:    IT 5795.  Remove some unused code and ensure all sent values default to known values.
 * HISTORY:    Revision 1.8  2003/05/28 20:25:58Z  rm70006
 * HISTORY:    IT 6067.
 * HISTORY:
 * HISTORY:    Fix problem with System State message.
 * HISTORY:    Revision 1.7  2003/05/19 22:05:19Z  rm70006
 * HISTORY:    IT 6067 - 6070.
 * HISTORY:
 * HISTORY:    Fixed problems with Periodic Status reporting.
 * HISTORY:    Revision 1.6  2003/05/09 20:21:32Z  ms10234
 * HISTORY:    5829 - removed runtargets message and replaced with runtargets cds
 * HISTORY:    Revision 1.5  2003/04/07 21:47:13Z  rm70006
 * HISTORY:    IT 5818.
 * HISTORY:
 * HISTORY:    Change logging levels.
 * HISTORY:    Revision 1.4  2003/01/27 14:53:58Z  rm70006
 * HISTORY:    Fix problem where some periodic data didn't update until procedure started.
 * HISTORY:    Revision 1.3  2003/01/24 16:54:51Z  rm70006
 * HISTORY:    Fix bug in periodic status
 * HISTORY:    Revision 1.2  2003/01/15 21:38:29Z  rm70006
 * HISTORY:    Fill in missing values.
 * HISTORY:    Revision 1.1  2003/01/02 22:30:34Z  rm70006
 * HISTORY:    Initial revision
 *
 *******************************************************************************/

#ifndef __VIP_PERIODIC_STATUS_MSG__
#define __VIP_PERIODIC_STATUS_MSG__

#include "vipif.h"       // Get Vista message defn's

#include "message.h"         // Trima message system
#include "trimamessages.h"   // Trima Int message defn's

#include "procstate.h"       // Include for proc state message.
#include "guiproc.h"         // Include for cassette position.

#include "config_cds.h"            // Gets trima configurations
#include "procrun_cds.h"           // Gets procedure run info
#include "proctime_cds.h"          // Gets procedure time
#include "procruntargets_cds.h"    // Gets procedure run targets info
#include "proctrimaset_cds.h"      // Gets cassette State
#include "procvolumes_cds.h"       // Gets procedure volumes
#include "proccycle_cds.h"         // Gets cycle values
#include "procdonor_cds.h"         // Gets donor values
#include "procplsrinseback_cds.h"  // Gets Plasma Rinseback values


class VistaPeriodicStatusMsg
{
public: // Member functions
   VistaPeriodicStatusMsg();
   virtual ~VistaPeriodicStatusMsg();

   void Update (trimaPeriodicStatusMsg& msg);


private:  // Member functions
   //
   // Callback for Trima messages
   //
   void HandleProcTargetMsg (void);
   void HandleProcStateMsg (void);
   void HandleCassettePositionMsg (void);
   void HandleSystemStateChangeMsg (void);
   void HandleRefSelectedMsg (void);

   //
   // CDS instances
   //
   Config_CDS           _config_CDS;
   ProcCycle_CDS        _procCycleCds;
   ProcDonor_CDS        _procDonorCds;
   ProcRun_CDS          _procRunCds;
   ProcRunTargets_CDS   _procRunTargetsCds;
   ProcTime_CDS         _procTimeCds;
   ProcTrimaSet_CDS     _procTrimaSetCds;
   ProcVolumes_CDS      _procVolumeCds;
   ProcPlsRinseback_CDS _procPlsRinsebackCds;



private:  // Member data
   // Periodic Status "feeder" messages.
   State_names               _systemState;
   SystemStateChangeMsg      _systemStateMsg;

   ProcState                 _procState;
   Message<ProcState>        _procStateMsg;

   CASSETTE_POSITION_STATUS  _cassettePosition;
   CassettePositionStatusMsg _cassettePositionMsg;
   CASSETTE_MESSAGE          _Ref;
   Message<CASSETTE_MESSAGE> _RefSelectedMsg;

   int  _procedureNumber;
   bool _isPtfDrbc;
   bool _isRasSet;
   bool _isDrbcSet;
};

#endif

/* FORMAT HASH 81723382a317e3c24e1d801ff9734b6d */
