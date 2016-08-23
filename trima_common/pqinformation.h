// Static Model

//

#ifndef __VerifyFlags__PQInformation__
#define __VerifyFlags__PQInformation__

#include <stdarg.h>

// Include files
#include "procpqi_cds.h"
#include "verifyflags.h"
#include "reasonMap.h"

// Interface Class used to access the flags contained in Product Quality Data.
class PQInformation
{
public:
   PQInformation();
   ~PQInformation();

   bool AtLeastOneFlag () const;

   bool AtLeastOneFlag (ENUMERATED_FLAG Flag1,
                        ENUMERATED_FLAG Flag2 = NO_FLAG,
                        ENUMERATED_FLAG Flag3 = NO_FLAG,
                        ENUMERATED_FLAG Flag4 = NO_FLAG,
                        ENUMERATED_FLAG Flag5 = NO_FLAG,
                        ENUMERATED_FLAG Flag6 = NO_FLAG) const;

   bool AtLeastOneReason (ENUMERATED_FLAG theFlag) const;

   bool AtLeastOneReason (ENUMERATED_FLAG theFlag,
                          ENUMERATED_REASON Reason1,
                          ENUMERATED_REASON Reason2 = NULL_REASON,
                          ENUMERATED_REASON Reason3 = NULL_REASON,
                          ENUMERATED_REASON Reason4 = NULL_REASON,
                          ENUMERATED_REASON Reason5 = NULL_REASON,
                          ENUMERATED_REASON Reason6 = NULL_REASON,
                          ENUMERATED_REASON Reason7 = NULL_REASON,
                          ENUMERATED_REASON Reason8 = NULL_REASON,
                          ENUMERATED_REASON Reason9 = NULL_REASON,
                          ENUMERATED_REASON Reason10 = NULL_REASON,
                          ENUMERATED_REASON Reason11 = NULL_REASON,
                          ENUMERATED_REASON Reason12 = NULL_REASON,
                          ENUMERATED_REASON Reason13 = NULL_REASON,
                          ENUMERATED_REASON Reason14 = NULL_REASON,
                          ENUMERATED_REASON Reason15 = NULL_REASON,
                          ENUMERATED_REASON Reason16 = NULL_REASON,
                          ENUMERATED_REASON Reason17 = NULL_REASON,
                          ENUMERATED_REASON Reason18 = NULL_REASON,
                          ENUMERATED_REASON Reason19 = NULL_REASON,
                          ENUMERATED_REASON Reason20 = NULL_REASON,
                          ENUMERATED_REASON Reason21 = NULL_REASON,
                          ENUMERATED_REASON Reason22 = NULL_REASON,
                          ENUMERATED_REASON Reason23 = NULL_REASON,
                          ENUMERATED_REASON Reason24 = NULL_REASON) const;

   int NumberOfReasons (ENUMERATED_FLAG theFlag) const;

   bool isReason (ENUMERATED_FLAG theFlag,
                  ENUMERATED_REASON theReason) const;

   ENUMERATED_REASON GetReason (ENUMERATED_FLAG theFlag,
                                int which) const;

   bool LabelPlatelets () const;

   bool LabelPlasma () const;

   bool LabelRbcs () const;
   bool LabelRbcsProd2 () const;

   bool ShouldShow (ENUMERATED_FLAG flag, ENUMERATED_REASON_TO_SHOW show) const;

   // returns true if reasons to show
   // codes contains the characters as listed in the trima operator manual (Table 6-1 on 5.1)
   bool ReasonsShown (ENUMERATED_FLAG flag, string& codes) const;

   void LogReasons () const;

   friend DataLog_Stream& operator << (DataLog_Stream& os, ENUMERATED_FLAG flag);
   friend DataLog_Stream& operator << (DataLog_Stream& os, ENUMERATED_REASON_TO_SHOW show);

protected:

private:

   static REASON_MAPPING _reason_map[];

   ProcPQI_CDS           _ProcPQICDS;

   bool orFlags (ENUMERATED_FLAG aFlag,
                 ... /*_Variable_*/) const;

   bool orReasons (ENUMERATED_FLAG theFlag,
                   ENUMERATED_REASON aReason,
                   ... /*_Variable_*/) const;
};

// END CLASS DEFINITION PQInformation


#endif // __VerifyFlags__PQInformation__

/* FORMAT HASH 7ca92d6fb287ce11ffc471dc5d92101f */
