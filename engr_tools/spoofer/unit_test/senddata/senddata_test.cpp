// FILENAME:  senddata_test.cpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/unit_test/senddata/rcs/senddata_test.cpp 6.1 2002/11/19 20:45:45Z td07711 Exp td07711 $
// PURPOSE:  test messageSend() functionality
//   BASIC USAGE - the user specifies the following keyword value command line options:
//     -o <object_name> - msg name or CDS element name
//     -d <data_name> - name of data to get
//     -v <value> - data value
//     -sd - shows the data supported for the specified object.
//     -sa - shows all object/data names supported
//     -debug - enables debug trace messages 
//
//   Messages supported - spoofable msgs defined in SpoofedMsgBase::create()
//                        sendable msgs defined in NamedMsgBase::create()
//   Fields supported - classes defined in fieldinfo.hpp
//                      instantiated in FieldInfo::init()
//   Enums supported - defined in FieldInfo::init()
//
// CHANGELOG:
//   $Log: senddata_test.cpp $
//   Revision 6.1  2002/11/19 20:45:45Z  td07711
//   extern __cplusplus
//   Revision 1.1  2002/11/18 18:04:44  td07711
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
extern "C" int senddata_test( const char* args );
#endif

int senddata_test( const char* args )
{
    Logger::Init("senddata_test");

    // parse command line
    OptionParser commandline("senddata_test", "tests messageSend() functionality of SpooferAPI");
    commandline.init(args);
    const char* objectName;
    const char* valueName;
    bool showObjectNamesFlag;
    bool showDataNamesFlag;
    bool debugFlag;
    bool showAllFlag;
    commandline.parse("-o", "object name", &objectName, 0);
    commandline.parse("-v", "value name", &valueName, 0);
    commandline.parse("-sa", "show info for all fields supported", &showAllFlag);
    commandline.parse("-so", "show object names supported", &showObjectNamesFlag);
    commandline.parse("-sd", "show data names supported for specified message type", &showDataNamesFlag);
    commandline.parse("-debug", "enable debug messages", &debugFlag);
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


    SpoofClient spoofer;
    SpooferAPI* p = &spoofer;
//    p->setTimeout( timeout );

    if( showAllFlag )
    {
        SpoofableData::dumpAll(); // dump all field info data
        return 0;
    }
    if( showObjectNamesFlag )
    {
        SpoofableData::showObjectNames();
        return 0;
    }
    if( showDataNamesFlag )
    {
        if( objectName == 0 )
        {
            LOG_ERROR("-sd usage requires a -o objectName");
            return 1;
        }
        SpoofableData::showDataNames( objectName );
        return 0;
    }


    // exercise messageSend as desired
    
    // restricted to sending int message
    int value;
    if( sscanf( valueName, "%d", &value) != 1 )
    {
	LOG_ERROR( "sscanf failed on %s", valueName );
	return 1;
    }

    if( SpoofableData::get( objectName, "data" ) == 0 )
    {
	LOG_ERROR( "%s.data is not supported", objectName ); 
	return 1;
    }

    p->messageSend( objectName, value );


    return 0;
}
