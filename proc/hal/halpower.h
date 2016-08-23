/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#ifndef _HALPOWER_INCLUDE
#define _HALPOWER_INCLUDE

// Conversion values for Power Supply
#define PSGN_M12V   (0.006161f)
#define PSOF_M12V   (-25.55f)
#define PSGN_P5V    (0.002f)
#define PSOF_P5V    (0.0f)
#define PSGN_P12V   (0.00611f)
#define PSOF_P12V   (0.0f)
#define PSGN_24V    (0.011f)
#define PSOF_24V    (0.0f)
#define PSGN_SW24V  (0.011f)
#define PSOF_SW24V  (0.0f)
#define PSGN_64V    (0.0531f)
#define PSOF_64V    (0.0f)
#define PSGN_SW64V  (0.0531f)
#define PSOF_SW64V  (0.0f)


class CPwrSupply
{
public:
   CPwrSupply(float gain, float offset); // Supply the conversion constants in the constructor
   ~CPwrSupply();

   float mVolts (short raw);
   float mVolts (void) const;

// Hide the default constructor
private:
   CPwrSupply();

private:
   float m_fMv;
   float m_fGain;
   float m_fOff;
};

#endif /* ifndef _HALPOWER_INCLUDE */

/* FORMAT HASH bb59beb22e002a940026e2b6068eb511 */
