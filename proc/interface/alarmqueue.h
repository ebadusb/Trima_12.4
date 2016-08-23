//
// Copyright (c) 2000 Gambro BCT Inc.   All rights reserved.
//

#ifndef _ALARMQUEUE_H_
#define _ALARMQUEUE_H_

#include "alarms.h"
#include "alarmentry.h"
#include <list>
#include "message.h"

class AlarmQueue
{
public:

   //
   // Default constructor
   AlarmQueue()
      : _Queue(), _locked(false) { }

   //
   // Default destructor
   virtual ~AlarmQueue() { cleanup(); }

   //
   // Equal operator
   AlarmQueue& operator = (const AlarmQueue& queue)
   {
      if ( &queue != this )
      {
         _Queue.erase(_Queue.begin(), _Queue.end() );
         std::list< AlarmQueueEntry* >::iterator entry;
         for ( entry = queue.alarmQ().begin() ;
               entry != queue.alarmQ().end()  ;
               entry++ )
            _Queue.push_back(new AlarmQueueEntry(*(*entry) ) );

         _locked = queue._locked;
      }

      return *this;
   }

   //
   // Add entry
   size_t add (const AlarmQueueEntry& e)
   {
      if ( !_locked )
      {
         remove(e.name() );
         _Queue.push_back(new AlarmQueueEntry(e) );
      }

      return _Queue.size();
   }

   //
   // Remove entry
   void remove (const ALARM_VALUES a)
   {
      if ( !_locked )
      {
         std::list< AlarmQueueEntry* >::iterator entry;
         for ( entry = _Queue.begin() ;
               entry != _Queue.end()  ;
               entry++ )
         {
            if ( (*entry)->name() == a )
            {
               std::list< AlarmQueueEntry* >::iterator next = entry;
               next++;
               _Queue.erase(entry);
               entry = next;
            }
         }
      }
   }

   //
   // Dump all Alarm Queue entries into a buffer, in queued order.
   void dump (AlarmQueueEntry* a)
   {
      int index = 0;
      std::list< AlarmQueueEntry* >::iterator entry;

      for ( entry = _Queue.begin() ;
            entry != _Queue.end()  ;
            entry++ )
      {
         a[index] = **entry;
         index   += 1;
      }
   }

   //
   // Determine if a particular alarm type is active
   int isActive (const ALARM_VALUES a)
   {
      int found = 0;

      std::list< AlarmQueueEntry* >::iterator entry;
      for ( entry = _Queue.begin() ;
            entry != _Queue.end() && !found ;
            entry++ )
      {
         if ( (*entry)->name() == a )
         {
            found = 1;
         }
      }

      return found;
   }

   //
   // Determine if a particular alarm type is active
   ALARM_TYPES highestType ()
   {
      ALARM_TYPES type = OPERATOR_ATTENTION;

      std::list< AlarmQueueEntry* >::iterator entry;
      for ( entry = _Queue.begin() ;
            entry != _Queue.end() ;
            entry++ )
      {
         if ( (*entry)->type() < type )
         {
            type = (*entry)->type();
         }
      }

      return type;
   }

   //
   // Determine if a particular alarm type is active
   ALARM_DISPLAY_TYPES highestDisplay ()
   {
      ALARM_DISPLAY_TYPES                     type = DISP_NONE;

      std::list< AlarmQueueEntry* >::iterator entry;
      for ( entry = _Queue.begin() ;
            entry != _Queue.end() ;
            entry++ )
      {
         if ( (*entry)->displayType() < type )
         {
            type = (*entry)->displayType();
         }
      }

      return type;
   }

   //
   // Determine if a particular alarm type is active
   ALARM_ACTIONS highestAction ()
   {
      ALARM_ACTIONS act = NOTHING;

      std::list< AlarmQueueEntry* >::iterator entry;
      for ( entry = _Queue.begin() ;
            entry != _Queue.end() ;
            entry++ )
      {
         if ( (*entry)->action() < act )
         {
            act = (*entry)->action();
         }
      }

      return act;
   }

   //
   // If the queue is locked, add and remove requests are ignored
   void lock (void) { _locked = true; }
   void unlock (void) { _locked = false; }
   bool locked (void) { return _locked; }

   //
   // Get the queue
   std::list< AlarmQueueEntry* >& alarmQ () const { return (std::list< AlarmQueueEntry* >&)_Queue; }

   //
   // Get the sizeof the Queue
   size_t sizeOf () { return _Queue.size(); }

   //
   // Cleanup the queue
   void cleanup () { _Queue.erase(_Queue.begin(), _Queue.end() ); }

protected:

   std::list< AlarmQueueEntry* > _Queue;
   bool _locked;

};

//
//
//   These classes included to allow differentiation of
//   AlarmQueueEntry messages from different sources.
//
#define ALARM_Q_MESSAGE(__name__) \
   class __name__\
      : public Message< AlarmQueueEntry > \
   { \
   public: \
      __name__()\
         : Message<AlarmQueueEntry>() { } \
      __name__(MessageBase::SendType t)\
         : Message<AlarmQueueEntry>() { init(t); } \
      __name__(const CallbackBase &cb, \
               MessageBase::SendReceiveType t = MessageBase::SNDRCV_RECEIVE_ONLY)   \
         : Message<AlarmQueueEntry>() { init(cb, t); } \
      virtual ~__name__() { }; \
   }

ALARM_Q_MESSAGE(AlarmActiveMessage);
ALARM_Q_MESSAGE(AlarmInactiveMessage);

#endif

/* FORMAT HASH 3f7ee5408a5956468145189abe1d62d7 */
