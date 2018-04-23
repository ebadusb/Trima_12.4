/*********************************************************************************
 NOTE: This file is shared with Vista. Take care when adding new dependencies
**********************************************************************************/

#ifndef WIN32
#include <vxworks.h>
#endif


#include "alarms.h"
#include "alarmcontainer.h"
#include "timermessage.h"
#include "an_alarm.h"


AlarmStruct::AlarmStruct()
   : alarm_name(NULL_ALARM),
     alarm_state(CLEARED),
     alarm_type(OPERATOR_ATTENTION),
     alarm_action(NOTHING),
     alarm_display_type(DISP_NONE),
     alarm_sound(NO_SOUND),
     alarm_description(),
     _InactionTimer(0),
     _Activated(true)
{ }

AlarmStruct::AlarmStruct(ALARM_VALUES new_name,
                         ALARM_TYPES new_type,
                         ALARM_ACTIONS new_action,
                         ALARM_DISPLAY_TYPES new_display_type,
                         SOUND_CODE new_alarm_sound,
                         std::string const& new_description)
   : alarm_name(new_name),
     alarm_state(CLEARED),
     alarm_type(new_type),
     alarm_action(new_action),
     alarm_display_type(new_display_type),
     alarm_sound(new_alarm_sound),
     alarm_description(new_description),
     _InactionTimer(0),
     _Activated(true)
{ }

AlarmStruct::AlarmStruct(const AlarmStruct& a)
   : alarm_name(a.alarm_name),
     alarm_state(a.alarm_state),
     alarm_type(a.alarm_type),
     alarm_action(a.alarm_action),
     alarm_display_type(a.alarm_display_type),
     alarm_sound(a.alarm_sound),
     alarm_description(a.alarm_description),
     _InactionTimer(0),
     _Activated(true)
{ }

AlarmStruct& AlarmStruct::operator = (const AlarmStruct& a)
{
   if ( &a != this )
   {
      alarm_name         = a.alarm_name;
      alarm_state        = a.alarm_state;
      alarm_type         = a.alarm_type;
      alarm_action       = a.alarm_action;
      alarm_sound        = a.alarm_sound;
      alarm_display_type = a.alarm_display_type;
      alarm_description  = a.alarm_description;
      _InactionTimer     = 0;
      _Activated         = a._Activated;
   }

   return *this;
}



AlarmStruct::~AlarmStruct()
{
   // In principle, this should not be necessary, as _InactionTimer should have been deleted
   // elsewhere, and set to NULL.  If it was, this is a no-op; otherwise, clean up here.
   delete _InactionTimer;
}

void AlarmStruct::setTimer (int time)
{
#ifndef WIN32
   if (time > 0)
   {
      // create timer
      if ( !_InactionTimer )
         _InactionTimer = new TimerMessage(time,
                                           Callback<AlarmStruct>(this, &AlarmStruct::reactivate) );
      // reset existing timer
      else
         _InactionTimer->interval(time);
   }
   else if (_InactionTimer)
   {
      delete _InactionTimer;
      _InactionTimer = 0;
   }
#endif
}


//////////////////////////////////////////////////////////////////////
//
//  Alarm Table
//


/// Static data
AlarmTable::AlarmMap_t AlarmTable::_alarmMap;     // singleton data structure
bool                   AlarmTable::_init (false); // flag that someone has loaded the data structure
SEM_ID                 AlarmTable::_loadMSem (semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE));

void load_alarms ();             // local function that actually loads the alarm table

AlarmTable::AlarmTable()
{

   // We need to load the alarm table the first time we are run; this can happen from many tasks at
   // the same time, so we need to lock the critical section.
   if (!_init)
   {
      // ____________________________________________________________________________________________________
      //
      // ENTER the critical section.  _init is only set to true once all the important work in here
      // is done.

      semTake(_loadMSem, WAIT_FOREVER);

      if (!_init )
      {
         load_alarms();

         DataLog(log_level_trima_library_info) << "AlarmTable Constructor, LOADING: "
                                               << _alarmMap.size() << " alarms loaded." << endmsg;
         _init = true;
      }

      semGive(_loadMSem);


      // EXIT critical section
      // ____________________________________________________________________________________________________

      // Class Invariant: the following two alarms MUST exist, otherwise we're complaining loudly
      if ( _alarmMap.find(NULL_ALARM) == _alarmMap.end())
         _FATAL_ERROR(__FILE__, __LINE__, "ALARM CONTAINER FAULT: NULL_ALARM not found");

      if ( _alarmMap.find(INVALID_ALARM_FAULT) == _alarmMap.end())
         _FATAL_ERROR(__FILE__, __LINE__, "ALARM CONTAINER FAULT: INVALID_ALARM_FAULT alarm not found");


   }
}


void AlarmTable::add (ALARM_VALUES value,
                      ALARM_TYPES type,
                      ALARM_ACTIONS action,
                      ALARM_DISPLAY_TYPES display,
                      SOUND_CODE sound,
                      const char* description)
{

   std::pair<AlarmTable::iterator, bool> insert_result
      = _alarmMap.insert(make_pair
                            (value, AlarmStruct(value, type, action, display, sound, description)));

   if (!insert_result.second)
   {
      // note: static_cast here so we don't call op<< for ALARM_VALUES, which uses AlarmTable::op[] and goes to a lot
      // of unnecessary work.
      DataLog(log_level_trima_library_info) << "AlarmTable: warning, alarm " << static_cast<int>(value)
                                            << '(' << description
                                            << ") added twice." << endmsg;
   }
}


AlarmStruct& AlarmTable::operator [] (ALARM_VALUES key)
{
   iterator here = _alarmMap.find(key);
   if (here == _alarmMap.end())
   {
      DataLog (log_level_critical) << "ERROR, AlarmTable: alarm " << key << " not found." << endmsg;

      _FATAL_ERROR(__FILE__, __LINE__, "Alarm Not Found, Invalid Alarm Request");
   }

   return here->second;
}


// This should really be a one-liner that forwards to std::find_if, but that doesn't seem to be
// happy with antediluvian GCC 2.95.  So, a quick reimplementation...

AlarmTable::iterator AlarmTable::findFirstNotCleared ()
{
   iterator i = _alarmMap.begin(), e = _alarmMap.end();
   for (; i!=e; ++i)
      if (i->second.getState() != CLEARED)
         return i;

   return e;
}

void load_alarms ()
{

   //                                                                                                       //
   // Safety Alarms                                                                                         //
   //                                                                                                       //
   AlarmTable::add(FIRST_SAFETY_ALARM,                     A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "FIRST_SAFETY_ALARM");

   AlarmTable::add(AC_OFF_DURING_DRAW,                     A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> AC OFF During DRAW.");
   AlarmTable::add(AC_ON_DURING_RETURN,                    A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> AC ON During Return");
   AlarmTable::add(AC_RATIO_TOO_HIGH_ALARM,                A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> AC Ratio TOO High.");
   AlarmTable::add(AC_RATIO_TOO_LOW_ALARM,                 A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> AC Ratio too LOW");
   AlarmTable::add(AC_INFUSION_HIGH_ALARM,                 A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Detected AC Infusion > 1.3 ml/min -l TBV");
   AlarmTable::add(AC_PUMP_HALL_TOO_BIG,                   A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> AC Pump HALL exceeds Encoder FAULT");
   AlarmTable::add(AC_PUMP_HALL_TOO_SMALL,                 A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> AC Pump Encoder exceeds Hall FAULT");
   AlarmTable::add(CASSETTE_NOT_DOWN,                      A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Cassette not DOWN ");
   AlarmTable::add(CENTRIFUGE_ERROR,                       A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Centrifuge ERROR");
   AlarmTable::add(CENTRIFUGE_OVERSPEED,                   A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Centrifuge OVERSPEED");
   AlarmTable::add(DOOR_NOT_LOCKED_CENTIFUGE_SPINNING,     A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Detected Door not LOCKED and CLOSED w/Centrifuge Spinning ");
   AlarmTable::add(DONOR_AIR_ALARM,                        A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Detected Donor Air FAULT");
   AlarmTable::add(DONOR_HCT_RANGE_ERROR,                  A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Donor Hct Range Error");
   AlarmTable::add(DONOR_HYPOVOLEMIA_ALARM,                A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Detected Donor Hypovolemia");
   AlarmTable::add(DONOR_PRECOUNT_RANGE_ERROR,             A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Donor Precount Range Error");
   AlarmTable::add(DONOR_SEX_INVALID,                      A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> TBV Processing FAILED - Donor Sex INVALID ");
   AlarmTable::add(DONOR_TBV_TOO_LOW,                      A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> TBV Processing FAILED - Donor TBV TOO LOW ");
   AlarmTable::add(DONOR_TBV_TOO_HIGH,                     A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> TBV Processing FAILED - Donor TBV TOO HIGH ");
   AlarmTable::add(DONOR_WEIGHT_TOO_HIGH,                  A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> TBV Processing FAILED - Donor Weight TOO HIGH ");
   AlarmTable::add(DONOR_WEIGHT_TOO_LOW,                   A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> TBV Processing FAILED - Donor Weight TOO LOW ");
   AlarmTable::add(DONOR_HEIGHT_TOO_HIGH,                  A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> TBV Processing FAILED - Donor Height TOO HIGH ");
   AlarmTable::add(DONOR_HEIGHT_TOO_LOW,                   A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> TBV Processing FAILED - Donor Height TOO LOW ");
   AlarmTable::add(DRAW_CYCLE_TOO_BIG,                     A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Detected Draw Cycle Too Big ");
   AlarmTable::add(FIRST_DRAW_CYCLE_TOO_SMALL,             A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> First Draw Cycle Too Small ");
   AlarmTable::add(HYPERVOLEMIA_DETECTED,                  A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Hypervolemia condition detected");
   AlarmTable::add(INAPPROPRIATE_AC_PUMP_MOVEMENT,         A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Inappropriate AC Pump Movement");
   AlarmTable::add(INAPPROPRIATE_CASSETTE_MOVEMENT,        A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Inappropriate Cassette movement.");
   AlarmTable::add(INLET_PUMP_HALL_TOO_BIG,                A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Inlet Pump HALL exceeds Encoder FAULT");
   AlarmTable::add(INLET_PUMP_HALL_TOO_SMALL,              A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Inlet Pump Encoder exceeds Hall FAULT");
   AlarmTable::add(PAUSE_SW_FAILED,                        A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Control did NOT respond to PAUSE Key.");
   AlarmTable::add(PLASMA_OUT_OF_POSITION,                 A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Plasma Valve Out Of Position");
   AlarmTable::add(PLATELET_OUT_OF_POSITION,               A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Platelet Valve Out Of Position");
   AlarmTable::add(PLASMA_PUMP_HALL_TOO_BIG,               A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Plasma Pump HALL exceeds Encoder FAULT");
   AlarmTable::add(PLASMA_PUMP_HALL_TOO_SMALL,             A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Plasma Pump Encoder exceeds Hall FAULT");
   AlarmTable::add(PLATELET_PUMP_HALL_TOO_BIG,             A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Platelet Pump HALL exceeds Encoder FAULT");
   AlarmTable::add(PLATELET_PUMP_HALL_TOO_SMALL,           A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Platelet Pump Encoder exceeds Hall FAULT");
   AlarmTable::add(PRIME_TOO_SOON,                         A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Detected Fluid at Low Level Sensor TOO SOON ");
   AlarmTable::add(PWR_SWITCH_24V_FAILED,                  A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> T1 24V Switch FAILED");
   AlarmTable::add(PWR_SWITCH_64V_FAILED,                  A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> T1 64V Switch FAILED");
   AlarmTable::add(PWR_SWITCH_BOTH_FAILED,                 A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> T1 24V AND 64V Switch FAILED");
   AlarmTable::add(RBC_OUT_OF_POSITION,                    A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> RBC Valve Out Of Position");
   AlarmTable::add(RBC_WB_PROCESSED_TOO_LARGE,             A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> RBC Collect WB Processed Limit Exceeded");
   AlarmTable::add(RESERVOIR_FAILED_T0,                    A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Low Reservoir Sensor FAILED - Not Air during Startup");
   AlarmTable::add(RETURN_PUMP_HALL_TOO_BIG,               A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Return Pump HALL exceeds Encoder FAULT");
   AlarmTable::add(RETURN_PUMP_HALL_TOO_SMALL,             A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Return Pump Encoder exceeds Hall FAULT");
   AlarmTable::add(RETURN_PUMP_SPEED_ALARM_RB,             A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Return Pump Speed > 250ml/min During RB");
   AlarmTable::add(SAFETY_STATUS_COMM_ALARM,               A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Status Message Timeout");
   AlarmTable::add(SAFETY_T1_FAILURE,                      A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> T1 Failure ");
   AlarmTable::add(STATE_TRANSITION_INVALID,               A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Detected INVALID State Transition");
   AlarmTable::add(STOP_SW_FAILED,                         A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Control did NOT respond to STOP Key.");
   AlarmTable::add(SW_FAULT,                               A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Detected Software Fault");
   AlarmTable::add(TBV_UNITS_UNKNOWN,                      A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> TBV Processing FAILED - Ht/Wt Units UNKNOWN ");
   AlarmTable::add(TBV_COMPARISON_FAILED,                  A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> TBV Processing FAILED - Safety and Procedure TBV Values Comparison Error");
   AlarmTable::add(VALVE_LED_ERROR,                        A2,   STOP_ALL, DISP_A2, ALERT_SOUND, "<Safety> Valve LED ERROR");

   //////////////////////////////////////////////////////////////////////////////////////////////////////
   //                                                                                                  //
   // Procedure Alarms                                                                                 //
   //                                                                                                  //
   //  Control Side A2s                                                                                //
   //                                                                                                  //
   AlarmTable::add(BARCODE_INITIALIZATION_ALARM,           A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Unable to initialize barcode sentinel");
   AlarmTable::add(BITMAP_LOAD_FAILURE,                    A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Unable to load bitmap");
   AlarmTable::add(CAL_DAT_BAD_FILE,                       A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "cal.dat file open error");
   AlarmTable::add(CAL_DAT_PARSE_ERROR,                    A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "cal.dat file parsing error");
   AlarmTable::add(CAL_DAT_NO_APS_CAL,                     A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "cal.dat APS not calibrated");
   AlarmTable::add(CAL_DAT_NO_CPS_CAL,                     A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "cal.dat CPS not calibrated");
   AlarmTable::add(CAL_DAT_NO_RBC_CAL,                     A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "cal.dat RBC detector not calibrated");
   AlarmTable::add(CAL_DAT_VALUE_NOT_FOUND,                A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "cal.dat file value not found");
   AlarmTable::add(CAL_DAT_VALUE_OUT_OF_RANGE,             A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "cal.dat file value out of range");
   AlarmTable::add(CENTRIFUGE_SHUTDOWN_FAILURE,            A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Cent. shutdown Failure");
   AlarmTable::add(CFR11_LOG_FILE_ERROR,                   A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "CFR11 log file operation failed");
   AlarmTable::add(CASSETTE_DAT_FILE_BAD_CRC,              A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "cassette.dat file CRC error");
   AlarmTable::add(CASSETTE_DAT_FILE_INVALID_ENTRY,        A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Invalid entry in file cassette.dat");
   AlarmTable::add(CASSETTE_DAT_FILE_OPEN_ERROR,           A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Failed to open file cassette.dat");
   AlarmTable::add(CASSETTE_DAT_FILE_PARSE_ERROR,          A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Parse error encountered in file cassette.dat");
   AlarmTable::add(COBE_CONFIG_BAD_FILE,                   A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Cobe configuration file open error");
   AlarmTable::add(COBE_CONFIG_PARSE_ERROR,                A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Cobe configuration file parsing error");
   AlarmTable::add(COBE_CONFIG_VALUE_NOT_FOUND,            A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Cobe configuration file value not found");
   AlarmTable::add(COBE_CONFIG_VALUE_OUT_OF_RANGE,         A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Cobe configuration file value out of range");
   AlarmTable::add(CONFIGDAT_BAD_FILE,                     A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Config.dat file error");
   AlarmTable::add(CONFIGDAT_PARSE_ERROR,                  A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Config.dat parsing error");
   AlarmTable::add(CONFIGDAT_VALUE_NOT_FOUND,              A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Config.dat value not found");
   AlarmTable::add(CONFIGDAT_VALUE_OUT_OF_RANGE,           A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Config.dat value out of range");
   AlarmTable::add(CONFIGDAT_WRITE_ERROR,                  A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Config.dat write error");
   AlarmTable::add(CONTROL_SOFT_WATCHDOG,                  A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Control Soft Watchdog");
   AlarmTable::add(CPUID_BAD_CONTROL_DATA_FILE,            A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Error reading control CPU data file");
   AlarmTable::add(CPUID_BAD_SAFETY_DATA_FILE,             A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Error reading safety CPU data file");
   AlarmTable::add(CPUID_CONTROL_HW_INVALID,               A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Control CPU is not on the list of validated hardware");
   AlarmTable::add(CPUID_NO_CONTROL_DATA_FILE,             A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Unable to open control CPU data file");
   AlarmTable::add(CPUID_NO_SAFETY_DATA_FILE,              A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Unable to open safety CPU data file");
   AlarmTable::add(CPUID_SAFETY_HW_INVALID,                A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Safety CPU is not on the list of validated hardware");
   AlarmTable::add(FLOW_CALCULATION_ERROR,                 A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Flow calculation error");
   AlarmTable::add(GUI_TRANSACTION_ERROR,                  A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "GUI Transaction Manager Error");
   AlarmTable::add(GUI_VERIFPRD_NO_EXTENDED_TEXT,          A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "GUI verify product screen invoked with no verification reasons");
   AlarmTable::add(GUI_UNEXPECTED_CHOICE,                  A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "GUI unexpected choice");
   AlarmTable::add(HARDWARE_FAULT,                         A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Proc cannot control the hardware");
   AlarmTable::add(HW_DAT_BAD_FILE,                        A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "hw.dat file open error");
   AlarmTable::add(HW_DAT_PARSE_ERROR,                     A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "hw.dat file parsing error");
   AlarmTable::add(HW_DAT_VALUE_NOT_FOUND,                 A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "hw.dat file value not found");
   AlarmTable::add(HW_DAT_VALUE_OUT_OF_RANGE,              A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "hw.dat file value out of range");
   AlarmTable::add(HYPOVOLEMIA_NOT_INITIALIZED,            A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Hypovolemia object used before being initialized");
   AlarmTable::add(INTERNAL_ATOD_ERROR,                    A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Internal A/D error detected");
   AlarmTable::add(INTERNAL_TIMING_ERROR,                  A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Internal timing error detected");
   AlarmTable::add(INVALID_ALARM_FAULT,                    A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Alarm handler fault");
   AlarmTable::add(LOW_LEVEL_DETECTED_PRIOR_TO_EXPECTED,   A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Low level before BLOOD Prime Return");
   AlarmTable::add(MEMORY_TEST_OSINFO_FAILURE,             A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Memory test OS information retrieval error");
   AlarmTable::add(MEMORY_TEST_SAFETY_MEMORY_INSUFFICIENT, A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Memory test safety memory insufficient");
   AlarmTable::add(PREDICT_BAD_TARGET_SUBSTATE,            A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Predict processed invalid target substate");
   AlarmTable::add(PREDICT_INVALID_GUI_REQUEST,            A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Predict received invalid GUI request");
   AlarmTable::add(PREDICT_INVALID_MODIFY_REQUEST,         A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Predict detected invalid product modification request");
   AlarmTable::add(PREDICT_NO_SELECTED_PROCEDURE,          A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Predict detected system running with no selected procedure");
   AlarmTable::add(PREDICT_NOT_RESPONDING,                 A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Predict is not responding to Proc");
   AlarmTable::add(PROC_STATES_FILE_NOT_FOUND,             A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Proc states file not found");
   AlarmTable::add(PROC_STATES_FILE_READ_ERROR,            A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Proc states file read error");
   AlarmTable::add(PROC_STATES_INIT_ERROR,                 A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Proc states file initialization error");
   AlarmTable::add(PROC_STATES_PROCESSING_ERROR,           A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Proc state manager processing error");
   AlarmTable::add(RANDOM_FILE_STORAGE_ERROR,              A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Error managing random file storage");
   AlarmTable::add(RBC_DAT_BAD_FILE,                       A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "rbc.dat file open error");
   AlarmTable::add(RBC_DAT_PARSE_ERROR,                    A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "rbc.dat file parsing error");
   AlarmTable::add(RBC_DAT_VALUE_NOT_FOUND,                A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "rbc.dat file value not found");
   AlarmTable::add(RBC_DAT_VALUE_OUT_OF_RANGE,             A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "rbc.dat file value out of range");
   AlarmTable::add(RET_OCC_MONITOR_FAILURE,                A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "retoccmonitor task failure");
   AlarmTable::add(SAFETY_NOT_RESPONDING,                  A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Safety not responding to starting protocol");
   AlarmTable::add(SAFETY_STATUS_INACTIVE,                 A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Safety status not seen within time limit");
   AlarmTable::add(SETCONFIG_DAT_FILE_BAD_CRC,             A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "setconfig.dat file CRC error");
   AlarmTable::add(SETCONFIG_DAT_FILE_INVALID_ENTRY,       A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Invalid entry in file setconfig.dat");
   AlarmTable::add(SETCONFIG_DAT_FILE_OPEN_ERROR,          A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Failed to open file setconfig.dat");
   AlarmTable::add(SETCONFIG_DAT_FILE_PARSE_ERROR,         A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Parse error encountered in file setconfig.dat");
   AlarmTable::add(SOFTWARE_CRC_CHECK_FAILURE,             A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "CRC check of installed software failed");
   AlarmTable::add(VIP_CANT_BIND_SOCKET,                   A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "can't bind vista socket");
   AlarmTable::add(VIP_CANT_LISTEN_ON_SOCKET,              A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "can't listen on vista socket");
   AlarmTable::add(VTARGETCHECKER_DEFAULT,                 A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "Target checker default case detected");
   AlarmTable::add(SOUNDS_DAT_BAD_FILE,                    A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "sounds.dat file open error");
   AlarmTable::add(SOUNDS_DAT_PARSE_ERROR,                 A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "sounds.dat file parsing error");
   AlarmTable::add(SOUNDS_DAT_VALUE_NOT_FOUND,             A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "sounds.dat file value not found");
   AlarmTable::add(SOUNDS_DAT_VALUE_OUT_OF_RANGE,          A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "sounds.dat file value out of range");
   AlarmTable::add(SOUNDS_DAT_CRC_CHECK_FAILURE,           A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "sounds.dat CRC check failed");
   AlarmTable::add(SW_CONFIG_VALUE_NOT_FOUND,              A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "SW configuration file value not found");
   AlarmTable::add(STRING_INFO_FILE_ERROR,                 A2,                 STOP_ALL,   DISP_A2, ALERT_SOUND, "String info file error");

   //                                                                                                       //
   //  Control Side A1s                                                                                     //
   //                                                                                                       //
   AlarmTable::add(plus64V_TOO_HIGH,                       A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, " 64V power too HIGH ");
   AlarmTable::add(plus64V_TOO_LOW,                        A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, " 64V power too LOW ");
   AlarmTable::add(plus24V_TOO_HIGH,                       A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, " 24V power too HIGH ");
   AlarmTable::add(plus24V_TOO_LOW,                        A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, " 24V power too LOW ");
   AlarmTable::add(plus12V_TOO_HIGH,                       A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, " 12V power too HIGH ");
   AlarmTable::add(plus12V_TOO_LOW,                        A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, " 12V power too LOW ");
   AlarmTable::add(AC_DETECTED_PRIOR_TO_AC_PRIME_FAILURE,  A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "AC prior to AC Prime Failure");
   AlarmTable::add(APS_AUTO_ZERO_FAILED_RANGE,             A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "APS auto-zero excessive variation in readings ");
   AlarmTable::add(APS_AUTO_ZERO_FAILED_MAX,               A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "APS auto-zero above maximum allowed ");
   AlarmTable::add(APS_AUTO_ZERO_FAILED_MIN,               A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "APS auto-zero below minimum allowed ");
   AlarmTable::add(APS_AUTO_ZERO_FILE_ERROR,               A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "APS file operation error ");
   AlarmTable::add(FIRST_CYCLE_TOO_SOON_ALARM,             A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "First cycle seen too soon");
   AlarmTable::add(HYPOVOLEMIA_ALARM,                      A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "HYPOVOLEMIA detected; STOP Procedure....");
   AlarmTable::add(INLET_VOLM_ALARM,                      A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Inlet volume too high ; STOP Procedure....");
   AlarmTable::add(LOW_LEVEL_SENSOR_SEEN_TOO_LATE_FAILURE, A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Low level too late");
   AlarmTable::add(LOW_LEVEL_SENSOR_SEEN_TOO_SOON_FAILURE, A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Low level too soon");
   AlarmTable::add(minus12V_TOO_HIGH,                      A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, " -12V power too HIGH ");
   AlarmTable::add(minus12V_TOO_LOW,                       A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, " -12V power too LOW ");
   AlarmTable::add(OVERTEMP_FAILURE,                       A1,                 STOP_ALL,   DISP_OPERATOR, ALARM_SOUND, "Basin overtemperature detected");
   AlarmTable::add(POWER_FAIL_ABORT,                       A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "PFR detected with recovery prohibited");
   AlarmTable::add(PRIME_FAILURE,                          A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Prime Failure");
   AlarmTable::add(TEN_MINUTE_PAUSE_ALARM,                 A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Paused 10 Minutes without access maintained");
   AlarmTable::add(TOO_MANY_RESERVOIR_DRAW_ALARMS,         A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Too many consecutive reservoir draw alarms");
   AlarmTable::add(TOO_MANY_RESERVOIR_RETURN_ALARMS,       A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Too many consecutive reservoir return alarms");
   AlarmTable::add(RESERVOIR_FULL_AFTER_RETURN,            A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Reservoir full after return cycle");
   AlarmTable::add(AC_PRIME_INLET_ALARM,                   A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "AC Occlusion during inlet prime");
   AlarmTable::add(RESERVOIR_RETURN_TOO_LONG_ALARM_A1,     A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Return cycle took too long to empty the reservoir - A1");
   AlarmTable::add(CPS_TOO_HIGH_ALARM,                     A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "CPS Too High Alarm during MSS ");
   AlarmTable::add(CHANNEL_VOLUME_REMAINING_HIGH,          A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Channel Volume Remaining High");
   AlarmTable::add(APS_TOO_HIGH_ALARM_MSS,                 A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Access/Return Pressure TOO High during MSS addition ");
   AlarmTable::add(RETURN_LINE_OCCLUSION_A1,               A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Possible return line occlusion detected - A1 ");
   AlarmTable::add(HEMOLYSIS_DETECTED_ALARM,               A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Hemolysis detected ");
   AlarmTable::add(AC_DETECTION_ALARM,                     A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "AC not detected at detector too much");
   AlarmTable::add(SAL_RINSEBACK_RETURN_TOO_HIGH,          A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Upper level sensor during saline rinseback ");
   AlarmTable::add(PERSISTENT_SPILLOVER_ALARM,             A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "Run Ended due to Persistent spillovers");

   AlarmTable::add(AC_CLOSURE_FAILED_ALARM,                A1,                 STOP_ALL,   DISP_A1,       ALARM_SOUND, "AC Closure Failed Alarm (RAS disabled)");


   AlarmTable::add(EVACUATION_PRESSURE_ALERT_FINAL,        A1,                 STOP_PUMPS, DISP_A1,       ALARM_SOUND, "Pressure detected during bag evacuation Ending Run ");
   AlarmTable::add(FAILURE_TO_PRESSURIZE_INLET_FINAL,      A1,                 STOP_PUMPS, DISP_A1,       ALARM_SOUND, "Pressure Test Failure Ending Run ");



   //                                                                                                       //
   //  Control Side R2s                                                                                     //
   //                                                                                                       //
   AlarmTable::add(APS_DECOUPLING,                         R2,                 STOP_ALL,   DISP_R2,       ALARM_SOUND, "APS Sensor decoupled");
   AlarmTable::add(CENTRIFUGE_HARDWARE_FAILURE,            R2,                 STOP_ALL,   DISP_R2,       ALARM_SOUND, "Cent. hardware Failure ");
   AlarmTable::add(HIGH_CENTRIFUGE_COMMAND,                R2,                 STOP_ALL,   DISP_R2,       ALARM_SOUND, "Centrifuge command too HIGH ");
   AlarmTable::add(POWER_FAIL_RINSEBACK,                   R2,                 STOP_ALL,   DISP_R2,       ALARM_SOUND, "PFR detected with rinseback allowed ");

   AlarmTable::add(CPS_HIGH_DURING_RBC_COLLECT_ALARM,      R2,                 STOP_ALL,   DISP_R2,       ALARM_SOUND, "Centrifuge Pressure Sensor detects possible clogged RBC filter");
   AlarmTable::add(CPS_PTF_BASELINE_TEST_ALARM,            R2,                 STOP_ALL,   DISP_R2,       ALARM_SOUND, "Centrifuge Pressure Sensor repeatedly failed baseline test");
   //                                                                                                       //
   //  Control Side R1s                                                                                     //
   //                                                                                                       //


   AlarmTable::add(AC_CLOSURE_FAILED_CONTINUE_PAS_ALARM,   R1,                 STOP_MSS_RBC,   DISP_R1,       ALARM_SOUND, "AC Closure Failed Alarm (RAS disabled. PAS allowed to continue)");

   AlarmTable::add(AC_CLOSURE_FAILED_ALERT,                R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "AC Closure Failed Alert");
   AlarmTable::add(AC_LEVEL_ALARM,                         R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "NO AC seen at fluid detector ");
   AlarmTable::add(AC_NOT_REMOVED_ALERT,                   R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "AC seen at fluid detector during closure test");
   AlarmTable::add(AC_OCCLUSION_DETECTED,                  R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Possible AC Occlusion check AC bag and line ");
   AlarmTable::add(ACCESS_NEGATIVE_ALARM_NOT_REACHED,      R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Access Neg. NOT reached ");
   AlarmTable::add(ACCESS_PRESSURE_DECAY_FAILURE,          R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Access decay failure ");
   AlarmTable::add(APS_DURING_PAUSE_HI,                    R1,                 STOP_PUMPS, DISP_R2,       ALARM_SOUND, "Access pressure alarm occurred in pause condition (high) ");
   AlarmTable::add(APS_PUMPS_SLOW,                         R1,          HALVE_PUMP_SPEEDS, DISP_R1,       ALARM_SOUND, "Automatic APS slowing of the pumps ");
   AlarmTable::add(APS_TOO_HIGH_ALERT,                     R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Access/Return Pressure TOO High");

   AlarmTable::add(APS_TOO_HIGH_ALERT_MSS,                 R1,                 STOP_MSS_RBC, DISP_R1,       ALARM_SOUND, "Access Pressure TOO High During Metered Storage");
   AlarmTable::add(APS_HIGH_MSS_P1_OK_ALERT,               R1,                 STOP_MSS_RBC, DISP_R1,       ALARM_SOUND, "Access Pressure TOO High During Metered Storage (product 1 Still Ok)");
   AlarmTable::add(APS_TOO_HIGH_PTF_ALERT,                 R1,                 STOP_MSS_RBC, DISP_R1,       ALARM_SOUND, "Access/Return Pressure TOO High during MSS addition ");

   AlarmTable::add(CONTROL_PRE_WATCHDOG_PAUSE,             R1,          STOP_DONOR_ACCESS, DISP_STATUS_LINE, ATTENTION1_SOUND, "Pre Watchdog pause");

   AlarmTable::add(APS_TOO_LOW,                            R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Access Pressure TOO Low ");
   AlarmTable::add(CASSETTE_FAILURE,                       R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Cassette Failure ");
   AlarmTable::add(CASSETTE_INVALID_ALARM,                 R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Cassette type not valid for procedure selected ");
   AlarmTable::add(CASSETTE_REF_MISMATCH_ALARM,            R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Cassette type detected does not match chosen REF number ");
   AlarmTable::add(CPS_OUT_OF_RANGE_AT_SET_LOWER,          R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "CPS out of range at set lower ");
   AlarmTable::add(CASSETTE_UNRECOGNIZED_ALARM,            R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Cassette stamp color not recognized ");
   AlarmTable::add(CPS_HIGH_DURING_RBC_COLLECT_ALERT,      R1,                 STOP_PUMPS_CLOSE_RBC, DISP_R1,         ALARM_SOUND, "Centrifuge Pressure Sensor detects possible clogged RBC filter during collect ");
   AlarmTable::add(CPS_HIGH_DURING_RBC_COLLECT_PROD2_OK_ALERT, R1,             STOP_PUMPS_CLOSE_RBC, DISP_R1,         ALARM_SOUND, "Centrifuge Pressure Sensor detects possible clogged RBC filter during product 1 collect ");
   AlarmTable::add(CENTRIFUGE_INTEGRAL_FAILURE,            R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Cent. integration Failure ");
   AlarmTable::add(CHANNEL_CLAMP_NOT_CLOSED_ALERT,         R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Channel Clamp not closed ");
   AlarmTable::add(CHANNEL_CLAMP_NOT_CLOSED_ADDITION_ALERT, R1,                 STOP_MSS_PLT, DISP_R1,       ALARM_SOUND, "Channel Clamp not closed during addition ");
   AlarmTable::add(CPS_PTF_BASELINE_TEST_ALERT,            R1,                 STOP_PUMPS_CLOSE_RBC, DISP_R1,       ALARM_SOUND, "Centrifuge Pressure Sensor failed baseline test");
   AlarmTable::add(CPS_PTF_PRIME_TEST,                     R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Centrifuge Pressure Sensor not responding as expected during PTF prime ");
   AlarmTable::add(CPS_TOO_HIGH_ALERT_NO_CLAMP,            R1,                   STOP_ALL, DISP_R1,       ALARM_SOUND, "Centrifuge pressure TOO High Alert (no AAS kit version) ");
   AlarmTable::add(CPS_TOO_HIGH_ALERT_ONE_CLAMP,           R1,                   STOP_ALL, DISP_R1,       ALARM_SOUND, "Centrifuge pressure TOO High Alert (RAS-only kit version) ");
   AlarmTable::add(CPS_TOO_HIGH_ALERT_THREE_CLAMP,         R1,                   STOP_ALL, DISP_R1,       ALARM_SOUND, "Centrifuge pressure TOO High Alert (PAS kit version) ");
   AlarmTable::add(CPS_TOO_HIGH_ALERT_THREE_CLAMP_PTF,     R1,                   STOP_ALL, DISP_R1,       ALARM_SOUND, "Centrifuge pressure TOO High Alert (PAS + RAS kit version) ");
   AlarmTable::add(DISCONNECT_TEST_ALARM,                  R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Disconnect donor - UNLOADING PUMPS!!! ");
   AlarmTable::add(DISCONNECT_TEST_ALERT,                  R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Disconnect alert ");
   AlarmTable::add(DISCONNECT_TEST_MSS_ALARM,              R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Disconnect donor  (Automated Solutions Pending) - Disconnect could not be confirmed!");
   AlarmTable::add(DISCONNECT_TEST_MSS_ALERT,              R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Disconnect alert  (Automated Solutions Pending) ");
   AlarmTable::add(DOOR_LATCH_FAILURE,                     R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Door latch Failure ");
   AlarmTable::add(DOOR_LOCK_ALARM,                        R1,                   STOP_ALL, DISP_R1,       ALARM_SOUND, "Door NOT Locked / Centrifuge can not run ");
   AlarmTable::add(DOOR_OPEN_ALARM,                        R1,                   STOP_ALL, DISP_R1,       ALARM_SOUND, "Door Open / Centrifuge can not run ");
   AlarmTable::add(DRBC_SPLIT_ALERT,                       OPERATOR_ATTENTION,                 STOP_PUMPS, DISP_R1,       NO_SOUND, "DRBC Split alert ");
   AlarmTable::add(EVACUATION_PRESSURE_ALERT,              R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Pressure detected during bag evacuation ");
   AlarmTable::add(EVACUATION_PRESSURE_ALERT_JP,           R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Pressure detected during bag evacuation (japanese bmp) ");
   AlarmTable::add(FAILURE_TO_CLOSE_CROSSOVER,             R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Failure to Close the crossover clamp ");
   AlarmTable::add(FAILURE_TO_PRESSURIZE_INLET,            R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Failure to pressurize inlet ");
   AlarmTable::add(FAILURE_TO_PRESSURIZE_INLET_NON_RAS_SET, R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Failure to pressurize inlet (non-RAS set) ");
   AlarmTable::add(FAILURE_TO_PRESSURIZE_INLET_RAS_SET,    R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Failure to pressurize inlet (RAS set) ");
   AlarmTable::add(FAILURE_TO_RELIEVE_PRESSURE,            R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Failure to Relieve access pressure ");
   AlarmTable::add(FIRST_CYCLE_TOO_LATE_ALARM,             R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "First cycle seen too late ");
   AlarmTable::add(LEAK_TEST_FAILURE,                      R1,                   STOP_ALL, DISP_R1,       ALARM_SOUND, "Leak test Failure ");
   AlarmTable::add(LEAK_DETECTED_ALERT,                    R1,                   STOP_ALL, DISP_R1,       ALARM_SOUND, "Leak detected Alert");
   AlarmTable::add(LEAK_DETECTOR_ERROR_FAILURE,            R1,                   STOP_ALL, DISP_R1,       ALARM_SOUND, "Leak detector ERROR Failure ");
   AlarmTable::add(MAINTAIN_DONOR_ACCESS_ALERT,            R1,            SLOW_CENTRIFUGE, DISP_FLOATING, ATTENTION1_SOUND, "Pumps Paused 3 Minutes ");
   AlarmTable::add(MOTOR_FAILED_TO_MAINTAIN_SPEED,         R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Motor Failed to Maintain Speed ");
   AlarmTable::add(MOTOR_INTEGRATION_ERROR,                R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Motor Integration error ");
   AlarmTable::add(MOTOR_VOLUME_ERROR,                     R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Motor Volume Error ");
   AlarmTable::add(MOTOR_DISABLED,                         R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Motor Disabled ");



   AlarmTable::add(MSS_PRIME_TOO_SOON_RBC_ALARM,           R1,                STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Fluid too soon during RAS prime ");
   AlarmTable::add(MSS_PRIME_DELAY_RBC_ALERT,              R1,                STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Reservoir status change not seen for RAS prime ");
   AlarmTable::add(MSS_PRIME_TOO_SOON_PLT_ALERT,           R1,                STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Fluid too soon during PAS prime");
   AlarmTable::add(MSS_PRIME_DELAY_PLT_ALERT,              R1,                STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Reservoir status change not seen for PAS prime ");
   AlarmTable::add(MSS_MISCONNECTED_2ND_PAS_BAG,           R1,                STOP_PUMPS, DISP_R1,       ALARM_SOUND, "RBC monitor indicates 2nd PAS bag misconnected");

   AlarmTable::add(MSS_PLT_SS_EMPTY,                       OPERATOR_ATTENTION, STOP_MSS_PLT,  DISP_R1,    NO_SOUND,    "Prompt operator to replace PAS bag ");
   AlarmTable::add(MSS_RBC_SS_EMPTY,                       OPERATOR_ATTENTION, STOP_MSS_RBC,  DISP_R1,    NO_SOUND,    "Prompt operator to replace RAS bag ");
   AlarmTable::add(MSS_DRBC_SS_EMPTY,                      OPERATOR_ATTENTION, STOP_MSS_RBC,  DISP_R1,    NO_SOUND,    "Prompt operator to replace RAS bag (drbc set) ");
   AlarmTable::add(ONE_MINUTE_PAUSE,                       R1,            SLOW_CENTRIFUGE, DISP_FLOATING, ATTENTION1_SOUND, "Pumps Paused 1 Minute RPM reduced ");
   AlarmTable::add(OVERTEMP_ALARM,                         R1,                 STOP_ALL,   DISP_OPERATOR, ALARM_SOUND, "Basin overtemperature detected ");
   AlarmTable::add(PAUSE_BUTTON_ALARM,                     R1,                 STOP_PUMPS, DISP_FLOATING, ALARM_SOUND, "PAUSE button pressed ");
   AlarmTable::add(PLASMA_VALVE_FAILURE,                   R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Plasma valve Failure ");
   AlarmTable::add(POWER_FAIL_CONTINUE,                    R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "PFR detected with continue allowed ");
   AlarmTable::add(POWER_TEST_FAILURE,                     R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Power test Failure ");
   AlarmTable::add(PROCEDURE_MODIFIED,                     R1,                 STOP_PUMPS, DISP_R1,       ATTENTION1_SOUND, "Procedure needs operator confirmation to continue ");
   AlarmTable::add(PUMP_RPM_COMMAND_FAILURE,               R1,                   STOP_ALL, DISP_R1,       ALARM_SOUND, "Pump RPM command Failure ");
   AlarmTable::add(RBC_DETECTOR_CAL_FAILURE,               R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "RBC Detector Calibration Failure ");
   AlarmTable::add(RBC_VALVE_FAILURE,                      R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "RBC valve Failure ");
   AlarmTable::add(RESERVOIR_ALARM,                        R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Reservoir Sensor Failure - Fluid high : Air low ");
   AlarmTable::add(COLLECT_VALVE_FAILURE,                  R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Collect valve Failure ");
   AlarmTable::add(RESERVOIR_DRAW_TOO_LONG_ALARM_NO_CLAMP,          R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Draw cycle took too long to fill the reservoir ");
   AlarmTable::add(RESERVOIR_DRAW_TOO_SHORT_ALARM_NO_CLAMP,         R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Draw cycle was too short to fill the reservoir ");
   AlarmTable::add(RESERVOIR_RETURN_TOO_LONG_ALARM_NO_CLAMP,        R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Return cycle took too long to empty the reservoir ");
   AlarmTable::add(RESERVOIR_RETURN_TOO_SHORT_ALARM_NO_CLAMP,       R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Return cycle was too short to empty the reservoir ");

   AlarmTable::add(RESERVOIR_DRAW_TOO_LONG_ALARM_ONE_CLAMP,          R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Draw cycle took too long to fill the reservoir ");
   AlarmTable::add(RESERVOIR_DRAW_TOO_SHORT_ALARM_ONE_CLAMP,         R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Draw cycle was too short to fill the reservoir ");
   AlarmTable::add(RESERVOIR_RETURN_TOO_LONG_ALARM_ONE_CLAMP,        R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Return cycle took too long to empty the reservoir ");
   AlarmTable::add(RESERVOIR_RETURN_TOO_SHORT_ALARM_ONE_CLAMP,       R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Return cycle was too short to empty the reservoir ");

   AlarmTable::add(RESERVOIR_DRAW_TOO_LONG_ALARM_THREE_CLAMP,          R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Draw cycle took too long to fill the reservoir ");
   AlarmTable::add(RESERVOIR_DRAW_TOO_SHORT_ALARM_THREE_CLAMP,         R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Draw cycle was too short to fill the reservoir ");
   AlarmTable::add(RESERVOIR_RETURN_TOO_LONG_ALARM_THREE_CLAMP,        R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Return cycle took too long to empty the reservoir ");
   AlarmTable::add(RESERVOIR_RETURN_TOO_SHORT_ALARM_THREE_CLAMP,       R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Return cycle was too short to empty the reservoir ");

   AlarmTable::add(RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_NO_CLAMP,    R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Draw cycle too long - RBC/Plasma set ");
   AlarmTable::add(RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_ONE_CLAMP,   R1,                 STOP_PUMPS, DISP_R1,       ALARM_SOUND, "Draw cycle too long - RBC/Plasma set ");

   AlarmTable::add(RETURN_LINE_OCCLUSION,                         R1,      STOP_PUMPS, DISP_R1,               ALARM_SOUND,      "Possible return line occlusion detected ");
   AlarmTable::add(STOP_BUTTON_ALARM,                             R1,      STOP_ALL,   DISP_FLOATING,         ALARM_SOUND,      "STOP button pressed ");
   AlarmTable::add(TEN_MINUTE_PAUSE,                              R1,      STOP_ALL,   DISP_FLOATING_HIGHPRI, ATTENTION1_SOUND, "Pumps Paused 10 Minutes ");
   AlarmTable::add(UNLOADING_BAD_CASSETTE_POSITION,               R1,      STOP_PUMPS, DISP_R1,               ALARM_SOUND,      "UNLOADING Bad Cassette Position ");
   AlarmTable::add(VALVE_HARDWARE_FAILURE,                        R1,      STOP_ALL,   DISP_R1,               ALARM_SOUND,      "Valve hardware Failure ");
   AlarmTable::add(VALVE_TEST_FAILURE,                            R1,      STOP_PUMPS, DISP_R1,               ALARM_SOUND,      "Valve test Failure ");
   AlarmTable::add(LOW_PLATELET_CONCENTRATION_ALERT,              R1,      NOTHING,    DISP_OPERATOR,         ATTENTION1_SOUND, "Low platelet concentration detected");
   AlarmTable::add(GENERIC_PLT_WBC_CONTAMINATION_ALERT,           R1,      NOTHING,    DISP_OPERATOR,         ALARM_SOUND,      "Inform operator about possible plt wbc contamination ");
   AlarmTable::add(GENERIC_PLT_WBC_CONTAMINATION_OBTRUSIVE_ALERT, R1,      NOTHING,    DISP_OPERATOR,         ALARM_SOUND,      "Inform operator about possible plt wbc contamination and offer endrun ");
   AlarmTable::add(AUTOFLOW_TIME_ALERT,                           R1,      NOTHING,    DISP_OPERATOR,         ATTENTION1_SOUND, "Inform operator proc time increased 10 mins due to autoflow ");
   AlarmTable::add(AUTOFLOW_TIME_ALARM,                           R1,      STOP_PUMPS, DISP_R1,               ALARM_SOUND,      "Inform operator proc time increased 10 mins due to autoflow AGAIN! ");
   AlarmTable::add(AUTOFLOW_TIME_DQ,                              R1,      STOP_PUMPS, DISP_FLOATING,         ALARM_SOUND,      "Inform operator proc time exceeded due to autoflow ");

   AlarmTable::add(APS_DURING_PAUSE_LOW,                          R1,      STOP_PUMPS, DISP_R2,               ALARM_SOUND, "Access pressure alarm occurred in pause condition (low) ");

   AlarmTable::add(APS_TOO_HIGH_ALARM_MSS_WITH_PAS,               R1,      STOP_ALL,   DISP_R1,       ALARM_SOUND, "Access/Return Pressure TOO High during MSS addition with PAS ");
   //                                                                                                        //
   // Control Side Operator Attention                                                                        //
   //                                                                                                        //
   AlarmTable::add(APS_DECOUPLING_PAUSE,                   OPERATOR_ATTENTION, STOP_PUMPS,        DISP_NONE,     NO_SOUND,         "APS sensor decoupled pause");

   AlarmTable::add(APS_PAUSE,                              OPERATOR_ATTENTION, STOP_DONOR_ACCESS, DISP_NONE,     NO_SOUND,         "Automatic APS pause ");

   AlarmTable::add(BARCODE_INFO_ALARM,                     OPERATOR_ATTENTION, NOTHING,           DISP_OPERATOR, NO_SOUND,         "Information alarm to raise the barcode mgmt. screen ");
   AlarmTable::add(CONCENTRATION_OUT_OF_TOLERANCE,         OPERATOR_ATTENTION, NOTHING,           DISP_OPERATOR, NO_SOUND,         "Concentration outside tolerance ");
   AlarmTable::add(PAUSE_BEFORE_SPILLOVER,                 OPERATOR_ATTENTION, STOP_PUMPS,        DISP_NONE,     NO_SOUND,         "Pausing to allow spillover confirmation ");
   AlarmTable::add(PREDICTION_PAUSE,                       OPERATOR_ATTENTION, STOP_PUMPS,        DISP_NONE,     NO_SOUND,         "Pumps paused while operator selects a procedure ");
   AlarmTable::add(REPLACEMENT_FLUID_ALARM,                OPERATOR_ATTENTION, STOP_PUMPS,        DISP_OPERATOR, NO_SOUND,         "Prompt operator to connect replacement fluid ");
   AlarmTable::add(REPLACEMENT_FLUID_BOLUS_ALARM,          OPERATOR_ATTENTION, STOP_PUMPS,        DISP_FLOATING, NO_SOUND,         "Prompt operator to connect replacement fluid for a bolus of fluid ");
   AlarmTable::add(REPLACEMENT_FLUID_NEW_BAG_ALARM,        OPERATOR_ATTENTION, STOP_PUMPS,        DISP_FLOATING, NO_SOUND,         "Prompt operator to replace empty replacement fluid bag ");
   AlarmTable::add(REPLACEMENT_FLUID_ALARM_RINSEBACK,      OPERATOR_ATTENTION, STOP_PUMPS,        DISP_FLOATING, NO_SOUND,         "Prompt operator to connect replacement fluid bag for saline rinseback ");
   AlarmTable::add(RESERVOIR_CHECK_SILENT_ALARM,           OPERATOR_ATTENTION, STOP_PUMPS,        DISP_NONE,     NO_SOUND,         "Silent ULS alarm to pause and check ULS signal on first cycle.");
   AlarmTable::add(SET_IDENTIFICATION_ALARM,               OPERATOR_ATTENTION, STOP_PUMPS,        DISP_OPERATOR, NO_SOUND,         "Prompt operator to manually determine the set type ");
   AlarmTable::add(SPILLOVER_DETECTED,                     OPERATOR_ATTENTION, NOTHING,           DISP_OPERATOR, ATTENTION1_SOUND, "Spillover Detected ");
   AlarmTable::add(SILENT_PAUSE_FOR_RET_TOO_SHORT_CHECK,   OPERATOR_ATTENTION, STOP_PUMPS,        DISP_NONE,     NO_SOUND,         "Pause for Return Too Short Check");


   //                                                                                                        //
   // Control Side CFR Audit Tracking Alarms (should be lowest priority so it doesn't preempt 'real' alarms) //
   //                                                                                                        //
   AlarmTable::add(CFR11_TRACKING_ALARM,                   OPERATOR_ATTENTION, NOTHING,           DISP_CFR11,    NO_SOUND, "Audit Tracking Alarm to raise selection mgmt. screen ");


   //                                                                                                         //
   // Do NOT move this alarm. It MUST be the last alarm in the table.                                         //
   //                                                                                                         //
   AlarmTable::add(NULL_ALARM,                             OPERATOR_ATTENTION, NOTHING,           DISP_NONE,     NO_SOUND, "NULL Alarm ");

}

/* FORMAT HASH 1e19d8b26a35195f24eb4c70f55ef69d */
