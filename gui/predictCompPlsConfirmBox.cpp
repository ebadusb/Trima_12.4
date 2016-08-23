#include <vxworks.h>

/** include files **/
#include "guiglobs.hpp"
#include "predictCompPlsConfirmBox.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_PREDICT_COMP_PLS_CONFIRM_BOX::Screen_PREDICT_COMP_PLS_CONFIRM_BOX()
   : ConfirmBoxBase(),

     _txtConfirmMsg (TEXT_COMP_PLS_OOS_WARNING_BODY),
     _txtDataLbl    (TEXT_COMP_PLS_OOS_WARNING_TITLE),

     _dataCompPlsToVol(textDataLabelPredConfPltVol,
                       textEmptyFormatPredConfirmPltVolData,
                       textDataUnitsPredConfPltVol),

     _dataCompPlsFromVol(textDataLabelPredConfPltVol,
                         textEmptyFormatPredConfirmPltVolData,
                         textDataUnitsPredConfPltVol)



{
   // fix jph:
   set_screen_id(GUI_SCREEN_PREDICT_COMP_PLS_CONFIRM_BOX);

   _dataCompPlsToVol.setDashesForZeros();
   _dataCompPlsFromVol.setDashesForZeros();

   // Set up all the dataTxtId in the parent class.
   setConfirmBoxData(KEY_INACTIVE,
                     BITMAP_PREDICT_CONFIRM_BOX,
                     168, 105,
                     BITMAP_CONFIG_SUB_UP,
                     BITMAP_CONFIG_SUB_DOWN,
                     20, 23, 30);


}  // END of Screen_PREDICT_CONFIRM_BOX CONSTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_PREDICT_COMP_PLS_CONFIRM_BOX::~Screen_PREDICT_COMP_PLS_CONFIRM_BOX()
{
   Screen_PREDICT_COMP_PLS_CONFIRM_BOX::deallocate_resources ();

}  // END of Screen_PREDICT_CONFIRM_BOX DESTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_PREDICT_COMP_PLS_CONFIRM_BOX::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;

   ConfirmBoxBase::allocate_resources (allocation_parameter);

   _txtConfirmMsg.allocate_resources(*this);
   _txtDataLbl.allocate_resources(*this);

//     ProductOptionStruct curr = guiglobs::proc_status.get_procedure_information().current_option;
//     float adjustedRbcVol = curr.estimated_rbcs;
//
//     if (curr.metered_rbc && (curr.estimated_rbc_dose > 0))
//     {
//         // We're filtering, so subtract a filter loss
//         adjustedRbcVol -= guiglobs::PtfFilterLoss;
//         // If there are two filters, subtract a second filter loss.
//         if (curr.ptf_split) adjustedRbcVol -= guiglobs::PtfFilterLoss;
//         // Make sure we didn't go negative, since we're making these subtractions.
//         if (adjustedRbcVol < 0.0f) adjustedRbcVol = 0.0f;
//     }
//
//     _dataPltVol.setData         (curr.estimated_platelet_volume );
//     _dataPltYield.setData       (curr.estimated_platelets, 1    );
//     _dataRbcVol.setData         (adjustedRbcVol                 );
//     _dataPlasVol.setData        (curr.estimated_plasma          );
//     _dataFlowRate.setData       (curr.estimated_qin_max         );
//     _dataAcRequired.setData     (curr.estimated_vac             );
//     _dataTime.setData           (curr.procedure_time, 0         );
//     _dataTbvProcessed.setData   (curr.estimated_tbv_processed   );
//
//     _dataPltVol.allocate_resources(*this);
//     _dataPltYield.allocate_resources(*this);
//     _dataRbcVol.allocate_resources(*this);
//     _dataPlasVol.allocate_resources(*this);
//     _dataFlowRate.allocate_resources(*this);
//     _dataAcRequired.allocate_resources(*this);
//     _dataTime.allocate_resources(*this);
//     _dataTbvProcessed.allocate_resources(*this);
}  // END of Screen_PREDICT_CONFIRM_BOX ALLOCATE_RESOURCES


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_PREDICT_COMP_PLS_CONFIRM_BOX::deallocate_resources ()
{
   if (!_allocated) return;

   _txtConfirmMsg.deallocate_resources();
   _txtDataLbl.deallocate_resources();
   _dataCompPlsToVol.deallocate_resources();
   _dataCompPlsFromVol.deallocate_resources();

   ConfirmBoxBase::deallocate_resources ();
}   // END of Screen_PREDICT_CONFIRM_BOX DEALLOCATE_RESOURCES

/* FORMAT HASH baebf3b1a8c5f1b9272f69f8927b1948 */
