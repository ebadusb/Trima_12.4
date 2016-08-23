/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      wbcchamberpurge.h
 *
 */

#ifndef _PURGECALCS_H_
#define _PURGECALCS_H_

class PurgeCalcs
{
private:

   //  at the moment there is only one static function so no need for these
   //  trivial functions
   PurgeCalcs();
   virtual ~PurgeCalcs();
   PurgeCalcs(const PurgeCalcs& rec);


public:
   //
   // Function to calculate the number of purge cycles to clear the
   //  chamber.  This is used by both predict and the WBC chamber purge routine.
   //
   static int calculateNumCycles (bool hyperconcentration, float vinTarget, float VPurgeTrigger);

};
#endif

/* FORMAT HASH 14670ca0c94dd9d3911a9c847b2f1eae */
