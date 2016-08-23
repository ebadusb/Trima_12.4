/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/proc/hal/rcs/halsense.cpp 1.3 2003/07/07 21:03:10Z jl11312 Exp jl11312 $
 * $Log: halsense.cpp $
 * Revision 1.3  2003/07/07 21:03:10Z  jl11312
 * - implemented APS auto-zero
 * Revision 1.2  2002/10/28 21:49:05Z  jl11312
 * - modified HAL to be part of proc instead of a separate task
 * - modified to handle the HAL status common data store
 *
 */

#include <vxWorks.h>
#include "halsense.h"


/*******************************************************************************
 *
 *  CSenseCPS
 *
 * Centrifuge pressure sensor
 *
 ******************************************************************************/

CSenseCPS::CSenseCPS()
	: m_fPres(0)
{
   SetCal(INITIAL_CPS_GAIN, INITIAL_CPS_OFFSET);
}

CSenseCPS::~CSenseCPS()
{}

void CSenseCPS::SetCal (float gain, float offset)
{
   m_fGain = gain;
   if ( m_fGain == 0 )
   {
	   m_fGain = INITIAL_CPS_GAIN;
   }
   m_fOff  = offset;
}

short CSenseCPS::Raw (float mmhg)
{
   m_fPres = mmhg;
   float tmp = ( mmhg - m_fOff ) / m_fGain;
   return static_cast<short>(tmp);
}

short CSenseCPS::Raw () const
{
   float tmp = ( m_fPres - m_fOff ) / m_fGain;
   return static_cast<short>(tmp);
}

float CSenseCPS::mmHg (short raw)
{
   m_fPres = static_cast<float>(raw * m_fGain + m_fOff);
   return m_fPres;
}

float CSenseCPS::mmHg () const
{
   return m_fPres;
}

/*******************************************************************************
 *
 *  CSenseAPS
 *
 * Access pressure sensor
 *
 ******************************************************************************/

CSenseAPS::CSenseAPS()
   : m_fRNeg(INITIAL_RAWNEG), m_fVNeg(INITIAL_ACTNEG),
     m_fRRef(INITIAL_RAWREF), m_fVRef(INITIAL_ACTREF),
     m_fRPos(INITIAL_RAWPOS), m_fVPos(INITIAL_ACTPOS)
{
   ComputeGains();
   mmHg(INITIAL_RAWREF);
}

CSenseAPS::~CSenseAPS()
{}

void CSenseAPS::SetCal (float rNeg, float vNeg, float rRef, float vRef, float rPos, float vPos)
{
   m_fRNeg = rNeg;
   m_fVNeg = vNeg;

   m_fRRef = rRef;
   m_fVRef = vRef;

   m_fRPos = rPos;
   m_fVPos = vPos;

   ComputeGains();
   mmHg(m_fRaw);
}

void CSenseAPS::ResetZero (void)
{
   ComputeGains();
}

void CSenseAPS::SetZero (short zero)
{
   m_fZero = zero;
   mmHg(m_fRaw);
}

void CSenseAPS::ComputeGains (void)
{
   float negDenom = m_fRNeg - m_fRRef;
   float posDenom = m_fRPos - m_fRRef;

   m_fNegGain = (negDenom != 0.0) ? (m_fVNeg - m_fVRef) / negDenom : 0.0;
   m_fPosGain = (posDenom != 0.0) ? (m_fVPos - m_fVRef) / posDenom : 0.0;

   if ( m_fVRef > 0 && m_fNegGain != 0 )
   {
      m_fZero = m_fRRef - m_fVRef / m_fNegGain;
   }
   else if ( m_fVRef < 0 && m_fPosGain != 0)
   {
      m_fZero = m_fRRef - m_fVRef / m_fPosGain;
   }
   else
   {
      m_fZero = m_fRRef;
   }
}

short CSenseAPS::Raw (float mmhg)
{
   m_fPres = mmhg;
   if ( m_fPres > 0 && m_fPosGain != 0 )
   {
      m_fRaw = m_fZero + m_fPres / m_fPosGain;
   }
   else if ( m_fPres < 0 && m_fNegGain != 0 )
   {
      m_fRaw = m_fZero + m_fPres / m_fNegGain;
   }
   else
   {
      m_fRaw = m_fZero;
   }

   return static_cast<short>(m_fRaw);
}

short CSenseAPS::Raw () const
{
   return static_cast<short>(m_fRaw);
}

float CSenseAPS::mmHg (short raw)
{
   m_fRaw  = raw;
   m_fPres = (m_fRaw - m_fZero) * ( (m_fRaw>=m_fZero) ? m_fPosGain : m_fNegGain );
   return m_fPres;
}

float CSenseAPS::mmHg () const
{
   return m_fPres;
}

/* FORMAT HASH cefd3491d8749f6fbdc0867b454c3648 */
