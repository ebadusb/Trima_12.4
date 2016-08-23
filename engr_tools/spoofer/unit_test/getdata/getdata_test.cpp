// FILENAME:  getdata_test.cpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/unit_test/getdata/rcs/getdata_test.cpp 6.3 2002/11/21 22:11:02Z td07711 Exp td07711 $
// PURPOSE:  test getData() functionality
//   BASIC USAGE - the user specifies the following keyword value command line options:
//     -t <timeout> - time to wait for the data in msec
//     -o <object_name> - msg name or CDS element name
//     -d <data_name> - name of data to get
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
//   $Log: getdata_test.cpp $
//   Revision 6.3  2002/11/21 22:11:02Z  td07711
//   indent
//   Revision 6.2  2002/11/19 20:58:47  td07711
//   extern __cplusplus
//   Revision 6.1  2002/11/19 20:42:22  td07711
//   ifdef __cplusplus
//   Revision 1.1  2002/11/18 18:04:26  td07711
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
extern "C" int getdata_test( const char* args );
#endif

int getdata_test( const char* args )
{
    Logger::Init("getdata_test");

    // parse command line
    OptionParser commandline("getdata_test", "tests getData() functionality of SpooferAPI");
    commandline.init(args);
    const char* objectName;
    const char* dataName;
    int timeoutMsec;
    int timeout;
    bool showObjectNamesFlag;
    bool showDataNamesFlag;
    bool debugFlag;
    bool showAllFlag;
    bool bailFlag;
    commandline.parse("-o", "object name", &objectName, 0);
    commandline.parse("-d", "data name", &dataName, 0);
    commandline.parse("-sa", "show info for all fields supported", &showAllFlag);
    commandline.parse("-so", "show object names supported", &showObjectNamesFlag);
    commandline.parse("-sd", "show data names supported for specified message type", &showDataNamesFlag);
    commandline.parse("-debug", "enable debug messages", &debugFlag);
    commandline.parse("-tmsec", "getData timeout in msec", &timeoutMsec, 10000, 1, 100000);
    commandline.parse("-timeout", "spoof timeout in seconds", &timeout, 30, 1, 10000);
    commandline.parse("-bail", "return right away", &bailFlag);
    commandline.done();

    if( commandline.anyErrors() )
    {
	return 1;
    }

    if( bailFlag )
    {
        LOG_INFO("bailing out");
        return 0;
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
    p->setTimeout( timeout );

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


    // exercise getData as desired
    char buf[ 10 ];
    const SpoofableData* pDataInfo = SpoofableData::get( objectName, dataName );
    if( pDataInfo == 0 )
    {
        LOG_ERROR( "get failed on %s.%s", objectName, dataName );
        return 1;
    }

    LOG_INFO( "calling getData with timeout=%d msec", timeoutMsec );
    bool rval = p->getData( objectName, dataName, buf, pDataInfo->size(), timeoutMsec );

    if( rval == false )
    {
        LOG_INFO( "getData on %s.%s timed out", objectName, dataName );
    }
    else
    {
        LOG_INFO( "getData on %s.%s = %s", 
                 objectName, dataName, pDataInfo->valueToString( buf ).c_str() );
    }

    return 0;
}
