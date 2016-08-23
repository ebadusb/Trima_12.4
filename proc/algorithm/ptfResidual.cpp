/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      ptfResidual.cpp
 *
 * ABSTRACT:   This class calculates the donor body mass index
 *
 */


#include <math.h>
#include <vxworks.h>
#include "cobeconfig.h"
#include "ptfResidual.h"



#ifdef WIN32
      #include <iostream>
using namespace std;
#else
    #include "procdata.h"
    #define cout    DataLog(log_level_proc_debug)
    #define endl    endmsg
#endif

const float PTF_DOSE = CobeConfig::data().MssPtfVolume * 0.8f;



float PTFresidual::calculateDose (float rasVolume)
{
   float rbcDose = -1.0;

   if ( (rasVolume >= 0.0f) && (rasVolume <= 23.0f) )
   {
      rbcDose = ((-0.4386f * rasVolume) + PTF_DOSE);

   }
   else if ( (rasVolume > 23.0f) && (rasVolume < 126.5f) )
   {
      rbcDose = (415.0f * pow(rasVolume, -0.77f));

   }
   else if (rasVolume >= 126.5f)
   {
      rbcDose = 10.0f;
   }


   //  old equation valid for 100+ RAS
   // float rbcDose = ((-0.2539f * rasVolume) + PTF_DOSE);   // cc the 47.2

   // limiting factors
   if (rbcDose < 10.0f)
      rbcDose = 10.0f;


   // cout << "PTFresidual::calculate( " << rasVolume << ") =  " << rbcDose/0.8f << endl;
   return rbcDose;

}

/* FORMAT HASH 05d8d573d0375f10a84aa2fbe96a727a */
