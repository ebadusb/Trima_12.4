/*******************************************************************************
 *
 * Copyright (c) 2003 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      cfr_list.h
 *             CFR11 list
 *             audit tracking list structure for CFR11
 *
 * ABSTRACT:   This file provides the data structure for the audit tracking list.
 *
 ******************************************************************************/

// Single include define guardian
#ifndef CFRLIST_HPP
#define CFRLIST_HPP

#include <list>

#include "guistring.h"
#include "bio_signatures.h"
#include "trima_datalog.h"


typedef struct CFR11_Tracking_List
{
   long                    key;
   guistring               btn_text;
   bool                    selected;
   AuthorizationStatusType authStatus;
   ComplianceTriggerType   trigger;

   CFR11_Tracking_List()
      : key(-1), selected(false), authStatus(BIO_AUTH_NONE), trigger(TRIGGER_NONE) {}
};


typedef list<CFR11_Tracking_List> CFR_LIST;

void dumpCfrList (const char* file, const int line);

// End of single include define guardian
#endif

/* FORMAT HASH d19022b62c3916c336ca5946dd15d5e2 */
