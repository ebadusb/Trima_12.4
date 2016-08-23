/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/proc/hal/rcs/halsense.h 1.3 2003/07/07 21:03:14Z jl11312 Exp jl11312 $
 * $Log: halsense.h $
 * Revision 1.3  2003/07/07 21:03:14Z  jl11312
 * - implemented APS auto-zero
 * Revision 1.2  2002/10/28 21:49:06Z  jl11312
 * - modified HAL to be part of proc instead of a separate task
 * - modified to handle the HAL status common data store
 *
 */

#ifndef _HALSENSE_INCLUDE
#define _HALSENSE_INCLUDE


// Initial defaults for centrifuge pressure cal
#define INITIAL_CPS_GAIN  (1.311104697f)
#define INITIAL_CPS_OFFSET (-886.1257588f)

// Initial Defaults for APS Pressure Cal
#define INITIAL_RAWNEG    (1075.0)
#define INITIAL_RAWREF    (1840.0)
#define INITIAL_RAWPOS    (2850.0)
#define INITIAL_ACTNEG    (-250.0)
#define INITIAL_ACTREF    (0.0)
#define INITIAL_ACTPOS    (330.0)


#define LEAK_FAULT_VALUE 3000      // leak detector reading > 3000 -> error
#define NO_LEAK_VALUE 2000         // leak detector reading > 2000, < 3000 -> no leak


////////////////////////////////////////////////////////////////
// Generic interface

/*******************************************************************************
 *
 *  CPS CLASS DEFINITION
 *
 *
 ******************************************************************************/

class CSenseCPS
{

public:
   CSenseCPS();
   ~CSenseCPS();

   void SetCal (float gain, float offset);

   // set the current pressure and convert to raw units
   short Raw (float mmHg);
   // obtain the current pressure in raw units
   short Raw (void) const;

   // set the current pressure and return it in mmhg
   float mmHg (short raw);
   // return the current pressure in mmhg
   float mmHg (void) const;


private:
   float m_fGain;       // Pressure sensor gain
   float m_fOff;        // Pressure sensor offset
   float m_fPres;       // Current pressure sensor reading in mmHg

};

/*******************************************************************************
 *
 *  CPS CLASS DEFINITION
 *
 *
 ******************************************************************************/

class CSenseAPS
{

public:
   CSenseAPS();
   ~CSenseAPS();

   void SetCal (float rNeg, float vNeg, float rRef, float vRef, float rPos, float vPos);

   void SetZero (short zero);
   void ResetZero (void);

   // set the current pressure and convert to raw units
   short Raw (float mmHg);
   // obtain the current pressure in raw units
   short Raw (void) const;

   // set the current pressure and return it in mmhg
   float mmHg (short raw);
   // return the current pressure in mmhg
   float mmHg (void) const;

private:
   void ComputeGains (void);

private:
   float m_fRNeg;           // raw calibration data - converted to positive/negative gain
   float m_fVNeg;
   float m_fRRef;
   float m_fVRef;
   float m_fRPos;
   float m_fVPos;

   float m_fZero;           // zero point raw reading (dynamically set at cassette load)
   float m_fPosGain;        // gain for readings above m_fZero
   float m_fNegGain;        // gain for readings above m_fZero;

   float m_fPres;       // Current pressure sensor reading in mmHg
   float m_fRaw;        // Current raw reading

};

#endif /* ifndef _HALSENSE_INCLUDE */

/* FORMAT HASH 2e21836b6aee6355501f5165ae535ddb */
