// FILENAME: namedobject.cpp
// $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/namedobject.cpp 6.14 2008/08/15 21:54:41Z kgreusb Exp $
// PURPOSE: implements NamedObject, base of NamedMsg, NamedIntMsg, NamedCds
//
// NOTE:  NamedObjects allow a datum to be read or written by name (one shot)
//        currently used directly by spoof_main.out
//        might refactor to combine with SpoofedObjects and have spoof_server do the work.
//
// CHANGELOG:
//   $Log: namedobject.cpp $
//   Revision 6.14  2008/08/15 21:54:41Z  kgreusb
//   changes for Next Generation spoofer - adding support for state/substate change messages
//   Revision 6.13  2008/05/13 14:56:16Z  kgreusb
//   Added Spoof_SafetyTestDoneMsg
//   Revision 6.12  2008/01/23 17:22:51Z  dslausb
//   IT 8435 - VxWorks Checkin
//   Revision 6.11  2007/10/30 14:58:18Z  kgreusb
//   part of IT-8260
//   Revision 6.10  2007/02/23 22:46:23Z  kgreusb
//   Adding Spoof_Safety_APS for IT 7828
//   Revision 6.9  2006/10/06 14:56:34Z  kgreusb
//   Added DisconnectStatMsg for It 7178 and 7179
//   Revision 6.8  2003/08/08 19:00:10Z  td07711
//   IT6254 - add support for volume targets and yield
//   Revision 6.7  2003/06/30 20:18:15Z  td07711
//   IT6166 - add support for RequestSafetyPowerMsg
//   Revision 6.6  2003/06/12 22:08:47Z  td07711
//   support SystemStateChangeMsg - IT6126
//   Revision 6.5  2003/06/12 17:56:11Z  td07711
//   support CDS fields that SpoofedObject supports
//   Revision 6.4  2003/06/06 17:17:15Z  jl11312
//   - end procedure message is no longer used
//   Revision 6.3  2003/05/22 22:44:45Z  td07711
//   only #includes cds files for control side 
//   Revision 6.2  2003/01/31 23:45:21Z  td07711
//   add compile flag support
//   Revision 6.1  2002/11/21 16:44:35Z  td07711
//   add NamedCds support
//   Revision 1.1  2002/11/20 22:49:54  td07711
//   Initial revision


//
// standard includes
//
#include <string.h> // for memset()


//
// trima includes - for structures being accessed
//
#include "chw.hpp"
#include "shw.hpp"
#include "trimamessages.h"

#ifdef SUPPORT_CONTROL_SPOOFS
#include "predict_cds.h"
#include "halstatus_cds.h"
#include "procvolumes_cds.h"
#include "procruntargets_cds.h"
#include "procrun_cds.h"
#include "proccycle_cds.h"
#include "proctrimaset_cds.h"
#include "procdonor_cds.h"
#include "procpqi_cds.h"
#endif // SUPPORT_CONTROL_SPOOFS


#undef LOG_ERROR



//
// spoofer includes
//
#include "namedmsg.hpp"
#include "namedintmsg.hpp"
#include "spooferlog.hpp"

#ifdef SUPPORT_CONTROL_SPOOFS
#include "namedcds.hpp"
#endif // SUPPORT_CONTROL_SPOOFS


NamedObject :: NamedObject( const char* name )
: _name( name ), _pDataBuffer( 0 ), _pDataInfo( 0 )
{
    ASSERT( _name );

    LOG_DEBUG( "NamedObject: %s constructed", _name );
}


NamedObject :: ~NamedObject()
{
    LOG_DEBUG("~NamedObject: %s destroyed", _name );
}


NamedObject* NamedObject :: create( const char* name )
{
    // PURPOSE: given a name, create an instance of the
    // associated msg

    NamedObject* p = 0;

#ifdef SUPPORT_NAMED_OBJECTS
#if (CPU!=SIMNT)
    //
    // create NamedMsg objects here
    //  
    if( strcmp( name, "CHwOrders" ) == 0 )
    {
        p = new NamedMsg< CHwOrders >( name );
    }
    else if( strcmp( name, "CHwStates" ) == 0 )
    {
        p = new NamedMsg< CHwStates >( name );
    }
    else if( strcmp( name, "SHwStates" ) == 0 )
    {
        p = new NamedMsg< SHwStates >( name );
    }
    else if( strcmp( name, "SHwOrders" ) == 0 )   // ryn was SHwStates which doesn't make sense
    {
        p = new NamedMsg< SHwOrders >( name );
    }

    //
    // create NamedIntMsg objects here
    //
    else if ( strcmp( name, "APSZeroMsg") == 0)
    {
        p = new NamedIntMsg< APSZeroMsg >( name );
    }
#if 0
    else if ( strcmp( name, "ButtonPressedIDMsg") == 0)
    {
        p = new NamedIntMsg< ButtonPressedIDMsg >( name );
    }
#endif
    else if( strcmp( name, "CassettePositionStatusMsg" ) == 0 )
    {
        p = new NamedIntMsg< CassettePositionStatusMsg >( name );
    }
    else if( strcmp( name, "CentrifugePressureStatusMsg" ) == 0 )
    {
        p = new NamedIntMsg< CentrifugePressureStatusMsg >( name );
    }
    else if( strcmp( name, "CentrifugeStopMsg" ) == 0 )
    {
        p = new NamedIntMsg< CentrifugeStopMsg >( name );
    }
    else if( strcmp( name, "ConfigChangedMsg" ) == 0 )
    {
        p = new NamedIntMsg< ConfigChangedMsg >( name );
    }
    else if( strcmp( name, "DateTimeSetMsg" ) == 0 )
    {
        p = new NamedIntMsg< DateTimeSetMsg >( name );
    }
    else if( strcmp( name, "DeleteProductMsg" ) == 0 )
    {
        p = new NamedIntMsg< DeleteProductMsg >( name );
    }
    else if( strcmp( name, "DisconnectCompleteMsg" ) == 0 )
    {
        p = new NamedIntMsg< DisconnectCompleteMsg >( name );
    }
    else if( strcmp( name, "DisposableIDMsg" ) == 0 )
    {
        p = new NamedIntMsg< DisposableIDMsg >( name );
    }
    else if ( strcmp( name, "DisposableLoweredMsg" ) == 0)
    {
        p = new NamedIntMsg< DisposableLoweredMsg>( name );
    }
    else if( strcmp( name, "DisposableRaisedMsg" ) == 0)
    {
        p = new NamedIntMsg< DisposableRaisedMsg >( name );
    }
    else if ( strcmp( name, "DonorDataStatusMsg") == 0)
    {
        p = new NamedIntMsg< DonorDataStatusMsg >( name );
    }
    else if( strcmp( name, "ProcStateMsg" ) == 0 )
    {
        p = new NamedIntMsg< ProcStateMsg >( name );
    }
    else if ( strcmp( name, "ProcSubstateChangeMsg") == 0)
    {
        p = new NamedIntMsg< ProcSubstateChangeMsg >( name);
    }
    else if ( strcmp( name, "RequestEndRunStatsMsg") == 0)
    {
        p = new NamedIntMsg< RequestEndRunStatsMsg >( name );
    }
    else if( strcmp( name, "RequestSafetyPowerMsg" ) == 0 )
    {
        p = new NamedIntMsg< RequestSafetyPowerMsg >( name );
    }
	else if ( strcmp( name, "SafetyDisconnectStatMsg" ) == 0 )
	{
		p = new NamedIntMsg< SafetyDisconnectStatMsg >( name );  // to test donor disconnect
	}
	else if ( strcmp( name, "SnapScreenshotMsg" ) == 0 )
	{
		p = new NamedIntMsg< SnapScreenshotMsg >( name );
	}
    else if( strcmp( name, "SystemStateChangeMsg" ) == 0 )
    {
        p = new NamedIntMsg< SystemStateChangeMsg >( name );
    }
    else if ( strcmp( name, "SystemStateChangeRequestMsg") == 0)
    {
        p = new NamedIntMsg< SystemStateChangeRequestMsg >( name );
    }
    else if ( strcmp( name, "SafetyTestDoneMsg" ) == 0)
    {
        p = new NamedIntMsg< SafetyTestDoneMsg >( name );
    }

    //
    // create NamedCds objects here
    //
    else if( strcmp( name, "Predict_SubstateInfo" ) == 0 )
    {
        NamedCds< Predict_CDS, Predict_CDS::substate_info_t >* pC;
        pC = new NamedCds< Predict_CDS, Predict_CDS::substate_info_t >( name );
        pC->init( &pC->cds().SubstateInfo );
        p = pC;
    }
    else if( strcmp( name, "HalStatus_aps") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds()._aps );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_cps") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds()._cps );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_inletCmdRpm") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().InletPump._CmdRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_acCmdRpm") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().ACPump._CmdRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_returnCmdRpm") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().ReturnPump._CmdRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_plateletCmdRpm") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().CollectPump._CmdRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_plasmaCmdRpm") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().PlasmaPump._CmdRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_inletActRpm") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().InletPump._ActRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_acActRpm") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().ACPump._ActRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_returnActRpm") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().ReturnPump._ActRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_plateletActRpm") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().CollectPump._ActRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_plasmaActRpm") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().PlasmaPump._ActRPM );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_inletCmdFlow") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().InletPump._CmdFlow );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_acCmdFlow") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().ACPump._CmdFlow );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_returnCmdFlow") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().ReturnPump._CmdFlow );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_plateletCmdFlow") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().CollectPump._CmdFlow );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_plasmaCmdFlow") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds().PlasmaPump._CmdFlow );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_doorStatus") == 0 )
	{
		NamedCds< HalStatus_CDS, HAL_DOORSTATE >* pC;
		pC = new NamedCds< HalStatus_CDS, HAL_DOORSTATE >( name );
		pC->init( &pC->cds()._doorStatus );
		p = pC;
	}
	else if( strcmp( name, "HalStatus_RBCValveStatus") == 0 )
    {
        NamedCds< HalStatus_CDS, HAL_VALVESTATE >* pC;
        pC = new NamedCds< HalStatus_CDS, HAL_VALVESTATE >( name );
        pC->init( &pC->cds().RBCValve._status );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_PlasmaValveStatus") == 0 )
    {
        NamedCds< HalStatus_CDS, HAL_VALVESTATE >* pC;
        pC = new NamedCds< HalStatus_CDS, HAL_VALVESTATE >( name );
        pC->init( &pC->cds().PlasmaValve._status );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_CollectValveStatus") == 0 )
    {
        NamedCds< HalStatus_CDS, HAL_VALVESTATE >* pC;
        pC = new NamedCds< HalStatus_CDS, HAL_VALVESTATE >( name );
        pC->init( &pC->cds().CollectValve._status );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_RBCValveOrder") == 0 )
    {
        NamedCds< HalStatus_CDS, HAL_VALVESTATE >* pC;
        pC = new NamedCds< HalStatus_CDS, HAL_VALVESTATE >( name );
        pC->init( &pC->cds().RBCValve._order );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_PlasmaValveOrder") == 0 )
    {
        NamedCds< HalStatus_CDS, HAL_VALVESTATE >* pC;
        pC = new NamedCds< HalStatus_CDS, HAL_VALVESTATE >( name );
        pC->init( &pC->cds().PlasmaValve._order );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_CollectValveOrder") == 0 )
    {
        NamedCds< HalStatus_CDS, HAL_VALVESTATE >* pC;
        pC = new NamedCds< HalStatus_CDS, HAL_VALVESTATE >( name );
        pC->init( &pC->cds().CollectValve._order );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_redValue") == 0 )
    {
        NamedCds< HalStatus_CDS, short >* pC;
        pC = new NamedCds< HalStatus_CDS, short >( name );
        pC->init( &pC->cds()._redValue );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_greenValue") == 0 )
    {
        NamedCds< HalStatus_CDS, short >* pC;
        pC = new NamedCds< HalStatus_CDS, short >( name );
        pC->init( &pC->cds()._greenValue );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_redDriveValue") == 0 )
    {
        NamedCds< HalStatus_CDS, short >* pC;
        pC = new NamedCds< HalStatus_CDS, short >( name );
        pC->init( &pC->cds()._redDriveValue );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_greenDriveValue") == 0 )
    {
        NamedCds< HalStatus_CDS, short >* pC;
        pC = new NamedCds< HalStatus_CDS, short >( name );
        pC->init( &pC->cds()._greenDriveValue );
        p = pC;
    }	
	else if( strcmp( name, "HalStatus_centActRPM") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds()._centRPM );
        p = pC;
    }	
	else if( strcmp( name, "HalStatus_centCmdRPM") == 0 )
    {
        NamedCds< HalStatus_CDS, float >* pC;
        pC = new NamedCds< HalStatus_CDS, float >( name );
        pC->init( &pC->cds()._centCmdRPM );
        p = pC;
    }	
	else if( strcmp( name, "HalStatus_PS64Vsw_nValue") == 0 )
    {
        NamedCds< HalStatus_CDS, int >* pC;
        pC = new NamedCds< HalStatus_CDS, int >( name );
        pC->init( &pC->cds().PS64Vsw._nValue );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_PS64V_nValue") == 0 )
	{
		NamedCds< HalStatus_CDS, int >* pC;
		pC = new NamedCds< HalStatus_CDS, int >( name );
		pC->init( &pC->cds().PS64V._nValue );
		p = pC;
	}
	else if( strcmp( name, "HalStatus_PS24Vsw_nValue") == 0 )
    {
        NamedCds< HalStatus_CDS, int >* pC;
        pC = new NamedCds< HalStatus_CDS, int >( name );
        pC->init( &pC->cds().PS24Vsw._nValue );
        p = pC;
    }
	else if( strcmp( name, "HalStatus_PS24V_nValue") == 0 )
	{
		NamedCds< HalStatus_CDS, int >* pC;
		pC = new NamedCds< HalStatus_CDS, int >( name );
		pC->init( &pC->cds().PS24V._nValue );
		p = pC;
	}
	else if( strcmp( name, "HalStatus_PSp12V_nValue") == 0 )
	{
		NamedCds< HalStatus_CDS, int >* pC;
		pC = new NamedCds< HalStatus_CDS, int >( name );
		pC->init( &pC->cds().PSp12V._nValue );
		p = pC;
	}
	else if( strcmp( name, "HalStatus_PSm12V_nValue") == 0 )
	{
		NamedCds< HalStatus_CDS, int >* pC;
		pC = new NamedCds< HalStatus_CDS, int >( name );
		pC->init( &pC->cds().PSm12V._nValue );
		p = pC;
	}
	else if( strcmp( name, "HalStatus_PS5V_nValue") == 0 )
	{
		NamedCds< HalStatus_CDS, int >* pC;
		pC = new NamedCds< HalStatus_CDS, int >( name );
		pC->init( &pC->cds().PS5V._nValue );
		p = pC;
	}
	else if( strcmp( name, "ProcRun_CrAv") == 0 )
    {
		NamedCds< ProcRun_CDS, float >* pC;
		pC = new NamedCds< ProcRun_CDS, float >( name );
		pC->init( &pC->cds().CrAv );
		p = pC;
    }
	else if( strcmp( name, "ProcRun_IntegratedPlateletYield") == 0 )
    {
		NamedCds< ProcRun_CDS, float >* pC;
		pC = new NamedCds< ProcRun_CDS, float >( name );
		pC->init( &pC->cds().IntegratedPlateletYield );
		p = pC;
    }
    else if( strcmp( name, "ProcVolumes_Vin") == 0 )
    {
        NamedCds< ProcVolumes_CDS, float >* pC;
        pC = new NamedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().Vin );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_VinTotal") == 0 )
    {
        NamedCds< ProcVolumes_CDS, float >* pC;
        pC = new NamedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().VinTotal );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_Vreturn") == 0 )
    {
        NamedCds< ProcVolumes_CDS, float >* pC;
        pC = new NamedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().Vreturn );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_Vac") == 0 )
    {
        NamedCds< ProcVolumes_CDS, float >* pC;
        pC = new NamedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().Vac );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_Vp") == 0 )
    {
        NamedCds< ProcVolumes_CDS, float >* pC;
        pC = new NamedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().Vp );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_PlateletVolume") == 0 )
    {
        NamedCds< ProcVolumes_CDS, float >* pC;
        pC = new NamedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().PlateletVolume );
        p = pC;
    }
    else if( strcmp( name, "ProcVolumes_ACinPlateletBag") == 0 )
    {
        NamedCds< ProcVolumes_CDS, float >* pC;
        pC = new NamedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().VacPlateletBag );
        p = pC;
    }
	else if( strcmp( name, "ProcVolumes_PlasmaVolume") == 0 )
	{
		NamedCds< ProcVolumes_CDS, float >* pC;
		pC = new NamedCds< ProcVolumes_CDS, float >( name );
		pC->init( &pC->cds().PlasmaVolume );
		p = pC;
	}
	else if( strcmp( name, "ProcVolumes_ACinPlasmaBag") == 0 )
	{
		NamedCds< ProcVolumes_CDS, float >* pC;
		pC = new NamedCds< ProcVolumes_CDS, float >( name );
		pC->init( &pC->cds().VacPlasmaBag );
		p = pC;
	}
	else if( strcmp( name, "ProcVolumes_RBCVolume") == 0 )
	{
		NamedCds< ProcVolumes_CDS, float >* pC;
		pC = new NamedCds< ProcVolumes_CDS, float >( name );
		pC->init( &pC->cds().RBCVolume );
		p = pC;
	}
	else if( strcmp( name, "ProcVolumes_ACinRBCBag") == 0 )
	{
		NamedCds< ProcVolumes_CDS, float >* pC;
		pC = new NamedCds< ProcVolumes_CDS, float >( name );
		pC->init( &pC->cds().VacRBCBag );
		p = pC;
	}
	else if( strcmp( name, "ProcVolumes_RBC1_Volume") == 0 )
	{
		NamedCds< ProcVolumes_CDS, float >* pC;
		pC = new NamedCds< ProcVolumes_CDS, float >( name );
		pC->init( &pC->cds().RBCProduct_A_FinalVolume );
		p = pC;
	}
	else if( strcmp( name, "ProcVolumes_RBC2_Volume") == 0 )
	{
		NamedCds< ProcVolumes_CDS, float >* pC;
		pC = new NamedCds< ProcVolumes_CDS, float >( name );
		pC->init( &pC->cds().RBCProduct_B_FinalVolume );
		p = pC;
	}
	else if( strcmp( name, "ProcVolumes_VSalineBolus") == 0 )
    {
        NamedCds< ProcVolumes_CDS, float >* pC;
        pC = new NamedCds< ProcVolumes_CDS, float >( name );
        pC->init( &pC->cds().VSalineBolus );
        p = pC;
    }
    else if( strcmp( name, "ProcRunTargets_RbcVolumeTarget") == 0 )
    {
        NamedCds< ProcRunTargets_CDS, float >* pC;
        pC = new NamedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().RbcVolumeTarget );
        p = pC;
    }

    else if( strcmp( name, "ProcRunTargets_PlateletVolumeTarget") == 0 )
    {
        NamedCds< ProcRunTargets_CDS, float >* pC;
        pC = new NamedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().PlateletVolumeTarget );
        p = pC;
    }

    else if( strcmp( name, "ProcRunTargets_PlasmaTarget") == 0 )
    {
        NamedCds< ProcRunTargets_CDS, float >* pC;
        pC = new NamedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().PlasmaTarget );
        p = pC;
    }

    else if( strcmp( name, "ProcRunTargets_PlateletYieldTarget") == 0 )
    {
        NamedCds< ProcRunTargets_CDS, float >* pC;
        pC = new NamedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().PlateletYieldTarget );
        p = pC;
    }
	else if( strcmp( name, "ProcRunTargets_SelectedPlateletYield") == 0 )
    {
        NamedCds< ProcRunTargets_CDS, float >* pC;
        pC = new NamedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().SelectedPlateletYield );
        p = pC;
    }
    else if( strcmp( name, "ProcRunTargets_SelectedPlateletVolume") == 0 )
    {
        NamedCds< ProcRunTargets_CDS, float >* pC;
        pC = new NamedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().SelectedPlateletVolume );
        p = pC;
    }
	else if( strcmp( name, "ProcRunTargets_PlateletConcenTarget") == 0 )
    {
        NamedCds< ProcRunTargets_CDS, float >* pC;
        pC = new NamedCds< ProcRunTargets_CDS, float >( name );
        pC->init( &pC->cds().PlateletConcenTarget );
        p = pC;
    }
    else if( strcmp( name, "ProcRun_RedPl") == 0 )
    {
        NamedCds< ProcRun_CDS, int >* pC;
        pC = new NamedCds< ProcRun_CDS, int >( name );
        pC->init( &pC->cds().RedPl );
        p = pC;
    }
    else if( strcmp( name, "ProcCycle_ResHct") == 0 )
    {
        NamedCds< ProcCycle_CDS, float >* pC;
        pC = new NamedCds< ProcCycle_CDS, float >( name );
        pC->init( &pC->cds().ResHct );
        p = pC;
    }
    else if( strcmp( name, "ProcCycle_Vres") == 0 )
    {
        NamedCds< ProcCycle_CDS, float >* pC;
        pC = new NamedCds< ProcCycle_CDS, float >( name );
        pC->init( &pC->cds().Vres );
        p = pC;
    }
    else if( strcmp( name, "ProcCycle_VresLast") == 0 )
    {
        NamedCds< ProcCycle_CDS, float >* pC;
        pC = new NamedCds< ProcCycle_CDS, float >( name );
        pC->init( &pC->cds().VresLast );
        p = pC;
    }
    else if( strcmp( name, "ProcCycle_ACRatio") == 0 )
    {
        NamedCds< ProcCycle_CDS, float >* pC;
        pC = new NamedCds< ProcCycle_CDS, float >( name );
        pC->init( &pC->cds().ACRatio );
        p = pC;
    }
    else if( strcmp( name, "ProcCycle_Vac") == 0 )
    {
        NamedCds< ProcCycle_CDS, float >* pC;
        pC = new NamedCds< ProcCycle_CDS, float >( name );
        pC->init( &pC->cds().Vac );
        p = pC;
    }
    else if( strcmp( name, "ProcTrimaSet_APSZero") == 0 )
    {
        NamedCds< ProcTrimaSet_CDS, float >* pC;
        pC = new NamedCds< ProcTrimaSet_CDS, float >( name );
        pC->init( &pC->cds().APSZero );
        p = pC;
    }
    else if( strcmp( name, "ProcDonorCDS_NadlerTBV") == 0 )
    {
        NamedCds< ProcDonor_CDS, float >* pC;
        pC = new NamedCds< ProcDonor_CDS, float >( name );
        pC->init( &pC->cds().TBV );
        p = pC;
    }
    else if( strcmp( name, "ProcDonorCDS_OgawaTBV") == 0 )
    {
        NamedCds< ProcDonor_CDS, float >* pC;
        pC = new NamedCds< ProcDonor_CDS, float >( name );
        pC->init( &pC->cds().TBVOgawa );
        p = pC;
    }
    else if( strcmp( name, "ProcDonorCDS_Weight") == 0 )
    {
        NamedCds< ProcDonor_CDS, float >* pC;
        pC = new NamedCds< ProcDonor_CDS, float >( name );
        pC->init( &pC->cds().Weight );
        p = pC;
    }
    else if( strcmp( name, "ProcDonorCDS_Hct") == 0 )
    {
        NamedCds< ProcDonor_CDS, float >* pC;
        pC = new NamedCds< ProcDonor_CDS, float >( name );
        pC->init( &pC->cds().Hct );
        p = pC;
    }
    else if( strcmp( name, "ProcDonorCDS_PreCnt") == 0 )
    {
        NamedCds< ProcDonor_CDS, float >* pC;
        pC = new NamedCds< ProcDonor_CDS, float >( name );
        pC->init( &pC->cds().PreCnt );
        p = pC;
    }
    else if( strcmp(name, "ProcPQI_spoofChamberSat") == 0 )
    {
        NamedCds< ProcPQI_CDS, int >* pC;
        pC = new NamedCds< ProcPQI_CDS, int >(name);
        pC->init(&pC->cds().spoofChamberSat);
        p = pC;
    }

#endif // #if (CPU!=SIMNT)
#endif // SUPPORT_NAMED_OBJECTS

    if (p)
       LOG_DEBUG("create: %s created", name );
    else
       LOG_DEBUG("create: name not found: %s", name );


    return p;
}

bool NamedObject :: getExactData(const SpoofableData* SpoofableData, void* buf, int timeoutMsec, int matchValue )
{
    LOG_DEBUG("No derived getExactData exists to wait for exact value so using normal getData" );
    LOG_INFO( "No derived getExactData exists to wait for exact value so using normal getData" );
    return getData( SpoofableData, buf, timeoutMsec);
}

