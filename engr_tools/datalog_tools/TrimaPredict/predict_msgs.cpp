/*
* AG
* This is a temporary solution for predictionator
* in windows. 
* TODO: add to this project the predict_msgs.cpp in 
* trima_common_reentrant
*/

#include "predict_msgs.h"

ProductOptionStruct::ProductOptionStruct()
   : validProcedure                (false),
     procedure_index               (0),
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
