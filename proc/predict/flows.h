/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      flows.hpp
 *             Focussed system prediction functions.
 *
 * AUTHOR:     Tate Moore
 *
 * ABSTRACT:   This file contains the objects necessary for
 *             calculations for determining the predicted values.
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 * EXAMPLE:
 *
 */

#ifndef FLOWS_H
#define FLOWS_H

#include "cobeconfig.h"
#include "predict_types.h"

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Virtual Base class of for calculating flow rates.
class Calculate_Flows
{

public:

   Calculate_Flows();
   virtual ~Calculate_Flows();


   void  SetCollectMaxFlow (float maxFlow) { _CollectMaxFlow = maxFlow; }
   float GetCollectMaxFlow (void) { return _CollectMaxFlow ; }

protected:
   // Pure virtual functions.
   virtual float       get_Qin (float DC)            = 0;
   virtual float       get_Qin (float DC, float Qrp) = 0;
   virtual float       get_Qrp (float DC, float Qin) = 0;
   virtual float       get_DC (const float Qin, const float Qrp)                 = 0;
   virtual void        get_Qin (float& Qin, const float DC, const float Qneedle) = 0;
   virtual const float get_Qn (const float Qin, const float Qrp)                 = 0;
   virtual const float get_maxQrp (const float Qin, const float Qneedle, const float QrpLimit, const float DC) = 0;

   virtual const float get_QnLimit (const float Qin, const float DC) {return _QnLimit; }

   virtual const int get_QinQrp (float& qin, float& qrp, float& DC);

   //
   //
   //   These methods protected to force use of derived class equivalents
   //
   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const float DCLimit);

//
// ----------------------------------------------------------------------------
//                           Member data declarations
//
public:
   static int _numCalculations;    // for offline debug

protected:

   //
   //
   //  My status flag.  0=success nonzero=failure
   //
   int _failure;

   //
   //
   //  These members are copies of the corresponding initialization values.
   //
   float _hct;                   // Donor Hematocrit (Hin)
   float _precount;
   float _TBV;
   float _IR;
   float _ratio;
   float _QinLimit;
   float _QrpLimit;
   float _QnLimit;
   float _DCLimit;                // Upper Limit and Init value

   //
   //
   //   Precalculated values
   //
   float _oneM1Ratio;
   float _facp;
   float _facb;
   float _Vsvnr;
   float _Vres;


   float _CollectMaxFlow;   // set from cobe configs based on concentration.

     #if (CPU!=SIMNT) // Static construction of the CobeConfigStruct object kills the simulator.
   static
     #endif
   const CobeConfig::CobeConfigStruct & _cc;
};

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_No_collection
   : public Calculate_Flows
{
public:
   // Constructor, destructor.
   //
   Calc_No_collection();
   virtual ~Calc_No_collection();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit);

protected:
   virtual float       get_Qin (float DC);
   virtual float       get_Qin (float DC, float Qrp);
   virtual float       get_Qrp (float DC, float Qin);
   virtual float       get_DC (const float Qin, const float Qrp);
   virtual void        get_Qin (float& Qin, const float DC, const float Qneedle);
   virtual const float get_Qn (const float Qin, const float Qrp);
   virtual const float get_maxQrp (const float Qin, const float Qneedle, const float QrpLimit, const float DC);

};



//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_Rbcpl_RBC_Setup
   : public Calculate_Flows
{
public:
   // Constructor, destructor.
   //
   Calc_Rbcpl_RBC_Setup();
   virtual ~Calc_Rbcpl_RBC_Setup();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const float Hrbc);

protected:
   virtual float get_Qin (float DC);
   virtual float get_Qin (float DC, float Qrp);
   virtual void  get_Qin (float& Qin, const float DC, const float Qneedle);

   virtual float       get_Qrp (float DC, float Qin);
   virtual float       get_DC (const float Qin, const float Qrp);
   virtual const float get_Qn (const float Qin, const float Qrp);
   virtual const float get_maxQrp (const float Qin, const float Qneedle, const float QrpLimit, const float DC);

   virtual const float get_QnLimit (const float /*Qin*/, const float /*DC*/) { return _QnLimit; }

   // Black stamp functionality
   virtual float get_Qp (const float Qin);
   // virtual float get_Qrbc(const float Qin);

protected:
   float _hrbc;

};

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_Rbcpl_plasma_collection
   : public Calculate_Flows
{
public:
   // Constructor, destructor.
   //
   Calc_Rbcpl_plasma_collection();
   virtual ~Calc_Rbcpl_plasma_collection();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const int replacementRequired,
                                 const float replacementPercent);

protected:
   //
   //  Overridden from base class
   //
   virtual float get_Qin (float DC);
   virtual float get_Qin (float DC, float Qrp);

   virtual float       get_Qrp (float DC, float Qin);
   virtual float       get_DC (const float Qin, const float Qrp);
   virtual void        get_Qin (float& Qin, const float DC, const float Qneedle);
   virtual const float get_Qn (const float Qin, const float Qrp);
   virtual const float get_maxQrp (const float Qin, const float Qneedle, const float QrpLimit, const float DC);
   virtual const float get_QnLimit (const float Qin, const float DC);

   //
   //
   //   Additional functionality
   //
   virtual float get_Qp (const float Qin);
   virtual float get_Qrep (const float Qin);

protected:
   int   _replacementRequired;
   float _replacementPercent;


};


//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_Rbcpl_rbc_collection
   : public Calculate_Flows
{
public:
   // Constructor, destructor.
   //
   Calc_Rbcpl_rbc_collection();
   virtual ~Calc_Rbcpl_rbc_collection();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const bool replacementRequired,
                                 const float replacementPercent,
                                 const float collectionCrit,
                                 const float qrbcMax,
                                 const float HCT_Delta);

protected:
   //
   //  Overridden from base class
   //
   virtual float get_Qin (float DC);
   virtual float get_Qin (float DC, float Qrp);

   virtual float       get_Qrp (float DC, float Qin);
   virtual float       get_DC (const float Qin, const float Qrp);
   virtual void        get_Qin (float& Qin, const float DC, const float Qneedle);
   virtual const float get_Qn (const float Qin, const float Qrp);
   virtual const float get_maxQrp (const float Qin, const float Qneedle, const float QrpLimit, const float DC);
   virtual const float get_QnLimit (const float Qin, const float DC);

   //
   //
   //   Additional functionality
   //
   virtual float get_Qp (const float Qin);
   virtual float get_Qrbc (const float Qin);
   virtual float get_Qrep (const float Qin);

protected:
   int   _replacementRequired;
   float _replacementPercent;
   float _collectionCrit;
   float _DCLimit;
   float _Vsvn;
   float _dcf;
};


//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_Rbcpl_ptf_rbc_collection
   : public Calc_Rbcpl_rbc_collection
{
public:
   // Constructor, destructor.
   //
   Calc_Rbcpl_ptf_rbc_collection();
   virtual ~Calc_Rbcpl_ptf_rbc_collection();

   // virtual const int calculate(struct IntermediateResults *substate);
   //  PTF calculate is no diff from regular RBC collect calc

   virtual void Initialize (const float hct,
                            const float precount,
                            const float TBV,
                            const float IR,
                            const float ratio,
                            const float QinLimit,
                            const float QrpLimit,
                            const float QnLimit,
                            const bool replacementRequired,
                            const float replacementPercent,
                            const float collectionCrit,
                            const float qrbcMax,
                            const float HCT_Delta);

protected:
   //
   //  Overridden from base class.  Limits Qin
   //
   virtual float get_Qin (float DC);
   virtual float get_Qin (float DC, float Qrp);

   virtual void get_Qin (float& Qin, const float DC, const float Qneedle);

   virtual float get_Qp (const float Qin);
   //
   //
   //   Additional functionality
   //
   virtual float limit_Qin (const float Qin);

protected:
   float _qrbcmax;
   float _hctDelta;
};



//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_Channel_Setup
   : public Calculate_Flows
{
public:
   // Constructor, destructor.
   //
   Calc_Channel_Setup();
   virtual ~Calc_Channel_Setup();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const float Qch);

protected:
   virtual float       get_Qin (float DC);
   virtual float       get_Qin (float DC, float Qrp);
   virtual float       get_Qrp (float DC, float Qin);
   virtual float       get_DC (const float Qin, const float Qrp);
   virtual void        get_Qin (float& Qin, const float DC, const float Qneedle);
   virtual const float get_Qn (const float Qin, const float Qrp);
   virtual const float get_maxQrp (const float Qin,
                                   const float Qneedle,
                                   const float QrpLimit,
                                   const float DC);
   virtual const float get_Qinr (const float Qin);

protected:
   float _Qch;
   float _Qpinstd;
   float _Qcinstd;
};

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_Preplatelet_Plasma
   : public Calculate_Flows
{
public:
   // Constructor, destructor.
   //
   Calc_Preplatelet_Plasma();
   virtual ~Calc_Preplatelet_Plasma();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const float Qch);

protected:
   virtual float       get_Qin (float DC);
   virtual float       get_Qin (float DC, float Qrp);
   virtual float       get_Qrp (float DC, float Qin);
   virtual float       get_DC (const float Qin, const float Qrp);
   virtual void        get_Qin (float& Qin, const float DC, const float Qneedle);
   virtual const float get_Qn (const float Qin, const float Qrp);
   virtual const float get_QnLimit (const float Qin, const float DC);
   virtual const float get_maxQrp (const float Qin,
                                   const float Qneedle,
                                   const float QrpLimit,
                                   const float DC);
   virtual const float get_Qinr (const float Qin, const float DC);
   virtual const float get_Qp (const float Qin, const float DC);
   virtual const float get_Qch (const float Qin) const ;
protected:
   float _Qch;
   float _generalfc;
};

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_Platelet_Plasma
   : public Calc_Preplatelet_Plasma
{
public:
   // Constructor, destructor.
   //
   Calc_Platelet_Plasma();
   virtual ~Calc_Platelet_Plasma();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const float Qcmr);

protected:
   virtual float       get_Qin (float DC);
   virtual float       get_Qin (float DC, float Qrp);
   virtual float       get_Qrp (float DC, float Qin);
   virtual float       get_DC (const float Qin, const float Qrp);
   virtual void        get_Qin (float& Qin, const float DC, const float Qneedle);
   virtual const float get_Qch (const float Qin) const ;
   virtual const float get_QnLimit (const float Qin, const float DC);

};

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_PCA
   : public Calculate_Flows
{
public:
   // Constructor, destructor.
   //
   Calc_PCA();
   virtual ~Calc_PCA();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const float Qcmr);

protected:
   virtual float get_Qin (float DC);
   virtual float get_Qin (float DC, float Qrp);
   virtual float get_Qrp (float DC, float Qin);
   virtual float get_DC (const float Qin, const float Qrp);
   virtual void  get_Qin (float& Qin, const float DC, const float Qneedle);

   virtual const float get_Qc (const float Qin, const float DC);
   virtual const float get_Qinr (const float Qin, const float DC);
   virtual const float get_Qn (const float Qin, const float Qrp);
   virtual const float get_QnLimit (const float Qin, const float DC);
   virtual const float get_maxQrp (const float Qin,
                                   const float Qneedle,
                                   const float QrpLimit,
                                   const float DC);

   virtual const float get_Qch (const float Qin) const ;
protected:
   float _Qcmr;
   float _generalfc;
};
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_Midrun
   : public Calc_PCA
{
public:
   // Constructor, destructor.
   //
   Calc_Midrun();
   virtual ~Calc_Midrun();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const float Qcmr);

protected:
   virtual float get_Qin (float DC);

   virtual const float get_Qc (const float Qin, const float DC);

   // protected:
   // float _Qcmr;
};

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_PIR_Plasma
   : public Calc_Platelet_Plasma
{
public:
   // Constructor, destructor.
   //
   Calc_PIR_Plasma();
   virtual ~Calc_PIR_Plasma();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const float Qch);
};

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_PIR_NoPlasma
   : public Calc_Midrun
{
public:
   // Constructor, destructor.
   //
   Calc_PIR_NoPlasma();
   virtual ~Calc_PIR_NoPlasma();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const float Qcmr);
};



//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_RBC
   : public Calculate_Flows
{
public:
   // Constructor, destructor.
   //
   Calc_RBC();
   virtual ~Calc_RBC();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const float collectionCrit,
                                 const float Qch,
                                 const float QrbcMax);

protected:
   //
   //  Overridden from base class
   //
   virtual float       get_Qin (float DC);
   virtual float       get_Qin (float DC, float Qrp);
   virtual float       get_Qrp (float DC, float Qin);
   virtual float       get_DC (const float Qin, const float Qrp);
   virtual void        get_Qin (float& Qin, const float DC, const float Qneedle);
   virtual const float get_Qn (const float Qin, const float Qrp);
   virtual const float get_maxQrp (const float Qin, const float Qneedle, const float QrpLimit, const float DC);

   //
   //   Additional functionality
   //
   virtual float get_Qc (const float Qin);

protected:
   float _collectionCrit;
   float _DCLimit;

};



//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_RBC_PTF
   : public Calc_RBC
{
public:
   // Constructor, destructor.
   //
   Calc_RBC_PTF();
   virtual ~Calc_RBC_PTF();

   virtual void Initialize (const float hct,
                            const float precount,
                            const float TBV,
                            const float IR,
                            const float ratio,
                            const float QinLimit,
                            const float QrpLimit,
                            const float QnLimit,
                            const float collectionCrit,
                            const float Qch,
                            const float QrbcMax);

   virtual const int calculate (struct IntermediateResults* substate);

   //
   //  Overridden from base class
   //
protected:
   virtual float get_Qin (float DC);
   virtual float get_Qin (float DC, float Qrp);

   //
   //   Additional functionality
   //
   virtual float limit_qin (const float Qin, const float DC);

protected:
   float _Qch;
   float _qrbcmax;
};



//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_RBC_Setup
   : public Calculate_Flows
{
public:
   // Constructor, destructor.
   //
   Calc_RBC_Setup();
   virtual ~Calc_RBC_Setup();

   virtual const int calculate (struct IntermediateResults* substate);
   virtual void      Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const float Qch,
                                 const float Hrbc);

protected:
   virtual const float get_Qc (const float Qin);

   virtual float       get_Qin (float DC);
   virtual float       get_Qin (float DC, float Qrp);
   virtual float       get_Qrp (float DC, float Qin);
   virtual float       get_DC (const float Qin, const float Qrp);
   virtual void        get_Qin (float& Qin, const float DC, const float Qneedle);
   virtual const float get_Qn (const float Qin, const float Qrp);
   virtual const float get_maxQrp (const float Qin, const float Qneedle, const float QrpLimit, const float DC);

   virtual const float get_QnLimit (const float /*Qin*/, const float /*DC*/) { return _QnLimit; }

protected:
   float _Qch;
   float _hrbc;
};


//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class Calc_RBC_PTF_PRIME
   : public Calc_RBC_PTF
{
public:
   // Constructor, destructor.
   //
   Calc_RBC_PTF_PRIME();
   virtual ~Calc_RBC_PTF_PRIME();

   virtual void Initialize (const float hct,
                            const float precount,
                            const float TBV,
                            const float IR,
                            const float ratio,
                            const float QinLimit,
                            const float QrpLimit,
                            const float QnLimit,
                            const float collectionCrit,
                            const float Qch,
                            const float QrbcMax);

   virtual const int calculate (struct IntermediateResults* substate);

   //
   //  Overridden from base class
   //
protected:
   // virtual float get_Qin(float DC);
   // virtual float get_Qin(float DC, float Qrp);

   //
   //   Additional functionality
   //
   // virtual float limit_qin(const float Qin,const float DC);

protected:
   float _Qch;
   float _hrbc;
   float _qrbcmax;
};


#endif

/* FORMAT HASH cf31410f145e04435bac704501e3e0a6 */
