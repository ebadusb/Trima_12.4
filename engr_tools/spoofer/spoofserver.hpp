// FILENAME: spoofserver.hpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/spoofserver.hpp 1.8 2003/11/11 18:49:29Z td07711 Exp td07711 $
// PURPOSE: serves up the requests spoofs
// This file does the following:
// 1. acks received spoofer control messages
// 2. implements SpooferAPI functions
//
// CHANGELOG:
//   $Log: spoofserver.hpp $
//   Revision 1.8  2003/11/11 18:49:29Z  td07711
//   IT6091 - support bssOverflow
//   Revision 1.7  2003/10/24 17:56:44Z  td07711
//   IT6091 - add support for memory protection testing
//   Revision 1.6  2003/09/05 20:58:51Z  td07711
//   IT6394 - support suspending named tasks via spoofer
//   Revision 1.5  2003/07/22 22:28:40Z  td07711
//   IT5303 - support turning spoof_server debug msgs on and off from a spoof_client.
//   Revision 1.4  2003/06/03 00:46:51Z  td07711
//   IT6108 - defined fatalError()
//   Revision 1.3  2003/05/29 18:42:36Z  td07711
//   support FATAL_ERROR spoofs
//   Revision 1.2  2003/03/14 23:51:24Z  td07711
//   add support for msgMultiplier()
//   Revision 1.1  2002/11/04 17:42:13Z  td07711
//   Initial revision



#ifndef SpoofServer_HPP // prevents multiple inclusion
#define SpoofServer_HPP


// vxworks includes

// common includes
#include "message.h"

// spoofer includes
#include "spoofercontrol.h"
#include "spooferapi.hpp"


// forward declarations
class SpooferMessageSystem;
class TimerMessage;


// CLASSNAME: SpoofServer
// RESPONSIBILITIES:
//   1. implements slave side of spoofer control API
class SpoofServer : public SpooferAPI
{
    public:

        SpoofServer( const char* name ); 
        virtual ~SpoofServer();

        //
        // SpooferControl API functions
        //
        void spoofExit(); // cleanup and exit
        void spoofEnd(); // stops all spoofing
        void spoofActivate(); // activates pending spoofs
        void spoofData( const char* objectName, const char* elementName, const char* valueName );
        void spoofData( const char* objectName, const char* elementName, float mult, float offset );
        void unspoofData( const char* objectName, const char* elementName );
        void setSpoofedMessageLimit( int n ); // end spoofing after n more message spoofs
        void showSpoofs(); // sets flag to show spoofs as they occur
        void spoofEnable();
        void spoofDisable();
	void msgMultiplier( int mult ); // used to stop or flood msg traffic
	void fatalError( const char* serverName ); // calls FATAL_ERROR
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

	const char* name() { return _name; };
	

    protected:

 
    private:

	static void dummy(); // used by textWrite()

        void init( const char* name ); // initializes messaging
        static bool _initFlag; // limit one master per machine
	char* _name;

        // 
        // spoofer control message protocol stuff
        //
        SpooferAck _ack; // ack transmit buffer 

        void controlHandler(); // handles control messages
        void ackTimeoutHandler(); // stub for slave task, currently does not
        // send commands that are acked

        void ackSend(int taskid, int seqnum, SCM_COMMAND command = SCM_ACK);

	SpoofServer(); // catch unauthorized use
        SpoofServer(const SpoofServer&); // catch unauthorized use 
        SpoofServer& operator=(const SpoofServer& rhs); // catch unauthorized use
};


#endif // SpoofServer_HPP
