/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/low_level/ctl_drv/rcs/cfastmsg.hpp 1.5 2003/06/19 19:43:18Z jl11312 Exp $
 * $Log: cfastmsg.hpp $
 * Revision 1.5  2003/06/19 19:43:18Z  jl11312
 * - moved hardware interface to separate modules so that it is available earlier in the boot process
 * Revision 1.4  2002/08/15 21:10:05Z  jl11312
 * - updates for message system changes
 * Revision 1.3  2002/06/13 17:06:47  jl11312
 * - initial vxWorks port changes
 * Revision 1.2  2000/10/11 22:10:11  bs04481
 * Add Speed Order to the pump 10ms data.  Rearrange enumeration.
 * Revision 1.1  2000/08/07 23:01:25  bs04481
 * Initial revision
 *
 * TITLE:      cfastmsg.hpp Trima task to manage optional and low-prioirty
 *             messages used by the control driver.
 *
 * ABSTRACT:
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 */

#ifndef _CFASTMSG_INCLUDE
#define _CFASTMSG_INCLUDE

#include "fast_msg.hpp"
#include "fastdata.h"
#include "message.h"
#include "msginfo.hpp"
#include "timermessage.h"
#include "trimamessages.h"

// SPECIFICATION:    fastDataControl class
//                   Receieves request for fast data types
//                   and enables or disables message flow.  Also
//							handles periodic update of fast data buffers.
//
// ERROR HANDLING:   none.

class fastDataControl
{
public:
   fastDataControl(void);
   ~fastDataControl();

   void update (void);
   void handleEnableMessage (void);

private:
   TimerMessage         _updateTimer;
   EnableFastDataMsg    _enableMessage;

   fastdataMessage*     _fastaps;                       // APS 20ms data

   fastdatalongMessage* _fastinletencoders;         // inlet pump 10ms data
   fastdatalongMessage* _fastinletorders;           // inlet pump 10ms data
   fastdatalongMessage* _fastinletinterr;           // inlet pump 10ms data
   fastdatalongMessage* _fastinletvolerr;           // inlet pump 10ms data
   fastdatalongMessage* _fastinletdeltaspd;         // inlet pump 10ms data
   fastdatalongMessage* _fastinleterrors;           // inlet pump 10ms data
   fastdatalongMessage* _fastinletspdorder;         // inlet pump 10ms data

   fastdatalongMessage* _fastacencoders;                // ac pump 10ms data
   fastdatalongMessage* _fastacorders;                  // ac pump 10ms data
   fastdatalongMessage* _fastacinterr;                  // ac pump 10ms data
   fastdatalongMessage* _fastacvolerr;                  // ac pump 10ms data
   fastdatalongMessage* _fastacdeltaspd;                // ac pump 10ms data
   fastdatalongMessage* _fastacerrors;                  // ac pump 10ms data
   fastdatalongMessage* _fastacspdorder;                // ac pump 10ms data

   fastdatalongMessage* _fastplasmaencoders;        // plasma pump 10ms data
   fastdatalongMessage* _fastplasmaorders;          // plasma pump 10ms data
   fastdatalongMessage* _fastplasmainterr;          // plasma pump 10ms data
   fastdatalongMessage* _fastplasmavolerr;          // plasma pump 10ms data
   fastdatalongMessage* _fastplasmadeltaspd;        // plasma pump 10ms data
   fastdatalongMessage* _fastplasmaerrors;          // plasma pump 10ms data
   fastdatalongMessage* _fastplasmaspdorder;        // plasma pump 10ms data

   fastdatalongMessage* _fastplateletencoders;      // platelet pump 10ms data
   fastdatalongMessage* _fastplateletorders;        // platelet pump 10ms data
   fastdatalongMessage* _fastplateletinterr;        // platelet pump 10ms data
   fastdatalongMessage* _fastplateletvolerr;        // platelet pump 10ms data
   fastdatalongMessage* _fastplateletdeltaspd;      // platelet pump 10ms data
   fastdatalongMessage* _fastplateleterrors;        // platelet pump 10ms data
   fastdatalongMessage* _fastplateletspdorder;      // platelet pump 10ms data

   fastdatalongMessage* _fastreturnencoders;        // return pump 10ms data
   fastdatalongMessage* _fastreturnorders;          // return pump 10ms data
   fastdatalongMessage* _fastreturninterr;          // return pump 10ms data
   fastdatalongMessage* _fastreturnvolerr;          // return pump 10ms data
   fastdatalongMessage* _fastreturndeltaspd;        // return pump 10ms data
   fastdatalongMessage* _fastreturnerrors;          // return pump 10ms data
   fastdatalongMessage* _fastreturnspdorder;        // return pump 10ms data

   fastdataMessage*     _fastusair;                 // ultrasonic 10ms data
   fastdataMessage*     _fastusfluid;               // ultrasonic 10ms data
   fastdataMessage*     _fastlsair;                 // ultrasonic 10ms data
   fastdataMessage*     _fastlsfluid;               // ultrasonic 10ms data

   fastdatalongMessage* _fastcentrawencoders;      // centrifuge 10ms data
   fastdatalongMessage* _fastcentavgencoders;      // centrifuge 10ms data
   fastdatalongMessage* _fastcentinternalorder;    // centrifuge 10ms data
   fastdatalongMessage* _fastcentdeltarpm;         // centrifuge 10ms data
   fastdatalongMessage* _fastcentintegral;         // centrifuge 10ms data
   fastdatalongMessage* _fastcentcontrol;          // centrifuge 10ms data
   fastdatalongMessage* _fastcenterrors;           // centrifuge 10ms data
};

#endif /* ifndef _CFASTMSG_INCLUDE */

/* FORMAT HASH e55d99c713c9466ddcb10ceb37e25020 */
