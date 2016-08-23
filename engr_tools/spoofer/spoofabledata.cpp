// FILENAME: spoofabledata.cpp
// $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/spoofabledata.cpp 6.57 2009/06/03 14:17:45Z jsylusb Exp $
// PURPOSE: implements SpoofableData class
// CHANGELOG:
//   $Log: spoofabledata.cpp $
//   Revision 6.57  2009/06/03 14:17:45Z  jsylusb
//   Fixed improper use of iterators which can cause page faults.
//   Revision 6.56  2009/02/03 19:01:11Z  kgreusb
//   Adding Pump revs for testing 7720 and 6514
//   Revision 6.55  2009/01/10 00:29:31Z  kgreusb
//   For IT 8937 changing control centrifugeError enum form HW_ERROR to HW_CENT_ERRORS
//   Revision 6.54  2008/12/01 18:44:36Z  kgreusb
//   Added spoofable object for testing IT 6933
//   Revision 6.53  2008/08/15 22:06:13Z  kgreusb
//   Adding substate names enum in support of Next Generation spoofer
//   Revision 6.52  2008/05/13 14:56:36Z  kgreusb
//   Added Spoof_SafetyTestDoneMsg
//   Revision 6.51  2007/10/30 21:31:52Z  kgreusb
//   Added substates to mapping since PTF RBC has increased the number of substates.
//   Revision 6.50  2007/09/11 14:12:07Z  kgreusb
//   Adding centrifugeRamp messages for autotest spoofs
//   Revision 6.49  2007/02/27 22:40:50Z  kgreusb
//   Revision 6.48  2007/02/23 22:50:52Z  kgreusb
//   Adding ability to spoof for safety only for IT 7828
//   Revision 6.47  2006/10/06 14:58:12Z  kgreusb
//   Added DisconnectStatMsg spoofing to test donor disconnect for ITs 7178, 7179
//   Revision 6.46  2004/04/15 18:47:03Z  td07711
//   IT6922 - make ProcRunTarget_CDS.SelectedYieldTarget spoofable
//   Revision 6.45  2003/12/30 21:42:46Z  td07711
//   IT6641 - support more data fields
//   Revision 6.44  2003/11/21 19:53:49Z  td07711
//   IT6585 - support control status cassette position
//   Revision 6.43  2003/10/27 22:30:13Z  td07711
//   IT6528 - bugfix introduced in 6.42, missing break statements.
//   Revision 6.42  2003/10/23 18:25:14Z  td07711
//   IT3338 - support fastdata spoofing
//   Revision 6.41  2003/10/15 20:34:01Z  td07711
//   IT6485 - support CHwStates.twentyFourVoltsSwitched
//   Revision 6.40  2003/09/22 22:19:01Z  td07711
//   IT6347 - support Predict_SubstateInfo valve position spoofs
//   Revision 6.39  2003/09/03 18:10:00Z  td07711
//   IT6347 - support Predict_CDS substateInfo VtargetType and OrVtargetType
//   Revision 6.38  2003/08/15 21:17:07Z  td07711
//   IT6347 - support other Predict_CDSsubstateInfo Vtargets
//   Revision 6.37  2003/08/14 18:59:35Z  td07711
//   IT6355 - support flow spoof during rbc collect
//   Revision 6.36  2003/08/13 15:20:23Z  td07711
//   IT6356 - moved SDonorVitals spoofing from control to safety computer.
//   Revision 6.35  2003/08/08 22:02:00Z  td07711
//   IT6254 - support more donor vitals stuff
//   Revision 6.34  2003/08/08 19:21:37Z  td07711
//   IT6330 - remove duplicate definition of HW_VALVE_STATES
//   Revision 6.33  2003/08/08 19:00:22Z  td07711
//   IT6254 - add support for volume targets and yield
//   Revision 6.32  2003/08/07 22:20:56Z  td07711
//   IT6254 - support control status for valve positions
//   Revision 6.31  2003/08/05 22:27:25Z  td07711
//   IT6290 - support spoofing of pump encoders
//   Revision 6.30  2003/07/24 20:32:09Z  td07711
//   IT6254 - support CHwStates.minusTwelvePS
//   Revision 6.29  2003/07/15 16:11:41Z  td07711
//   IT6202 - added support for CHwStates.centrifugeRPM
//   Revision 6.28  2003/06/27 16:39:45Z  td07711
//   IT6164 - added support for CHwStates.doorLocks
//   Revision 6.27  2003/06/12 17:59:36Z  td07711
//   support ProcRunTargets_RbcVolumeTarget.data - IT5443
//   Revision 6.26  2003/05/22 22:45:08Z  td07711
//   only #include cds files for control side build
//   Revision 6.25  2003/05/14 23:22:38Z  td07711
//   support CHwStates.twentyFourVoltSupply and twelveVoltSupply
//   Revision 6.24  2003/05/14 17:54:40Z  td07711
//   support CHwStates.pauseSwitch and CHwStates.stopSwitch
//   Revision 6.23  2003/05/12 23:17:41Z  td07711
//   support predict_cds.h QpD, QpR, QcD, QcR
//   Revision 6.22  2003/04/22 18:48:02Z  td07711
//   support "RequestSafetyPowerMsg"
//   Revision 6.21  2003/04/16 19:56:55Z  td07711
//   support CHwStates.sixtyFourVoltSupply and DVitalsFromProc.DonorHct
//   Revision 6.20  2003/04/14 23:26:22Z  td07711
//   support CHwStates.leakDetector and SHwStates.xxxValve
//   Revision 6.19  2003/04/03 23:59:46Z  td07711
//   support CHwStates.inletPumpAccum
//   support procvolumes_cds
//   Revision 6.18  2003/04/03 21:15:40Z  td07711
//   support spoofing DVitalsFromProc msgs
//   Revision 6.17  2003/04/02 00:11:13Z  td07711
//   support SHwStates.centrifugeRPM
//   Revision 6.16  2003/04/01 01:17:09Z  td07711
//   support SystemStateChangeMsg and SHwOrders.valveLED
//   Revision 6.15  2003/03/28 02:35:01Z  td07711
//   parse and display CHAR data as hex
//   Revision 6.14  2003/03/14 23:49:23Z  td07711
//   add support for safety status door state
//   Revision 6.13  2003/02/18 00:30:51Z  td07711
//   support SHwStates.valveLedTest
//   Revision 6.12  2003/02/11 22:44:44Z  td07711
//   enable safety side spoofs
//   Revision 6.11  2003/02/06 23:29:36Z  td07711
//   rename compile flags for building safety version of spoof_server
//   Revision 6.10  2003/01/31 23:44:47Z  td07711
//   split support for SAFETY and CONTROL
//   Revision 6.9  2003/01/30 00:19:33Z  td07711
//   add some spoofs
//   Revision 6.8  2003/01/23 23:43:01Z  td07711
//   add support for red/green detectors
//   Revision 6.7  2003/01/22 01:25:33Z  td07711
//   add support for HalStatus_aps
//   Revision 6.6  2003/01/17 01:56:53Z  td07711
//   bugfix - for -so option, filter redundant objects properly
//   Revision 6.5  2002/11/12 00:47:57Z  td07711
//   add OBJECT_TYPE info
//   Revision 6.4  2002/10/24 21:17:20  td07711
//   support char and short data types
//   Revision 6.3  2002/10/22 23:19:47  td07711
//   make more data fields spoofable
//   Revision 6.2  2002/10/17 23:03:50  td07711
//   add support for Predict_CDS SubstateInfo Qac spoofing
//   Revision 6.1  2002/10/09 20:58:14  td07711
//   add min/max for float to string conversion

//
// trima includes for stuff that can be spoofed
//
#include "chw.hpp"
#include "shw.hpp"
#include "states.h"
#include "safproc.h"
#include "fastdata.h"

#ifdef SUPPORT_CONTROL_SPOOFS
#include "predict_cds.h"
#include "haldefs.h"
#include "procrun_cds.h"
#include "halstatus_cds.h"
#include "procvolumes_cds.h"
#include "procruntargets_cds.h"
#include "proccycle_cds.h"
#include "proctrimaset_cds.h"
#include "procdonor_cds.h"

#endif // SUPPORT_CONTROL_SPOOFS

#undef LOG_FATAL
#undef LOG_ERROR


//
// spoofer includes
//
#include "spoofabledata.hpp"
#include "spooferlog.hpp"
#include "enuminfo.hpp"
#include "procstate.h"


// initialize statics
map<string, SpoofableData*> SpoofableData :: _database;


SpoofableData :: SpoofableData( const char* objectName, const char* name,
                               void* objectAddress, void* dataAddress,
                               DATA_TYPE type, const char* enumName )
: _name( name ), _objectName( objectName ), 
  _type( type), _pEnum( 0 ), _objectType( MSG )
{
    commonInit( objectAddress, dataAddress, enumName );
}


SpoofableData :: SpoofableData( OBJECT_TYPE objectType, const char* objectName, const char* name,
                               void* objectAddress, void* dataAddress,
                               DATA_TYPE type, const char* enumName )
: _name( name ), _objectName( objectName ), 
  _type( type), _pEnum( 0 ), _objectType( objectType )
{
    commonInit( objectAddress, dataAddress, enumName );
}


void SpoofableData :: commonInit( void* objectAddress, void* dataAddress, const char* enumName )
{
    ASSERT( _objectName );
    ASSERT( _name );
    ASSERT( get( _objectName, _name) == 0 ); // verify not added already

    _offset = (int)dataAddress - (int)objectAddress;
    ASSERT( _offset >= 0 );
    ASSERT( _offset < 10000 ); 

    switch( _type )
    {
        case INT :
            _size = sizeof( int );
            break;

        case FLOAT :
            _size = sizeof( float );
            break;

        case ENUM :
            _size = sizeof( int );
            _pEnum = EnumInfo::get( enumName );
            ASSERT( _pEnum );
            break;

        case SHORT :
            _size = sizeof( short );
            break;

        case CHAR :
            _size = sizeof( char );
	    break;

        case FASTDATA_ARRAY :
	    _size = sizeof( short ) * FASTDATALENGTH;
	    break;

        default :
            ASSERT( "invalid data type" );
    }

    //
    // add to spoofable data database
    //
    string key = _objectName;
    key += ".";
    key += _name;
    _database[ key ] = this;
}


SpoofableData :: SpoofableData( const char* intMessageName )
: _objectName( intMessageName ), _name( "data" ), 
  _type( INT ), _pEnum( 0 ), _offset( 0 ), _size( sizeof(int) ),
  _objectType( MSG )
{
    ASSERT( _objectName );
    ASSERT( get( _objectName, _name) == 0 ); // verify not added already

    //
    // add to spoofable data database
    //
    string key = _objectName;
    key += ".";
    key += _name;
    _database[ key ] = this;
}


SpoofableData :: ~SpoofableData( )
{
    _pEnum = 0;
}


const SpoofableData* SpoofableData :: get( const char* objectName, const char* name )
{
    // PURPOSE: given object name and data name, return ptr to SpoofableData object
    // returns 0 if not found

    string key = objectName;
    key += ".";
    key += name;

    if( _database.find( key ) != _database.end() )
    {
        return _database[ key ];
    }
    else
    {
        return 0;
    }
}


void SpoofableData :: dump() const
{
    // PURPOSE: dumps data fields and values

    LOG_INFO( "dump: SPOOFABLE DATA %s.%s", objectName(), name() );
    LOG_INFO( "dump:     offset = %d", offset() );
    LOG_INFO( "dump:     size= %d", size() );

    switch( _type )
    {
        case INT:
            LOG_INFO( "dump:     type = INT" );
            break;

        case FLOAT:
            LOG_INFO( "dump:     type = FLOAT" );
            break;

        case ENUM:
            LOG_INFO( "dump:     type = ENUM - %s", _pEnum->enumName() );
            break;

        case SHORT:
            LOG_INFO( "dump:     type = SHORT" );
            break;

        case CHAR:
            LOG_INFO( "dump:     type = CHAR" );
            break;

        case FASTDATA_ARRAY:
	    LOG_INFO( "dump:     type = FASTDATA_ARRAY" );
	    break;

        default:
            LOG_ERROR( "dump: invalid type = %d", type() );
            cleanup( );
            exit( 1 );
    }
}


void SpoofableData :: dumpAll()
{
    // PURPOSE: dumps all spoofable data 
    
    for( databaseIterator_t i = _database.begin();
        i != _database.end(); i++ )
    {
        ( ( *i ).second )->dump( );
    }
}


void SpoofableData :: showObjectNames( )
{
    // PURPOSE: list all spoofable objects supported

    LOG_INFO("Spoofable objects supported:");

    const char* last = 0;
    SpoofableData* p;

    for( databaseIterator_t i = _database.begin();
        i != _database.end(); i++ )
    {
        p = ( *i ).second;
        if( last == 0  || strcmp( last, p->objectName() ) != 0 )
        {
            LOG_INFO( "    %s", p->objectName() );
            last = p->objectName();
        }
    }
}


void SpoofableData :: showDataNames ( const char* objectName )
{
    // PURPOSE: list all spoofable data supported for given object

    LOG_INFO("Spoofable data supported for %s:", objectName );

    SpoofableData* p;

    for( databaseIterator_t i = _database.begin();
        i != _database.end(); i++)
    {
        p = ( *i ).second;
        if( strcmp( objectName, p->objectName() ) == 0 )
        {
            LOG_INFO("    %s", p->name() );
        }
    }
}


void SpoofableData :: cleanup()
{
    // PURPOSE:  free _database memory

    LOG_DEBUG("cleanup: n=%d", _database.size() );

    for(databaseIterator_t i = _database.begin(); i != _database.end(); ++i)
    {
        LOG_DEBUG("cleanup: erasing entry for %s", i->first.c_str());

		delete i->second;
    }

	_database.clear();

    EnumInfo::cleanup( ); // also cleanup enum info
}


void SpoofableData :: validate(const char* objectName, const char* name)
{
    // PURPOSE: verifies presence of SpoofableData object in database
    // cleanup and exit on error

    string key = objectName;
    key += ".";
    key += name;

    validate( key.c_str() );
}


void SpoofableData::validate( const char* key )
{
    // PURPOSE: verifies presense of SpoofableData object in database
    // cleanup and exit on error

    if( _database.find( key ) == _database.end() )
    {
        LOG_ERROR("validate: failed, key = %s", key );
        cleanup( );
        exit( 1 );
    }

    LOG_DEBUG("validate: key %s is valid", key );
}


const string SpoofableData :: valueToString( void* pValue ) const
{
    // PURPOSE: converts actual data to string representation

    char buf[20]; // needs to hold char rep of biggest data typet

    ASSERT( pValue );

    LOG_DEBUG("valueToString: 0x%x", pValue);

    switch( type() )
    {
        case INT :
            sprintf(buf, "%d", *((int*)pValue) );
            return buf; // converts to string

        case SHORT :
        case FASTDATA_ARRAY : // only convert the 1st value in the array
            sprintf(buf, "%hd", *((short*)pValue) );
            return buf; // converts to string

        case CHAR :
            sprintf(buf, "0x%2x", *((char*)pValue) );
            return buf; // converts to string (hex value)

        case FLOAT :
            if( *(float*)pValue > 1000000. )
            {
                LOG_ERROR("valueToString: %s.%s float too big to convert %3.3f", 
                          objectName(), name(), *(float*)pValue );
                return ">1000000";
            }
            if( *(float*)pValue < -1000000. )
            {
                LOG_ERROR("valueToString: %s.%s float too big to convert %3.3f", 
                          objectName(), name(), *(float*)pValue );
                return "<-1000000";
            }
            sprintf(buf, "%3.3f", *((float*)pValue) );
            return buf;

        case ENUM :
            return _pEnum->getValueName( *(int*)pValue );
    }

    LOG_ERROR("valueToString: %s.%s invalid type %d",
              objectName(), name(), type() );
    cleanup();
    exit(1);

    return "unknown value"; // needed to avoid compiler warning
}


void SpoofableData :: stringToValue( void* pValue, const char* stringValue ) const
{
    // PURPOSE: convert string rep of data to actual data

    switch( type() )
    {
        case INT :
            sscanf( stringValue, "%d", (int*)pValue );
            break;

        case SHORT :
        case FASTDATA_ARRAY :
            sscanf( stringValue, "%hd", (short*)pValue );
            break;

        case CHAR :
	    int temp;
            sscanf( stringValue, "%x", &temp );
	    *( (char*)pValue ) = temp;
            break;

        case FLOAT :
            sscanf( stringValue, "%f", (float*)pValue );
            break;

        case ENUM :
            *( int* )pValue = _pEnum->getValue( stringValue );
            break;

        default :
            LOG_ERROR("stringToValue: %s.%s invalid type %d",
                      objectName(), name(), type() );
            cleanup();
            exit(1);
    }
}


void SpoofableData :: initdb( )
{
    // PURPOSE: intialize a database of spoofable data


    //
    // All supported enum type and value names are defined here
    //
    EnumInfo* pE;

    pE = new EnumInfo( "HW_ORDERS" );
    pE->addValue( "HW_DISABLE", HW_DISABLE );
    pE->addValue( "HW_ENABLE", HW_ENABLE );

    pE = new EnumInfo( "State_names" );
    pE->addValue( "FIRST_STATE", FIRST_STATE );
    pE->addValue( "SELF_TEST", SELF_TEST );
    pE->addValue( "STARTUP_TESTS", STARTUP_TESTS );
    pE->addValue( "DISPOSABLE_TEST", DISPOSABLE_TEST );
    pE->addValue( "AC_CONNECTED", AC_CONNECTED );
    pE->addValue( "AC_PRIME", AC_PRIME );
    pE->addValue( "DONOR_CONNECTED", DONOR_CONNECTED );
    pE->addValue( "BLOOD_PRIME", BLOOD_PRIME );
    pE->addValue( "BLOOD_RUN", BLOOD_RUN );
    pE->addValue( "BLOOD_RINSEBACK", BLOOD_RINSEBACK );
    pE->addValue( "DONOR_DISCONNECT", DONOR_DISCONNECT );
    pE->addValue( "POST_RUN", POST_RUN );
    pE->addValue( "METERED_STORAGE", METERED_STORAGE );
    pE->addValue( "METERED_STORAGE_DISCONNECT", METERED_STORAGE_DISCONNECT );
    pE->addValue( "SERVICE_MODE", SERVICE_MODE );
    pE->addValue( "DEFAULT_STATE", DEFAULT_STATE );

    // Note these may be subject to change is there anyway to check against states.h?
    pE = new EnumInfo( "Substate_names" );
    pE->addValue( "CHANNELSETUP", SS_CHANNEL_SETUP );
    pE->addValue( "PREPLATELETPLASMA", SS_PREPLATELET_PLASMA );
    pE->addValue( "PREPLATELETNOPLASMA", SS_PREPLATELET_NOPLASMA );
    pE->addValue( "PLATELETPLASMA", SS_PLATELET_PLASMA );
    pE->addValue( "PCA", SS_PCA );
    pE->addValue( "MIDRUN", SS_MIDRUN );
    pE->addValue( "PIRPLASMA", SS_PIR_PLASMA );
    pE->addValue( "PIRNOPLASMA", SS_PIR_NOPLASMA );
    pE->addValue( "EXTENDEDPCA", SS_EXTENDED_PCA );
    pE->addValue( "EXTENDEDPLASMA", SS_EXTENDED_PLASMA );
    pE->addValue( "RBCSETUP1", SS_RBC_PTF_SETUP_1 );
    pE->addValue( "RBCSETUP2", SS_RBC_PTF_SETUP_2 );
    pE->addValue( "RBCPTFPRIME", SS_RBC_PTF_PRIME );
    pE->addValue( "RBCCOLLECTION", SS_RBC );
    pE->addValue( "RPCHANNELSETUP", SS_RP_CHANNEL_SETUP ); 
    pE->addValue( "PLASMAONLY", SS_RP_PLASMA_ONLY );        
    pE->addValue( "RBCRPSETUP1", SS_RP_RBC_PTF_SETUP_1 );
    pE->addValue( "RBCRPSETUP2", SS_RP_RBC_PTF_SETUP_2 ); 
    pE->addValue( "RBCRPCOLLECTION", SS_RP_RBC ); 
    pE->addValue( "RBCSETUP1DOUBLE", SS_RP_RBC_PTF_SETUP_1_DOUBLE );  
    pE->addValue( "REBSETUP2DOUBLE", SS_RP_RBC_PTF_SETUP_2_DOUBLE ); 
    pE->addValue( "RBCCOLLECTIONDOUBLE", SS_RP_RBC_DOUBLE );
    pE->addValue( "MSSSETUP", SS_MSSSETUP );    
    pE->addValue( "MSSPFR", SS_MSSPFR );     
    pE->addValue( "RESSETUP", SS_RESSETUP );  
    pE->addValue( "RESSETUP2", SS_RESSETUP2 );               
    pE->addValue( "RESSETUP3", SS_RESSETUP3 );               
    pE->addValue( "VERIFYCLOSURE", SS_VERIFYCLOSURE );         
    pE->addValue( "VERIFYCLOSURE3", SS_VERIFYCLOSURE3 );         
    pE->addValue( "MSSCONNECT", SS_MSSCONNECT );            
    pE->addValue( "MSSPRIME", SS_MSSPRIME );   
    pE->addValue( "MSSRBCPRIME1", SS_MSSRBCPRIME1 );            
    pE->addValue( "MSSPLTPRIMESALVAGE", SS_MSSPLTPRIMESALVAGE );               
    pE->addValue( "MSSPLTPRIME1", SS_MSSPLTPRIME1 );            
    pE->addValue( "MSSPLTPRIME2", SS_MSSPLTPRIME2 );
    pE->addValue( "MSSPARALLEL", SS_MSSPARALLELPROC );
    pE->addValue( "VACPURGE", SS_MSSVACPURGE );    
    pE->addValue( "PASFINALFLOWVERIFICATION", SS_PASFINALFLOWVERIFICATION );
    pE->addValue( "PASVACPURGEFINAL", SS_MSSVACPURGEFINAL );            
    pE->addValue( "MSSPLTADDPAUSE", SS_MSSPLTADDITIONPAUSE );    
    pE->addValue( "MSSADDITION", SS_MSSADDITION );   
    pE->addValue( "MSSRBCADDITION", SS_MSSRBCADDITION ); 
    pE->addValue( "MSSPLTADDITION", SS_MSSPLTADDITION );    

	// ////////////////////////////////////////////////////////////////////////////////////
	//ProcState_names ENUM: all possible values for ProcState._StateName from states file.
	// ////////////////////////////////////////////////////////////////////////////////////
	pE = new EnumInfo("ProcState_names");
	pE->addValue( "StartProtocol", 1000 );
	pE->addValue( "PFR", 1001 );
	pE->addValue( "PFInit", 1002 );
	pE->addValue( "PFRecover", 1003 );
	pE->addValue( "PFPowerTest", 1004 );
	pE->addValue( "PowerTest", 1005 );
	pE->addValue( "SafetyPowerTest", 1006 );
	pE->addValue( "PowerOnTest", 1007 );
	pE->addValue( "CentrifugeTests", 1008 );
	pE->addValue( "SafetyPowerTest64", 1009 );

	pE->addValue( "CentShutdownTest", 1010 );
	pE->addValue( "PFBranch", 1011 );
	pE->addValue( "StartupTest", 1012 );
	pE->addValue( "CalibVerification", 1013 );
	pE->addValue( "ValvesTest", 1014 );
	pE->addValue( "LeakDetectorTest", 1015 );
	pE->addValue( "DoorLatchTest", 1016 );
	pE->addValue( "GUIStarted", 1017 );
	pE->addValue( "OperatorResponse", 1018 );
	pE->addValue( "LoadCassette", 1019 );

	pE->addValue( "OpenValves", 1020 );
	pE->addValue( "StartPumps", 1021 );
	pE->addValue( "LowerCassette", 1022 );
	pE->addValue( "CassetteID", 1023 );
	pE->addValue( "StopPumps", 1024 );
	pE->addValue( "EvacuateSetValves", 1025 );
	pE->addValue( "EvacuateBags", 1026 );
	pE->addValue( "LowerNotification", 1027 );
	pE->addValue( "UnloadCassette", 1028 );
	pE->addValue( "RaiseCassette", 1029 );

	pE->addValue( "UnlockDoor", 1030 );
	pE->addValue( "DisposableTest", 1031 );
	pE->addValue( "CloseValves", 1032 );
	pE->addValue( "CheckSampleBag", 1033 );
	pE->addValue( "PltBagEvac", 1034 );
	pE->addValue( "PltBagEvacPressureRelief", 1035 );
	pE->addValue( "PressInletLine", 1036 );
	pE->addValue( "DisposableTest1", 1037 );
	pE->addValue( "InletPressTest", 1038 );
	pE->addValue( "InletDecayTest", 1039 );

	pE->addValue( "NegativePressTest", 1040 );
	pE->addValue( "Air2ChannelPrime", 1041 );
	pE->addValue( "PlsEvacFinished", 1042 );
	pE->addValue( "Air2Channel", 1043 );
	pE->addValue( "ShowClampARM", 1044 );
	pE->addValue( "NegativePressRelief", 1045 );
	pE->addValue( "ConnectAC", 1046 );
	pE->addValue( "DisposableTest2", 1047 );
	pE->addValue( "InletPressTest2", 1048 );
	pE->addValue( "InletDecayTest2", 1049 );

	pE->addValue( "PressInletLine2", 1050 );
	pE->addValue( "CloseCrossoverClamp", 1051 );
	pE->addValue( "PressInletLine3", 1052 );
	pE->addValue( "Air2ChannelPrime2", 1053 );
	pE->addValue( "Air2Channel2", 1054 );
	//pE->addValue( "ShowClampARM", 1055 ); dupe
	pE->addValue( "ACPrime", 1056 );
	pE->addValue( "ACPrimeInlet", 1057 );
	pE->addValue( "ACPressReturnLine", 1058 );
	pE->addValue( "ConnectDonor", 1059 );

	pE->addValue( "DonorConnected", 1060 );
	pE->addValue( "DonorDisconnect", 1061 );
	pE->addValue( "DoorLockCheck", 1062 );
	pE->addValue( "BloodPrime", 1063 );
	pE->addValue( "BloodRun", 1064 );
	pE->addValue( "BloodPrimeInlet", 1065 );
	pE->addValue( "BloodPrimeReturn", 1066 );
	pE->addValue( "EvacEndSetValves", 1067 );
	pE->addValue( "EvacEndSetValvesARM", 1068 );
	pE->addValue( "EvacEndSetValvesARMBlack", 1069 );

	pE->addValue( "EvacEndResetValves", 1070 );
	pE->addValue( "PreRinseback", 1071 );
	pE->addValue( "PrimeChannel", 1072 );
	pE->addValue( "PrimeChannel2A", 1073 );
	pE->addValue( "PrimeChannel2", 1074 );
	pE->addValue( "PrimeChannel3", 1075 );
	pE->addValue( "PrimeChannel4", 1076 );
	pE->addValue( "PrimeVent", 1077 );
	pE->addValue( "RampCentrifuge", 1078 );
	pE->addValue( "PrimeAirout1", 1079 );

	pE->addValue( "PrimeAirout2", 1080 );
	pE->addValue( "Collect", 1081 );
	pE->addValue( "NormalRinseback", 1082 );
	pE->addValue( "PlasmaRinseback", 1083 );
	pE->addValue( "SalineRinseback", 1084 );
	pE->addValue( "RinsebackLower", 1085 );
	pE->addValue( "RinsebackRecirculate", 1086 );
	pE->addValue( "RinsebackReservoir", 1087 );
	pE->addValue( "Rinseback", 1088 );
	pE->addValue( "DisconnectPrompt", 1089 );

	pE->addValue( "PlsRinsebackSpillover", 1090 );
	pE->addValue( "PlsRinsebackNo1", 1091 );
	pE->addValue( "PlsRinsebackRinse", 1092 );
	pE->addValue( "PlsRinsebackRecirculate", 1093 );
	pE->addValue( "PlsRinsebackNo2", 1094 );
	pE->addValue( "PlsRinsebackPurge", 1095 );
	pE->addValue( "PlsRinsebackEvacuate", 1096 );
	pE->addValue( "SalCheckConnect", 1097 );
	pE->addValue( "SalRecirc1", 1098 );
	pE->addValue( "SalEvac1", 1099 );

	pE->addValue( "SalineDraw", 1090 );
	pE->addValue( "SalRecirc2", 1091 );
	pE->addValue( "SalEvac2", 1092 );
	pE->addValue( "SalFlush", 1093 );
	pE->addValue( "PostRun", 1094 );
	pE->addValue( "MeteredStorage", 1095 );
	pE->addValue( "MSSPFR", 1096 );
	pE->addValue( "VerifyClosure", 1097 );
	pE->addValue( "MSSConnect", 1098 );
	pE->addValue( "MSSPrime", 1099 );

	pE->addValue( "MSSParallelProcessing", 1100 );
	pE->addValue( "MSSDisconnect", 1101 );
	pE->addValue( "ResSetup", 1102 );
	pE->addValue( "VerifyClosure3", 1103 );
	pE->addValue( "ResSetup2", 1104 );
	pE->addValue( "MSSRBCPrimeSalvage", 1105 );
	pE->addValue( "MSSRBCPrime1", 1106 );
	pE->addValue( "ResSetup3", 1107 );
	pE->addValue( "MSSPLTPrimeSalvage", 1108 );
	pE->addValue( "MSSPLTPrime1", 1109 );

	pE->addValue( "MSSPLTPrime2", 1110 );
	pE->addValue( "PostRunDisplay", 1111 );
	pE->addValue( "MSSPLT", 1112 );
	pE->addValue( "VacPurge", 1113 );
	pE->addValue( "PasFinalFlowVerification", 1114 );
	pE->addValue( "VacPurgeFinal", 1115 );
	pE->addValue( "MSSPltAdditionPause", 1116 );
	pE->addValue( "MSSPLTAddition", 1117 );
	pE->addValue( "MSSAddition", 1118 );
	pE->addValue( "MSSRBCAddition", 1119 );

	pE->addValue( "PlateletConcentrationAdjustment", 1120 );


    pE = new EnumInfo( "HW_SWITCH_STATES" );
    pE->addValue( "HW_SWITCH_DISABLED", HW_SWITCH_DISABLED );
    pE->addValue( "HW_SWITCH_ENABLED", HW_SWITCH_ENABLED );
    
    pE = new EnumInfo("HW_DOOR_STATES");
    pE->addValue( "HW_DOOR_OPEN", HW_DOOR_OPEN );
    pE->addValue( "HW_DOOR_LOCKED_STATE", HW_DOOR_LOCKED_STATE );
    pE->addValue( "HW_DOOR_CLOSED_STATE", HW_DOOR_CLOSED_STATE );
    pE->addValue( "HW_DOOR_CLOSED_AND_LOCKED", HW_DOOR_CLOSED_AND_LOCKED );
    
    pE = new EnumInfo("HW_VALVE_STATES");
    pE->addValue( "HW_VALVE_UNKNOWN", HW_VALVE_UNKNOWN );
    pE->addValue( "HW_VALVE_COLLECT", HW_VALVE_COLLECT );
    pE->addValue( "HW_VALVE_OPEN", HW_VALVE_OPEN );
    pE->addValue( "HW_VALVE_RETURN", HW_VALVE_RETURN );

    pE = new EnumInfo("HW_CASSETTE_STATES");
    pE->addValue("HW_CASSETTE_UNKNOWN", HW_CASSETTE_UNKNOWN);
    pE->addValue("HW_CASSETTE_DOWN", HW_CASSETTE_DOWN);
    pE->addValue("HW_CASSETTE_UP", HW_CASSETTE_UP);


#ifdef SUPPORT_CONTROL_SPOOFS
    //
    // control only enums
    //
    pE = new EnumInfo( "CHW_CASSETTE_ORDERS" );
    pE->addValue( "CHW_CASSETTE_DONTMOVE", CHW_CASSETTE_DONTMOVE );
    pE->addValue( "CHW_CASSETTE_DOWN", CHW_CASSETTE_DOWN );
    pE->addValue( "CHW_CASSETTE_UP", CHW_CASSETTE_UP );

    pE = new EnumInfo( "CHW_VALVE_ORDERS" );
    pE->addValue( "CHW_VALVE_COLLECT", CHW_VALVE_COLLECT );
    pE->addValue( "CHW_VALVE_OPEN", CHW_VALVE_OPEN );
    pE->addValue( "CHW_VALVE_RETURN", CHW_VALVE_RETURN );

    pE = new EnumInfo( "CHW_DOOR_ORDERS" );
    pE->addValue( "CHW_DOOR_UNLOCK", CHW_DOOR_UNLOCK );
    pE->addValue( "CHW_DOOR_LOCK", CHW_DOOR_LOCK );

    pE = new EnumInfo( "CHW_AC_DETECTOR_STATES" );
    pE->addValue( "CHW_AC_DETECTOR_FLUID", CHW_AC_DETECTOR_FLUID );
    pE->addValue( "CHW_AC_DETECTOR_AIR", CHW_AC_DETECTOR_AIR );

    pE = new EnumInfo( "CHW_RESERVOIR_STATES" );
    pE->addValue( "CHW_RESERVOIR_ERROR", CHW_RESERVOIR_ERROR );
    pE->addValue( "CHW_RESERVOIR_EMPTY", CHW_RESERVOIR_EMPTY );
    pE->addValue( "CHW_RESERVOIR_MIDDLE", CHW_RESERVOIR_MIDDLE );
    pE->addValue( "CHW_RESERVOIR_HIGH", CHW_RESERVOIR_HIGH );

    pE = new EnumInfo( "CHW_EVENTS" );
    pE->addValue( "CHW_NORMAL_UPDATE", CHW_NORMAL_UPDATE );
    pE->addValue( "CHW_AC_EVENT", CHW_AC_EVENT );
    pE->addValue( "CHW_DOOR_EVENT", CHW_DOOR_EVENT );
    pE->addValue( "CHW_PAUSE_EVENT", CHW_PAUSE_EVENT );
    pE->addValue( "CHW_STOP_EVENT", CHW_STOP_EVENT );
    pE->addValue( "CHW_RESERVOIR_EVENT", CHW_RESERVOIR_EVENT );

    pE = new EnumInfo("VolTargetDetail");
    pE->addValue( "NOCOLLECT", VTarget::NOCOLLECT );
    pE->addValue( "VIN", VTarget::VIN );
    pE->addValue( "VBP", VTarget::VBP );
    pE->addValue( "VPBAG", VTarget::VPBAG );
    pE->addValue( "VRBCBAG", VTarget::VRBCBAG );
    pE->addValue( "VCOLBAG", VTarget::VCOLBAG );

    pE = new EnumInfo( "HAL_VALVESTATE" );
    pE->addValue( "HAL_VALVE_UNKNOWN", HAL_VALVE_UNKNOWN );
    pE->addValue( "HAL_VALVE_COLLECT", HAL_VALVE_COLLECT );
    pE->addValue( "HAL_VALVE_OPEN", HAL_VALVE_OPEN );
    pE->addValue( "HAL_VALVE_RETURN", HAL_VALVE_RETURN );

    pE = new EnumInfo( "HAL_DOORSTATE" );
    pE->addValue( "HAL_DOOR_OPEN", HAL_DOOR_OPEN );
    pE->addValue( "HAL_DOOR_LOCKED_STATE", HAL_DOOR_LOCKED_STATE );
    pE->addValue( "HAL_DOOR_CLOSED_STATE", HAL_DOOR_CLOSED_STATE );
    pE->addValue( "HAL_DOOR_CLOSED_AND_LOCKED", HAL_DOOR_CLOSED_AND_LOCKED );

    pE = new EnumInfo( "HW_ERROR" );
    pE->addValue( "HW_NO_FAULT", HW_NO_FAULT );
    pE->addValue( "HW_FAULT", HW_FAULT );

    pE = new EnumInfo( "CHW_PS_OT_ERRORS" );
    pE->addValue( "CHW_PS_NO_FAULT", CHW_PS_NO_FAULT );
    pE->addValue( "CHW_DC_NOT_OK", CHW_DC_NOT_OK );
    pE->addValue( "CHW_POWER_FAIL", CHW_POWER_FAIL );
    pE->addValue( "CHW_PS_OTW", CHW_PS_OTW );
    pE->addValue( "CHW_SS_OTW", CHW_SS_OTW );

    pE = new EnumInfo( "CHW_PUMP_ERRORS" );
    pE->addValue( "CHW_PUMP_NO_FAULT", HW_NO_FAULT );
    pE->addValue( "CHW_PUMP_FAIL_TO_MAINTAIN_SPEED", CHW_PUMP_FAIL_TO_MAINTAIN_SPEED ); // 1
    pE->addValue( "CHW_PUMP_INTEGRAL_ERROR", CHW_PUMP_INTEGRAL_ERROR );     // 2
    pE->addValue( "CHW_PUMP_VOLUME_ERROR", CHW_PUMP_VOLUME_ERROR );         //4
    pE->addValue( "CHW_MOTOR_FAULT", CHW_MOTOR_FAULT );                     //8

// rinseback status for Next Gen Spoofer
    pE = new EnumInfo( "RINSEBACK_OUTCOMES" );
    pE->addValue( "RINSEBACK_UNSUCCESSFUL", RINSEBACK_UNSUCCESSFUL );
    pE->addValue( "RINSEBACK_SUCCESSFUL_NORMAL", RINSEBACK_SUCCESSFUL_NORMAL);
    pE->addValue( "RINSEBACK_SUCCESSFUL_PLASMA", RINSEBACK_SUCCESSFUL_PLASMA );
    pE->addValue( "RINSEBACK_SUCCESSFUL_SALINE", RINSEBACK_SUCCESSFUL_SALINE );

// for testing STS
    pE = new EnumInfo("HW_CENT_ERRORS" );
    pE->addValue( "HW_CENT_NO_FAULT", 0 );
    pE->addValue( "HW_CENT_HARDWARE_ERROR", 1 );
    pE->addValue( "HW_CENT_INTEGRAL_ERROR", 2 );
	pE->addValue( "HW_CENT_REVERSE_BIT", 4 );

    pE = new EnumInfo( "CHW_SEAL_SAFE_OVER_TEMP_WARN" );
    pE->addValue( "CHW_SEAL_SAFE_OTW_NOT_AVAILABLE", CHW_SEAL_SAFE_OTW_NOT_AVAILABLE );
    pE->addValue( "CHW_SEAL_SAFE_OTW", CHW_SEAL_SAFE_OTW );
    pE->addValue( "CHW_SEAL_SAFE_TEMP_OK", CHW_SEAL_SAFE_TEMP_OK );
#endif // SUPPORT_CONTROL_SPOOFS


#ifdef SUPPORT_SAFETY_SPOOFS
    //
    // safety only enums
    //
    pE = new EnumInfo("SHW_DIRECTION");
    pE->addValue("SHW_RETURN", SHW_RETURN);
    pE->addValue("SHW_DRAW", SHW_DRAW);

    pE = new EnumInfo("SHW_RESERVOIR_STATES");
    pE->addValue("SHW_RESERVOIR_EMPTY", SHW_RESERVOIR_EMPTY);
    pE->addValue("SHW_RESERVOIR_LOW", SHW_RESERVOIR_LOW);

#endif // SUPPORT_SAFETY_SPOOFS

    
    //
    // All supported spoofable data is defined below...
    //

#ifdef SUPPORT_CONTROL_SPOOFS
    //
    // Control Orders
    //
    CHwOrders co;
    
    new SpoofableData( "CHwOrders", "cassettePosition", 
        &co, &co.cassettePosition, ENUM, "CHW_CASSETTE_ORDERS" );

    new SpoofableData( "CHwOrders", "centrifugeRPM", 
        &co, &co.centrifugeRPM, FLOAT );

    new SpoofableData( "CHwOrders", "inletRPM", 
        &co, &co.inletRPM, FLOAT );

    new SpoofableData( "CHwOrders", "plateletRPM", 
        &co, &co.plateletRPM, FLOAT );

    new SpoofableData( "CHwOrders", "plasmaRPM", 
        &co, &co.plasmaRPM, FLOAT );

    new SpoofableData( "CHwOrders", "acRPM", 
        &co, &co.acRPM, FLOAT );

    new SpoofableData( "CHwOrders", "returnRPM", 
        &co, &co.returnRPM, FLOAT );

    new SpoofableData( "CHwOrders", "plasmaValve", 
        &co, &co.plasmaValve, ENUM, "CHW_VALVE_ORDERS" );

    new SpoofableData( "CHwOrders", "plateletValve", 
        &co, &co.plateletValve, ENUM, "CHW_VALVE_ORDERS" );

    new SpoofableData( "CHwOrders", "rbcValve", 
        &co, &co.rbcValve, ENUM, "CHW_VALVE_ORDERS" );

    new SpoofableData( "CHwOrders", "doorLatches", 
        &co, &co.doorLatches, ENUM, "HW_ORDERS" );

    new SpoofableData( "CHwOrders", "doorDirection", 
        &co, &co.doorDirection, ENUM, "CHW_DOOR_ORDERS" );

    new SpoofableData( "CHwOrders", "centrifugeRampUpRate",
        &co, &co.centrifugeRampUpRate, FLOAT );

    new SpoofableData( "CHwOrders", "centrifugeRampDownRate",
        &co, &co.centrifugeRampUpRate, FLOAT );

    //
    // Control Status
    //
    CHwStates cs;

    new SpoofableData( "CHwStates", "returnRPM", 
        &cs, &cs.returnPump.RPM, FLOAT );

    new SpoofableData( "CHwStates", "acRPM", 
        &cs, &cs.acPump.RPM, FLOAT );

    new SpoofableData( "CHwStates", "inletRPM", 
        &cs, &cs.inletPump.RPM, FLOAT );

    new SpoofableData( "CHwStates", "plasmaRPM", 
        &cs, &cs.plasmaPump.RPM, FLOAT );

    new SpoofableData( "CHwStates", "plateletRPM", 
        &cs, &cs.plateletPump.RPM, FLOAT );

    new SpoofableData( "CHwStates", "returnRevs", 
        &cs, &cs.returnPump.Revs, FLOAT );

    new SpoofableData( "CHwStates", "acRevs", 
        &cs, &cs.acPump.Revs, FLOAT );

    new SpoofableData( "CHwStates", "inletRevs", 
        &cs, &cs.inletPump.Revs, FLOAT );

    new SpoofableData( "CHwStates", "plasmaRevs", 
        &cs, &cs.plasmaPump.Revs, FLOAT );

    new SpoofableData( "CHwStates", "plateletRevs", 
        &cs, &cs.plateletPump.Revs, FLOAT );

    new SpoofableData( "CHwStates", "acDetector", 
        &cs, &cs.acDetector, ENUM, "CHW_AC_DETECTOR_STATES" );

    new SpoofableData( "CHwStates", "reservoir", 
        &cs, &cs.reservoir, ENUM, "CHW_RESERVOIR_STATES" );

    new SpoofableData( "CHwStates", "event", 
        &cs, &cs.event, ENUM, "CHW_EVENTS" );

    new SpoofableData( "CHwStates", "accessPressure", 
        &cs, &cs.accessPressure, SHORT );

    new SpoofableData( "CHwStates", "red", 
        &cs, &cs.red, SHORT );

    new SpoofableData( "CHwStates", "green", 
        &cs, &cs.green, SHORT );

    new SpoofableData( "CHwStates", "inletPumpAccum", 
        &cs, &cs.inletPump.Accum, INT );
    
    new SpoofableData( "CHwStates", "leakDetector", 
        &cs, &cs.leakDetector, SHORT );

    new SpoofableData( "CHwStates", "sixtyFourVoltSupply", 
        &cs, &cs.sixtyFourVoltSupply, SHORT );

    new SpoofableData( "CHwStates", "twelveVoltSupply", 
        &cs, &cs.twelveVoltSupply, SHORT );

    new SpoofableData( "CHwStates", "twentyFourVoltSupply", 
        &cs, &cs.twentyFourVoltSupply, SHORT );

    new SpoofableData( "CHwStates", "minusTwelvePS", 
        &cs, &cs.minusTwelvePS, SHORT );

    new SpoofableData( "CHwStates", "stopSwitch", 
        &cs, &cs.stopSwitch, ENUM, "HW_SWITCH_STATES" );

    new SpoofableData( "CHwStates", "pauseSwitch", 
        &cs, &cs.pauseSwitch, ENUM, "HW_SWITCH_STATES" );
    
    new SpoofableData( "CHwStates", "doorLocks", 
        &cs, &cs.doorLocks, ENUM, "HW_DOOR_STATES" );

    new SpoofableData( "CHwStates", "centrifugeRPM", 
	&cs, &cs.centrifugeRPM, FLOAT );

    new SpoofableData( "CHwStates", "returnPumpAccum", 
        &cs, &cs.returnPump.Accum, INT );

    new SpoofableData( "CHwStates", "acPumpAccum", 
        &cs, &cs.acPump.Accum, INT );

    new SpoofableData( "CHwStates", "plasmaPumpAccum", 
        &cs, &cs.plasmaPump.Accum, INT );

    new SpoofableData( "CHwStates", "plateletPumpAccum", 
        &cs, &cs.plateletPump.Accum, INT );
    
    new SpoofableData( "CHwStates", "plasmaValve", 
        &cs, &cs.plasmaValve, ENUM, "HW_VALVE_STATES" );

    new SpoofableData( "CHwStates", "plateletValve", 
        &cs, &cs.plateletValve, ENUM, "HW_VALVE_STATES" );
    
    new SpoofableData( "CHwStates", "rbcValve", 
        &cs, &cs.rbcValve, ENUM, "HW_VALVE_STATES" );
    
    new SpoofableData( "CHwStates", "sixtyFourVoltSwitched", 
        &cs, &cs.sixtyFourVoltSwitched, SHORT );

    new SpoofableData( "CHwStates", "twentyFourVoltSwitched", 
        &cs, &cs.twentyFourVoltSwitched, SHORT );

    new SpoofableData( "CHwStates", "cassettePosition", 
        &cs, &cs.cassettePosition, ENUM, "HW_CASSETTE_STATES" );
    
    new SpoofableData( "CHwStates", "centrifugeCurrent", 
        &cs, &cs.centrifugeCurrent, SHORT );

    new SpoofableData( "CHwStates", "twentyFourVoltCurrent", 
        &cs, &cs.twentyFourVoltCurrent, SHORT );

    new SpoofableData( "CHwStates", "fiveVoltSupply", 
        &cs, &cs.fiveVoltSupply, SHORT );

    new SpoofableData( "CHwStates", "emiTemperature", 
        &cs, &cs.emiTemperature, SHORT );

    new SpoofableData( "CHwStates", "lowAGC", 
        &cs, &cs.lowAGC, SHORT );

    new SpoofableData( "CHwStates", "highAGC", 
        &cs, &cs.highAGC, SHORT );
    
    new SpoofableData( "CHwStates", "centrifugePressure", 
        &cs, &cs.centrifugePressure, SHORT );

    new SpoofableData( "CHwStates", "centrifugeError",
        &cs, &cs.centrifugeError, ENUM, "HW_CENT_ERRORS" );   // for STS Testing

    new SpoofableData( "CHwStates", "valveFault",
        &cs, &cs.valveFault, ENUM, "HW_ERROR" );  //for STS testing

    new SpoofableData( "CHwStates", "solenoidFault",
         &cs, &cs.solenoidFault, ENUM, "HW_ERROR" );  //for STS testing

    new SpoofableData( "CHwStates", "rbcCommError",
         &cs, &cs.rbcCommError, ENUM, "HW_ERROR" );  //for STS testing

    new SpoofableData( "CHwStates", "fanSense0", 
        &cs, &cs.fanSense0, ENUM, "HW_ERROR" );

    new SpoofableData( "CHwStates", "fanSense1", 
        &cs, &cs.fanSense1, ENUM, "HW_ERROR" );

    new SpoofableData( "CHwStates", "fanSense2", 
        &cs, &cs.fanSense2, ENUM, "HW_ERROR" );

    new SpoofableData( "CHwStates", "tempError", 
        &cs, &cs.tempError, ENUM, "HW_ERROR" );

    new SpoofableData( "CHwStates", "psTempWarning", 
        &cs, &cs.psTempWarning, ENUM, "CHW_PS_OT_ERRORS" );

    new SpoofableData( "CHwStates", "motorFault", 
        &cs, &cs.motorFault, ENUM, "HW_ERROR" );

    
    new SpoofableData( "CHwStates", "acPump_Errors",
                       &cs, &cs.acPump.errors, ENUM, "CHW_PUMP_ERRORS" );
    new SpoofableData( "CHwStates", "returnPump_Errors",
                       &cs, &cs.returnPump.errors, ENUM, "CHW_PUMP_ERRORS" );
    new SpoofableData( "CHwStates", "inletPump_Errors",
                       &cs, &cs.inletPump.errors, ENUM, "CHW_PUMP_ERRORS" );
    new SpoofableData( "CHwStates", "plateletPump_Errors",
                       &cs, &cs.plateletPump.errors, ENUM, "CHW_PUMP_ERRORS" );
    new SpoofableData( "CHwStates", "plasmaPump_Errors",
                       &cs, &cs.plasmaPump.errors, ENUM, "CHW_PUMP_ERRORS" );                                         

    new SpoofableData( "CHwStates", "sealSafeOvertemp", 
        &cs, &cs.sealSafeOvertemp, ENUM, "CHW_SEAL_SAFE_OVER_TEMP_WARN" );

	//ProcState ps;
	//new SpoofableData( "ProcStateMsg", "_StateName",
		//&ps, &ps._StateName, char[ NAME_SIZE + 1 ] );



    //
    // Int Messages
    //
    new SpoofableData( "ProcStateMsg" );
    new SpoofableData( "CassettePositionStatusMsg" );
    new SpoofableData( "CentrifugePressureStatusMsg" );
    new SpoofableData( "CentrifugeStopMsg" );
    new SpoofableData( "ConfigChangedMsg" );
    new SpoofableData( "DateTimeSetMsg" );
    new SpoofableData( "DeleteProductMsg" );
    new SpoofableData( "DisconnectCompleteMsg" );
    new SpoofableData( "DisposableIDMsg" );
    new SpoofableData( "DisposableLoweredMsg" );
    new SpoofableData( "DisposableRaisedMsg" );
    new SpoofableData( "DonorConnectedMsg" );
    new SpoofableData( "DonorDataStatusMsg" );
    new SpoofableData( "DonorDisconnectedMsg" );
    new SpoofableData( "DoPredictionMsg" );
    new SpoofableData( "EnableFastDataMsg" );
    new SpoofableData( "EnableLoadSystemMsg" );
    new SpoofableData( "EndProcedureMsg" );
    new SpoofableData( "FinishedViewingStatsMsg" );
    new SpoofableData( "GUIRequestBeginRunMsg" );   
    new SpoofableData( "GUIRequestRaiseCassetteMsg" );
    new SpoofableData( "GUIRequestStartDrawMsg" ); 
    new SpoofableData( "GUISysDonorStateMsg" );
    new SpoofableData( "GuiToProcDonorTimeMsg" );   
    new SpoofableData( "GuiToProcPredictRequestMsg" );  
    new SpoofableData( "InitializeInternalStatesMsg" );
    new SpoofableData( "ProcSubstateChangeMsg" );
    new SpoofableData( "RequestSafetyPowerMsg" );
    new SpoofableData( "SystemStateChangeMsg" );
	new SpoofableData( "SafetyDisconnectStatMsg" );

    // New Messages
    new SpoofableData( "ACConnectedMsg" );
    new SpoofableData( "AirInPlasmaLineMsg" );
    new SpoofableData( "AlarmResponseMsg" );
    new SpoofableData( "ButtonPressedIDMsg" );      // to anyone no just proc
    new SpoofableData( "CancelProcedureMsg" );
    new SpoofableData( "CancelMssMsg" );
    new SpoofableData( "CassetteFunctionBitsMsg" );
    new SpoofableData( "ClampedForTestingMsg" );
    new SpoofableData( "ConfirmDisposableLoadedMsg" );
    new SpoofableData( "CrossoverClampedMsg" );
    new SpoofableData( "MSSAckCompletionMsg" ); 
    new SpoofableData( "MSSConfirmUnloadMsg" );
    new SpoofableData( "MSSConnectedMsg" );
    new SpoofableData( "MSSEndRunMsg" );
    new SpoofableData( "MSSSetupMsg" );
    new SpoofableData( "MSSWriteBagVolPLT" );
    new SpoofableData( "MSSWriteBagVolRBC" );
    new SpoofableData( "OperatorSetIdentificationMsg" );
    new SpoofableData( "PartialCancellationMsg" );
    new SpoofableData( "RasConnectedPasToBeConnected" );
    new SpoofableData( "RinsebackRequestMsg" );
    new SpoofableData( "SpilloverRecoveryMsg" );
    new SpoofableData( "SystemStateChangeRequestMsg");
    new SpoofableData( "RequestEndRunStatsMsg");
 
    //
    // SubstateInfo CDS data
    //
    Predict_CDS::substate_info_t si;

    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_1",
        &si, &si[ 1 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_2",
        &si, &si[ 2 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_3",
        &si, &si[ 3 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_4",
        &si, &si[ 4 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_5",
        &si, &si[ 5 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_6",
        &si, &si[ 6 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_7",
        &si, &si[ 7 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_8",
        &si, &si[ 8 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_9",
        &si, &si[ 9 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_10",
        &si, &si[ 10 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_11",
        &si, &si[ 11 ].Qac, FLOAT );
    // with PTF/RAS there can be 15 substates on a white stamp set
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_12",
        &si, &si[ 12 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_13",
        &si, &si[ 13 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_14",
        &si, &si[ 14 ].Qac, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qac_15",
        &si, &si[ 15 ].Qac, FLOAT );

    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_1",
        &si, &si[ 1 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_1",
        &si, &si[ 1 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_2",
        &si, &si[ 2 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_2",
        &si, &si[ 2 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_3",
        &si, &si[ 3 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_3",
        &si, &si[ 3 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_4",
        &si, &si[ 4 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_4",
        &si, &si[ 4 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_5",
        &si, &si[ 5 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_5",
        &si, &si[ 5 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_6",
        &si, &si[ 6 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_6",
        &si, &si[ 6 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_7",
        &si, &si[ 7 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_7",
        &si, &si[ 7 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_8",
        &si, &si[ 8 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_8",
        &si, &si[ 8 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_9",
        &si, &si[ 9 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_9",
        &si, &si[ 9 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_10",
        &si, &si[ 10 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_10",
        &si, &si[ 10 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_11",
        &si, &si[ 11 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_11",
        &si, &si[ 11 ].QinR, FLOAT );
    // with PTF/RAS there can be 15 substates on a white stamp set
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_12",
        &si, &si[ 12 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_12",
        &si, &si[ 12 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_13",
        &si, &si[ 13 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_13",
        &si, &si[ 13 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_14",
        &si, &si[ 14 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_14",
        &si, &si[ 14 ].QinR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinD_15",
        &si, &si[ 15 ].QinD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QinR_15",
        &si, &si[ 15 ].QinR, FLOAT );

    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_1",
        &si, &si[ 1 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_2",
        &si, &si[ 2 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_3",
        &si, &si[ 3 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_4",
        &si, &si[ 4 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_5",
        &si, &si[ 5 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_6",
        &si, &si[ 6 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_7",
        &si, &si[ 7 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_8",
        &si, &si[ 8 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_9",
        &si, &si[ 9 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_10",
        &si, &si[ 10 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_11",
        &si, &si[ 11 ].Qrp, FLOAT );
    // with PTF/RAS there can be 15 substates on a white stamp set
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_12",
        &si, &si[ 12 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_13",
        &si, &si[ 13 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_14",
        &si, &si[ 14 ].Qrp, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Qrp_15",
        &si, &si[ 15 ].Qrp, FLOAT );
 
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_1",
        &si, &si[ 1 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_1",
        &si, &si[ 1 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_2",
        &si, &si[ 2 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_2",
        &si, &si[ 2 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_3",
        &si, &si[ 3 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_3",
        &si, &si[ 3 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_4",
        &si, &si[ 4 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_4",
        &si, &si[ 4 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_5",
        &si, &si[ 5 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_5",
        &si, &si[ 5 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_6",
        &si, &si[ 6 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_6",
        &si, &si[ 6 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_7",
        &si, &si[ 7 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_7",
        &si, &si[ 7 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_8",
        &si, &si[ 8 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_8",
        &si, &si[ 8 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_9",
        &si, &si[ 9 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_9",
        &si, &si[ 9 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_10",
        &si, &si[ 10 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_10",
        &si, &si[ 10 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_11",
        &si, &si[ 11 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_11",
        &si, &si[ 11 ].QpR, FLOAT );
    // with PTF/RAS there can be 15 substates on a white stamp set
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_12",
        &si, &si[ 12 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_12",
        &si, &si[ 12 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_13",
        &si, &si[ 13 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_13",
        &si, &si[ 13 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_14",
        &si, &si[ 14 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_14",
        &si, &si[ 14 ].QpR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpD_15",
        &si, &si[ 15 ].QpD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QpR_15",
        &si, &si[ 15 ].QpR, FLOAT );


    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_1",
        &si, &si[ 1 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_1",
        &si, &si[ 1 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_2",
        &si, &si[ 2 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_2",
        &si, &si[ 2 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_3",
        &si, &si[ 3 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_3",
        &si, &si[ 3 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_4",
        &si, &si[ 4 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_4",
        &si, &si[ 4 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_5",
        &si, &si[ 5 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_5",
        &si, &si[ 5 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_6",
        &si, &si[ 6 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_6",
        &si, &si[ 6 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_7",
        &si, &si[ 7 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_7",
        &si, &si[ 7 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_8",
        &si, &si[ 8 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_8",
        &si, &si[ 8 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_9",
        &si, &si[ 9 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_9",
        &si, &si[ 9 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_10",
        &si, &si[ 10 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_10",
        &si, &si[ 10 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_11",
        &si, &si[ 11 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_11",
        &si, &si[ 11 ].QcR, FLOAT );
    // with PTF/RAS there can be 15 substates on a white stamp set
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_12",
        &si, &si[ 12 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_12",
        &si, &si[ 12 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_13",
        &si, &si[ 13 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_13",
        &si, &si[ 13 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_14",
        &si, &si[ 14 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_14",
        &si, &si[ 14 ].QcR, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcD_15",
        &si, &si[ 15 ].QcD, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "QcR_15",
        &si, &si[ 15 ].QcR, FLOAT );

    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_1",
        &si, &si[ 1 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_2",
        &si, &si[ 2 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_3",
        &si, &si[ 3 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_4",
        &si, &si[ 4 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_5",
        &si, &si[ 5 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_6",
        &si, &si[ 6 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_7",
        &si, &si[ 7 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_8",
        &si, &si[ 8 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_9",
        &si, &si[ 9 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_10",
        &si, &si[ 10 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_11",
        &si, &si[ 11 ].Vtarget, FLOAT );
    // with PTF/RAS there can be 15 substates on a white stamp set
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_12",
        &si, &si[ 12 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_13",
        &si, &si[ 13 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_14",
        &si, &si[ 14 ].Vtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "Vtarget_15",
        &si, &si[ 15 ].Vtarget, FLOAT );

    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_1",
        &si, &si[ 1 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_2",
        &si, &si[ 2 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_3",
        &si, &si[ 3 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_4",
        &si, &si[ 4 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_5",
        &si, &si[ 5 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_6",
        &si, &si[ 6 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_7",
        &si, &si[ 7 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_8",
        &si, &si[ 8 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_9",
        &si, &si[ 9 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_10",
        &si, &si[ 10 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_11",
        &si, &si[ 11 ].OrVtarget, FLOAT );
    // with PTF/RAS there can be 15 substates on a white stamp set
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_12",
        &si, &si[ 12 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_13",
        &si, &si[ 13 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_14",
        &si, &si[ 14 ].OrVtarget, FLOAT );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtarget_15",
        &si, &si[ 15 ].OrVtarget, FLOAT );

    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_1",
        &si, &si[ 1 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_2",
        &si, &si[ 2 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_3",
        &si, &si[ 3 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_4",
        &si, &si[ 4 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_5",
        &si, &si[ 5 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_6",
        &si, &si[ 6 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_7",
        &si, &si[ 7 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_8",
        &si, &si[ 8 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_9",
        &si, &si[ 9 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_10",
        &si, &si[ 10 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_11",
        &si, &si[ 11 ].VtargetType, ENUM, "VolTargetDetail" );
    // with PTF/RAS there can be 15 substates on a white stamp set
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_12",
        &si, &si[ 12 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_13",
        &si, &si[ 13 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_14",
        &si, &si[ 14 ].VtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "VtargetType_15",
        &si, &si[ 15 ].VtargetType, ENUM, "VolTargetDetail" );

    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_1",
        &si, &si[ 1 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_2",
        &si, &si[ 2 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_3",
        &si, &si[ 3 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_4",
        &si, &si[ 4 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_5",
        &si, &si[ 5 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_6",
        &si, &si[ 6 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_7",
        &si, &si[ 7 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_8",
        &si, &si[ 8 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_9",
        &si, &si[ 9 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_10",
        &si, &si[ 10 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_11",
        &si, &si[ 11 ].OrVtargetType, ENUM, "VolTargetDetail" );
    // with PTF/RAS there can be 15 substates on a white stamp set
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_12",
        &si, &si[ 12 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_13",
        &si, &si[ 13 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_14",
        &si, &si[ 14 ].OrVtargetType, ENUM, "VolTargetDetail" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "OrVtargetType_15",
        &si, &si[ 15 ].OrVtargetType, ENUM, "VolTargetDetail" );

    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_1",
        &si, &si[ 1 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_2",
        &si, &si[ 2 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_3",
        &si, &si[ 3 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_4",
        &si, &si[ 4 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_5",
        &si, &si[ 5 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_6",
        &si, &si[ 6 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_7",
        &si, &si[ 7 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_8",
        &si, &si[ 8 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_9",
        &si, &si[ 9 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_10",
        &si, &si[ 10 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_11",
        &si, &si[ 11 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_12",
        &si, &si[ 12 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_13",
        &si, &si[ 13 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_14",
        &si, &si[ 14 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlasmaValve_15",
        &si, &si[ 15 ].PlasmaValve, ENUM, "HAL_VALVESTATE" );

    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_1",
        &si, &si[ 1 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_2",
        &si, &si[ 2 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_3",
        &si, &si[ 3 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_4",
        &si, &si[ 4 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_5",
        &si, &si[ 5 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_6",
        &si, &si[ 6 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_7",
        &si, &si[ 7 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_8",
        &si, &si[ 8 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_9",
        &si, &si[ 9 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_10",
        &si, &si[ 10 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_11",
        &si, &si[ 11 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_12",
        &si, &si[ 12 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_13",
        &si, &si[ 13 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_14",
        &si, &si[ 14 ].PlateletValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "PlateletValve_15",
        &si, &si[ 15 ].PlateletValve, ENUM, "HAL_VALVESTATE" );

    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_1",
        &si, &si[ 1 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_2",
        &si, &si[ 2 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_3",
        &si, &si[ 3 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_4",
        &si, &si[ 4 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_5",
        &si, &si[ 5 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_6",
        &si, &si[ 6 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_7",
        &si, &si[ 7 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_8",
        &si, &si[ 8 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_9",
        &si, &si[ 9 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_10",
        &si, &si[ 10 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_11",
        &si, &si[ 11 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_12",
        &si, &si[ 12 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_13",
        &si, &si[ 13 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_14",
        &si, &si[ 14 ].RBCValve, ENUM, "HAL_VALVESTATE" );
    new SpoofableData( CDS, "Predict_SubstateInfo", "RBCValve_15",
        &si, &si[ 15 ].RBCValve, ENUM, "HAL_VALVESTATE" );
	
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_1",
        &si, &si[ 1 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_2",
        &si, &si[ 2 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_3",
        &si, &si[ 3 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_4",
        &si, &si[ 4 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_5",
        &si, &si[ 5 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_6",
        &si, &si[ 6 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_7",
        &si, &si[ 7 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_8",
        &si, &si[ 8 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_9",
        &si, &si[ 9 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_10",
        &si, &si[ 10 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_11",
        &si, &si[ 11 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_12",
        &si, &si[ 12 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_13",
        &si, &si[ 13 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_14",
        &si, &si[ 14 ].InfusionRate, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "InfusionRate_15",
        &si, &si[ 15 ].InfusionRate, FLOAT );
		
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_1",
        &si, &si[ 1 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_2",
        &si, &si[ 2 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_3",
        &si, &si[ 3 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_4",
        &si, &si[ 4 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_5",
        &si, &si[ 5 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_6",
        &si, &si[ 6 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_7",
        &si, &si[ 7 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_8",
        &si, &si[ 8 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_9",
        &si, &si[ 9 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_10",
        &si, &si[ 10 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_11",
        &si, &si[ 11 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_12",
        &si, &si[ 12 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_13",
        &si, &si[ 13 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_14",
        &si, &si[ 14 ].HypoD, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "HypoD_15",
        &si, &si[ 15 ].HypoD, FLOAT );
		
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_1",
        &si, &si[ 1 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_2",
        &si, &si[ 2 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_3",
        &si, &si[ 3 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_4",
        &si, &si[ 4 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_5",
        &si, &si[ 5 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_6",
        &si, &si[ 6 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_7",
        &si, &si[ 7 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_8",
        &si, &si[ 8 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_9",
        &si, &si[ 9 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_10",
        &si, &si[ 10 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_11",
        &si, &si[ 11 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_12",
        &si, &si[ 12 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_13",
        &si, &si[ 13 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_14",
        &si, &si[ 14 ].Vacr, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "Vacr_15",
        &si, &si[ 15 ].Vacr, FLOAT );

	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_1",
        &si, &si[ 1 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_2",
        &si, &si[ 2 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_3",
        &si, &si[ 3 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_4",
        &si, &si[ 4 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_5",
        &si, &si[ 5 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_6",
        &si, &si[ 6 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_7",
        &si, &si[ 7 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_8",
        &si, &si[ 8 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_9",
        &si, &si[ 9 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_10",
        &si, &si[ 10 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_11",
        &si, &si[ 11 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_12",
        &si, &si[ 12 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_13",
        &si, &si[ 13 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_14",
        &si, &si[ 14 ].EndVin, FLOAT );
	new SpoofableData( CDS, "Predict_SubstateInfo", "EndVin_15",
        &si, &si[ 15 ].EndVin, FLOAT );

    // ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // HAL CDS data
    //

	//Hal: Pressure sensors
    new SpoofableData( CDS, "HalStatus_aps", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_cps", "data", 0, 0, FLOAT );

	//Hal: Pumps
	new SpoofableData( CDS, "HalStatus_inletCmdRpm", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_acCmdRpm", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_returnCmdRpm", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_plateletCmdRpm", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_plasmaCmdRpm", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_inletActRpm", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_acActRpm", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_returnActRpm", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_plateletActRpm", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_plasmaActRpm", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_inletCmdFlow", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_acCmdFlow", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_returnCmdFlow", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_plateletCmdFlow", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_plasmaCmdFlow", "data", 0, 0, FLOAT );

	//Hal: Valves
	new SpoofableData( CDS, "HalStatus_RBCValveStatus", "data", 0, 0, ENUM, "HAL_VALVESTATE" ); //HAL_VALVE_UNKNOWN, HAL_VALVE_COLLECT, HAL_VALVE_OPEN, HAL_VALVE_RETURN
	new SpoofableData( CDS, "HalStatus_PlasmaValveStatus", "data", 0, 0, ENUM, "HAL_VALVESTATE" );
	new SpoofableData( CDS, "HalStatus_CollectValveStatus", "data", 0, 0, ENUM, "HAL_VALVESTATE" );
	new SpoofableData( CDS, "HalStatus_RBCValveOrder", "data", 0, 0, ENUM, "HAL_VALVESTATE" );
	new SpoofableData( CDS, "HalStatus_PlasmaValveOrder", "data", 0, 0, ENUM, "HAL_VALVESTATE" );
	new SpoofableData( CDS, "HalStatus_CollectValveOrder", "data", 0, 0, ENUM, "HAL_VALVESTATE" );

	//Hal: RBC detector
	new SpoofableData( CDS, "HalStatus_redValue", "data", 0, 0, SHORT );
	new SpoofableData( CDS, "HalStatus_greenValue", "data", 0, 0, SHORT );
	new SpoofableData( CDS, "HalStatus_redDriveValue", "data", 0, 0, SHORT );
	new SpoofableData( CDS, "HalStatus_greenDriveValue", "data", 0, 0, SHORT );

	//Hal: Centrifuge
	new SpoofableData( CDS, "HalStatus_centActRPM", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "HalStatus_centCmdRPM", "data", 0, 0, FLOAT );

	//Hal: Power supply voltage readings (mV)
	new SpoofableData( CDS, "HalStatus_PS64Vsw_nValue", "data", 0, 0, INT );
	new SpoofableData( CDS, "HalStatus_PS64V_nValue", "data", 0, 0, INT );
	new SpoofableData( CDS, "HalStatus_PS24Vsw_nValue", "data", 0, 0, INT );
	new SpoofableData( CDS, "HalStatus_PS24V_nValue", "data", 0, 0, INT );
	new SpoofableData( CDS, "HalStatus_PSp12V_nValue", "data", 0, 0, INT );
	new SpoofableData( CDS, "HalStatus_PSm12V_nValue", "data", 0, 0, INT );
	new SpoofableData( CDS, "HalStatus_PS5V_nValue", "data", 0, 0, INT );

	//Hal: Door
	new SpoofableData( CDS, "HalStatus_doorStatus", "data", 0, 0, ENUM, "HAL_DOORSTATE" ); //HAL_DOOR_OPEN, HAL_DOOR_LOCKED_STATE, HAL_DOOR_CLOSED_STATE, HAL_DOOR_CLOSED_AND_LOCKED 

	// ///////////////////////////////////////////////////////////End of Hal////////////////////////////////////////////////////////////



    //
    // ProcRun CDS data
    //
    new SpoofableData( CDS, "ProcRun_CrAv", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcRun_IntegratedPlateletYield", "data", 0, 0, FLOAT );


    //
    // ProcVolumes CDS data
    //
	new SpoofableData( CDS, "ProcVolumes_Vin", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcVolumes_VinTotal", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcVolumes_Vac", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcVolumes_Vp", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcVolumes_VacTotal", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcVolumes_Vreturn", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcVolumes_PlateletVolume", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcVolumes_ACinPlateletBag", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcVolumes_PlasmaVolume", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcVolumes_ACinPlasmaBag", "data", 0, 0, FLOAT ); // for testing IT10221
    new SpoofableData( CDS, "ProcVolumes_RBCVolume", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcVolumes_ACinRBCBag", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcVolumes_RBC1_Volume", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcVolumes_RBC2_Volume", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcVolumes_VinCurentSubstate", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcVolumes_VstorageRBCBag", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcVolumes_VstoragePLTBag", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcVolumes_PreviousPlateletYield", "data", 0, 0, FLOAT ); // for testing It 6933
    new SpoofableData( CDS, "ProcVolumes_PlateletYield", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcVolumes_VSalineBolus", "data", 0, 0, FLOAT );

    //
    // ProcRunTargets CDS data
    //
    new SpoofableData( CDS, "ProcRunTargets_RbcVolumeTarget", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcRunTargets_PlateletVolumeTarget", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcRunTargets_PlasmaTarget", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcRunTargets_PlateletYieldTarget", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcRunTargets_SelectedPlateletYield", "data", 0, 0, FLOAT );
    new SpoofableData( CDS, "ProcRunTargets_SelectedPlateletVolume", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcRunTargets_PlateletConcenTarget", "data", 0, 0, FLOAT );

     
    // 
    // ProcRun CDS data -for the draw cycle
    //
    new SpoofableData( CDS, "ProcRun_DrawCycle", "data", 0, 0, INT );
    new SpoofableData( CDS, "ProcRun_FirstDrawComplete", "data", 0, 0, INT );
    new SpoofableData( CDS, "ProcRun_RedPl", "data", 0, 0, INT );

    // 
    // ProcCycle CDS data
    //
    new SpoofableData( CDS, "ProcCycle_ResHct", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcCycle_ACRatio", "data", 0, 0, FLOAT ); 
	new SpoofableData( CDS, "ProcCycle_Vac", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcCycle_Vres", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcCycle_VresLast", "data", 0, 0, FLOAT );

	//
	// ProcTrimaSet CDS data
	//
	new SpoofableData( CDS, "ProcTrimaSet_APSZero", "data", 0, 0, FLOAT );

	//
	// ProcDonor_CDS data
	//
	new SpoofableData( CDS, "ProcDonorCDS_NadlerTBV", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcDonorCDS_OgawaTBV", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcDonorCDS_Weight", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcDonorCDS_Hct", "data", 0, 0, FLOAT );
	new SpoofableData( CDS, "ProcDonorCDS_PreCnt", "data", 0, 0, FLOAT );

    //
    // FastData
    //
    FASTDATA fd;
    new SpoofableData( "FastData", "raw",
	&fd, &fd.raw, FASTDATA_ARRAY ); 

#endif // SUPPORT_CONTROL_SPOOFS


#ifdef SUPPORT_SAFETY_SPOOFS

    //
    // Safety Orders
    //

    SHwOrders so;
    
    new SpoofableData( "SHwOrders", "pumpPower", 
        &so, &so.pumpPower, ENUM, "HW_ORDERS" );

    new SpoofableData( "SHwOrders", "centrifugePower", 
        &so, &so.centrifugePower, ENUM, "HW_ORDERS" );

    new SpoofableData( "SHwOrders", "valveLED", 
        &so, &so.valveLED, ENUM, "HW_ORDERS" );

	new SpoofableData( "SHwOrders", "doorSolenoidPower",
		&so, &so.doorSolenoidPower, ENUM, "HW_ORDERS" );

    //
    // Safety Status
    //
    SHwStates ss;

    new SpoofableData( "SHwStates", "returnDirection", 
        &ss, &ss.returnDirection, ENUM, "SHW_DIRECTION" );

    new SpoofableData( "SHwStates", "cassettePosition", 
        &ss, &ss.cassettePosition, ENUM, "HW_CASSETTE_STATES" );

    new SpoofableData( "SHwStates", "reservoir", 
        &ss, &ss.reservoir, ENUM, "SHW_RESERVOIR_STATES" );

    new SpoofableData( "SHwStates", "valveLedTest", 
        &ss, &ss.valveLedTest, CHAR );

    new SpoofableData( "SHwStates", "doorLocks", 
        &ss, &ss.doorLocks, ENUM, "HW_DOOR_STATES" );

    new SpoofableData( "SHwStates", "centrifugeRPM", 
        &ss, &ss.centrifugeRPM, FLOAT );

    new SpoofableData( "SHwStates", "rbcValve", 
        &ss, &ss.rbcValve, ENUM, "HW_VALVE_STATES" );

    new SpoofableData( "SHwStates", "plateletValve", 
        &ss, &ss.plateletValve, ENUM, "HW_VALVE_STATES" );

    new SpoofableData( "SHwStates", "plasmaValve", 
        &ss, &ss.plasmaValve, ENUM, "HW_VALVE_STATES" );

    new SpoofableData( "SHwStates", "centrifugeError", 
        &ss, &ss.centrifugeError, CHAR );

    new SpoofableData( "SHwStates", "returnRPM", 
        &ss, &ss.returnRPM, INT );

    new SpoofableData( "SHwStates", "acRPM", 
        &ss, &ss.acRPM, INT );

    new SpoofableData( "SHwStates", "inletRPM", 
        &ss, &ss.inletRPM, INT );

    new SpoofableData( "SHwStates", "plasmaRPM", 
        &ss, &ss.plasmaRPM, INT );

    new SpoofableData( "SHwStates", "plateletRPM", 
        &ss, &ss.plateletRPM, INT );

    // for testing new pump code as in ITs 6514, 7720, 8917
    new SpoofableData( "SHwStates", "acRevs", &ss, &ss.acRevs, INT );
    new SpoofableData( "SHwStates", "inletRevs", &ss, &ss.inletRevs, INT );
    new SpoofableData( "SHwStates", "plasmaRevs", &ss, &ss.plasmaRevs, INT );
    new SpoofableData( "SHwStates", "plateletRevs", &ss, &ss.plateletRevs, INT );
    new SpoofableData( "SHwStates", "returnRevs", &ss, &ss.returnRevs, INT );

    new SpoofableData( "SHwStates", "acPumpAccum", &ss, &ss.acAccumRevs, INT );
	new SpoofableData( "SHwStates", "inletPumpAccum", &ss, &ss.inletAccumRevs, INT );
    new SpoofableData( "SHwStates", "plasmaPumpAccum", &ss, &ss.plasmaAccumRevs, INT );
    new SpoofableData( "SHwStates", "plateletPumpAccum", &ss, &ss.plateletAccumRevs, INT );
    new SpoofableData( "SHwStates", "returnPumpAccum", &ss, &ss.returnAccumRevs, INT );

    //
    // Donor Vitals From Proc
    //
    SDonorVitals dv;

    new SpoofableData( "DVitalsFromProc", "DonorVitalStatus",
        &dv, &dv.cDonorVitalsStatus, CHAR );
	
	new SpoofableData( "DVitalsFromProc", "DonorSex",
        &dv, &dv.cDonorSex, CHAR );

    new SpoofableData( "DVitalsFromProc", "DonorWeight",
        &dv, &dv.fDonorWeight, FLOAT );

    new SpoofableData( "DVitalsFromProc", "DonorHeight",
        &dv, &dv.fDonorHeight, FLOAT );

    new SpoofableData( "DVitalsFromProc", "ProcDonorTBV",
        &dv, &dv.fProcDonorTBV, FLOAT );
		
    new SpoofableData( "DVitalsFromProc", "SafetyDonorTBV",
        &dv, &dv.fSafetyDonorTBV, FLOAT );
		
    new SpoofableData( "DVitalsFromProc", "OgawaProcDonorTBV",
        &dv, &dv.fProcDonorTBVOgawa, FLOAT );

    new SpoofableData( "DVitalsFromProc", "DonorHct",
        &dv, &dv.fDonorHct, FLOAT );

    new SpoofableData( "DVitalsFromProc", "PltPrecount",
        &dv, &dv.fDonorPltPrecount, FLOAT );

    new SpoofableData( "DVitalsFromProc", "DonorWeightUnits",
        &dv, &dv.cDonorWeightUnits, CHAR );

    new SpoofableData( "DVitalsFromProc", "DonorHeightUnits",
        &dv, &dv.cDonorHeightUnits, CHAR );
		
	 new SpoofableData( "DVitalsFromSafety", "DonorVitalStatus",
        &dv, &dv.cDonorVitalsStatus, CHAR );
		
	new SpoofableData( "DVitalsFromSafety", "DonorSex",
        &dv, &dv.cDonorSex, CHAR );

    new SpoofableData( "DVitalsFromSafety", "DonorWeight",
        &dv, &dv.fDonorWeight, FLOAT );

    new SpoofableData( "DVitalsFromSafety", "DonorHeight",
        &dv, &dv.fDonorHeight, FLOAT );

    new SpoofableData( "DVitalsFromSafety", "ProcDonorTBV",
        &dv, &dv.fProcDonorTBV, FLOAT );
		
    new SpoofableData( "DVitalsFromSafety", "SafetyDonorTBV",
        &dv, &dv.fSafetyDonorTBV, FLOAT );
		
    new SpoofableData( "DVitalsFromSafety", "OgawaProcDonorTBV",
        &dv, &dv.fProcDonorTBVOgawa, FLOAT );

    new SpoofableData( "DVitalsFromSafety", "DonorHct",
        &dv, &dv.fDonorHct, FLOAT );

    new SpoofableData( "DVitalsFromSafety", "PltPrecount",
        &dv, &dv.fDonorPltPrecount, FLOAT );

    new SpoofableData( "DVitalsFromSafety", "DonorWeightUnits",
        &dv, &dv.cDonorWeightUnits, CHAR );

    new SpoofableData( "DVitalsFromSafety", "DonorHeightUnits",
        &dv, &dv.cDonorHeightUnits, CHAR );
		
	new SpoofableData( "DVitalsFromGUI", "DonorVitalStatus",
        &dv, &dv.cDonorVitalsStatus, CHAR );
		
	new SpoofableData( "DVitalsFromGUI", "DonorSex",
        &dv, &dv.cDonorSex, CHAR );

    new SpoofableData( "DVitalsFromGUI", "DonorWeight",
        &dv, &dv.fDonorWeight, FLOAT );

    new SpoofableData( "DVitalsFromGUI", "DonorHeight",
        &dv, &dv.fDonorHeight, FLOAT );

    new SpoofableData( "DVitalsFromGUI", "ProcDonorTBV",
        &dv, &dv.fProcDonorTBV, FLOAT );
		
    new SpoofableData( "DVitalsFromGUI", "SafetyDonorTBV",
        &dv, &dv.fSafetyDonorTBV, FLOAT );
		
    new SpoofableData( "DVitalsFromGUI", "OgawaProcDonorTBV",
        &dv, &dv.fProcDonorTBVOgawa, FLOAT );

    new SpoofableData( "DVitalsFromGUI", "DonorHct",
        &dv, &dv.fDonorHct, FLOAT );

    new SpoofableData( "DVitalsFromGUI", "PltPrecount",
        &dv, &dv.fDonorPltPrecount, FLOAT );

    new SpoofableData( "DVitalsFromGUI", "DonorWeightUnits",
        &dv, &dv.cDonorWeightUnits, CHAR );

    new SpoofableData( "DVitalsFromGUI", "DonorHeightUnits",
        &dv, &dv.cDonorHeightUnits, CHAR );
 
    //
    // Safety Status normally shared with control
    //
    CHwStates sc;

    new SpoofableData( "HwStatesFromControl", "accessPressure", 
    &sc, &sc.accessPressure, SHORT );

    new SpoofableData( "SafetyTestDoneMsg"); // testing IT 5802

    // Safety Status message latency support
    new SpoofableData( "LatentSafetyStatusRequest", "delay", 0, 0, INT );
    new SpoofableData( "LatentSafetyStatus", "delay", 0, 0, INT );

#endif // SUPPORT_SAFETY_SPOOFS

}
