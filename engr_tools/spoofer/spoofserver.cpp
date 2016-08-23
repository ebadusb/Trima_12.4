// FILENAME: spoofserver.cpp
// $Header: H:/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/spoofserver.cpp 1.17 2004/01/19 19:38:18Z td07711 Exp kgreusb $
// PURPOSE: implements SpoofServer class - serves up requested spoofs
// CHANGELOG:
//   $Log: spoofserver.cpp $
//   Revision 1.17  2004/01/19 19:38:18Z  td07711
//   IT6091 - failed to parse SCM_BSS_OVERFLOW command
//   Revision 1.16  2003/12/18 20:27:14Z  td07711
//   IT6140 - add loggingTest support
//   Revision 1.15  2003/11/11 23:29:09Z  td07711
//   IT6091 - avoid optimizer problem in nullptrRead
//   Revision 1.14  2003/11/11 18:49:21Z  td07711
//   IT6091 - support bssOverflow
//   Revision 1.13  2003/10/28 21:39:33Z  td07711
//   IT6091 - add code to handle new commands for memory protection validation.
//   Revision 1.12  2003/10/24 17:56:36Z  td07711
//   IT6091 - add support for memory protection testing
//   Revision 1.11  2003/09/05 20:58:43Z  td07711
//   IT6394 - support suspending named tasks via spoofer
//   Revision 1.10  2003/07/22 22:28:34Z  td07711
//   IT5303 - support turning spoof_server debug msgs on and off from a spoof_client.
//   Revision 1.9  2003/07/15 17:16:04Z  td07711
//   IT6199 - use predefined datalog_levels for logging
//   Revision 1.8  2003/06/12 22:06:55Z  td07711
//   log warnings instead of error when unable to resolve objectName, since safety doesn't support control side objectNames and vice versa.
//   Revision 1.7  2003/06/03 00:46:35Z  td07711
//   IT6108 - defined fatalError()
//   Revision 1.6  2003/05/29 18:42:28Z  td07711
//   support FATAL_ERROR spoofs
//   Revision 1.5  2003/03/18 02:09:06Z  td07711
//   support msgMultiplier
//   Revision 1.4  2003/03/15 00:00:01Z  td07711
//   stub support for msgMultiplier()
//   Revision 1.3  2003/03/14 22:16:05Z  td07711
//   use datalog directly instead of stderr for a few msgs
//   Revision 1.2  2003/01/15 19:48:05Z  td07711
//   warn, don't abort, recurring initialization
//   Revision 1.1  2002/11/04 17:42:13Z  td07711
//   Initial revision


// vxworks includes
#include <timers.h>

// common includes
#include "message.h"
#include "spoofermessagesystem.h"
#include "timermessage.h"
#include "datalog_levels.h"
#include "error.h"
#include "ostime.hpp"

// libtools includes
#include "spooferlog.hpp"

// spoofer includes
#include "spoofserver.hpp"
#include "spoofercontrol.h"
#include "spoofedmsgbase.hpp"
#include "spoofedobject.hpp"


// initialize statics
bool SpoofServer :: _initFlag = false;


SpoofServer :: SpoofServer( const char* name )
: SpooferAPI() 
{
    LOG_DEBUG("SpoofServer: constructing %s", name);

    init( name );
}


SpoofServer :: ~SpoofServer()
{
    cleanup();
    _initFlag = false;
    delete[] _name;

    LOG_DEBUG("~SpoofServer:destroyed");
}


void SpoofServer :: cleanup()
{
    // PURPOSE: full cleanup

    spoofEnd();
    SpooferAPI::cleanup(); 
}


void SpoofServer :: init( const char* name )
{
    // PURPOSE: init messaging

    if(_initFlag)
    {
        LOG_WARNING("init: already initialized, initializing again");
    }

    TASK_DESC                taskDescriptor;
    int taskID = taskIdSelf();

    // Get the information on the task
    if ( (taskID != 0) && taskInfoGet (taskID, &taskDescriptor) == OK)
    {
        DataLog( log_level_spoofer_info ) << "SpoofServer:" 
            << " stack margin: " << taskDescriptor.td_stackMargin 
            << " stack size: " << taskDescriptor.td_stackSize
            << endmsg;
    }

    _name = new char[ strlen( name ) + 1];
    strcpy( _name, name );

    _taskid = 1; // FIXME
    SpooferAPI::init();
    _initFlag = true;
    
    //
    // setup message system
    //
    _pMessageSystem = new SpooferMessageSystem;
    _pMessageSystem->initBlocking("spooferSlave", 30);

    //
    // register SpooferControl message callback receive only
    //
    _pControl = new Message< SpooferControl > ;
    _pControl->init( Callback<SpoofServer>(this, &SpoofServer::controlHandler), 
                    MessageBase::SNDRCV_RECEIVE_ONLY );

    //
    // register SpooferAck message for sending only
    //
    _pAck = new Message< SpooferAck > ;
    _pAck->init( MessageBase::SEND_GLOBAL );

    LOG_DEBUG("init: init complete");
}


void SpoofServer :: controlHandler()
{
    // PURPOSE: dispatcher callback to handle receipt of SpooferControl messages
    
    const SpooferControl& scm = _pControl->getData();
    
    switch( scm.command )
    {
        case SCM_SPOOF_EXIT :
            spoofExit();
            break;
  
        case SCM_SPOOF_LIMIT :
            setSpoofedMessageLimit( scm.data );
            break;

        case SCM_SPOOF_END :
            spoofEnd();
            break;
            
        case SCM_SPOOF_ACTIVATE :
            spoofActivate();
            break;

        case SCM_SPOOF_DATA :
            if(scm.valueName[0] != 0)
            {
                spoofData( scm.structName, scm.fieldName, scm.valueName );
            }
            else
            {
                spoofData( scm.structName, scm.fieldName, scm.multiplier, scm.offset );
            }
            break;

        case SCM_UNSPOOF_DATA :
            unspoofData( scm.structName, scm.fieldName );
            break;

        case SCM_SHOW_SPOOFS :
            showSpoofs();
            break;

        case SCM_SPOOF_ENABLE :
            spoofEnable();
            break;

        case SCM_SPOOF_DISABLE :
            spoofDisable();
            break;

        case SCM_MSG_MULTIPLIER :
	    msgMultiplier( scm.data );
	    break;

        case SCM_FATAL_ERROR :
	    fatalError( scm.valueName );
	    break;

        case SCM_ENABLE_DEBUG :
	    enableDebug();
	    break;

        case SCM_DISABLE_DEBUG :
	    disableDebug();
	    break;

        case SCM_SUSPEND_TASK :
	    suspend( scm.valueName, scm.fieldName );
	    break;

        case SCM_NULLPTR_READ :
	    nullptrRead( scm.valueName );
	    break;

        case SCM_TEXT_WRITE :
	    textWrite( scm.valueName );
	    break;

        case SCM_STACK_OVERFLOW :
	    stackOverflow( scm.valueName );
	    break;

        case SCM_DATA_OVERFLOW :
	    dataOverflow( scm.valueName );
	    break;

        case SCM_BSS_OVERFLOW :
	    bssOverflow( scm.valueName );
	    break;

        case SCM_LOGGING_TEST :
	    loggingTest( (int)scm.multiplier, scm.data );
	    break;

        default :
            LOG_ERROR( "controlHandler: invalid command %d", (int)scm.command );
            ackSend( scm.taskid, scm.seqnum, SCM_NACK );
            return;
    }

    ackSend(scm.taskid, scm.seqnum);
}


void SpoofServer :: loggingTest( int burst, int duration )
{
    // PURPOSE: logs <burst> msgs each second for <duration> seconds

    int i,j;
    rawTime myRawtime;
    osTime myOstime;
    int msec;
    struct timespec sleepTime;

    for( i = 1; i <= duration; i++ )
    {
	// snapshot time and send a burst
	myOstime.snapshotRawTime( myRawtime );

	for( j = 1; j <= burst; j++ )
	{
	    DataLog( log_level_spoofer_info ) << "Logging Test:  Second " << i 
		<< " of " << burst << "  message " << j << " of " << duration 
        << " from server named " << name() << endmsg;
	}

	// sleep for remainder of the second
	msec = myOstime.howLongMilliSec( myRawtime );

	if( msec > 1000)
        {
	    DataLog( log_level_spoofer_error ) << "Logging Test: burst required " 
		<< msec << " to send from server named " << name() << endmsg;
	}
	else
	{
	    DataLog( log_level_spoofer_info ) << "Logging Test: burst required " 
		<< msec << " to send from server named " << name() << endmsg;
	    sleepTime.tv_sec = 0;
	    sleepTime.tv_nsec = msec * 1000000;
	    nanosleep( &sleepTime, 0 );
	}
    }
}


void SpoofServer :: fatalError( const char* serverName )
{
    // PURPOSE:  call FATAL_ERROR if directed at this server

    if( strcmp( serverName, name() ) == 0 )
    {
	_FATAL_ERROR( __FILE__, __LINE__, "spoofing call to FATAL_ERROR" );
    }
}


void SpoofServer :: stackOverflow( const char* serverName )
{
    // PURPOSE:  read past end of stack to test stack boundary protection

    if( strcmp( serverName, name() ) == 0 )
    {
	DataLog( log_level_spoofer_info ) << "stackOverflow test starting for server " << name() << endmsg;

	int temp;
	int* p = &temp;

	for( int i = 1; i <= 8000; i++ ) // stack size is specified in trima_tasks.h as 30000 bytes
	{
	    temp = *p--; // stack grows downward, protection page at the bottom
	}

	// Log error if still running
	DataLog( log_level_spoofer_error ) << "stackOverflow test failed for server " << name() << endmsg;
    }
}


void SpoofServer :: dataOverflow( const char* serverName )
{
    // PURPOSE:  read past end of data segment to test boundary protection

    if( strcmp( serverName, name() ) == 0 )
    {
	DataLog( log_level_spoofer_info ) << "dataOverflow test starting for " << name() << endmsg;

	static int temp = 1; // initialized data located in data segment
	int* p = &temp;

	for( int i = 1; i <= 4000; i++ ) // data size is presumed less than 16000 bytes
	{
	    temp ^= *p++; // 
	}

	// Log error if still running
	DataLog( log_level_spoofer_error ) << "dataOverflow test failed for " << name() << endmsg;
    }
}


void SpoofServer :: bssOverflow( const char* serverName )
{
    // PURPOSE:  read past end of bss segment to test boundary protection

    if( strcmp( serverName, name() ) == 0 )
    {
	DataLog( log_level_spoofer_info ) << "bssOverflow test starting for " << name() << endmsg;

	static int temp; // uninitialized data located in bss segment
	int* p = &temp;

	for( int i = 1; i <= 4000; i++ ) // bss size is presumed less than 16000 bytes
	{
	    temp ^= *p++; // 
	}

	// Log error if still running
	DataLog( log_level_spoofer_error ) << "bssOverflow test failed for " << name() << endmsg;
    }
}


void SpoofServer :: dummy()
{
    // PURPOSE: provides text address for use by textWrite()

    int temp = 1; // just to do something
}


void SpoofServer :: textWrite( const char* serverName )
{
    // PURPOSE:  attempt to write text area to test text write protection

    if( strcmp( serverName, name() ) == 0 )
    {
	DataLog( log_level_spoofer_info ) << "textWrite test starting for " << name() << endmsg;

	char* p = (char*)&dummy;

	char temp = *p;
	*p = 0; // writes text
	*p = temp; // restores it back in case the write succeeded

	// Log error if still running
	DataLog( log_level_spoofer_error ) << "textWrite test failed for " << name() << endmsg;
    }
}


void SpoofServer :: nullptrRead( const char* serverName )
{
    // PURPOSE:  deref nullptr to test page0 protection

    if( strcmp( serverName, name() ) == 0 )
    {
	DataLog( log_level_spoofer_info ) << "nullptrRead test starting on " << name() << endmsg;

	char* p = 0;

	static char temp = *p; // static keeps optimizer from optimizing the read away

	// Log error if still running
	DataLog( log_level_spoofer_error ) << "nullptrRead test failed on " << name() << endmsg;
    }
}


void SpoofServer :: suspend( const char* serverName, const char* taskName )
{
    // PURPOSE:  call taskSuspend if directed at this server

    if( strcmp( serverName, name() ) == 0 )
    {
	int tid = taskNameToId( (char*)taskName );
	if( tid != ERROR )
	{
	    if( taskSuspend( tid ) != ERROR )
	    {
		DataLog( log_level_spoofer_info ) << taskName << " suspended on " << name() << endmsg;
	    }
	    else
	    {
		DataLog( log_level_spoofer_error ) << "taskSuspend failed for " << taskName << " on server " << name() << endmsg;
	    }
	}
	else
	{
	    DataLog( log_level_spoofer_error ) << "taskNameToId failed for " << taskName << " on server " << name() << endmsg;
	}
    }
}


void SpoofServer :: spoofEnd()
{
    // PURPOSE: ends spoofing, but continue handling SpooferControl messages

    DataLog( log_level_spoofer_info ) << "spoofEnd" << endmsg;

    SpoofedObject::disableSpoofing();
    SpoofedObject::cleanup();
    SpoofedMsgBase::setMsgMultiplier( 1 );
    SpoofedMsgBase::setMaxSpoofedMsgs( 0 );
}


void SpoofServer :: spoofExit()
{
    // PURPOSE: cleanup and exit
    // ends spoofing, drops out of message loop

    spoofEnd();
    exitMessageLoop();
}


void SpoofServer :: showSpoofs()
{
    // PURPOSE: sets flag to show spoofs as they occur
    
    SpoofedObject::showSpoofs();
}


void SpoofServer :: enableDebug()
{
	log_level_spoofer_debug.logOutput(DataLog_LogEnabled);
}


void SpoofServer :: disableDebug()
{
	log_level_spoofer_debug.logOutput(DataLog_LogDisabled);
}


void SpoofServer :: spoofActivate()
{
    // PURPOSE: activates pending spoofs
    // FIXME - currently just enables spoofing
    // intent is to eventually support activating pending spoofs,
    // then allow more spoofs to be setup but not activated until
    // this call is made again.  Will require managing both an active and an 
    // inactive Spoof.
    
    DataLog( log_level_spoofer_info ) << "spoofActivate" << endmsg;

    SpoofedObject::enableSpoofing();
}


void SpoofServer :: unspoofData( const char* objectName, const char* elementName )
{
    // PURPOSE: stop spoofing particular objectName.elementName

    SpoofedObject* p = SpoofedObject::find( objectName );

    if(p == 0)
    {
        LOG_WARNING( "unspoofData: %s.%s object not being spoofed",
                    objectName, elementName );
    }
    else
    {
        p->removeSpoof( elementName );

        if( p->elements() == 0 )
        {
            SpoofedObject::remove( objectName );
        }
    }

    LOG_DEBUG( "unspoofData: stopped spoofing %s.%s", objectName, elementName );
}


void SpoofServer :: spoofData(const char* objectName, const char* dataName, const char* valueName)
{
    // PURPOSE: spoof stuctName.fieldName with valueName

    SpoofedObject* p = SpoofedObject::find( objectName );

    if(p == 0)
    {
        p = SpoofedObject::create( objectName );
    }

    if( p )
    {
        p->addSpoof( dataName, valueName );
        return;
    }
}


void SpoofServer :: spoofData(const char* objectName, const char* dataName, float mult, float offset)
{
    // PURPOSE: spoof stuctName.fieldName with multiplier and offset

    SpoofedObject* p = SpoofedObject::find( objectName );

    if(p == 0) 
    {
        p = SpoofedObject::create( objectName );
    }

    if( p )
    {
        p->addSpoof( dataName, mult, offset );
        return;
    }
}


void SpoofServer :: ackSend(int taskid, int seqnum, SCM_COMMAND command)
{
    // PURPOSE: sends an ack/nack for last received spoofer control message
    ASSERT(command == SCM_ACK || command == SCM_NACK);

    _ack.command = command;
    _ack.taskid = _taskid;
    _ack.ackSeqnum = seqnum;
    _ack.ackTaskid = taskid;
    _pAck->send(_ack);
}


void SpoofServer :: ackTimeoutHandler()
{
    // PURPOSE: stub, currently no acks

    LOG_ERROR("ackTimeoutHandler: stub should not get called");
}


void SpoofServer :: setSpoofedMessageLimit( int n )
{
    // PURPOSE: sets limit on number of messages spoofed

    SpoofedMsgBase::setMaxSpoofedMsgs( n );
}


void SpoofServer :: msgMultiplier( int n )
{
    // PURPOSE: halts or replicates messages spoofed

    SpoofedMsgBase::setMsgMultiplier( n );
}


void SpoofServer :: spoofEnable()
{
    SpoofedObject::enableSpoofing();
}


void SpoofServer :: spoofDisable()
{
    SpoofedObject::disableSpoofing();
}
