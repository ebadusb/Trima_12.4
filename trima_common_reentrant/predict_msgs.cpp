#include <vxworks.h>
#include "predict_msgs.h"



//
// Decode Predict results enum for dlog file
//
DataLog_Stream & operator << (DataLog_Stream& os, const INTERNAL_PREDICT_RESULTS& value)
{
   switch (value)
   {
      case GOOD_PRODUCT :
         os << "GOOD PRODUCT";
         break;

      case BAD_POSTHCT :
         os << "BAD POSTHCT";
         break;

      case BAD_POSTCOUNT :
         os << "BAD POSTCOUNT";
         break;

      case BAD_HCT_WITH_PLASMA :
         os << "BAD HCT WITH PLASMA";
         break;

      case YIELD_NOT_ACHIEVED :
         os << "YIELD NOT ACHIEVED";
         break;

      case PLASMA_NOT_ACHIEVED :
         os << "PLASMA NOT ACHIEVED";
         break;

      case RBC_NOT_ACHIEVED :
         os << "RBC NOT ACHIEVED";
         break;

      case HYPOVOLEMIA_EXCEEDED :
         os << "HYPOVOLEMIA EXCEEDED";
         break;

      case BAD_POSTHCT_TYPE2 :
         os << "BAD POSTHCT TYPE2";
         break;

      case BAD_POSTCOUNT_TYPE2 :
         os << "BAD POSTCOUNT TYPE2";
         break;

      case INTERNAL_ERROR :
         os << "INTERNAL ERROR";
         break;

      case INVALID_PRODUCT_COMBINATION :
         os << "INVALID PRODUCT COMBINATION";
         break;

      case PRODUCT_ACHIEVED :
         os << "PRODUCT ACHIEVED";
         break;

      case TIME_EXCEEDED :
         os << "TIME EXCEEDED";
         break;

      case RBC_VBP_EXCEEDED :
         os << "RBC VBP EXCEEDED";
         break;

      case BLOOD_TYPE_MISMATCH :
         os << "BLOOD TYPE MISMATCH";
         break;

      case CASSETTE_TYPE_MISMATCH :
         os << "CASSETTE TYPE MISMATCH";
         break;

      case VBP_EXCEEDED :
         os << "VBP EXCEEDED";
         break;

      case DUPLICATE_PRODUCT :
         os << "DUPLICATE PRODUCT";
         break;

      case QIN_BELOW_MINIMUM_SPEC :
         os << "QIN BELOW MINIMUM SPEC";
         break;

      case QRP_BELOW_MINIMUM_SPEC :
         os << "QRP BELOW MINIMUM SPEC";
         break;

      case INCORRECT_DESIGNATOR :
         os << "INCORRECT DESIGNATOR";
         break;

      case DONOR_INFO_OUT_OF_RANGE :
         os << "DONOR INFO OUT OF RANGE";
         break;

      case UNOPTIMIZED :
         os << "UNOPTIMIZED";
         break;

      case VIN_MAX_EXCEEDED :
         os << "VIN MAX EXCEEDED";
         break;

      case VIN_MIN_NOT_MET :
         os << "VIN MIN NOT MET";
         break;

      case CPS_PRESSURE_PRECLUDES_RBCS :
         os << "CPS PRESSURE PRECLUDES RBCS";
         break;

      case TRALI_CRITERIA_NOT_MET :
         os << "TRALI CRITERIA NOT MET";
         break;

      case RBC_BAG_VOLUME_VIOLATION :
         os << "RBC BAG VOLUME VIOLATION";
         break;

      case DRBC_VOL_LIMIT_VIOLATION :
         os << "DRBC VOL LIMIT VIOLATION";
         break;


      case PRODUCT_METERING_STATUS_LOCKED :
         os << "PRODUCT METERING STATUS LOCKED";
         break;

      default :
         os << "UNKNOWN";
         break;
   }

   os << "(" << int(value) << ")";

   return os;
}




//
// Decode Predict commands enum for dlog file
//
DataLog_Stream& operator << (DataLog_Stream& os, const DoPrediction_t& value)
{
   switch (value)
   {
      case DONOR_INFO_CHANGED :
         os << "DONOR INFO CHANGED";
         break;

      case DONOR_INFO_UNCHANGED :
         os << "DONOR INFO UNCHANGED";
         break;

      case DONOR_INFO_PRECONNECT :
         os << "DONOR INFO PRECONNECT";
         break;

      case ADJUSTMENT :
         os << "ADJUSTMENT";
         break;

      case REPREDICT_ONLY :
         os << "REPREDICT_ONLY";
         break;

      case PTF_FILTER_RECALC :
         os << "PTF_FILTER_RECALC";
         break;

      case AUTO_FLOW_ADJUSTMENT :
         os << "AUTO FLOW ADJUSTMENT";
         break;


      default :
         os << "UNKNOWN";
         break;
   }

   os << "(" << int(value) << ")";

   return os;
}


DataLog_Stream& operator << (DataLog_Stream& os, const PROCEDURE_INDEX& index)
{
   return os << (index == PREV ? "prev" :
                 index == CURR ? "curr" :
                 index == NEXT ? "next" : "undefined");
}


DataLog_Stream& operator << (DataLog_Stream& os, const GuiProceduresUpdated_t& request)
{
   switch (request)
   {
      case NON_REQUEST               : return os << "NO REQUEST    "            ;
      case IS_PROCEDURE_VALID_ACK    : return os << "IS_PROCEDURE_VALID_ACK"    ;
      case SELECT_NEXT_ACK           : return os << "SELECT_NEXT_ACK"           ;
      case SELECT_PREVIOUS_ACK       : return os << "SELECT_PREVIOUS_ACK"       ;
      case SELECT_CURRENT_ACK        : return os << "SELECT_CURRENT_ACK"        ;
      case REPREDICT_ALL_ACK         : return os << "REPREDICT_ALL_ACK"         ;
      case REPREDICT_ALL_OPTIMAL_ACK : return os << "REPREDICT_ALL_OPTIMAL_ACK" ;
      case FLOW_ADJUSTMENT_ACK       : return os << "FLOW_ADUSTMENT_ACK"        ;
      case DONOR_TIME_ACK            : return os << "DONOR_TIME_ACK"            ;
   }
   return os << "UNKNOWN(" << int(request) << ")";
}


DataLog_Stream& operator << (DataLog_Stream& os, const ProcToGuiPredictStatus_t& status)
{
   switch (status)
   {
      case PROCEDURE_VALID          : os << "PROCEDURE_VALID";          break;
      case REQUEST_SELECTION_SCREEN : os << "REQUEST_SELECTION_SCREEN"; break;
      case PROCEDURES_SENT          : os << "PROCEDURES_SENT";          break;
   }

   return os << '(' << (int)status << ')';
}


ProductOptionStruct::ProductOptionStruct()
   : validProcedure                (false),
     procedure_index               (-999),
     is_collecting_platelets       (false),
     estimated_platelets           (0),
     estimated_platelet_volume     (0),
     estimated_plasma              (0),
     estimated_plasma_no_ac        (0),
     estimated_rbcs                (0),
     estimated_rbc_dose            (0),
     procedure_time                (0),
     recommended_cassette          (cassette::UNKNOWN),
     replacement_fluid_required    (false),
     count_icon_needed             (false),
     metered_platelet              (false),
     metered_rbc                   (false),
     ptf_rbc                       (false),
     ptf_split                     (false),
     estimated_qin_max             (0),
     estimated_vac                 (0),
     estimated_tbv_processed       (0),
     plateletsAmapSize             (AMAP_PLT_NONE),
     comp_pls_entire               (false),
     comp_pls_volume               (0),
     comp_pls_PAS_volume           (0),
     estimated_post_platelet_count (0)
{}

// DataLog_Stream & operator << (DataLog_Stream &os, const ProductOptionStruct& optionStruct)
// {
//
//     return os << "P" << optionStruct.procedure_index << ": plt=[yeild("
//         << optionStruct.estimated_platelets << ");vol(" << optionStruct.estimated_platelet_volume
//         << ");pas(" << optionStruct.metered_platelet << ");count(" << optionStruct.count_icon_needed
//         << ")] pls=[vol(" << optionStruct.estimated_plasma << ")] rbc=[dose("
//         << optionStruct.estimated_rbc_dose << ");vol(" << optionStruct.estimated_rbcs
//         << ");ras(" << optionStruct.metered_rbc << ");split(" << optionStruct.ptf_split
//         << "); info=[qinMax(" << optionStruct.estimated_qin_max << ");tbv("
//         << optionStruct.estimated_tbv_processed << ");ac(" << optionStruct.estimated_vac
//         << ");time(" << optionStruct.procedure_time << ");kit(" << optionStruct.recommended_cassette
//         << ");NaCL(" << optionStruct.replacement_fluid_required << ");valid("
//         << optionStruct.validProcedure << ")]";
//
//
//
// //     static char logstring[200];
// //
// //     sprintf(logstring,"%.1f %.1f %.1f repl=%d count=%d",
// //             prod.estimated_platelets,
// //             prod.estimated_plasma,
// //             prod.estimated_rbcs,
// //             prod.replacement_fluid_required,
// //             prod.count_icon_needed);
// //
// //     return os << logstring;
// }
//

// DataLog_Stream & operator << (DataLog_Stream &os, const ProcedureInformationStruct& prod)
// {
//    os << "prev: " << prod.previous_option << endmsg;
//    os << "curr: " << prod.current_option << endmsg;
//    os << "next: " << prod.next_option << endmsg;
//    return os;
// }

// DataLog_Stream & operator << (DataLog_Stream &os, ProcedureInformationStruct& procInfoStruct)
// {
//     return os << PREV << " " << procInfoStruct[PREV] << endmsg
//         << CURR << " " << procInfoStruct[CURR] << endmsg
//         << NEXT << " " << procInfoStruct[NEXT] << endmsg;
// }
//


// DataLog_Stream & operator << (DataLog_Stream &os, const CreateProductStruct& prod)
// {
//     return os
//         << "[prodNum="      << prod.product_number
//         << "; pltYield="    << prod.yield
//         << "; pltVol="      << prod.platelet_volume
//         << "; plasVol="     << prod.plasma_volume
//         << "; rbcDose="     << prod.rbc_dose
//         << "; rbcCrit="     << prod.rbc_crit
//         << "; maxTime="     << prod.max_procedure_time
//         << "; bloodType="   << prod.blood_type
//         << "]"              << endmsg;
// }

DataLog_Stream& operator << (DataLog_Stream& os, const ModifyProduct_t& prod)
{
   switch (prod)
   {
      case SET_PRODUCT_STATUS :    os << "SET PRODUCT STATUS";     break;
      case REOPTIMIZE_PRODUCT :    os << "REOPTIMIZE PRODUCT";     break;
      case SET_PRODUCT_AS_CURRENT : os << "SET PRODUCT AS CURRENT"; break;
      case DELETE_ALL_PRODUCTS :   os << "DELETE ALL PRODUCTS";    break;
   }
   return os << "(" << (int)prod << ")";
}

DataLog_Stream& operator << (DataLog_Stream& os, const ModifyProductStruct& prod)
{
   return os << "prod " << prod.product_number << ": " << prod.action << " (aux=" << prod.aux_data << ")";
}

DataLog_Stream& operator << (DataLog_Stream& os, const AMAPPLTTYPES& type)
{
   switch (type)
   {
      case AMAP_PLT_NONE   :  os << "NONE";   break;
      case AMAP_PLT_SINGLE :  os << "SINGLE"; break;
      case AMAP_PLT_DOUBLE :  os << "DOUBLE"; break;
      case AMAP_PLT_TRIPLE :  os << "TRIPLE"; break;
      case AMAP_PLT_PAS_SINGLE :  os << "PAS SINGLE"; break;
      case AMAP_PLT_PAS_DOUBLE :  os << "PAS DOUBLE"; break;
      case AMAP_PLT_PAS_TRIPLE :  os << "PAS TRIPLE"; break;
      default              :  os << "UNKNOWN"; break;
   }

   return os << "(" << (int)type << ")";
}

DataLog_Stream& operator << (DataLog_Stream& os, const ProcedureDescription& prod)
{
   if (prod.amap_platelet_size > AMAP_PLT_NONE)
   {
      os << "[plt=(amapSize:" << prod.amap_platelet_size
         << "; minYield:" << prod.amap_platelet_min_yield
         << "; maxYield:" << prod.amap_platelet_max_yield
         << "; conc:" << prod.amap_platelet_conc
         << "); ";
   }
   else os << "[plt=(yield:" << prod.yield << "; vol=" << prod.platelet_volume << "); ";

   return os << "[plas=(vol:" << prod.plasma_volume << "); rbc=(dose:" << prod.rbc_dose
             << "; crit:" << prod.rbc_crit << "); maxTime:" << prod.max_procedure_time
             << "; bloodType:" << prod.blood_type << "; designator:" << prod.designator << "]";
}

/* FORMAT HASH 1f5ce3b35829b69efd07e7436750b32a */
