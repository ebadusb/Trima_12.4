/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 *
 * ABSTRACT:   This file contains the definitions for AlarmStruct, which
 *             collects fields and values concerning alarms; and the AlarmTable,
 *             which is the global dictionary for alarms.
 *
 */

/**********************************************************************************
 NOTE: This file is now shared with Vista. Take care when adding new dependencies
 **********************************************************************************/

#ifndef ALARMCONTAINER_H
#define ALARMCONTAINER_H

#include <string>
#include <map>
#include "alarms.h"
#include "sounds.hpp"
#include "semLib.h"

class TimerMessage;

class AlarmStruct
{
private:
   ALARM_VALUES        alarm_name;         // Name by which the alarm is associated.
   ALARM_STATES        alarm_state;        // Current state of the alarm.
   ALARM_TYPES         alarm_type;         // Type of alarm A2, A1.....
   ALARM_ACTIONS       alarm_action;       // Action to take when needed.
   ALARM_DISPLAY_TYPES alarm_display_type; // Display level of alarm.
   SOUND_CODE          alarm_sound;        // Sound for this alarm to make
   std::string         alarm_description;  // Description of alarm (goes to log file)

   TimerMessage*       _InactionTimer;
   bool                _Activated;

public:
   AlarmStruct();

   AlarmStruct(ALARM_VALUES new_name,
               ALARM_TYPES new_type,
               ALARM_ACTIONS new_action,
               ALARM_DISPLAY_TYPES new_display_type,
               SOUND_CODE new_alarm_sound,
               std::string const& new_description);

   AlarmStruct(const AlarmStruct& a);

   AlarmStruct& operator = (const AlarmStruct&);


   ALARM_ACTIONS       getAction      () const { return alarm_action; }
   ALARM_VALUES        getName        () const { return alarm_name; }
   ALARM_STATES        getState       () const { return alarm_state; }
   ALARM_TYPES         getType        () const { return alarm_type; }
   SOUND_CODE          getSound       () const { return alarm_sound; }
   ALARM_DISPLAY_TYPES getDisplayType () const { return alarm_display_type; }
   const char*         getDescription () const { return alarm_description.c_str(); }

   inline void setState (ALARM_STATES new_state) { alarm_state = new_state; }

   virtual ~AlarmStruct();

   inline bool activated () { return _Activated; }

   void inactivate (int time = 2000 /*milliseconds = 2 seconds */) { _Activated = false; setTimer(time); }
   void reactivate () { _Activated = true; setTimer(0); }

protected:

   void setTimer (int time /* in milliseconds */);
};


////////////////////////////////////////////////////////////////////////////////
//
//  AlarmTable serves two functions: first, as an easy way to access a global
//  data structure; second, as a 'safe wrapper' around that data structure,
//  which is just a std::map.  The interface is largely identical to std::map,
//  except that...

class AlarmTable
{
private:

   typedef std::map<ALARM_VALUES, AlarmStruct> AlarmMap_t;

   AlarmTable(const AlarmTable&);              // unimplemented, noncopyable
   AlarmTable& operator = (const AlarmTable&); // unimplemented, nonassignable

public:

   AlarmTable();

   ~AlarmTable() {}

   // Used to load table
   static void add (ALARM_VALUES value,
                    ALARM_TYPES type,
                    ALARM_ACTIONS action,
                    ALARM_DISPLAY_TYPES display,
                    SOUND_CODE sound,
                    const char* description);

   // std::map interface matching...
   typedef AlarmMap_t::iterator iterator;

   iterator begin () { return _alarmMap.begin(); }
   iterator end  () { return _alarmMap.end()  ; }

   // Safe lookup: operator[] is guaranteed to find your alarm.  If it doesn't, that means there has
   // been a programming error, and the system exits with an A2 alarm.
   AlarmStruct& operator [] (ALARM_VALUES);

   iterator find (ALARM_VALUES key) { return _alarmMap.find(key); }


   // Convenience function: find the first, or the next, non-cleared alarm in the table.
   iterator findFirstNotCleared ();

private:

   // The actual singleton data structure, shared by all AlarmTable objects; initialized during the
   // first call to AlarmTable's ctor.
   static AlarmMap_t _alarmMap;

   // Since multiple tasks will have AlarmTable objects, we need to arrange for the data structure
   // loading to be in a mutex-protected critical section.  The boolean _init avoids mucking with
   // mutexes when it isn't absolutely necessary.
   static bool   _init;          // true when loaded
   static SEM_ID _loadMSem;      // mutex for loading
};


#endif

/* FORMAT HASH 3fab23b2bb8272b79489db2399d684d8 */
