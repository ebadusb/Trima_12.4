/*
 *
 * TITLE:      procdet.h
 *
 */


#ifndef PROCDET_H
#define PROCDET_H

// ******************************

#include "sharedstruct.h"
#include "predict_msgs.h"
#include "cobeconfig.h"
#include "hypovolemia.h"
#include "states.h"
#include "trima_datalog.h"
#include "predict_cds.h"
#include "datalog.h"
#include "predictConfig.h"
#include "procDataState.h"

#include "flows.h"
#include "CatalogChecker.h"
#include "donor.h"
#include "procruntargets_cds.h"


#define MAXITERATIONS 25

class procedureDetailPltPlsRBC;
class procedureDetailRBCPls;


class procedureDetail
{
private:
   procedureDetail(const procedureDetail&);              // not implemented
   procedureDetail& operator = (const procedureDetail&); // not implemented

public:
   procedureDetail();
   virtual ~procedureDetail();
   virtual void copy (const procedureDetail& orig);

//
//
//   Virtual functions
//

   virtual void  LogTargets ()                                   const = 0;
   virtual void  SetConstants ()                                       = 0;
   virtual float instantaneousInfusion (int substateindex)       const = 0;
   virtual int   AlgSubstate (const State_names procsubstate)    const = 0;
   virtual void  generatesubstates ()                                  = 0;
   virtual int   RBCState ()                                     const = 0;
   virtual int   RBCDoubleState ()                               const = 0;
   virtual bool  PlasmaHctOK ()                                  const = 0;


   virtual int ReplacementFluidRequired () const { return 0; }
   virtual int NoMorePlatelets () const { return 0; }

   virtual INTERNAL_PREDICT_RESULTS PreCheckProcedure () { return GOOD_PRODUCT; }
   virtual INTERNAL_PREDICT_RESULTS PostCheckProcedure () { return GOOD_PRODUCT; }

   //
   INTERNAL_PREDICT_RESULTS GeneralPreCheck ();
   INTERNAL_PREDICT_RESULTS GeneralPostCheck ();


   virtual float VinTarget ()               const { return 0.0f; }
   virtual float VinPIRStart ()             const { return 0.0f; }
   virtual float YdchNoPIR ()               const { return 0.0f; }
   // virtual                               const float Ydch() const { return 0.0f; }
   virtual float YdchPIR ()                 const { return 0.0f; }
   virtual float getPIRLockin ()            const { return 0.0f; }
   virtual float getPrePltPlasmaEndQin ()   const { return 0.0f; }
   virtual float getPrePltNoPlasmaEndQin () const { return 0.0f; }

#ifdef __WIN32__
/* for robustness tests */
   virtual void ResetProcDet () = 0; // for robustness tests
#endif
   virtual void SetPIRRequired (bool value) { return; }
   virtual int  GetPIRRequired () const {return 0; }


   virtual void  AccumulateVolumes ();
   virtual float CalculateCsf (int) const { return 1.0f; }
   virtual float getCollectMaxFlow (void) const { return 0.0f; }

   void CalculateNoCollectFlows ();
   void CalculateNoCollectFirstReturnFlows ();
   void setNoCollectFirstReturn ();

   bool  checkForTraliFiltering ();
   float estimateVbpForRbcCollection () const;

   INTERNAL_PREDICT_RESULTS Optimize ();

   void initialize (float yield,
                    float platelet_volume,
                    float plasma_volume,
                    float plasma_product_volume,       // specical regionalization, plasma volume does not include AC
                    float rbc_dose,
                    float rbc_crit,
                    float rsf,
                    float max_procedure_time,
                    int blood_type,
                    int designator,
                    bool mss_plt,
                    float mss_plt_volume,
                    int mss_plt_pct_carryover,
                    bool mss_rbc,
                    int mss_rbc_volume,
                    int procedure_number);


   void initializeAmapPlt (float requestedYield,
                           AMAPPLTTYPES amap_platelet_size,
                           float plasma_volume,
                           float rbc_dose,
                           float rbc_crit,
                           float rsf,
                           float max_procedure_time,
                           int blood_type,
                           int designator,
                           bool metering_on,
                           bool mss_rbc,
                           int mss_rbc_volume,
                           int procedure_number);

   void SetUnoptimized ();
   void SetTargetsCds (ProcRunTargets_CDS& targetsCds);

   // SetTargets implements the "NVI" (NonVirtual Interface) pattern, also
   // called the "Template" pattern.  See Meyers, Effective C++ 3rd Ed. item 35
   // for a thorough discussion.  Forwards to the private virtual function do_SetTargets.
   virtual substates_t SetTargets ();

   virtual bool doingPTFDouble () const        = 0; // is it a double for PTF ?
   virtual bool secondPartofPTFDouble () const = 0; // is it in the second part of the double for PTF? (after split)

//
//
//   Data accessors and mutators-----------------------------------------------
//
public:
   //  disposable specific information (no mutators)
   int                    getNumOfSubstates ()       const { return _numOfSubstates; }
   cassette::cassetteType getRecommendedCassette ()  const { return _recommendedCassette; }
   float                  getExtracorporealVolume () const { return _extracorporealVolume; }

   float getResidualRBCVolume () const { return _residualRBCVolume; }


   float getPrimeDuration () const { return _primeDuration; }
   float getVinPrime () const { return _vinPrime; }


   // CONVERGENCE STUFF
   int  getnumOfTimes () const { return _numOfTimes; }
   void setnumOfTimes (int t) { _numOfTimes = t; }

   int  getconvergenceCtr () {return _convergenceCtr; }
   void setconvergenceCtr (int t) {_convergenceCtr = t; }



   // configured data members
   int  getProcedureNumber () const {return _ProcNumber; }
   void setProcedureNumber (const int t) {_ProcNumber = t; }


   int  getDesignator () const {return _Designator; }
   void setDesignator (int t) {_Designator = t; }

   int  getProcAMAPorMC () const {return _ProcAMAPorMC; }
   void setProcAMAPorMC (int t) {_ProcAMAPorMC = t; }

   int  getProcBloodType () const {return _ProcBloodType; }
   void setProcBloodType (int t) {_ProcBloodType = t; }

   bool getProcCollectPlasma () const {return _ProcCollectPlasma; }
   void setProcCollectPlasma (bool t) {_ProcCollectPlasma = t; }

   bool getProcCollectVolFixed () const {return _ProcCollectVolFixed; }
   void setProcCollectVolFixed (bool t) {_ProcCollectVolFixed = t; }

   float getProcFixedCollectVol () const {return _ProcFixedCollectVol; }
   void  setProcFixedCollectVol (float t) {_ProcFixedCollectVol = t; }

   float getProcFixedPlasmaVol () const {return _ProcFixedPlasmaVol; }
   void  setProcFixedPlasmaVol (float t) {_ProcFixedPlasmaVol = t; }

   float getProcPlasmaProductVol () const {return _ProcPlasmaProductVol; }
   void  setProcPlasmaProductVol (float t) {_ProcPlasmaProductVol = t; }

   int  getProcPlasmaVolFixed () const {return (_ProcPlasmaVolFixed); }
   void setProcPlasmaVolFixed (int t) {_ProcPlasmaVolFixed = t; }

   float getProcPlateletProdCon () const {return _ProcPlateletProdCon; }
   void  setProcPlateletProdCon (float t) {_ProcPlateletProdCon = t; }

   float getProcRBCDose () const {return _ProcRBCDose; }
   void  setProcRBCDose (float t) {_ProcRBCDose = t; }

   float getProcRBCHct () const {return _ProcRBCHct; }
   void  setProcRBCHct (float t) {_ProcRBCHct = t; }

   float getProcRBCVolume () const {return _ProcRBCVolume; }
   void  setProcRBCVolume (float t) {_ProcRBCVolume = t; }

   bool getProcPtfIsSplit () const {return _ProcPtfIsSplit; }
   void setProcPtfIsSplit (bool t) {_ProcPtfIsSplit = t; }

   float getProcTime () const {return _ProcTime; }
   void  setProcTime (float t) {_ProcTime = t; }

   float getDesiredYield () const {return _DesiredYield; }
   void  setDesiredYield (float t) { _DesiredYield = t; }

   bool getMSSPlt () const { return _mssPlt; }
   void setMSSPlt (bool t) { _mssPlt = t; }

   int  getMSSPltVolume () const { return _mssPltVolume; }
   void setMSSPltVolume (int t) { _mssPltVolume = t; }

   int  getMSSPltPctCarryover () const { return _mssPltPctCarryover; }
   void setMSSPltPctCarryover (int t) { _mssPltPctCarryover = t; }


   float getOriginalPltVolume () const { return _mssOriginalPltVolume; }
   void  setOriginalPltVolume (float t) { _mssOriginalPltVolume = t; }

   float getOriginalPltYeild () const { return _mssOriginalPltYield; }
   void  setOriginalPltYeild (float t) { _mssOriginalPltYield = t; }


   bool getPltAmap () const {return _PltAmap; }

   AMAPPLTTYPES getAmapPltSize () const { return _amapPlateletSize; }
   void         setAmapPltSize (const AMAPPLTTYPES t) { _amapPlateletSize = t; }

   float getAmapPltHighestYield () const {return _amapPlateletHighestYield; }
   void  setAmapPltHighestYield (float t) { _amapPlateletHighestYield = t; }


   bool getAMAPPLTinvalid () { return _amapPltInvalid; }
   void setAMAPPLTinvalid (bool t) { _amapPltInvalid = t; }

   bool getMSSRbc () const { return _mssRbc; }
   void setMSSRbc (bool t) { _mssRbc = t; }

   int  getMSSRbcVolume () const { return _mssRbcVolume; }
   void setMSSRbcVolume (int t) { _mssRbcVolume = t; }


   // calculated data members
   float getMaxPlasma () const {return _MaxPlasma; }
   void  setMaxPlasma (float t) {_MaxPlasma = t; }

   int  hypovolemic () const { return _hypovolemic; }
   void hypovolemic (int status) { _hypovolemic = status; }

   float hypovolemiaDistance () const { return _hypovolemiaDistance; }
   void  hypovolemiaDistance (float distance) {_hypovolemiaDistance = distance; }

   float getHypoMargin () const { return _HypoMargin; }
   void  setHypoMargin (float t) {_HypoMargin = t; }

   int  getPlasmaTargetCannotChange () const { return _PlasmaTargetCannotChange; }
   void setPlasmaTargetCannotChange (int t) {_PlasmaTargetCannotChange = t; }

   float getMaxACRate () const { return _MaxACRate; }
   void  setMaxACRate (float t) {_MaxACRate = t; }

   float getPredRBCDose () const {return _PredRBCDose; }
   void  setPredRBCDose (float t) {_PredRBCDose = t; }

   float getPredRBCVolume () const {return _PredRBCVolume; }
   void  setPredRBCVolume (float t) {_PredRBCVolume = t; }

   float getPredRBCCrit () const {return _PredRBCCrit; }
   void  setPredRBCCrit (float t) {_PredRBCCrit = t; }

   // used by black stamp PTF only, for IR infusion adjustment
   float getRBCCalculatedCrit () const {return _CalcedRBCCrit; }
   void  setRBCCalculatedCrit (float t) {_CalcedRBCCrit = t; }

   INTERNAL_PREDICT_RESULTS getStatus () const {return _status; }
   void                     setStatus (const INTERNAL_PREDICT_RESULTS t) {_status = t; }

   bool getOptimized () const {return _optimized; }
   void setOptimized (bool t) {_optimized = t; }

   bool getCannotBeDisqualifiedAsDuplicate () const {return _CannotBeDisqualifiedAsDuplicate; }
   void setCannotBeDisqualifiedAsDuplicate (bool t) {_CannotBeDisqualifiedAsDuplicate = t; }

   bool getPlasmaLimitedByHct () const {return _PlasmaLimitedByHct; }
   void setPlasmaLimitedByHct (bool t) {_PlasmaLimitedByHct = t; }

   float getInfusionRate () const { return _I; }
   void  setInfusionRate (float t) { _I = t; }

   float getYtarg () const { return _Ytarg; }
   void  setYtarg (float t) { _Ytarg = t; }

   float getYdpur   () const { return _Ydpur; }
   float getYdch    () const { return _Ydch; }
   float getEc      () const { return _Ec; }
   float getQinc    () const { return _Qinc; }
   float getVintarg () const { return _Vintarg; }
   float getVptarg  () const { return _Vptarg; }
   float getVrtarg  () const { return _Vrtarg; }
   float getVpmin   () const { return _Vpmin; }
   float getVinpur  () const { return _Vinpur; }
   float getQcmr    () const { return _Qcmr; }
   float getTc      () const { return _Tc; }

   float getPredVc () const { return _PredVc; }
   void  setPredVc (float vc) { _PredVc = vc; }

   float getPredVp () const { return _PredVp; }
   void  setPredVp (float vp) { _PredVp = vp; }

   float getPredVrep () const { return _PredVrep; }
   void  setPredVrep (float vp) { _PredVrep = vp; }

   float getPredTp () const { return _PredTp; }
   void  setPredTp (float tp) { _PredTp = tp; }

   float getPredVac () const { return _PredVac; }
   void  setPredVac (float vac) { _PredVac = vac; }

   int                        getInDisplayList () const { return _inDisplayList; }
   void                       setInDisplayList (const int flag) { _inDisplayList = flag; }
   const IntermediateResults& getSubstate (int index) const { return substate[index]; }


   void setMyAMAPPltYieldRange (AMAPPLTTYPES prodSize,      float& min, float& max, float& conc);
   void setMyAMAPPltYieldRange (float& min, float& max, float& conc);


   // Max qind across all procedures.
   float getMaxQin () const;





   //   Copy results into substate information struct in Predict CDS.
   void  copySubstateInfoToCds (substates_t& info);
   void  restoreSubstateInfoFromCds (substates_t& info);
   short getConvergenceTrail (int i) const { return _convergenceTrail[i]; }


private:

   virtual void do_SetTargets (substates_t&) = 0; // called from SetTargets (usual "NVI" design)

protected:
   IntermediateResults substate[ALLOCATED_SUBSTATES];

   float               _PredVc;        // Predicted collect volume
   float               _PredVp;        // Predicted plasma volume
   float               _PredRBCVolume; // total predicted rbc volume
   float               _PredVac;
   float               _PredVrep;
   float               _PredTp; // Predicted time in procedure (total)
   int                 _inDisplayList;

protected:

   //  disposable specific information
   int                    _numOfSubstates;
   cassette::cassetteType _recommendedCassette;


   float _extracorporealVolume;
   float _primeDuration;
   float _vinPrime;
   float _residualRBCVolume;


   // CONVERGENCE STUFF
   int _numOfTimes;           // How many times have we gone through optimizer loop.
   int _convergenceCtr;       // What was the convergence ctr (0 = all done).

   // configured data members
   int   _ProcNumber;          // Holds the procedure number for this procedure
   int   _Designator;          // 1=primary set 2=secondary set etc.
   int   _ProcAMAPorMC;        // Procedure: 0=As much as Possible, 1=Maximum Concurrent
   int   _ProcBloodType;       // Procedure: Enum for procedure blood type.....
   bool  _ProcCollectPlasma;   // Procedure: Collect Plasma? 0=No, 1=YES
   bool  _ProcCollectVolFixed; // Procedure: Yes or No, is Collect vol fixed
   float _ProcFixedCollectVol; // Procedure: Actual volume platelets is fixed at.

   float _ProcFixedPlasmaVol;   // Procedure Actual volume plasma is fixed at.
   float _ProcPlasmaProductVol; // Plasma volume for procedures where volume is defined without AC
   int   _ProcPlasmaVolFixed;   // Procedure: Yes or NO, Plasma volume fixed? 0=NO, 1=YES
   float _ProcPlateletProdCon;  // Procedure: Platelet Product Concentration
   float _ProcRBCDose;          // Procedure: RBC dosage
   float _ProcRBCHct;           // Procedure:: RBC HCT
   float _ProcRBCVolume;        // Procedure: RBC product volume
   float _ProcTime;             // Procedure: time
   float _DesiredYield;         // Procedure: desired yield (e11)
   bool  _ProcPtfIsSplit;       // Is this a split PTF procedure (do we need to adjust GUI for two filters?)

   int   _mssPltVolume;     // Amount of Plt MSS to meter
   int   _mssPltPctCarryover;
   int   _mssRbcVolume;     // Amount of RBC MSS to meter
   float _mssOriginalPltYield;
   float _mssOriginalPltVolume;
   bool  _mssPlt;             // Procedure includes metered storage
   bool  _mssRbc;             // Procedure includes metered storage

   float _CalcedRBCCrit;  // used by black stamp PTF to estimate safety IR
   bool  _CannotBeDisqualifiedAsDuplicate;
   bool  _PlasmaLimitedByHct;
   //

   // amap plt stuff:
   bool         _PltAmap;
   float        _amapPlateletHighestYield;
   AMAPPLTTYPES _amapPlateletSize;
   bool         _amapPltInvalid;
   bool         _AMAP_PLATELETS_ON;



   // calculated data members
   float _MaxPlasma; // for currently recommended cassette
   int   _hypovolemic;
   float _hypovolemiaDistance;      // + is OK, - is hypovolemic
   float _HypoMargin;               // margin for hypovolemia
   float _HypoMarginMin;            // Minimum margin
   int   _PlasmaTargetCannotChange; // true if forcing fixed product for configured AMAP/MC procedure
   float _MaxACRate;                // current maximum allowable infusion cap
   float _PredRBCDose;              // total predicted rbc dose
   float _PredRBCCrit;              // collection crit target


   INTERNAL_PREDICT_RESULTS _status;                   // Holds status code for this procedure


   bool  _optimized;               // Flag whether procedure needs optimization or not...
   float __MIN_CRIT;
   float __MAX_CRIT;
   float _I;
   float _Tc; // Time during platelet collection
   float _Ytarg;
   float _Ec;   // Platelet Collection efficiency.  RKM.  Not needed at top level anymore.
   float _Qinc; // Average Qin during Platelet collection
   float _Ydpur;
   float _Vintarg;
   float _Vptarg; // Plasma volume target
   float _Vrtarg; // RBC volume target
   float _Qcmr;   // RKM.  Not needed at top level anymore.
   float _Vpmin;  // Minimum amount of plasma to collect
   float _Vinpur; // RKM.  Not needed at top level anymore.
   float _Ydch;   // Yield decrement due to platelets trapped in LRS chamber.  RKM.  Not needed at top level anymore.

//
//
//   I keep a local reference to the Cobe configuration file.  I do not
//   initialize the file but expect my containing task to do so.
//
   const CobeConfig::CobeConfigStruct& cobeConfig;


#ifdef __WIN32__
public:
#endif
   Config&        _config;
   ProcDataState& _procState;
#ifdef __WIN32__
protected:
#endif
   Calc_No_collection           _nocollectFlows;
   IntermediateResults          _firstReturnNoCollect;
   carray<short, MAXITERATIONS> _convergenceTrail; // for logging all convergenceCtr values during an optimization
};


#endif

/* FORMAT HASH caf45243e141da7be9a45b5a428705e8 */
