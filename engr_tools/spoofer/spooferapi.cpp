// FILENAME: spooferapi.cpp
// $Header: H:/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/spooferapi.cpp 6.20 2008/08/15 22:07:44Z kgreusb Exp kgreusb $
// PURPOSE: provides API for setting up and controlling a spoof
// CHANGELOG:
//   $Log: spooferapi.cpp $
//   Revision 6.20  2008/08/15 22:07:44Z  kgreusb
//   adding waitForSubstate member as part of Next Genration spoofer project
//   Revision 6.19  2003/06/12 22:07:51Z  td07711
//   log warnings instead of errors when unable to resolve objectName since safety doesn't support control side objectNames and vice versa.
//   Revision 6.18  2003/04/02 00:10:42Z  td07711
//   support spoof timeout value of 0 - means no timeout
//   Revision 6.17  2003/02/13 00:23:53Z  td07711
//   bugfix - don't sleep if timed out
//   Revision 6.16  2003/01/29 22:19:00Z  td07711
//   function renamed
//   Revision 6.15  2003/01/17 02:26:03Z  td07711
//   bugfix - delete timers before deleting messageSystem
//   Revision 6.14  2003/01/17 01:57:30Z  td07711
//   use timedOut() function
//   Revision 6.13  2003/01/15 19:45:48Z  td07711
//   bugfix - don't delete timer from timer's timeout function
//   Revision 6.12  2002/11/21 18:44:14Z  td07711
//   var name change
//   Revision 6.11  2002/11/21 17:26:05  td07711
//   add NamedCds supporr
//   Revision 6.10  2002/11/21 16:46:25  td07711
//   replaced NamedMsgBase with NamedObject
//   Revision 6.9  2002/11/13 23:44:44  td07711
//   add getData() support
//   Revision 6.8  2002/10/31 22:49:01  td07711
//   change timerExpired() to timedOut()
//   Revision 6.7  2002/10/24 23:03:52  td07711
//   add timeout support
//   Revision 6.6  2002/10/09 21:04:17  td07711
//   debug msg
//   Revision 6.5  2002/10/08 23:43:00  td07711
//   remove a debug msg
//   Revision 6.4  2002/10/04 21:16:39  td07711
//   replace FieldInfo with SpoofableData
//   Revision 6.3  2002/09/27 19:39:19  td07711
//   renamed intSend() to messageSend()
//   Revision 6.2  2002/09/26 21:50:51  td07711
//   setSpoofLimit() implementaiton moved to derived classes
//   added intSend() for sending an int message
//   Revision 6.1  2002/09/24 23:53:23  td07711
//   add messageSend()
//   add setSpoofLimit()
//   Revision 1.2  2002/09/18 23:29:39  td07711
//   Revision 1.1  2002/09/17 23:51:29  td07711
//   Initial revision


// common includes
#include "message.h"
#include "messagesystem.h"
#include "timermessage.h"

// libtools includes
#include "spooferlog.hpp"

// spoofer includes
#include "spooferapi.hpp"
#include "spoofabledata.hpp"
#include "namedobject.hpp"

#include <string>

// initialize statics
MessageSystem* SpooferAPI::_pMessageSystem = 0;

// for now these are global - no time/budget to work on them - just be aware for now
Message< ProcState > _SpooferAPIProcStateMsg;
const char *_targetProcState;
bool rval = false;


SpooferAPI :: SpooferAPI( )
: _pAck( 0 ),  _pControl( 0 ),
  _pTimeout( 0 ), _timeoutCleanupPending( false ), 
  _pAckTimer( 0 ), _ackTimerCleanupPending( false ), 
  _pSleepTimer( 0 ), _sleepTimerCleanupPending( false ),
  _timedOut( false )
{
    SpoofableData::cleanup(); // potential cleanup from prior aborted run
}


SpooferAPI :: ~SpooferAPI( )
{
    cleanup();
}


void SpooferAPI::cleanup( )
{
    SpoofableData::cleanup();

    // local cleanup
    LOG_DEBUG("cleanup: deleting messages and msg system");
    delete _pAck            ; _pAck = 0;
    delete _pAckTimer       ; _pAckTimer = 0;
    delete _pTimeout        ; _pTimeout = 0;
    delete _pControl        ; _pControl = 0;
    // must delete timers before deleting messageSystem
    delete _pSleepTimer     ; _pSleepTimer = 0;
    delete _pMessageSystem  ; _pMessageSystem = 0;
}


void SpooferAPI :: init( )
{
    //
    // define stuff that can be spoofed
    //
    SpoofableData::initdb();

}


void SpooferAPI :: exitMessageLoop( )
{
    // PURPOSE: exits the message loop

    LOG_DEBUG( "exitMessageLoop:" );
    _ackTimerCleanupPending = true; // ackTimer needs cleanup after exiting dispatcher
    _pMessageSystem->stopLoop(); // drop out of dispatcher
}


void SpooferAPI :: enterMessageLoop( int seconds )
{
    // PURPOSE: enter the message loop

    LOG_DEBUG("enterMessageLoop: %d", seconds);

    if( timedOut() )
    {
        LOG_ERROR("enterMessageLoop: failed, already timed out");
        return;
    }


    //
    // start new ack timeout if necessary
    //
    if( seconds != 0 ) // stop any active timer
    {
        delete _pAckTimer;
        _pAckTimer = 0;
    }
    //
    // start a new ack timeout if necessary
    // same overhead as re-arming an existing one, and no race condition
    //
    if( seconds > 0 )
    {
        _pAckTimer = new TimerMessage( seconds * 1000, 
                            Callback<SpooferAPI>(this, &SpooferAPI::ackTimeoutHandler),
                            TimerMessage::ARMED );
    }

    _pMessageSystem->allowLoop();
    _pMessageSystem->dispatchMessages(); // block in dispatcher

    // 
    // cleanup any expiring timers
    //
    if( _ackTimerCleanupPending )
    {
	delete _pAckTimer;
        _pAckTimer = 0;
	_ackTimerCleanupPending = false;
    }
    if( _sleepTimerCleanupPending )
    {
	delete _pSleepTimer;
        _pSleepTimer = 0;
	_sleepTimerCleanupPending = false;
    }
    if( _timeoutCleanupPending )
    {
	delete _pTimeout;
        _pTimeout = 0;
	_timeoutCleanupPending = false;
    }
}


void SpooferAPI :: messageSend( const char* messageName, const void* pData, int size )
{
    // PURPOSE: creates a message by name, and sends specified payload

    NamedObject* p = NamedObject::create(messageName);

    if( p != 0 )
    {
       p->send( pData, size );
       delete p;
    }
    else
    {
       LOG_INFO( "messageSend: messageName not found: %s", messageName );
    }
}


void SpooferAPI :: messageSend( const char* messageName, int data )
{
    // PURPOSE: create int message by name, and send specified data

    int dataCopy = data;

    LOG_INFO( "messageSend: %s intdata=%d", messageName, dataCopy );

    messageSend( messageName, &dataCopy, sizeof(dataCopy) );
}


void SpooferAPI :: timeoutHandler()
{
    // PURPOSE: times out the spoof, no more message processing

    LOG_DEBUG( "timeoutHandler:" );

    _timedOut = true;

    // 
    // stop all timers and set for cleanup
    //
    if( _pTimeout )
    {
	_pTimeout->interval( 0 );
	_timeoutCleanupPending = true;
    }
    if( _pAckTimer )
    {
	_pAckTimer->interval( 0 );
	_ackTimerCleanupPending = true;
    }
    if( _pSleepTimer )
    {
	_pSleepTimer->interval( 0 );
	_sleepTimerCleanupPending = true;
    }

    exitMessageLoop();
}


void SpooferAPI :: setTimeout( int seconds )
{
    // PURPOSE: sets a timer to timeout the spoof

    // remove any active timer
    if( _pTimeout )
    {
        delete _pTimeout;
        _pTimeout = 0;
    }

    // sets new timer
    _timeoutCleanupPending = false; // set flag first
    _timedOut = false;

    if( seconds > 0 ) // timeout of 0 is forever (i.e. no timer needed)
    {
	_pTimeout = new TimerMessage( seconds * 1000, 
			    Callback<SpooferAPI>(this, &SpooferAPI::timeoutHandler),
			    TimerMessage::ARMED );
    }
}


void SpooferAPI :: waitForTimeout()
{
    // PURPOSE: sits in message loop until timeout

    while( !timedOut() )
    {
        enterMessageLoop();
    }

}


void SpooferAPI :: sleepMsec( int msec )
{
    // PURPOSE: sleeps here for specified interval in msec

    if( timedOut() )
    {
	return;
    }

    ASSERT( _pSleepTimer == 0 );

    // sets new timer
    _sleepTimerCleanupPending = false;
    _pSleepTimer = new TimerMessage( msec, 
                        Callback<SpooferAPI>(this, &SpooferAPI::sleepTimeoutHandler),
                        TimerMessage::ARMED );

    while( _pSleepTimer && !timedOut() )
    {
        enterMessageLoop();
    }

}


void SpooferAPI :: sleepTimeoutHandler()
{
    // PURPOSE: end of sleep, back to processing

    LOG_DEBUG("sleepTimeoutHandler:");

    _pSleepTimer->interval( 0 ); // stops the timer
    _sleepTimerCleanupPending = true;

    exitMessageLoop();
}


bool SpooferAPI :: getData( const char* objectName, const char* dataName, 
                           void* buf, int size, int timeoutMsec )
{
    // PURPOSE: returns specified data to caller's buffer
    // CDS data is returned immediately
    // Message data is returned following receipt of message
    // If timeout waiting for message, returns false

    ASSERT( objectName );
    ASSERT( dataName );

    const SpoofableData* pSpoofableData = SpoofableData::get( objectName, dataName );
    if( pSpoofableData == 0 )
    {
        LOG_ERROR( "getData: %s.%s are not valid names", objectName, dataName );
        return false;
    }
    if( pSpoofableData->size() != size )
    {
        LOG_ERROR( "getData: %s.%s buffer size %d not equal to data size %d",
                  objectName, dataName, size, pSpoofableData->size() );
        return false;
    }

    NamedObject* p = NamedObject::create( objectName);
    bool rval = p->getData( pSpoofableData, buf, timeoutMsec );
    delete p; 
        
    return rval;
}

bool SpooferAPI :: waitForSubstate( const char* waitForSS,  void* buf, int bufsize, int maxWait)
{
    // PURPOSE: returns true if new substate message was received within the maxWait time (milliseconds)
    // or false if there was an error or the message was not recieved in time

    // for now will assume this is only called to wait substate/state message and not other triggering messages
    // assuming getData member will be used to wait for triggering messages. 

    const char *substateMsgName = "ProcSubstateChangeMsg";
    const char *stateMsgName = "SystemStateChangeMsg";
    const char *procStateMsgName = "ProcStateMsg";
    const char *dataName = "data";

	_targetProcState = waitForSS;
        
    ASSERT( dataName );

    bool isSubstate = false; // the assumption is that this is a substate & using ProcSubstateChangeMsg
    bool isState = false;
    bool isProcState = false;

    // Need to verify the state/substate name which was passed
    EnumInfo *eData;
    int lookFor = 0;
    eData = EnumInfo::get ("Substate_names" );
    if (eData == 0)
    {
        LOG_ERROR( "waitForSubstate: Substate names not returned by EnumInfo::get('Substate_names') " );
        return false;
    }
    lookFor = eData->getValue( waitForSS);
    LOG_DEBUG( "lookFor substate is %d",lookFor);

    if (lookFor <= 0)
    {
        eData = EnumInfo::get ("State_names" );
        if (eData == 0)
        {
            LOG_ERROR( "waitForSubstate: State names not returned by EnumInfo::get('State_names') " );
            return false;
        }
        lookFor = eData->getValue( waitForSS);
        LOG_DEBUG( " lookFor state is %d", lookFor);
        if (lookFor <= 0)
        {
            eData = EnumInfo::get ("ProcState_names" );

			if (eData == 0)
            {
                LOG_ERROR( "waitForSubstate: ProcState names not returned by EnumInfo::get('ProcState_names') ");
                return false;
            }

			lookFor = eData->getValue( waitForSS);
            
            if( lookFor <= 0 )
            {
                LOG_ERROR( "Substate_name/State_name/ProcState_name is not a valid name" );
                return false;
            }
            else
			{
				isProcState = true;
				LOG_DEBUG( "substate name found in ProcState_names ENUM" );
			}
        }
        else 
		{
			isState = true;
			LOG_DEBUG( "substate name found in State_names ENUM" );
		}
    }
    else 
	{
		isSubstate = true;
		LOG_DEBUG( "substate name found in Substate_names ENUM" );
	}

 

    const SpoofableData* pSpoofableData;
    if (isSubstate)
    { 
        pSpoofableData = SpoofableData::get ( substateMsgName, dataName );
    }
    else if( isState )
    {
        pSpoofableData = SpoofableData::get( stateMsgName, dataName );
    }
    else if( isProcState )
    {
        pSpoofableData = SpoofableData::get( procStateMsgName, dataName );
    }
    else
    {
        pSpoofableData = 0;
        LOG_ERROR( "waitForSubstate: %s.%s.%s are not valid message names", substateMsgName, stateMsgName, procStateMsgName );
        return false;
    }

    if( pSpoofableData->size() != bufsize )
    {
        LOG_ERROR( "getData: buffer size %d not equal to data size %d",
               bufsize, pSpoofableData->size() );
        return false;
    }

    NamedObject *p; 

    if (isSubstate)
    {
        p = NamedObject::create( substateMsgName );

        if (p == 0)
        {
            LOG_DEBUG( "waitForSubstate: -beginSubstate parameter was not in SubState_names enum" );
            return false;
        }
    }
    else if( isState )
    {
        p = NamedObject::create( stateMsgName );

        if (p==0)
        {
            LOG_DEBUG( "waitForSubstate: -beginSubstate parameter was not in SubState_names enum" );
            return false;
        }
    }
    else if( isProcState )
    {
		_SpooferAPIProcStateMsg.init( Callback< SpooferAPI >( this, &SpooferAPI::handleProcStateMessage ), MessageBase::SNDRCV_RECEIVE_ONLY );

		p = 0;

		// enter message loop
        SpooferAPI::messageSystem().allowLoop();
        SpooferAPI::messageSystem().dispatchMessages(); // block in dispatcher

		_SpooferAPIProcStateMsg.deregisterMsg();
        
        return rval;
    }
    else
    {
        p = 0;
        LOG_ERROR( "waitForSubstate: Invalid message " );
        return false; 
    }

    if ( !isProcState )
    {
            rval = p->getExactData(pSpoofableData, buf, maxWait, lookFor );

            int *sawValuePtr = (int *)buf;
            int sawValue = *sawValuePtr;
            
            if (rval && (sawValue == lookFor))
            {
                LOG_DEBUG ("waitForSubstate: saw substate/state value %d was looking for", sawValue );
                delete p;
                return rval;
            }
            else if (rval)
            {
                LOG_DEBUG( "value seen %d did not match the value %d looking for", sawValue, lookFor );
                rval = false;
            }
    }
    delete p;

    return rval;
}

void SpooferAPI :: handleProcStateMessage()
{
    int debugCounter = 0;
    char _currentProcState[ NAME_SIZE + 1 ];

	strcpy(_currentProcState, _SpooferAPIProcStateMsg.getData().stateName());
	
	if ( !( strstr( _currentProcState, _targetProcState ) == NULL ) ) // Check if _targetProcState string is in _currentProcState string, like "MIDDLE" in "LOWER MIDDLE UPPER"
	{
        LOG_DEBUG("waitforsubstate: ProcState matched, spoof triggering imminent" );

        rval = true;
		delete _pTimeout;
		_pTimeout = 0;

		SpooferAPI::messageSystem().stopLoop();
	}
	else 
	{
		debugCounter++;

		if( debugCounter % 4 ) //log only every 2 seconds to avoid excessive overhead by logging every half second
		{
			LOG_DEBUG( "waitforsubstate: no ProcState match yet, count: %d", debugCounter);
			LOG_DEBUG( "_currentProcState = %s, _targetProcState = %s", _currentProcState, _targetProcState );
		}
	}
}
