// FILENAME:  spoof.cpp
// $Header: H:/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/spoof/rcs/spoof_main.cpp 6.26 2008/08/15 22:17:02Z kgreusb Exp kgreusb $
// PURPOSE:  defines and runs specified spoof for Trima testing
//   BASIC USAGE - the user specifies the following keyword value command line options:
//     spoof [options]
//     -id <spoof_id> - specifies what spoof to run
//     -show_spoofs - shows all spoof ID's defined
//     -show_spoofable_data - shows all data fields that can be spoofed
//     -show_enums - shows enums and values supported
//     -show_spoofable_objects - show all objects that can be spoofed
//     -object <object_name> - specifies a spoofable object
//     -timeout <n> - timeout the spoof after n seconds
//     -debug - enables debug trace messages 
//
//   Messages supported - spoofable msgs defined in SpoofedMsgBase::create()
//                        sendable msgs defined in NamedMsgBase::create()
//   Fields supported - classes defined in fieldinfo.hpp
//                      instantiated in FieldInfo::init()
//   Enums supported - defined in FieldInfo::init()
//
// CHANGELOG:
//   $Log: spoof_main.cpp $
//   Revision 6.26  2008/08/15 22:17:02Z  kgreusb
//   Adding new user parameters and parsing for Next Generation Spoofer project
//   Revision 6.25  2008/06/12 19:11:35Z  kgreusb
//   Allowing for only one parameter to a parameterized spoof instead of the previously required two parameters
//   Revision 6.24  2006/11/09 18:05:09Z  kgreusb
//   Adding parameterized spoofs for ITs such as 7220 and 7334
//   Revision 6.23  2003/12/18 20:28:19Z  td07711
//   IT6140 - add loggingTest support
//   Revision 6.22  2003/11/11 18:39:32Z  td07711
//   IT6099 - increase timeout max to 36000 secs (10 hrs)
//   Revision 6.21  2003/09/15 19:22:59Z  td07711
//   IT6421 - log removeFile to rundata
//   Revision 6.20  2003/09/11 18:02:48Z  td07711
//   IT6421 - add -removeFile cmd line option
//   Revision 6.19  2003/09/05 21:10:21Z  td07711
//   IT6394 - support task suspend from command line
//   Revision 6.18  2003/07/22 22:30:51Z  td07711
//   IT5303 - provide -serverDebug cmd line option to turn on debug msgs on spoof_servers.  Default turns off server debug msgs.
//   Revision 6.17  2003/07/17 23:19:19Z  td07711
//   IT6217 - change default priority to from 35 to 30 to avoid ackTimeout on 1st message sent.
//   Revision 6.16  2003/07/08 18:31:12Z  td07711
//   IT6181 - changing default priority from 10 to 35 (spoof_server and messaging is at 30).
//   Revision 6.15  2003/04/14 23:27:17Z  td07711
//   use spoofer datalog level
//   Revision 6.14  2003/04/07 21:08:07Z  td07711
//   add -traffic option to multiply/block msg traffic
//   Revision 6.13  2003/03/15 00:00:40Z  td07711
//   use datalogger to log end of spoof
//   Revision 6.12  2003/02/14 18:53:12Z  td07711
//   default -servers to 2 (control and safety)
//   Revision 6.11  2003/02/13 00:25:30Z  td07711
//   changed option -slaves to -servers
//   Revision 6.10  2003/02/12 00:28:37Z  td07711
//   log to stderr
//   Revision 6.9  2003/01/25 00:43:22Z  td07711
//   add cmd line options
//   Revision 6.8  2003/01/21 22:10:53Z  td07711
//   add -priority option to set task priority
//   Revision 6.7  2003/01/21 00:22:57Z  td07711
//   add -help option for help on specified spoof
//   Revision 6.6  2003/01/17 01:55:17Z  td07711
//   spoof mods
//   Revision 6.5  2002/12/20 23:31:55Z  td07711
//   return 1 if any cmdline parsing errors
//   Revision 6.4  2002/11/19 20:59:58Z  td07711
//   extern __cplusplus
//   Revision 6.3  2002/11/04 17:40:38  td07711
//   renamed SpooferMaster to SpoofClient
//   Revision 6.2  2002/10/31 22:41:22  td07711
//   support 1-10 spoofer slave tasks
//   support spoofData(), unspoofData(), spoofEnd(), spoofEnable(), and
//   spoofDisable() from command line
//   Revision 6.1  2002/10/24 21:15:29  td07711
//   fix bug to show info on single spoof
//   Revision 1.1  2002/10/23 21:53:04  td07711
//   Initial revision


// vxworks includes
#include <vxWorks.h>
#include <usrLib.h>
#include <taskLib.h>

// standard includes
#include <stdio.h>

// common includes
#include "spoofermessagesystem.h"
#include "optionparser.h"
#include "datalog_levels.h"


// libtools includes
#include "clock.hpp"
 
// spoofer includes
#include "spoofclient.hpp"
#include "myspoofs.hpp"
#include "spooferlog.hpp"

//Auto generated spoofer include
#include "project_build_id.h"

// verification code
#include "verify.hpp"

#ifdef __cplusplus
extern "C" int spoof_main( const char* args );
#endif

int spoof_main( const char* args )
{
	DataLog(log_level_spoofer_info) << "spoof_main(" << args << ")"<< endmsg;

#ifdef LOCALBUILD
    LOG_INFO("NOT using build version check");
#endif
#ifndef LOCALBUILD
	//Before doing anything, confirm that we have the expected build number
   	 char* actual_build = getenv("TRIMA_BUILD");
	 
	 if (!actual_build) {
		 LOG_INFO(
			 "Spoofer build number confirmation failure.  Actual Build number is NULL.  Exiting.");
		 return 0;
	 }

	 if (strchr(actual_build, ' '))
	 {
		 LOG_INFO(
			 "Spoofer build number confirmation failure.  Actual Build number contains spaces.  Exiting.");
		 return 0;
	 }

	 if(strcmp (actual_build, TRIMA_BUILD_ID))
	 {
		 LOG_INFO(
			 "Spoofer build number confirmation failure.  Expected: %s; Got: %s. Exiting.", 
			 TRIMA_BUILD_ID, actual_build);
		 return 0;
	 }
	 else 
	 {
		 DataLog(log_level_spoofer_info) << 
			 "Spoofer build number confirmed."<< endmsg;
	 }
#endif // LOCALBUILD	

    // parse command line
    OptionParser commandline("spoof_main", "runs a Trima spoof");
    commandline.init(args);

    // parsed command line options
    const char* spoofID;
    const char* objectName;
    bool showSpoofableObjects;
    bool showSpoofableData;
    bool debug;
    bool serverDebug;
    bool showEnums;
    const char* intMessageName;
    bool traceSpoofs;
    bool showSpoofs;
    int intData;
    int timeout;
    const char* dataName;
    const char* valueName;
    bool enable, disable, end, unspoof; 
    int slaves;
    bool activate;
    bool help;
    int priority;
    float multiplier;
    float offset;
    int traffic; 
    const char* suspendControlTask;
    const char* suspendSafetyTask;
    const char* removeFile;
    int logBurst;
	const char* Var1;
	const char* Var2;
    const char* beginSubstate;
    const char* beginMsg;
    // bool timeFromActive = FALSE;
    int timeFromActive = 0;
    const char* endSubstate;
    const char* endMsg;
    int delta = 0;
    bool isSS;
    int snapPrefix = 0;

    commandline.parse( "-id", "specifies which spoof to run", &spoofID, 0 );
    commandline.parse( "-object", "object name", &objectName, 0 );
    commandline.parse( "-data", "data name", &dataName, 0 );
    commandline.parse( "-value", "value to spoof data to", &valueName, 0 );
    commandline.parse( "-mult", "spoof multiplier", &multiplier, 0, -1000000, 1000000 );
    commandline.parse( "-offset", "spoof offset", &offset, 0, -1000000, 1000000 );
    commandline.parse( "-enable", "enables spoofing", &enable );
    commandline.parse( "-disable", "disables spoofing", &disable );
    commandline.parse( "-end", "ends spoofing", &end );
    commandline.parse( "-unspoof", "unspoofs given object and data", &unspoof );
    commandline.parse( "-sd", "show data that can be spoofed", &showSpoofableData );
    commandline.parse( "-se", "show enums supported", &showEnums );
    commandline.parse( "-so", "show objects that can be spoofed", &showSpoofableObjects );
    commandline.parse( "-ss", "show spoofs that can be run", &showSpoofs );
    commandline.parse( "-trace", "trace spoofs as they occur", &traceSpoofs );
    commandline.parse( "-send", "send specified int message", &intMessageName, 0 );
    commandline.parse( "-int", "int data to send", &intData, 1, -2000000000, 2000000000 );
    commandline.parse( "-debug", "enable debug messages", &debug );
    commandline.parse( "-serverDebug", "enable server debug messages", &serverDebug );
    commandline.parse( "-timeout", "new timeout for this spoof", &timeout, 0, 1, 36000 );
    commandline.parse( "-servers", "number of spoofer server tasks", &slaves, 2, 1, 10 );
    commandline.parse( "-help", "prints help on specified spoof", &help );
    commandline.parse( "-priority", "sets task priority", &priority, 30, 0, 255 );
    commandline.parse( "-traffic", "set msg traffic multiple, -1 = no change", &traffic, -1, -1, 100 );
    commandline.parse( "-suspendControl", "task to suspend on control computer", &suspendControlTask, 0 );
    commandline.parse( "-suspendSafety", "task to suspend on safety computer", &suspendSafetyTask, 0 );
    commandline.parse( "-removeFile", "looks for file and removes it when found", &removeFile, 0 );
    commandline.parse( "-logBurst", "burst n logmsgs each second", &logBurst, 0, 0, 1000 );
	commandline.parse( "-var1", "generic value 1", &Var1, 0 );
	commandline.parse( "-var2", "generic value 2", &Var2, 0 );
    commandline.parse( "-beginSubstate", "substate transition at which to activate the spoof", &beginSubstate, 0 );
    commandline.parse( "-timeFromActive", "start duration timer when spoof is activated by triggering message or substate transition", 
                       &timeFromActive, 0, 1, 36000 );
    commandline.parse( "-endSubstate", "substate transition at which to stop spoofing", &endSubstate, 0);
    commandline.parse( "-delta", "duration in seconds after triggering substate transition or message to start spoofing",
                       &delta, 0, 1, 36000 );
    commandline.parse( "-beginMsg", "message to wait for before activating the spoof", &beginMsg, 0 );
    commandline.parse( "-endMsg", "message trigger to stop spoofing", &endMsg, 0);
    commandline.parse( "-snap", "save screenshot w. alpha prefix: [-1 to -26, +1 to +26] => [a-z, A-Z]",
                       &snapPrefix, 0, -26, 26 );

    
    commandline.done();

    if( commandline.anyErrors() )
    {
        fprintf(stdout, "\n");
        LOG_ERROR( "command-line parsing error!" );
        return 1;
    }
    else if (((delta >0) && (beginSubstate == NULL) && (beginMsg == NULL)) || (delta < 0))
    {
        LOG_ERROR( "delta option can only be used with beginMsg or beginSubstate options" );
        return 1;
    }
    else if ( (timeFromActive > 0) && (beginSubstate == NULL) && (beginMsg == NULL))
    {
        LOG_ERROR( "timeFromActive option can only be used with beginMsg or BeginSubstate options" );
        return 1;
    }
    else if ((!(beginSubstate == NULL) && !(endMsg == NULL)) || (!(endSubstate == NULL) && !(endMsg == NULL))
             || (!(beginSubstate == NULL) && !(beginMsg == NULL)) || (!(beginMsg == NULL) && !(endSubstate == NULL)) )
    {
        LOG_ERROR( "must specify only substate based or only message based begin and end options not a mixture" );
        return 1;
    }


    // enable debug msgs
    // verify that any object name entered is actually supported
    if( debug )
    {
		log_level_spoofer_debug.logOutput(DataLog_LogEnabled);
        if (objectName)
        {
            if (!verify::isSupportedObject(objectName)) {
                LOG_ERROR("Object %s is not supported", objectName);
                return 1;
            }
            else
                LOG_DEBUG("Object %s supported", objectName);
        }
    }
    else
    {
		log_level_spoofer_debug.logOutput(DataLog_LogDisabled);
    }

    taskPrioritySet( taskIdSelf(), priority ); 

    //
    // following handles remove file request
    //
    if( removeFile )
    {
       DataLog( log_level_spoofer_info ) << "looking for " << removeFile
          << " for " << timeout << " seconds" << endmsg;
       LOG_INFO( "looking for %s for %d seconds", removeFile, timeout );

       // loop until timeout or until file is removed
       Clock tmo;
       for( tmo.start(); tmo.getElapsedSec() <= timeout; )
       {
          if( remove( removeFile ) == OK )
          {
             DataLog( log_level_spoofer_info ) << removeFile
                << " removed after " << tmo.getElapsedSec() << " seconds" << endmsg;
             LOG_INFO( "%s removed after %d seconds", removeFile, tmo.getElapsedSec() );
             return 0;
          }
          else
          {
             taskDelay( 10 ); // give someone else time to run
          }
       }

       DataLog( log_level_spoofer_error ) << removeFile
          << " not found after " << timeout << " seconds" << endmsg;
       LOG_ERROR( "%s not found after %d seconds", removeFile, timeout );
       return 0;
    }

    SpoofClient spoofer( slaves );
    SpooferAPI* p = &spoofer;

    //
    // following handles help options
    //

    if( objectName == 0 && showSpoofableData == true )
    {
        SpoofableData::dumpAll(); // dump all field info data
        SpoofableData::cleanup();
        return 0;
    }

    if( showSpoofableObjects )
    {
        SpoofableData::showObjectNames();
        SpoofableData::cleanup();
        return 0;
    }

    if( showSpoofableData )
    {
        SpoofableData::showDataNames( objectName );
        SpoofableData::cleanup();
        return 0;
    }

    if( showEnums )
    {
        EnumInfo::dumpAll();
        SpoofableData::cleanup();
        return 0;
    }


    //
    // following handles logging requests 
    //
    if( logBurst )
    {
       p->loggingTest( logBurst, timeout );
       return 0;
    }

    //
    // following handles task suspend requests
    //

    if( suspendControlTask )
    {
       p->suspend( "CONTROL", suspendControlTask );
       return 0;
    }

    if( suspendSafetyTask )
    {
       p->suspend( "SAFETY", suspendSafetyTask );
       return 0;
    }


    //
    //  following handles sending int messages
    //

    if( intMessageName )
    {
        p->messageSend( intMessageName, intData );
        return 0;
    }

    //
    // following handles screenshot request (-snap)
    //
    if (snapPrefix != 0)
    {
       int asciiCode = ( (snapPrefix < 0)
                         ? (96 - snapPrefix)
                         : (64 + snapPrefix) );

       if ( isalpha(asciiCode) )
       {
          LOG_INFO( "Sending SnapScreenShotMsg: prefix %d => '%c%c_'",
                    snapPrefix, (char)asciiCode, (char)asciiCode );
          p->messageSend( "SnapScreenshotMsg", asciiCode );
          return 0;
       }
       else
       {
          LOG_ERROR( "Screenshot prefix %d is *not* a printable letter!", snapPrefix );
          return 2;
       }
    }

    //
    //  following handles spoofing options
    //
    if( serverDebug )
    {
       p->enableDebug();
    }
    else
    {
       p->disableDebug();
    }

    if( traceSpoofs )
    {
       p->showSpoofs();
    }

    // following 3 conditionals are mutually exclusive
    if( objectName && dataName && valueName )
    {
        p->spoofData( objectName, dataName, valueName );
    }
    else if( objectName && dataName && unspoof )
    {
        p->unspoofData( objectName, dataName );
    }
    else if( objectName && dataName )
    {
        p->spoofData( objectName, dataName, multiplier, offset );
    }

    // set traffic multiplier prior to enable/disable/end
    if( traffic != -1 )
    {
        p->msgMultiplier( traffic );
    }

    if( enable )
    {
        p->spoofEnable();
    }
    if( disable )
    {
        p->spoofDisable();
    }
    if( end )
    {
        p->spoofEnd();
    }

    //
    // instantiate my spoofs
    //
    MySpoofs mySpoofs( p );
    mySpoofs.init();

    if( showSpoofs == true )
    {
        if( spoofID == 0 )
        {
            mySpoofs.showAll();
        }
        else
        {
            mySpoofs.show( spoofID );
        }
    }
    // added to allow for parameterized spoofs (IT 7220)
	else if (spoofID && !help && (!(Var1 == NULL) || !(Var2 == NULL)))
	{   
        if (!(beginSubstate == NULL) && !(endSubstate == NULL))
        {
            isSS = TRUE;
            mySpoofs.run_parameterized_after_endby( spoofID, timeout, 
                         beginSubstate, delta, isSS, timeFromActive, endSubstate, Var1, Var2 );
            p->spoofEnd();
        }
        else if (!(beginSubstate == NULL)) 
        {  
            isSS = TRUE;
            mySpoofs.run_parameterized_after( spoofID, timeout, beginSubstate, delta, isSS, timeFromActive, Var1, Var2 );
            p->spoofEnd();
        }
        else if (!(endSubstate == NULL)) 
        {   
            isSS = TRUE;
            mySpoofs.run_parameterized_endby(spoofID, timeout, endSubstate, isSS, Var1, Var2 );
            p->spoofEnd();
        }
        else if (!(beginMsg == NULL) && !(endMsg == NULL) )
        {
            isSS = FALSE;
            mySpoofs.run_parameterized_after_endby( spoofID, timeout, 
                          beginMsg, delta, isSS, timeFromActive, endMsg, Var1, Var2 );
             p->spoofEnd();
        }
        else if (!(beginMsg == NULL))
        {
            isSS = FALSE;
            mySpoofs.run_parameterized_after( spoofID, timeout, beginMsg, delta, 
                                              isSS, timeFromActive, Var1, Var2 );
            p->spoofEnd();
        }
        else if (!(endMsg == NULL))
        {
            isSS = FALSE;
            mySpoofs.run_parameterized_endby(spoofID, timeout, endMsg, isSS, Var1, Var2 );
            p->spoofEnd();
        }
        else
        {
            mySpoofs.run_parameterized( spoofID, timeout, Var1, Var2 );
            p->spoofEnd();
        }
	}
    else if (spoofID && !help && !(Var1 == NULL) && (Var2 == NULL)) 
    {
        if (!(beginSubstate == NULL) && !(endSubstate == NULL))
        {
            isSS = TRUE;
            mySpoofs.run_parameterized_after_endby( spoofID, timeout, 
                         beginSubstate, delta, isSS, timeFromActive, endSubstate, Var1, Var2 );
            p->spoofEnd();
        }
        else if (!(beginSubstate == NULL)) 
        {
            isSS = TRUE;
            mySpoofs.run_parameterized_after( spoofID, timeout, beginSubstate, delta, 
                                              isSS, timeFromActive, Var1, Var1 );
            p->spoofEnd();
        }
        else if (!(endSubstate == NULL)) 
        {   
            isSS = TRUE;
            mySpoofs.run_parameterized_endby(spoofID, timeout, endSubstate, isSS, Var1, Var1 );
            p->spoofEnd();
        }
        else if (!(beginMsg == NULL) && !(endMsg == NULL) )
        {
            isSS = FALSE;
            mySpoofs.run_parameterized_after_endby( spoofID, timeout, 
                          beginMsg, delta, isSS, timeFromActive, endMsg, Var1, Var1 );
             p->spoofEnd();
        }
        else if (!(beginMsg == NULL))
        {
            isSS = FALSE;
            mySpoofs.run_parameterized_after( spoofID, timeout, beginMsg, delta, 
                                              isSS, timeFromActive, Var1, Var1 );
            p->spoofEnd();
        }
        else if (!(endMsg == NULL))
        {
            isSS = FALSE;
            mySpoofs.run_parameterized_endby(spoofID, timeout, endMsg, isSS, Var1, Var1 );
            p->spoofEnd();
        }
        else
        {
            mySpoofs.run_parameterized( spoofID, timeout, Var1, Var1 );
            p->spoofEnd();
        }
    }
    else if ( spoofID && !help)
    {
        if (!(beginSubstate == NULL) && !(endSubstate == NULL))
        {
            isSS = TRUE;
            mySpoofs.run_after_endby( spoofID, timeout, 
                         beginSubstate, delta, isSS, timeFromActive, endSubstate );
            p->spoofEnd();
        }
        else if (!(beginSubstate == NULL)) 
        {
            isSS = TRUE;
            mySpoofs.run_after( spoofID, timeout, beginSubstate, delta, isSS, timeFromActive );
            p->spoofEnd();
        }
        else if (!(endSubstate == NULL)) 
        {   
            isSS = TRUE;
            mySpoofs.run_endby(spoofID, timeout, endMsg, isSS );
            p->spoofEnd();
        }
        else if (!(beginMsg == NULL) && !(endMsg == NULL) )
        {
            isSS = FALSE;
            mySpoofs.run_after_endby( spoofID, timeout, 
                          beginMsg, delta, isSS, timeFromActive, endMsg );
             p->spoofEnd();
        }
        else if (!(beginMsg == NULL))
        {
            isSS = FALSE;
            mySpoofs.run_after( spoofID, timeout, beginMsg, delta,  isSS, timeFromActive );
            p->spoofEnd();
        }
        else if (!(endMsg == NULL))
        {
            isSS = FALSE;
            mySpoofs.run_endby(spoofID, timeout, endMsg, isSS );
            p->spoofEnd();
        }
        else
        {
            mySpoofs.run( spoofID, timeout );
            p->spoofEnd();
        }
    }
    else if( spoofID && help )
    {
        mySpoofs.help( spoofID );
    }

    DataLog( log_level_spoofer_info ) << "normal exit" << endmsg;

    return 0;
} 
