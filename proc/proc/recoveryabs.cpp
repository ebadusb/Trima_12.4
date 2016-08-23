/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      recoveryabs.cpp
 *
 */

#include "recoveryabs.h"
#include "procdata.h"


RecoveryAbs::RecoveryAbs()
   : _RecoveryName(),
     _RecoveryType(Stage1Recovery),
     _QueuedUp(0),
     _Active(0),
     _TransitionsList(),
     _QueueableList(),
     _StackableList(),
     _QueueConsumingList(),
     _MutuallyExclusiveRecsList(),
     _RecoveryCountIndex(0)
{}

RecoveryAbs::RecoveryAbs(const RecoveryAbs& rec)
   : _RecoveryType(rec._RecoveryType),
     _QueuedUp(0),
     _Active(0),
     _RecoveryCountIndex(0)
{
   copyOver(rec);
}

RecoveryAbs::~RecoveryAbs()
{
   cleanup();
}

RecoveryAbs& RecoveryAbs::operator = (const RecoveryAbs& rec)
{
   if ( &rec != this )
   {
      _QueuedUp = 0;
      _Active   = 0;
      copyOver(rec);
   }
   return *this;
}

int RecoveryAbs::operator == (const RecoveryAbs& rec) const
{
   return ( compare(rec) == 0 );
}

int RecoveryAbs::operator == (const string& name) const
{
   return ( compare(name) == 0 );
}

int RecoveryAbs::operator == (const char* name) const
{
   return ( compare(name) == 0 );
}

int RecoveryAbs::operator < (const RecoveryAbs& rec) const
{
   return ( compare(rec)<0 ) ? 1 : 0;
}

int RecoveryAbs::operator < (const string& name) const
{
   return ( compare(name)<0 ) ? 1 : 0;
}

int RecoveryAbs::operator < (const char* name) const
{
   return ( compare(name)<0 ) ? 1 : 0;
}

int RecoveryAbs::operator < (RecoveryAbs& rec)
{
   return ( compare(rec)<0 ) ? 1 : 0;
}

int RecoveryAbs::operator < (const string& name)
{
   return ( compare(name)<0 ) ? 1 : 0;
}

int RecoveryAbs::operator < (const char* name)
{
   return ( compare(name)<0 ) ? 1 : 0;
}

int RecoveryAbs::canTransition (const RecoveryAbs* rec)
{
   // If this is disabled, then we can't transition to it.
   if (disabled()) return 0;

   int status = 0;

   list< RecoveryAbs* >::iterator state;
   for ( state = _TransitionsList.begin() ;
         state != _TransitionsList.end() && !status ;
         state++ )
      if ( rec->compare(*(*state) ) == 0 )
         status = 1;

   for ( state = _QueueableList.begin() ;
         state != _QueueableList.end() && !status ;
         state++ )
      if ( rec->compare(*(*state) ) == 0 )
         status = 1;

   for ( state = _StackableList.begin() ;
         state != _StackableList.end() && !status ;
         state++ )
      if ( rec->compare(*(*state) ) == 0 )
         status = 1;

   for ( state = _QueueConsumingList.begin() ;
         state != _QueueConsumingList.end() && !status ;
         state++ )
      if ( rec->compare(*(*state) ) == 0 )
         status = 1;

   return ( status && rec->mayEnter() );
}

int RecoveryAbs::recoveryNeeded ()
{
   // If this is disabled, then we don't need it.
   if (disabled()) return 0;

   // If queued up and inactive, return 1.
   if ( _QueuedUp && !_Active )
      return 1;

   // Otherwise, call the shouldRecover() function.
   return shouldRecover();
}

bool RecoveryAbs::disabled () const
{
   return false;
}

const int RecoveryAbs::recoveryCount () const
{
   ProcData pd;
   const ProcRun_CDS::recovery_count_type& info = pd.Run().RecoveryCountArray.Get();
   return info[_RecoveryCountIndex];
}

const int RecoveryAbs::operator ++ (int)
{
   ProcData pd;
   ProcRun_CDS::recovery_count_type info = pd.Run().RecoveryCountArray.Get();
   info[_RecoveryCountIndex]++;
   pd.MakeRunWritable();
   pd.Run().RecoveryCountArray.Set(info);
   return info[_RecoveryCountIndex];
}

int RecoveryAbs::compare (const RecoveryAbs& rec) const
{
   return compare(rec.recoveryName() );
}

int RecoveryAbs::compare (const string& name) const
{
   return _RecoveryName.compare(name);
}

int RecoveryAbs::compare (const char* name) const
{
   return _RecoveryName.compare(name);
}

void RecoveryAbs::copyOver (const RecoveryAbs& rec)
{
   //
   // Copy the name ...
   //
   _RecoveryName = rec._RecoveryName;

   RecoveryAbs* rptr = (RecoveryAbs*)&rec;
   //
   // Copy the lists ...
   //
   list< RecoveryAbs* >::iterator state;
   for ( state = rptr->transitions().begin() ;
         state != rptr->transitions().end() ;
         state++ )
      _TransitionsList.push_back( (*state) );

   for ( state = rptr->queueableTransitions().begin() ;
         state != rptr->queueableTransitions().end() ;
         state++ )
      _QueueableList.push_back( (*state) );

   for ( state = rptr->stackableTransitions().begin() ;
         state != rptr->stackableTransitions().end() ;
         state++ )
      _StackableList.push_back( (*state) );

   for ( state = rptr->queueConsumingTransitions().begin() ;
         state != rptr->queueConsumingTransitions().end() ;
         state++ )
      _QueueConsumingList.push_back( (*state) );

   for ( state = rptr->mutuallyExclusiveRecs().begin() ;
         state != rptr->mutuallyExclusiveRecs().end() ;
         state++ )
      _MutuallyExclusiveRecsList.push_back( (*state) );
   _RecoveryCountIndex = 0;
}

void RecoveryAbs::cleanup ()
{
   //
   // Clear the lists ...
   //
   _TransitionsList.erase(_TransitionsList.begin(),
                          _TransitionsList.end() );
   _QueueableList.erase(_QueueableList.begin(),
                        _QueueableList.end() );
   _StackableList.erase(_StackableList.begin(),
                        _StackableList.end() );
   _QueueConsumingList.erase(_QueueConsumingList.begin(),
                             _QueueConsumingList.end() );
   _MutuallyExclusiveRecsList.erase(_MutuallyExclusiveRecsList.begin(),
                                    _MutuallyExclusiveRecsList.end() );

}

RecoveryAbs::CENT_SPEED_ALGORITHM RecoveryAbs::getCentAlgType () const
{
   // todo: make this conditional on sw.dat setting and selected procedure type.
   return SCALED;
}

/* FORMAT HASH 160b592c1517f7fe7520789ec9f67a46 */
