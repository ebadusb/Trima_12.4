#ifndef __PREDICT_STRINGS_H__
#define __PREDICT_STRINGS_H__
const char* PredictStrings[] = {
   "GOOD_PRODUCT",                   // 0
   "BAD_POSTHCT",                    // 1
   "BAD_POSTCOUNT",                  // 2
   "BAD_HCT_WITH_PLASMA",            // 3
   "YIELD_NOT_ACHIEVED",             // 4
   "PLASMA_NOT_ACHIEVED",            // 5
   "RBC_NOT_ACHIEVED",               // 6
   "HYPOVOLEMIA_EXCEEDED",           // 7
   "BAD_POSTHCT_TYPE2",              // 8
   "BAD_POSTCOUNT_TYPE2",            // 9
   "INTERNAL_ERROR",                 // 10 returned when flows cannot be determined
   "INVALID_PRODUCT_COMBINATION",    // 11 product cannot be satisfied without
									//    exceeding desired configured product
   "PRODUCT_ACHIEVED",               // 12 product already achieved
   "TIME_EXCEEDED",                  // 13 product cannot be collected in specified time
   "RBC_VBP_EXCEEDED",               // 14 VBP allowed during RBC collection exceeded
   "BLOOD_TYPE_MISMATCH",            // 15 blood type does not match request
   "CASSETTE_TYPE_MISMATCH",         // 16 cassette type does not match request
   "VBP_EXCEEDED",                   // 17 VBP needed to collect product exceeds
   "REMAINING_TIME_TOO_SHORT",       // 18 time remaining in procedure less than
   "DUPLICATE_PRODUCT",              // 19
   "QIN_BELOW_MINIMUM_SPEC",         // 20 instantaneous Qin below pump specification
   "QRP_BELOW_MINIMUM_SPEC",         // 21 instantaneous Qrp below pump specification
   "INCORRECT_DESIGNATOR",           // 22 primary/secondary designator not in effect
   "DONOR_INFO_OUT_OF_RANGE",        // 23 redundant donor information check fails
   "UNOPTIMIZED",                     // 24
	"VIN_MAX_EXCEEDED",               // 25 procedure exceeded VinMaxTarget
	"VIN_MIN_NOT_MET",                // 26 procedure did not meet VinMinTarget
	"CPS_PRESSURE_PRECLUDES_RBCS",    // 27  no RBC collections due to CPS pressure alarms IT8160
	"TRALI_CRITERIA_NOT_MET",         // 28 procedure contains plt or pls when male-only pls or plt are turned on.
	"RBC_BAG_VOLUME_VIOLATION",       // 29 if your RBC prod dose is GT one bag max (275) and less than your split threshold.      
	"DRBC_VOL_LIMIT_VIOLATION", 	   // 30 Donor's TBV < DRBC Volume Limit,
	"PRODUCT_METERING_STATUS_LOCKED"

};

#endif /* __PREDICT_STRINGS_H__ */
