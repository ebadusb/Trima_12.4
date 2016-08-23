// FILENAME:  ss_test.cpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/unit_test/ss_test/rcs/ss_test.cpp 6.4 2002/11/19 20:45:19Z td07711 Exp td07711 $
// PURPOSE: test SpooferSlave
//   USAGE:
//     -debug - enable debug trace messages
//     -timeout <n> - timeout in n seconds
//
//   Messages supported - spoofable msgs defined in SpoofedMsgBase::create()
//                        sendable msgs defined in NamedMsgBase::create()
//   Fields supported - classes defined in fieldinfo.hpp
//                      instantiated in FieldInfo::init()
//   Enums supported - defined in FieldInfo::init()
//
// CHANGELOG:
//   $Log: ss_test.cpp $
//   Revision 6.4  2002/11/19 20:45:19Z  td07711
//   extern __cplusplus
//   Revision 6.3  2002/11/18 18:07:17  td07711
//   replace SpooferSlave with SpoofServer
//   Revision 6.2  2002/10/25 20:43:29  td07711
//   timeout mods
//   Revision 6.1  2002/10/04 21:12:43  td07711
//   moved #includes into spooferapi.hpp
//   Revision 1.1  2002/10/01 22:42:09  td07711
//   Initial revision


// vxworks includes
#include <vxWorks.h>
#include <usrLib.h>

// standard includes
#include <stdio.h>

// common includes
#include "optionparser.h"

// libtools includes
#include "spooferlog.hpp"
 
// spoofer includes
#include "spoofserver.hpp"


#ifdef __cplusplus
extern "C" int ss_test( const char* args );
#endif

int ss_test( const char* args )
{
    Logger::Init("ss_test");

    // parse command line
    OptionParser commandline("ss_test", "testing a SpooferSlave task");
    commandline.init(args);
    bool debugFlag;
    int timeout;
    commandline.parse("-debug", "enable debug messages", &debugFlag);
    commandline.parse("-timeout", "timeout in seconds to cleanup and exit", &timeout, 10, 1, 10000);
    commandline.done();

    if( commandline.anyErrors() )
    {
	return 1;
    }

    // enable debug msgs
    if(debugFlag)
    {
		log_level_spoofer_debug.logOutput(DataLog_LogEnabled);
    }
    else
    {
		log_level_spoofer_debug.logOutput(DataLog_LogDisabled);
    }


    SpoofServer spoofer;
    SpooferAPI* p = &spoofer;

    LOG_INFO("entering message loop");
    spoofer.setTimeout( timeout );
    spoofer.enterMessageLoop();
    spoofer.spoofEnd();
    
    LOG_INFO("normal exit");
    return 0;
}
