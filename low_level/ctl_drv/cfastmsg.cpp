/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/low_level/ctl_drv/rcs/cfastmsg.cpp 1.5 2003/03/28 20:01:31Z jl11312 Exp $
 * $Log: cfastmsg.cpp $
 * Revision 1.5  2003/03/28 20:01:31Z  jl11312
 * - modified to use new data log levels
 * Revision 1.4  2002/06/13 17:06:46Z  jl11312
 * - initial vxWorks port changes
 * Revision 1.3  2001/08/21 15:18:31  ms10234
 * 5268 - Use new trima common sys library.
 * Revision 1.2  2000/10/11 22:10:07  bs04481
 * Add Speed Order to the pump 10ms data.  Rearrange enumeration.
 * Revision 1.1  2000/08/07 23:01:33  bs04481
 * Initial revision
 *
 * TITLE:      cfastmsg.cpp Trima task to manage optional and low-priority
 *             messages used by the control driver
 *
 * ABSTRACT:
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 *
 * PUBLIC FUNCTIONS:
 * main() - main entry point
 *
 */

#include <vxWorks.h>

#include "cfastmsg.hpp"
#include "messagesystem.h"
#include "trima_assert.h"
#include "trima_datalog.h"

// SPECIFICATION:    derived message class, enables/disables fast messages
//

// SPECIFICATION:    fastDataControl constructor
//
// ERROR HANDLING:   None.

fastDataControl::fastDataControl(void)
   : _updateTimer(500, Callback<fastDataControl>(this, &fastDataControl::update)),
     _enableMessage(Callback<fastDataControl>(this, &fastDataControl::handleEnableMessage), MessageBase::SNDRCV_RECEIVE_ONLY)
{
   _fastaps               = new fastdataMessage("aps");
   _fastinletencoders     = new fastdatalongMessage("inletPumpEncoders");
   _fastinletorders       = new fastdatalongMessage("inletPumpOrders");
   _fastinletinterr       = new fastdatalongMessage("inletPumpIntErr");
   _fastinletvolerr       = new fastdatalongMessage("inletPumpVolErr");
   _fastinletdeltaspd     = new fastdatalongMessage("inletPumpDeltaSpd");
   _fastinleterrors       = new fastdatalongMessage("inletPumpErrors");
   _fastinletspdorder     = new fastdatalongMessage("inletPumpSpdOrder");

   _fastacencoders        = new fastdatalongMessage("acPumpEncoders");
   _fastacorders          = new fastdatalongMessage("acPumpOrders");
   _fastacinterr          = new fastdatalongMessage("acPumpIntErr");
   _fastacvolerr          = new fastdatalongMessage("acPumpVolErr");
   _fastacdeltaspd        = new fastdatalongMessage("acPumpDeltaSpd");
   _fastacerrors          = new fastdatalongMessage("acPumpErrors");
   _fastacspdorder        = new fastdatalongMessage("acPumpSpdOrder");

   _fastplasmaencoders    = new fastdatalongMessage("plasmaPumpEncoders");
   _fastplasmaorders      = new fastdatalongMessage("plasmaPumpOrders");
   _fastplasmainterr      = new fastdatalongMessage("plasmaPumpIntErr");
   _fastplasmavolerr      = new fastdatalongMessage("plasmaPumpVolErr");
   _fastplasmadeltaspd    = new fastdatalongMessage("plasmaPumpDeltaSpd");
   _fastplasmaerrors      = new fastdatalongMessage("plasmaPumpErrors");
   _fastplasmaspdorder    = new fastdatalongMessage("plasmaPumpSpdOrder");

   _fastplateletencoders  = new fastdatalongMessage("plateletPumpEncoders");
   _fastplateletorders    = new fastdatalongMessage("plateletPumpOrders");
   _fastplateletinterr    = new fastdatalongMessage("plateletPumpIntErr");
   _fastplateletvolerr    = new fastdatalongMessage("plateletPumpVolErr");
   _fastplateletdeltaspd  = new fastdatalongMessage("plateletPumpDeltaSpd");
   _fastplateleterrors    = new fastdatalongMessage("plateletPumpErrors");
   _fastplateletspdorder  = new fastdatalongMessage("plateletPumpSpdOrder");

   _fastreturnencoders    = new fastdatalongMessage("returnPumpEncoders");
   _fastreturnorders      = new fastdatalongMessage("returnPumpOrders");
   _fastreturninterr      = new fastdatalongMessage("returnPumpIntErr");
   _fastreturnvolerr      = new fastdatalongMessage("returnPumpVolErr");
   _fastreturndeltaspd    = new fastdatalongMessage("returnPumpDeltaSpd");
   _fastreturnerrors      = new fastdatalongMessage("returnPumpErrors");
   _fastreturnspdorder    = new fastdatalongMessage("returnPumpSpdOrder");

   _fastusair             = new fastdataMessage("controlUSAir");
   _fastusfluid           = new fastdataMessage("controlUSFluid");
   _fastlsair             = new fastdataMessage("controlLSAir");
   _fastlsfluid           = new fastdataMessage("controlLSFluid");

   _fastcentrawencoders   = new fastdatalongMessage("centRawEncoders");
   _fastcentavgencoders   = new fastdatalongMessage("centAvgEncoders");
   _fastcentinternalorder = new fastdatalongMessage("centInternalOrder");
   _fastcentdeltarpm      = new fastdatalongMessage("centDeltaRPM");
   _fastcentintegral      = new fastdatalongMessage("centIntegral");
   _fastcentcontrol       = new fastdatalongMessage("centControl");
   _fastcenterrors        = new fastdatalongMessage("centErrors");

   // always enable this one because the occlusion monitor needs the data
   _fastaps->activate();
}

// SPECIFICATION:    fastDataControl denstructor
//
// ERROR HANDLING:   None.

fastDataControl::~fastDataControl()                   // destructor
{
   //
   // Stop update timer before deleting data items
   //
   _updateTimer.armTimer(TimerMessage::DISARMED);

   //
   // Delete all fast data items
   //
   if (_fastaps != NULL)
      delete _fastaps;
   _fastaps = NULL;

   if (_fastinletencoders != NULL)
      delete _fastinletencoders;
   _fastinletencoders = NULL;

   if (_fastinletorders != NULL)
      delete _fastinletorders;
   _fastinletorders = NULL;

   if (_fastinletinterr != NULL)
      delete _fastinletinterr;
   _fastinletinterr = NULL;

   if (_fastinletvolerr != NULL)
      delete _fastinletvolerr;
   _fastinletvolerr = NULL;

   if (_fastinletdeltaspd != NULL)
      delete _fastinletdeltaspd;
   _fastinletdeltaspd = NULL;

   if (_fastinleterrors != NULL)
      delete _fastinleterrors;
   _fastinleterrors = NULL;

   if (_fastinletspdorder != NULL)
      delete _fastinletspdorder;
   _fastinletspdorder = NULL;

   if (_fastacencoders != NULL)
      delete _fastacencoders;
   _fastacencoders = NULL;

   if (_fastacorders != NULL)
      delete _fastacorders;
   _fastacorders = NULL;

   if (_fastacinterr != NULL)
      delete _fastacinterr;
   _fastacinterr = NULL;

   if (_fastacvolerr != NULL)
      delete _fastacvolerr;
   _fastacvolerr = NULL;

   if (_fastacdeltaspd != NULL)
      delete _fastacdeltaspd;
   _fastacdeltaspd = NULL;

   if (_fastacerrors != NULL)
      delete _fastacerrors;
   _fastacerrors = NULL;

   if (_fastacspdorder != NULL)
      delete _fastacspdorder;
   _fastacspdorder = NULL;

   if (_fastplasmaencoders != NULL)
      delete _fastplasmaencoders;
   _fastplasmaencoders = NULL;

   if (_fastplasmaorders != NULL)
      delete _fastplasmaorders;
   _fastplasmaorders = NULL;

   if (_fastplasmainterr != NULL)
      delete _fastplasmainterr;
   _fastplasmainterr = NULL;

   if (_fastplasmavolerr != NULL)
      delete _fastplasmavolerr;
   _fastplasmavolerr = NULL;

   if (_fastplasmadeltaspd != NULL)
      delete _fastplasmadeltaspd;
   _fastplasmadeltaspd = NULL;

   if (_fastplasmaerrors != NULL)
      delete _fastplasmaerrors;
   _fastplasmaerrors = NULL;

   if (_fastplasmaspdorder != NULL)
      delete _fastplasmaspdorder;
   _fastplasmaspdorder = NULL;

   if (_fastplateletencoders != NULL)
      delete _fastplateletencoders;
   _fastplateletencoders = NULL;

   if (_fastplateletorders != NULL)
      delete _fastplateletorders;
   _fastplateletorders = NULL;

   if (_fastplateletinterr != NULL)
      delete _fastplateletinterr;
   _fastplateletinterr = NULL;

   if (_fastplateletvolerr != NULL)
      delete _fastplateletvolerr;
   _fastplateletvolerr = NULL;

   if (_fastplateletdeltaspd != NULL)
      delete _fastplateletdeltaspd;
   _fastplateletdeltaspd = NULL;

   if (_fastplateleterrors != NULL)
      delete _fastplateleterrors;
   _fastplateleterrors = NULL;

   if (_fastplateletspdorder != NULL)
      delete _fastplateletspdorder;
   _fastplateletspdorder = NULL;

   if (_fastreturnencoders != NULL)
      delete _fastreturnencoders;
   _fastreturnencoders = NULL;

   if (_fastreturnorders != NULL)
      delete _fastreturnorders;
   _fastreturnorders = NULL;

   if (_fastreturninterr != NULL)
      delete _fastreturninterr;
   _fastreturninterr = NULL;

   if (_fastreturnvolerr != NULL)
      delete _fastreturnvolerr;
   _fastreturnvolerr = NULL;

   if (_fastreturndeltaspd != NULL)
      delete _fastreturndeltaspd;
   _fastreturndeltaspd = NULL;

   if (_fastreturnerrors != NULL)
      delete _fastreturnerrors;
   _fastreturnerrors = NULL;

   if (_fastreturnspdorder != NULL)
      delete _fastreturnspdorder;
   _fastreturnspdorder = NULL;

   if (_fastusair != NULL)
      delete _fastusair;
   _fastusair = NULL;

   if (_fastusfluid != NULL)
      delete _fastusfluid;
   _fastusfluid = NULL;

   if (_fastlsair != NULL)
      delete _fastlsair;
   _fastlsair = NULL;

   if (_fastlsfluid != NULL)
      delete _fastlsfluid;
   _fastlsfluid = NULL;

   if (_fastcentrawencoders != NULL)
      delete _fastcentrawencoders;
   _fastcentrawencoders = NULL;

   if (_fastcentavgencoders != NULL)
      delete _fastcentavgencoders;
   _fastcentavgencoders = NULL;

   if (_fastcentinternalorder != NULL)
      delete _fastcentinternalorder;
   _fastcentinternalorder = NULL;

   if (_fastcentdeltarpm != NULL)
      delete _fastcentdeltarpm;
   _fastcentdeltarpm = NULL;

   if (_fastcentintegral != NULL)
      delete _fastcentintegral;
   _fastcentintegral = NULL;

   if (_fastcentcontrol != NULL)
      delete _fastcentcontrol;
   _fastcentcontrol = NULL;

   if (_fastcenterrors != NULL)
      delete _fastcenterrors;
   _fastcenterrors = NULL;
}

void fastDataControl::handleEnableMessage (void)
{
   DATA_SOURCE src;

   src = (DATA_SOURCE)_enableMessage.getData();

   switch (src)
   {
      case CONTROL_ACCESS_PRESSURE :
         DataLog_Default << "Ignore fastDataControl.  Won't control APS data" << endmsg;
         break;
      case CONTROL_UPPER_US_AIR :
         _fastusair->toggle();
         break;
      case CONTROL_UPPER_US_FLUID :
         _fastusfluid->toggle();
         break;
      case CONTROL_LOWER_US_AIR :
         _fastlsair->toggle();
         break;
      case CONTROL_LOWER_US_FLUID :
         _fastlsfluid->toggle();
         break;
      case CENT_RAW_ENCODER :
         _fastcentrawencoders->toggle();
         break;
      case CENT_AVG_ENCODER :
         _fastcentavgencoders->toggle();
         break;
      case CENT_INTERNAL_ORDER :
         _fastcentinternalorder->toggle();
         break;
      case CENT_DELTA_RPM :
         _fastcentdeltarpm->toggle();
         break;
      case CENT_INTEGRAL :
         _fastcentintegral->toggle();
         break;
      case CENT_CONTROL :
         _fastcentcontrol->toggle();
         break;
      case CENT_ERRORS :
         _fastcenterrors->toggle();
         break;
      case AC_PUMP_ENCODER :
         _fastacencoders->toggle();
         break;
      case INLET_PUMP_ENCODER :
         _fastinletencoders->toggle();
         break;
      case PLASMA_PUMP_ENCODER :
         _fastplasmaencoders->toggle();
         break;
      case PLATELET_PUMP_ENCODER :
         _fastplateletencoders->toggle();
         break;
      case RETURN_PUMP_ENCODER :
         _fastreturnencoders->toggle();
         break;
      case AC_PUMP_ORDER :
         _fastacorders->toggle();
         break;
      case INLET_PUMP_ORDER :
         _fastinletorders->toggle();
         break;
      case PLASMA_PUMP_ORDER :
         _fastplasmaorders->toggle();
         break;
      case PLATELET_PUMP_ORDER :
         _fastplateletorders->toggle();
         break;
      case RETURN_PUMP_ORDER :
         _fastreturnorders->toggle();
         break;
      case AC_PUMP_INTEGRATION_ERROR :
         _fastacinterr->toggle();
         break;
      case INLET_PUMP_INTEGRATION_ERROR :
         _fastinletinterr->toggle();
         break;
      case PLASMA_PUMP_INTEGRATION_ERROR :
         _fastplasmainterr->toggle();
         break;
      case PLATELET_PUMP_INTEGRATION_ERROR :
         _fastplateletinterr->toggle();
         break;
      case RETURN_PUMP_INTEGRATION_ERROR :
         _fastreturninterr->toggle();
         break;
      case AC_PUMP_VOLUME_ERROR :
         _fastacvolerr->toggle();
         break;
      case INLET_PUMP_VOLUME_ERROR :
         _fastinletvolerr->toggle();
         break;
      case PLASMA_PUMP_VOLUME_ERROR :
         _fastplasmavolerr->toggle();
         break;
      case PLATELET_PUMP_VOLUME_ERROR :
         _fastplateletvolerr->toggle();
         break;
      case RETURN_PUMP_VOLUME_ERROR :
         _fastreturnvolerr->toggle();
         break;
      case AC_PUMP_DELTA_SPEED :
         _fastacdeltaspd->toggle();
         break;
      case INLET_PUMP_DELTA_SPEED :
         _fastinletdeltaspd->toggle();
         break;
      case PLASMA_PUMP_DELTA_SPEED :
         _fastplasmadeltaspd->toggle();
         break;
      case PLATELET_PUMP_DELTA_SPEED :
         _fastplateletdeltaspd->toggle();
         break;
      case RETURN_PUMP_DELTA_SPEED :
         _fastreturndeltaspd->toggle();
         break;
      case AC_PUMP_ERRORS :
         _fastacerrors->toggle();
         break;
      case INLET_PUMP_ERRORS :
         _fastinleterrors->toggle();
         break;
      case PLASMA_PUMP_ERRORS :
         _fastplasmaerrors->toggle();
         break;
      case PLATELET_PUMP_ERRORS :
         _fastplateleterrors->toggle();
         break;
      case RETURN_PUMP_ERRORS :
         _fastreturnerrors->toggle();
         break;
      case AC_PUMP_SPEED_ORDER :
         _fastacspdorder->toggle();
         break;
      case INLET_PUMP_SPEED_ORDER :
         _fastinletspdorder->toggle();
         break;
      case PLASMA_PUMP_SPEED_ORDER :
         _fastplasmaspdorder->toggle();
         break;
      case PLATELET_PUMP_SPEED_ORDER :
         _fastplateletspdorder->toggle();
         break;
      case RETURN_PUMP_SPEED_ORDER :
         _fastreturnspdorder->toggle();
         break;
      default :
         DataLog_Default << "unrecognized fastDataControl: src=" << src << endmsg;
         break;
   }
}

void fastDataControl::update (void)
{
   _fastaps->update();

   _fastinletencoders->update();
   _fastinletorders->update();
   _fastinletinterr->update();
   _fastinletvolerr->update();
   _fastinletdeltaspd->update();
   _fastinleterrors->update();
   _fastinletspdorder->update();

   _fastacencoders->update();
   _fastacorders->update();
   _fastacinterr->update();
   _fastacvolerr->update();
   _fastacdeltaspd->update();
   _fastacerrors->update();
   _fastacspdorder->update();

   _fastplasmaencoders->update();
   _fastplasmaorders->update();
   _fastplasmainterr->update();
   _fastplasmavolerr->update();
   _fastplasmadeltaspd->update();
   _fastplasmaerrors->update();
   _fastplasmaspdorder->update();

   _fastplateletencoders->update();
   _fastplateletorders->update();
   _fastplateletinterr->update();
   _fastplateletvolerr->update();
   _fastplateletdeltaspd->update();
   _fastplateleterrors->update();
   _fastplateletspdorder->update();

   _fastreturnencoders->update();
   _fastreturnorders->update();
   _fastreturninterr->update();
   _fastreturnvolerr->update();
   _fastreturndeltaspd->update();
   _fastreturnerrors->update();
   _fastreturnspdorder->update();

   _fastusair->update();
   _fastusfluid->update();
   _fastlsair->update();
   _fastlsfluid->update();

   _fastcentrawencoders->update();
   _fastcentavgencoders->update();
   _fastcentinternalorder->update();
   _fastcentdeltarpm->update();
   _fastcentintegral->update();
   _fastcentcontrol->update();
   _fastcenterrors->update();
}


// SPECIFICATION:    main entry point
//
// ERROR HANDLING:   none.

extern "C" void cfastmsg (void)
{
   datalog_SetDefaultLevel(log_handle_cfastmsg_info);

   MessageSystem msgSystem;
   msgSystem.initBlocking("cfast_msg");

   fastDataControl ctrl;

   // enter main message loop
   msgSystem.dispatchMessages();
}

/* FORMAT HASH 6dffbf4ca7aacb49a7a3e624d75b35f4 */
