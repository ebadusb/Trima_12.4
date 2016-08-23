// FILENAME: spoofeddonorvitalsmsg.hpp
// $Header: $
// PURPOSE: manages spoofing for a given DONOR_MESSAGE type
// CHANGELOG:
//   $Log: $



#ifndef SpoofedDonorVitalsMsg_HPP // prevents multiple inclusion
#define SpoofedDonorVitalsMsg_HPP


// trima includes
#include "message.h"
#include "spoofermessagesystem.h"


// spoofer includes
#include "spoofedmsgbase.hpp"
#include "spoofedelement.hpp"
#include "spooferlog.hpp"


// CLASSNAME: SpoofedDonorVitalsMsg
// RESPONSIBILITIES:
//   1. provides transmit/receive buffers for desired Message type
//   2. registers the message type and callbacks with the Message System
template < class T >
class SpoofedDonorVitalsMsg : public SpoofedMsgBase
{
    public:

        SpoofedDonorVitalsMsg( const char* name );
        virtual ~SpoofedDonorVitalsMsg();


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
        SDonorVitals _rbuf; // receive buffer
        SDonorVitals _stbuf; // spoofed transmit buffer
        SDonorVitals _urbuf; // unspoofed receive buffer
     
        SpoofedDonorVitalsMsg(const SpoofedDonorVitalsMsg&); // catch unauthorized use 
        SpoofedDonorVitalsMsg& operator=(const SpoofedDonorVitalsMsg& rhs); // catch unauthorized use
};



template < class T >
SpoofedDonorVitalsMsg< T > :: SpoofedDonorVitalsMsg( const char* name)
: SpoofedMsgBase( name ), _msg()
{
    // PURPOSE:  construct spoofable donor vitals msg by name

    // clear unspoofed receive buffer since it can be used before receiving
    memset( &_urbuf, 0, sizeof( _urbuf ) );

    // install callback to receive spoofed msgs
    _msg.init( Callback< SpoofedDonorVitalsMsg< T > >( this, &SpoofedDonorVitalsMsg< T >::notify ),
               MessageBase::SNDRCV_LOCAL );

    // install callback to spoof unspoofed msgs
    ((SpooferMessageSystem*)MessageSystem::MsgSystem())->spoofMessage( _msg, 
            Callback< SpoofedDonorVitalsMsg< T > >( this, &SpoofedDonorVitalsMsg< T >::spoof ) );

    LOG_DEBUG("SpoofedDonorVitalsMsg: %s constructed", name );
}


template < class T >
SpoofedDonorVitalsMsg< T > :: ~SpoofedDonorVitalsMsg( )
{
    // PURPOSE: destroy spoofable message instance

    LOG_DEBUG("~SpoofedDonorVitalsMsg: %s destroyed", _name);
}


template < class T >
void SpoofedDonorVitalsMsg< T > :: spoof( )
{
    // PURPOSE: callback for msgs to spoof

    LOG_DEBUG( "spoof: spoofing %s", _name );

    // copy received data into unspoofed receive buffer
    memcpy( &_urbuf, &_msg.getData(), sizeof( _urbuf ) );

    dospoof();
}


template < class T >
void SpoofedDonorVitalsMsg < T > :: dospoof( )
{
    // PURPOSE:  copy receive data into transmit buffer and
    // spoof each data field

    // FIXME - make this function generic to SpoofedMsgBase
    // or duplicate in SpoofedIntMsg

    // copy unspoofed received buffer into spoofed transmit buffer
    memcpy( &_stbuf, &_urbuf, sizeof( _stbuf ) );


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
void SpoofedDonorVitalsMsg< T > :: notify( )
{
    // PURPOSE: callback for msgs that have already been spoofed

    // FIXME - make this generic to SpoofedMsgBase

    LOG_DEBUG( "notify: %s", _name );

    // move to receive buffer
    memcpy( &_rbuf, &_msg.getData(), sizeof( _rbuf ) );

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
void SpoofedDonorVitalsMsg< T > :: send( )
{
    // PURPOSE: sends a spoofed msg
    // uses last received msg, or zeroed out msg buf if nothing yet received

    LOG_DEBUG("send: %s", _name );

    dospoof();
}



#endif // SpoofedDonorVitalsMsg_HPP
