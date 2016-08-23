/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      ResidualAlgorithm.cpp
 *
 * ABSTRACT:   This class calculates Residuals in all permutations ... Yeeehh haaa
 *
 */



#include <vxworks.h>
#ifdef WIN32
  #include <iostream>
  #include <fstream>
using namespace std;
#else
  #include "procdata.h"
  #include "cassette.h"
  #include "CatalogChecker.h"
#endif
#include "cobeconfig.h"
#include "ptfResidual.h"
#include "residualAlgorithm.h"




ResidualAlgorithm::ResidualAlgorithm()
   : myReportedRBCVolume_prod1(0.0f),
     myReportedSSVolume_prod1(0.0f),
     myOfflineSSVolume_prod1(0.0f),
     myReportedRBCVolume_prod2(0.0f),
     myReportedSSVolume_prod2(0.0f),
     myOfflineSSVolume_prod2(0.0f),
     myRBCResidualDose(0.0f),
     myPLSResidualVolume(0.0f),
     pRbc_target1(0.0f),            // D5
     pRbc_target2(0.0f),            // F5
     RAS_target1(0.0f),             // D6
     RAS_target2(0.0f),             // F6
     pRbc_actual1(0.0f),
     pRbc_actual2(0.0f),
     RAS_actual1(0.0f),
     RAS_actual2(0.0f),
     hct_donor(0.0f),            // D17      hct*100 so like 45.4f
     vb_processed(0.0f),         // D18
     incompleteRASVolume1(0.0f), // F34
     incompleteRASVolume2(0.0f), // F34
     mssPtf(0),                  // D29
     SalineRB(0),                // D31
     PlasmaRB(0),                // D31
     NormalRB(0),                // D31
     firstReturnEntered(0),      // D30
     BlackStamp(0),              // D32
     rinsebackComplete(0),       // D33
     myErrorCode(0),
     RbcVolm_actual1(0.0),
     RbcVolm_actual2(0.0),
     MSS_TOLERANCE1(0.0f),
     MSS_TOLERANCE2(0.0f)
{}

ResidualAlgorithm::~ResidualAlgorithm()
{}




void ResidualAlgorithm::initialize (bool logData)
{
#ifndef __WIN32__

   ProcData pd;

   mssPtf             = pd.RunTargets().MeterRbcs.Get();
   firstReturnEntered = pd.Run().FirstDrawComplete.Get();

   // the stamp bit is set to 1 if white or 0 if black
   if (CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::STAMP))
      BlackStamp = false;
   else
      BlackStamp = true;



   rinsebackComplete = (pd.Run().RinsebackComplete.Get() != RINSEBACK_UNSUCCESSFUL);
   SalineRB          = (pd.Run().RinsebackComplete.Get() == RINSEBACK_SUCCESSFUL_SALINE);
   PlasmaRB          = (pd.Run().RinsebackComplete.Get() == RINSEBACK_SUCCESSFUL_PLASMA);
   NormalRB          = (pd.Run().RinsebackComplete.Get() == RINSEBACK_SUCCESSFUL_NORMAL);

   if (!rinsebackComplete)
   {
      // for the algorith we need to select a rinseback type
      // if Rinseback doesnt complete we still need to have a type... but it doesnt effect anything
      // its just to keep from getting an error for not having the data.
      // we select normal as it is common to both stamps
      PlasmaRB = false;
      SalineRB = false;
      NormalRB = true;    // just default to normal in not completed
   }


   // IT 8306 - Always determine whether this is prompted split based on configured
   // rbc dose with no scaling or adjustments
   bool doubleTargeted = pd.Config().isPromptedDrbcSplit(pd.RunTargets().ProcNumber.Get());


   if (doubleTargeted && mssPtf && BlackStamp)
   {
      pRbc_target1 = pd.RunTargets().RbcDoseTarget.Get() / 2.0f;
      pRbc_target2 = pd.RunTargets().RbcDoseTarget.Get() / 2.0f;
      RAS_target1  = pd.RunTargets().RBCSolutionTarget.Get() / 2.0f;
      RAS_target2  = pd.RunTargets().RBCSolutionTarget.Get() / 2.0f;
   }
   else
   {
      pRbc_target1 = pd.RunTargets().RbcDoseTarget.Get();
      pRbc_target2 = 0.0f;
      RAS_target1  = pd.RunTargets().RBCSolutionTarget.Get();
      RAS_target2  = 0.0f;
   }

   pRbc_actual1    = pd.Volumes().RBCProduct_A_FinalDose.Get();
   pRbc_actual2    = pd.Volumes().RBCProduct_B_FinalDose.Get();

   RbcVolm_actual1 = pd.Volumes().RBCProduct_A_FinalVolume.Get();
   RbcVolm_actual2 = pd.Volumes().RBCProduct_B_FinalVolume.Get();

   RAS_actual1     = pd.Volumes().RBCProduct_A_FinalVss.Get();
   RAS_actual2     = pd.Volumes().RBCProduct_B_FinalVss.Get();

   if (RAS_actual1 < 0.0f)
      RAS_actual1 = 0.0f;


   if (RAS_actual2 < 0.0f)
      RAS_actual2 = 0.0f;


   hct_donor =  pd.Donor().Hct.Get();

   // get the volume of blood processed only if first return fAils
   // otherwise it isnt used anyplace
   vb_processed =   pd.Volumes().VBP.Get();        // where we were when the run ened... no rinseback will be done.


   incompleteRASVolume1 = RAS_target1 - RAS_actual1 - CobeConfig::data().mssPtfFilterLoss + CobeConfig::data().MssPtfVolume;
   if ( (incompleteRASVolume1 < 0.0f) || (RAS_target1 <= 0.0f) )
      incompleteRASVolume1 = 0.0f;


   incompleteRASVolume2 = RAS_target2 - RAS_actual2 - CobeConfig::data().mssPtfFilterLoss + CobeConfig::data().MssPtfVolume;
   if  ( (incompleteRASVolume2 < 0.0f) || (RAS_target2 <= 0.0f) )
      incompleteRASVolume2 = 0.0f;

   // if metering had a process error then we have zero tolerance for error
   if (!pd.Run().MeterRbcs_OK.Get() || pd.Run().PTFRBCprod1Disabled)
      MSS_TOLERANCE1 = 0.0f;                      // the tolerance to a failure by volume or Process error
   else                                           // we have the normal CC volume margin for error
      MSS_TOLERANCE1 = CobeConfig::data().Vrbc1;  // the tolerance to a failure by volume or Process error

   // if metering had a process error then we have zero tolerance for error
   if (!pd.Run().MeterRbcs_OK.Get() || pd.Run().PTFRBCprod2Disabled)
      MSS_TOLERANCE2 = 0.0f;                      // the tolerance to a failure by volume or Process error
   else                                           // we have the normal CC volume margin for error
      MSS_TOLERANCE2 = CobeConfig::data().Vrbc1;  // the tolerance to a failure by volume or Process error



   if (logData)
   {
      float tmp_ras1_mss =  RAS_target1 - CobeConfig::data().mssPtfFilterLoss + CobeConfig::data().MssPtfVolume;
      float tmp_ras2_mss =  RAS_target2 - CobeConfig::data().mssPtfFilterLoss + CobeConfig::data().MssPtfVolume;
      if (RAS_target1 == 0.0f) tmp_ras1_mss = 0.0f;
      if (RAS_target2 == 0.0f) tmp_ras2_mss = 0.0f;



      DataLog(log_level_proc_external) << "ResidualAlgorithm init() :"   << endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: mssPtf            :"   << mssPtf <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: firstReturnEntered     :"   << firstReturnEntered <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: BlackStamp        :"   << BlackStamp <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: rinsebackComplete :"   << rinsebackComplete <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: SalineRB          :"   << SalineRB <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: PlasmaRB          :"   << PlasmaRB <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: NormalRB          :"   << NormalRB <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: doubleTargeted    :"   << doubleTargeted <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: pRbc_target1 :"   << pRbc_target1 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: pRbc_target2 :"   << pRbc_target2 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: RAS_target1_cfg  :"   << RAS_target1 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: RAS_target2_cfg  :"   << RAS_target2 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: RAS_target1_mss  :"   << tmp_ras1_mss <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: RAS_target2_mss  :"   << tmp_ras2_mss <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: pRbc_actual1 :"   << pRbc_actual1 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: pRbc_actual2 :"   << pRbc_actual2 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: RbcVolm_actual1 :"   << RbcVolm_actual1 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: RbcVolm_actual2 :"   << RbcVolm_actual2 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: RAS_actual1  :"   << RAS_actual1 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: RAS_actual2  :"   << RAS_actual2 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: hct_donor    :"   << hct_donor <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: vb_processed :"   << vb_processed <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: incompleteRASVolume1 :"   << incompleteRASVolume1 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: incompleteRASVolume2 :"   << incompleteRASVolume2 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: MSS_TOLERANCE1       :"   << MSS_TOLERANCE1 <<  endmsg;
      DataLog(log_level_proc_external) << "    ResidualAlgorithm :: MSS_TOLERANCE2       :"   << MSS_TOLERANCE2 <<  endmsg;
   }


#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

void ResidualAlgorithm::calculate (bool logData)
{

   myReportedRBCVolume_prod1 =   reportedRBCVolume(RbcVolm_actual1, RAS_actual1, incompleteRASVolume1);
   myReportedSSVolume_prod1  =   reportedSSVolume(RbcVolm_actual1, RAS_actual1, incompleteRASVolume1);
   myOfflineSSVolume_prod1   =   offlineSSVolume(RAS_target1, RAS_actual1, incompleteRASVolume1, MSS_TOLERANCE1);

   myReportedRBCVolume_prod2 =   reportedRBCVolume(RbcVolm_actual2, RAS_actual2, incompleteRASVolume2);
   myReportedSSVolume_prod2  =   reportedSSVolume(RbcVolm_actual2, RAS_actual2, incompleteRASVolume2);
   myOfflineSSVolume_prod2   =   offlineSSVolume(RAS_target2, RAS_actual2, incompleteRASVolume2, MSS_TOLERANCE2);

   myRBCResidualDose         =   reportedResidual_pRBC(RbcVolm_actual1,
                                                       RAS_actual1,
                                                       RbcVolm_actual2,
                                                       RAS_actual2,
                                                       incompleteRASVolume1,
                                                       incompleteRASVolume2);

   myPLSResidualVolume =   reportedResidual_pls(RbcVolm_actual1,
                                                RAS_actual1,
                                                RbcVolm_actual2,
                                                RAS_actual2,
                                                incompleteRASVolume1,
                                                incompleteRASVolume2);



#ifndef __WIN32__

   if (logData)
   {

      DataLog(log_level_proc_info) << "ResidualAlgorithm calculate() :"   << endmsg;
      if (!algorithmError())
      {
         DataLog(log_level_proc_info) << "    ResidualAlgorithm :: myReportedRBCVolume_prod1  :"   << myReportedRBCVolume_prod1 <<  endmsg;
         DataLog(log_level_proc_info) << "    ResidualAlgorithm :: myReportedSSVolume_prod1   :"   << myReportedSSVolume_prod1 <<  endmsg;
         DataLog(log_level_proc_info) << "    ResidualAlgorithm :: myOfflineSSVolume_prod1    :"   << myOfflineSSVolume_prod1 <<  endmsg;
         DataLog(log_level_proc_info) << "    ResidualAlgorithm :: myReportedRBCVolume_prod2  :"   << myReportedRBCVolume_prod2 <<  endmsg;
         DataLog(log_level_proc_info) << "    ResidualAlgorithm :: myReportedSSVolume_prod2   :"   << myReportedSSVolume_prod2 <<  endmsg;
         DataLog(log_level_proc_info) << "    ResidualAlgorithm :: myOfflineSSVolume_prod2    :"   << myOfflineSSVolume_prod2 <<  endmsg;
         DataLog(log_level_proc_info) << "    ResidualAlgorithm :: myRBCResidualDose          :"   << myRBCResidualDose <<  endmsg;
         DataLog(log_level_proc_info) << "    ResidualAlgorithm :: myPLSResidualVolume        :"   << myPLSResidualVolume <<  endmsg;
      }
      else
      {
         DataLog(log_level_proc_info) << "   ResidualAlgorithm :: Algorithm Error Occured! error code :"   << algorithmError() <<  endmsg;
      }

   }
#endif

}


///////////////////////////////////////////////////////////////////////////////////////////////
float ResidualAlgorithm::reportedResidual_pRBC (const float RbcVolm_actual1,   const float RAStoProduct1,
                                                const float RbcVolm_actual2,   const float RAStoProduct2,
                                                const float incompleteRas1, const float incompleteRas2)
{
   float pRbcResiduals = 0.0f;

   if (!firstReturnEntered)
   {
      pRbcResiduals = vb_processed * hct_donor;

   }
   else         // firstReturnEntered is OK

   {
      if (NormalRB)
      {

         if (BlackStamp)
         {
            if (rinsebackComplete)
            {
               pRbcResiduals =  CobeConfig::data().ResPlsRbc_NormalRB_RBC
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);
            }
            else
            {

               pRbcResiduals =   CobeConfig::data().ResPlsRbc_NoRB_RBC
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);
            }

         }
         else           // duh, white stamp

         {
            if (rinsebackComplete)
            {
               pRbcResiduals =   CobeConfig::data().ResPltPlsRbc_NormalRB_RBC
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);

            }
            else
            {

               pRbcResiduals =   CobeConfig::data().ResPltPlsRbc_NoRB_RBC
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);
            }
         }



      }
      else if (PlasmaRB)
      {
         if (BlackStamp)
         {
            myErrorCode = 1;
         }
         else          // duh, white stamp

         {
            if (rinsebackComplete)
            {
               pRbcResiduals =   CobeConfig::data().ResPltPlsRbc_PlasmaRB_RBC
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);

            }
            else
            {

               pRbcResiduals =   CobeConfig::data().ResPltPlsRbc_NoRB_RBC
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);
            }
         }

      }
      else if (SalineRB)
      {

         if (BlackStamp)
         {
            if (rinsebackComplete)
            {
               pRbcResiduals =  CobeConfig::data().ResPlsRbc_SalineRB_RBC
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);
            }
            else
            {

               pRbcResiduals =   CobeConfig::data().ResPlsRbc_NoRB_RBC
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                               + PTFFilterHoldUpRBCDose(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);
            }

         }
         else          // duh, white stamp
         {
            myErrorCode = 2;
         }

      }
      else
      {

         myErrorCode   = 3;
         pRbcResiduals = 0.0f;
      }

   }



   return pRbcResiduals;
   return 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
float ResidualAlgorithm::reportedResidual_pls (const float RbcVolm_actual1,   const float RAStoProduct1,
                                               const float RbcVolm_actual2,   const float RAStoProduct2,
                                               const float incompleteRas1, const float incompleteRas2)
{


   float plsResiduals = 0.0f;

   if (!firstReturnEntered)
   {
      plsResiduals = vb_processed * (1.0F - hct_donor);

   }
   else       // firstReturnEntered is OK

   {
      if (NormalRB)
      {

         if (BlackStamp)
         {
            if (rinsebackComplete)
            {
               plsResiduals =  CobeConfig::data().ResPlsRbc_NormalRB_PLS
                              + PTFFilterHoldUpPls(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                              + PTFFilterHoldUpPls(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);
            }
            else
            {

               plsResiduals =   CobeConfig::data().ResPlsRbc_NoRB_PLS
                              + PTFFilterHoldUpPls(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                              + PTFFilterHoldUpPls(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);
            }

         }
         else         // duh, white stamp

         {
            if (rinsebackComplete)
            {
               plsResiduals =   CobeConfig::data().ResPltPlsRbc_NormalRB_PLS
                              + PTFFilterHoldUpPls(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                              + PTFFilterHoldUpPls(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);

            }
            else
            {

               plsResiduals =   CobeConfig::data().ResPltPlsRbc_NoRB_PLS
                              + PTFFilterHoldUpPls(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                              + PTFFilterHoldUpPls(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);
            }
         }



      }
      else if (PlasmaRB)
      {
         if (BlackStamp)
         {
            myErrorCode = 1;
         }
         else        // duh, white stamp

         {
            if (rinsebackComplete)
            {
               plsResiduals =   CobeConfig::data().ResPltPlsRbc_PlasmaRB_PLS
                              + PTFFilterHoldUpPls(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                              + PTFFilterHoldUpPls(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);

            }
            else
            {

               plsResiduals =   CobeConfig::data().ResPltPlsRbc_NoRB_PLS
                              + PTFFilterHoldUpPls(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                              + PTFFilterHoldUpPls(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);
            }
         }

      }
      else if (SalineRB)
      {

         if (BlackStamp)
         {
            if (rinsebackComplete)
            {
               plsResiduals =  CobeConfig::data().ResPlsRbc_SalineRB_PLS
                              + PTFFilterHoldUpPls(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                              + PTFFilterHoldUpPls(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);
            }
            else
            {

               plsResiduals =   CobeConfig::data().ResPlsRbc_NoRB_PLS
                              + PTFFilterHoldUpPls(RbcVolm_actual1,  RAStoProduct1, incompleteRas1)
                              + PTFFilterHoldUpPls(RbcVolm_actual2,  RAStoProduct2, incompleteRas2);
            }

         }
         else        // duh, white stamp
         {
            myErrorCode = 2;
         }




      }
      else
      {

         myErrorCode  = 3;
         plsResiduals = 0.0f;
      }

   }


   return plsResiduals;
}


///////////////////////////////////////////////////////////////////////////////////////////////


float ResidualAlgorithm::reportedRBCVolume (const float RbcVolm_actual,   const float RAStoProduct, const float incompleteRas)
{

   const float rbcDelv   = RbcVolm_actual; // rbcVolumePastValue(rbcProdDose);

   float       reportRBC = 0.0f;

   if (mssPtf)
   {

      if (rbcDelv > CobeConfig::data().MssPtfVolume)
      {
         reportRBC =  (  rbcDelv
                         - PTFFilterHoldUpRBCDose(RbcVolm_actual, RAStoProduct, incompleteRas)
                         - PTFFilterHoldUpPls(RbcVolm_actual, RAStoProduct, incompleteRas)
                         )
                     +  reportedSSVolume(RbcVolm_actual, RAStoProduct, incompleteRas);
      }
      else
      {
         reportRBC = 0.0f;
      }

   }
   else
   {

      reportRBC =  rbcDelv;

   }


   return reportRBC;

}


///////////////////////////////////////////////////////////////////////////////////////////////
float ResidualAlgorithm::reportedSSVolume (const float RbcVolm_actual,   const float RAStoProduct, const float incompleteRas)
{
   const float ssVolume         = ssVolumePastValve(RAStoProduct, incompleteRas);
   float       reportedSSvolume = 0.0f;

   if (ssVolume > 0.0f)
   {
      reportedSSvolume =  ssVolume - ssInFilter(RbcVolm_actual, RAStoProduct, incompleteRas);
   }

   if (RbcVolm_actual == 0.0f)
      reportedSSvolume = 0.0f;

   return reportedSSvolume;

}
///////////////////////////////////////////////////////////////////////////////////////////////
float ResidualAlgorithm::offlineSSVolume (const float targetSS,   const float RAStoProduct, const float incompleteRas, const float MSS_VOLUME_TOLERANCE)
{

   float offlineSSVolume = 0.0f;

   if (incompleteRas > MSS_VOLUME_TOLERANCE)      // if !meteringCompleted
   {
      offlineSSVolume =    targetSS
                        +  filterVolume()
                        -  CobeConfig::data().mssPtfFilterLoss
                        -  ssVolumePastValve(RAStoProduct, incompleteRas) ;
   }
   else
   {

      offlineSSVolume = 0.0f;
   }


   if (!mssPtf)
      offlineSSVolume = 0.0f;


   if (targetSS == 0.0f)
      offlineSSVolume = 0.0f;

   return offlineSSVolume;

}


///////////////////////////////////////////////////////////////////////////////////////////////

// float ResidualAlgorithm ::  rbcVolumePastValue(const float  rbcProdDose)
// {
//
//     float theVolm = 0.0f;
//
//    // This was needed in modeling... but in the trima the CobeConfig::data().mssPtfFilterLoss has
//    // already been added to the rbcProdDose to do the predict collection of the extra filter loss....
//    // if (mssPtf)
//    // {
//    //    if (rbcProdDose > 0.0f)
//    //      theVolm = ( (rbcProdDose + CobeConfig::data().mssPtfFilterLoss * 0.8f) / 0.8f);
//    //
//    // } else {
//          theVolm =  (rbcProdDose / 0.8f);
//
//    // }
//
//      //
//      //DataLog( log_level_proc_info ) << "ResidualAlgorithm   : rbcVolumePastValue"  << theVolm  << endmsg;
//      return theVolm;
//
// }
//
///////////////////////////////////////////////////////////////////////////////////////////////


float ResidualAlgorithm::ssVolumePastValve (const float RAStoProduct, const float /* incompleteRas */)
{

   float theVolume = 0.0f;

   if (mssPtf)
   {

      if (RAStoProduct > 0.0f)
      {
         theVolume = RAStoProduct ;  // + CobeConfig::data().MssPtfVolume - CobeConfig::data().mssPtfFilterLoss;
      }
      else
      {
         theVolume = 0.0f;
      }

   }
   else
   {

      theVolume = 0.0f;
   }

   // DataLog( log_level_proc_info ) << "ResidualAlgorithm   : ssVolumePastValve"  << theVolume  << endmsg;
   return theVolume;


}
///////////////////////////////////////////////////////////////////////////////////////////////



float ResidualAlgorithm::PTFFilterHoldUpRBCDose (const float RbcVolm_actual, const float RAStoProduct, const float incompleteRas)
{

   const float volumeRbcPastValve =  RbcVolm_actual;  // rbcVolumePastValue(rbcProdDose);
   float       dose               = 0.0f;

   if ( volumeRbcPastValve > CobeConfig::data().MssPtfVolume )
   {
      if (mssPtf)
      {
         dose = PTFresidual::calculateDose(ssVolumePastValve(RAStoProduct, incompleteRas) ) ;

      }
      else
      {

         dose = 0.0f;
      }

   }
   else
   {

      if (mssPtf)
      {

         dose = volumeRbcPastValve * 0.8f;
      }
      else
      {

         dose = 0.0f;
      }

   }


   if (RbcVolm_actual == 0.0f)
      dose = 0.0f;


   // DataLog( log_level_proc_info ) << "ResidualAlgorithm   : PTFFilterHoldUpRBCDose"  << dose  << endmsg;
   return dose;
}

///////////////////////////////////////////////////////////////////////////////////////////////




float ResidualAlgorithm::PTFFilterHoldUpPls (const float RbcVolm_actual, const float RAStoProduct, const float incompleteRas)
{

   const float pRBCfilter = PTFFilterHoldUpRBCDose(RbcVolm_actual, RAStoProduct, incompleteRas);
   float       plsVolume  = 0.0f;

   if (pRBCfilter > 0.0f)
   {
      plsVolume = (pRBCfilter / 0.8f - pRBCfilter);
   }
   else
   {

      plsVolume =  0.0f;
   }

   if (plsVolume < 2.5f)
      plsVolume = 2.5f;


   if (RbcVolm_actual == 0.0f)
      plsVolume = 0.0f;

   if (!mssPtf)
      plsVolume = 0.0f;


   // DataLog( log_level_proc_info ) << "ResidualAlgorithm   : PTFFilterHoldUpPls"  << plsVolume  << endmsg;
   return plsVolume;

}

///////////////////////////////////////////////////////////////////////////////////////////////

float ResidualAlgorithm::filterVolume ()
{
   if (mssPtf)
   {
      return CobeConfig::data().MssPtfVolume;
   }
   else
   {
      return 0.0f;
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////

float ResidualAlgorithm::ssInFilter (const float RbcVolm_actual, const float RAStoProduct, const float incompleteRas)
{

   float ssInFilterVolm = 0.0f;
   if (!mssPtf)
   {
      ssInFilterVolm = 0.0f;
   }
   else
   {

      ssInFilterVolm = ( filterVolume()
                         - PTFFilterHoldUpRBCDose(RbcVolm_actual, RAStoProduct, incompleteRas)
                         - PTFFilterHoldUpPls(RbcVolm_actual, RAStoProduct, incompleteRas) );
   }

   // DataLog( log_level_proc_info ) << "ResidualAlgorithm   : ssInFilter"  << ssInFilterVolm  << endmsg;
   return ssInFilterVolm;
}

///////////////////////////////////////////////////////////////////////////////////////////////
float ResidualAlgorithm::getBaseResidual (const cassette::cassetteType setType,
                                          const int plasmaRinseback_ON,
                                          const int salineRinseback_ON)
{

   float baseRes = 0.0;
   if (setType == cassette::PLTPLSRBC)
   {
      if (plasmaRinseback_ON)
      {
         baseRes =  CobeConfig::data().ResPltPlsRbc_PlasmaRB_RBC;
      }
      else
      {
         baseRes =  CobeConfig::data().ResPltPlsRbc_NormalRB_RBC;

      }

   }
   else if (setType == cassette::RBCPLS)
   {
      if (salineRinseback_ON)
      {
         baseRes = CobeConfig::data().ResPlsRbc_SalineRB_RBC;
      }
      else
      {
         baseRes = CobeConfig::data().ResPlsRbc_NormalRB_RBC;

      }
   }



   return baseRes;


}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// for DEBUG only::::

#ifdef __WIN32__
void ResidualAlgorithm::load_values_for_debug (float pRbc_target1_in,    // D5
                                               float pRbc_target2_in,    // F5
                                               float RAS_target1_in,     // D6
                                               float RAS_target2_in,     // F6
                                               float pRbc_actual1_in,
                                               float pRbc_actual2_in,
                                               float RbcVolm1_actual1_in,
                                               float RbcVolm2_actual2_in,
                                               float RAS_actual1_in,
                                               float RAS_actual2_in,
                                               float hct_donor_in,         // D17      hct*100 so like 45.4f
                                               float vb_processed_in,      // D18
                                               bool mssPtf_in,             // D29
                                               bool SalineRB_in,           // D31
                                               bool PlasmaRB_in,           // D31
                                               bool NormalRB_in,           // D31
                                               bool firstReturnEntered_in, // D30
                                               bool BlackStamp_in,         // D32
                                               bool rinsebackComplete_in   // D33
                                               )
{





   pRbc_target1 = pRbc_target1_in;
   pRbc_target2 = pRbc_target2_in;
   RAS_target1  = RAS_target1_in;
   RAS_target2  = RAS_target2_in;




   RbcVolm_actual1 = RbcVolm1_actual1_in;
   RbcVolm_actual2 = RbcVolm2_actual2_in;

   pRbc_actual1    = pRbc_actual1_in;
   pRbc_actual2    = pRbc_actual2_in;
   RAS_actual1     = RAS_actual1_in;
   RAS_actual2     = RAS_actual2_in;


   hct_donor            =  hct_donor_in / 100.0f;
   vb_processed         =  vb_processed_in;

   incompleteRASVolume1 = RAS_target1 - RAS_actual1 - CobeConfig::data().mssPtfFilterLoss + CobeConfig::data().MssPtfVolume;
   incompleteRASVolume2 = RAS_target2 - RAS_actual2 - CobeConfig::data().mssPtfFilterLoss + CobeConfig::data().MssPtfVolume;

   mssPtf               = mssPtf_in;

   // only one is true!
   SalineRB           = SalineRB_in;
   PlasmaRB           = PlasmaRB_in;
   NormalRB           = NormalRB_in;

   firstReturnEntered = firstReturnEntered_in;
   BlackStamp         = BlackStamp_in;
   rinsebackComplete  = rinsebackComplete_in;

}
#endif

/* FORMAT HASH 858ac08597fff9a67f7e6cf499d8eb3b */
