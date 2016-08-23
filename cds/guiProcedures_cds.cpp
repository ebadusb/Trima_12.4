/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  GuiProcedures_CDS

*     File name:   guiProcedures_cds.cpp

*     Contents:    Implementations of class methods

*     Description: Common data structure for procedures sent between predict and proc.

*************************< FOCUSSED GUI DOMAIN >*****************************/

#include <vxWorks.h>
#include "guiProcedures_cds.h"
#include "cassette.h"
#include "config_cds.h"

GuiProcedures_CDS::GuiProcedures_CDS(Role role)
   : SingleWriteDataStore("GuiProcedures_CDS", role)
{
   ProcedureInfo.Register      (this, PFR_RECOVER);
}

GuiProcedures_CDS::~GuiProcedures_CDS()
{}

ProductOptionStruct GuiProcedures_CDS::getProcedure (const PROCEDURE_INDEX index)
{
   return ProcedureInfo.Get()[index];
}


bool GuiProcedures_CDS::kitIsCompatibleWithCurrentProcedure (const CatalogNumber* kit)
{
   ProductOptionStruct product = ProcedureInfo.Get()[CURR];

   /////////////////////////////////////////////////////////////////////
   // Test the physical volume capabilities of the set.
   /////////////////////////////////////////////////////////////////////
   cassette    cs;

   const char* refNumStr = kit->RefNum();

   if ((kit->isWhiteStamp())
       && (!cs.compatible(cassette::PLTPLSRBC, product.estimated_platelets, product.estimated_plasma, (product.estimated_rbc_dose - 0.5f))))
   {  // CASSETTE IS WHITE, PROCEDURE NEEDS BLACK.
      DataLog (log_level_gui_info) << "Cassette " << refNumStr << " disqualified. Procedure incompatible with PLT-PLS-RBC set." << endmsg;
      return false;
   }

   if ((!kit->isWhiteStamp())
       && (!cs.compatible(cassette::RBCPLS, product.estimated_platelets, product.estimated_plasma, (product.estimated_rbc_dose - 0.5f))))
   {  // CASSETTE IS BLACK, PROCEDURE NEEDS WHITE.
      DataLog (log_level_gui_info) << "Cassette " << refNumStr << " disqualified. Procedure incompatible with RBC-PLS set." << endmsg;
      return false;
   }

   /////////////////////////////////////////////////////////////////////
   // Test the AAS capabilities of the set.
   /////////////////////////////////////////////////////////////////////
   {
      Config_CDS configCds(ROLE_RO);

      if (configCds.isPromptedDrbcSplit(product.procedure_index) && (!kit->isDrbc()))
      {
         DataLog (log_level_gui_info) << "Cassette " << refNumStr << " disqualified. Procedure requires DRBC capability." << endmsg;
         return false;
      }
   }

   if (product.metered_platelet && (!kit->isPas()))
   {
      DataLog (log_level_gui_info) << "Cassette " << refNumStr << " disqualified. Procedure requires PAS capability." << endmsg;
      return false;
   }

   if (product.metered_rbc && (!kit->isRas()))
   {
      DataLog (log_level_gui_info) << "Cassette " << refNumStr << " disqualified. Procedure requires RAS/PTF cassette." << endmsg;
      return false;
   }

   if (((product.estimated_rbc_dose - 0.5f) > 0) && (!product.metered_rbc) && kit->isRas())
   {
      DataLog (log_level_gui_info) << "Cassette " << refNumStr
                                   << " disqualified. Procedure requires non-RAS/PTF cassette." << "." << endmsg;

      return false;
   }

   if ((product.estimated_platelets > 0.0f) && (!kit->isPlt()))
   {
      DataLog (log_level_gui_info) << "Cassette " << refNumStr << " disqualified. Procedure requires a set with platelet bags." << endmsg;
      return false;
   }

   if ((product.estimated_plasma > 0.0f) && (!kit->isPls()))
   {
      DataLog (log_level_gui_info) << "Cassette " << refNumStr << " disqualified. Procedure requires a set with a plasma bag." << endmsg;
      return false;
   }

   if (((product.estimated_rbc_dose - 0.5f) > 0.0f) && (!kit->isRbc()))
   {
      DataLog (log_level_gui_info) << "Cassette " << refNumStr << " disqualified. Procedure requires a set with an RBC bag." << endmsg;
      return false;
   }

   /////////////////////////////////////////////////////////////////////
   // Test Replacement Fluid
   /////////////////////////////////////////////////////////////////////

   if (product.replacement_fluid_required && (kit->isWhiteStamp()))
   {
      DataLog (log_level_gui_info) << "Cassette " << refNumStr << " disqualified. Procedure requires replacement fluid; cassette is white stamp." << endmsg;
      return false;
   }

   /////////////////////////////////////////////////////////////////////
   // Test DRBC Special Case (Don't allow white stamp sets non-PLT procedures). ---> See IT 8066 for more info.
   /////////////////////////////////////////////////////////////////////

   if ((kit->isWhiteStamp()) &&
       ((product.estimated_platelets == 0.0f) &&
        ((product.estimated_plasma > 0.0f) ||
         ((product.estimated_rbc_dose - 0.5f) > 0.0f))))
   {
      DataLog (log_level_gui_info) << "Cassette " << refNumStr << " disqualified. Procedure is non-PLT, and cassette is white stamp." << endmsg;
      return false;
   }

   /////////////////////////////////////////////////////////////////////

   // If we haven't returned false by now, all tests have passed.
   return true;
}

bool GuiProcedures_CDS::currentProcedureIsValid ()
{
   ProductOptionStruct product = ProcedureInfo.Get()[CURR];

   return product.validProcedure &&
          ((product.estimated_plasma != 0) ||
           (product.estimated_platelets != 0) ||
           (product.estimated_rbc_dose != 0));
}

/* FORMAT HASH 7e64a133304362abded3d76d9025a015 */
