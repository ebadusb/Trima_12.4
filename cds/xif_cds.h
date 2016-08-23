#ifndef __EXTERNAL_IF_CDS
#define __EXTERNAL_IF_CDS

/*******************************************************************************
 *
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      xif_cds.h
 *             External Interfaces Common Data Store
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the Common Data Store for the External Interfaces
 *             like Vista and Trap.
 *
 * HISTORY:    $Log: xif_cds.h $
 * HISTORY:    Revision 1.12  2009/03/17 18:46:15Z  jsylusb
 * HISTORY:    Updated the cds variable in order to be more generic
 * HISTORY:    Revision 1.11  2009/03/13 20:53:47Z  dslausb
 * HISTORY:    IT 8335 - Only show run summary barcode screen if vista has at some point been connected
 * HISTORY:    Revision 1.10  2003/11/05 19:55:33Z  rm70006
 * HISTORY:    IT 6572.  Added flag for when T1 tests complete.
 * HISTORY:    Revision 1.9  2003/08/14 14:47:49Z  rm70006
 * HISTORY:    IT 6285.  Set PFR variables for Vista Interface.
 * HISTORY:    Revision 1.8  2003/05/22 18:52:43Z  rm70006
 * HISTORY:    IT 5733.
 * HISTORY:
 * HISTORY:    Add new field for tracking when Vista status process is up.
 * HISTORY:    Revision 1.7  2003/01/30 17:59:16Z  rm70006
 * HISTORY:    Save off first run sequence number
 * HISTORY:    Revision 1.6  2003/01/02 23:05:32Z  rm70006
 * HISTORY:    Change variable name to be more readible in usage.
 * HISTORY:    Revision 1.5  2003/01/02 16:35:42Z  rm70006
 * HISTORY:    Changed int types to bool.
 * HISTORY:    Revision 1.4  2002/10/18 20:01:55Z  rm70006
 * HISTORY:    Changed register call to remove role.
 * HISTORY:    Revision 1.3  2002/09/03 14:37:48Z  rm70006
 * HISTORY:    Changed datastore's to derive from new cds class.
 * HISTORY:    Revision 1.2  2002/08/09 21:06:15Z  rm70006
 * HISTORY:    Updated to use new CDS convention.
 * HISTORY:    Revision 1.1  2002/06/07 14:58:59Z  rm70006
 * HISTORY:    Initial revision
 *
*******************************************************************************/

#include "datastore.h"

#include <string>

enum ACCESS_TYPE
{
   XIF_CASSETTE_LOWERED = 1,
   XIF_DONOR_COMMITTED  = 2,
   XIF_DONOR_DOWNLOADED = 4
};


class External_IF_CDS
   : public MultWriteDataStore
{
// Data Members
public:
   BaseElement<bool>         vistaStatusReady;  // true when Vista Status process is ready for commands.
   BaseElement<bool>         vistaConnected;    // true when Vista Client is connected (used by GUI to shortcircuit vista wait time)
   BaseElement<bool>         vistaHasConnected; // true if Vista has connected at some point during the run
   BaseElement<bool>         vistaInControl;    // true if Trima is in 'Vista' mode (i.e., Vista can download donor)
   BaseElement<bool>         pfrComplete;       // true if PFR has completed
   BaseElement<bool>         broadcastComplete; // true if broadcast has completed
   BaseElement<bool>         loadSetReady;      // true if the set is ready to load

   BaseElement<bool>         cassetteLowered; // true if the cassette is lowered.
   BaseElement<bool>         donorCommitted;  // true if the donor is committed.
   BaseElement<bool>         donorDownloaded; // true if donor has been downloaded

   BaseElement<bool>         safetyStarted; // true when Safety is ready for commands

   BaseElement<unsigned int> replyToAddress;

   BaseElement<int>          firstSequenceNumber; // Sequence Number of the originating run (before PFRs)

// Class Methods
public:
   virtual ~External_IF_CDS(){}

   External_IF_CDS(Role role);

   // Class Methods

   // This is used for many things.
   //    It is used by VIP to restrict certain commands.
   //    It is used by Trap to terminate a connection
   //    It is used by Vista to mark that a run will continue on a PFR.
   //
   // Returns true if access is restricted based on access type.
   // Use as bit field for multiple restrictions
   //
   bool restrictedAccess (unsigned int access) const;

private:
   External_IF_CDS();  // Base Constructor not available

};

#endif

/* FORMAT HASH 85aa46f6950f89fce680d3fa74a0975c */
