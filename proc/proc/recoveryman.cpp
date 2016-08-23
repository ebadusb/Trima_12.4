/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      recoveryman.cpp
 *
 */

#include "message.h"
#include "recoveryman.h"
#include "objdictionary.h"
#include "trima_datalog.h"
#include "procdata.h"

#include <iostream.h>

RecoveryMan* RecoveryMan::_This = 0;

#define _RECOVERYMAN_CPP_
#include "recoveries.h"
#undef _RECOVERYMAN_CPP_

int RecoveryMan::recoveryAvailable (const char* rec)
{
   if ( _This )
   {
      //
      // Find the recovery in the list ...
      //
      RecoveryAbs*                 recPtr = 0;
      list<RecoveryAbs*>::iterator state;
      for ( state = _This->_Recoveries.begin() ;
            state != _This->_Recoveries.end() ;
            state++ )
      {
         if ( *(*state) == rec )
         {
            recPtr = (*state);
            break;
         }
      }
      //
      // If we found the requested recovery ...
      //
      if ( recPtr )
      {
         //
         // Find out what the current recovery state is ...
         //
         RecoveryAbs* current = _This->currentRecovery();
         if ( current )
         {
            //
            // Can we go to the requested from here ...
            //
            return ( current->canTransition(recPtr) && !recPtr->queuedUp() );
         }
         else
         {
            //
            // We are not in recovery, may we enter the
            //  requested ...
            //
            return recPtr->mayEnter();
         }
      }
   }
   //
   // The recovery manager has not been initialized yet or,
   //  the requested recovery could not be found ...
   //
   return 0;
}
int RecoveryMan::recoveryAvailable (const RecoveryTypes::Type rec)
{
   return RecoveryMan::recoveryAvailable(RecoveryTypes::RecoveryStr(rec) );
}

RecoveryTypes::InRecovery RecoveryMan::inRecovery ()
{
   if ( _This )
   {
      //
      // Find out what the current recovery state is ...
      //
      RecoveryAbs* current = _This->currentRecovery();
      if ( current )
      {
         return current->recoveryExpected();
      }
   }
   return RecoveryTypes::NoRecovery;
}

RecoveryTypes::Type RecoveryMan::current ()
{
   if ( _This )
   {
      RecoveryAbs* current = _This->currentRecovery();
      if ( current )
      {
         return (RecoveryTypes::Type)RecoveryTypes::RecoveryId(current->recoveryName() );
      }
   }
   return RecoveryTypes::None;
}

RecoveryMan::RecoveryMan()
   : _Recoveries(),
     _RecoveryQueue(),
     _ReportedRecovery(RecoveryTypes::None)
{}

RecoveryMan::~RecoveryMan()
{
   cleanup();
}

int RecoveryMan::init ()
{
   cleanup();

   //
   // Initialize the lists ...
   //
   if ( !initRecoveries() ||
        !initTransitionList() ||
        !initQueueableTransList() ||
        !initStackableTransList() ||
        !initQueueConsumingTransList() ||
        !initMutuallyExclusiveRecsList() )
   {
      DataLog(log_level_proc_error) << "Recovery Lists failed to initialize" << endmsg;
      return 0;
   }

   //
   // Initialize all recoveries ...
   //
   list<RecoveryAbs*>::iterator state;
   for ( state = _Recoveries.begin(); state != _Recoveries.end(); state++ )
   {
      (*state)->init();
   }

   for ( int count = (int)RecoveryAbs::Stage1Recovery ; count<(int)RecoveryAbs::NullStage ; count++ )
   {
      _CurrentRecovery[ count ] = 0;
   }



   if ( log_level_proc_info.logOutput() == DataLog_LogEnabled )
   {
      dump();
   }


   //
   // Initialize the static this pointer ...
   //
   _This = this;

   return 1;
}

int RecoveryMan::doRecovery ()
{
   //
   // Current substate for ease of reference ...
   //
   RecoveryAbs* current = 0;
   ProcData     pd;
   //
   // Loop through the stages of recoveries for coverage of recovery
   //  actions which cause other recoveries ...
   //
   for ( int count = (int)RecoveryAbs::Stage1Recovery ;
         count<(int)RecoveryAbs::NullStage ;
         count++ )
   {
      //
      // Order the recoveries ...
      //
      processRecoveries( (RecoveryAbs::RecoveryType)count);

      current = currentRecovery( (RecoveryAbs::RecoveryType)count);

      // Save the current and previous recoveries. Used by predict
      // and Centrifuge Monitor
      RecoveryTypes::Type prevRecoveryType = pd.Run().CurrentRecovery.Get();
      if ( prevRecoveryType != this->current())
      {
         pd.MakeRunWritable();
         pd.Run().PreviousRecovery.Set(prevRecoveryType);
         pd.Run().CurrentRecovery.Set(this->current());
         pd.MakeRunReadable();
      }

      if ( current )
      {
         //
         // Remove any recoveries that the current one doesn't like ...
         //
         list<RecoveryAbs*>::iterator tstate;
         for ( tstate = current->mutuallyExclusiveRecs().begin() ;
               tstate != current->mutuallyExclusiveRecs().end() ;
               tstate++ )
         {
            //
            // Remove the recovery ...
            //
            removeRecovery(*tstate);
         }

         //
         // After the queue has been set up, let the first recovery in the queue
         //  have some processing time ...
         //
         current->doRecovery();

         //
         // Increment the recovery count ...
         if ( _CurrentRecovery[ count ] != current )
         {
            _CurrentRecovery[count] = current;
            (*current)++;

            DataLog(log_level_proc_debug) << "Current Recovery ( " << current->recoveryName() <<
               " ) executed " << (int)current->recoveryCount() << " times" << endmsg;
         }
      }
      else
      {
         //
         // Reset the history ...
         _CurrentRecovery[count] = current;
      }
   }

   //
   // Inform others of my progress ...
   //
   RecoveryTypes::Type rt = RecoveryMan::current();
   if ( rt != _ReportedRecovery )
   {
      RecoveryStatus status;
      status._Type       = rt;
      status._InRecovery = RecoveryMan::inRecovery();

      Message<RecoveryStatus> recStatusMsg(MessageBase::SEND_GLOBAL);
      recStatusMsg.send(status);

      _ReportedRecovery = rt;
   }

   if ( log_level_proc_info.logOutput() == DataLog_LogEnabled )
   {
      dumpQueue();
   }

   return 1;
}

void RecoveryMan::processRecoveries (const RecoveryAbs::RecoveryType type)
{
   //
   // Grab the current substate for ease of reference ...
   //
   RecoveryAbs* current = currentRecovery(type);

   //
   // Determine if the current recovery has completed ...
   //
   if ( current &&
        !current->recoveryNeeded() )
   {
      removeRecovery(current);
      current = currentRecovery(type);
   }

   //
   // Inactivate the old current recovery ...
   //
   if ( current )
      current->active(0);

   //
   // Iterate though the recoveries list and find out which recoveries need to
   //  be done ...
   //
   list<RecoveryAbs*>::iterator state;
   for ( state = _Recoveries.begin() ;
         state != _Recoveries.end() ;
         state++ )
   {
      //
      // Only look at recoveries of this type ...
      //
      if ( (*state)->recoveryType() == type )
      {
         //
         // If the recovery can and needs to be done ...
         //
         if ( ( !current || current->canTransition( (*state) ) ) &&
              ( (*state)->mayEnter() && (*state)->recoveryNeeded() ) )
         {
            //
            // Order the queue ...
            //
            addRecovery( (*state) );
         }
      }
   }
}

RecoveryAbs* RecoveryMan::currentRecovery (const RecoveryAbs::RecoveryType type)
{
   RecoveryAbs*                 current = 0;

   list<RecoveryAbs*>::iterator state;
   for ( state = _RecoveryQueue.begin() ;
         state != _RecoveryQueue.end() ;
         state++ )
   {
      if ( (*state)->recoveryType() == type ||
           type == RecoveryAbs::NullStage )
      {
         current = (*state);
         current->active(1);
         break;
      }
   }

   return current;
}

const char* RecoveryMan::currentState ()
{
   //
   // Get the current recovery ...
   //
   RecoveryAbs* current = currentRecovery();

   if ( current )
   {
      return current->recoveryName();
   }
   return 0;
}

int RecoveryMan::addRecovery (RecoveryAbs* rec)
{
   DataLog(log_level_proc_debug) << "addRecovery" << endmsg;
   //
   // Get the current recovery ...
   //
   RecoveryAbs* current = currentRecovery();

   if ( !current )
   {
      //
      // No current substate, so just add this substate ...
      //
      addRecoveryAsQueue(rec);

      //
      // Done ...
      //
      return 1;
   }

   //
   // Iterate through the recovery list and find the transition
   //
   list<RecoveryAbs*>::iterator rstate;
   for ( rstate = _Recoveries.begin() ;
         rstate != _Recoveries.end() ;
         rstate++ )
   {
      if ( *(RecoveryAbs*)(*rstate) == *current )
      {
         list<RecoveryAbs*>::iterator tstate;

         //
         // If this transition is in the queueable transition list ...
         //
         for ( tstate = ((RecoveryAbs*)*rstate)->queueableTransitions().begin() ;
               tstate != ((RecoveryAbs*)*rstate)->queueableTransitions().end() ;
               tstate++ )
         {
            if ( *(RecoveryAbs*)(*tstate) == *rec )
            {
               //
               // Add the next substate to the queue ...
               //
               addRecoveryAsQueue(*tstate);
               //
               // Add the current substate back in the front of the list ...
               //
               addRecoveryToStack(current);

               return 1;
            }
         }



         //
         // If this transition is in the stackable list ...
         //
         for ( tstate = ((RecoveryAbs*)*rstate)->stackableTransitions().begin() ;
               tstate != ((RecoveryAbs*)*rstate)->stackableTransitions().end() ;
               tstate++ )
         {
            if ( *(RecoveryAbs*)(*tstate) == *rec )
            {
               //
               // Stack the next substate ...
               //
               addRecoveryAsStack(rec);

               return 1;
            }
         }


         //
         // If this transition is in the clear queue list ...
         //
         for ( tstate = ((RecoveryAbs*)*rstate)->queueConsumingTransitions().begin() ;
               tstate != ((RecoveryAbs*)*rstate)->queueConsumingTransitions().end() ;
               tstate++ )
         {
            if ( (*(RecoveryAbs*)*tstate) == *rec )
            {
               //
               // Clear the queue then add the substate
               //
               clearQueueThenAddRecovery(*tstate);

               return 1;
            }
         }

      }
   }
   return 0;
}

void RecoveryMan::addRecoveryAsQueue (RecoveryAbs* rec)
{
   DataLog(log_level_proc_debug) << "addRecoveryAsQueue" << endmsg;
   //
   // Add this state
   //
   if ( !addRecoveryToQueue(rec) ) return;

   //
   //  ... and all recovery transitions from this state ...
   //
   list<RecoveryAbs*>::iterator tstate;
   for ( tstate = rec->transitions().begin() ;
         tstate != rec->transitions().end() ;
         tstate++ )
   {
      addRecoveryAsQueue(*tstate);
   }
}

void RecoveryMan::addRecoveryAsStack (RecoveryAbs* rec)
{
   if ( !rec || !rec->mayEnter() ) return;

   //
   //  add all recovery transitions from this state ...
   //
   list<RecoveryAbs*>::iterator tstate;
   for ( tstate = rec->transitions().begin() ;
         tstate != rec->transitions().end() ;
         tstate++ )
   {
      if ( !recoveryInQueue( (*tstate) ) )
      {
         addRecoveryAsStack(*tstate);
      }
   }

   //
   // ... then, add this state
   //
   addRecoveryToStack(rec);

}

void RecoveryMan::clearQueueThenAddRecovery (RecoveryAbs* rec)
{
   DataLog(log_level_proc_debug) << "clearQueueThenAddRecovery" << endmsg;
   //
   // Iterate through the queue and
   //   check to see that this state is not in the queue,
   //   before adding it.
   //
   int inQueue = 0;
   list<RecoveryAbs*>::iterator qstate;
   for ( qstate = _RecoveryQueue.begin() ;
         qstate != _RecoveryQueue.end() ;
         qstate++ )
      if ( *(RecoveryAbs*)(*qstate) == *rec )
      {
         inQueue = 1;
         break;
      }

   //
   // If this state is in the queue, then don't do
   //  anything, else ...
   if ( !inQueue )
   {
      //
      // Clear the queue of all entries
      //
      clearQueue();

      //
      // Add this state to the queue
      //
      addRecoveryAsQueue(rec);
   }
}

void RecoveryMan::removeRecoveryFromQueue (RecoveryAbs* rec)
{
   DataLog(log_level_proc_debug) << "removeRecoveryFromQueue" << endmsg;
   //
   // remove this state if it's in the list ...
   //
   removeRecovery(rec);


   //
   //  ... and all recovery transitions from this state ...
   //
   list<RecoveryAbs*>::iterator tstate;
   for ( tstate = rec->transitions().begin() ;
         tstate != rec->transitions().end() ;
         tstate++ )
   {
      removeRecoveryFromQueue(*tstate);
   }
}

int RecoveryMan::recoveryInQueue (RecoveryAbs* rec)
{
   //
   // Iterate through the queue and
   //   check to see that this state is not in the queue
   //
   int inQueue = 0;
   list<RecoveryAbs*>::iterator qstate;
   for ( qstate = _RecoveryQueue.begin() ;
         qstate != _RecoveryQueue.end() ;
         qstate++ )
   {
      if ( *(RecoveryAbs*)(*qstate) == *rec )
      {
         return 1;
      }
   }
   return 0;
}

int RecoveryMan::addRecoveryToQueue (RecoveryAbs* rec)
{
   if ( !rec || !rec->mayEnter() )
      return 0;

   _RecoveryQueue.remove(rec);
   _RecoveryQueue.push_back(rec);
   rec->active(0);
   rec->queuedUp(1);

   return 1;
}

int RecoveryMan::addRecoveryToStack (RecoveryAbs* rec)
{
   if ( !rec || !rec->mayEnter() )
      return 0;

   _RecoveryQueue.remove(rec);
   _RecoveryQueue.push_front(rec);
   rec->active(0);
   rec->queuedUp(1);

   return 1;
}

void RecoveryMan::removeRecovery (RecoveryAbs* rec)
{
   _RecoveryQueue.remove(rec);
   rec->active(0);
   rec->queuedUp(0);
   rec->reset();
}

void RecoveryMan::clearQueue ()
{
   //
   // Iterate through the queue and
   //   clear every entry's queued up flag ...
   //
   list<RecoveryAbs*>::iterator state;
   for ( state = _Recoveries.begin() ;
         state != _Recoveries.end() ;
         state++ )
   {
      (*state)->active(0);
      (*state)->queuedUp(0);
      (*state)->reset();
   }

   //
   // Clear the queue of all entries
   //
   _RecoveryQueue.clear();
}

int RecoveryMan::initRecoveries ()
{
   int index = 0;
   while (    _Stage1Recoveries[index] != RecoveryTypes::None
              && _Stage1Recoveries[index] != RecoveryTypes::RecoveryNum )
   {
      RecoveryAbs*                 statePtr = 0;
      list<RecoveryAbs*>::iterator state;
      for ( state = _Recoveries.begin() ;
            state != _Recoveries.end() ;
            state++ )
      {
         //
         // We already have created this 'recovery state' in
         // our list
         //
         if ( *((RecoveryAbs*)*state) == RecoveryTypes::RecoveryStr(_Stage1Recoveries[index]) )
            break;
      }

      if ( state == _Recoveries.end() )
      {
         if ( !ObjDictionary::valid(RecoveryTypes::RecoveryStr(_Stage1Recoveries[index]) ) )
         {
            DataLog(log_level_proc_error) << "Invalid recovery requested -> " << RecoveryTypes::RecoveryStr(_Stage1Recoveries[index]) << endmsg;
            return 0;
         }
         statePtr = (RecoveryAbs*)ObjDictionary::create(RecoveryTypes::RecoveryStr(_Stage1Recoveries[index]) );
         statePtr->recoveryName(RecoveryTypes::RecoveryStr(_Stage1Recoveries[index]) );
         statePtr->recoveryCountIndex(_Stage1Recoveries[ index ]);
         statePtr->recoveryType(RecoveryAbs::Stage1Recovery);
         _Recoveries.push_back(statePtr);
      }
      index++;
   }

   index = 0;
   while (    _Stage2Recoveries[index] != RecoveryTypes::None
              && _Stage2Recoveries[index] != RecoveryTypes::RecoveryNum )
   {
      RecoveryAbs*                 statePtr = 0;
      list<RecoveryAbs*>::iterator state;
      for ( state = _Recoveries.begin() ;
            state != _Recoveries.end() ;
            state++ )
      {
         //
         // We already have created this 'recovery state' in
         // our list
         //
         if ( *((RecoveryAbs*)*state) == RecoveryTypes::RecoveryStr(_Stage2Recoveries[index]) )
            break;
      }

      if ( state == _Recoveries.end() )
      {
         if ( !ObjDictionary::valid(RecoveryTypes::RecoveryStr(_Stage2Recoveries[index]) ) )
         {
            DataLog(log_level_proc_error) << "Invalid recovery requested -> " << RecoveryTypes::RecoveryStr(_Stage2Recoveries[index]) << endmsg;
            return 0;
         }
         statePtr = (RecoveryAbs*)ObjDictionary::create(RecoveryTypes::RecoveryStr(_Stage2Recoveries[index]) );
         statePtr->recoveryName(RecoveryTypes::RecoveryStr(_Stage2Recoveries[index]) );
         statePtr->recoveryCountIndex(_Stage2Recoveries[ index ]);
         statePtr->recoveryType(RecoveryAbs::Stage2Recovery);
         _Recoveries.push_back(statePtr);
      }
      index++;
   }

   return 1;
}

int RecoveryMan::initTransitionList ()
{
   int index = 0;
   while ( _Transitions[index]._CurrentState != RecoveryTypes::None
           && _Transitions[index]._CurrentState != RecoveryTypes::RecoveryNum )
   {
      RecoveryAbs*                 statePtr = 0;
      list<RecoveryAbs*>::iterator state;
      for ( state = _Recoveries.begin() ;
            state != _Recoveries.end() ;
            state++ )
      {
         //
         // We already have created this 'recovery state' in
         // our list, use this one
         //
         if ( *((RecoveryAbs*)*state) == RecoveryTypes::RecoveryStr(_Transitions[index]._CurrentState) )
            break;
      }

      if ( state == _Recoveries.end() )
      {
         DataLog(log_level_proc_error) << "Invalid recovery requested -> " << RecoveryTypes::RecoveryStr(_Transitions[index]._CurrentState) << endmsg;
         return 0;
      }
      else
      {
         statePtr = *state;
      }

      if ( _Transitions[index]._NextState != RecoveryTypes::None
           && _Transitions[index]._NextState != RecoveryTypes::RecoveryNum )
      {
         //
         // Now set up the transitions list
         //
         list<RecoveryAbs*>::iterator tstate;
         for ( tstate = statePtr->transitions().begin() ;
               tstate != statePtr->transitions().end() ;
               tstate++ )
         {
            //
            // We already have created this 'recovery state' in
            // our list, we are done
            //
            if ( *((RecoveryAbs*)*tstate) == RecoveryTypes::RecoveryStr(_Transitions[index]._NextState) )
               break;
         }

         if ( tstate == statePtr->transitions().end() )
         {
            //
            // Check to see if the transition state is in our
            //  'recovery state' list
            //
            list<RecoveryAbs*>::iterator newstate;
            for ( newstate = _Recoveries.begin() ;
                  newstate != _Recoveries.end() ;
                  newstate++ )
            {
               //
               // We already have created this 'recovery state'
               //
               if ( *((RecoveryAbs*)*newstate) == RecoveryTypes::RecoveryStr(_Transitions[index]._NextState) )
                  break;
            }

            RecoveryAbs* newStatePtr = 0;
            if ( newstate == _Recoveries.end() )
            {
               DataLog(log_level_proc_error) << "Invalid recovery requested -> " << RecoveryTypes::RecoveryStr(_Transitions[index]._CurrentState) << endmsg;
               return 0;
            }
            else
            {
               newStatePtr = *newstate;
            }
            statePtr->transitions().push_back(newStatePtr);

         }
      }
      index++;
   }

   return 1;
}

int RecoveryMan::initQueueableTransList ()
{
   int index = 0;
   DataLog(log_level_proc_debug) << "initQueueableTransList" << endmsg;


   while ( _QueueableTransitions[index]._CurrentState != RecoveryTypes::None
           && _QueueableTransitions[index]._CurrentState != RecoveryTypes::RecoveryNum )
   {
      RecoveryAbs*                 statePtr = 0;
      list<RecoveryAbs*>::iterator state;
      for ( state = _Recoveries.begin() ;
            state != _Recoveries.end() ;
            state++ )
      {
         //
         // We already have created this 'recovery state' in
         // our list, use this one
         //
         if ( *((RecoveryAbs*)*state) == RecoveryTypes::RecoveryStr(_QueueableTransitions[index]._CurrentState) )
            break;
      }

      if ( state == _Recoveries.end() )
      {
         DataLog(log_level_proc_error) << "Invalid recovery requested -> " << RecoveryTypes::RecoveryStr(_QueueableTransitions[index]._CurrentState) << endmsg;
         return 0;
      }
      else
      {
         statePtr = *state;
      }

      if ( _QueueableTransitions[index]._NextState != RecoveryTypes::None
           && _QueueableTransitions[index]._NextState != RecoveryTypes::RecoveryNum )
      {
         //
         // Now set up the transitions list
         //
         list<RecoveryAbs*>::iterator tstate;
         for ( tstate = statePtr->queueableTransitions().begin() ;
               tstate != statePtr->queueableTransitions().end() ;
               tstate++ )
         {
            //
            // We already have created this 'recovery state' in
            // our list, we are done
            //
            if ( *((RecoveryAbs*)*tstate) == RecoveryTypes::RecoveryStr(_QueueableTransitions[index]._NextState) )
               break;
         }

         if ( tstate == statePtr->queueableTransitions().end() )
         {
            //
            // Check to see if the transition state is in our
            //  'recovery state' list
            //
            list<RecoveryAbs*>::iterator newstate;
            for ( newstate = _Recoveries.begin() ;
                  newstate != _Recoveries.end() ;
                  newstate++ )
            {
               //
               // We already have created this 'recovery state'
               //
               if ( *((RecoveryAbs*)*newstate) == RecoveryTypes::RecoveryStr(_QueueableTransitions[index]._NextState) )
                  break;
            }

            RecoveryAbs* newStatePtr = 0;
            if ( newstate == _Recoveries.end() )
            {
               DataLog(log_level_proc_error) << "Invalid recovery requested -> " << RecoveryTypes::RecoveryStr(_QueueableTransitions[index]._CurrentState) << endmsg;
               return 0;
            }
            else
            {
               newStatePtr = *newstate;
            }
            statePtr->queueableTransitions().push_back(newStatePtr);

         }
      }
      index++;
   }

   return 1;
}

int RecoveryMan::initStackableTransList ()
{
   int index = 0;
   DataLog(log_level_proc_debug) << "initStackableTransList" << endmsg;
   while ( _StackableTransitions[index]._CurrentState != RecoveryTypes::None
           && _StackableTransitions[index]._CurrentState != RecoveryTypes::RecoveryNum )
   {
      RecoveryAbs*                 statePtr = 0;
      list<RecoveryAbs*>::iterator state;
      for ( state = _Recoveries.begin() ;
            state != _Recoveries.end() ;
            state++ )
      {
         //
         // We already have created this 'recovery state' in
         // our list, use this one
         //
         if ( *((RecoveryAbs*)*state) == RecoveryTypes::RecoveryStr(_StackableTransitions[index]._CurrentState) )
            break;
      }

      if ( state == _Recoveries.end() )
      {
         DataLog(log_level_proc_error) << "Invalid recovery requested -> " << RecoveryTypes::RecoveryStr(_StackableTransitions[index]._CurrentState) << endmsg;
         return 0;
      }
      else
      {
         statePtr = *state;
      }

      if ( _StackableTransitions[index]._NextState != RecoveryTypes::None
           && _StackableTransitions[index]._NextState != RecoveryTypes::RecoveryNum )
      {
         //
         // Now set up the transitions list
         //
         list<RecoveryAbs*>::iterator tstate;
         for ( tstate = statePtr->stackableTransitions().begin() ;
               tstate != statePtr->stackableTransitions().end() ;
               tstate++ )
         {
            //
            // We already have created this 'recovery state' in
            // our list, we are done
            //
            if ( *((RecoveryAbs*)*tstate) == RecoveryTypes::RecoveryStr(_StackableTransitions[index]._NextState) )
               break;
         }

         if ( tstate == statePtr->stackableTransitions().end() )
         {
            //
            // Check to see if the transition state is in our
            //  'recovery state' list
            //
            list<RecoveryAbs*>::iterator newstate;
            for ( newstate = _Recoveries.begin() ;
                  newstate != _Recoveries.end() ;
                  newstate++ )
            {
               //
               // We already have created this 'recovery state'
               //
               if ( *((RecoveryAbs*)*newstate) == RecoveryTypes::RecoveryStr(_StackableTransitions[index]._NextState) )
                  break;
            }

            RecoveryAbs* newStatePtr = 0;
            if ( newstate == _Recoveries.end() )
            {
               DataLog(log_level_proc_error) << "Invalid recovery requested -> " << RecoveryTypes::RecoveryStr(_StackableTransitions[index]._CurrentState) << " " << index << endmsg;
               return 0;
            }
            else
            {
               newStatePtr = *newstate;
            }
            statePtr->stackableTransitions().push_back(newStatePtr);

         }
      }
      index++;
   }

   return 1;
}

int RecoveryMan::initQueueConsumingTransList ()
{
   int index = 0;
   DataLog(log_level_proc_debug) << "initQueueConsumingTransList" << endmsg;
   while ( _QueueConsumingTransitions[index]._CurrentState != RecoveryTypes::None
           && _QueueConsumingTransitions[index]._CurrentState != RecoveryTypes::RecoveryNum )
   {
      RecoveryAbs*                 statePtr = 0;
      list<RecoveryAbs*>::iterator state;
      for ( state = _Recoveries.begin() ;
            state != _Recoveries.end() ;
            state++ )
      {
         //
         // We already have created this 'recovery state' in
         // our list, use this one
         //
         if ( *((RecoveryAbs*)*state) == RecoveryTypes::RecoveryStr(_QueueConsumingTransitions[index]._CurrentState) )
            break;
      }

      if ( state == _Recoveries.end() )
      {
         DataLog(log_level_proc_error) << "Invalid recovery requested -> " << RecoveryTypes::RecoveryStr(_QueueConsumingTransitions[index]._CurrentState) << endmsg;
         return 0;
      }
      else
      {
         statePtr = *state;
      }

      if ( _QueueConsumingTransitions[index]._NextState != RecoveryTypes::None
           && _QueueConsumingTransitions[index]._NextState != RecoveryTypes::RecoveryNum )
      {
         //
         // Now set up the transitions list
         //
         list<RecoveryAbs*>::iterator tstate;
         for ( tstate = statePtr->queueConsumingTransitions().begin() ;
               tstate != statePtr->queueConsumingTransitions().end() ;
               tstate++ )
         {
            //
            // We already have created this 'recovery state' in
            // our list, we are done
            //
            if ( *((RecoveryAbs*)*tstate) == RecoveryTypes::RecoveryStr(_QueueConsumingTransitions[index]._NextState) )
               break;
         }

         if ( tstate == statePtr->queueConsumingTransitions().end() )
         {
            //
            // Check to see if the transition state is in our
            //  'recovery state' list
            //
            list<RecoveryAbs*>::iterator newstate;
            for ( newstate = _Recoveries.begin() ;
                  newstate != _Recoveries.end() ;
                  newstate++ )
            {
               //
               // We already have created this 'recovery state'
               //
               if ( *((RecoveryAbs*)*newstate) == RecoveryTypes::RecoveryStr(_QueueConsumingTransitions[index]._NextState) )
                  break;
            }

            RecoveryAbs* newStatePtr = 0;
            if ( newstate == _Recoveries.end() )
            {
               DataLog(log_level_proc_error) << "Invalid recovery requested -> " << RecoveryTypes::RecoveryStr(_QueueConsumingTransitions[index]._CurrentState) << endmsg;
               return 0;
            }
            else
            {
               newStatePtr = *newstate;
            }
            statePtr->queueConsumingTransitions().push_back(newStatePtr);

         }
      }
      index++;
   }

   return 1;
}

int RecoveryMan::initMutuallyExclusiveRecsList ()
{
   int index = 0;
   DataLog(log_level_proc_debug) << "initMutuallyExclusiveRecsList" << endmsg;
   while ( _MutuallyExclusiveRecoveries[index]._CurrentState != RecoveryTypes::None
           && _MutuallyExclusiveRecoveries[index]._CurrentState != RecoveryTypes::RecoveryNum )
   {
      RecoveryAbs*                 statePtr = 0;
      list<RecoveryAbs*>::iterator state;
      for ( state = _Recoveries.begin() ;
            state != _Recoveries.end() ;
            state++ )
      {
         //
         // We already have created this 'recovery state' in
         // our list, use this one
         //
         if ( *((RecoveryAbs*)*state) == RecoveryTypes::RecoveryStr(_MutuallyExclusiveRecoveries[index]._CurrentState) )
            break;
      }

      if ( state == _Recoveries.end() )
      {
         DataLog(log_level_proc_error) << "Invalid recovery requested -> " << RecoveryTypes::RecoveryStr(_MutuallyExclusiveRecoveries[index]._CurrentState) << endmsg;
         return 0;
      }
      else
      {
         statePtr = *state;
      }

      if ( _MutuallyExclusiveRecoveries[index]._NextState != RecoveryTypes::None
           && _MutuallyExclusiveRecoveries[index]._NextState != RecoveryTypes::RecoveryNum )
      {
         //
         // Now set up the mutually exclusive list
         //
         list<RecoveryAbs*>::iterator tstate;
         for ( tstate = statePtr->mutuallyExclusiveRecs().begin() ;
               tstate != statePtr->mutuallyExclusiveRecs().end() ;
               tstate++ )
         {
            //
            // We already have created this 'recovery state' in
            // our list, we are done
            //
            if ( *((RecoveryAbs*)*tstate) == RecoveryTypes::RecoveryStr(_MutuallyExclusiveRecoveries[index]._NextState) )
               break;
         }

         if ( tstate == statePtr->mutuallyExclusiveRecs().end() )
         {
            //
            // Check to see if the transition state is in our
            //  'recovery state' list
            //
            list<RecoveryAbs*>::iterator newstate;
            for ( newstate = _Recoveries.begin() ;
                  newstate != _Recoveries.end() ;
                  newstate++ )
            {
               //
               // We already have created this 'recovery state'
               //
               if ( *((RecoveryAbs*)*newstate) == RecoveryTypes::RecoveryStr(_MutuallyExclusiveRecoveries[index]._NextState) )
                  break;
            }

            RecoveryAbs* newStatePtr = 0;
            if ( newstate == _Recoveries.end() )
            {
               DataLog(log_level_proc_error) << "Invalid recovery requested -> " << RecoveryTypes::RecoveryStr(_MutuallyExclusiveRecoveries[index]._CurrentState) << endmsg;
               return 0;
            }
            else
            {
               newStatePtr = *newstate;
            }
            statePtr->mutuallyExclusiveRecs().push_back(newStatePtr);

         }
      }
      index++;
   }

   return 1;
}

void RecoveryMan::dump ()
{

   list<RecoveryAbs*>::iterator state;
   for ( state = _Recoveries.begin() ;
         state != _Recoveries.end() ;
         state++ )
   {
      DataLog(log_level_proc_debug) << "Recovery Name: " << (*state)->recoveryName() << endmsg;
      DataLog(log_level_proc_debug) << "Transitions: ";
      list<RecoveryAbs*>::iterator tstate;
      for ( tstate = ((RecoveryAbs*)*state)->transitions().begin() ;
            tstate != ((RecoveryAbs*)*state)->transitions().end() ;
            tstate++ )
      {
         DataLog(log_level_proc_debug) << (*tstate)->recoveryName() << " ";
      }
      DataLog(log_level_proc_debug) << endmsg;
      DataLog(log_level_proc_debug) << "Queueable Transitions: ";
      for ( tstate = ((RecoveryAbs*)*state)->queueableTransitions().begin() ;
            tstate != ((RecoveryAbs*)*state)->queueableTransitions().end() ;
            tstate++ )
      {
         DataLog(log_level_proc_debug) << (*tstate)->recoveryName() << " ";
      }
      DataLog(log_level_proc_debug) << endmsg;
      DataLog(log_level_proc_debug) << "Stackable Transitions: ";
      for ( tstate = ((RecoveryAbs*)*state)->stackableTransitions().begin() ;
            tstate != ((RecoveryAbs*)*state)->stackableTransitions().end() ;
            tstate++ )
      {
         DataLog(log_level_proc_debug) << (*tstate)->recoveryName() << " ";
      }
      DataLog(log_level_proc_debug) << endmsg;
      DataLog(log_level_proc_debug) << "Queue Consuming Transitions: ";
      for ( tstate = ((RecoveryAbs*)*state)->queueConsumingTransitions().begin() ;
            tstate != ((RecoveryAbs*)*state)->queueConsumingTransitions().end() ;
            tstate++ )
      {
         DataLog(log_level_proc_debug) << (*tstate)->recoveryName() << " ";
      }
      DataLog(log_level_proc_debug) << endmsg;
      DataLog(log_level_proc_debug) << "Mutually Exclusive Recoveries: ";
      for ( tstate = ((RecoveryAbs*)*state)->mutuallyExclusiveRecs().begin() ;
            tstate != ((RecoveryAbs*)*state)->mutuallyExclusiveRecs().end() ;
            tstate++ )
      {
         DataLog(log_level_proc_debug) << (*tstate)->recoveryName() << " ";
      }
      DataLog(log_level_proc_debug) << endmsg;

   }
}

void RecoveryMan::dumpQueue ()
{

   DataLog(log_level_proc_debug) << "Current Recovery Queue: ";
   list<RecoveryAbs*>::iterator state;
   for ( state = _RecoveryQueue.begin() ;
         state != _RecoveryQueue.end() ;
         state++ )
   {
      DataLog(log_level_proc_debug) << (*state)->recoveryName() << "-" << (*state)->recoveryType()
                                    << "-" << (*state)->active() << " ";
   }
   DataLog(log_level_proc_debug) << endmsg;
}

void RecoveryMan::cleanup ()
{
   //
   // Clear the lists ...
   //
   clearQueue();

   list<RecoveryAbs*>::iterator state;
   for ( state = _Recoveries.begin() ;
         state != _Recoveries.end() ;
         state++ )
   {
      delete (*state);
   }
   _Recoveries.clear();

   //
   // Reset the static this pointer ...
   //
   _This = 0;
}

/* FORMAT HASH 054b8207c22c30a84823d8952dea5c3e */
