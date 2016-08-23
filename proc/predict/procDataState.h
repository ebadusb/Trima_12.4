#ifndef _PROCDATASTATE_
#define _PROCDATASTATE_


#include "predict_types.h"
#include "procdonor_cds.h"
#include "procrun_cds.h"
#include "procadjust_cds.h"
#include "procvolumes_cds.h"
#include "proctime_cds.h"
#include "proctrimaset_cds.h"
#include "proccycle_cds.h"
#include "procadjust_cds.h"
#include "predict_cds.h"
#include "recoverytypes.h"
#include "states.h"
#include "cassette.h"
#include "bloodtype.h"
#include "predict_cds.h"
#include "postcount.hpp"
#include "posthct.hpp"
#include "hypovolemia.h"
#include "cassette.h"
#include "substatemap.h"

#include <ctime>

// Defined in WinBase.h.
// (_MSC_VER >= 1500)
// [... editorial comments redacted... ]
#ifdef Yield
# undef Yield
#endif

//////////////////////////////////////////////////////////////////////////
//
//  ProcDataSate
//
//  This object is a singleton, accessed via the static Instance method.  It is
//  initialized on first access.
//
//  This initialization is not thread safe, and its reentrancy is somewhat
//  debateable; neither is an issue if it is only used in the Predict task.  Use
//  of this object in other tasks may need some coordination.
//
//  Note that the important initialization is done via the nonstatic update
//  member function, which is neither thread-safe nor reentrant (although it
//  safely accesses a CDS object); calling this function from multiple threads
//  will need locking or other coordination.


class ProcDataState
{

public:

   // Singleton access
   static ProcDataState& Instance ();

   // Client has to call update before accessing "current" data
   virtual bool Update ();

   CurrentConditions const&            Get () const { return _current; }
   void                                Set (CurrentConditions& cc) { _current = cc; }
   cassette::cassetteType              CassetteType () const { return _setCDS.Cassette.Get(); }
   ProcTrimaSet_CDS::CassetteStateType CassetteState () const { return _setCDS.CassetteState.Get(); }
   unsigned int                        GetCassetteFunctionBits () const { return _runCDS.CassetteFunctionBits.Get(); }
   AdjustmentCaps const&               AdjCaps ();
   void                                LogCurrentConditions () const;

   float Yield () const { return _current.bag.Yield; }
   float RbcDose () const { return _current.bag.rbcDose; }
   float RbcVolume () const { return _current.bag.Vrbc; }
   float AcInRbc () const { return _current.bag.Vac_rbc; }
   float PlasmaVolume () const { return _current.bag.Vplasma; }
   float AcInPlasma () const { return _current.bag.Vac_plasma; }
   float CollectVolume () const { return _current.bag.Vcol; }
   float AcInCollect () const { return _current.bag.Vac_col; }
   float RbcProduct_A_FinalVolume () const {return _current.bag.Vrbc_a_final; }
   float RbcProduct_B_FinalVolume () const {return _current.bag.Vrbc_a_final; }

   float RbcSetup1Volume () const { return _current.volumes.VrbcS1; }
   float RbcSetup2Volume () const { return _current.volumes.VrbcS2; }
   float RbcPrimeVolume () const { return _current.volumes.VrbcPrime; }

   float ReplacementVolume () const { return _current.volumes.Vrep; }


   float Vin () const { return _current.volumes.Vin; }
   float Vincv () const { return _current.volumes.Vincv; }
   float VincvTime () const { return _current.volumes.VincvTime; }
   float Vac () const { return _current.volumes.Vac; }
   float RbcDoubleVolume () const { return _current.volumes.VrbcDouble; }
   float RbcSetup1DoubleVolume () const { return _current.volumes.VrbcS1Double; }
   float RbcSetup2DoubleVolume () const { return _current.volumes.VrbcS2Double; }
   float VSalineBolus () const { return _current.volumes.VSalineBolus; }
   float VinEpcaStart () const { return _current.volumes.VinEpcaStart; }
   float RbcDoubleDose () const { return _current.volumes.rbcDoubleDose; }

   float ProcRunTime () const { return _current.time.procRunTime; }

   State_names         SubState () const { return _current.run.substate; }
   RecoveryTypes::Type Recovery () const { return _current.run.recovery; }
   int                 Purges () const { return _current.run.npurges; } /* purges done - as recorded by proc */
   int                 ScheduledPurges () const { return _current.run.nScheduledPurges; }
   //   const float VinClearLine() const { return _current.run.VinClearLine; }
   float QchMin () const { return (_current.run.QchMin == 0.0f) ? _current.run.Qch : _current.run.QchMin; }
   float Qch () const { return _current.run.Qch; }
   int   WbcPurgeType () const { return _current.run.WbcPurgeType; }
   float WbcPurgeStartTime () const { return _current.run.WbcPurgeStartTime; }
   float WbcLastProcPurge () const { return _current.run.WbcLastProcPurge; }
   float LastCentrifugeSpeed () const { return _current.run.LastCentrifugeSpeed; }
   bool  PirSpillover () const { return _current.run.PirSpillover; }
   bool  PtfRbcProd1Disabled () const { return _current.run.PtfRbcProd1Disabled; }
   float FirstDrawComplete () const { return _current.run.FirstDrawComplete; }

   float FirstCycleComplete () const { return _current.run.FirstDrawComplete; }
   float SubstateStartTime () const { return _current.run.SubstateStartTime; }
   short MidrunEntered () const { return _current.run.MidrunEntered; }

   bool  StopRamping () const { return _current.run.StopRamping; }
   float DeadRampQinmax () const { return _current.run.DeadRampMaxQin; }



   float Tbv () const { return _current.donor.tbv; }
   float Bmi () const { return _current.donor.bmi; }
   float PreCount () const { return _current.donor.preCount; }
   float Hct () const { return _current.donor.hct; }
   float InitialHct () const { return _current.donor.initialHct; }
   bool  Female () const { return _current.donor.female; }
   int   BloodType () const { return _current.donor.bloodType; }
   float Hrbc () const { return _current.cycle.Hrbc; }

   float ResHct () const { return _current.cycle.ResCrit; }

   float VAccumOverdraw () const { return _current.cycle.VAccumOverdraw; }
   float Weight () const { return _current.donor.weight; }
   float SampleVolume () const { return _current.donor.sampleVol; }

   bool PtfCpsTestPassed () const { return _current.cassette.PtfCpsTestPassed; }

   float AdjQinCap () const { return _adjCDS.Draw.CurrentCap.Get(); }
   float AdjQrpCap () const { return _adjCDS.Return.CurrentCap.Get(); }
   float AdjIrCap () const { return _adjCDS.Tingling.CurrentCap.Get(); }
   float AdjRatioCap () const { return _adjCDS.Clumping.CurrentCap.Get(); }
   float ActualPrimeDuration () const { return _runCDS.ActualPrimeDuration.Get(); }

   // Predict related stuff from here
   void  Reset (int substate);
   float getQinistdMax (void) const {return _QinistdMax; }
   void  setQinistdMax (float t) {_QinistdMax = t; }

   float getMaxTime () const {return _maxTime; }
   void  setMaxTime (float t) { _maxTime = t; }
   void  setRcvDonorTimeMsg (bool t) { _rcvDonorTimeMsg = t; }

   bool           getRcvDonorTimeMsg () const { return _rcvDonorTimeMsg; }
   bool           getOptimizingSelectedProc () const { return _predictCDS.OptimizingSelProc.Get(); }
   float          getPirQinLimit () const { return _predictCDS.PirQinLimit.Get(); }
   float          getQcmr () const { return _predictCDS.Qcmr.Get(); }
   float          getPirCsf () const { return _predictCDS.PirCsf.Get(); }
   float          getAccumYdPur () const { return _predictCDS.accumulatedYdpur.Get(); }
   int            getAccumPurges () const { return _predictCDS.accumulatedPurges.Get(); }
   float          getLastCalcQchMax () const { return _predictCDS.lastCalcQchMax.Get(); }
   float          getPIRLockin () const { return _predictCDS.PIRLockin.Get(); }
   float          getPlateletPlasmaQch () const { return _predictCDS.PlateletPlasmaQch.Get(); }
   float          getPrePirQch () const { return _predictCDS.PrePirQch.Get(); }
   float          getLimitingMidrunQrp () const { return _predictCDS.LimitingMidrunQrp.Get(); }
   float          getCsf (State_names substate) { return _predictCDS.Csf(substate); }
   DoPrediction_t getPredictRequest () const { return _predictRequest; }
   bool           getPrePirRamp () { return _runCDS.PrePirRamp.Get(); }

   void setOptimizingSelectedProc (bool t) { _predictCDS.OptimizingSelProc.Set(t); }
   void setPirQinLimit            (float Qin) { _predictCDS.PirQinLimit.Set(Qin); }
   void setQcmr                   (float Qin) { _predictCDS.Qcmr.Set(Qin); }
   void setPirCsf                 (float t) { _predictCDS.PirCsf.Set(t); }
   void setAccumYdPur             (float t) { _predictCDS.accumulatedYdpur.Set(t); }
   void setAccumPurges            (int t) { _predictCDS.accumulatedPurges.Set(t); }
   void setLastCalcQchMax         (float t) { _predictCDS.lastCalcQchMax.Set(t); }
   void setPIRLockin              (float t) { _predictCDS.PIRLockin.Set(t); }
   void setPlateletPlasmaQch      (float t) { _predictCDS.PlateletPlasmaQch.Set(t); }
   void setLimitingMidrunQrp      (float t) { _predictCDS.LimitingMidrunQrp.Set(t); }
   void setPredictRequest (DoPrediction_t request) { _predictRequest = request; }
   void setPrePirQch              (float t) { _predictCDS.PrePirQch.Set(t); }
   void setPrePirRamp             (bool t) {  _runCDS.PrePirRamp.Set(t); }

   Postcount   _postcount;
   PostHct     _posthct;
   Hypovolemia _hypovolemia;
   cassette    _cassette;

   Predict_CDS _predictCDS;

protected:
   ProcDataState();
   virtual ~ProcDataState();
   ProcDataState(const ProcDataState&);
   ProcDataState& operator = (const ProcDataState&); // nonassignable, not implemented

private:

   CurrentConditions _current;

   ProcDonor_CDS     _donorCDS;
   ProcVolumes_CDS   _volumesCDS;
   ProcTime_CDS      _timeCDS;
   ProcTrimaSet_CDS  _setCDS;
   ProcRun_CDS       _runCDS;
   ProcCycle_CDS     _cycleCDS;
   ProcAdjust_CDS    _adjCDS;

   bool              _logDonor;
   bool              _rcvDonorTimeMsg;
   float             _QinistdMax;
   float             _maxTime;
   DoPrediction_t    _predictRequest;

};

#endif /* _PROCSTATE_ */

/* FORMAT HASH 866469d61815186a8138636c40c5ef62 */
