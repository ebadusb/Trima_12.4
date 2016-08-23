#ifndef _PREDICTTYPES_
#define _PREDICTTYPES_
#include "states.h"
#include "recoverytypes.h"
#include "cassette.h"
#include "predict_msgs.h"

#include <cstdlib>

template <class T> inline
const T& MAX (const T& a, const T& b)
{
   return (a > b ? a : b);
}

template <class T> inline
const T& MIN (const T& a, const T& b)
{
   return (a < b ? a : b);
}

// Written to minimize logging. Check use in predict\procState.cpp
template <class T> inline
bool UpdateIfNeq (T& oldVal, T newVal, bool& log)
{
   if (oldVal != newVal)
   {
      oldVal = newVal;
      log    = true;
      return true;
   }
   return false;
}

////////////////////////////////////////////////////////////////////////////////////////////

enum PIRType
{
   PIR_STD = 0,
   PIR_PPC = 1

};

/////////////////////////////////////////////////////////////////////////////////////////////
//
struct Donor
{
   int   id; /* not used in trima */
   float height;
   float weight;
   float tbv;
   float preCount;
   float hct;
   float initialHct;
   float bmi;
   bool  female;
   int   bloodType;
   float sampleVol;
};

// Volumes processed
struct Volumes  // Volumes_CDS
{
   float Vin;
   float Vac;
   float Vrep;
   float VSalineBolus;
   float VincvTime;
   float Vincv;
   float VinEpcaStart;

   float VrbcS1;
   float VrbcS2;
   float VrbcPrime;

   float VrbcS1Double;
   float VrbcS2Double;
   float VrbcDouble;
   float rbcDoubleDose;

};

// What's in the bag?
struct Bag // Volumes_CDS
{
   float Vplasma;
   float Vac_plasma;
   float Yield;
   float Vcol;
   float Vac_col;
   float Vrbc;
   float Vac_rbc;
   float rbcDose;
   float Vrbc_a_final;
   float Vrbc_b_final;

};

struct Run // ProcRun_CDS
{
   State_names         substate;
   float               QchMin;
   float               Qch;
   int                 npurges;
   int                 nScheduledPurges;
   float               adjustedHct;
   RecoveryTypes::Type recovery;
   float               VinClearLine;
   float               WbcPurgeStartTime;
   int                 WbcPurgeType;
   float               WbcLastProcPurge;
   bool                PirSpillover;
   bool                PtfRbcProd1Disabled;
   bool                FirstDrawComplete;
   bool                FirstCycleComplete;
   float               SubstateStartTime;
   short               MidrunEntered;
   bool                StopRamping;
   float               DeadRampMaxQin;
   float               LastCentrifugeSpeed;
};

struct Time // Time_CDS
{
   float procRunTime;
};

struct Cassette
{
   cassette::cassetteType type;
   bool                   rbcDetectorCalibrated;
   bool                   PtfCpsTestPassed;
};

struct AdjustmentCaps
{
   float Qin;
   float Qrp;
   float ir;
   float ratio;
};

struct Cycle
{
   float Hrbc;
   float VAccumOverdraw;
   float ResCrit;
};

struct SPredict
{
   bool           OptimizingSelProc;
   float          PirQinLimit;
   float          PirCsf;
   float          AccumYdPur;
   float          AccumPurges;
   float          LastCalcQchmax;
   float          PIRLockin;
   float          Qcmr;
   float          PlateletPlasmaQch;
   DoPrediction_t PredictRequest;
   float          PrePirQch;
};


struct CurrentConditions
{
   Donor    donor;
   Volumes  volumes;
   Bag      bag;
   Run      run;
   Time     time;
   Cassette cassette;
   Cycle    cycle;
   SPredict predict;
};



#endif /* _PREDICTTYPES_ */

/* FORMAT HASH b339b2da69e2b9d146bb8ce337915015 */
