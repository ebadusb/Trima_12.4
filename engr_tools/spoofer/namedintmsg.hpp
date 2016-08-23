// FILENAME: namedintmsg.hpp
// $Header: H:/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/namedintmsg.hpp 6.9 2008/08/15 21:52:08Z kgreusb Exp kgreusb $
// PURPOSE: provides templated int msg by name
// CHANGELOG:
//   $Log: namedintmsg.hpp $
//   Revision 6.9  2008/08/15 21:52:08Z  kgreusb
//   Changes for next Generation spoofer - adding getExactData
//   Revision 6.8  2008/01/23 17:22:44Z  dslausb
//   IT 8435 - VxWorks Checkin
//   Revision 6.7  2002/11/20 22:50:28Z  td07711
//   replace NamedMsgBase with NamedObject
//   Revision 6.6  2002/11/20 21:08:57  td07711
//   delete _pTimeout in destructor
//   Revision 6.5  2002/11/14 22:02:16  td07711
//   init msg SNDRCV to support getData()
//   Revision 6.4  2002/11/13 23:40:57  td07711
//   add getData()
//   Revision 6.3  2002/10/17 23:00:57  td07711
//   use default msg constructor
//   Revision 6.2  2002/10/09 21:00:29  td07711
//   remove some code
//   Revision 6.1  2002/10/08 23:47:16  td07711
//   replace strings with const char*
//   Revision 1.1  2002/09/26 22:03:09  td07711
//   Initial revision


#ifndef NamedIntMsg_HPP // prevents multiple inclusion
#define NamedIntMsg_HPP

#include "spooferlog.hpp"

#include "message.h"
#include "spoofermessagesystem.h"
#include "timermessage.h"


template < class T >
class NamedIntMsg : public NamedObject
{
    // RESPONSIBILITIES:
    //   1. implements NamedObject::send()
    //   2. registers the int message type with the Message System
    
    public:

        NamedIntMsg( const char* name );
        virtual ~NamedIntMsg();


        //
        // base class function overides
        //
        void send( const void* pData, int size ); // sends data in message
        bool getData( const SpoofableData* pDataInfo, void* buf, int timeoutMsec );
        bool getExactData( const SpoofableData* pDataInfo, void* buf, int timeoutMsec, int matchValue );

    protected:
        int _matchValue;

    private:

        void notify();
        // void notifyExact();
        void timeout();
        TimerMessage* _pTimeout;

        T _msg; // provides templated message interface
        int _tbuf; // transmit buffer
     
        NamedIntMsg(const NamedIntMsg&); // catch unauthorized use 
        NamedIntMsg& operator=(const NamedIntMsg& rhs); // catch unauthorized use
};


template < class T >
NamedIntMsg< T > :: NamedIntMsg( const char* name )
: NamedObject( name ), _msg( ), _pTimeout( 0 )
{
    // register message with message system
    _msg.init( Callback< NamedIntMsg< T > >( this, &NamedIntMsg::notify ), MessageBase::SNDRCV_GLOBAL );

    LOG_DEBUG("NamedIntMsg: %s constructed", name );
}


template < class T >
NamedIntMsg< T > :: ~NamedIntMsg()
{
    delete _pTimeout;
    _pTimeout = 0;

    LOG_DEBUG( "~NamedIntMsg: %s destroyed", _name);
}


template < class T >
void NamedIntMsg< T > :: send( const void* pData, int size )
{
    // PURPOSE: sends specified data, verifies correct message size

    ASSERT( size == sizeof(int) );

    memcpy( &_tbuf, pData, size );
    _msg.send( _tbuf );
}


template < class T >
bool NamedIntMsg< T > :: getData( const SpoofableData* pDataInfo, void* buf, int timeoutMsec )
{
    // if data received within timeout returns data to caller and returns true
    // otherwise returns false

    // store caller buffer and data info for use by notify
    _pDataBuffer = buf;
    _pDataInfo = pDataInfo;
    _matchValue = -1; // not used - the normal condition

    // setup timeout timer
    _pTimeout = new TimerMessage( timeoutMsec, Callback< NamedIntMsg< T > >( this, &NamedIntMsg::timeout ),
                                 TimerMessage::ARMED );

    // enter message loop
    SpooferAPI::messageSystem().allowLoop();
    SpooferAPI::messageSystem().dispatchMessages(); // block in dispatcher

    return _getDataRval;
}


template < class T >
void NamedIntMsg< T > :: timeout()
{
    // PURPOSE: timeout for getData()
    // deletes timer and exits message loop

    delete _pTimeout;
    _pTimeout = 0;
    _getDataRval = false; // getData() returns false if timeout
    SpooferAPI::messageSystem().stopLoop(); // control back to getData()
}


template < class T >
void NamedIntMsg< T > :: notify()
{
    // PURPOSE: handles receipt of message
    // copies received data into getData() caller's buffer

    bool done = false;
    LOG_DEBUG("NamedIntMsg: %s notified", _name );
 
    // retrieve the data
    memcpy( _pDataBuffer, &_msg.getData() + _pDataInfo->offset(), 
            _pDataInfo->size() );

    if ( !(_matchValue == -1) ) // if have a value need to wait for 
    {
        int *sawValuePtr = (int *) _pDataBuffer;
        int sawValue = *sawValuePtr;

        if ( !(_matchValue == sawValue) )
        {
            LOG_DEBUG( "notify: received %d in callback but value doesn't match %d", sawValue, _matchValue );
        }
        else
        {
            done = true;
            LOG_DEBUG( "notify: received value %d it was looking for", _matchValue );
        }
    }
    else
    {
        done = true; // we received a message and copied data, didn't need a special message content
    }

    if (done)
    {
        _getDataRval = true; // getData() returns true if data copied OK
        delete _pTimeout;
        _pTimeout = 0;

        SpooferAPI::messageSystem().stopLoop(); // control back to getData()
    }
}

// added these routine for Next Generation spoofer state awareness
template < class T >
bool NamedIntMsg< T > :: getExactData( const SpoofableData* pDataInfo, void* buf, int timeoutMsec, int matchValue )
{
    // if data received within timeout returns data to caller and returns true
    // otherwise returns false
    LOG_DEBUG( "NamedIntMsg: getExactData %d", matchValue );

    // store caller buffer and data info for use by notify
    _pDataBuffer = buf;
    _pDataInfo = pDataInfo;
    _matchValue = matchValue;

    // setup timeout timer
    _pTimeout = new TimerMessage( timeoutMsec, Callback< NamedIntMsg< T > >( this, &NamedIntMsg::timeout ),
                                 TimerMessage::ARMED );

    // enter message loop
    SpooferAPI::messageSystem().allowLoop();
    SpooferAPI::messageSystem().dispatchMessages(); // block in dispatcher

    return _getDataRval;
}

/*
template < class T >
void NamedIntMsg< T > :: notifyExact()
{
    // PURPOSE: handles receipt of message, for the case that we need to check/match
    // the data value & copies received data into getData() caller's buffer

    LOG_DEBUG("NamedIntMsg: %s notified for matching data %d", _name, _matchValue );
    delete _pTimeout;
    _pTimeout = 0;

    // retrieve the data
    memcpy( _pDataBuffer, &_msg.getData() + _pDataInfo->offset(), 
            _pDataInfo->size() );

    _getDataRval = true; // getData() returns true if data copied OK
    SpooferAPI::messageSystem().stopLoop(); // control back to getData()
}
*/


#endif // NamedIntMsg_HPP
