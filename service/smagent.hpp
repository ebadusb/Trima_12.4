/*
 * Copyright (C) 2007 Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      smagent.hpp
 *             Service Mode Agent process
 *
 * AUTHOR:     Shawn Price
 *
 * ABSTRACT:   This file defines the class/task responsible for control-side processing
 *             under Service Mode.  Primary responsibilities for SMAgent are 1) instantiate
 *             and provide the process context for the hardware abstraction layer (HAL), and
 *             2) effect periodic logging.  In both regards, SMAgent acts as a lean
 *             surrogate for Proc.
 */

#ifndef _SMAGENT_INCLUDE
#define _SMAGENT_INCLUDE

#include "alarms.h"
#include "hal.h"          // For Hardware Abstraction Layer (HAL)
#include "chw.hpp"        // Gets Control HW Orders
#include "shw.hpp"        // Gets Safety HW Orders
#include "tcorders.hpp"   // Gets TED Control Orders Message
#include "smdata.hpp"

class SMAgent
{
public:
   SMAgent();

   virtual ~SMAgent();

   bool Initialize ();

private:
   // Control status interface
   void HandleHalStatusMsg ();

   // Control orders interface
   void HandleControlOrdersMsg ();

private:
   //
   // HAL interface
   CHal                _HAL;

   TedControlOrdersMsg _smControlOrdersMsg;

   SMData              data;
};
#endif  // _SMAGENT_INCLUDE

/* FORMAT HASH f23e93f55743e4c5a18d4e65ab39ec53 */
