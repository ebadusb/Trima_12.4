#ifndef _PREDICTCONFIG_
#define _PREDICTCONFIG_

#include "config_cds.h"
#include "configdef.h"
#include "cobeconfig.h"
#include <string>


////////////////////////////////////////////////////////////////////////////////
//
//  Predict Configuration:
//
//  This object is a singleton, accessed via the static Instance method.  It is
//  initialized on first access.
//
//  This initialization is not thread safe, and its reentrancy is somewhat
//  debateable; neither is an issue if it is only used in the Predict task.  Use
//  of this object in other tasks may need some coordination.
//
//  Note that the important initialization is done via the nonstatic
//  readConfigDat member function, which is neither thread-safe nor reentrant
//  (although it safely accesses a CDS object); calling this function from
//  multiple threads will need locking or other coordination.

class Config
{

public:

   // Singleton access
   static Config& Instance ();


#ifdef __WIN32__
   bool InitializeCds (std::string path);
#endif
// Accessors for Config.dat settings
   float getACRatio () const {return _ACRatio; }
   void  setACRatio (float t) {_ACRatio = t; }

   float getACRatioDuringRBC () const {return _ACRatioDuringRBC; }
   void  setACRatioDuringRBC (float t) {_ACRatioDuringRBC = t; }

   float getACRatioDuringPlasmaOnly () const {return _ACRatioDuringPlasmaOnly; }
   void  setACRatioDuringPlasmaOnly (float t) {_ACRatioDuringPlasmaOnly = t; }

   float getCqInlet () const {return _CqInlet; }
   void  setCqInlet (float t) {_CqInlet = t; }

   float getCqReturn () const {return _CqReturn; }
   void  setCqReturn (float t) {_CqReturn = t; }

   float getACRate () const {return _ACRate; }
   void  setACRate (float t) {_ACRate = t; }

   float getReferenceACRate () const {return _ReferenceACRate; }
   void  setReferenceACRate (float t) {_ReferenceACRate = t; }

   float getYSF () const {return _YSF; }
   void  setYSF (float t) {_YSF = t; }

   float getYSF_CV () const {return _YSF_CV; }
   void  setYSF_CV (float t) {_YSF_CV = t; }

   int  getFluidBalYN () const {return _FluidBalYN; }
   void setFluidBalYN (int t) {_FluidBalYN = t; }

   float getFluidBalActiveVol () const {return _FluidBalActiveVol; }
   void  setFluidBalActiveVol (float t) {_FluidBalActiveVol = t; }

   float getFluidBal () const {return _FluidBal; }
   void  setFluidBal (float t) {_FluidBal = t; }

   float getTbvQinmaxScaleConst () const {return _TbvQinmaxScaleConst; }
   void  setTbvQinmaxScaleConst (float t) {_TbvQinmaxScaleConst = t; }

   float getTbvQnretmaxScaleConst () const {return _TbvQnretmaxScaleConst; }
   void  setTbvQnretmaxScaleConst (float t) {_TbvQnretmaxScaleConst = t; }

   float getRSF () const {return _RSF; }
   void  setRSF (float t) {_RSF = t; }


   // added min for clarity.
   int  getAMAPVolume () const {return _AMAPVolume; }
   void setAMAPVolume (int t) {_AMAPVolume = t; }
   int  getAMAPMinVolume () const {return _AMAPVolume; }
   void setAMAPMinVolume (int t) {_AMAPVolume = t; }

   int  getAMAPMaxVolume () const {return _AMAPMaxVolume; }
   void setAMAPMaxVolume (int t) {_AMAPMaxVolume = t; }


   int  getInletFlowRamp () const {return _InletFlowRamp; }
   void setInletFlowRamp (int t)  {_InletFlowRamp = t; }

   float getMaxInstQin () const {return _MaxInstQin; }
   void  setMaxInstQin (float t) {_MaxInstQin = t; }

   float getMaxNeedleRetFlow () const {return _MaxNeedleRetFlow; }
   void  setMaxNeedleRetFlow (float t) {_MaxNeedleRetFlow = t; }

   int  getPlasmaRinseback () const {return _PlasmaRinseback; }
   void setPlasmaRinseback (int t) {_PlasmaRinseback = t; }

   int  getSalineRinseback () const {return _SalineRinseback; }
   void setSalineRinseback (int t) {_SalineRinseback = t; }

   int  getCertifiedYield () const {return _CertifiedYield; }
   void setCertifiedYield (int t) {_CertifiedYield = t; }

   int  getYtarg_CL () const {return _Ytarg_CL; }
   void setYtarg_CL (int t) {_Ytarg_CL = t; }

   float getMinPostCount () const {return _MinPostCount; }
   void  setMinPostCount (float t) {_MinPostCount = t; }

   float getMinPostHct () const {return _MinPostHct; }
   void  setMinPostHct (float t) {_MinPostHct = t; }

   int  getMaxProcedureTime () const {return _MaxProcedureTime; }
   void setMaxProcedureTime (int t) {_MaxProcedureTime = t; }

   float getFFPVolume () const;
   float getConfigMaxQin () const;
   float getConfigACRatio () const;
   float getConfigMaxACRate () const;

   bool                 isAutoFlowOn () const;
   int                  isPltMeteringOn () const;
   int                  isRbcMeteringOn () const;
   int                  isCompPlasmaOn () const;
   int                  getNumProcedures () const;
   ProcedureDescription getProcedureDescription (int procedureNum) const;
   int                  isDrbcAlertOn () const;
   int                  getDrbcThreshold () const;
   int                  getKeyACRate () const;
   int                  getPlasmaGenderOption () const;
   int                  getPltGenderOption () const;
   bool                 isPromptedDrbcSplit (int procedureNum) const;
   int                  getWeightUnits () const;
   float                getRbcDose (int procedureNum) const;
   float                getDrbcTbvLimit () const;
   int                  getTbvVolLimitSetting () const;

   int AdjustConfig (float QinCap, float QrpCap, float IrCap, float RatioCap, bool IsAfAdjust = false);

   bool PirAlways () const;

   float AMAPPltConfigConc (AMAPPLTTYPES prodSize);
   float AMAPPltMinYield (AMAPPLTTYPES prodSize);
   float AMAPPltMaxYield (AMAPPLTTYPES prodSize);
   int   AMAPPltCarryover (AMAPPLTTYPES prodSize);


protected:
   Config();
   virtual ~Config ();
   Config(const Config&);
   Config& operator = (const Config&);

private:
#ifndef __WIN32__
   const
#endif
   Config_CDS _pConfigCds;

   const CobeConfig::CobeConfigStruct& _cc;
   float _ACRatio;                 // Normal AC Ratio
   float _ACRatioDuringRBC;        // AC Ratio during RBC collection
   float _ACRatioDuringPlasmaOnly; // AC Ratio during black stamp plasma only collection
   float _CqInlet;                 // constant for inlet flow management
   float _CqReturn;                // constant for inlet flow management
   float _ACRate;                  // Maximum AC Rate
   float _ReferenceACRate;         // The reference AC Rate
   float _YSF;                     // Yield Scaling Factor;
   float _YSF_CV;                  // Yield Scaling Factor - coeffecient of variance
   int   _FluidBalYN;              // replacement fluid bal yes/no flag
   float _FluidBalActiveVol;       // replacement fluid bal volume
   float _FluidBal;                // replacement fluid bal percentage
   float _TbvQinmaxScaleConst;
   float _TbvQnretmaxScaleConst;
   float _RSF;
   int   _AMAPVolume;          // AMAP plasma volume
   int   _AMAPMinVolume;       // maximum plasma volume for AMAP collections
   int   _AMAPMaxVolume;       // maximum plasma volume for AMAP collections
   int   _InletFlowRamp;       // Do we do ramping
   float _MaxInstQin;          // Maximum Instantaneous Inlet Flow from user config or adjustments
   float _MaxNeedleRetFlow;    // Maximum needle return flow cap
   int   _PlasmaRinseback;     // 1= plasma rinseback enabled 0=disabled
   int   _SalineRinseback;     // 1= saline rinseback enabled 0=disabled
   int   _CertifiedYield;      // Certified Yield 0=No, 1=YES
   int   _Ytarg_CL;            // Target Yield - confidence level
   float _MinPostCount;        // Minimum platelet post count
   float _MinPostHct;          // Minimum post HCT
   int   _MaxProcedureTime;    // Maximum procedure time
   bool  _FirstTimeQinCapDown; // Used to track first time QinCap down
                               //

// API
public:
   void readConfigDat ();

   static const float __plasmaTolerance;
   static const float __MaxConcentrationError;

   static const int   __NOCOLLECT_STATE;
   static const int   __CHANNEL_SETUP;
   static const int   __PREPLATELET_PLASMA;
   static const int   __PREPLATELET_NOPLASMA;
   static const int   __PLATELET_PLASMA;
   static const int   __PCA;
   static const int   __MIDRUN;
   static const int   __PIR_NOPLASMA;
   static const int   __PIR_PLASMA;
   static const int   __EXTENDED_PCA;
   static const int   __EXTENDED_PLASMA;
   static const int   __RBC;
   static const int   __RBC_SETUP_1;
   static const int   __RBC_SETUP_2;
   static const int   __RBC_PRIME;

   static const int   __RP_CHANNEL_SETUP;
   static const int   __RP_PLASMA_ONLY;
   static const int   __RP_RBC_SETUP_1;
   static const int   __RP_RBC_SETUP_2;
   static const int   __RP_RBC;

   static const int   __RP_RBC_SETUP_1_DOUBLE;
   static const int   __RP_RBC_SETUP_2_DOUBLE;
   static const int   __RP_RBC_DOUBLE;


};

#endif /* _PREDICTCONFIG_ */

/* FORMAT HASH a454cd07d63c180cd0f241d19e8c1c64 */
