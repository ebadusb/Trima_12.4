/*
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeRbc_Monitor.h
 *
 */


#ifndef __PRIME_RBC_MONITIOR_H_
#define __PRIME_RBC_MONITIOR_H_

#include "monitorbase.h"
#include "objdictionary.h"
#include "an_alarm.h"

#include "aPQMonitor.h"


class PrimeRBC_Monitor
   : public aPQMonitor
{
   DECLARE_OBJ(PrimeRBC_Monitor);
public:

   PrimeRBC_Monitor();
   virtual ~PrimeRBC_Monitor();

   virtual void Monitor ();

protected:

   virtual void enable ();
   virtual void disable ();
   void         check (const float& ratio);
   void         average (const float& ratio) ;

protected:
   float _avgRatio;
   float _sumRatios;
   float _numRatios;
   bool  _avgComputed; // the average is ready
   bool  _detected;    // an event has been detected
   float _hits;        // number of samples over limit
   bool  _logOnce;     // for verification purposes

private:



};

#endif

/* FORMAT HASH c64c89503dce3e83c19669399d41acd0 */
