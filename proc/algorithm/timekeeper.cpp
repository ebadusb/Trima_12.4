/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      timekeeper.cpp
 *             Simple time keeper class
 *
 *
 ******************************************************************************/

#include <vxworks.h>
#include "timekeeper.h"
#include "trima_datalog.h"

#include <taskLib.h>

static const long            E3       = 1000;
static const long            E6       = 1000000;
static const long            E9       = 1000000000;
static const struct timespec ZEROTIME = {0, 0};

unsigned long                TimeKeeper::_TheUpdateInterval = 0;


// ----------------------------------------------------------------------
TimeKeeper::TimeKeeper()
   : _AccumulatedTime(ZEROTIME),
     _ActiveFlag(false),
     _Interval(0),
     _Notify(),
     _Element(0)
{
   addTimeKeeper();
}

TimeKeeper::TimeKeeper(const TimeKeeper& src)
   : _AccumulatedTime(src._AccumulatedTime),
     _ActiveFlag(src._ActiveFlag),
     _Interval(src._Interval),
     _Notify(src._Notify),
     _Element(src._Element)
{
   if ( _Element && _Element->isWritable() == false )
   {
      // can't update a read-only element
   }
   else
   {
      addTimeKeeper();
   }
}

TimeKeeper::~TimeKeeper()
{
   removeTimeKeeper();
}

// ----------------------------------------------------------------------
//
//   Initialization variants.
//
void TimeKeeper::init (long secs, long nsecs)
{
   _AccumulatedTime.tv_sec  = secs;
   _AccumulatedTime.tv_nsec = nsecs;
}

void TimeKeeper::init (BaseElement<struct timespec>& element, bool readOnly)
{
   _Element = &element;
   init(element.Get().tv_sec, element.Get().tv_nsec);

   if ( readOnly )
   {
      removeTimeKeeper();
      inactivate();
   }
}

void TimeKeeper::initUpdateTimersInterval (unsigned long theUpdateInterval)
{
   _TheUpdateInterval = theUpdateInterval;
   DataLog(log_level_proc_info) << "TimeKeeper update interval: " << _TheUpdateInterval << endmsg;
}

// ----------------------------------------------------------------------
void TimeKeeper::activate ()
{
   _ActiveFlag = true;
}

void TimeKeeper::inactivate ()
{
   _ActiveFlag = false;
}

double TimeKeeper::getMins () const
{
   return (double)getMSecs() / (60e3); //  60 second/minute * 1000 msecs/second
}
double TimeKeeper::getSecs () const
{
   return (double)getMSecs() * (1e-3); //  1000 msecs/second
}
long TimeKeeper::getMSecs () const
{
   struct timespec dt;
   time(dt);
   long            msec = dt.tv_nsec / E6; // 1000000 nanoseconds/millisecond
   msec += dt.tv_sec * E3;                 // 1000 milliseconds/second

   return msec;
}

void TimeKeeper::time (struct timespec& tm) const
{
   // If using a CDS element, it will have the persisted copy, so
   // use that one if it exists ...
   if ( _Element )
   {
      _AccumulatedTime = _Element->Get();
      tm               = _AccumulatedTime;
   }
   else // otherwise, use the accumulator we are maintaining
   {
      tm = _AccumulatedTime;
   }
}

int TimeKeeper::timesUp ()
{
   if ( _Interval > 0 )
   {
      time_t secs = _Interval / E3;
      if ( secs < _AccumulatedTime.tv_sec )
      {
         _Notify();
         return 1;
      }
      else if ( secs == _AccumulatedTime.tv_sec )
      {
         long nsecs = ( _Interval % E3 ) * E6;
         if ( nsecs <= _AccumulatedTime.tv_nsec )
         {
            _Notify();
            return 1;
         }
      }
   }
   return 0;
}

void TimeKeeper::update ()
{
   if ( _ActiveFlag )
   {
      time_t secs  = _AccumulatedTime.tv_sec;
      long   nsecs = _AccumulatedTime.tv_nsec;

      // Increment the time elapsed since last update
      nsecs += _TheUpdateInterval * E6;  // milliseconds to nanoseconds

      if ( nsecs >= E9)
      {
         secs++;
         nsecs -= E9;
      }

      _AccumulatedTime.tv_sec  = secs;
      _AccumulatedTime.tv_nsec = nsecs;

      //
      //
      // If we have a common data store update that value also
      //
      if (_Element)
      {
         _Element->Set(_AccumulatedTime);
      }

      //
      // Check to see if the time interval specified by the
      //  user has been reached ...
      //
      timesUp();
   }
}

void TimeKeeper::addTimeKeeper ()
{
   TimeKeeper_CDS tkcds(ROLE_RO);
   TKStaticData&  data = tkcds.tkd();

   //
   // Add the new time keeper object to our list ...
   //
   data.TimeKeeperList.push_back(this);
   data.ReferenceCount++;
}

void TimeKeeper::updateTimers ()
{
   TimeKeeper_CDS tkcds(ROLE_RO);
   TKStaticData&  data = tkcds.tkd();

   //
   // Update all of the time keepers in the list ...
   //
   list<TimeKeeper*>::iterator tk;
   for ( tk = data.TimeKeeperList.begin() ;
         tk != data.TimeKeeperList.end() ;
         tk++ )
   {
      //
      // Call the update function of the time keeper
      //
      (*tk)->update();
   }
}

void TimeKeeper::removeTimeKeeper ()
{
   TimeKeeper_CDS tkcds(ROLE_RO);
   TKStaticData&  data = tkcds.tkd();

   //
   // Remove the new time keeper object from our list ...
   //
   data.TimeKeeperList.remove(this);
   data.ReferenceCount--;
}

TimeKeeper::TKD_MAP& TimeKeeper::TimeKeeper_CDS::tkdMap ()
{
   TimeKeeper::TKD_MAP* pMap = TimeKeeperDataMap.Get();
   if ( 0 == pMap )
   {
      SetWrite();
      TimeKeeperDataMap.Set(new TimeKeeper::TKD_MAP);
      SetRead();
      pMap = TimeKeeperDataMap.Get();
   }
   return ( *pMap );
}

TimeKeeper::TKStaticData& TimeKeeper::TimeKeeper_CDS::tkd ()
{
   TimeKeeper::TKD_MAP& theMap(tkdMap() );
   TKD_MAP::iterator    diter = theMap.find(taskIdSelf() );
   if ( diter == theMap.end() )
   {
      //
      // Assign this instance to be the task wide object
      theMap[ taskIdSelf() ] = new TimeKeeper::TKStaticData();

      diter                  = theMap.find(taskIdSelf() );
   }
   return (*(TimeKeeper::TKStaticData*)(*diter).second);
}

/* FORMAT HASH 15ce61cc6154c95e39547f84d006bd6a */
