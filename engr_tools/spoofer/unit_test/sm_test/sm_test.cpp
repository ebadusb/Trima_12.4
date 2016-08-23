// FILENAME:  sm_test.cpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/unit_test/sm_test/rcs/sm_test.cpp 6.9 2002/11/19 20:45:28Z td07711 Exp td07711 $
// PURPOSE:  main for playing with SpooferMaster functionality
//   BASIC USAGE - the user specifies the following keyword value command line options:
//     -t <type_name> - specifies the message type
//     -i <iterations> - number of times to send/receive
//     -st - shows the message types supported
//     -sf - shows the fields supported for the specified message type.
//     -se - shows the enums and values supported
//     -sa - shows all spoofable fields
//     -debug - enables debug trace messages 
//
//   Messages supported - spoofable msgs defined in SpoofedMsgBase::create()
//                        sendable msgs defined in NamedMsgBase::create()
//   Fields supported - classes defined in fieldinfo.hpp
//                      instantiated in FieldInfo::init()
//   Enums supported - defined in FieldInfo::init()
//
// CHANGELOG:
//   $Log: sm_test.cpp $
//   Revision 6.9  2002/11/19 20:45:28Z  td07711
//   extern __cplusplus
//   Revision 6.8  2002/11/18 18:07:28  td07711
//   replace SpooferMaster with SpoofClient
//   Revision 6.7  2002/10/25 20:43:37  td07711
//   timeout mods
//   Revision 6.6  2002/10/18 23:21:35  td07711
//   spoof testing
//   Revision 6.5  2002/10/17 22:50:15  td07711
//   add CDS element spoofing
//   Revision 6.4  2002/10/10 21:31:31  td07711
//   exercise showSpoofs() and unspoofData()
//   Revision 6.3  2002/10/08 00:07:25  td07711
//   add -send and -int options
//   Revision 6.2  2002/10/04 21:11:46  td07711
//   replace FieldInfoBase with SpoofableData
//   Revision 6.1  2002/10/02 22:48:44  td07711
//   add -delay <n> option
//   Revision 1.1  2002/10/01 22:42:08  td07711
//   Initial revision


// vxworks includes
#include <vxWorks.h>
#include <usrLib.h>

// standard includes
#include <stdio.h>

// common includes
#include "spoofermessagesystem.h"
#include "optionparser.h"

// trima includes
#include "chw.hpp"
#include "predict_cds.h"

#undef LOG_FATAL
#undef LOG_ERROR

// libtools includes
#include "spooferlog.hpp"
 
// spoofer includes
#include "spoofclient.hpp"


#ifdef __cplusplus
extern "C" int sm_test( const char* args );
#endif

int sm_test( const char* args )
{
    Logger::Init("sm_test");

    // parse command line
    OptionParser commandline("sm_test", "testing a SpooferMaster task");
    commandline.init(args);
    const char* objectName;
    bool showObjectNamesFlag;
    bool showDataNamesFlag;
    bool debugFlag;
    bool showAllFlag;
    int iterations;
    int maxSpoofs;
    int delaySeconds;
    bool showEnumsFlag;
    bool sendFlag;
    bool showSpoofsFlag;
    int intData;
    int timeout;
    commandline.parse("-object", "object name", &objectName, "unknown");
    commandline.parse("-sa", "show info for all fields supported", &showAllFlag);
    commandline.parse("-se", "show enums supported", &showEnumsFlag);
    commandline.parse("-so", "show object names supported", &showObjectNamesFlag);
    commandline.parse("-sd", "show data names supported for specified message type", &showDataNamesFlag);
    commandline.parse("-ss", "show spoofs as they occur", &showSpoofsFlag);
    commandline.parse("-send", "send specified int message", &sendFlag);
    commandline.parse("-int", "int data to send", &intData, 1, -2000000000, 2000000000);
    commandline.parse("-n", "iterations to do something", &iterations, 1, 1, 1000);
    commandline.parse("-max", "max msgs to spoof, 0 = no limit", &maxSpoofs, 0, 0, 1000);
    commandline.parse("-delay", "delay (seconds) between iterations", &delaySeconds, 1, 0, 100);
    commandline.parse("-debug", "enable debug messages", &debugFlag);
    commandline.parse("-timeout", "timeout in seconds", &timeout, 10000, 1, 10000);
    commandline.done();

    if( commandline.anyErrors() )
    {
	return 1;
    }

    // enable debug msgs
    if( debugFlag )
    {
		log_level_spoofer_debug.logOutput(DataLog_LogEnabled);
    }
    else
    {
		log_level_spoofer_debug.logOutput(DataLog_LogDisabled);
    }


    if( objectName )
    {
        LOG_DEBUG("message type = %s", objectName);
    }
    LOG_DEBUG("iterations = %i", iterations);


    SpoofClient spoofer;
    SpooferAPI* p = &spoofer;
    p->setTimeout( timeout );

    if( showAllFlag )
    {
        SpoofableData::dumpAll(); // dump all field info data
        SpoofableData::cleanup();
        return 0;
    }
    if( showObjectNamesFlag )
    {
        SpoofableData::showObjectNames();
        SpoofableData::cleanup();
        return 0;
    }
    if( showDataNamesFlag )
    {
        if( objectName == 0 )
        {
            LOG_ERROR("-sf requires a message type");
            SpoofableData::cleanup();
            return 1;
        }
        SpoofableData::showDataNames( objectName );
        SpoofableData::cleanup();
        return 0;
    }
    if( showEnumsFlag )
    {
        EnumInfo::dumpAll();
        SpoofableData::cleanup();
        return 0;
    }
    if( sendFlag )
    {
        p->messageSend( objectName, intData );
        return 0;
    }

    //
    // setup CDS data
    //
    Predict_CDS pcds( ROLE_RW );
    Predict_CDS::substate_info_t si;
    si[ 1 ].Qac = 10;
    pcds.SubstateInfo.Set( si );

    Predict_CDS::substate_info_t siGot;
    siGot = pcds.SubstateInfo.Get();
    LOG_INFO( "original Predict_SubstateInfo.Qac = %1.1f", siGot[ 1 ].Qac );

    // 
    // setup a spoof
    //
    p->spoofData( "CHwOrders", "acRPM", "100" );
    p->spoofData( "CHwOrders", "cassettePosition", "CHW_CASSETTE_DOWN" );
    p->spoofData( "CHwOrders", "plasmaValve", "CHW_VALVE_OPEN" );
    p->spoofData( "ProcStateMsg", "data", "456" ); // intdata spoofed to 456
    p->spoofData( "EndProcedureMsg", "data", 0, -5 ); // intdata*0 -5
    p->spoofData( "CHwStates", "returnRPM", 2, 5 ); // data*2 + 5
    p->spoofData( "Predict_SubstateInfo", "Qac_1", "999" );
    p->spoofActivate();

    p->setSpoofedMessageLimit( maxSpoofs );

    if( showSpoofsFlag )
    {
        p->showSpoofs();
    }

    // 
    // originate some spoofable messages 
    //
    CHwOrders order;
    memset(&order, 0, sizeof(order));
    order.acRPM = 123;

    CHwStates status;
    memset( &status, 0, sizeof(status) );
    status.returnPump.RPM = 175;

    struct timespec delayTime;
    delayTime.tv_sec = delaySeconds;
    delayTime.tv_nsec = 0;

    int i;
    for(i = 1; i <= iterations; i++)
    {
        LOG_INFO("messageSend ProcStateMsg");
        p->messageSend( "ProcStateMsg", i );

        LOG_INFO("messageSend CHwOrders");
        p->messageSend( "CHwOrders", &order, sizeof(order) );

        LOG_INFO("messageSend CHwStates");
        p->messageSend( "CHwStates", &status, sizeof(status) );

        LOG_INFO("messageSend EndProcedureMsg");
        p->messageSend( "EndProcedureMsg", i+10 );

        siGot = pcds.SubstateInfo.Get();
        LOG_INFO( "getting spoofed Predict_SubstateInfo.Qac = %1.1f", siGot[ 1 ].Qac );

        nanosleep( &delayTime, 0 );
    }

    p->unspoofData( "CHwOrders", "acRPM" );
    p->unspoofData( "ProcStateMsg", "data" );
    p->unspoofData( "CHwStates", "returnRPM" );
    p->unspoofData( "Predict_SubstateInfo", "Qac_1" );

    
    LOG_INFO("messageSend ProcStateMsg - data should no longer be spoofed");
    p->messageSend( "ProcStateMsg", i );

    LOG_INFO("messageSend CHwOrders - acRPM should no longer be spoofed");
    p->messageSend( "CHwOrders", &order, sizeof(order) );

    LOG_INFO("messageSend CHwStates - returnRPM should no longer be spoofed");
    p->messageSend( "CHwStates", &status, sizeof(status) );

    LOG_INFO("messageSend EndProcedureMsg");
    p->messageSend( "EndProcedureMsg", i+10 );

    siGot = pcds.SubstateInfo.Get();
    LOG_INFO( "Predict_SubstateInfo.Qac=%1.1f should no longer be spoofed", 
             siGot[ 1 ].Qac );

    p->spoofEnd();


    LOG_INFO("normal exit");
    return 0;
}
