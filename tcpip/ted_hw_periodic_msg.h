/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      tedif.h
 *             Trima External Diagnostic Interface
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the interface between Trima and EMS.
 *
 *******************************************************************************/


#ifndef __TED_HW_PERIODIC_MSG__
#define __TED_HW_PERIODIC_MSG__

#include "sockinet.h"

#include "tedif.h"
#include "ted_message.h"

#include "timermessage.h"



class TedHwPeriodicStatus
{
// Methods
public:
   TedHwPeriodicStatus(TED_BoardType board);
   virtual ~TedHwPeriodicStatus();

   void set (const sockinetaddr& addr, unsigned int period);
   bool isSet () const { return (_client.getport() != 0); }

   void deactivate ();

private:
   // Don't allow
   TedHwPeriodicStatus();
   TedHwPeriodicStatus& operator = (const TedHwPeriodicStatus& rhs);
   TedHwPeriodicStatus(const TedHwPeriodicStatus&);

// Methods
private:
   // Periodic Timer callback.
   void sendControlPeriodicStatus (void);
   void sendSafetyPeriodicStatus (void);

   // Hw Status message handlers.
   void handleControlHwStatusMsg (void);
   void handleSafetyHwStatusMsg (void);

// Members
private:
   unsigned int  _period;
   TED_BoardType _board;

   TimerMessage  _tm;

   // Send-to client
   sockinetaddr _client;

   // Status messages sent.
   TED_ControlHwStatusMsg _controlStatus;
   TED_SafetyHwStatusMsg  _safetyStatus;

   // Status messages received
   Message<CHwStates>                     _controlHwStatesMsg;
   Message<SHwStates>                     _safetyHwStatesMsg;

   CHwStates                              _controlHwStates;
   SHwStates                              _safetyHwStates;

   TedMessageObj<TED_ControlHwStatusMsg>* _controlStatusMsg;
   TedMessageObj<TED_SafetyHwStatusMsg>*  _safetyStatusMsg;

   bool _noControlData;
   bool _noSafetyData;
   bool _registeredForMessages;
};

#endif

/* FORMAT HASH 982a700b6f27f02697f10f146df54442 */
