// FILENAME: spoofclient.cpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/spoofclient.cpp 1.13 2003/11/11 18:48:50Z td07711 Exp td07711 $
// PURPOSE: provides API for setting up and controlling a spoof
// CHANGELOG:
//   $Log: spoofclient.cpp $
//   Revision 1.13  2003/11/11 18:48:50Z  td07711
//   IT6091 - support bssOverflow
//   Revision 1.12  2003/10/24 17:56:52Z  td07711
//   IT6091 - add support for memory protection testing
//   Revision 1.11  2003/09/05 20:58:11Z  td07711
//   IT6394 - support suspending named tasks via spoofer
//   Revision 1.10  2003/07/22 22:28:02Z  td07711
//   IT5303 - support turning spoof_server debug msgs on and off from a spoof_client.
//   Revision 1.9  2003/07/18 18:02:57Z  td07711
//   IT6217 - enable message delivery to dispatcher prior to entering the dispatch loop.  This avoids race with spoof_server acknowledging spoof_client's 1st message before spoof_client has enabled message delivery to itself by entereing its dispatch loop.
//   Revision 1.8  2003/07/17 23:34:57Z  td07711
//   IT6217 - log ackTimeouts to rundata
//   Revision 1.7  2003/07/17 20:20:44Z  td07711
//   IT6217 - add debug info to ackTimeout error msg
//   Revision 1.6  2003/05/29 18:41:50Z  td07711
//   support FATAL_ERROR spoofs
//   Revision 1.5  2003/03/14 23:48:11Z  td07711
//   add msgMultiplier() function
//   Revision 1.4  2003/02/06 23:28:03Z  td07711
//   bugfix - reinit msg system after task crash
//   Revision 1.3  2003/01/16 00:37:31Z  td07711
//   bugfix - can't delete timer in timer's timeout function
//   Revision 1.2  2002/11/13 23:42:55Z  td07711
//   make MessageSystem static for getData() support
//   Revision 1.1  2002/11/04 17:42:12  td07711
//   Initial revision


// common includes
#include "message.h"
#include "messagesystem.h"
#include "timermessage.h"
#include "datalog_levels.h"

// spoofer includes
#include "spoofclient.hpp"
#include "spoofercontrol.h"
#include "spooferlog.hpp"

// initialize statics
bool SpoofClient :: _initFlag = false;


SpoofClient :: SpoofClient( int numSlaves )
: SpooferAPI(), _lastSeqnum(0), _pendingSeqnum(0), _pendingCount(0),
  _numSlaves( numSlaves )
{
    LOG_DEBUG("SpoofClient: constructed taskid = %d", _taskid);
    
    cleanup();
    init();
}


SpoofClient :: ~SpoofClient()
{
    cleanup();
    _initFlag = false;

    LOG_DEBUG("~SpoofClient:destroyed");
}


void SpoofClient :: cleanup()
{
    SpooferAPI::cleanup();
}


void SpoofClient :: init()
{
    // PURPOSE: init messaging

    // enforce single instance
    if(_initFlag)
    {
        LOG_WARNING("init: already initialized, initializing again");
    }

    _taskid = 2; // FIXME get real taskid
    SpooferAPI::init(); // base class init
    _initFlag = true;

    //
    // setup message system
    //
    if( _pMessageSystem == 0 )
    {
        _pMessageSystem = new MessageSystem;
        _pMessageSystem->initBlocking("spooferMaster", 30);
    }

    //
    // register SpooferControl message send only
    //
    _pControl = new Message< SpooferControl > ;
    _pControl->init( MessageBase::SEND_GLOBAL );

    //
    // register SpooferAck message callback receive only
    //
    _pAck = new Message< SpooferAck > ;
    _pAck->init( Callback<SpoofClient>(this, &SpoofClient::ackHandler), 
                    MessageBase::SNDRCV_RECEIVE_ONLY );

    //
    // IT6217 - enable dispatcher to receive messages from router (prior to entering dispatch loop)
    //
    _pMessageSystem->dispatcher().enableQueue();

    LOG_DEBUG("init: init complete");
}


void SpoofClient :: ackHandler()
{
    // PURPOSE:  dispatcher callback to handle SpooferAck messages
    // drops out of dispatcher when pending acks are received or timeout

    const SpooferAck& ack = _pAck->getData();

    //
    // ignore acks that we're not waiting for
    //
    if(ack.ackTaskid != _taskid)
    {
        LOG_WARNING("ackHandler: IGNORED, ackTaskid=%d waiting for %d", 
                  ack.ackTaskid, _taskid);
        return;
    }
    if(ack.ackSeqnum != _pendingSeqnum)
    {
        LOG_WARNING("ackHandler: IGNORED, ackSeqnum=%d waiting for %d", 
                  ack.ackSeqnum, _pendingSeqnum);
        return;
    }

    //
    // if nack, then error message
    //
    if(ack.command == SCM_NACK)
    {
        LOG_ERROR("ackHandler: NACK received from %d", ack.taskid);
    }

    //
    // ack/nack received
    //
    _pendingCount--;
    if(_pendingCount > 0)
    {
        LOG_DEBUG("ackHandler: %d acks still pending", _pendingCount);
        return;
    }

    //
    // all acks received
    //
    _pendingSeqnum = 0;
    
    // disarm timer and set for cleanup
    _pAckTimer->interval( 0 );
    _ackTimerCleanupPending = true;

    exitMessageLoop();
}


void SpoofClient :: ackTimeoutHandler()
{
    // PURPOSE: if acks aren't received as expected, error msg and drop out of dispatcher

    LOG_ERROR("ackTimeoutHandler:  seqnum=%d count=%d", _pendingSeqnum, _pendingCount);
    
    DataLog( log_level_spoofer_info ) << "ackTimeout: seqnum=" 
	<< _pendingSeqnum << endmsg;

    _pendingSeqnum = 0;
    _pendingCount = 0;

    // disarm timer and set for cleanup
    _pAckTimer->interval( 0 );
    _ackTimerCleanupPending = true;

    exitMessageLoop();
}


void SpoofClient :: spoofEnd()
{
    // PURPOSE: ends spoofing

    if( timedOut() )
    {
        setTimeout( 1 ); // just allow time to end the spoof
    }

    _scm.command = SCM_SPOOF_END;
    scmSend();
}


void SpoofClient :: spoofEnable()
{
    // PURPOSE: enables spoofing via flag

    _scm.command = SCM_SPOOF_ENABLE;
    scmSend();
}


void SpoofClient :: spoofDisable()
{
    // PURPOSE: disables spoofing via flag, low overhead

    _scm.command = SCM_SPOOF_DISABLE;
    scmSend();
}


void SpoofClient :: showSpoofs()
{
    // PURPOSE: show spoofs as they occur

    _scm.command = SCM_SHOW_SPOOFS;
    scmSend();
}


void SpoofClient :: enableDebug()
{
    // PURPOSE: enable debug msgs

    _scm.command = SCM_ENABLE_DEBUG;
    scmSend();
}


void SpoofClient :: disableDebug()
{
    // PURPOSE: disable debug msgs

    _scm.command = SCM_DISABLE_DEBUG;
    scmSend();
}


void SpoofClient :: spoofActivate()
{
    // PURPOSE: activate latest spoofs

    _scm.command = SCM_SPOOF_ACTIVATE;
    scmSend();
}


void SpoofClient :: unspoofData( const char* objectName, const char* elementName )
{
    // PURPOSE: quit spoofing objectName.elementName

    _scm.command = SCM_UNSPOOF_DATA;

    strncpy(_scm.structName, objectName, sizeof( _scm.structName ));
    _scm.structName[ sizeof(_scm.structName) - 1 ] = 0;

    strncpy(_scm.fieldName, elementName, sizeof( _scm.fieldName ));
    _scm.fieldName[ sizeof(_scm.fieldName) - 1 ] = 0;

    scmSend();
}


void SpoofClient :: spoofData(const char* structName, const char* fieldName, const char* valueName)
{
    // PURPOSE: spoof stuctName.fieldName with valueName
    _scm.command = SCM_SPOOF_DATA;

    strncpy(_scm.structName, structName, sizeof( _scm.structName ));
    _scm.structName[ sizeof(_scm.structName) - 1 ] = 0;

    strncpy(_scm.fieldName, fieldName, sizeof( _scm.fieldName ));
    _scm.fieldName[ sizeof(_scm.fieldName) - 1 ] = 0;

    strncpy(_scm.valueName, valueName, sizeof( _scm.valueName ));
    _scm.valueName[ sizeof(_scm.valueName) - 1 ] = 0;

    _scm.multiplier = 0;
    _scm.offset = 0;

    scmSend();
}


void SpoofClient :: spoofData(const char* structName, const char* fieldName, float mult, float offset)
{
    // PURPOSE: spoof stuctName.fieldName with valueName
    _scm.command = SCM_SPOOF_DATA;

    strncpy(_scm.structName, structName, sizeof( _scm.structName ));
    _scm.structName[ sizeof(_scm.structName) - 1 ] = 0;

    strncpy(_scm.fieldName, fieldName, sizeof( _scm.fieldName ));
    _scm.fieldName[ sizeof(_scm.fieldName) - 1 ] = 0;

    memset(_scm.valueName, 0, sizeof( _scm.valueName ));

    _scm.multiplier = mult;
    _scm.offset = offset;

    scmSend();
}


void SpoofClient :: scmSend( int ackTimeout )
{
    // PURPOSE: sends a spoofer control message

    if( timedOut() )
    {
        return;
    }

    _scm.taskid = _taskid;
    _scm.seqnum = ++_lastSeqnum;
    _pendingSeqnum = _scm.seqnum;
    _pendingCount = _numSlaves;

    _pControl->send(_scm);
    enterMessageLoop( ackTimeout ); 
}


void SpoofClient :: setSpoofedMessageLimit( int n )
{
    // PURPOSE: limits spoofing per processor to n more spoofs

    _scm.command = SCM_SPOOF_LIMIT;
    _scm.data = n;
    scmSend();
}


void SpoofClient :: msgMultiplier( int mult )
{
    // PURPOSE: sends spoofed msgs this many times

    _scm.command = SCM_MSG_MULTIPLIER;
    _scm.data = mult;
    scmSend();
}


void SpoofClient :: spoofExit()
{
    // PURPOSE: end spoofing and exit

    _scm.command = SCM_SPOOF_EXIT;
    scmSend();

    cleanup();
    exit(0);
}


void SpoofClient :: fatalError( const char* serverName )
{
    // PURPOSE: specified spoof server calls fatal error
    _scm.command = SCM_FATAL_ERROR;

    // passes serverName in _scm.valueName
    strncpy(_scm.valueName, serverName, sizeof( _scm.valueName ));
    _scm.valueName[ sizeof(_scm.valueName) - 1 ] = 0;

    scmSend();
}


void SpoofClient :: nullptrRead( const char* serverName )
{
    // PURPOSE: specified spoof server calls nullptrRead
    _scm.command = SCM_NULLPTR_READ;

    // passes serverName in _scm.valueName
    strncpy(_scm.valueName, serverName, sizeof( _scm.valueName ));
    _scm.valueName[ sizeof(_scm.valueName) - 1 ] = 0;

    scmSend();
}


void SpoofClient :: textWrite( const char* serverName )
{
    // PURPOSE: specified spoof server calls textWrite
    _scm.command = SCM_TEXT_WRITE;

    // passes serverName in _scm.valueName
    strncpy(_scm.valueName, serverName, sizeof( _scm.valueName ));
    _scm.valueName[ sizeof(_scm.valueName) - 1 ] = 0;

    scmSend();
}


void SpoofClient :: stackOverflow( const char* serverName )
{
    // PURPOSE: specified spoof server calls stackOverflow
    _scm.command = SCM_STACK_OVERFLOW;

    // passes serverName in _scm.valueName
    strncpy(_scm.valueName, serverName, sizeof( _scm.valueName ));
    _scm.valueName[ sizeof(_scm.valueName) - 1 ] = 0;

    scmSend();
}


void SpoofClient :: dataOverflow( const char* serverName )
{
    // PURPOSE: specified spoof server calls dataOverflow
    _scm.command = SCM_DATA_OVERFLOW;

    // passes serverName in _scm.valueName
    strncpy(_scm.valueName, serverName, sizeof( _scm.valueName ));
    _scm.valueName[ sizeof(_scm.valueName) - 1 ] = 0;

    scmSend();
}


void SpoofClient :: bssOverflow( const char* serverName )
{
    // PURPOSE: specified spoof server calls bssOverflow
    _scm.command = SCM_BSS_OVERFLOW;

    // passes serverName in _scm.valueName
    strncpy(_scm.valueName, serverName, sizeof( _scm.valueName ));
    _scm.valueName[ sizeof(_scm.valueName) - 1 ] = 0;

    scmSend();
}


void SpoofClient :: suspend( const char* serverName, const char* taskName )
{
    // PURPOSE: specified spoof server calls taskSuspend
    _scm.command = SCM_SUSPEND_TASK;

    // passes serverName in _scm.valueName
    strncpy(_scm.valueName, serverName, sizeof( _scm.valueName ));
    _scm.valueName[ sizeof(_scm.valueName) - 1 ] = 0;

    // passes taskName is _scm.fieldName
    strncpy(_scm.fieldName, taskName, sizeof( _scm.fieldName ));
    _scm.fieldName[ sizeof(_scm.fieldName) - 1 ] = 0;

    scmSend();
}


void SpoofClient :: loggingTest( int burst, int duration )
{
    // PURPOSE: all spoof servers respond to this request
    //  each server logs <burst> msgs each second for <duration> seconds
    _scm.command = SCM_LOGGING_TEST;
    _scm.multiplier = burst;
    _scm.data = duration;

    scmSend();
}
