/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      smdata.hpp
 *
 * ABSTRACT:   This file contains the declaration for the interface
 *             for the globally used Service Mode data.  It acts a
 *             surrogate for ProcData with respect to common data
 *             stores, and maintained by the Service Mode Agent, which
 *             is a surrogate for Proc.
 */

#ifndef _SM_DATA_H_
#define _SM_DATA_H_

#include <vxWorks.h>
#include <string>

#include "modaldata.hpp"
#include "config_cds.h"
#include "halstatus_cds.h"
#include "halorders.h"
#include "proctrimaset_cds.h"
#include "trimamessages.h"
#include "shw.hpp"

class AbsModalData;

class SMData
   : public AbsModalData
{
public:

   //
   // Default Constructor
   SMData();

   //
   // Default Destructor
   virtual ~SMData();

   //
   // Get operators for data ...
   Config_CDS&       Config ()                { return _TheData->_ConfigurationData; }
   HalStatus_CDS&    Status ()                { return _TheData->_StatusData; }
   ProcTrimaSet_CDS& TrimaSet ()              { return _TheData->_ROTrimaSetData; }
   CHalOrders&       Orders ()                { return _TheData->_Orders; }

   //
   // These functions change the permissions on our writable datastores to
   //  allow one and only one user to change the datastore ...
   // void                    MakeTrimaSetReadable();

public:
   //
   // AbsModalData inteface implementations
   //
   virtual void definePeriodicSet (DataLog_SetHandle pset);

   virtual void setPeriodicLogging (bool enabled) { m_periodicLoggingEnabled = enabled; }

private:

   class SMDataStruct
   {
   public:
      SMDataStruct();

      //
      // Read only common data stores...
      Config_CDS       _ConfigurationData;
      HalStatus_CDS    _StatusData;
      ProcTrimaSet_CDS _ROTrimaSetData;

      //
      // HAL information
      CHalOrders _Orders;
   };

   //
   // Task wide static data pointer ...
   static SMDataStruct* _TheData;
   static unsigned long _TheOwningTaskId;

private:
   SMData(const SMData&);              // not implemented
   SMData& operator = (const SMData&); // not implemented

};
#endif  // _SM_DATA_H_

/* FORMAT HASH a93621e52eefcfcb753f14521d66aadd */
