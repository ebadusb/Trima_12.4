// FILENAME: spoofedmsg.hpp
// $Header: H:/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/spoofedmsg.hpp 6.23 2007/02/23 22:48:04Z kgreusb Exp kgreusb $
// PURPOSE: manages spoofing for a given Message type
// CHANGELOG:
//   $Log: spoofedmsg.hpp $
//   Revision 6.23  2007/02/23 22:48:04Z  kgreusb
//   Adding Spoof_Safety_APS for IT 7828
//   Revision 6.22  2003/05/22 22:42:24Z  td07711
//   add linefeed at end of msg notify when -trace is used
//   Revision 6.21  2003/04/07 21:11:32Z  td07711
//   logs receipt of spoofed msgs if run -trace
//   Revision 6.20  2003/03/28 02:35:42Z  td07711
//   log msgMultiplier if run -trace
//   Revision 6.19  2003/03/18 02:07:30Z  td07711
//   support msgMultiplier to block or multiply msg traffic
//   Revision 6.18  2003/01/21 22:09:00Z  td07711
//   bugfix - spoofed msgs must be registered SNDRCV_GLOBAL to get to safety.
//   Revision 6.17  2002/10/18 23:19:11Z  td07711
//   replace spoofedElement spoofing with SpoofedObject::spoofTheData()
//   Revision 6.16  2002/10/11 23:27:59  td07711
//   remove getbuf()
//   Revision 6.15  2002/10/10 21:25:53  td07711
//   debug msg
//   Revision 6.14  2002/10/09 21:03:12  td07711
//   debug msgs
//   Revision 6.13  2002/10/08 00:13:27  td07711
//   replace SpoofedMsgField with SpoofedElement
//   Revision 6.12  2002/10/04 21:14:27  td07711
//   removed receive(), replace string use with const char*
//   Revision 6.11  2002/09/25 22:36:30  td07711
//   some cleanup if max limit exceeded
//   Revision 6.10  2002/09/24 23:51:48  td07711
//   support limit on spoofs executed
//   Revision 6.9  2002/09/13 23:47:36  td07711
//   Revision 6.8  2002/09/06 22:49:54  td07711
//   Revision 6.7  2002/09/05 23:44:25  td07711
//   Revision 6.6  2002/09/04 22:36:36  td07711
//   Revision 6.5  2002/09/03 23:02:36  td07711
//   Revision 6.4  2002/08/31 00:36:15  td07711
//   Revision 6.3  2002/08/29 23:34:31  td07711
//   Revision 6.2  2002/08/27 23:00:26  td07711
//   Revision 6.1  2002/08/26 23:19:37  td07711
//   Revision 1.1  2002/08/23 23:08:26  td07711
//   Initial revision


#ifndef SpoofedMsg_HPP // prevents multiple inclusion
#define SpoofedMsg_HPP

#include "spooferlog.hpp"
#include "datalog.h"

#include "message.h"
#include "spoofermessagesystem.h"

#include "spoofedmsgbase.hpp"
#include "spoofedelement.hpp"
#include "spoofabledata.hpp"

typedef CHwStates HwStatesFromControl;


// CLASSNAME: SpoofedMsg
// RESPONSIBILITIES:
//   1. provides transmit/receive buffers for desired Message type
//   2. registers the message type and callbacks with the Message System
template < class T >
class SpoofedMsg : public SpoofedMsgBase
{
    public:

        SpoofedMsg(const char* name);
        virtual ~SpoofedMsg();


        //
        // base class function overides
        //
        void spoof(); // blocking callback for spoofing an unspoofed msg
        void notify(); // blocking callback for receiving spoofed msg

        void receive(); // nonblocking msg receive
        void send(); // sends a spoofed msg, with fields set as specified

        char* spoofedBuf() { return (char*)&_stbuf; };
        char* originalBuf() { return (char*)&_urbuf; };

    
    protected:


    private:

        void dospoof();

        Message< T > _msg; // provides templated message interface
        T _rbuf; // receive buffer
        T _stbuf; // spoofed transmit buffer
        T _urbuf; // unspoofed receive buffer
     
        SpoofedMsg(const SpoofedMsg&); // catch unauthorized use 
        SpoofedMsg& operator=(const SpoofedMsg& rhs); // catch unauthorized use
};



template < class T >
SpoofedMsg< T > :: SpoofedMsg( const char* name) 
: SpoofedMsgBase( name ), _msg()
{
    // PURPOSE:  construct spoofable msg by name

    ASSERT( name );

    memset( &_urbuf, 0, sizeof(_urbuf) ); // clear it, might be used before receiving

    LOG_DEBUG( "SpoofedMsg: installing notify to receive spoofed msgs" );
    _msg.init( Callback< SpoofedMsg< T > >( this, &SpoofedMsg< T >::notify ),
               MessageBase::SNDRCV_GLOBAL );

    LOG_DEBUG( "SpoofedMsg: installing spoofing callback to receive unspoofed msgs" );
    ((SpooferMessageSystem*)MessageSystem::MsgSystem())->spoofMessage( _msg, 
            Callback< SpoofedMsg< T > >( this, &SpoofedMsg< T >::spoof ) );

    LOG_DEBUG("SpoofedMsg: %s constructed", name );
}


// added to support IT 7828 spoofing just safety with CHwState
SpoofedMsg< HwStatesFromControl > :: SpoofedMsg( const char* name) 
: SpoofedMsgBase( name ), _msg()
{
    // PURPOSE:  construct spoofable msg by name

    
    LOG_DEBUG( "Specialized spoof case for HwStatesFromControl name %s", name );
    ASSERT( name );

    memset( &_urbuf, 0, sizeof(_urbuf) ); // clear it, might be used before receiving

    LOG_DEBUG( "SpoofedMsg: installing notify to receive spoofed msgs" );
    _msg.init( Callback< SpoofedMsg< CHwStates > >( this, &SpoofedMsg< HwStatesFromControl >::notify ),
               MessageBase::SNDRCV_GLOBAL );

    LOG_DEBUG( "SpoofedMsg: installing spoofing callback to receive unspoofed msgs" );
   ((SpooferMessageSystem*)MessageSystem::MsgSystem())->spoofMessage( _msg, 
           Callback< SpoofedMsg< CHwStates > >( this, &SpoofedMsg< HwStatesFromControl >::spoof ) );


    LOG_DEBUG("SpoofedMsg: %s constructed just for Safety", name );
    
}


template < class T >
SpoofedMsg< T > :: ~SpoofedMsg()
{
    // PURPOSE: destroy spoofable message instance

    LOG_DEBUG( "~SpoofedMsg: %s destroyed", _name );
}


template < class T >
void SpoofedMsg< T > :: spoof()
{
    // PURPOSE: callback for msgs to spoof

    LOG_DEBUG( "spoof: spoofing %s", _name );

    // copy received data into unspoofed receive buffer
    memcpy( &_urbuf, &_msg.getData(), sizeof(_urbuf) );

    dospoof();
}


template < class T >
void SpoofedMsg < T > :: dospoof()
{
    // PURPOSE:  copy unspoofed receive buffer to spoofed transmit buffer
    // and spoof each of data fields

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
void SpoofedMsg< T > :: notify()
{
    // PURPOSE: callback for msgs that have already been spoofed

    // move to receive buffer
    memcpy( &_rbuf, &_msg.getData(), sizeof(_rbuf) );

    if( isShowSpoofs() )
    {
	DataLog_Level log( "Spoofer" );

        // show values received
        SpoofedElement* pElement;
        void* pData;
        for( spoofedElementsIterator_t i = _spoofedElements.begin();
            i != _spoofedElements.end(); i++ )
        {
            pElement = (*i).second; 

            //
            // NOTE: char* cast required to get correct address of _rbuf
            //
            pData = (char*)&_rbuf + pElement->dataInfo().offset();
	    log( __FILE__, __LINE__ ) << "notify: " << pElement->dataInfo().objectName()
	        << "." << pElement->dataInfo().name() << " = " 
		<< pElement->dataInfo().valueToString( pData ) << endmsg;
        }
    }
}


template < class T >
void SpoofedMsg< T > :: send()
{
    // PURPOSE: sends a spoofed msg
    // uses last received msg, or zeroed out msg buf if nothing yet received

    LOG_DEBUG( "send: %s", _name );

    dospoof();
}



#endif // SpoofedMsg_HPP
