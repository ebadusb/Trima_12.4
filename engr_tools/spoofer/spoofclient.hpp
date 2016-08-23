// FILENAME: spoofclient.hpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/spoofclient.hpp 6.8 2003/11/11 18:48:59Z td07711 Exp td07711 $
// PURPOSE: defines SpoofClient class - control spoof execution via requests to SpoofServer
// This file does the following:
// 1. implements the SpooferControl protocol - client function calls block until
//    the expected message acknowledgement is received, or a timeout error.
// 2. provides client side API functions to setup and control a spoof
// 3. handles receipt of SpooferControl messages from multiple SpoofServers
//
// CHANGELOG:
//   $Log: spoofclient.hpp $
//   Revision 6.8  2003/11/11 18:48:59Z  td07711
//   IT6091 - support bssOverflow
//   Revision 6.7  2003/10/24 17:57:00Z  td07711
//   IT6091 - add support for memory protection testing
//   Revision 6.6  2003/09/05 20:58:21Z  td07711
//   IT6394 - support suspending named tasks via spoofer
//   Revision 6.5  2003/07/22 22:28:12Z  td07711
//   IT5303 - support turning spoof_server debug msgs on and off from a spoof_client.
//   Revision 6.4  2003/07/17 23:22:54Z  td07711
//   IT2617 - change ack timeout default from 5 to 3 seconds
//   Revision 6.3  2003/05/29 18:41:59Z  td07711
//   support FATAL_ERROR spoofs
//   Revision 6.2  2003/03/14 23:47:47Z  td07711
//   add msgMultiplier() function
//   Revision 6.1  2002/11/13 23:43:57Z  td07711
//   allow ack timeout overide
//   Revision 1.1  2002/11/04 17:42:12  td07711
//   Initial revision


#ifndef SpoofClient_HPP // prevents multiple inclusion
#define SpoofClient_HPP


#include "spoofercontrol.h"
#include "message.h"
#include "spooferapi.hpp"


// forward declarations
class MessageSystem;
class TimerMessage;


// CLASSNAME: SpoofClient
// RESPONSIBILITIES:
//   1. provides functions needed by spoofer master to setup and control a spoof
//   2. handles SpooferControl messages from multiple spoofer slaves
class SpoofClient : public SpooferAPI
{
    public:

        SpoofClient( int numSlaves = 1 ); 
        virtual ~SpoofClient();

        //
        // functions used by a spoofer_main task to setup and control a spoof
        //
        void spoofExit(); // ends spoofing and exits
        void spoofEnd(); // stops all spoofing
        void spoofActivate(); // activates latest spoofs
        void spoofData( const char* objectName, const char* elementName, const char* valueName );
        void spoofData( const char* objectName, const char* elementName, float mult, float offset );
        void unspoofData( const char* objectname, const char* elementName );
        void setSpoofedMessageLimit( int n ); // limits spoofing to n more message spoofs
        void showSpoofs(); // show spoofs as they occur
        void spoofEnable();
        void spoofDisable();
	void msgMultiplier( int mult ); // to stop or flood spoofed message traffic
	void fatalError( const char* serverName );
	void enableDebug();
	void disableDebug();
	void suspend( const char* serverName, const char* taskName );
	void loggingTest( int burst, int duration );

	void stackOverflow( const char* serverName );
	void dataOverflow( const char* serverName );
	void bssOverflow( const char* serverName );
	void textWrite( const char* serverName );
	void nullptrRead( const char* serverName );
        
	void cleanup(); // frees up heap and msg resources

    
    protected:

 
    private:


        static bool _initFlag; // limit one master per machine

        void init(); // initializes messaging

        //
        // spoofer control message protocol stuff
        //
        SpooferControl _scm; // transmit buffer
        int _lastSeqnum; // last sequence number sent
        int _pendingSeqnum;
        int _pendingCount; // set for number of acks needed to proceed
        const int _numSlaves; // number of spoofer slave tasks in system

        void ackHandler(); // handles acks
        void ackTimeoutHandler(); 
        void scmSend( int ackTimeout = 3 ); // 3 second default on getting acks back

        SpoofClient(const SpoofClient&); // catch unauthorized use 
        SpoofClient& operator=(const SpoofClient& rhs); // catch unauthorized use
};


#endif // SpoofClient_HPP
