/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/safety/rcs/s_pmsg.hpp 1.13 2008/04/21 19:21:43Z jheiusb Exp $
 * $Log: s_pmsg.hpp $
 * Revision 1.13  2008/04/21 19:21:43Z  jheiusb
 * fix MKS branching
 * Revision 1.10.1.2  2008/03/27 17:09:09Z  jheiusb
 * fix to mks head
 * Revision 1.10.1.1  2007/10/17 17:18:00Z  spriusb
 * Rolling back dRBC auto-split changes
 * Revision 1.10  2003/07/01 22:34:58Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.9  2003/04/04 15:39:39Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.8  2003/01/02 16:06:24Z  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.7  2002/08/08 11:35:29  pn02526
 * Substitute #include of trimamessages.h for obsolete message system files.
 * Remove "trace" external; use DataLog_Default instead.
 * Rather than deriving from an INT32 message base class:
 * 1. Add aDisposableIDMsg_rcvr :: _theDisposableIDMsg,
 * 2. Add aMidCycleSwitchMsg_rcvr :: _theMidCycleSwitchMsg,
 * 3. Add aNotifySafetyOfA2Msg_rcvr :: _theNotifySafetyOfA2Msg,
 * +     all to use the new buffered message class definition.
 * Revision 1.6  2002/07/23 16:52:19  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.5  2002/04/11 17:11:50  pn02526
 * Revise logging to use DataLogLevel class.  Change ASSERT() macro calls to trima_assert().
 * Revision 1.4  2001/04/11 14:55:37Z  ms10234
 * IT5031 - Allow reception of cycle switch message in rinseback.
 * Revision 1.3  2001/01/11 00:07:36  ms10234
 * IT4663 - Changed safety to not respond to the cassette ID message if
 * it contains the NOT_LOADED type.
 * Revision 1.2  2000/08/01 20:48:49  pn02526
 * This change effects the solution to IT3633.
 * When Proc notifies saf_exec of an A2 ,
 * power is shut off on the 24 and 64 V lines,
 * and a message is logged.
 * Revision 1.1  2000/07/11 23:50:52  bs04481
 * Initial revision
 *
 *
 * TITLE:      s_pmsg.hpp
 *
 *
 * ABSTRACT:   Several one-time messages received from procedure.
 *                aDisposableIDMsg
 *                aMidCycleSwitchMsg
 *
 */

#ifndef S_PMSG_HPP  //  Prevents multiple compilations.
#define S_PMSG_HPP

#include "trimamessages.h"
#include "s_res.hpp"

// SPECIFICATION:    "rcvr" for DisposableID
//
// NOTES :
//
// ERROR HANDLING:   none.

class aDisposableIDMsg_rcvr
{
private:
   aReservoir_Safety* _paReservoir;
   DisposableIDMsg    _theDisposableIDMsg;

   aDisposableIDMsg_rcvr();                                          // not implemented
   aDisposableIDMsg_rcvr(aDisposableIDMsg_rcvr const&);              // not implemented
   aDisposableIDMsg_rcvr& operator = (aDisposableIDMsg_rcvr const&); // not implemented

public:
   aDisposableIDMsg_rcvr(aReservoir_Safety* paReservoir);
   virtual ~aDisposableIDMsg_rcvr();
   void notify ();

}; // END aDisposableIDMsg_rcvr class definition


// SPECIFICATION:    "rcvr" for MidCycleSwitch
//
// NOTES :
//
// ERROR HANDLING:   none.

class aMidCycleSwitchMsg_rcvr
{
private:
   aReservoir_Safety* _paReservoir;
   MidCycleSwitchMsg  _theMidCycleSwitchMsg;
   aMidCycleSwitchMsg_rcvr();                                            // not implemented
   aMidCycleSwitchMsg_rcvr(aMidCycleSwitchMsg_rcvr const&);              // not implemented
   aMidCycleSwitchMsg_rcvr& operator = (aMidCycleSwitchMsg_rcvr const&); // not implemented

public:
   aMidCycleSwitchMsg_rcvr(aReservoir_Safety* paReservoir);
   virtual ~aMidCycleSwitchMsg_rcvr();
   void notify ();

}; // END aMidCycleSwitchMsg_rcvr class definition


// SPECIFICATION:    "rcvr" for NotifySafetyOfA2
//
// NOTES :
//
// ERROR HANDLING:   none.

class aNotifySafetyOfA2Msg_rcvr
{
private:
   NotifySafetyOfA2Msg _theNotifySafetyOfA2Msg;

   aNotifySafetyOfA2Msg_rcvr(aNotifySafetyOfA2Msg_rcvr const&);                 // not implemented
   aNotifySafetyOfA2Msg_rcvr& operator = (aNotifySafetyOfA2Msg_rcvr const&);    // not implemented

public:
   aNotifySafetyOfA2Msg_rcvr();
   virtual ~aNotifySafetyOfA2Msg_rcvr();

   void notify ();

}; // END aNotifySafetyOfA2Msg_rcvr class definition



#endif /* S_PMSG_HPP */

/* FORMAT HASH 8d218da3de4268fcde57c1167bfe581e */
