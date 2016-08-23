// FILENAME: namedmsg.hpp
// $Header: H:/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/namedmsg.hpp 6.6 2008/01/23 17:22:48Z dslausb Exp $
// PURPOSE: provides templated msg by name
// CHANGELOG:
//   $Log: namedmsg.hpp $
//   Revision 6.6  2008/01/23 17:22:48Z  dslausb
//   IT 8435 - VxWorks Checkin
//   Revision 6.5  2002/11/21 16:43:51Z  td07711
//   #include restructuring
//   Revision 6.4  2002/11/20 22:50:16  td07711
//   replace NamedMsgBase with NamedObject
//   Revision 6.3  2002/11/20 21:07:46  td07711
//   delete _pTimeout in destructor
//   Revision 6.2  2002/11/13 23:41:26  td07711
//   add getData()
//   Revision 6.1  2002/10/08 23:46:55  td07711
//   replace strings with const char*
//   Revision 1.1  2002/09/25 15:49:18  td07711
//   Initial revision


#ifndef NamedMsg_HPP // prevents multiple inclusion
#define NamedMsg_HPP


// trima includes
#include "message.h"

// trima includes
#include "timermessage.h"

// spoofer includes
#include "namedobject.hpp"
#include "spoofermessagesystem.h"
#include "spooferapi.hpp"
#include "spooferlog.hpp"


// CLASSNAME: NamedMsg
// RESPONSIBILITIES:
//   1. implements NamedObject::send()
//   2. registers the message type with the Message System
template < class T >
class NamedMsg : public NamedObject
{
    public:

        NamedMsg( const char* name );
        virtual ~NamedMsg( );


        //
        // base class function overides
        //
        void send( const void* pData, int size ); // sends data in message

        bool getData( const SpoofableData* pSpoofableData, void* buf, int timeoutMsec );

        bool getExactData( const SpoofableData* pSpoofableData, void* buf, int timeoutMsec, int matchValue );

    protected:


    private:

        void notify(); // msg system callback for getData()
        void timeout(); // handles timeout for getData()
        TimerMessage* _pTimeout;

        Message< T > _msg; // provides templated message interface
        T _tbuf; // transmit buffer

        NamedMsg(const NamedMsg&); // catch unauthorized use 
        NamedMsg& operator=(const NamedMsg& rhs); // catch unauthorized use
};


template < class T >
NamedMsg< T >::NamedMsg( const char* name )
: NamedObject( name ), _msg(), _pTimeout( 0 )
{
    // register message with message system
    _msg.init( Callback< NamedMsg< T > >( this, &NamedMsg::notify ), MessageBase::SNDRCV_GLOBAL );

    LOG_DEBUG( "NamedMsg: %s constructed", name );
}


template < class T >
NamedMsg< T >::~NamedMsg()
{
    delete _pTimeout;
    _pTimeout = 0;

    LOG_DEBUG("~NamedMsg: destroyed");
}


template < class T >
void NamedMsg< T >::send(const void* pData, int size)
{
    // PURPOSE: sends specified data, verifies correct message size

    LOG_DEBUG("send: %s size = %d", _name, size );

    int nbytes = size;
    if( size != sizeof(_tbuf) )
    {
        LOG_WARNING("send: %s size = %d, expected %d", 
                    _name, size, sizeof( _tbuf ) );
    }

    if( size > sizeof(_tbuf) )
    {
        nbytes = sizeof(_tbuf);
        LOG_WARNING("send: %s data size too big, sending truncated", _name );
    }
    else if( size < sizeof(_tbuf) )
    {
        memset( &_tbuf, 0, sizeof(_tbuf) ); // zero fill buffer
        LOG_WARNING( "send: %s data size too small, sending with zero fill", _name);
    }

    memcpy( &_tbuf, pData, nbytes );
    _msg.send( _tbuf );
}


template < class T >
bool NamedMsg< T > :: getData( const SpoofableData* pDataInfo, void* buf, int timeoutMsec )
{
    // if data received within timeout returns data to caller and returns true
    // otherwise returns false

    // store caller buffer and data info for use by notify
    _pDataBuffer = buf;
    _pDataInfo = pDataInfo;

    // setup timeout timer
    _pTimeout = new TimerMessage( timeoutMsec, Callback< NamedMsg< T > >( this, &NamedMsg::timeout ),
                                 TimerMessage::ARMED );

    // enter message loop
    SpooferAPI::messageSystem().allowLoop();
    SpooferAPI::messageSystem().dispatchMessages(); // block in dispatcher

    return _getDataRval;
}


template < class T >
void NamedMsg< T > :: timeout()
{
    // PURPOSE: timeout for getData()
    // deletes timer and exits message loop

    delete _pTimeout;
    _pTimeout = 0;
    _getDataRval = false; // getData() returns false if timeout
    SpooferAPI::messageSystem().stopLoop(); // control back to getData()
}


template < class T >
void NamedMsg< T > :: notify()
{
    // PURPOSE: handles receipt of message
    // copies received data into getData() caller's buffer

    delete _pTimeout;
    _pTimeout = 0;

    // retrieve the data
    memcpy( _pDataBuffer, &_msg.getData() + _pDataInfo->offset(), 
            _pDataInfo->size() );

    _getDataRval = true; // getData() returns true if data copied OK
    SpooferAPI::messageSystem().stopLoop(); // control back to getData()
}

// added for Next Generation Spoofer
template < class T >
bool NamedMsg< T > :: getExactData( const SpoofableData* pDataInfo, void* buf, int timeoutMsec, int matchValue )
{
    // TODO: use the match value - right now just a clone of  getData member
    // if data received within timeout returns data to caller and returns true
    // otherwise returns false

    // store caller buffer and data info for use by notify
    _pDataBuffer = buf;
    _pDataInfo = pDataInfo;

    // setup timeout timer
    _pTimeout = new TimerMessage( timeoutMsec, Callback< NamedMsg< T > >( this, &NamedMsg::timeout ),
                                 TimerMessage::ARMED );

    // enter message loop
    SpooferAPI::messageSystem().allowLoop();
    SpooferAPI::messageSystem().dispatchMessages(); // block in dispatcher

    return _getDataRval;
}


#endif // NamedMsg_HPP
