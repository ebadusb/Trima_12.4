/*__________________________________________________________________________________________

  Predict Diver:

  Interface to the Predict Engine for external use (external to Trima, that
  is). Uses the compilation firewall (pimpl) idiom to reduce compile-time
  dependencies.

  Usage:

  Usage details depend on the nature of the application, but the process will be
  something approximately like this:


  1) Obviously, create an object of PredictDriver.

  2) Fill in configuration information by accessing the various config objects:
     Config                       getPredictConifg() (proc/predict/predictConfig.h)
     ProcTrimaSet_CDS             getTrimaSet()      (cds/proctrimaset_cds.h)
     CobeConifg::CobeConfigStruct getCobeConfig()    (trima_common/cobeconfig.h)

  3) Configure the donor with FillInDonor().  (Note: previously, external users
     accessed the ProcDonor_CDS object to perform this step.  For user
     convenience, we now take donor info as parameters to this function, and
     handle the CDS object internally.)

  4) Create procedures using the create_procedure call.  The predict engine has
     strict preconditions for dealing with its list of procedures, which it
     checks and enforces with an iron fist. The following usage rules are

     !!!  IMPORTANT  !!

     The procedure numbers must form a contiguous, increasing block of integers
     starting from 1.  That is:

     -  Procedures must be created in procedure_number order.
     -  Procedure numbers must start at 1.
     -  There must be no gaps.
     -  The procedureDetail pointers are owned by the predict engine, so the
        pointers returned by the getProcedure call are *non-owning*.  Don't
        delete them!

     Failure to follow these rules will result in fatal errors at best, and
     undefined behavior at anything-less-than-best.  How Trima code fatal errors
     manifest in your operating system is anybody's guess, but it won't be
     pretty.

  5) Call checkCassetteTypes().  This matches the procedures against the list of
     available cassettes (as specified in the config objects).

  6) Call SetAllUnoptimized().

  7) For each procedure, get the procedureDetail via the getProcedure() method.
     Call predictProcedure() on it; the result is the predicted procedure.

  8) At some point, your PredictDriver object will go out of scope.  It's
     destructor will release any resources that need to be released. This
     includes all procedures and the various configuration object.  The
     procedureDetail pointers and config objects will be deleted during, or at
     some time after, the PredictDriver's destructor runs (depending on
     operating system, compiler, and internal Predict engine details).  All
     pointers and references to such objects should therefore be considered to
     be undefined behavior.

  __________________________________________________________________________________________*/



#ifndef PREDICT_DRIVER_H
#define PREDICT_DRIVER_H

#include "cobeconfig.h"
#include "proctrimaset_cds.h"
#include "predictConfig.h"

struct ProcedureDescription;
class procedureDetail;


class PredictDriver
{
public:

   PredictDriver();
   ~PredictDriver();

   // see trima_common/configdef.h for ProcedureDescription struct.
   void create_procedure (const ProcedureDescription& proc,
                          int procedure_number);

   procedureDetail* getProcedure (int procindex);

   void checkCassetteTypes ();

   void UpdateProcData ();

   void predictProcedure (procedureDetail& pd);

   void FillInDonor (int Female,
                     float Height,    // in configuration units,
                     float Weight,    // in configuration units,
                     float BMI,
                     float TBV,       // ml,
                     float PreCnt,
                     float Hct,       // as fraction, e.g. 0.4,
                     int BloodType,
                     float SampleVolume);

   void SetAllUnoptimized ();

   void FillInConfig ();

   void reset ();                // clears all procedures from procedure list

   // Configuration object access

   Config&                             getPredictConfig ();
   ProcTrimaSet_CDS&                   getTrimaSet ();
   const CobeConfig::CobeConfigStruct& getCobeConfig ();

private: // methods
   // Noncopyable, nonassignable
   PredictDriver(const PredictDriver&);
   PredictDriver& operator = (const PredictDriver&);

private: // data

   struct PDriver* _driver;

};

#endif // include guard

/* FORMAT HASH 0b791eca91262b7e0af1d6a28bbc668f */
