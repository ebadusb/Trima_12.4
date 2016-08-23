// FILENAME: spoofedintmsg.hpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/spoofedintmsg.hpp 6.7 2002/10/18 23:18:44Z td07711 Exp td07711 $
// PURPOSE: manages spoofing for a given INT_MESSAGE type
// CHANGELOG:
//   $Log: spoofedintmsg.hpp $
//   Revision 6.7  2002/10/18 23:18:44Z  td07711
//   replace spoofedElement spoofing with SpoofedObject::spoofTheData()
//   Revision 6.6  2002/10/17 23:02:24  td07711
//   use default msg constructor
//   Revision 6.5  2002/10/11 23:27:45  td07711
//   remove getbuf()
//   Revision 6.4  2002/10/10 21:25:40  td07711
//   debug msg
//   Revision 6.3  2002/10/09 21:02:48  td07711
//   debug msgs
//   Revision 6.2  2002/10/08 00:12:40  td07711
//   replace SpoofedMsgField with SpoofedElement
//   Revision 6.1  2002/10/04 21:14:55  td07711
//   replace most string use with const char*
//   Revision 1.1  2002/09/26 22:03:09  td07711
//   Initial revision


#ifndef SpoofedIntMsg_HPP // prevents multiple inclusion
#define SpoofedIntMsg_HPP


// trima includes
#include "message.h"
#include "spoofermessagesystem.h"

// spoofer includes
#include "spoofedmsgbase.hpp"
#include "spoofedelement.hpp"
#include "spooferlog.hpp"


// CLASSNAME: SpoofedMsg
// RESPONSIBILITIES:
//   1. provides transmit/receive buffers for desired Message type
//   2. registers the message type and callbacks with the Message System
template < class T >
class SpoofedIntMsg : public SpoofedMsgBase
{
    public:

        SpoofedIntMsg( const char* name );
        virtual ~SpoofedIntMsg();


        //
        // base class function overides
        //
        void spoof(); // blocking callback for spoofing an unspoofed msg
        void notify(); // blocking callback for receiving spoofed msg

        void send(); // sends a spoofed msg, with fields set as specified

        char* spoofedBuf() { return (char*)&_stbuf; };
        char* originalBuf() { return (char*)&_urbuf; };

    
    protected:


    private:

        void dospoof();

        T _msg; // provides templated message interface
        int _rbuf; // receive buffer
        int _stbuf; // spoofed transmit buffer
        int _urbuf; // unspoofed receive buffer
     
        SpoofedIntMsg(const SpoofedIntMsg&); // catch unauthorized use 
        SpoofedIntMsg& operator=(const SpoofedIntMsg& rhs); // catch unauthorized use
};



template < class T >
SpoofedIntMsg< T > :: SpoofedIntMsg( const char* name)
: SpoofedMsgBase( name ), _msg( )
{
    // PURPOSE:  construct spoofable int msg by name

    // clear unspoofed receive buffer since it can be used before receiving
    _urbuf = 0;

    // install callback to receive spoofed msgs
    _msg.init( Callback< SpoofedIntMsg< T > >( this, &SpoofedIntMsg< T >::notify ),
               MessageBase::SNDRCV_LOCAL );

    // install callback to spoof unspoofed msgs
    ((SpooferMessageSystem*)MessageSystem::MsgSystem())->spoofMessage( _msg, 
            Callback< SpoofedIntMsg< T > >( this, &SpoofedIntMsg< T >::spoof ) );

    LOG_DEBUG("SpoofedIntMsg: %s constructed", name );
}


template < class T >
SpoofedIntMsg< T > :: ~SpoofedIntMsg( )
{
    // PURPOSE: destroy spoofable message instance

    LOG_DEBUG("~SpoofedIntMsg: %s destroyed", _name);
}


template < class T >
void SpoofedIntMsg< T > :: spoof( )
{
    // PURPOSE: callback for msgs to spoof

    LOG_DEBUG( "spoof: spoofing %s", _name );

    // copy received data into unspoofed receive buffer
    _urbuf = _msg.getData();

    dospoof();
}


template < class T >
void SpoofedIntMsg < T > :: dospoof( )
{
    // PURPOSE:  copy receive data into transmit buffer and
    // spoof and send

    //
    // copy unspoofed received buffer into spoofed transmit buffer
    //
    memcpy( &_stbuf, &_urbuf, sizeof(_stbuf) );

    if( _spoofingEnabled )
    {
	_numSpoofedMsgs++;
	LOG_DEBUG( "dospoof: sending spoof #%d %d times", _numSpoofedMsgs, _msgMultiplier );
	spoofTheData(); // spoofs the transmit buffer

	//
	// multiply msg traffic as specified
	//
	if( isShowSpoofs() )
	{
	    DataLog_Level log( "Spoofer" );
	    log( __FILE__, __LINE__ ) << "sending msg " << name() 
		<< " " << _msgMultiplier << " times" << endmsg;
	}
	for( int i = 1; i <= _msgMultiplier; i++ )
	{
	    _msg.send( _stbuf );
	}

	//
	// stop spoofing if max limit has been reached
	//
	if( _maxSpoofedMsgs > 0 && _numSpoofedMsgs >= _maxSpoofedMsgs )
	{
	    LOG_INFO( "dospoof: did %d spoofs, that's it", _numSpoofedMsgs );

	    SpoofedMsgBase::disableSpoofing();
	    SpoofedMsgBase::cleanup(); // cleanup what was being spoofed
	}
    }
    else // forward the message with no changes
    {
	_msg.send( _stbuf );
    }
}


template < class T >
void SpoofedIntMsg< T > :: notify( )
{
    // PURPOSE: callback for msgs that have already been spoofed

    LOG_DEBUG( "notify: %s", _name );

    // move to receive buffer
    _rbuf = _msg.getData();

#if FORREFERENCE
    if( Logger::Is_debug() )
    {

        // show values received
        SpoofedElement* pElement;
        void* pData;
        for( spoofedElementsIterator_t i = _spoofedElements.begin();
            i != _spoofedElements.end(); i++ )
        {
            pElement = (*i).second;
            pData = (char*)&_rbuf + pElement->dataInfo().offset();
            LOG_DEBUG( "notify: %s.%s = %s", pElement->dataInfo().objectName(),
                     pElement->dataInfo().name(), 
                     pElement->dataInfo().valueToString( pData ).c_str() );
        }
    }
#endif
}


template < class T >
void SpoofedIntMsg< T > :: send( )
{
    // PURPOSE: sends a spoofed msg
    // uses last received msg, or zeroed out msg buf if nothing yet received

    LOG_DEBUG("send: %s", _name );

    dospoof();
}



#endif // SpoofedIntMsg_HPP
