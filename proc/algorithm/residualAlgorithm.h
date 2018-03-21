/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      ResidualAlgorithm.h
 *
 * ABSTRACT:   This class calculates the set residuals
 *
 */


#ifndef _RESIDUAL_ALGORITHM_H_  //  Prevents multiple inclusions.
#define _RESIDUAL_ALGORITHM_H_


#include "cassette.h"

class ResidualAlgorithm
{
public:




   ResidualAlgorithm();
   virtual ~ResidualAlgorithm();

   void initialize (bool logData = true);
   void calculate (bool logData = true);

   int algorithmError ()              const {return myErrorCode; }

   float getReportedRBCVolume_Prod1 ()      const {return myReportedRBCVolume_prod1; }
   float getReportedSSVolume_Prod1 ()       const {return myReportedSSVolume_prod1; }
   float getOfflineSSVolume_Prod1 ()        const {return myOfflineSSVolume_prod1; }
   float getReportedRBCVolume_Prod2 ()      const {return myReportedRBCVolume_prod2; }
   float getReportedSSVolume_Prod2 ()       const {return myReportedSSVolume_prod2; }
   float getOfflineSSVolume_Prod2 ()        const {return myOfflineSSVolume_prod2; }
   float getResidualRBCDose ()              const {return myRBCResidualDose; }
   float getResidualPLSVolume ()            const {return myPLSResidualVolume; }
   float getCassetteOnlyResidualRBCDose ()  const {return  _cassettOnlyRBCResidualDose; } // does not includes filter loss, just cassette only

   static float getBaseResidual (const cassette::cassetteType setType,
                                 const int plasmaRinseback_ON,
                                 const int salineRinseback_ON);



// protected:



   float reportedRBCVolume (const float rbcProdVolm, const float RAStoProduct, const float incompleteRas);                                     // D20
   float reportedSSVolume (const float rbcProdVolm,  const float RAStoProduct, const float incompleteRas);                                     // D21
   float offlineSSVolume (const float targetSS,   const float RAStoProduct, const float incompleteRasconst, const float MSS_VOLUME_TOLERANCE); // D22


   float reportedResidual_pRBC (const float rbcProdVolm1,   const float RAStoProduct1,
                                const float rbcProdVolm2,   const float RAStoProduct2,
                                const float incompleteRas1,  const float incompleteRas2
                                );   // D25

   float reportedResidual_pls (const float rbcProdVolm1,   const float RAStoProduct1,
                               const float rbcProdVolm2,   const float RAStoProduct2,
                               const float incompleteRas1,  const float incompleteRas2
                               );    // D26


   // float  rbcVolumePastValue(const float  rbcProdVolm);    // D8
   float ssVolumePastValve (const float RAStoProduct, const float incompleteRas);                               // D9
   float PTFFilterHoldUpRBCDose (const float rbcProdVolm, const float RAStoProduct, const float incompleteRas); // D11
   float PTFFilterHoldUpPls (const float rbcProdVolm, const float RAStoProduct, const float incompleteRas);     // D12
   float filterVolume ();                                                                                       // D13
   float ssInFilter (const float rbcProdVolm, const float RAStoProduct, const float incompleteRas);             // D14


#ifdef __WIN32__
public:
   void load_values_for_debug (float pRbc_target1_in,                    // D5
                               float pRbc_target2_in,                    // F5
                               float RAS_target1_in,                     // D6
                               float RAS_target2_in,                     // F6
                               float pRbc_actual1_in,
                               float pRbc_actual2_in,
                               float RbcVolm1_actual1_in,
                               float RbcVolm2_actual2_in,
                               float RAS_actual1_in,
                               float RAS_actual2_in,
                               float hct_donor_in,                       // D17      hct*100 so like 45.4f
                               float vb_processed_in,                    // D18
                               bool mssPtf_in,                           // D29
                               bool SalineRB_in,                         // D31
                               bool PlasmaRB_in,                         // D31
                               bool NormalRB_in,                         // D31
                               bool firstReturnEntered_in,               // D30
                               bool BlackStamp_in,                       // D32
                               bool rinsebackComplete_in                 // D33
                               );


#endif

   // private:


   /***************************************
   the answers:                           */

   float myReportedRBCVolume_prod1;
   float myReportedSSVolume_prod1;
   float myOfflineSSVolume_prod1;

   float myReportedRBCVolume_prod2;
   float myReportedSSVolume_prod2;
   float myOfflineSSVolume_prod2;

   float myRBCResidualDose;
   float myPLSResidualVolume;
   float _cassettOnlyRBCResidualDose;  // no filter losses

   /**************************************/

   ////  SETUP SYSTEM GATHERED INFO



   float pRbc_target1;                   // D5
   float pRbc_target2;                   // F5
   float RAS_target1;                    // D6
   float RAS_target2;                    // F6

   float RbcVolm_actual1;
   float RbcVolm_actual2;

   float pRbc_actual1;
   float pRbc_actual2;
   float RAS_actual1;
   float RAS_actual2;

   float hct_donor;                      // D17      hct*100 so like 45.4f
   float vb_processed;                   // D18

   float incompleteRASVolume1;            // F34
   float incompleteRASVolume2;            // F34

   bool  mssPtf;             // D29
   bool  SalineRB;           // D31
   bool  PlasmaRB;           // D31
   bool  NormalRB;           // D31
   bool  firstReturnEntered; // D30
   bool  BlackStamp;         //                  //D32
   bool  rinsebackComplete;  // D33

   int   myErrorCode;


   float MSS_TOLERANCE1;   // the tolerance to a failure by volume or Process error
   float MSS_TOLERANCE2;   // the tolerance to a failure by volume or Process error



};


#endif

/* FORMAT HASH a3520ae16e6f5c8e3b70d8ed85d19d90 */
