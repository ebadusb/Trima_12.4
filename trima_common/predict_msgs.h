#ifndef PREDICT_MSGS_H
#define PREDICT_MSGS_H

#include <sys/types.h>
#include "cassette.h"
#include "trima_datalog.h"
#include "configdef.h"
#include "datalog.h"
#include "carray.h"
#include "message_payload.h"

const int NumGuiProcedures = 3;

enum INTERNAL_PREDICT_RESULTS
{
   GOOD_PRODUCT,                   // 0
   BAD_POSTHCT,                    // 1
   BAD_POSTCOUNT,                  // 2
   BAD_HCT_WITH_PLASMA,            // 3
   YIELD_NOT_ACHIEVED,             // 4
   PLASMA_NOT_ACHIEVED,            // 5
   RBC_NOT_ACHIEVED,               // 6
   HYPOVOLEMIA_EXCEEDED,           // 7
   BAD_POSTHCT_TYPE2,              // 8
   BAD_POSTCOUNT_TYPE2,            // 9
   INTERNAL_ERROR,                 // 10 returned when flows cannot be determined
   INVALID_PRODUCT_COMBINATION,    // 11 product cannot be satisfied without
                                   //    exceeding desired configured product
   PRODUCT_ACHIEVED,               // 12 product already achieved
   TIME_EXCEEDED,                  // 13 product cannot be collected in specified time
   RBC_VBP_EXCEEDED,               // 14 VBP allowed during RBC collection exceeded
   BLOOD_TYPE_MISMATCH,            // 15 blood type does not match request
   CASSETTE_TYPE_MISMATCH,         // 16 cassette type does not match request
   VBP_EXCEEDED,                   // 17 VBP needed to collect product exceeds
   /*
       IT 7775 -- status 18 is obsolete with 5.1 or higher Trimas
   REMAINING_TIME_TOO_SHORT,       // 18 time remaining in procedure less than
   */
   DUPLICATE_PRODUCT = 19,  /* keep the numbering the same*/ // 19
   QIN_BELOW_MINIMUM_SPEC,                                   // 20 instantaneous Qin below pump specification
   QRP_BELOW_MINIMUM_SPEC,                                   // 21 instantaneous Qrp below pump specification
   INCORRECT_DESIGNATOR,                                     // 22 primary/secondary designator not in effect
   DONOR_INFO_OUT_OF_RANGE,                                  // 23 redundant donor information check fails
   UNOPTIMIZED,                                              // 24
   VIN_MAX_EXCEEDED,                                         // 25 procedure exceeded VinMaxTarget
   VIN_MIN_NOT_MET,                                          // 26 procedure did not meet VinMinTarget
   CPS_PRESSURE_PRECLUDES_RBCS,                              // 27  no RBC collections due to CPS pressure alarms IT8160
   TRALI_CRITERIA_NOT_MET,                                   // 28 procedure contains plt or pls when male-only pls or plt are turned on.
   RBC_BAG_VOLUME_VIOLATION,                                 // 29 if your RBC prod dose is GT one bag max (275) and less than your split threshold.
   DRBC_VOL_LIMIT_VIOLATION,                                 // 30 Donor's TBV < DRBC Volume Limit,
   PRODUCT_METERING_STATUS_LOCKED,                           // 31 Cannot switch from PAS to non-PAS procedure
   DRBC_WITH_PLASMA,                                         // 32 DRBC procedures with plasma not allowed (regional feature)
   NOT_SUPPORTED_PROCEDURE                                   // 33 procedure with PAS, RAS/PTF, RBC collection,AMAP-Plt not allowed (regional feature)
};


// Communication between GUI, Proc, and Predict needs to use sequence numbers in
// several messages.  Some of those sequence numbers have special meanings; here
// is the definition.  [Note: the namespace is to simulate C++11's "enum class"
// behavior --DGS]
namespace GuiPredictSequenceValues
{

const int DEFAULT      = 0;     // default value for 'no sequence number'
const int DVITALS_ACK  = 1;     // response to a DVitalsFromGUI event
const int ADJUST_START = 10;    // AdjustManager starts sequence numbers from
                                // here; all values above this are reserved for
                                // AdjustManager's use
}



// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//         ---------------- GUI to Procedure messages -----------------
//
//   These constants should be used for messages from GUI to Procedure
//   (message enumeration GuiToProcPredictRequestMsg)
//
enum GuiToProcPredictRequest_t
{

   IS_PROCEDURE_VALID,              // returns ProcToGuiPredictStatus
   SELECT_NEXT,                     // returns ProcedureInformation
   SELECT_PREVIOUS,                 // returns ProcedureInformation
   SELECT_CURRENT,                  // returns ProcedureSelectedMsg
   REPREDICT_ALL,                   // returns ProcedureInformation
   REPREDICT_ALL_OPTIMAL            // returns ProcedureInformation
};


//  Enumeration of codes for GuiProceduresUpdated messages; these mirror the
//  GuiToProcPredictRequest_t values, plus a few more.  (NOTE: this mirroring is
//  important, since we check for (int-casted) equality.
enum GuiProceduresUpdated_t
{
   NON_REQUEST = -1,
   IS_PROCEDURE_VALID_ACK,
   SELECT_NEXT_ACK,
   SELECT_PREVIOUS_ACK,
   SELECT_CURRENT_ACK,            // NOTE: this one is never used, included for completeness
   REPREDICT_ALL_ACK,
   REPREDICT_ALL_OPTIMAL_ACK,
   FLOW_ADJUSTMENT_ACK,
   DONOR_TIME_ACK
};


//
//
//   Message enumeration GuiToProcDonorTime should be used to send donor
//   time.  The integer data should contain the time in minutes.  Procedure
//   will respond with a ProcedureInformation message.
//


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//         ---------------- Procedure to GUI messages -----------------
//
//
//   These constants should be used for messages from procedure to GUI
//   (message enumeration ProcToGuiPredictStatus)
//
enum ProcToGuiPredictStatus_t
{
   PROCEDURE_VALID,
   REQUEST_SELECTION_SCREEN,
   PROCEDURES_SENT
};


//
//
//   These constants should be used for messages from procedure to GUI
//   (message enumeration ProcedureInformation)
//
struct ProductOptionStruct
{
   ProductOptionStruct();


   bool                   validProcedure;
   int                    procedure_index; // GUI may want to check configured values
   bool                   is_collecting_platelets;
   float                  estimated_platelets;
   float                  estimated_platelet_volume;
   float                  estimated_plasma;
   float                  estimated_plasma_no_ac;
   float                  estimated_rbcs;
   float                  estimated_rbc_dose;
   float                  procedure_time;
   cassette::cassetteType recommended_cassette;
   bool                   replacement_fluid_required;
   bool                   count_icon_needed;
   bool                   metered_platelet;
   bool                   metered_rbc;
   bool                   ptf_rbc;
   bool                   ptf_split;
   float                  estimated_qin_max;       // MAX(Qin-draw) for all the substates.
   float                  estimated_vac;           // Volume of AC needed.
   float                  estimated_tbv_processed; // Total blood volume processed
   AMAPPLTTYPES           plateletsAmapSize;
   bool                   comp_pls_entire;
   float                  comp_pls_volume;
   float                  comp_pls_PAS_volume;
   int                    estimated_post_platelet_count;

};


enum PROCEDURE_INDEX { PREV, CURR, NEXT };
typedef carray<ProductOptionStruct, NumGuiProcedures> ProcedureInformationStruct;


bool operator == (const ProductOptionStruct& left, const ProductOptionStruct& right);
bool operator == (const ProcedureInformationStruct& left, const ProcedureInformationStruct& right);



//
//
//   Message enumeration ProcToGuiCassetteInfo should be used to send the
//   desired cassette to GUI.  The integer data will contain the cassette
//   type (cassette::cassetteType) cast into an int.
//


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//     ---------------- Procedure to Procedure messages -----------------
//
//
//  These constants should be used internally in procedure
//  (message enumeration DoPrediction) (can be OR'd together)
//
enum DoPrediction_t
{
   CLEAR_ALL             = 0,
   DONOR_INFO_CHANGED    = 1,
   DONOR_INFO_UNCHANGED  = 2,
   DONOR_INFO_PRECONNECT = 4,
   ADJUSTMENT            = 8,
   REPREDICT_ONLY        = 16,
   PTF_FILTER_RECALC     = 32,
   AUTO_FLOW_ADJUSTMENT  = 64,
   //////////////////////////////////  the typdef below needs the biggest value always defined.
   BIGGEST_PREDICT_MSG   =127

};

typedef Payload <DoPrediction_t, BIGGEST_PREDICT_MSG> DoPredictionPayload;



//
//   Message enumeration DeleteProduct can be used to delete any currently
//   active product.  The integer data should specify the product number
//   to be deleted.

//
//   Message enumeration ModifyProduct can be used to perform various actions
//   on a specific product.
//
enum ModifyProduct_t
{ SET_PRODUCT_STATUS, // send desired status in auxdata
  REOPTIMIZE_PRODUCT,
  SET_PRODUCT_AS_CURRENT,
  DELETE_ALL_PRODUCTS, // delete all defined products
};

struct ModifyProductStruct
{
   int             product_number;
   ModifyProduct_t action;
   int             aux_data;
};


// Dlog output functions for the predict message structs and enums.
//
DataLog_Stream& operator << (DataLog_Stream& os, const INTERNAL_PREDICT_RESULTS& results);
DataLog_Stream& operator << (DataLog_Stream& os, const GuiProceduresUpdated_t& request);
DataLog_Stream& operator << (DataLog_Stream& os, const ProcToGuiPredictStatus_t& status);
DataLog_Stream& operator << (DataLog_Stream& os, const ProductOptionStruct& prod);
DataLog_Stream& operator << (DataLog_Stream& os, const DoPrediction_t& predOpts);
DataLog_Stream& operator << (DataLog_Stream& os, const ModifyProduct_t& prod);
DataLog_Stream& operator << (DataLog_Stream& os, const ModifyProductStruct& prod);
DataLog_Stream& operator << (DataLog_Stream& os, const PROCEDURE_INDEX& index);

template<class Stream>
Stream& operator << (Stream& os, const GuiToProcPredictRequest_t& request)
{
   switch (request)
   {
      case IS_PROCEDURE_VALID    : os << "IS_PROCEDURE_VALID"    ; break;
      case SELECT_NEXT           : os << "SELECT_NEXT"           ; break;
      case SELECT_PREVIOUS       : os << "SELECT_PREVIOUS"       ; break;
      case SELECT_CURRENT        : os << "SELECT_CURRENT"        ; break;
      case REPREDICT_ALL         : os << "REPREDICT_ALL"         ; break;
      case REPREDICT_ALL_OPTIMAL : os << "REPREDICT_ALL_OPTIMAL" ; break;
   }
//   return os;
   return os << "(" << (int)request << ")";
}


#endif

/* FORMAT HASH fa7d5d4d8dc8814ff2e7bbb1ebf300f0 */
