#ifndef rbcdat_h
#define rbcdat_h

#include "alarms.h"

//
//  This class encapsulates the rbc.dat calibration file.  It is designed
//  as a static class, i.e., there is only one rbc.dat file per task
//  and each task has a single instance of the file.
//
//  At the start of task execution, it is the task's responsibility to
//  initialize() this class.  From that point on, any object within
//  the task can access a value via a reference:
//     float tmp = RbcDat::data().RbcRawValue*gain;
//
//  alternately, after the initialization, the task could create a singleton
//  data reference for use:
//     const RbcDat::RbcDatStruct &rbcdat = RbcDat::data();
//  at which point the object would reference the values like:
//     float tmp = rbcdat.RbcRawValue*gain;
//
//
//  To add a value to the rbc.dat file:
//    1: Change rbc.dat to include the value
//    2: Add the equivalent value to this structure
//    3: Add an initialization statement in initialize() in rbcdat.cpp
//
class RbcDat
{

public:
   struct RbcDatStruct
   {
      float InitRedReturn;
      float DesiredRBCRatio;
      float RBCRatioTolerance;
      float RBCRatioFilter;
      float RedLowPltPlsRBCStamp;
      float RedHighPltPlsRBCStamp;
      float GreenLowPltPlsRBCStamp;
      float GreenHighPltPlsRBCStamp;
      float RedLowGreyStamp;
      float RedHighGreyStamp;
      float GreenLowGreyStamp;
      float GreenHighGreyStamp;
      float RedLowRBCPlsStamp;
      float RedHighRBCPlsStamp;
      float GreenLowRBCPlsStamp;
      float GreenHighRBCPlsStamp;

      float SpilloverTriggerRatio;
      int   SpilloverTriggerSamples;

      int   RBCTestMinRed;
      int   RBCTestMaxRed;
      int   RBCTestMinGreen;
      int   RBCTestMaxGreen;
      int   RBCTestFreq;
      int   RBCTestSamples;

      float MinimumCassetteReflection;
      float MaximumCassetteReflection;

      // # [VERIFY_FLAGS]
      // Number of Peak-to-Peak RG ratio samples constituting a full set for
      // Platelet Contaminant detection.
      int RGP2Psamples;
      // Frequency of Peak-to-Peak RG ratio sampling for Platelet Contaminant detection,
      // expressed as a number of .5 second intervals.
      int   RGP2Pfrequency;
      // Red/Green Ratio trigger for declaring Platelet Contamination.
      float RGP2Ptrigger;
      // Factor of actual platelet yield from which the RBC-detector integrated yield
      // must deviate before a PLATELET_VOLUME_ERROR reason is declared.
      float YieldCutoffConst;

      // # [INTEGRATED_YIELD]
      // # Yield Integration constants used to compute RBC Detector's Yield
      float VinStartSamplingREDpl;
      float dVinSamplingREDpl;
      float YieldReflectancePltPlsRBCSet;

      float LowConcentrationTriggerJapan;
      float LowConcentrationTrigger;
      float LowConcentrationThreshold;
      float LowConcentrationShutoffConcentration;

      // [CHAMBER_SATURATION]
      float ChamberSatFsat;
      bool  ChamberSatSWs3a;
      bool  ChamberSatSWs3b;
      bool  ChamberSatSWs4a;
      bool  ChamberSatSWs4b;

      // [PRIME_HEMOLYSIS]
      bool CheckForPrimeHemolysis;
   };


public:
   //
   //
   //   When you initialize me I will read in the file
   //   and report back with an alarm value if something goes wrong.  I do
   //   not actually raise an alarm.  If everything initializes properly
   //   I return NULL_ALARM.
   //
   static ALARM_VALUES        initialize (bool logError = false);
   static const RbcDatStruct& data ();

protected:
   //
   //
   //   My static singleton instance of the data
   //
   static RbcDatStruct _data;

//
//
//   These methods are currently protected since we do not expect
//   anyone to actually instantiate a RbcDat class.
//
protected:
   RbcDat();
   virtual ~RbcDat();

};
#endif

/* FORMAT HASH d3d2a5000159f056812e918eb852205e */
