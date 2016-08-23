/*******************************************************************************
 *
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      gooddog.h
 *             Good Dog process
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file implements the good dog task.  It allows EMS to issue hw commands
 *             while Trima is in Service Mode
 *
 * REVISION HISTORY:
 * $Header$
 *
 */

#ifndef GOODDOG_H
#define GOODDOG_H

#include "chw.hpp"         // Gets Control HW Orders
#include "shw.hpp"         // Gets Safety HW Orders
#include "message.h"       // Gets Message System message object
#include "timermessage.h"  // Gets Timer Message
#include "tcorders.hpp"
#include "ostime.hpp"


class GoodDog
{
public:
   GoodDog();

   virtual ~GoodDog();

   bool Initialize ();

private:
   typedef struct
   {
      float inletRevsControl;
      float acRevsControl;
      float plateletRevsControl;
      float plasmaRevsControl;
      float returnRevsControl;
      long  inletRevsSafety;
      long  acRevsSafety;
      long  plateletRevsSafety;
      long  plasmaRevsSafety;
      long  returnRevsSafety;
   } PumpDeltaRevs;

   void handleSafetyServiceModeMsg ();

   void HandleControlHwStatusMsg ();
   void HandleSafetyHwStatusMsg ();

   void HandleOrdersTimerMsg ();

   void HandleSafetyOrdersMsg ();
   void HandleControlOrdersMsg ();

   void HandleStopGoodDogMsg ();

   void setControlOrders (const CHwOrders& newControlHwOrders);
   void setSafetyOrders (const SHwOrders& newSafetyHwOrders);

private:
   PumpDeltaRevs _pumpDeltaRevs;
   SHwOrders     _safetyOrders;
   CHwOrders     _controlOrders;

   // Received Messages
   SafetyServiceModeMsg _safetyServiceModeMsg;

   TedControlOrdersMsg  _controlOrdersFromTed;
   TedSafetyOrdersMsg   _safetyOrdersFromTed;

   Message<CHwStates>   _controlHwStatusMsg;
   Message<SHwStates>   _safetyHwStatusMsg;

   StopGoodDogMsg       _stopGoodDogMsg;

   // Orders Timer
   TimerMessage _ordersTimer;

   rawTime      _lastOrdersSendTime;

   // Sent messages
   Message<CHwOrders> _outgoingControlOrdersMsg;
   Message<SHwOrders> _outgoingSafetyOrdersMsg;

   unsigned int       _safetyOrdersSequenceNumber;
   unsigned int       _controlOrdersSequenceNumber;
};

#endif

/* FORMAT HASH ad2b95e4024e38983b331aeac27410f9 */
