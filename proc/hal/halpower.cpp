/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 * $Log$
 *
 */


#include <vxWorks.h>

#include "halpower.h"

CPwrSupply::CPwrSupply()
   : m_fMv(0),
     m_fGain(1000.0f),
     m_fOff(0)
{}

CPwrSupply::CPwrSupply(float gain, float offset)
   : m_fMv(0),
     m_fGain(gain * 1000.0f),
     m_fOff(offset * 1000.0f)
{
   if (m_fGain == 0)
   {
      m_fGain = 1;
   }
}

CPwrSupply::~CPwrSupply()
{}

float CPwrSupply::mVolts (short raw)
{
   m_fMv = raw * m_fGain + m_fOff;

   return m_fMv;
}

float CPwrSupply::mVolts () const
{
   return m_fMv;
}

/* FORMAT HASH 210afe7770b29972269a87e8918d2e66 */
