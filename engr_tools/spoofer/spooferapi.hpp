// FILENAME: spooferapi.hpp
// $Header: H:/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/spooferapi.hpp 6.23 2003/12/18 20:26:58Z td07711 Exp $
// PURPOSE: defines an API for spoofing applications
// CHANGELOG:
//   $Log: spooferapi.hpp $
//   Revision 6.23  2003/12/18 20:26:58Z  td07711
//   IT6140 - add loggingTest support
//   Revision 6.22  2003/11/11 18:49:06Z  td07711
//   IT6091 - support bssOverflow
//   Revision 6.21  2003/10/24 17:56:28Z  td07711
//   IT6091 - add support for memory protection testing
//   Revision 6.20  2003/09/09 16:37:34Z  td07711
//   IT6419 - assign suspend() = 0 to make it pure virtual.
//   Revision 6.19  2003/09/05 20:58:29Z  td07711
//   IT6394 - support suspending named tasks via spoofer
//   Revision 6.18  2003/07/22 22:28:19Z  td07711
//   IT5303 - support turning spoof_server debug msgs on and off from a spoof_client.
//   Revision 6.17  2003/06/03 00:46:08Z  td07711
//   IT6108 - made fatalError() pure virtual
//   Revision 6.16  2003/05/29 18:42:11Z  td07711
//   support FATAL_ERROR spoofs
//   Revision 6.15  2003/03/14 23:47:07Z  td07711
//   add msgMultiplier() function
//   Revision 6.14  2003/01/15 19:46:40Z  td07711
//   bugfix - don't delete timer from timer's timeout function
//   Revision 6.13  2002/11/13 23:45:05Z  td07711
//   add getData() support
//   Revision 6.12  2002/10/31 22:48:35  td07711
//   add spoofEnable() and spoofDisable()
//   change timerExpired() to timedOut()
//   Revision 6.11  2002/10/24 23:04:21  td07711
//   add timeout support
//   Revision 6.10  2002/10/11 23:29:52  td07711
//   remove old comments
//   Revision 6.9  2002/10/10 21:28:30  td07711
//   add showSpoofs() and unspoofData()
//   Revision 6.8  2002/10/08 00:15:17  td07711
//   removed spoofSend()
//   changed setSpoofLimit() to setSpoofedMessageLimit()
//   Revision 6.7  2002/10/04 21:13:11  td07711
//   moved some #includes here
//   Revision 6.6  2002/10/01 22:35:15  td07711
//   add comment
//   Revision 6.5  2002/09/27 19:39:44  td07711
//   renamed intSend() to messageSend()
//   Revision 6.4  2002/09/26 21:52:25  td07711
//   setSpoofLimit() = 0 to require definition in derived classes
//   added intSend() for sending int messages
//   Revision 6.3  2002/09/24 23:53:50  td07711
//   add setSpoofLimit()
//   add messageSend()
//   Revision 6.2  2002/09/18 23:29:43  td07711
//   Revision 6.1  2002/09/17 23:50:58  td07711
//   Revision 1.1  2002/09/17 00:13:20  td07711
//   Initial revision


#ifndef SpooferAPI_HPP // prevents multiple inclusion
#define SpooferAPI_HPP


#include "spoofercontrol.h"
#include "message.h"
#include "spooferlog.hpp"

#include "spoofabledata.hpp"
#include "enuminfo.hpp"

#include "procstate.h"              // Used to access the ProcState class


// forward declarations
class MessageSystem;
class TimerMessage;


// CLASSNAME: SpooferAPI
// RESPONSIBILITIES:
//   1. defines spoof control and setup functions
class SpooferAPI
{
    //
    // static stuff
    //

    public:
        
        static MessageSystem& messageSystem() { return *_pMessageSystem; };


    protected:

        static MessageSystem* _pMessageSystem;


    //
    // non static stuff
    //

    public:

        SpooferAPI();
        virtual ~SpooferAPI(); 

        //
        // functions used by a spoofing task to setup and control a spoof
        //
        void setTimeout( int seconds ); // ends spoofing upon timeout
        void waitForTimeout(); // blocks until timeout ends the spoof
        bool timedOut() { return _timedOut; }; // returns true if timeout timer expired
        void sleepMsec( int msec ); // sleeps for specified msec

        virtual void spoofActivate() = 0; // activates pending spoofs
        virtual void spoofEnable() = 0; // enables spoofing
        virtual void spoofDisable() = 0; // disables spoofing, but no cleanup
        virtual void spoofEnd() = 0; // ends all spoofing
        virtual void spoofExit() = 0; // spoofing tasks cleanup and exit

        virtual void spoofData( const char* objectName, const char* dataName, const char* valueName ) = 0;
        virtual void spoofData( const char* objectName, const char* dataName, float mult, float offset ) = 0;
        virtual void unspoofData( const char* objectName, const char* dataName ) = 0;

        virtual void setSpoofedMessageLimit( int n ) = 0; // limits spoofing to n more spoofs
        virtual void showSpoofs() = 0; // whenever an element is spoofed, show it
        virtual void msgMultiplier( int mult ) = 0; // set multiplier on message spoofs 
        virtual void enableDebug() = 0; // enable debug msgs
        virtual void disableDebug() = 0; // disable debug messages
        virtual void suspend( const char* serverName, const char* taskName ) = 0;
        virtual void loggingTest( int burst, int duration ) = 0;

        virtual void cleanup(); // frees up heap and msg resources

        // blocks on message queue, exits loop in timeout seconds, no change if 0, forever if <0
        void enterMessageLoop( int timeout = 0 ); // default no change to timeout

        void messageSend( const char* messageName, const void* pData, int size ); // sends a msg
        void messageSend( const char* messageName, int data ); // sends an int msg 
        bool getData( const char* objectName, const char* dataName,  // retrieve specified data
                      void* buf, int bufsize, int timeoutMsec ); // returns false if timeout or error

        virtual void fatalError( const char* serverName ) = 0; // specified server calls fatal error
        virtual void stackOverflow( const char* serverName ) = 0;
        virtual void dataOverflow( const char* serverName ) = 0;
        virtual void bssOverflow( const char* serverName ) = 0;
        virtual void textWrite( const char* serverName ) = 0;
        virtual void nullptrRead( const char* serverName ) = 0;
	
        // waits for state/substate transition message up to maxWait in milliseconds returns false if error or times out
        bool waitForSubstate(const char* waitForSS, void * buf, int bufsize, int maxWait);

    protected:

        Message< SpooferControl >* _pControl;
		Message< ProcState > _SpooferAPIProcStateMsg;
		Message< SpooferAck >* _pAck;
        TimerMessage* _pAckTimer;
        virtual void ackTimeoutHandler() = 0;
	bool _ackTimerCleanupPending;

        int _taskid;

        void exitMessageLoop();  // forces enterMessageLoop() to return
        virtual void init(); // object init

    
    private:

        TimerMessage* _pTimeout;
        void timeoutHandler();
        bool _timeoutCleanupPending;
        bool _timedOut;
        TimerMessage* _pSleepTimer;
        void sleepTimeoutHandler();
        bool _sleepTimerCleanupPending;
        void handleProcStateMessage();       

        SpooferAPI(const SpooferAPI&); // catch unauthorized use 
        SpooferAPI& operator=(const SpooferAPI& rhs); // catch unauthorized use
};


#endif // SpooferAPI_HPP
