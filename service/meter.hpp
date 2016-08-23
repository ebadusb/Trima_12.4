/*******************************************************************
*    Copyright(c) 1995 - 1998 COBE BCT, Inc. All rights reserved
*
*    AUTHOR:     Barry Fulkerson
*
*    FILE NAME:  meter.hpp
*
*    DATE:       1/22/1998
*
*    DESCRIPTION:
  This class keeps track of machine ON time, centrifuge ON time and number of successful
  runs.  This classes is created when the control driver ctl_drv is spawned.  It will
  accumulate time in one minute increments.  Upon creatation, this class will read
  the prvious data from the disk and accumulate that time.

*******************************************************************/

#ifndef METER_HPP
#define METER_HPP

// Includes

#include "timermessage.h"
#include "trimamessages.h"
#include "chw.hpp"
#include "halstatus_cds.h"


class hourMeter
   : public TimerMessage
{
public:
   hourMeter();
   virtual ~hourMeter();

   void notify ();
   void registerForMessages ();

protected:
   // Received struct messages
   Message<CHwStates> _controlDataMsg;
   CHwStates          _controlData;
   STSConnectedMsg    _stsConnected;

   // Received Int messages
   ACConnectedMsg _acConnectedMsg;

   long           _trima_on_time;
   long           _centrifuge_on_time;
   long           _no_of_successful_runs;

   HalStatus_CDS* _halStatusCds;

protected:
   void updateMeterFile ();
   void handleHardwareStatesMsg ();
   void handleAcConnectedMsg ();
   void handleSTSConnectedMsg ();
   void logMeter ();
};

#endif
////////////////////////////////////////////////////////////////////////////////
// end of meter.hpp

/* FORMAT HASH 0b3da5c45d78ac26bacec516817bc2a8 */
