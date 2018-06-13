/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_PREDICT_CONFIRM_BOX

*     File name:   predictConfirmBox.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$
*/

#include <vxworks.h>

/** include files **/
#include "guiglobs.hpp"
#include "predictConfirmBox.hpp"
#include "cobeconfig.h"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_PREDICT_CONFIRM_BOX::Screen_PREDICT_CONFIRM_BOX()
   : ConfirmBoxBase(),

     _txtConfirmMsg(textMiscPredConfMsg),

     _txtDataLbl(textMiscPredConfDataLabel),

     _dataPltVol(textDataLabelPredConfPltVol,
                 textEmptyFormatPredConfirmPltVolData,
                 textDataUnitsPredConfPltVol),

     _dataPltYield(textDataLabelPredConfPltYield,
                   textEmptyFormatPredConfirmPltYieldData,
                   textDataUnitsPredConfPltYieldX10,
                   textDataUnitsPredConfPltYieldExp),

     _dataRbcVol(textDataLabelPredConfRbcVol,
                 textEmptyFormatPredConfirmRbcVolData,
                 textDataUnitsPredConfRbcVol),

     _dataPlasVol(textDataLabelPredConfPlasVol,
                  textEmptyFormatPredConfirmPlasVolData,
                  textDataUnitsPredConfPlasVol),

     _dataAcRequired(textDataLabelPredConfAcRequired,
                     textEmptyFormatPredConfirmAcData,
                     textDataUnitsPredConfAcRequired),

     _dataTime(textDataLabelPredConfDuration,
               textEmptyFormatPredConfirmTimeData,
               textDataUnitsPredConfDuration),

     _dataTbvProcessed(textDataLabelPredConfTbvProcessed,
                       textEmptyFormatPredConfirmTbvProcessedData,
                       textDataUnitsPredConfTbvProcessed),

     _dataPostPlt(textDataLabelPredConfPostPlt,
                  textEmptyFormatPredConfPostPlt,
                  textDataUnitsPredConfPostPlt)
{
   Screen_PREDICT_CONFIRM_BOX::set_screen_id(GUI_SCREEN_PREDICT_CONFIRM_BOX);

   _dataPltVol.setDashesForZeros();
   _dataPltYield.setDashesForZeros();
   _dataRbcVol.setDashesForZeros();
   _dataPlasVol.setDashesForZeros();
   _dataPostPlt.setDashesForZeros(); // in case default precount is used

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
Screen_PREDICT_CONFIRM_BOX::~Screen_PREDICT_CONFIRM_BOX()
{
   Screen_PREDICT_CONFIRM_BOX::deallocate_resources ();

}  // END of Screen_PREDICT_CONFIRM_BOX DESTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_PREDICT_CONFIRM_BOX::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;

   ConfirmBoxBase::allocate_resources (allocation_parameter);

   _txtConfirmMsg.allocate_resources(*this);
   _txtDataLbl.allocate_resources(*this);

   // ProductOptionStruct curr = guiglobs::proc_status.get_procedure_information()[CURR];
   ProductOptionStruct curr =  guiglobs::cdsData.procedures.getProcedure(CURR);


   float adjustedRbcVol = curr.estimated_rbcs;

   if (curr.metered_rbc && (curr.estimated_rbc_dose > 0))
   {
      // We're filtering, so subtract a filter loss
      adjustedRbcVol -= CobeConfig::data().mssPtfFilterLoss;

      // If there are two filters, subtract a second filter loss.
      if (curr.ptf_split)
         adjustedRbcVol -= CobeConfig::data().mssPtfFilterLoss;

      // Make sure we didn't go negative, since we're making these subtractions.
      if (adjustedRbcVol < 0.0f)
         adjustedRbcVol = 0.0f;
   }

   _dataPltVol.setData         (curr.estimated_platelet_volume);
   _dataPltYield.setData       (curr.estimated_platelets, 1);
   _dataRbcVol.setData         (adjustedRbcVol);
   _dataPlasVol.setData        (curr.estimated_plasma);
   _dataAcRequired.setData     (curr.estimated_vac);
   _dataTime.setData           (curr.procedure_time, 0);
   _dataTbvProcessed.setData   (curr.estimated_tbv_processed);
   if (guiglobs::donor_platelet_precount_entered)
   {
      _dataPostPlt.setData        (curr.estimated_post_platelet_count);
   }
   else
   {
      _dataPostPlt.setData        (0); // using default precount, display dashes
   }

   _dataPltVol.allocate_resources(*this);
   _dataPltYield.allocate_resources(*this);
   _dataRbcVol.allocate_resources(*this);
   _dataPlasVol.allocate_resources(*this);
   _dataAcRequired.allocate_resources(*this);
   _dataTime.allocate_resources(*this);
   _dataTbvProcessed.allocate_resources(*this);
   _dataPostPlt.allocate_resources(*this);
}  // END of Screen_PREDICT_CONFIRM_BOX ALLOCATE_RESOURCES


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_PREDICT_CONFIRM_BOX::deallocate_resources ()
{
   if (!_allocated) return;

   _txtConfirmMsg.deallocate_resources();
   _txtDataLbl.deallocate_resources();

   _dataPltVol.deallocate_resources();
   _dataPltYield.deallocate_resources();
   _dataRbcVol.deallocate_resources();
   _dataPlasVol.deallocate_resources();
   _dataAcRequired.deallocate_resources();
   _dataTime.deallocate_resources();
   _dataTbvProcessed.deallocate_resources();
   _dataPostPlt.deallocate_resources();

   ConfirmBoxBase::deallocate_resources ();
}   // END of Screen_PREDICT_CONFIRM_BOX DEALLOCATE_RESOURCES

/* FORMAT HASH 056c472ab6122974c74474c303ac329d */
