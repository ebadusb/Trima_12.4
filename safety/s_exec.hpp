/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      s_exec.hpp
 *             Safety Executive class definitions.
 *
 * ABSTRACT:   This file contains the class definitions for safety devices
 *             door, cassette, valves, and switches
 *
 *
 ******************************************************************************/

#ifndef S_EXEC_HPP  //  Prevents multiple compilations.
#define S_EXEC_HPP

#include <taskLib.h>
#include <vector>

#include "trima_assert.h"
#include "trima_datalog.h"

#include "message.h"

#include "chw.hpp"
#include "shw.hpp"

#include "alarms.h"

#include "s_pfr.hpp"
#include "s_state.hpp"
#include "s_res.hpp"
#include "s_cont.hpp"
#include "s_power.hpp"
#include "s_donor.hpp"
#include "ostime.hpp"
#include "s_set.hpp"

#include "state_objects.hpp"
#include "s_monitor_group.hpp"
#include "stroke_calc.hpp"

#include "safety_warm_ram.h"


#define T1_NOT_COMPLETE                      0                      // aggregate status flag for all T1 tests
#define T1_COMPLETE_NOMINAL                  1                      // all T1 Completed nominally.
#define T1_FAILURE_BYPASS_ALLOWED            2                      // TESTING STATE


#define ENCODER_TICKS_PER_REV   20608L

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

class SafetyExecData
{
public:
   SafetyExecData(void)
   {
      // initialize data stores
      memset(&control, 0, sizeof(ControlStatus_Export));
      memset(&safety, 0, sizeof(SafetyStatus_Export));
      memset(&orders, 0, sizeof(ControlCommand_Export));
      memset(&derived, 0, sizeof(SafetyDerived));

   }

   SafetyExecData(SafetyExecData const& original)
   {
      control = original.control;
      safety  = original.safety;
      orders  = original.orders;
      derived = original.derived;
   }

   virtual ~SafetyExecData(){ }

   ControlStatus_Export  control;
   SafetyStatus_Export   safety;
   ControlCommand_Export orders;
   SafetyDerived         derived;
};



class SafetyExec
{
public:
   SafetyExec(void);

   virtual ~SafetyExec() {delete _strokeCalculatorP; }

   static SafetyExec* instance ()
   {
      if ( !m_instance )
         m_instance = new SafetyExec();

      return m_instance;
   }

   void init (void);

   /** basic acessors for internals **/
   SafetyExecData& data () { return execData; }

   MonitorGroup* getGroup (ModuleGroup::Group type) { return groups[type]; }

   /** callbacks for message handlers **/
   void update (const SHwStates& state);
   void update (const CHwStates& state);
   void update (const CHwOrders& orders);
   void update (State_names state);
   void update (SET_TYPE type);

   virtual void SaveToPFR ();
   virtual void RestoreFromPFR ();

   //
   //  The following data members and accessors are global to all safety objects
   //
public:
   const SET_TYPE    SetType () const { return _setType; }
   const State_names SystemState () const { return _procState; }

   safetyRamSafetyApplicationData* AppData (void) { return _appData; }


protected:
   State_names       _procState; // Proc system state
   SET_TYPE          _setType;   // cassette type

   StrokeCalculator* _strokeCalculatorP;

   // Stroke volume logging, for QA only
   bool _pump_stroke_volume_logging;

private:
   SafetyExec(const SafetyExec* s);              // Prevent usage.
   SafetyExec& operator = (const SafetyExec& s); // prevent usage.

   void addGroup (MonitorGroup* group) { groups[group->getType()] = group; }

   // dispatcher for monitor processing
   void process (ModuleGroup::Group group);

   static SafetyExec*              m_instance;

   std::vector<MonitorGroup*>      groups;

   SafetyExecData                  execData;

   safetyRamSafetyApplicationData* _appData;
};

/*+++++++++++++++++++++++++++++
       Safety Safe State Defines : End
++++++++++++++++++++++++++++++*/

// SPECIFICATION:    "receiver" for safety hardware command messages.
//
// NOTES : Pursuant to 2.7 requirements review of service issues, saf_exec needs to
//         check for and sw_fault alarm if a safety command msg is ever detected WHERE
//         the nid != safety node and pid != saf_exec.
//
// ERROR HANDLING:   none.
class aSafetyCommandsMsg_rcvr
{
public:
   aSafetyCommandsMsg_rcvr(safetyRamSafetyApplicationData* _appData);
   virtual ~aSafetyCommandsMsg_rcvr() { }

   void notify ();

private:
   aSafetyCommandsMsg_rcvr();
   aSafetyCommandsMsg_rcvr(aSafetyCommandsMsg_rcvr const&);                 // not implemented
   aSafetyCommandsMsg_rcvr& operator = (aSafetyCommandsMsg_rcvr const&);    // not implemented

private:
   Message<SHwOrders>              _theSHwOrdersMsg;

   unsigned long                   _pidSafExec;         // pid of saf_exec
   unsigned long                   _nidSafetyProcessor; // nid of safety processor

   safetyRamSafetyApplicationData* _appData;
}; // END aSafetyCommandsMsg_rcvr class definition


// SPECIFICATION:    "receiver" for safety hardware status messages.
//
// NOTES :
//
// ERROR HANDLING:   none.

class aSafetyStatusMsg_rcvr
{
public:
   aSafetyStatusMsg_rcvr(aPauseSwitch_Safety* paPauseSwitch
                         , aStopSwitch_Safety* paStopSwitch
                         , aCassette_Safety* paCassette
                         , aDoor_Safety* paDoor
                         , aCentrifuge_Safety* paCentrifuge
                         , aReservoir_Safety* paRes
                         , aDonor* paDonor
                         , needleMonitor* needle
                         , aBlinkTest_Safety* blink
                         , safetyRamSafetyApplicationData* appData);

   virtual ~aSafetyStatusMsg_rcvr();

   void notify ();

   int GetSafetyStatusCount (void) { return _iSafetyStatusCount; }

   void checkForbiddenMode (HW_ORDERS mode);

private:
   void deliveryTime (void);

   aSafetyStatusMsg_rcvr();                                          // not implemented
   aSafetyStatusMsg_rcvr(aSafetyStatusMsg_rcvr const&);              // not implemented
   aSafetyStatusMsg_rcvr& operator = (aSafetyStatusMsg_rcvr const&); // not implemented

private:
   SHwStates SHwStateData;
   Message<SHwStates>              _theSHwStatesMsg;

   aPauseSwitch_Safety*            _paPauseSwitch;
   aStopSwitch_Safety*             _paStopSwitch;
   aCassette_Safety*               _paCassette;
   aDoor_Safety*                   _paDoor;
   aCentrifuge_Safety*             _paCentrifuge;
   aReservoir_Safety*              _paRes;
   aDonor*                         _paDonor;
   needleMonitor*                  _needle;
   aBlinkTest_Safety*              _blink;
   safetyRamSafetyApplicationData* _appData;

   T1STATUS                        _cT1Status;
   int _iSafetyStatusCount;


}; // END aSafetyStatusMsg_rcvr class definition


// SPECIFICATION:    derived message class, "receiver" for control hardware status messages.
//
// NOTES :
//
// ERROR HANDLING:   none.

class aControlStatusMsg_rcvr
{
public:
   aControlStatusMsg_rcvr(aDoor_Safety* paDoor,
                          aReservoir_Safety* paReservoir,
                          aSafety24vPowerTest* pa24vPowerTest,
                          aSafety64vPowerTest* pa64vPowerTest,
                          aDonorDisconnectTest* paDonorDisconnectTest,
                          safetyRamSafetyApplicationData* appData);

   virtual ~aControlStatusMsg_rcvr();

   void       notify ();
   void       newAPSZero (void);
   inline int GetControlStatusCount (void) { return _iControlStatusCount; }

private:
   aControlStatusMsg_rcvr();                                             // not implemented
   aControlStatusMsg_rcvr(aControlStatusMsg_rcvr const&);                // not implemented
   aControlStatusMsg_rcvr& operator = (aControlStatusMsg_rcvr const&);   // not implemented

private:
   Message<CHwStates>           _theCHwStatesMsg;
   APSZeroMsg                   _theAPSZeroMsg;
   RequestSafetyDriverStatusMsg _requestSafetyStatusMsg;

   // Pointers to passed in objects
   aDoor_Safety*                   _paDoor;
   aReservoir_Safety*              _paReservoir;
   aSafety24vPowerTest*            _pa24vPowerTest;
   aSafety64vPowerTest*            _pa64vPowerTest;
   aDonorDisconnectTest*           _paDonorDisconnectTest;
   safetyRamSafetyApplicationData* _appData;

   int     _iControlStatusCount;

   bool    _lowLevelIgnore;
   int     _lowLevelDebounceCount;

   rawTime _lastCHWStatesUpdate;

}; // END aControlStatusMsg_rcvr class defintion



// SPECIFICATION:    derived message class, "receiver" for control hardware orders messages.
//
// NOTES :
//
// ERROR HANDLING:   none.

class aControlCommandsMsg_rcvr
{
public:
   aControlCommandsMsg_rcvr(aProcOpMonitor_Safety* paProcOpMonitor
                            , aCentrifuge_Safety* paCentrifuge
                            , safetyRamSafetyApplicationData* appData);

   virtual ~aControlCommandsMsg_rcvr();

   void notify ();

   inline int GetControlCommandsCount (void) { return( _iControlCommandsCount ); }

private:
   aControlCommandsMsg_rcvr();                                             // not implemented
   aControlCommandsMsg_rcvr(aControlCommandsMsg_rcvr const&);              // not implemented
   aControlCommandsMsg_rcvr& operator = (aControlCommandsMsg_rcvr const&); // not implemented

private:
   CHwOrders CHwOrderData;
   Message<CHwOrders>              _theCHwOrdersMsg;

   aProcOpMonitor_Safety*          _paProcOpMonitor;
   aCentrifuge_Safety*             _paCentrifuge;
   safetyRamSafetyApplicationData* _appData;

   int _iControlCommandsCount;

}; // END aControlCommandsMsg_rcvr class defintion


// SPECIFICATION:    derived message class, "receiver" for SafetyAlive ping messages from procedure.
//
// NOTES :
//
// ERROR HANDLING:   none.

class aQuerySafetyAliveMsg_rcvr
{
public:
   aQuerySafetyAliveMsg_rcvr(aControlCommandsMsg_rcvr* paControlCommandsMsg_rcvr
                             , aControlStatusMsg_rcvr* paControlStatusMsg_rcvr
                             , aSafetyStatusMsg_rcvr* paSafetyStatusMsg_rcvr
                             , aPFR_Safety* paPFR_Safety
                             , safetyRamSafetyApplicationData* appData);

   virtual ~aQuerySafetyAliveMsg_rcvr();

   void notify ();

private:
   aQuerySafetyAliveMsg_rcvr();                                              // not implemented
   aQuerySafetyAliveMsg_rcvr(aQuerySafetyAliveMsg_rcvr const&);              // not implemented
   aQuerySafetyAliveMsg_rcvr& operator = (aQuerySafetyAliveMsg_rcvr const&); // not implemented

private:
   QuerySafetyAliveMsg             _theQuerySafetyAliveMsg;
   AnswerSafetyAliveMsg*           _panAnswerSafetyAliveMsg_sndr;
   aControlCommandsMsg_rcvr*       _paControlCommandsMsg_rcvr;
   aControlStatusMsg_rcvr*         _paControlStatusMsg_rcvr;
   aSafetyStatusMsg_rcvr*          _paSafetyStatusMsg_rcvr;
   safetyRamSafetyApplicationData* _appData;

   aPFR_Safety*                    _paPFR_Safety;
   bool _PFRStatusMsgSent;

}; // END aQuerySafetyAliveMsg_sndr class defintion


#endif // S_EXEC_HPP

/* FORMAT HASH 37358a51ef7c98ae44e80c610fa122ec */
