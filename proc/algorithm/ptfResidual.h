/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      ptfResidual.h
 *
 * ABSTRACT:   This class calculates the donor body mass index
 *
 */


#ifndef _PTF_RESIDUAL_H_  //  Prevents multiple inclusions.
#define _PTF_RESIDUAL_H_




class PTFresidual
{
public:

   // returns RBC DOSE left in the filter after metering x volume
   static float calculateDose (float rasVolume); // rasVolume past the valve

};

#endif

/* FORMAT HASH ef73a860e4e28399ebe8d4637523732e */
