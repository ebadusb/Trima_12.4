/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      meteredStorage.h
 *
 *   This class is the master state for all of Metered Storage.
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/meteredStorage.h 1.5 2006/12/04 16:16:21Z jheiusb Exp jheiusb $
$Log: meteredStorage.h $
Revision 1.5  2006/12/04 16:16:21Z  jheiusb
remerge from original rbc+plt mss 5.2
Revision 1.4  2006/07/21 16:23:09Z  jheiusb
7662 -- remove alarm
Revision 1.3  2006/03/03 21:22:06Z  jheiusb
Revision 1.2  2006/03/03 17:31:16Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:16:15Z  jheiusb
Initial revision
Revision 1.7  2006/01/04 21:07:01Z  jheiusb
7330 -- fix ACTotal to not include the SS used during mss
Revision 1.6  2005/12/19 23:25:32Z  dslausb
IT 7301 - Fixed override default bag volume so that it actually writes the override value to CDS like it's supposed to.
Revision 1.5  2005/10/25 20:57:30Z  jheiusb
7202 -- initial PLT mss path
Revision 1.4  2005/10/07 22:06:14Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.3  2005/10/05 20:34:57Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

#ifndef METERED_STORAGE_H  //  Prevents multiple inclusions.
#define METERED_STORAGE_H

#include <stateabs.h>
#include <an_alarm.h>
#include "trimamessages.h"

// Class Declaration
class MeteredStorage
   : public StateAbs
{
   DECLARE_STATE(MeteredStorage);
public:

   // Default constructor
   MeteredStorage();

   // Copy constructor
   MeteredStorage(const MeteredStorage& state);

   // Default destructor
   virtual ~MeteredStorage();

   virtual StateAbs* clone () const { return ( new MeteredStorage(*this)); }

protected:

   //
   // Functions to do this states processing ...
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Function called upon first entrance to this state
   //
   virtual int preEnter ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const MeteredStorage& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   MSSWriteBagVolRBC*      ptr_writeBagVolRBC;
   MSSWriteBagVolPLT*      ptr_writeBagVolPLT;
   PartialCancellationMsg* ptr_partialCancelMSS;

   void overwriteSSBagVolRBC (void* data);
   void overwriteSSBagVolPLT (void* data);
   void partialMeteringCancel (void* data);


protected:

   // save the initial AC volm count so we can restore it for endrun sum
   // MSS also uses the ACTotal
   float initialACVolmTotal;

   bool  callbackSetUp; // have we set up the callback yet?
};

#endif

/* FORMAT HASH e913c8d7168e3b47bc3537065af08ba5 */
