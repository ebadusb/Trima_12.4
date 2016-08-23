/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      dvitals.cpp
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/

#include <vxworks.h>
#include "cobeconfig.h"
#include "donorbmi.h"
#include "dvitals.h"
#include "trima_datalog.h"
#include "callback.h"
#include "message.h"
#include "predict_msgs.h"
#include "procdata.h"
#include "datalog_periodic.h"

static const float DEFAULT_PRECOUNT    = 200.0f;
DataLog_SetHandle  CDVitals::_DonorSet = DATALOG_NULL_SET_HANDLE;
ProcData*          CDVitals::_ProcData = NULL;

using GuiPredictSequenceValues::DVITALS_ACK; // sent to predict to identify
                                             // responses to DVitalsFromGui events.

CDVitals::CDVitals()
   : _msgFromGui      (Callback<CDVitals>(this, &CDVitals::ProcessGuiMessage)),
     _msgToSafety     (MessageBase::SEND_GLOBAL),
     _doPredictionMsg (MessageBase::SEND_LOCAL),
     _predictResponded(MessageBase::SEND_LOCAL),
     _tempDonor       (),
     _final           (),
     _previousFinal   (),
     _donorConnected  (false)
{}

CDVitals::~CDVitals()
{}

void CDVitals::Initialize ()
{
   if ( _DonorSet == DATALOG_NULL_SET_HANDLE )
   {
      datalog_CreatePeriodicSet("Proc_DonorData", &_DonorSet);
      _ProcData = new ProcData;

      int (BaseElement<int>::* int_func) (void) const       = &BaseElement<int>::Get;
      float (BaseElement<float>::* float_func) (void) const = &BaseElement<float>::Get;

      datalog_AddMemberFunc(_DonorSet, &_ProcData->Donor().Female, int_func, "DonorGender", "donor gender (1=female 0=male)", "%d");
      datalog_AddMemberFunc(_DonorSet, &_ProcData->Donor().Height, float_func, "DonorHeight", "donor height", "%.1lf");
      datalog_AddMemberFunc(_DonorSet, &_ProcData->Donor().Weight, float_func, "DonorWeight", "donor weight", "%.1lf");
      datalog_AddMemberFunc(_DonorSet, &_ProcData->Donor().TBV, float_func, "DonorTBV", "donor TBV (ml)", "%.1lf");
      datalog_AddMemberFunc(_DonorSet, &_ProcData->Donor().BMI, float_func, "DonorBMI", "donor BMI", "%.1lf");
      datalog_AddMemberFunc(_DonorSet, &_ProcData->Donor().PreCnt, float_func, "DonorPreCount", "donor platelet pre-count", "%.1lf");
      datalog_AddMemberFunc(_DonorSet, &_ProcData->Donor().Hct, float_func, "DonorHematocrit", "donor hematocrit", "%.2lf");
      datalog_AddMemberFunc(_DonorSet, &_ProcData->Donor().BloodType, int_func, "DonorBloodType", "donor blood type (see documentation)", "%d");
      datalog_AddMemberFunc(_DonorSet, &_ProcData->Donor().SampleVolume, float_func, "DonorSampleVolume", "donor sample volume (ml)", "%.1lf");

      forceLogging();
   }
}

void CDVitals::SetDonorIsConnected ()
{
   _donorConnected = true;
}

int CDVitals::DonorIsConnected () const
{
   return _donorConnected;
}

void CDVitals::forceLogging ()
{
   datalog_ForcePeriodicOutput(_DonorSet);
}

void CDVitals::ProcessGuiMessage ()
{
   bool        sendProcMsg = false;
   ProcData    pd;

   State_names curState = pd.SystemState();

   _tempDonor = _msgFromGui.getData();

   // Only fill this stuff in if it is missing.  The Vista interface sets all of this where
   // GUI does not.  Also there is a potential race condition in the Vista interface as to
   // what gets set first, the config or the donor since they are both messages.
   if (!_tempDonor.ValidUnits())
   {
      _ProcData->FillInDonorUnits(_tempDonor.Data() );
   }

   //
   // Set the sample volume if it is unset ...
   if ( _tempDonor.SampleVolume() < 0.0f )
      _tempDonor.Data().fDonorSampleVolume = CobeConfig::data().SampleVolume;

   if (!_tempDonor.DonorInfoConfirmed() && (curState >= DONOR_CONNECTED))
   {
      if (_previousFinal.ValidHct())
      {
         // In the case operator changes Donor Info without confirmed the change
         _tempDonor.Data().fDonorHct         = _previousFinal.Hct();
         _tempDonor.Data().fDonorPltPrecount = _previousFinal.PreCnt();
         _tempDonor.Data().DonorBloodType    = _previousFinal.BloodType();
      }
      else
      {
         // Shall only occur in PFR operator changes Donor Info without confirmed the change
         _previousFinal.Data().fDonorHct         = _tempDonor.Hct();
         _previousFinal.Data().fDonorPltPrecount = _tempDonor.PreCnt();
         _previousFinal.Data().DonorBloodType    = _tempDonor.BloodType();
      }
   }

   // donor vitals not allowed to change after donor is connected
   if ( !DonorIsConnected() )
   {
      // if all vitals are good, transfer them
      if ( _tempDonor.DonorVitalsEntered() )
      {
         _tempDonor.CalTBV();
         _final = _tempDonor;

         if ( _tempDonor.DonorInfoConfirmed() )
         {
            sendProcMsg = true;
         }
      }
   }

   // send on to safety
   _msgToSafety.send(_tempDonor.cData());


   if ( _tempDonor.PreCnt() == 0 )
   {
      _tempDonor.Data().fDonorPltPrecount = DEFAULT_PRECOUNT;
   }

   if ( _tempDonor.DonorInfoConfirmed() )
   {
      // compare info to final donor and fatal error if different?

      // check the validity of the info
      if ( _tempDonor.DonorInfoEntered() )
      {
         sendProcMsg = true;

      }
   }


   // Transfer donor info if they are meaningful
   if ( ( _tempDonor.Hct() != 0.0f ) && _tempDonor.ValidHct() )
   {
      _final.Data().fDonorHct = _tempDonor.Hct();
   }

   if ( _tempDonor.ValidPreCnt() )
   {
      _final.Data().fDonorPltPrecount = _tempDonor.PreCnt();
   }

   if ( _tempDonor.ValidBloodType() )
   {
      _final.Data().DonorBloodType = _tempDonor.BloodType();
   }

   _ProcData->MakeDonorWritable();
   _ProcData->Donor().Female.Set(_final.Female());
   _ProcData->Donor().Height.Set(_final.Height());
   _ProcData->Donor().Weight.Set(_final.Weight());
   _ProcData->Donor().TBV.Set(_final.TBV());
   _ProcData->Donor().TBVOgawa.Set(_final.TBVOgawa());
   _ProcData->Donor().PreCnt.Set(_final.PreCnt());
   _ProcData->Donor().Hct.Set(_final.Hct() / 100.0f);
   _ProcData->Donor().BloodType.Set(_final.BloodType());
   _ProcData->Donor().SampleVolume.Set(_final.SampleVolume());
   _ProcData->Donor().DownloadStatus.Set(_final.DonorDownloadStatus());
   _ProcData->MakeDonorReadable();

   // if ( sendProcMsg )
   // {
   //    bool dataUnchanged = _previousFinal.SameAs( _final );
   //    _previousFinal = _final;

   //    _ProcData->MakeRunWritable();
   //    if ( _ProcData->TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::BLOOD ) // V5: "or if PFR"
   //       _ProcData->Run().PredictRequest.Set(_ProcData->Run().PredictRequest.Get() | DONOR_INFO_PRECONNECT );
   //    else if ( dataUnchanged )
   //       _ProcData->Run().PredictRequest.Set(_ProcData->Run().PredictRequest.Get() | DONOR_INFO_UNCHANGED );
   //    else
   //       _ProcData->Run().PredictRequest.Set(_ProcData->Run().PredictRequest.Get() | DONOR_INFO_CHANGED );
   //    _ProcData->MakeRunReadable();

   //    DonorBMI::calculate();
   // }



   if ( sendProcMsg )
   {
      bool           dataUnchanged = _previousFinal.SameAs(_final);

      DoPrediction_t request       = (_ProcData->TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::BLOOD
                                      ?  DONOR_INFO_PRECONNECT
                                      : (dataUnchanged
                                         ?  DONOR_INFO_UNCHANGED
                                         :  DONOR_INFO_CHANGED ) );

      DoPredictionPayload pred_payload(request, DVITALS_ACK);
      _doPredictionMsg.send(pred_payload);

      DataLog(log_level_proc_info) << "DVitals PREDICT_REQUEST " << request
                                   << " send_msg " << sendProcMsg
                                   << endmsg;

      _previousFinal = _final;
      DonorBMI::calculate();
   }
   else
   {
      // We don't really have any work for Predict to do, but GUI is waiting for
      // a response; so give GUI what it wants
      DataLog(log_level_proc_info) << "DVitals responding to GUI with PredictResponded("
                                   << DVITALS_ACK << ')'
                                   << endmsg;
      _predictResponded.send(DVITALS_ACK);
   }

   forceLogging();
}

int CDVitals::SameAsCDS (const CDonor& donor) const
{
   //
   //
   //  Note use of fabs necessary for proper comparison.  Only Vista can set
   //  noninteger values so tolerance set to a small arbitrary threshold.
   //
   return(
      (donor.Female() == _ProcData->Donor().Female.Get()) &&
      (donor.BloodType() == _ProcData->Donor().BloodType.Get()) &&
      (fabs(donor.Hct() - (_ProcData->Donor().Hct.Get() * 100.0)) < 0.01) &&
      (fabs(donor.Height() - _ProcData->Donor().Height.Get()) < 0.01) &&
      (fabs(donor.Weight() - _ProcData->Donor().Weight.Get()) < 0.01) &&
      (fabs(donor.PreCnt() - _ProcData->Donor().PreCnt.Get()) < 0.01) &&
      (fabs(donor.SampleVolume() - _ProcData->Donor().SampleVolume.Get()) < 0.01)
      );
}

const char* CDVitals::convertGender (const BaseElement< int >& female)
{
   if (female == 1) return "Female";
   return "Male";
}

const char* CDVitals::convertHeight (const BaseElement<float>& height)
{
   static char heightStr[20];

   if ( _ProcData->Config().LangUnit.Get().key_height == HEIGHT_UNITS_IN )
      sprintf(heightStr, "%f in", height.Get() );
   else
      sprintf(heightStr, "%f cm", height.Get() );

   return heightStr;
}

const char* CDVitals::convertWeight (const BaseElement<float>& weight)
{
   static char weightStr[20];

   if ( _ProcData->Config().LangUnit.Get().key_weight == WEIGHT_UNITS_LB )
      sprintf(weightStr, "%f lb", weight.Get() );
   else
      sprintf(weightStr, "%f kg", weight.Get() );

   return weightStr;
}

/* FORMAT HASH 926b33e22002ca31af0ff72864b12cd5 */
