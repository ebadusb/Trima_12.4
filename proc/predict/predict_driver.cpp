/*******************************************************************************
 *
 *  Predict Diver:
 *
 *  Interface to the Predict Engine for external use (external to Trima, that is).
 *
 ******************************************************************************/

#include "predict_driver.h"


#include "predict.h"
#include "procedure_list.hpp"
#include "procdonor_cds.h"

//////////////////////////////////////////////////////////////////////
//
//  Pimpl implementation: here's the impl part of pimpl.
//
//   All functions inline for speed and convenience, since there's almost no
//   real logic here.

struct PDriver
{
public:
   PDriver()
      : _predict(),
        _procedures  (ProcedureList::Instance()),
        _config      (Config::Instance()),
        _trimaSetCDS (ROLE_RW),
        _donorCDS    (ROLE_RW),
        _cobeConfig  (CobeConfig::data())
   {
      _predict.Init();
   }

   ~PDriver() {}


   // see trima_common/configdef.h for ProcedureDescription struct.
   void create_procedure (const ProcedureDescription& def,
                          int procedure_number)
   {
      _procedures.insert(_predict.create_procedure(def, procedure_number));
   }

   procedureDetail* getProcedure (int procindex)       { return _procedures[procindex]; }
   void             checkCassetteTypes       ()                    { _predict.checkCassetteTypes(); }
   void             UpdateProcData           ()                    { _predict.UpdateProcData();     }
   void             predictProcedure         (procedureDetail& pd) { _predict.predictProcedure(pd); }
   void             SetAllUnoptimized        ()                    { _predict.SetAllUnoptimized();  }
   void             FillInConfig             ()                    { _predict.FillInConfig();       }

   void FillInDonor (int Female,
                     float Height,    // in configuration units,
                     float Weight,    // in configuration units,
                     float BMI,
                     float TBV,       // ml,
                     float PreCnt,
                     float Hct,       // as fraction, e.g. 0.4,
                     int BloodType,
                     float SampleVolume)
   {
      _donorCDS.Female.Set(Female);
      _donorCDS.Height.Set(Height);
      _donorCDS.Weight.Set(Weight);
      _donorCDS.BMI.Set(BMI);
      _donorCDS.TBV.Set(TBV);
      _donorCDS.PreCnt.Set(PreCnt);
      _donorCDS.Hct.Set(Hct);
      _donorCDS.BloodType.Set(BloodType);
      _donorCDS.SampleVolume.Set(SampleVolume);
      //
      //
      //   Dynamic hct is not calculated until the run starts so use the initial hct
      //   if necessary.
      //
      // float hct = _ProcRunCDS->AdjustedHct.Get();
      // if (hct <= 0.0f) hct = _ProcDonorCDS->Hct.Get();

   }


   void cleanup () { _predict.cleanup(); }

   // Configuration object access

   Config&                             getPredictConfig () { return _config;      }
   ProcTrimaSet_CDS&                   getTrimaSet      () { return _trimaSetCDS; }
   const CobeConfig::CobeConfigStruct& getCobeConfig    () { return _cobeConfig;  }


private: // methods
   // Noncopyable, nonassignable,
   PDriver(const PDriver&);
   PDriver& operator = (const PDriver&);

private:                    // data
   Predict        _predict; // the actual predict engine
   ProcedureList& _procedures;


   Config&                             _config;

   ProcTrimaSet_CDS                    _trimaSetCDS;
   ProcDonor_CDS                       _donorCDS;
   const CobeConfig::CobeConfigStruct& _cobeConfig;

};

////////////////////////////////////////////////////////////////////////////////////
//
//  There's only ever one Predict engine, so there's only ever one PDriver impl,
//  so make it a proper singleton (this way wew don't have to fiddle around with
//  creating one on the heap, etc).
//

PDriver& Instance ()
{
   static PDriver d;
   return d;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  Now, implementation of PredictDriver -- everything just forwards to the
//  PDriver instance

PredictDriver::PredictDriver()
   : _driver(&Instance()) {}
PredictDriver::~PredictDriver() {}

void PredictDriver::create_procedure (const ProcedureDescription& proc,
                                      int procedure_number)
{
   _driver->create_procedure (proc, procedure_number);
}

procedureDetail* PredictDriver::getProcedure (int procindex)
{
   return _driver->getProcedure(procindex);
}

void PredictDriver::checkCassetteTypes ()                    { _driver->checkCassetteTypes(); }
void PredictDriver::UpdateProcData     ()                    { _driver->UpdateProcData();     }
void PredictDriver::predictProcedure   (procedureDetail& pd) { _driver->predictProcedure(pd); }
void PredictDriver::SetAllUnoptimized  ()                    { _driver->SetAllUnoptimized();  }
void PredictDriver::FillInConfig       ()                    { _driver->FillInConfig();       }
void PredictDriver::reset              ()                    { _driver->cleanup();            }

void PredictDriver::FillInDonor (int Female,
                                 float Height,
                                 float Weight,
                                 float BMI,
                                 float TBV,
                                 float PreCnt,
                                 float Hct,
                                 int BloodType,
                                 float SampleVolume)
{
   _driver->FillInDonor (Female, Height, Weight, BMI, TBV, PreCnt, Hct, BloodType,
                         SampleVolume);
}



Config&                             PredictDriver::getPredictConfig () { return _driver->getPredictConfig (); }
ProcTrimaSet_CDS&                   PredictDriver::getTrimaSet      () { return _driver->getTrimaSet      (); }
const CobeConfig::CobeConfigStruct& PredictDriver::getCobeConfig    () { return _driver->getCobeConfig    (); }

/* FORMAT HASH 16e2b8192a4f25c0580011aa657b375c */
