/*
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.25.1.2  2009/10/05 16:04:02Z  dslausb
 * Shawn's safety code
 * Revision 1.1  2009/10/02 19:12:19Z  dslausb
 * Initial revision
 * Revision 1.25  2009/04/01 19:20:56Z  spriusb
 * IT9100: spiral 3.5 changes to help mitigate return pump halls vs. encoder and improve overall logging
 * Revision 1.24  2008/12/20 01:59:52Z  spriusb
 * Fix for IT 6514: using non-accumulated halls for safety pump checks
 * Revision 1.23  2007/10/17 22:39:35Z  spriusb
 * Rolling back dRBC auto-split changes
 * Revision 1.22  2007/09/19 18:01:39Z  spriusb
 * Fix for IT8210: Various refactors to Container classes in safety model
 * Revision 1.21  2007/08/30 20:06:17Z  spriusb
 * Enhancements to support dRBC auto-split cassettes
 * Revision 1.20  2003/07/21 16:38:09Z  td07711
 * IT6157 - remove redundant log msg
 * Revision 1.19  2003/07/01 22:34:29Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.18  2003/04/03 23:30:20Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.17  2003/01/23 21:48:02Z  pn02526
 * Remove "datalog_delete" comments.  Improve some logging.
 * Revision 1.16  2002/12/26 23:49:47Z  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.15  2002/08/07 11:57:55  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Revision 1.14  2002/07/23 16:52:18  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.13  2002/04/23 17:51:49  pn02526
 * Clean up "eString" definitions left over after DataLogLevel class revisions.
 * Revision 1.12  2002/04/15 15:11:02  pn02526
 * Change DumpBaseContainer() to use DataLog Level LOG_ALARM_DETAIL rahter than LOG_SAFETY.
 * Revision 1.11  2002/04/15 09:55:54  pn02526
 * Change log(LOG_ALARM_DETAIL) to alarm_detail(LOG_ALARM_DETAIL)
 * Revision 1.10  2002/04/05 16:39:50  pn02526
 * Revise logging to use DataLogLevel class.  Revise FATAL_ERROR calls to use the new one.  Change ASSERT() macro calls to trima_assert().
 * Revision 1.9  2000/07/20 13:32:13  bs04481
 * Change variable name to be more representative
 * Revision 1.8  2000/06/30 21:29:13  bs04481
 * Re-did valve monitoring to not monitor by draw and return cycles
 * but to alarm anytime there is a valve out of position for 20
 * consecutive seconds while the associated pump is running.
 * Revision 1.7  2000/06/22 16:46:42  bs04481
 * Plumbing changes for single-stage.  1) Change to RBC flow.
 * 2) Add valve position monitoring during Blood Run and Blood
 * Rinseback. 3)  Add compile flag to support dual-stage
 * Revision 1.6  2000/05/26 17:11:58  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.5  1999/08/01 20:53:14  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.4  1999/04/06 18:32:49  BS04481
 * Remove default of set type to white when unknown.  If set type is
 * not known in states from Donor Connect to Rinseback, inclusive,
 * and any pumps run, alarm.  Default to white if unknown in Donor
 * Disconnect
 * Revision 1.3  1998/11/05 19:28:30  bs04481
 * Correct usage of ASSERT.  Code review change.
 * Revision 1.2  1998/06/24 15:19:08  bs04481
 * Changes to make pump stroke volumes dependent on set type.
 * Change necessary for DRBC.
 * Revision 1.1  1998/05/12 19:49:56  bs04481
 * Initial revision
 *
 *
 * TITLE:      s_cont.hpp, a container
 *
 * ABSTRACT:   Safety's view of a container.  A container is a bag having
 *             volume and some combination of pumps and valves to control flow.
 *             A container understands the monitoring types that go on
 *             in the Safety System.
 *
 *
 */

#include "trima_datalog.h"
#include "hw.h"
#include "chw.hpp"
#include "shw.hpp"
#include "error.h"
#include "s_set.hpp"
#include "s_bag.hpp"
#include "time.h"

#ifndef S_CONT_HPP  //  Prevents multiple compilations.
#define S_CONT_HPP

enum MON_TYPE
{
   MON_INACTIVE            = 0,
   MON_FIRST_DRAW          = 1,
   MON_RUN_CYCLES          = 2,
   MON_RINSEBACK           = 5,
   MON_FIRST_DRAW_COMPLETE = 8,
   MON_DISCONNECT          = 10
};

enum VOL_TYPE
{
   VOL_DELTA                 = 1,
   VOL_ACCUM                 = 2,
   VOL_DRAW_CYCLE            = 3,
   VOL_RETURN_CYCLE          = 4,
   VOL_DRAW_CYCLE_INFUSION   = 5,
   VOL_RETURN_CYCLE_INFUSION = 6
};

DataLog_Stream& operator << (DataLog_Stream& os, MON_TYPE monType);
DataLog_Stream& operator << (DataLog_Stream& os, FLOW_DIRECTION flowIO);
DataLog_Stream& operator << (DataLog_Stream& os, HW_VALVE_STATES valveIO);
DataLog_Stream& operator << (DataLog_Stream& os, HW_CASSETTE_STATES position);

/*******************************************************************************
*
*  baseContainer CLASS DEFINITION
*
******************************************************************************/
class baseContainer
{
public:
   char         _containerName[MAX_CHAR];
   ALARM_VALUES _minimumAlarm;
   ALARM_VALUES _maximumAlarm;
   bool         _pumpErrorLogFlag;
   MON_TYPE     _monType;
   baseContainer()
   {
      _monType          = MON_INACTIVE;
      _pumpErrorLogFlag = FALSE;
      strcpy(_containerName, "Unknown Container");
      _minimumAlarm     = NULL_ALARM;
      _maximumAlarm     = NULL_ALARM;
   }
   virtual ~baseContainer() {}

   /** Interface for advance Container behavior **/

   // calculate the volumes
   virtual void UpdateContainer (char cCycle) = 0;

   // resets the cycle volume
   virtual void ResetCycleVol (char cCycle) = 0;

   // return the requested volume
   virtual float GetVolume (VOL_TYPE type) const = 0;

   // output container state
   virtual void DumpContainer (DataLog_Level& log_level) const = 0;

   /** Simple accessors **/

   // set the name of the container
   void SetName (const char* name)
   {
      if (strlen(name) < MAX_CHAR)
         strcpy(_containerName, name);
      else
         strcpy(_containerName, "Unknown Container");
   }

   // set the minimum volume exceeded alarm
   void SetMinimumAlarm (ALARM_VALUES alarm) { _minimumAlarm = alarm; }

   // set the maximum volume exceeded alarm
   void SetMaximumAlarm (ALARM_VALUES alarm) { _maximumAlarm = alarm; }

   // set the monitoring type
   void SetMonitoringType (MON_TYPE cCycleMon) { _monType = cCycleMon; }

protected:
   virtual void SaveToPFR () = 0;
};


/*******************************************************************************
*
*  DefaultContainer CLASS DEFINITION
*
******************************************************************************/
class DefaultContainer
   : public baseContainer
{
public:
   bag* _bag;

   DefaultContainer()
      : baseContainer(), _bag(NULL) {}
   virtual ~DefaultContainer() { _bag = NULL; }

   //
   // Base Container Interface
   //
   virtual void  UpdateContainer (char cCycle);
   virtual void  ResetCycleVol (char cCycle);
   virtual float GetVolume (VOL_TYPE type) const;
   virtual void  DumpContainer (DataLog_Level& log_level) const;
};

/*******************************************************************************
 *
 *  InletContainer CLASS DEFINITION
 *    Will be combined with the ReturnContainer to make a Donor
 *
 ******************************************************************************/
class InletContainer
   : public DefaultContainer
{
protected:
   virtual void SaveToPFR ();

public:
   InletContainer();
   virtual ~InletContainer();
   void SetSetType (SET_TYPE setType);
   void InitializeForState (long newState);
   void UpdateContainerControl (PumpStatus InletPump, timespec timestamp, short accessPressure);
   void UpdateContainerSafety (long InletHalls);
   void RestoreFromPFR ();
};

/*******************************************************************************
 *
 *  ReturnContainer CLASS DEFINITION
 *    Will be combined with the InletContainer to make a Donor
 *
 ******************************************************************************/
class ReturnContainer
   : public DefaultContainer
{
protected:
   virtual void SaveToPFR ();

public:
   float _fReturnVolumeAwayFromDonor;
   float _fReturnPrimeVolume;
   ReturnContainer();
   virtual ~ReturnContainer();
   void SetSetType (SET_TYPE setType);
   void InitializeForState (long newState);
   void UpdateContainerControl (PumpStatus ReturnPump, timespec timestamp);
   void UpdateContainerSafety (long ReturnHalls);
   void RestoreFromPFR ();
};

/*******************************************************************************
 *
 *  ACContainer CLASS DEFINITION
 *
 ******************************************************************************/
class ACContainer
   : public DefaultContainer
{
protected:
   virtual void SaveToPFR ();

public:
   float _fACPrimeVolume;
   ACContainer();
   virtual ~ACContainer();
   void SetSetType (SET_TYPE setType);
   void InitializeForState (long newState);
   void UpdateContainerControl (PumpStatus ACPump, timespec timestamp);
   void UpdateContainerSafety (long ACHalls);
   void RestoreFromPFR ();
};

/*******************************************************************************
*
*  PlateletContainer CLASS DEFINITION
*
******************************************************************************/
class PlateletContainer
   : public DefaultContainer
{
protected:
   virtual void SaveToPFR ();

public:
   PlateletContainer();
   virtual ~PlateletContainer();
   void SetSetType (SET_TYPE setType);
   void InitializeForState (long newState);
   void UpdateContainerControl (PumpStatus PlateletPump, timespec timestamp, HW_VALVE_STATES PlateletValveControl);
   void UpdateContainerSafety (long PlateletHalls, HW_VALVE_STATES PlateletValveSafety);
   void RestoreFromPFR ();
   void updateValveToPFR ();
   void restoreValveFromPFR ();
};

/*******************************************************************************
*
*  PlasmaContainer CLASS DEFINITION
*
******************************************************************************/
class PlasmaContainer
   : public DefaultContainer
{
protected:
   virtual void SaveToPFR ();

public:
   PlasmaContainer();
   virtual ~PlasmaContainer();
   void SetSetType (SET_TYPE setType);
   void InitializeForState (long newState);
   void UpdateContainerControl (PumpStatus PlasmaPump, timespec timestamp, HW_VALVE_STATES PlasmaValveControl);
   void UpdateContainerSafety (long PlasmaHalls, HW_VALVE_STATES PlasmaValveSafety);
   void RestoreFromPFR ();
   void updateValveToPFR ();
   void restoreValveFromPFR ();
};

/*******************************************************************************
*
*  RBCContainer CLASS DEFINITION
*
* We differentiate the RBC Container from all other derived implementations
* due to it's specialized bag and "pseudo-pump" members, and special behaviors
* to support dRBC automated split disposables.
******************************************************************************/
class RBCContainer
   : public baseContainer
{
protected:
   virtual void SaveToPFR ();

public:
   rbcBag* _bag;

   RBCContainer();
   virtual ~RBCContainer();

   void SetSetType (SET_TYPE setType);
   void InitializeForState (long newState);
   void UpdateContainerControl (HW_VALVE_STATES RBCValveControl, HW_VALVE_STATES PlasmaValveControl, HW_VALVE_STATES PlateletValveControl);
   void UpdateContainerSafety (HW_VALVE_STATES RBCValveSafety);
   void RestoreFromPFR ();
   void updateValveToPFR ();
   void restoreValveFromPFR ();

   //
   // Base Container Interface
   //
   void  UpdateContainer (char cCycle);
   void  ResetCycleVol (char cCycle);
   float GetVolume (VOL_TYPE type) const;
   void  DumpContainer (DataLog_Level& log_level) const;
};



#endif // end of S_CONT_HPP

/* FORMAT HASH 8992066a01ba10288292ce0f1be8743b */
