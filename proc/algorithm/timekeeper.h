/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      timekeeper.h
 *
 *             Simple time class used to keep track of time.
 *
 *
 ******************************************************************************/


#ifndef _TIMEKEEPER_H_
#define _TIMEKEEPER_H_

#include <list>
#include <map>
#include <time.h>

#include "callback.h"
#include "datastore.h"


// A simple time class used to keep track of time and too also provide automatic
// notification when a specified interval has passed.
//
// Each TimeKeeper is registered in a static list for the task in which it is
// instantiated. The timer update/notification relies on the associated task(s)
// to poke it on a periodic basis via the static updateTimers() method. Or a
// task can setup a TimerMessage with a callback to invoke updateTimers(), such
// that a single timer message can service all of the task's TimeKeepers.
//
class TimeKeeper
{
public:
   // Default Constructor
   TimeKeeper();
   // Copy Constructor
   TimeKeeper(const TimeKeeper& src);
   // Destructor
   virtual ~TimeKeeper();

   //
   // Function used to initialize (reset) the accumulated time. When using a
   // notification interval, the notifier callback should call this method
   // (with default values) to reset the accumulator, effectively re-arming it
   // it as a recurring, periodic timer.
   //
   void init (long secs = 0, long nsecs = 0);

   // This initializer sets up to use a CDS element for the accumulated time.
   // It assumes ROLE_RW. For ROLE_RO, set readOnly flag to true.
   // Note that for the read-only case, the automatic update & notification
   // provided by updateTimers() is disabled; thus, only the time measurement
   // methods are valid/useful: getMins(), getSecs(), getMSecs(), and time().
   //
   void init (BaseElement<struct timespec>& element, bool readOnly = false);

   //
   // Defines the interval in milliseconds at which the calling task or TimerMessage
   // callback will invoke updateTimers(). This is the minimum resolution for all
   // TimeKeeper objects.
   static void initUpdateTimersInterval (unsigned long theUpdateInterval);

   //
   // Updates all of the TimeKeeper objects associated with the calling task.
   // This should be invoked within a task's periodic update loop, or by a
   // TimerMessage callback.
   //
   static void updateTimers ();

   //
   // Set/Get methods for the notification interval (in milliseconds)
   //
   void                interval (const unsigned long t) { _Interval = t; }
   const unsigned long interval () const { return _Interval; }

   //
   // Set the callback to invoke when notification interval expires.
   //
   void notifier (CallbackBase cb) { _Notify = cb; }

   // Start accumulation of time
   void activate ();

   // Stop accumulation of time until activate is called again
   void inactivate ();

   // True if activated (accumulating time)
   bool activated () const { return _ActiveFlag; }

   // Time measurement to obtain a value of time measured
   double getMins () const;   // time in minutes.
   double getSecs () const;   // time in seconds
   long   getMSecs () const;  // time in milliseconds

   // Obtain the time accumulated since last activated.
   void time (struct timespec& dt) const;

private:

   //
   // Function used to see if the user specified
   //  notification interval is up
   //
   int timesUp ();

   //
   // Add self to the calling task's static list of TimeKeeper objects
   void addTimeKeeper ();
   //
   // Remove self from the calling task's static list of TimeKeeper objects
   void removeTimeKeeper ();

   //
   // Callback for the update timer
   //
   void update ();


private:

   // The interval at which updateTimers() is assumed to be invoked.
   static unsigned long _TheUpdateInterval;

   //
   // Current accumulated time the timer has been active
   //
   mutable struct timespec _AccumulatedTime;

   //
   // Flag to signify the timer should be accumulating time
   //
   bool _ActiveFlag;

   //
   // Time Interval upon which the user will be notified
   //
   unsigned long _Interval;

   //
   // Notification callback
   //
   CallbackBase _Notify;

   //
   //
   //  Storage for common data store updates if any
   //
   BaseElement<struct timespec>* _Element;

private:

   struct TKStaticData
   {
      TKStaticData()
         : TimeKeeperList(),
           ReferenceCount(0) { }

      std::list< TimeKeeper* > TimeKeeperList;
      unsigned int             ReferenceCount;
   };

   class TKD_MAP
      : public std::map<unsigned long, TKStaticData*>
   {
   public:
      TKD_MAP() {}
      virtual ~TKD_MAP() {}
   };


   class TimeKeeper_CDS
      : public DynamicSingleWriteDataStore
   {
   public:

      TimeKeeper_CDS (Role role)
         : DynamicSingleWriteDataStore ("TimeKeeper_CDS", role)
      {
         TimeKeeperDataMap.Register (this, NO_PFR, 0);
      }

      virtual ~TimeKeeper_CDS() {}

      TKD_MAP&      tkdMap ();
      TKStaticData& tkd ();

   private:
      // Data Members
      BaseElement<TKD_MAP*> TimeKeeperDataMap;
      TimeKeeper_CDS();  // Base Constructor not available
   };

   friend class TimeKeeper::TKStaticData;

};

#endif

/* FORMAT HASH df0d596719eb13afc3fac82eea600dbb */
