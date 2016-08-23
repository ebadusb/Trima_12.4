/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      procdetpltplsrbc.h
 *
 */

#ifndef PROCDET_PLTPLSRBC_INCLUDED
#define PROCDET_PLTPLSRBC_INCLUDED

#include "procdet.h"
#include "piralgo.h"
#include <sstream>


struct IntermediateResults;

class procedureDetailPltPlsRBC
   : public procedureDetail
{
   friend class PirAlgo;
   friend class PirAlgoLR;

private:
   procedureDetailPltPlsRBC(const procedureDetailPltPlsRBC&);              // not implemented
   procedureDetailPltPlsRBC& operator = (const procedureDetailPltPlsRBC&); // not implemented

public:
   procedureDetailPltPlsRBC(PIRType pt);
   virtual ~procedureDetailPltPlsRBC();

   virtual void LogTargets () const;
   virtual void LogFirstReturnNoCollectFlows () const;
   virtual void SetConstants ();

   virtual float instantaneousInfusion (int substateindex) const;
   virtual int   AlgSubstate (const State_names procsubstate) const;

   virtual void generatesubstates ();
   virtual int  RBCState () const;

   virtual int RBCDoubleState () const;

   virtual bool PlasmaHctOK () const;

   virtual int                      NoMorePlatelets () const;
   virtual INTERNAL_PREDICT_RESULTS PostCheckProcedure ();
   virtual INTERNAL_PREDICT_RESULTS PreCheckProcedure ();
   virtual float                    VinTarget () const;
   virtual float                    VinPIRStart () const;
   virtual float                    YdchNoPIR () const;
   virtual float                    YdchPIR () const;
   virtual float                    getPIRLockin () const { return _pirAlgo->getLockin(); }
   virtual float                    getPrePltPlasmaEndQin () const { return _prePltPlsEndOfRamp.Qininstd; }
   virtual float                    getPrePltNoPlasmaEndQin () const { return _prePltNoPlsEndOfRamp.Qininstd; }



#ifdef __WIN32__
   virtual void ResetProcDet ();
#endif
   virtual void SetPIRRequired (bool value) { return _pirAlgo->setPirRequired(value); }
   virtual int  GetPIRRequired () const {return _pirAlgo->getPirRequired(); }

   virtual void AccumulateVolumes ();  // overridden from base class


   virtual float CalculateCsf (int substateId) const;
   virtual float getCollectMaxFlow (void) const {return _CollectMaximumFlow; }

   // sloppy!, but this i going away i think
   virtual bool doingPTFDouble () const { return false; }        // is it a double for PTF ?
   virtual bool secondPartofPTFDouble () const { return false; } // is it in the second part of the double for PTF? (after split)

   static float getWBCRmpUpRmpDnPurgeTime (float centrifugeSpeed, const CobeConfig::CobeConfigStruct& cobeConfig);
private:

   virtual void do_SetTargets (substates_t&);

protected:
   // const bool DidPirBracketChange(float oldlockin, float newlockin) const;
   virtual float getYield (float Vin, float Vac);
   virtual void  countRemainingPurges (float startVin, float endVin);

   // was calculateYdch
   virtual float Ydch (float Vin, int PIRRequired) const;


   virtual void  setHypovolemia ();
   virtual void  initialFlowRamp (float Vin, float& QinLimit, float& QnLimit);
   virtual float limitState (const IntermediateResults& currentsubstate,
                             const IntermediateResults& previoussubstate,
                             float deltavp,
                             float deltavc,
                             float deltavin,
                             float maxvin,
                             int endonyield = 1);

   void Toplevel ();
   void ChannelSetup        (IntermediateResults&, IntermediateResults&);
   void PreplateletPlasma   (IntermediateResults&, IntermediateResults&);
   void PreplateletNoplasma (IntermediateResults&, IntermediateResults&);
   void PlateletPlasma      (IntermediateResults&, IntermediateResults&);
   void PCA                 (IntermediateResults&, IntermediateResults&);
   void Midrun              (IntermediateResults&, IntermediateResults&);
   void PIRPlasma           (IntermediateResults&, IntermediateResults&);
   void PIRNoplasma         (IntermediateResults&, IntermediateResults&);
   void EPCA                (IntermediateResults&, IntermediateResults&);
   void ExtendedPlasma      (IntermediateResults&, IntermediateResults&);

   void RBCSetup1           (IntermediateResults&, IntermediateResults&);
   void RBCSetup2           (IntermediateResults&, IntermediateResults&);
   void RBCPrime            (IntermediateResults&, IntermediateResults&);
   void RBC                 (IntermediateResults&, IntermediateResults&);

   //
   // Determine which config variable to use based on donor BMI ...
   float MNCPrecountForPurge () const;
   float PostPurgeWBCInChamber () const;


   void setChamberFlowConstants (); // set flow constants based on concentration

   void UpdateYdpur ();
   // const float Npur(const float Vin, int substateId) const;
   float Ypur (int substateId) const;
   float Ydpur (float Vin, int substateId) const;
   // void CalculateQcmr();
   void  CalculateVincv ();
   void  PrecalculateFlows ();
   void  CalculatePurgePoints ();
   float CalculatePurgeTime (float vintarg, float purgePoint);

   // PIR Ramp related calculations.
   float       PirCsf () const;
   std::string LogQin (int subndx) const;
   std::string LogQp (int subndx) const;
   bool        InPir () const
   {
      return ( ( _startingsubstate == Config::__PIR_PLASMA) ||
               (_startingsubstate == Config::__PIR_NOPLASMA) );
   }

   float GetRBCCentSetupVolume ();
   // AG: Review
   void CalculateQcmr ();
#ifdef __WIN32__
public:
#endif
   float MovePurgePoint (float vinPur);

protected:

   int   _startingsubstate;
   float _Vincv;                    // Vin when the collect valve opens
   float _VincvTime;                // Time when collect valve opens
   float _Qnmax;
   float _Qinr;                     // Qin at end of inlet flow ramp
   float _QinrQnLimit;              // Qin at end of inlet flow ramp
   float _TBV;
   float _hct;
   float _precount;
   float _bmi;
   float _ratio;
   float _oneM1Ratio;
   float _totalCollectTime;
   float _QcmrTime;
   float _QcmrVol;
   bool  _latchQcmr;
   float _Yield;
   float _PIRAvgQin;
   float _RBCTime;
   float _EPCAVc;
   float _PCAVc;
   float _maxEPCAVc;
   float _PCAmlVinpermlVc;
   float _EPCAmlVinpermlVc;
   float _MidrunmlVinpermlVc;
   float _ACinRBC;
   float _RBCVin;
   float _RBCVac;
   float _VinPIR;
   float _PIRVcAtMax;
   float _VcOvercollect;
   float _lastPredVc;
   bool  _lastPredPIR; // true -> yeah do a PIR.

   float _RBCSetup1Time;
   float _RBCSetup1Vin;
   float _RBCSetup1Vac;

   float _RBCSetup2Time;
   float _RBCSetup2Vin;
   float _RBCSetup2Vac;

   float _RBCPrimeTime;
   float _RBCPrimeVin;
   float _RBCPrimeVac;

   bool  _PTF;

   //
   //
   //   Values to keep track of purges and related data
   //
   int           _purges;
   float         _deltaPurge;
   float         _Ypur;
   bool          _purgeSkipped;
   float         _purgeTime;             // time in minutes of a single scheduled purge
   float         _purgeEndTime;          // end time of a purge in progress
   float         _purgeQcmrLock;         // value to lock in during purge
   bool          _deadbandBeingEnforced; // Qcmr limited to deadband
   float         _QchConvergenceFloor;   // Qch cannot drop below this if convergence fails for volume
   mutable float _Fw;                    // Fraction of WBCs; mutable's there because we are using this variable only for logging
   // and we are assigning to Fw in a const function.
   //
   //
   //  Starting conditions
   //
   float _currentvp;
   float _currentacinvp;
   float _currentvc;
   float _currentacinvc;
   float _currenttime;
   float _currentQch;
   float _Qch;
   float _prevQcmr;
   float _EndVinPir;


   // set from CC based on conectration of PLT product;
   float     _CollectMaximumFlowLock;
   float     _CollectMaximumFlow;

   float     _QcmrConvCoeff;
   float     _nextProcPurge;
   float     _nextProcPurgeTime;

   IPirAlgo* _pirAlgo;

   float     _DTp;
   bool      _ppcQcmrDropFlag;


   IntermediateResults _prePltPlsEndOfRamp;
   IntermediateResults _prePltNoPlsEndOfRamp;
};

#endif

/* FORMAT HASH 68ce59e1d0e054101703e927e126415b */
