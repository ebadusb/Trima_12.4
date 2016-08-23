/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * adjustctrl.h
 *
 *        This object manages the interaction between the GUI adjust
 *        screen and PROC.
 *
 ******************************************************************************/

#ifndef _INCLUDE_ADJUSTCTRL_
#define _INCLUDE_ADJUSTCTRL_

#include "adjust.h"
#include "proctrimaset_cds.h"
#include "procadjust_cds.h"
#include "trimamessages.h"
#include "timermessage.h"

class AdjustCtrl
{
public:

   AdjustCtrl();
   virtual ~AdjustCtrl();

   //
   //  Object must be initialized (after Cobe configuration has been read in)
   //
   virtual void Initialize ();

   //
   // Status update called periodically by the owner.  Returns true when a CDS
   // is changed.
   //
   bool Update ();

   //
   // Set/Get button status
   //
   BUTTON_VISIBILITY_STATUS BtnSpillover  (void) const;
   void                     BtnSpillover  (BUTTON_VISIBILITY_STATUS st);
   BUTTON_VISIBILITY_STATUS BtnAirblock   (void) const;
   void                     BtnAirblock   (BUTTON_VISIBILITY_STATUS st);
   BUTTON_VISIBILITY_STATUS BtnSalinebolus (void) const;
   void                     BtnSalinebolus (BUTTON_VISIBILITY_STATUS st);

protected:

   //
   // Callback for the Request message from GUI
   void ProcessRequest (void);

   // //
   // // Send the repredict message ...
   // virtual void SendMsg();

   //
   // Send the adjust status update to GUI
   void SendAdjustStatus ();

   //
   // Increment the passed in Adjustment by val
   void IncAdjustment (ProcAdjustBar_CDS& cds, float val);

   // update these values before each send.
   bool SetQin     (const bool active, float current);
   bool SetQreturn (const bool active, float current);
   bool SetInfusion (const bool active, float current);
   bool SetRatio   (const bool active, float current);


protected:

   ProcedureAdjustmentMsg       _RequestMsg;         // GUI request for adjust info
   ProcedureAdjustmentStatusMsg _StatusMsg;          // Update adjust info to GUI

   TimerMessage                 _UpdateTimer;        // Timer used for periodically updating GUI status

   ProcTrimaSet_CDS::CassetteStateType               // Cassette state on last update
        _LastCassetteState;                          //  cycle.  Send update on change

   int  _ChangeCount;                                // Count of changes used to test for changes during
                                                     //  the current update cycle
   bool _AlarmActivePrevious;                        // true if an alarm was active last time through
   bool _MaxQinMonitoringActive;                     // true if we are capturing max draw flow

   int  _currentAdjustment;                         // set to the serial number
                                                    // of the current adjustment
                                                    // from the trouble screen

};


#endif

/* FORMAT HASH 6c1c6178039a69820dea27ad4571cc33 */
