// FILENAME : spoofedlatentmsg.hpp
// PURPOSE  : manages spoofing of a latent message
//

#ifndef SpoofedLatentMsg_HPP // prevents multiple inclusion
#define SpoofedLatentMsg_HPP


// trima includes
#include "message.h"
#include "spoofermessagesystem.h"


// spoofer includes
#include "spoofedmsgbase.hpp"
#include "spoofedelement.hpp"
#include "spooferlog.hpp"
#include "spooferdispatcher.h"

//
// CLASSNAME: SpoofedLatentMsg<>
//
// RESPONSIBILITIES:
//   1. provides a way to inject a delivery time latency on message type T
//   2. registers the message type and spoofing callbacks with the Message System
//
template < class T >
class SpoofedLatentMsg : public SpoofedMsgBase
{
    public:

        SpoofedLatentMsg( const char* name );
        virtual ~SpoofedLatentMsg();

        //
        // base class function overrides
        //
        void spoof();  // blocking callback for spoofing an unspoofed msg
        void notify(); // blocking callback for receiving spoofed msg
        void send();   // sends a spoofed msg with a given latency

        char* originalBuf() { return (char*)&_msecsDelayOrig; };
        char* spoofedBuf()  { return (char*)&_msecsDelaySpoof; };

    protected:

    private:

        void dospoof();

        T _msg; // provides templated message interface
        int _msecsDelayOrig;
        int _msecsDelaySpoof;
     
        SpoofedLatentMsg(const SpoofedLatentMsg&); // catch unauthorized use
        SpoofedLatentMsg& operator=(const SpoofedLatentMsg& rhs); // catch unauthorized use
};

template < class T >
SpoofedLatentMsg< T > :: SpoofedLatentMsg( const char* name)
   : SpoofedMsgBase( name ),
     _msg(), _msecsDelayOrig( 0 ), _msecsDelaySpoof( 0 )
{
    // PURPOSE:  construct this object by name

    // install callback to receive spoofed msgs
    _msg.init( Callback< SpoofedLatentMsg< T > >( this, &SpoofedLatentMsg< T >::notify ),
               MessageBase::SNDRCV_LOCAL );

    // install callback to spoof unspoofed msgs
    ((SpooferMessageSystem*)MessageSystem::MsgSystem())->spoofMessage( _msg, 
            Callback< SpoofedLatentMsg< T > >( this, &SpoofedLatentMsg< T >::spoof ) );

    LOG_DEBUG( "SpoofedLatentMsg: %s constructed", name );
}


template < class T >
SpoofedLatentMsg< T > :: ~SpoofedLatentMsg( )
{
    // PURPOSE: destroy spoofable message instance
    LOG_DEBUG( "~SpoofedLatentMsg: %s destroyed", _name );
}


template < class T >
void SpoofedLatentMsg< T > :: spoof( )
{
    // PURPOSE: callback for msgs to spoof
    LOG_DEBUG( "spoof: spoofing %s", _name );
    dospoof();
}


template < class T >
void SpoofedLatentMsg < T > :: dospoof( )
{
    // PURPOSE:  inject a latency on the message

    if ( _spoofingEnabled )
    {
       // Here, the value of msecsDelaySpoof gets set
       spoofTheData();

       // Do a sanity check
       if ( _msecsDelaySpoof < 0 )
       {
          LOG_ERROR( "Invalid latency msecsDelaySpoof=%d orig=%d",
                     _msecsDelaySpoof, _msecsDelayOrig );
       }
    }

    if( _spoofingEnabled && _msecsDelaySpoof > 0 )
    {
       _numSpoofedMsgs++;
       LOG_DEBUG( "dospoof: sending spoof #%d %d times with latency=%d msecs",
                  _numSpoofedMsgs, _msgMultiplier, _msecsDelaySpoof );

       //
       // multiply msg traffic as specified
       //
       if( isShowSpoofs() )
       {
          DataLog_Level log( "Spoofer" );
          log( __FILE__, __LINE__ ) << "sending msg " << name()
		      << " " << _msgMultiplier << " times"
		      << " with latency=" << _msecsDelaySpoof << " msecs"
		      << endmsg;
       }
       for( int i = 1; i <= _msgMultiplier; i++ )
       {
          SpooferDispatcher::sendLatentMessage( _msg,
                                                (unsigned long)_msecsDelaySpoof );
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
       _msg.send();
    }
}


template < class T >
void SpoofedLatentMsg< T > :: notify( )
{
    // PURPOSE: callback for msgs that have already been spoofed

    LOG_DEBUG( "notify: %s", _name );

    if( isShowSpoofs() && _spoofingEnabled )
    {
       // Log the latency
       DataLog_Level log( "Spoofer" );
       log( __FILE__, __LINE__ ) << name()
          << ": got msgid=0x" << hex << _msg.msgId() << dec
          << " with latency=" << _msg.latency()
          << " expected delay=" << _msecsDelaySpoof << " msecs"
          << endmsg;
    }
}


template < class T >
void SpoofedLatentMsg< T > :: send( )
{
    // PURPOSE: sends a spoofed msg
    // uses last received msg, or zeroed out msg buf if nothing yet received

    LOG_DEBUG("send: %s", _name );
    dospoof();
}

#endif // SpoofedLatentMsg_HPP
