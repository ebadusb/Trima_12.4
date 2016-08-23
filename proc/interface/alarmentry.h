//
// Copyright (c) 2000 Gambro BCT Inc.   All rights reserved.
//

#ifndef _ALARMQUEUEENTRY_H_
#define _ALARMQUEUEENTRY_H_

#include "alarms.h"

class AlarmQueueEntry
{
public:

   //
   // Default constructor
   AlarmQueueEntry()
      : _AlarmName       (NULL_ALARM),
        _AlarmType       (OPERATOR_ATTENTION),
        _AlarmDisplayType(DISP_NONE),
        _AlarmAction     (NOTHING) { }

   //
   // Copy constructor
   AlarmQueueEntry(const AlarmQueueEntry& entry)
      : _AlarmName       (entry._AlarmName),
        _AlarmType       (entry._AlarmType),
        _AlarmDisplayType(entry._AlarmDisplayType),
        _AlarmAction     (entry._AlarmAction) { }


   //
   // Equal operator
   AlarmQueueEntry& operator = (const AlarmQueueEntry& entry)
   {
      _AlarmName        = entry._AlarmName;
      _AlarmType        = entry._AlarmType;
      _AlarmDisplayType = entry._AlarmDisplayType;
      _AlarmAction      = entry._AlarmAction;

      return *this;
   }

   //
   // Comparison operator
   int operator == (const AlarmQueueEntry& entry)
   {
      if (    entry._AlarmName        == _AlarmName
              && entry._AlarmType        == _AlarmType
              && entry._AlarmDisplayType == _AlarmDisplayType
              && entry._AlarmAction      == _AlarmAction )
      {
         return 1;
      }
      return 0;
   }

   //
   // Set/Get the alarm name
   void               name (const ALARM_VALUES a) { _AlarmName = a; }
   const ALARM_VALUES name () const { return _AlarmName; }

   //
   // Set/Get the alarm type
   void              type (const ALARM_TYPES t) { _AlarmType = t; }
   const ALARM_TYPES type () const { return _AlarmType; }

   //
   // Set/Get the alarm display type
   void                      displayType (const ALARM_DISPLAY_TYPES t) { _AlarmDisplayType = t; }
   const ALARM_DISPLAY_TYPES displayType () const { return _AlarmDisplayType; }

   //
   // Set/Get the alarm action
   void                action (const ALARM_ACTIONS t) { _AlarmAction = t; }
   const ALARM_ACTIONS action () const { return _AlarmAction; }

protected:

   ALARM_VALUES        _AlarmName;
   ALARM_TYPES         _AlarmType;
   ALARM_DISPLAY_TYPES _AlarmDisplayType;
   ALARM_ACTIONS       _AlarmAction;

};


#endif

/* FORMAT HASH 88e753cbe191447381f3978a3e3b7aab */
