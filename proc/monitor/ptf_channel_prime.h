/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ptf_channel_prime.h
 *
 * ABSTRACT:  This alarm checks for a bad PTF during channel setup.
 *
 */


#ifndef _PTF_CHANNEL_PRIME_TEST_MONITOR_H_
#define _PTF_CHANNEL_PRIME_TEST_MONITOR_H_

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"

class PTFChannelPrimeTest
   : public MonitorBase
{

   DECLARE_OBJ(PTFChannelPrimeTest);

public:
   PTFChannelPrimeTest();
   virtual ~PTFChannelPrimeTest();

   virtual void Monitor ();

protected:

};


#endif

/* FORMAT HASH 21a01a7a4d12e25d3215120cf08b9a56 */
