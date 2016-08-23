/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.6  2003/04/03 23:00:54Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.5  2002/08/07 17:46:58Z  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Revision 1.4  2002/07/23 16:52:15  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.3  2002/04/11 15:28:29  pn02526
 * "Revise logging to use DataLogLevel class."
 * Revision 1.2  2001/07/18 11:10:04  ms10234
 * 4712 - Fixed return/draw switching problem
 * Revision 1.1  2000/03/15 18:28:41  BS04481
 * Initial revision
 * TITLE:      procop.hpp
 *             Safety class to capture procedure's stops and starts.
 *
 *
 * ABSTRACT:   This file contains the class definition and method for
 *             an object the monitors the starting and stopping of the
 *             pumps and centrifuge
 *
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/

#ifndef PROCOP_HPP  //  Prevents multiple compilations.
#define PROCOP_HPP

#include <stdio.h>
#include <stdlib.h>

#include "trima_datalog.h"
#include "s_glob.hpp"

enum SYSTEM_OP_STATUS
{
   SYSTEM_IS_RUNNING,
   SYSTEM_IS_PAUSED,
   SYSTEM_IS_STOPPED,
   SYSTEM_IS_PRE_DONOR_CONNECT
};

/*******************************************************************************
 *
 *  aProcOpMonitor_Safety CLASS DEFINITION
 *
 ******************************************************************************/
// this class keeps track of what procedure is commanding the system state to
// at any given time.
class aProcOpMonitor_Safety
{
private:
   // current order information
   float            _orderInlet;
   float            _orderReturn;
   float            _orderAC;
   float            _orderPlatelet;
   float            _orderPlasma;
   float            _orderCentrifuge;
   SYSTEM_OP_STATUS _op;
public:
   aProcOpMonitor_Safety()
   {
      _orderInlet      = 0.0f;
      _orderReturn     = 0.0f;
      _orderAC         = 0.0f;
      _orderPlatelet   = 0.0f;
      _orderPlasma     = 0.0f;
      _orderCentrifuge = 0.0f;
      _op              = SYSTEM_IS_PRE_DONOR_CONNECT;

   }

   ~aProcOpMonitor_Safety()
   {}

   // check the procedure orders and keep track of what
   // is being commanded
   void Update (CHwOrders orders)
   {

      _orderInlet      = orders.inletRPM;
      _orderReturn     = orders.returnRPM;
      _orderAC         = orders.acRPM;
      _orderPlatelet   = orders.plateletRPM;
      _orderPlasma     = orders.plasmaRPM;
      _orderCentrifuge = orders.centrifugeRPM;

      if (   (_orderInlet == 0.0f)
             &&(_orderReturn == 0.0f)
             &&(_orderAC == 0.0f)
             &&(_orderPlatelet == 0.0f)
             &&(_orderPlasma == 0.0f)
             &&(_orderCentrifuge != 0.0f) )
      {
         // pumps are paused
         if ( _op != SYSTEM_IS_PAUSED )
         {
            _op = SYSTEM_IS_PAUSED;
            DataLog(log_level_safe_exec_info) << "Safety sees Control ordering System PAUSED" << endmsg;
         }
      }
      else if (   (_orderInlet == 0.0f)
                  &&(_orderReturn == 0.0f)
                  &&(_orderAC == 0.0f)
                  &&(_orderPlatelet == 0.0f)
                  &&(_orderPlasma == 0.0f)
                  &&(_orderCentrifuge == 0.0f) )
      {
         // pumps and centrifuge are stopped
         if ( _op != SYSTEM_IS_STOPPED )
         {
            _op = SYSTEM_IS_STOPPED;
            DataLog(log_level_safe_exec_info) << "Safety sees Control ordering System STOPPED" << endmsg;
         }
      }
      else if (   (_orderInlet != 0.0f)
                  ||(_orderReturn != 0.0f)
                  ||(_orderAC != 0.0f)
                  ||(_orderPlatelet != 0.0f)
                  ||(_orderPlasma != 0.0f) )
      {
         if ( _op != SYSTEM_IS_RUNNING )
         {
            _op = SYSTEM_IS_RUNNING;
            DataLog(log_level_safe_exec_info) << "Safety sees Control start System RUNNING" << endmsg;
         }
      }
   }

   SYSTEM_OP_STATUS GetOperationalStatus ()
   {
      return(_op);
   }

}; // END aProcOpMonitor_Safety class definition

#endif // PROCOP_HPP

/* FORMAT HASH ba2c24eb29e7ac75cc573271a68358ad */
