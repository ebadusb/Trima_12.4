// FILENAME: spoofedobject.cpp
// $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/spoofedobject.cpp 6.26 2008/12/01 18:45:10Z kgreusb Exp kgreusb $
// PURPOSE: SpoofedObject class implementation
// CHANGELOG:
//   $Log: spoofedobject.cpp $
//   Revision 6.26  2008/12/01 18:45:10Z  kgreusb
//   Adding object to test IT-6933
//   Revision 6.25  2007/02/27 22:35:25Z  kgreusb
//   Revision 6.24  2007/02/23 22:49:32Z  kgreusb
//   Adding Spoof_Safety_APS for IT 7828
//   Revision 6.23  2004/04/16 14:59:42Z  td07711
//   IT6922 - make ProcRunTargets_CDS.SelectedPlateletYield spoofable.
//   Revision 6.22  2003/10/23 18:25:38Z  td07711
//   IT3338 - support fastdata spoofing
//   Revision 6.21  2003/08/13 16:54:58Z  td07711
//   IT6356 - move donor vitals spoofing from control to safety computer.
//   Revision 6.20  2003/06/30 20:18:36Z  td07711
//   IT6166 - add support for RequestSafetyPowerMsg
//   Revision 6.19  2003/06/12 22:08:25Z  td07711
//   support SystemStateChangeMsg - IT6126
//   Revision 6.18  2003/06/12 17:58:45Z  td07711
//   support ProcRunTargets_RBCVolumeTarget - IT5443
//   Revision 6.17  2003/06/06 17:15:24Z  jl11312
//   - end procedure message is no longer used by the system
//   Revision 6.16  2003/05/22 22:45:17Z  td07711
//   only #include cds files for control side build
//   Revision 6.15  2003/04/04 00:00:15Z  td07711
//   support procvolumes_cds
//   Revision 6.14  2003/04/03 21:16:20Z  td07711
//   support spoofing DVitalsFromProc msgs
//   Revision 6.13  2003/03/18 02:08:49Z  td07711
//   support msgMultiplier
//   Revision 6.12  2003/02/10 21:53:14Z  td07711
//   add SHwOrders support
//   Revision 6.11  2003/01/31 23:44:31Z  td07711
//   split support for SAFETY and CONTROL
//   Revision 6.10  2003/01/22 01:26:46Z  td07711
//   add support for HalStatus_aps
//   Revision 6.9  2002/11/21 16:45:56Z  td07711
//   #include restructuring
//   Revision 6.8  2002/11/09 01:20:23  td07711
//   add spoof buffer caching to avoid spoofing when necessary
//   Revision 6.7  2002/10/23 21:42:23  td07711
//   added error checking
//   Revision 6.6  2002/10/22 23:20:40  td07711
//   support more int messages
//   Revision 6.5  2002/10/18 23:19:36  td07711
//   add spoofTheData() to spoof the spoofedElements
//   Revision 6.4  2002/10/15 23:24:49  td07711
//   fix SpoofedCds creation
//   Revision 6.3  2002/10/11 23:29:30  td07711
//   add CDS support
//   Revision 6.2  2002/10/10 21:27:34  td07711
//   add remove()
//   Revision 6.1  2002/10/09 17:09:48  td07711
//   allocate storage for SpoofedObject name


// standard includes
#include <string.h> // for memset()

// trima includes - for structures being spoofed
#include "chw.hpp"
#include "shw.hpp"
#include "trimamessages.h"
#include "safproc.h"
#include "fastdata.h"
#include "rwFastData.hpp" // Added for spoofs inside of Automated Smoke Test
#include "../../safety/s_pfr.hpp"
#include "haldefs.h"


#ifdef SUPPORT_CONTROL_SPOOFS
#include "procvolumes_cds.h"
#include "predict_cds.h"
#include "halstatus_cds.h"
#include "procruntargets_cds.h"
#include "proccycle_cds.h"
#include "procrun_cds.h"
#include "proctrimaset_cds.h"
#include "procdonor_cds.h"
#endif // SUPPORT_CONTROL_SPOOFS

#undef LOG_ERROR

// spoofer includes
#include "spoofedmsg.hpp"
#include "spoofedintmsg.hpp"
#include "spoofedcds.hpp"
#include "spoofeddonorvitalsmsg.hpp"
#include "spoofedlatentmsg.hpp"
#include "spooferlog.hpp"

typedef CHwStates CHwStatesClone; // IT 7828

//
// initialize statics
//
map< string, SpoofedObject* > SpoofedObject :: _allSpoofedObjects;
bool SpoofedObject :: _spoofingEnabled = false;
bool SpoofedObject :: _showSpoofs = false;


SpoofedObject :: SpoofedObject( const char* name )
: _name( 0 ), _spoofedElements(), _spoofBufferNotValid( true )
{
    // add to _allSpoofedObjects, error if already there 
    if( _allSpoofedObjects.find( name ) != _allSpoofedObjects.end() )
    {
        LOG_ERROR("SpoofedObject: entry for %s already exists", name );
        exit(1);
    }

    // copy the object name, as original will be overwritten in scm receive buffer
    _name = new char[ strlen( name ) + 1 ];
    strcpy( (char*)_name, name ); // cast away constness to allow intentional copy

    _allSpoofedObjects[ name ] = this;

    LOG_DEBUG("SpoofedObject: %s constructed", name );
}


SpoofedObject :: ~SpoofedObject( )
{
    // delete all entries from _spoofedElements container
    for( spoofedElementsIterator_t i = _spoofedElements.begin(); 
        i != _spoofedElements.end(); i++ )
    {
        delete i->second;
    }

    LOG_DEBUG( "~SpoofedObject: %s destroyed", _name );

    delete [] _name;
    _name = 0;
}


void SpoofedObject :: addSpoof( const char* dataName, const char* spoofValue )
{
    // PURPOSE: adds a spoof
    // if already present, replace with new spoof

    LOG_DEBUG( "addSpoof: %s spoofing %s to %s", 
              _name, dataName, spoofValue );

    // verify requested data is spoofable
    if( 0 == SpoofableData::get( _name, dataName ) )
    {
        LOG_ERROR( "addSpoof: %s.%s is not supported", 
                  _name, dataName );
        return;
    }

    // create SpoofMsgField if not already in use
    if( _spoofedElements.find( dataName ) == _spoofedElements.end() )
    {
        _spoofedElements[ dataName ] = new SpoofedElement( this, dataName );
    }
    
    _spoofedElements[ dataName ]->setSpoofValue( spoofValue ); 
    _spoofBufferNotValid = true;
}


void SpoofedObject :: addSpoof( const char* dataName, float mult, float offset )
{
    // PURPOSE: adds a spoof
    // if already present, replace with new spoof

    LOG_DEBUG( "addSpoof: %s spoofing %s %fx + %f", 
              _name, dataName, mult, offset );

    // verify requested data is spoofable
    if( 0 == SpoofableData::get( _name, dataName ) )
    {
        LOG_ERROR( "addSpoof: %s.%s is not supported", 
                  _name, dataName );
        return;
    }

    // create SpoofMsgField if not already in use
    if( _spoofedElements.find( dataName ) == _spoofedElements.end() )
    {
        _spoofedElements[ dataName ] = new SpoofedElement( this, dataName );
    }
    
    _spoofedElements[ dataName ]->setSpoofValue( mult, offset ); 
    _spoofBufferNotValid = true;
}


void SpoofedObject :: dump()
{
    // PURPOSE: dumps all spoofs for this SpoofedObject

    LOG_INFO( "dump: _name = %s", _name );

    for( spoofedElementsIterator_t i = _spoofedElements.begin();
        i != _spoofedElements.end(); i++ )
    {
        (( *i ).second)->dump();
    }
}


void SpoofedObject :: removeSpoof( const char* dataName )
{
    // PURPOSE: removes spoofing of specified field

    spoofedElementsIterator_t i = _spoofedElements.find( dataName );
    if( i != _spoofedElements.end() )
    {
        LOG_DEBUG("removeSpoof: %s removing %s", name(), dataName );

        delete ( *i ).second;
        _spoofedElements.erase( i );
        _spoofBufferNotValid = true;
    }
    else
    {
        LOG_WARNING( "removeSpoof: %s no entry for %s", 
                    name(), dataName );
    }
}


void SpoofedObject :: dumpAll( )
{
    // PURPOSE: dumps info for all SpoofMsg objects in use

    for( SpoofedObjectsIterator_t i = _allSpoofedObjects.begin();
         i != _allSpoofedObjects.end(); i++ )
    {
        ( *i ).second->dump();
    }
}


SpoofedObject* SpoofedObject :: find( const char* name )
{
    // PURPOSE: retrieve ptr to SpoofedObject given a name

    if( _allSpoofedObjects.find( name ) != _allSpoofedObjects.end() )
    {
        return _allSpoofedObjects[ name ];
    }
    else
    {
        return 0;
    }
}


void SpoofedObject :: remove( const char* name )
{
    // PURPOSE: remove object from spoofing

    SpoofedObject* p = find( name );
    if( p == 0 )
    {
        LOG_ERROR( "remove: %s not found", name );
    }
    else
    {
        _allSpoofedObjects.erase( name );
        delete p;

        LOG_DEBUG( "remove: %s removed", name );
    }
}


void SpoofedObject :: cleanup()
{
    // PURPOSE: delete all SpoofedObject instances

    LOG_DEBUG( "cleanup: n=%d", _allSpoofedObjects.size() );

    for( SpoofedObjectsIterator_t i = _allSpoofedObjects.begin();
        i != _allSpoofedObjects.end(); i++ )
    {
        delete i->second; 
    }

    _allSpoofedObjects.clear();
    _showSpoofs = false;
}


void SpoofedObject :: spoofTheData()
{
    // PURPOSE: spoofs each of the _spoofedElements for this SpoofedObject

    for( spoofedElementsIterator_t i = _spoofedElements.begin();
	i != _spoofedElements.end(); i++ )
    {
	(*i).second->spoof();
    }

    _spoofBufferNotValid = false; 
}


SpoofedObject* SpoofedObject :: create( const char* name )
{
    // PURPOSE: given a name, create an instance of the
    // associated SpoofedObject
    //
    // NOTE: all types supported must be defined here
    //

    SpoofedObject* p = find( name );
    if(p != 0)
    {
        LOG_ERROR( "create: %s already exists", name );
        return p;
    }

#ifdef SUPPORT_CONTROL_SPOOFS
    //
    // define all non-int messages supported here
    //
    if( strcmp( name, "CHwOrders") == 0 )
    {
        p = new SpoofedMsg< CHwOrders >( name );
    }
    else if( strcmp( name, "CHwStates" ) == 0 )
    {
        p = new SpoofedMsg< CHwStates >( name );
    }
    else if( strcmp( name, "FastData" ) == 0 )
    {
        p = new SpoofedMsg< FASTDATA >( name );
    }
	else if( strcmp( name, "RW_FastData" ) == 0 )
    {
        p = new RW_FastData();
    }

    // 
    // define all int messages supported here
    //
    else if( strcmp( name, "CassettePositionStatusMsg" ) == 0 )
    {
        p = new SpoofedIntMsg< CassettePositionStatusMsg >( name );
    }
    else if( strcmp( name, "CentrifugePressureStatusMsg" ) == 0 )
    {
        p = new SpoofedIntMsg< CentrifugePressureStatusMsg >( name );
    }
    else if( strcmp( name, "CentrifugeStopMsg" ) == 0 )
    {
        p = new SpoofedIntMsg< CentrifugeStopMsg >( name );
    }
    else if( strcmp( name, "ConfigChangedMsg" ) == 0 )
    {
        p = new SpoofedIntMsg< ConfigChangedMsg >( name );
    }
    else if( strcmp( name, "DateTimeSetMsg" ) == 0 )
    {
        p = new SpoofedIntMsg< DateTimeSetMsg >( name );
    }
    else if( strcmp( name, "DeleteProductMsg" ) == 0 )
    {
        p = new SpoofedIntMsg< DeleteProductMsg >( name );
    }
    else if( strcmp( name, "DisconnectCompleteMsg" ) == 0 )
    {
        p = new SpoofedIntMsg< DisconnectCompleteMsg >( name );
    }
    else if( strcmp( name, "DisposableIDMsg" ) == 0 )
    {
        p = new SpoofedIntMsg< DisposableIDMsg >( name );
    }
    else if( strcmp( name, "ProcStateMsg" ) == 0 )
    {
        p = new SpoofedIntMsg< ProcStateMsg >( name );
    }
    else if( strcmp( name, "SystemStateChangeMsg" ) == 0 )
    {
        p = new SpoofedIntMsg< SystemStateChangeMsg >( name );
    }
    else if( strcmp( name, "RequestSafetyPowerMsg" ) == 0 )
    {
        p = new SpoofedIntMsg< RequestSafetyPowerMsg >( name );
    }
    else if ( strcmp( name, "APSZeroMsg") == 0)
    {
        p = new SpoofedIntMsg< APSZeroMsg >( name );
    }
#if 0
    else if ( strcmp( name, "ButtonPressedIDMsg") == 0)     
    {                                                     
        p = new SpoofedIntMsg< ButtonPressedIDMsg >( name );
    }
#endif
    else if ( strcmp( name, "DonorDataStatusMsg") == 0)     
    {                                                     
        p = new SpoofedIntMsg< DonorDataStatusMsg >( name );
    }               
    else if( strcmp( name, "DVitalsFromGUI" ) == 0 )            
    {                                                           
        p = new SpoofedDonorVitalsMsg< DVitalsFromGUI >( name );
    }                                                           

	                                      


    //
    // define all CDS objects supported here
    //
    else if( strcmp( name, "Predict_SubstateInfo") == 0 )
    {
        SpoofedCds< Predict_CDS, Predict_CDS::substate_info_t >* pC;
        pC = new SpoofedCds< Predict_CDS, Predict_CDS::substate_info_t >( name );
        pC->init( &pC->cds().SubstateInfo );
        p = pC;
    }
    else if( strcmp( name, "HalStatus_aps") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds()._aps );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_cps") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds()._cps );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_inletCmdRpm") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().InletPump._CmdRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_acCmdRpm") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().ACPump._CmdRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_returnCmdRpm") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().ReturnPump._CmdRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_plateletCmdRpm") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().CollectPump._CmdRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_plasmaCmdRpm") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().PlasmaPump._CmdRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_inletActRpm") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().InletPump._ActRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_acActRpm") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().ACPump._ActRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_returnActRpm") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().ReturnPump._ActRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_plateletActRpm") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().CollectPump._ActRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_plasmaActRpm") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().PlasmaPump._ActRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_inletCmdFlow") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().InletPump._CmdFlow );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_acCmdFlow") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().ACPump._CmdFlow );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_returnCmdFlow") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().ReturnPump._CmdFlow );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_plateletCmdFlow") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().CollectPump._CmdFlow );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_plasmaCmdFlow") == 0 )
    {
        SpoofedCds< HalStatus_CDS, float >* pC;
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().PlasmaPump._CmdFlow );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_doorStatus") == 0 )
	{
		SpoofedCds< HalStatus_CDS, HAL_DOORSTATE >* pC;
		pC = new SpoofedCds< HalStatus_CDS, HAL_DOORSTATE >( name );
		pC->init( &pC->cds()._doorStatus );
		p = pC;
	}
	else if( strcmp( name, "HalStatus_RBCValveStatus") == 0 )
    {
        SpoofedCds< HalStatus_CDS, HAL_VALVESTATE >* pC;
        pC = new SpoofedCds< HalStatus_CDS, HAL_VALVESTATE >( name );
        pC->init( &pC->cds().RBCValve._status );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_PlasmaValveStatus") == 0 )
    {
        SpoofedCds< HalStatus_CDS, HAL_VALVESTATE >* pC;
        pC = new SpoofedCds< HalStatus_CDS, HAL_VALVESTATE >( name );
        pC->init( &pC->cds().PlasmaValve._status );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_CollectValveStatus") == 0 )
    {
        SpoofedCds< HalStatus_CDS, HAL_VALVESTATE >* pC;
        pC = new SpoofedCds< HalStatus_CDS, HAL_VALVESTATE >( name );
        pC->init( &pC->cds().CollectValve._status );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_RBCValveOrder") == 0 )
    {
        SpoofedCds< HalStatus_CDS, HAL_VALVESTATE >* pC;
        pC = new SpoofedCds< HalStatus_CDS, HAL_VALVESTATE >( name );
        pC->init( &pC->cds().RBCValve._order );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_PlasmaValveOrder") == 0 )
    {
        SpoofedCds< HalStatus_CDS, HAL_VALVESTATE >* pC;
        pC = new SpoofedCds< HalStatus_CDS, HAL_VALVESTATE >( name );
        pC->init( &pC->cds().PlasmaValve._order );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_CollectValveOrder") == 0 )
    {
        SpoofedCds< HalStatus_CDS, HAL_VALVESTATE >* pC;
        pC = new SpoofedCds< HalStatus_CDS, HAL_VALVESTATE >( name );
        pC->init( &pC->cds().CollectValve._order );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_redValue") == 0 )
    {
        SpoofedCds< HalStatus_CDS, short >* pC;
        pC = new SpoofedCds< HalStatus_CDS, short >( name );
        pC->init( &pC->cds()._redValue );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_greenValue") == 0 )
    {
        SpoofedCds< HalStatus_CDS, short >* pC;
        pC = new SpoofedCds< HalStatus_CDS, short >( name );
        pC->init( &pC->cds()._greenValue );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_redDriveValue") == 0 )
    {
        SpoofedCds< HalStatus_CDS, short >* pC;
        pC = new SpoofedCds< HalStatus_CDS, short >( name );
        pC->init( &pC->cds()._redDriveValue );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_greenDriveValue") == 0 )
    {
        SpoofedCds< HalStatus_CDS, short >* pC;
        pC = new SpoofedCds< HalStatus_CDS, short >( name );
        pC->init( &pC->cds()._greenDriveValue );
        p = pC;
    }	
    else if( strcmp( name, "HalStatus_centActRPM") == 0 )   
    {                                                       
        SpoofedCds< HalStatus_CDS, float >* pC;             
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds()._centRPM );                    
        p = pC;                                             
    }                                                       
    else if( strcmp( name, "HalStatus_centCmdRPM") == 0 )   
    {                                                       
        SpoofedCds< HalStatus_CDS, float >* pC;             
        pC = new SpoofedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds()._centCmdRPM );                    
        p = pC;                                             
    }               
	else if( strcmp( name, "HalStatus_PS64Vsw_nValue") == 0 )
    {
        SpoofedCds< HalStatus_CDS, int >* pC;
        pC = new SpoofedCds< HalStatus_CDS, int >( name );
        pC->init( &pC->cds().PS64Vsw._nValue );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_PS64V_nValue") == 0 )
	{
		SpoofedCds< HalStatus_CDS, int >* pC;
		pC = new SpoofedCds< HalStatus_CDS, int >( name );
		pC->init( &pC->cds().PS64V._nValue );
		p = pC;
	}
	else if( strcmp( name, "HalStatus_PS24Vsw_nValue") == 0 )
	{
		SpoofedCds< HalStatus_CDS, int >* pC;
		pC = new SpoofedCds< HalStatus_CDS, int >( name );
		pC->init( &pC->cds().PS24Vsw._nValue );
		p = pC;
	}
	else if( strcmp( name, "HalStatus_PS24V_nValue") == 0 )
	{
		SpoofedCds< HalStatus_CDS, int >* pC;
		pC = new SpoofedCds< HalStatus_CDS, int >( name );
		pC->init( &pC->cds().PS24V._nValue );
		p = pC;
	}
	else if( strcmp( name, "HalStatus_PSp12V_nValue") == 0 )
	{
		SpoofedCds< HalStatus_CDS, int >* pC;
		pC = new SpoofedCds< HalStatus_CDS, int >( name );
		pC->init( &pC->cds().PSp12V._nValue );
		p = pC;
	}
	else if( strcmp( name, "HalStatus_PSm12V_nValue") == 0 )
	{
		SpoofedCds< HalStatus_CDS, int >* pC;
		pC = new SpoofedCds< HalStatus_CDS, int >( name );
		pC->init( &pC->cds().PSm12V._nValue );
		p = pC;
	}
	else if( strcmp( name, "HalStatus_PS5V_nValue") == 0 )
	{
		SpoofedCds< HalStatus_CDS, int >* pC;
		pC = new SpoofedCds< HalStatus_CDS, int >( name );
		pC->init( &pC->cds().PS5V._nValue );
		p = pC;
	}
    else if( strcmp( name, "ProcRun_CrAv") == 0 )
    {
        SpoofedCds< ProcRun_CDS, float >* pC;
        pC = new SpoofedCds< ProcRun_CDS, float >( name );
        pC->init( &pC->cds().CrAv );
        p = pC;
    }
	else if( strcmp( name, "ProcRun_IntegratedPlateletYield") == 0 )
    {
        SpoofedCds< ProcRun_CDS, float >* pC;
        pC = new SpoofedCds< ProcRun_CDS, float >( name );
        pC->init( &pC->cds().IntegratedPlateletYield );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_Vin") == 0 )
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().Vin );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_VinTotal") == 0 )
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().VinTotal );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_Vreturn") == 0 )
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().Vreturn );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_Vac") == 0 )
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().Vac );
        p = pC;
    }
	else if( strcmp( name, "ProcVolumes_Vp") == 0 )
	{
		SpoofedCds< ProcVolumes_CDS, float >* pC;
		pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
		pC->init( &pC->cds().Vp );
		p = pC;
	}
    else if( strcmp( name, "ProcVolumes_PlateletVolume") == 0 )
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().PlateletVolume );
        p = pC;
    }
    else if ( strcmp(name, "ProcVolumes_ACinPlateletBag") == 0)
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().VacPlateletBag );
        p= pC;
    }
    else if( strcmp( name, "ProcVolumes_PlasmaVolume") == 0 )
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().PlasmaVolume );
        p = pC;
    }
    else if ( strcmp(name, "ProcVolumes_ACinPlasmaBag") == 0)
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().VacPlasmaBag );
        p= pC;
    }
    else if( strcmp( name, "ProcVolumes_RBCVolume") == 0 )
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().RBCVolume );
        p = pC;
    }
    else if ( strcmp(name, "ProcVolumes_ACinRBCBag") == 0)
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().VacRBCBag );
        p= pC;
    }
    else if( strcmp( name, "ProcVolumes_RBC1_Volume") == 0 )
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().RBCProduct_A_FinalVolume );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_RBC2_Volume") == 0 )
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().RBCProduct_B_FinalVolume );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_VinCurrentSubstate") == 0 )
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().VinCurrentSubstate );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_VstorageRBCBag") == 0 )
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().VstorageRBCBag );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_VstoragePLTBag") == 0 )
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().VstoragePltBag );
        p = pC;
    }
    else if ( strcmp(name, "ProcVolumes_PreviousPlateletYield") == 0)
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().PreviousPlateletYield );
        p = pC;
    }
    else if ( strcmp(name, "ProcVolumes_PlateletYield") == 0)
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().PlateletYield );
        p= pC;
    }
	else if ( strcmp(name, "ProcVolumes_VSalineBolus") == 0)
    {
        SpoofedCds< ProcVolumes_CDS, float >* pC;
        pC = new SpoofedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().VSalineBolus );
        p= pC;
    }
    else if( strcmp( name, "ProcRunTargets_RbcVolumeTarget") == 0 )
    {
        SpoofedCds< ProcRunTargets_CDS, float >* pC;
        pC = new SpoofedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().RbcVolumeTarget );
        p = pC;
    }
    else if( strcmp( name, "ProcRunTargets_PlateletVolumeTarget") == 0 )
    {
        SpoofedCds< ProcRunTargets_CDS, float >* pC;
        pC = new SpoofedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().PlateletVolumeTarget );
        p = pC;
    }
    else if( strcmp( name, "ProcRunTargets_PlasmaTarget") == 0 )
    {
        SpoofedCds< ProcRunTargets_CDS, float >* pC;
        pC = new SpoofedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().PlasmaTarget );
        p = pC;
    }
    else if( strcmp( name, "ProcRunTargets_PlateletYieldTarget") == 0 )
    {
        SpoofedCds< ProcRunTargets_CDS, float >* pC;
        pC = new SpoofedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().PlateletYieldTarget );
        p = pC;
    }
    else if( strcmp( name, "ProcRunTargets_SelectedPlateletYield") == 0 )
    {
        SpoofedCds< ProcRunTargets_CDS, float >* pC;
        pC = new SpoofedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().SelectedPlateletYield );
        p = pC;
    }
    else if( strcmp( name, "ProcRunTargets_SelectedPlateletVolume") == 0 )
    {
        SpoofedCds< ProcRunTargets_CDS, float >* pC;
        pC = new SpoofedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().SelectedPlateletVolume );
        p = pC;
    }
	else if( strcmp( name, "ProcRunTargets_PlateletConcenTarget") == 0 )
    {
        SpoofedCds< ProcRunTargets_CDS, float >* pC;
        pC = new SpoofedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().PlateletConcenTarget );
        p = pC;
    }
    else if( strcmp( name, "ProcRun_RedPl") == 0 )
    {
        SpoofedCds< ProcRun_CDS, int >* pC;
        pC = new SpoofedCds< ProcRun_CDS, int >( name );
        pC->init( &pC->cds().RedPl );
        p = pC;
    }
    else if( strcmp( name, "ProcCycle_ResHct") == 0 )
    {
        SpoofedCds< ProcCycle_CDS, float >* pC;
        pC = new SpoofedCds< ProcCycle_CDS, float >( name );
        pC->init( &pC->cds().ResHct );
        p = pC;
    }
    else if( strcmp( name, "ProcCycle_Vres") == 0 )
    {
        SpoofedCds< ProcCycle_CDS, float >* pC;
        pC = new SpoofedCds< ProcCycle_CDS, float >( name );
        pC->init( &pC->cds().Vres );
        p = pC;
    }
    else if( strcmp( name, "ProcCycle_VresLast") == 0 )
    {
        SpoofedCds< ProcCycle_CDS, float >* pC;
        pC = new SpoofedCds< ProcCycle_CDS, float >( name );
        pC->init( &pC->cds().VresLast );
        p = pC;
    }
    else if( strcmp( name, "ProcCycle_ACRatio") == 0 )
    {
        SpoofedCds< ProcCycle_CDS, float >* pC;
        pC = new SpoofedCds< ProcCycle_CDS, float >( name );
        pC->init( &pC->cds().ACRatio );
        p = pC;
    }
    else if( strcmp( name, "ProcCycle_Vac") == 0 )
    {
        SpoofedCds< ProcCycle_CDS, float >* pC;
        pC = new SpoofedCds< ProcCycle_CDS, float >( name );
        pC->init( &pC->cds().Vac );
        p = pC;
    }
    else if( strcmp( name, "ProcTrimaSet_APSZero") == 0 )
    {
        SpoofedCds< ProcTrimaSet_CDS, float >* pC;
        pC = new SpoofedCds< ProcTrimaSet_CDS, float >( name );
        pC->init( &pC->cds().APSZero );
        p = pC;
    }
    else if( strcmp( name, "ProcDonorCDS_NadlerTBV") == 0 )
    {
        SpoofedCds< ProcDonor_CDS, float >* pC;
        pC = new SpoofedCds< ProcDonor_CDS, float >( name );
        pC->init( &pC->cds().TBV );
        p = pC;
    }
    else if( strcmp( name, "ProcDonorCDS_OgawaTBV") == 0 )
    {
        SpoofedCds< ProcDonor_CDS, float >* pC;
        pC = new SpoofedCds< ProcDonor_CDS, float >( name );
        pC->init( &pC->cds().TBVOgawa );
        p = pC;
    }
    else if( strcmp( name, "ProcDonorCDS_Weight") == 0 )
    {
        SpoofedCds< ProcDonor_CDS, float >* pC;
        pC = new SpoofedCds< ProcDonor_CDS, float >( name );
        pC->init( &pC->cds().Weight );
        p = pC;
    }
    else if( strcmp( name, "ProcDonorCDS_Hct") == 0 )
    {
        SpoofedCds< ProcDonor_CDS, float >* pC;
        pC = new SpoofedCds< ProcDonor_CDS, float >( name );
        pC->init( &pC->cds().Hct );
        p = pC;
    }
    else if( strcmp( name, "ProcDonorCDS_PreCnt") == 0 )
    {
        SpoofedCds< ProcDonor_CDS, float >* pC;
        pC = new SpoofedCds< ProcDonor_CDS, float >( name );
        pC->init( &pC->cds().PreCnt );
        p = pC;
    }

#endif // SUPPORT_CONTROL_SPOOFS

#ifdef SUPPORT_SAFETY_SPOOFS
    if( strcmp( name, "SHwStates" ) == 0 )
    {
	p = new SpoofedMsg< SHwStates >( name );
    }
    else if( strcmp( name, "SHwOrders" ) == 0 )
    {
        p = new SpoofedMsg< SHwOrders >( name );
    }
    else if( strcmp( name, "DVitalsFromProc" ) == 0 )
    {
        p = new SpoofedDonorVitalsMsg< DVitalsFromProc >( name );
    }
	else if( strcmp( name, "DVitalsFromSafety" ) == 0 )
    {
        p = new SpoofedDonorVitalsMsg< DVitalsFromSafety >( name );
    }
    else if( strcmp( name, "DVitalsFromGUI" ) == 0 )            
    {                                                           
        p = new SpoofedDonorVitalsMsg< DVitalsFromGUI >( name );
    }                                                           

    // add this to spoof APS for just Safety
    else if( strcmp( name, "HwStatesFromControl" ) == 0)   
    {
        LOG_DEBUG( "creating HwStatesFromControl Spoofedobject" );
        p = new SpoofedMsg< CHwStatesClone >( name ); // IT 7828
    }
    else if ( strcmp( name , "SPFRecovery" ) == 0)
    {
		LOG_DEBUG( "creating SPFRecovery Spoofedobject" );
		p = new SpoofedMsg< SPFRecovery >( name );
	}
	else if ( strcmp( name, "LatentSafetyStatusRequest" ) == 0 )
	{
		LOG_DEBUG( "creating Latent Safety Status Request SpoofedObject" );
		p = new SpoofedLatentMsg< RequestSafetyDriverStatusMsg >( name );
	}
        else if ( strcmp( name, "LatentSafetyStatus" ) == 0 )
	{
		LOG_DEBUG( "creating Latent Safety Status SpoofedObject" );
		p = new SpoofedLatentMsg< Message<SHwStates> >( name );
	}

#endif // SUPPORT_SAFETY_SPOOFS

        LOG_DEBUG( "create: %s created, n=%d", name, _allSpoofedObjects.size() );

        return p;
}
