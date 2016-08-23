/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#ifndef __HALSTATUS_CDS_INCLUDE
#define __HALSTATUS_CDS_INCLUDE

#include <cstdlib>
#include "datastore.h"
#include "cassette.h"
#include "haldefs.h"

#ifndef WIN32
#pragma interface
#endif

class HalPump_CDS
   : public SingleWriteDataStore
{
public:
   enum PumpError
   {
      PumpError_NoFault                = 0, // no errors must be reported as 0
      PumpError_FailureToMaintainSpeed = 1,
      PumpError_IntegrationError       = 2,
      PumpError_VolumeError            = 4,
      PumpError_Disabled               = 8
   };

   BaseElement<PumpError> _error;                   // bitwise or of error conditions above

   BaseElement<float>     _CmdRPM;                  // commanded RPM
   BaseElement<float>     _ActRPM;                  // current actual reported RPM
   BaseElement<float>     _CmdFlow;                 // commanded flow rate (ml/min)
   BaseElement<float>     _ActFlow;                 // current actual flow rate (ml/min)

   BaseElement<float>     _Revs;                    // Delta revs since last update
   BaseElement<float>     _dV;                      // pump delta volume this update (ml)

   BaseElement<long>      _Accum;               // total revs, encoder ticks
   BaseElement<long>      _Integral;            // snap-shot of the integration error
   BaseElement<long>      _AccumIntegral;       // cummulative integration error

public:
   HalPump_CDS(const char* name, Role role);
   virtual ~HalPump_CDS();

   inline bool  Disabled (void) const { return ( (_error & PumpError_Disabled) != 0 ); }
   inline float dV (void) const { return _dV; }

   inline float ActFlow (void) const { return _ActFlow; }
   inline float ActRPM (void) const { return _ActRPM; }

   // previous commanded values
   inline float CmdFlow (void) const { return _CmdFlow; }
   inline float CmdRPM (void) const { return _CmdRPM; }

   // error bit field
   inline PumpError Error (void) const { return _error; }
};


DataLog_Stream& operator << (DataLog_Stream& os, HalPump_CDS::PumpError id);


///////////////////////////////////////////////
// Power supply

//
// Define lowest acceptable values for voltages for normal operation
// and high and low alarm points.
//
// High and low alarm points are as specified in memo from Tom
// Bezdek "Voltage limit checks for Beta 2.2".
//
enum
{
   HL_ACCEPTABLE_64V_POWER      = (int)( 64000.0 * 0.85 ),
   HL_LOW_64V_POWER             = 44000, // Low mV level for 64V supply
   HL_HIGH_64V_POWER            = 84000, // High mV level for 64V supply

   HL_ACCEPTABLE_24V_POWER      = (int)( 24000.0 * 0.85 ),
   HL_LOW_24V_POWER             = 19200, // Low mV level for 24V supply
   HL_HIGH_24V_POWER            = 28800, // High mV level for 24V supply

   HL_ACCEPTABLE_12V_POWER      = (int)( 12000.0 * 0.85 ),
   HL_LOW_12V_POWER             = 11000, // Low mV level for 12V supply
   HL_HIGH_12V_POWER            = 13000, // High mV level for 12V supply

   HL_ACCEPTABLE_minus12V_POWER = (int)( 12000.0 * 0.85 ),
   HL_LOW_minus12V_POWER        = 11000, // Low mV level for -12V supply
   HL_HIGH_minus12V_POWER       = 13000, // High mV level for -12V supply.

   HL_ACCEPTABLE_5V_POWER       = (int)( 5000.0 * 0.85 ),
   HL_LOW_5V_POWER              = 4000, // Low mV level for 12V supply.
   HL_HIGH_5V_POWER             = 6000  // High mV level for 12V supply.
};

class HalPowerSupply_CDS
{
public:
   BaseElement<int>& _nValue;
   int               _nLow;
   int               _nHigh;
   int               _nAcceptable;

public:
   HalPowerSupply_CDS(BaseElement<int>& value, int low, int high, int acceptable);
   virtual ~HalPowerSupply_CDS();

   inline int  mV (void) const { return _nValue; }
   inline bool TooHigh (void) const { return ( std::abs(_nValue) > _nHigh ); }
   inline bool TooLow (void) const { return  (std::abs(_nValue) < _nLow ); }
   inline bool InRange (void) const { return ( !TooLow() && !TooHigh() ); }
   inline bool Acceptable (void) const { return ( std::abs(_nValue) > _nAcceptable ); }
};

class HalValve_CDS
   : public SingleWriteDataStore
{
public:
   BaseElement<HAL_VALVESTATE> _order;
   BaseElement<HAL_VALVESTATE> _status;

public:
   HalValve_CDS(const char* name, Role role);
   virtual ~HalValve_CDS();

   inline bool           IsOpen (void) const { return ( _status == (unsigned int)HAL_VALVE_OPEN ); }
   inline bool           IsCollecting (void) const { return ( _status == (unsigned int)HAL_VALVE_COLLECT ); }
   inline bool           IsReturning (void) const { return ( _status == (unsigned int)HAL_VALVE_RETURN ); }
   inline bool           IsUnknown (void) const { return ( _status == (unsigned int)HAL_VALVE_UNKNOWN ); }
   inline HAL_VALVESTATE State (void) const { return _status; }

   // previously commanded value
   inline bool           CmdOpen (void) const { return ( _order == (unsigned int)HAL_VALVE_OPEN ); }
   inline bool           CmdCollect (void) const { return ( _order == (unsigned int)HAL_VALVE_COLLECT ); }
   inline bool           CmdReturn (void) const { return ( _order == (unsigned int)HAL_VALVE_RETURN ); }
   inline HAL_VALVESTATE Command (void) const { return _order; }
};

// Event flags this update
// For Update field, see HAL_EVENTS for defn.
// For all single bit errors, see HAL_ERROR for defn.
// For switches, 1 = pressed, 0 = not pressed.
typedef struct
{
   unsigned int Update               : 4;     // event that caused the Hw update
   unsigned int CentErr              : 1;     // 1 = filtered centrifuge hardware error
   unsigned int CentIntErr           : 1;     // 1 = centrifuge integration error
   unsigned int Vibration            : 1;     // 1 = Vibration detected
   unsigned int ValveFault           : 1;     // 1 = valve hardware error
   unsigned int Cassette             : 1;     // 1 = Cassette error
   unsigned int MotorErr             : 1;     // 1 = Motor fault
   unsigned int SwStop               : 1;     // 1 = Stop button pressed
   unsigned int SwPause              : 1;     // 1 = Pause button pressed
   unsigned int CentErrNoReverse     : 1;     // 1 = centrifuge hardware error with reverse bit masked
} EventFlags;

class HalStatus_CDS
   : public SingleWriteDataStore
{
public:
   HalPump_CDS             ACPump;
   HalPump_CDS             InletPump;
   HalPump_CDS             PlasmaPump;
   HalPump_CDS             CollectPump;
   HalPump_CDS             ReturnPump;

   HalValve_CDS            RBCValve;
   HalValve_CDS            PlasmaValve;
   HalValve_CDS            CollectValve;

   HalPowerSupply_CDS      PS24V;   // +24V supply
   HalPowerSupply_CDS      PS24Vsw; // +24V switched
   HalPowerSupply_CDS      PS64V;   // +64V supply
   HalPowerSupply_CDS      PS64Vsw; // +64V switched
   HalPowerSupply_CDS      PSp12V;  // +12V supply
   HalPowerSupply_CDS      PSm12V;  // -12V supply
   HalPowerSupply_CDS      PS5V;    // +5V supply

   BaseElement<int>        _msgCount;           // sequence count
   BaseElement<EventFlags> _Event;              // events and errors

   BaseElement<float>      _aps;                // current APS pressure in mmHg
   BaseElement<short>      _apsLow;             // current APS Low setting for driver
   BaseElement<short>      _apsHigh;            // current APS high setting for driver

   // Centrifuge info
   BaseElement<float>              _cps;        // Centrifuge pressure in mmHg
   BaseElement<float>              _centRPM;    // current RPM
   BaseElement<float>              _centCmdRPM; // previously commanded RPM

   BaseElement<HAL_FLUIDDETECT>    _acDetect;
   BaseElement<HAL_RESERVOIRSTATE> _reservoir;
   BaseElement<short>              _lowAGC;     // ultrasonic level low sensor
   BaseElement<short>              _highAGC;    // ultrasonic level high sensor

   // Cassette info
   BaseElement<HAL_CASSETTESTATE>      _cassettePos;
   BaseElement<cassette::cassetteType> _cassetteId;

   BaseElement<HAL_DOORORDERS>         _doorOrders; // previously commanded door order
   BaseElement<HAL_DOORSTATE>          _doorStatus; // current door status

   BaseElement<short>                  _LeakValue; // leak detector value from adc
   BaseElement<HAL_FLUIDDETECT>        _Leak;      // lead detector state

   // RBC Detector values
   BaseElement<short>         _redValue;             // red reflectance
   BaseElement<short>         _redDriveValue;        // red LED drive
   BaseElement<short>         _greenValue;           // green refelectance
   BaseElement<short>         _greenDriveValue;      // green LED drive
   BaseElement<unsigned long> _newDriveCounts;       // Track Drive value changes
   BaseElement<unsigned long> _newReflectanceCounts; // Track Reflectance value changes

   // Sound Level
   BaseElement<short> _SoundLevel;              // reported sound level

   // voltage/current levels
   BaseElement<int> _centI;                     // Centrifuge current
   BaseElement<int> _plus5V;                    // +5 volt supply value
   BaseElement<int> _minus12V;                  // -12 volt supply value
   BaseElement<int> _sixtyFourVI;               // +64 volt supply current value
   BaseElement<int> _sixtyFourV;                // +64 volt supply value before switch
   BaseElement<int> _sixtyFourSwV;              // +64 volt supply value after switch
   BaseElement<int> _twelveV;                   // +12 volt supply value
   BaseElement<int> _twentyFourVI;              // +24 volt supply current value
   BaseElement<int> _twentyFourV;               // +24 volt supply voltage value before switch
   BaseElement<int> _twentyFourSwV;             // +24 volt supply voltage value after switch
   BaseElement<int> _EMITemp;                   // temperature in EMI box

public:
   HalStatus_CDS(Role role);
   virtual ~HalStatus_CDS();

   inline int        Count (void) const { return _msgCount; }
   inline HAL_EVENTS Event (void) const { return (HAL_EVENTS)(_Event.Get().Update); }
   inline bool       CentrifugeError (void) const { return (_Event.Get().CentErr != 0); }
   inline bool       CentrifugeErrorNoReverse () const { return (_Event.Get().CentErrNoReverse != 0); }
   inline bool       CentrifugeIntError (void) const { return (_Event.Get().CentIntErr != 0); }
   inline bool       ValveFault (void) const { return (_Event.Get().ValveFault != 0); }
   inline bool       CassetteError (void) const { return (_Event.Get().Cassette != 0); }
   inline bool       MotorError (void) const { return (_Event.Get().MotorErr != 0); }
   inline bool       StopSwitch (void) const { return (_Event.Get().SwStop != 0); }
   inline bool       PauseSwitch (void) const { return (_Event.Get().SwPause != 0); }

   inline float APS (void) const { return _aps; }
   inline float APSLow (void) const { return _apsLow; }
   inline float APSHigh (void) const { return _apsHigh; }

   inline float CPS (void) const { return _cps; }

   inline float CentrifugeRPM (void) const { return _centRPM; }
   inline float CmdCentrifugeRPM (void) const { return _centCmdRPM; }

   inline bool ACDetectFluid (void) const { return _acDetect == HAL_FLUID_DETECTED; }
   inline bool ACDetectError (void) const { return _acDetect == HAL_ERROR_DETECTED; }
   inline bool ACDetectAir (void) const { return _acDetect == HAL_AIR_DETECTED; }

   inline bool ReservoirIsEmpty (void) const { return ( _reservoir == HAL_RESERVOIR_EMPTY ); }
   inline bool ReservoirIsFull (void) const { return ( _reservoir == HAL_RESERVOIR_HIGH ); }
   inline bool ReservoirLowSensorCovered (void) const { return ( _reservoir == HAL_RESERVOIR_MIDDLE || _reservoir == HAL_RESERVOIR_HIGH ); }
   inline bool ReservoirError (void) const { return ( _reservoir == HAL_RESERVOIR_ERROR ); }

   inline int AGCLow (void) const { return _lowAGC; }
   inline int AGCHigh (void) const { return _highAGC; }

   inline bool CassIsUp (void) const { return ( _cassettePos == HAL_CASSETTE_UP ); }
   inline bool CassIsDown (void) const { return ( _cassettePos == HAL_CASSETTE_DOWN ); }
   inline bool CassPosIsUnknown (void) const { return ( _cassettePos == HAL_CASSETTE_UNKNOWN ); }

   inline bool CassIsUnknown (void) const { return ( _cassetteId == cassette::UNKNOWN || _cassetteId == cassette::NOT_LOADED ); }
   inline bool CassIsPltPlsRBC (void) const { return ( _cassetteId == cassette::PLTPLSRBC ); }
   inline bool CassIsRBCPls (void) const { return ( _cassetteId == cassette::RBCPLS ); }

   inline bool DoorIsOpen (void) const { return ( _doorStatus == HAL_DOOR_OPEN || _doorStatus == HAL_DOOR_LOCKED_STATE ); }
   inline bool DoorIsLocked (void) const {    return ( _doorStatus == HAL_DOOR_CLOSED_AND_LOCKED || _doorStatus == HAL_DOOR_LOCKED_STATE ); }
   inline bool DoorIsCommandedLocked (void) const { return ( _doorOrders == (unsigned int)HAL_DOORLOCK ); }

   inline bool LeakDetected (void) const { return _Leak == HAL_FLUID_DETECTED; }
   inline bool LeakError (void) const { return _Leak == HAL_ERROR_DETECTED; }
   inline int  LeakValue (void) const { return _LeakValue; }

   inline int           RedValue (void) const { return _redValue; }
   inline int           RedDriveValue (void) const { return _redDriveValue; }
   inline int           GreenValue (void) const { return _greenValue; }
   inline int           GreenDriveValue (void) const { return _greenDriveValue; }
   inline unsigned long NewDriveCounts (void) const { return _newDriveCounts; }
   inline unsigned long NewReflectanceCounts (void) const { return _newReflectanceCounts; }

   inline int Sound (void) const { return _SoundLevel; }

   inline int CentI (void) const { return (int)(0.002f * _centI); }   // Return Amps
   inline int EMITemp (void) const { return _EMITemp; }
};

#endif /* ifndef __HALSTATUS_CDS_INCLUDE */

/* FORMAT HASH 29b57023bd0553ebe97849087cda1be3 */
