// Static Model

//

#ifndef __VerifyFlags__PQControl__
#define __VerifyFlags__PQControl__


// Include files
#include "reasonlist.h"
#include "verifyflags.h"
#include "productqualitydata.h"

// This is the service class for the classes dervived from aPQMonitor.
class PQControl
{
public:
   PQControl();
   virtual ~PQControl();

   int GetReasons (ENUMERATED_FLAG theFlag) const;
   int GetReasons (const ProductQualityData& data, ENUMERATED_FLAG theFlag) const;

   int isReason (ENUMERATED_REASON theReason) const;

   int isReason (ENUMERATED_REASON theReason,
                 ENUMERATED_FLAG theFlag) const;

   int Initialized () const
   {
      return initialized;
   }

   int Initialized ();

   // all plt pls and RBC product 1 (or RBC singles) reasons
   void PutReason (ENUMERATED_REASON theReason);
   // only RBC product 2 reasons!!
   void PutReasonRBCprod2 (ENUMERATED_REASON theReason);

   void RemoveReason (ENUMERATED_REASON theReason);

   void AddReasonToFlag (ENUMERATED_REASON theReason,
                         ENUMERATED_FLAG theFlag);

   void RemoveReasonFromFlag (ENUMERATED_REASON theReason,
                              ENUMERATED_FLAG theFlag);
private:

   int                initialized;

   ProductQualityData valid;

   void Initialize ();  // May eliminate, called only by constructor
};

// END CLASS DEFINITION PQControl

#endif // __VerifyFlags__PQControl__

/* FORMAT HASH 7fa179f595b12dee3d81d6b158bd48fb */
