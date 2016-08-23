// FILENAME:  spoof_server.cpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/spoof_server/rcs/spoof_server.cpp 6.7 2003/04/02 00:22:04Z td07711 Exp td07711 $
// PURPOSE: spoof server task - serves spoofs upon demand
//   USAGE:
//     -debug - enable debug trace messages
//     -timeout <n> - timeout in n seconds
//
//   Spoofable data defined in SpoofableData::init() in spoofabledata.cpp
//   Spoofable objects added in SpoofedObjects::create() in spoofedobjects.cpp
//
// CHANGELOG:
//   $Log: spoof_server.cpp $
//   Revision 6.8  2010/11/01 10:47:13Z  IT9898
//   added Telnet functionality - mcurusb
//   Revision 6.7  2003/04/02 00:22:04Z  td07711
//   default -timeout to 0 = forever
//   Revision 6.6  2003/02/12 00:27:44Z  td07711
//   log to stderr
//   Revision 6.5  2003/01/21 22:11:26Z  td07711
//   add -priority option to set task priority
//   Revision 6.4  2002/12/20 23:32:41Z  td07711
//   return 1 if any cmdline parsing errors
//   Revision 6.3  2002/11/19 21:01:58Z  td07711
//   extern __cplusplus
//   Revision 6.2  2002/11/04 17:40:19  td07711
//   renamed SpooferSlave to SpoofServer
//   Revision 6.1  2002/10/24 19:20:44  td07711
//   modified timeout support
//   Revision 1.1  2002/10/23 22:11:15  td07711
//   Initial revision


// vxworks includes
#include <vxWorks.h>
#include <usrLib.h>
#include <taskLib.h>
#include <routeLib.h>

// standard includes
#include <stdio.h>

// common includes
#include "optionparser.h"

// spoofer includes
#include "spoofserver.hpp"
#include "spooferlog.hpp"

#ifndef SUPPORT_SAFETY_SPOOFS
extern "C" STATUS	shellInit(int, int);
extern "C" STATUS	shellParserControl(...); 
extern "C" STATUS 	telnetdParserSet(FUNCPTR pParserCtrlRtn);
extern "C" STATUS 	telnetdInit(int numClients, BOOL staticFlag);
extern "C" STATUS 	telnetdStart(int port);
#endif

extern int shellTaskPriority;

#ifdef __cplusplus
extern "C" int spoof_server( const char* args );
#endif

int spoof_server( const char* args )
{
	// parse command line
    OptionParser commandline("spoof_server", "serves spoofs upon demand");
    commandline.init(args);
    bool debugFlag;
    int timeout;
    int priority;
    bool flag;
    const char* name;

    commandline.parse("spoof_server", "allowed as cmd line token", &flag);
    commandline.parse("-debug", "enable debug messages", &debugFlag);
    commandline.parse("-timeout", "timeout in seconds to cleanup and exit, 0=forever", &timeout, 0, 0, 10000);
    commandline.parse("-priority", "set task priority", &priority, 30, 0, 255);
    commandline.parse( "-name", "set server name", &name, "TheSpoofServerWithNoName" );
    commandline.done();

    if( commandline.anyErrors() )
    {
	return 1;
    }

    // logging flags
    if( debugFlag )
    {
		log_level_spoofer_debug.logOutput(DataLog_LogEnabled);
    }
    else
    {
		log_level_spoofer_debug.logOutput(DataLog_LogDisabled);
    }

    taskPrioritySet( taskIdSelf(), priority );

#ifdef SUPPORT_SAFETY_SPOOFS
    // add route on safety so it can talk to external ethernet for tornado shell
    routeAdd( "172.21.0.0", "10.0.0.2" );
#else

	if ((taskNameToId ("tShell")) == ERROR)	/* Shell not started yet. */
	{
	    shellTaskPriority = 100;
	    shellInit(60000, TRUE);
	}

	telnetdParserSet(shellParserControl);	
	telnetdInit(1, 1);	
	telnetdStart(23);
#endif

    SpoofServer spoofer( name );

    LOG_INFO("%s entering message loop", spoofer.name() );
    spoofer.setTimeout( timeout );
    spoofer.enterMessageLoop(); 
    spoofer.spoofEnd();
    
    LOG_INFO("%s normal exit", spoofer.name());
    return 0;
}
