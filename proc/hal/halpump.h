/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/hal/rcs/halpump.h 1.6 2007/08/23 19:24:56Z spriusb Exp jheiusb $
 * $Log: halpump.h $
 * Revision 1.6  2007/08/23 19:24:56Z  spriusb
 * Fix for IT 7742:  Additional interfaces for stroke volumes to support service mode
 * Revision 1.5  2002/10/28 21:49:05Z  jl11312
 * - modified HAL to be part of proc instead of a separate task
 * - modified to handle the HAL status common data store
 *
 */

#ifndef _HALPUMP_INCLUDE
#define _HALPUMP_INCLUDE

#include "chw.hpp"
#include "haldata.h"
#include "halstatus_cds.h"
#include "trimamessages.h"

/*
 *  Pump Calibration constants.
 */

#define PUMP_A0                            (0.837f)

// *****************************************************************************
// New pump calibration variables put in 9/22/96 with Mark B. data.
#define AC_PUMP_A0                         (0.814f)  // = 0.837 * (100% -2.7%)
#define PLASMA_PUMP_A0                     (0.794f)  // = 0.837 * (100% -5.15%)
#define COLLECT_PUMP_A0                    (0.794f)  // = 0.837 * (100% -5.15%)
#define REPLACEMENT_PUMP_A0                (0.814f)  // = 0.837 * (100% -2.7%)
// *****************************************************************************

#define INLET_PUMP_A0                      (0.812f * 0.985f) // IT5240
#define PUMP_A1                            (1.4154e-4f)
#define PUMP_A2                            (1.8167e-4f)

#define AC_PUMP_INLET_PRESSURE             (30.0f)

#define PLASMA_PUMP_INLET_PRESSURE         (250.0f)
#define PLASMA_PUMP_MULT                   (1.043f)    // Mark Brierton factor added 1/15/97

#define COLLECT_PUMP_INLET_PRESSURE        (250.0f)

#define REPLACEMENT_PUMP_INLET_PRESSURE    (30.0f)

/*
 *  Pump max RPM constants
 */

#define MAX_INLET_RPM    (250.0f)
#define MAX_AC_RPM       (250.0f)
#define MAX_PLASMA_RPM   (250.0f)
#define MAX_COLLECT_RPM  (250.0f)
#define MAX_RETURN_RPM   (400.0f)
#define MAX_REPLACEMENT_RPM (250.0f)


/*
 *  Dynamic pump stroke volume for return pump.  Coefficients derived from
 *  experiments done by Mark Brierton.  Mark recorded encoder counts and
 */

#define RETURN_PUMP_STROKE_VOLUME  (2.73713f)


// Base class for all pumps.  Provides standard interface.

class CPump
{

public:
   CPump(float MaxRPM);
   virtual ~CPump();

   // Update pump info in HalPumpStatus given hw status info
   void UpdatePump (HalPump_CDS& stat);

   // obtain the pump specific stroke volume in mls/rev
   // store the this as current stroke volume and return.
   virtual float StrokeVolume (float InletP, float CmdFlow) = 0;
   // Just return the current stroke volume
   float StrokeVolume (void) const;

   // conversion equations based on the current stroke volume
   float RPM (float flowrate) const;
   float Flow (float RPM) const;


protected:
   void Init (void);
   // Just set the stroke volume
   float StrokeVolume (float sv);

private:
   float m_fStrokeVolume;
   float m_fMaxRPM;

};

// Derived pumps with specific stroke volume equations

class CPumpInlet
   : public CPump
{

public:
   CPumpInlet();
   virtual ~CPumpInlet();

   virtual float StrokeVolume (float InletP, float CmdFlow);
   float         StrokeVolume (void) const { return CPump::StrokeVolume(); }
};


class CPumpAC
   : public CPump
{

public:
   CPumpAC();
   virtual ~CPumpAC();

   virtual float StrokeVolume (float InletP, float CmdFlow);
   float         StrokeVolume (void) const { return CPump::StrokeVolume(); }
};

class CPumpReturn
   : public CPump
{

public:
   CPumpReturn();
   virtual ~CPumpReturn();

   virtual float StrokeVolume (float InletP, float CmdFlow);
   float         StrokeVolume (void) const { return CPump::StrokeVolume(); }
};

class CPumpPlasma
   : public CPump
{

public:
   CPumpPlasma();
   virtual ~CPumpPlasma();

   virtual float StrokeVolume (float InletP, float CmdFlow);
   float         StrokeVolume (void) const { return CPump::StrokeVolume(); }
};

class CPumpCollect
   : public CPump
{

public:
   CPumpCollect();
   virtual ~CPumpCollect();

   virtual float StrokeVolume (float InletP, float CmdFlow);
   float         StrokeVolume (void) const { return CPump::StrokeVolume(); }
};

class CPumpReplace
   : public CPump
{

public:
   CPumpReplace();
   virtual ~CPumpReplace();

   virtual float StrokeVolume (float InletP, float CmdFlow);
   float         StrokeVolume (void) const { return CPump::StrokeVolume(); }
};


////////////////////////////////////////////////////////////////////
// Pump sets based on cassette or procedure

class CPumpSet
{

public:
   virtual ~CPumpSet();

protected:
   CPumpSet();

   static bool pump_stroke_volume_logging;
   static bool pump_stroke_volume_logging_set;
};

class CNormPumpSet
   : public CPumpSet
{

public:
   CNormPumpSet();
   virtual ~CNormPumpSet();

   // Update stroke volumes and calculate delta volumes and actual flows
   void UpdatePumps (HalStatus_CDS& stat);
   // Update Stroke volumes and calculate RPM commands
   void ObtainRPMs (HalStatus_CDS& stat);
   // update Stroke volumes and DERIVE flow commands (Service Mode)
   void TransferPumpOrders (const CHwOrders& ctrl, HalOrders& orders);
   // Update the stroke volumes using pressures and cmd flows
   void UpdateStrokeVolumes (HalStatus_CDS& stat);


private:
   CPumpInlet   m_pmpInlet;
   CPumpPlasma  m_pmpPlasma;
   CPumpAC      m_pmpAC;
   CPumpReturn  m_pmpReturn;
   CPumpCollect m_pmpCollect;

};

class CDRBCPumpSet
   : public CPumpSet
{

public:
   CDRBCPumpSet();
   virtual ~CDRBCPumpSet();

   // Update stroke volumes and calculate delta volumes and actual flows
   void UpdatePumps (HalStatus_CDS& stat);
   // Update Stroke volumes and calculate RPM commands
   void ObtainRPMs (HalStatus_CDS& stat);
   // update Stroke volumes and DERIVE flow commands (Service Mode)
   void TransferPumpOrders (const CHwOrders& ctrl, HalOrders& orders);
   // Update the stroke volumes using pressures and cmd flows
   void UpdateStrokeVolumes (HalStatus_CDS& stat);



protected:

   static const float PLASMA_PUMP_INLET_PRESSURE_RBC1;
   static const float PLASMA_PUMP_INLET_PRESSURE_RBC2;

private:
   CPumpInlet   m_pmpInlet;
   CPumpPlasma  m_pmpPlasma;
   CPumpAC      m_pmpAC;
   CPumpReturn  m_pmpReturn;
   CPumpCollect m_pmpReplace;

};

#endif /* ifndef _HALPUMP_INCLUDE */

/* FORMAT HASH 7cf9ed76f5ba247d9ad52b59a2f8fefc */
