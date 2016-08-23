/*
 *
 * TITLE:      procdetrbcpls.h
 *
 */

#ifndef PROCDET_RBCPLS_INCLUDED
#define PROCDET_RBCPLS_INCLUDED

#include "procdet.h"

struct IntermediateResults;

class procedureDetailRBCPls
   : public procedureDetail
{
public:
   procedureDetailRBCPls();
   virtual ~procedureDetailRBCPls();

   virtual void  LogTargets () const;
   virtual void  SetConstants ();
   virtual float instantaneousInfusion (int substateindex) const;

   virtual int AlgSubstate (const State_names procsubstate) const;

   virtual void                     generatesubstates ();
   virtual int                      ReplacementFluidRequired () const;
   virtual int                      RBCState () const;
   virtual int                      RBCDoubleState () const;
   virtual INTERNAL_PREDICT_RESULTS PreCheckProcedure ();
   virtual INTERNAL_PREDICT_RESULTS PostCheckProcedure ();
   virtual bool                     PlasmaHctOK () const;

   virtual bool doingPTFDouble () const;        // is it a double for PTF ?
   virtual bool secondPartofPTFDouble () const; // is it in the second part of the double for PTF? (after split)

#ifdef __WIN32__
   virtual void ResetProcDet ();
#endif
protected:

   void InterfaceSetup ();
   void PlasmaOnly (float maxendtime);

   void RBCSetup1 ();
   void RBCSetup2 ();
   void RBCCollection ();

   // repeate for Double collects
   void RBCSetup1Double ();
   void RBCSetup2Double ();
   void RBCCollectionDouble ();

private:

   virtual void do_SetTargets (substates_t&);

protected:


   IntermediateResults _rbcsubstate;       // for hypovolemia calculation use
   IntermediateResults _rbcsubstateDouble; // for hypovolemia calculation use
   float               _hct;               // Donor Hct

   bool                _PTF;

   float               _RBCSetup1Time;
   float               _RBCSetup1Vin;
   float               _RBCSetup1Vac;

   float               _RBCSetup2Time;
   float               _RBCSetup2Vin;
   float               _RBCSetup2Vac;

   float               _RBCSetup1DoubleTime;
   float               _RBCSetup1DoubleVin;
   float               _RBCSetup1DoubleVac;

   float               _RBCSetup2DoubleTime;
   float               _RBCSetup2DoubleVin;
   float               _RBCSetup2DoubleVac;

   float               _lowestIRDamping;
   bool                _disableProd1Done;
};

#endif

/* FORMAT HASH 92ae8d0120ec1a3eb096a1cbba717a0a */
