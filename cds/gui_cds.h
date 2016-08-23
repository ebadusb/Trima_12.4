#ifndef __GUI_CDS
#define __GUI_CDS

/*******************************************************************************
 *
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      gui_cds.h
 *             GUI Common Data Store
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the Common Data Store for GUI
 *
 * HISTORY:    $Log: gui_cds.h $
 * HISTORY:    Revision 1.2  2004/02/17 22:47:58Z  rm70006
 * HISTORY:    IT 6868.  Add new variable for tracking procedure number.
 * HISTORY:    Revision 1.1  2003/04/28 15:57:51Z  rm70006
 * HISTORY:    Initial revision
 *
*******************************************************************************/

#include "datastore.h"

class Gui_CDS
   : public MultWriteDataStore
{
// Data Members
public:
   BaseElement<unsigned int> cfr11SequenceNumber;    // CFR11 sequence number
   BaseElement<int>          cfr11ProcedureNumber;   // CFR11 procedure number

// Class Methods
public:
   virtual ~Gui_CDS(){}

   Gui_CDS(Role role)
      : MultWriteDataStore("Gui_CDS", role)
   {
      cfr11SequenceNumber.Register (this, PFR_RECOVER, 0);
      cfr11ProcedureNumber.Register (this, PFR_RECOVER, 0);
   }

   // Class Methods
private:
   Gui_CDS();  // Base Constructor not available

};

#endif

/* FORMAT HASH 3573fe281a7d3ceb98afddda92979abe */
