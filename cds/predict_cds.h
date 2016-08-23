#ifndef __PREDICT_CDS
#define __PREDICT_CDS

#include "datastore.h"
#include "carray.h"
#include "haldefs.h"
#include "vtargetchecker.h"
#include "substatemap.h"

#ifndef WIN32
#pragma interface
#endif

#define ALLOCATED_SUBSTATES 15
//
// Derived Container.
// Contains collection of elements making up a logical data store.
// User must register his role.  Role of ROLE_RW grants authority to write data.
//
class Predict_CDS
   : public MultWriteDataStore
{

public:

   class one_substate_info_t
   {
   public:

      float                    T;             // time in substate
      float                    EndVin;        // ending Vin for substate
      float                    QinD, QinR;    // Inlet flow rate
      float                    Qac;           // AC flow rate
      float                    Qrp;           // Return flow rate
      float                    QpD, QpR;      // Plasma flow rate
      float                    QcD, QcR;      // Collect flow rate
      float                    DutyCycle;     // The duty cycle for this stuff
      float                    InfusionRate;  // Calculated instantaneous infusion rate
      float                    Vtarget;       // The specific end volume target for this substate
      float                    OrVtarget;     //  an alternative specific end volume target for this substate
      VTarget::VolTargetDetail VtargetType;   // The specific end volume target type for this substate
      VTarget::VolTargetDetail OrVtargetType; //  an alternative specific end volume target type for this substate
      float                    UnadjustedQin; // Max Qin for this substate if no adjustments were performed
      float                    UnadjustedQn;
      float                    UnadjustedIR;
      float                    UnadjustedRatio;
      HAL_VALVESTATE           PlateletValve;
      HAL_VALVESTATE           PlasmaValve;
      HAL_VALVESTATE           RBCValve;
      float                    AccumVac;
      float                    Vacr;
      float                    AccumVacp;
      float                    AccumVp;
      float                    AccumVrep;
      float                    HypoD;
      float                    Hend;
      float                    Csf;
      float                    Rpm;

   public:
      one_substate_info_t()
         : T(0.0f),
           EndVin(0.0f),
           QinD(0.0f), QinR(0.0f),
           Qac(0.0f),
           Qrp(0.0f),
           QpD(0.0f), QpR(0.0f),
           QcD(0.0f), QcR(0.0f),
           DutyCycle(0.0f),
           InfusionRate(0.0f),
           Vtarget(0.0f),
           OrVtarget(0.0f),
           VtargetType(VTarget::NOCOLLECT),
           OrVtargetType(VTarget::NOCOLLECT),
           UnadjustedQin(0.0f),
           UnadjustedQn(0.0f),
           UnadjustedIR(0.0f),
           UnadjustedRatio(0.0f),
           PlateletValve(HAL_VALVE_UNKNOWN),
           PlasmaValve(HAL_VALVE_UNKNOWN),
           RBCValve(HAL_VALVE_UNKNOWN),
           AccumVac(0.0f),
           Vacr(0.0f),
           AccumVacp(0.0f),
           AccumVp(0.0f),
           AccumVrep(0.0f),
           HypoD(0.0f),
           Hend(0.0f),
           Csf(0.0f),
           Rpm(0.0f)
      {}
      ~one_substate_info_t() {}
   };

   enum { NumOfSubstates       = ALLOCATED_SUBSTATES }; // code uses this number for allocation;
                                                        // assumed larger than NumOfRBCPlsSubstates
   enum { NumOfRBCPlsSubstates = 9 };

   typedef carray<one_substate_info_t, NumOfSubstates> substate_info_t;

public:
   BaseElement<substate_info_t>     SubstateInfo;
   BaseElement<float>               VinTarget;                // Vin target for platelet collection
   BaseElement<float>               VinPIRStart;              // Vin target for PIR substate start
                                                              //
   BaseElement<float>               VinPurge;                 // Inlet volume target for WBC purging of the chamber
   BaseElement<float>               VinDeltaPurge;            // The delta for the next purge according to VIN
                                                              //
   BaseElement<float>               MaxPIRQc;                 // max collect flow during PIR
   BaseElement<float>               Ydch;                     // Chamber capacity during non-PIR
   BaseElement<float>               YdchPIR;                  // Chamber capacity during PIR
   BaseElement<float>               RBCVsvn;                  // stroke volume during RBC collection
   BaseElement<float>               accumulatedYdpur;         // total yield decrement due to purges performed so far
   BaseElement<int>                 accumulatedPurges;        // total number of purges performed so far
   BaseElement<int>                 maxDonorTime;             // time filter for donor's desired procedure time max
   BaseElement<bool>                PTFReduceFlows;           // Flow reduction flag for PTF.
   BaseElement<float>               lastCalcQchMax;           // the qch max based on concentration
   BaseElement<float>               VinNextPurge;             // The next purge point
   BaseElement<float>               TimeNextPurge;            // Duration of the next purge
   BaseElement<float>               AccumulatedPurgeTime;     // Total time spent in purges so far
   BaseElement<float>               PurgeEndTime;             // Procedure time when current purge ends
   BaseElement<float>               PIRLockin;                // PIR lockin bracket
   BaseElement<one_substate_info_t> NoCollectFirstReturnOnly; // the no collect calcs for first return with lower AC rate (IT8680)

   BaseElement<float>               PirCsf;
   BaseElement<float>               PirQinLimit;
   BaseElement<bool>                OptimizingSelProc;
   BaseElement<float>               Qcmr;
   BaseElement<float>               PlateletPlasmaQch;
   BaseElement<float>               LimitingMidrunQrp;
   BaseElement<float>               PrePirQch;

// Class Methods
public:
   Predict_CDS (Role role);
   virtual ~Predict_CDS();

// Data Members
protected:



// Class Methods
public:

   float Qin (bool draw, int substate) const {substate_info_t info; SubstateInfo.Get(&info); return (draw ? info[substate].QinD : info[substate].QinR); }
   float Qac (bool draw, int substate) const {substate_info_t info; SubstateInfo.Get(&info); return (draw ? info[substate].Qac : 0.0f); }
   float Qrp (bool draw, int substate) const {substate_info_t info; SubstateInfo.Get(&info); return (draw ? 0.0f : info[substate].Qrp); }
   float Qc (bool draw, int substate) const {substate_info_t info; SubstateInfo.Get(&info); return (draw ? info[substate].QcD : info[substate].QcR); }
   float Qp (bool draw, int substate) const {substate_info_t info; SubstateInfo.Get(&info); return (draw ? info[substate].QpD : info[substate].QpR); }

   HAL_VALVESTATE plateletValve (int substate) const {substate_info_t info; SubstateInfo.Get(&info); return info[substate].PlateletValve; }
   HAL_VALVESTATE plasmaValve   (int substate) const {substate_info_t info; SubstateInfo.Get(&info); return info[substate].PlasmaValve; }
   HAL_VALVESTATE rbcValve      (int substate) const {substate_info_t info; SubstateInfo.Get(&info); return info[substate].RBCValve; }
   float          InfusionRate  (int substate) const {substate_info_t info; SubstateInfo.Get(&info); return info[substate].InfusionRate; }

   float                    volumeTarget (int substate) const {substate_info_t info; SubstateInfo.Get(&info); return info[substate].Vtarget; }
   float                    orVolumeTarget (int substate) const {substate_info_t info; SubstateInfo.Get(&info); return info[substate].OrVtarget; }
   VTarget::VolTargetDetail targetEnum (int substate) const {substate_info_t info; SubstateInfo.Get(&info); return info[substate].VtargetType; }
   VTarget::VolTargetDetail orTargetEnum (int substate) const {substate_info_t info; SubstateInfo.Get(&info); return info[substate].OrVtargetType; }

   float Qin (bool draw, State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return (draw ? info[alg].QinD : info[alg].QinR); }
   float Qac (bool draw, State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return (draw ? info[alg].Qac : 0.0f); }
   float Qrp (bool draw, State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return (draw ? 0.0f : info[alg].Qrp); }
   float Qc (bool draw, State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return (draw ? info[alg].QcD : info[alg].QcR); }
   float Qp (bool draw, State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return (draw ? info[alg].QpD : info[alg].QpR); }

   float EndVin (State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return info[alg].EndVin; }

   HAL_VALVESTATE plateletValve (State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return info[alg].PlateletValve; }
   HAL_VALVESTATE plasmaValve   (State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return info[alg].PlasmaValve; }
   HAL_VALVESTATE rbcValve      (State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return info[alg].RBCValve; }
   float          InfusionRate  (State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return info[alg].InfusionRate; }

   float                    volumeTarget (State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return info[alg].Vtarget; }
   float                    orVolumeTarget (State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return info[alg].OrVtarget; }
   VTarget::VolTargetDetail targetEnum (State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return info[alg].VtargetType; }
   VTarget::VolTargetDetail orTargetEnum (State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return info[alg].OrVtargetType; }

   float Rpm (State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return (info[alg].Rpm); }
   float Csf (State_names substate) const {int alg = SubstateMap::alg(substate); substate_info_t info; SubstateInfo.Get(&info); return (info[alg].Csf); }

private:
   Predict_CDS();  // Base Constructor not available
};


// Provide a handy typedef for code that deals in such things...

typedef Predict_CDS::substate_info_t substates_t;




#endif

/* FORMAT HASH 2f08de804afb134da6e1731c463320e0 */
