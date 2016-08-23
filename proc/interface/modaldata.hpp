/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      modaldata.hpp
 *
 * ABSTRACT:   This file contains the declaration for the abstract base
 *             class for mode-specific aggregate data -
 *             SMData for service mode and ProcData for normal
 *             mode - aggregating Trima common data elements for processing
 *             and periodic logging.  The periodic logging class will
 *             use this interface to identify the type of data class
 *             when creating the class-specific periodic set, and to
 *             deligate responsibility for filling out set elements.
 */

#ifndef _IMODAL_DATA_H
#define _IMODAL_DATA_H

#include "trima_datalog.h"

class AbsModalData
{
public:

   enum ModalDataType
   {
      SMDATA,       // Service Mode data
      NMDATA,       // Normal Mode data
      UNDEFINED     // Undefined (default)
   };

public:
   virtual ~AbsModalData() {}

   ModalDataType getType () { return m_dataType; }

   bool isPeriodicLoggingEnabled () { return m_periodicLoggingEnabled; }

   //
   // Interfaces for class-specific periodic logging set creation.
   virtual void definePeriodicSet (DataLog_SetHandle pset) = 0;

   // Define overrideable functions with default (null) behavior
   virtual void defineEORSet (DataLog_SetHandle set) {}


protected:
   AbsModalData()
      : m_dataType(AbsModalData::UNDEFINED), m_periodicLoggingEnabled(false) {}
   AbsModalData(ModalDataType type)
      : m_dataType(type), m_periodicLoggingEnabled(true) {}

   ModalDataType m_dataType;

   bool          m_periodicLoggingEnabled;
};

#endif     // _IMODAL_DATA_H

/* FORMAT HASH 1d6d0eb5f7aa61f6b27d3eb755384e10 */
