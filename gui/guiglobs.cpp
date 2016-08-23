/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 */

#include <vxworks.h>
#include "guiglobs.hpp"
#include "dirent.h"
#include "statline.hpp"
#include "statBarNoSolution.hpp"
#include "statBarWithSolution.hpp"
#include "trima_string_info.h"
#include "scrncomm.hpp"
#include "trima_variable_database.h"
#include "software_cds.h"


GuiCdsData::GuiCdsData()
   : config      (ROLE_RO),
     run         (ROLE_RO),
     run_RW      (ROLE_RW),
     targets     (ROLE_RO),
     volumes     (ROLE_RO),
     set         (ROLE_RO),
     time        (ROLE_RO),
     xif         (ROLE_RO),
     procedures  (ROLE_RO),
     pqi_cds     (ROLE_RO),
     adjustments (ROLE_RO)
{ }

GuiMain*              guiglobs::guiMain               = NULL;
StatBarBase*          guiglobs::apheresis_status_bar  = NULL;
Status_Line*          guiglobs::apheresis_status_line = NULL;
Current_Screen_Status guiglobs::screen_status ;
Audio_Feedback        guiglobs::button_audio_feedback ;
Alarm_Handler         guiglobs::gui_alarm_manager ;
Donor_Data_Repository guiglobs::gui_donor_data_repository ;
GuiCdsData            guiglobs::cdsData;
MessageSystem         guiglobs::message_system;
CCfgVarList           guiglobs::config_var_list;
Barcode_Repository*   guiglobs::barcode_repo                    = NULL;
ProcedureStatus       guiglobs::proc_status;
CassetteMgr*          guiglobs::cassette_mgr                    = NULL;
int                   guiglobs::display_dont_connect_donor_icon = 0;
Bitmap                guiglobs::dont_connect_icon(450, 417, BITMAP_DONT_CONNECT_DONOR);
int                   guiglobs::display_vista_connected_icon    = 0;
Bitmap                guiglobs::vista_connected_icon(570, 423, BITMAP_VISTA_CONNECTED);
CFR_LIST              guiglobs::cfr_list;
AasRunInfo            guiglobs::aasRunInfo;
Screen_Commutator*    guiglobs::scrnComm = NULL;
float                 guiglobs::concWbcFlagThreshold                = 0.0;
float                 guiglobs::PtfFilterLoss                       = 0.0;
float                 guiglobs::rasBagEmptyPercentage               = 0.05;
float                 guiglobs::pasBagEmptyPercentage               = 0.05;
float                 guiglobs::ptfFilterVol                        = 0.0;
float                 guiglobs::minPlsRinseVol                      = 0.0;
float                 guiglobs::pasPrimeVol                         = 0.0;
float                 guiglobs::pasVolBiasPercent                   = 0.0f;
float                 guiglobs::pasNoRinsebackPenalty               = 0.0f;
float                 guiglobs::flowAdjMin                          = 20.0;
float                 guiglobs::flowAdjMax                          = 142.0;

unsigned char         guiglobs::stack_screens_to_process            = 0;
ALARM_VALUES          guiglobs::current_displayed_alarm             = NULL_ALARM;

bool                  guiglobs::run_summary_display_active          = false;
bool                  guiglobs::donor_disconnect_confirmed          = false;
bool                  guiglobs::procedure_selected                  = false;
bool                  guiglobs::timeOnlyConfigMode                  = false;
bool                  guiglobs::currentlyInPreConnectDonorDataCheck = false;
bool                  guiglobs::donor_platelet_precount_entered     = false;


BioSignaturesMgr& guiglobs::bioMgr () {static BioSignaturesMgr b; return b; }

float guiglobs::donor_tbv ()
{
   SDonorVitals gui_donor_vitals_struct;
   gui_donor_data_repository.get_donor_vitals_struct (&gui_donor_vitals_struct);

   const bool display_ogawa_tbv = Software_CDS::GetInstance().getFeature(JapanFeatures);


   if (!display_ogawa_tbv)
   {
      return gui_donor_vitals_struct.fSafetyDonorTBV;
   }


   float height = 0;
   float weight = 0;

   //
   //  Formula is calculated in meters not centimeters.
   //
   switch (cdsData.config.LangUnit.Get().key_height)
   {
      case FT_HEIGHT :
         height = ((gui_donor_vitals_struct.fDonorHeight * CM_PER_IN) / 100);
         break;

      case CENTI_HEIGHT :
         height = (gui_donor_vitals_struct.fDonorHeight / 100);
         break;

      default :
         DataLog (log_level_gui_error) << "Unrecognized height setting." << endmsg;
         break;
   }

   switch (cdsData.config.LangUnit.Get().key_weight)
   {
      case LB_WEIGHT :
         weight = (gui_donor_vitals_struct.fDonorWeight / LB_PER_KG);
         break;

      case KG_WEIGHT :
         weight = gui_donor_vitals_struct.fDonorWeight;
         break;

      default :
         DataLog (log_level_gui_error) << "Unrecognized weight setting." << endmsg;
         break;

   }

   //
   //  Formula is calculated in kilograms.
   //
   //

   //  float  ogawaTbvLocal = 0.0f;
   if (gui_donor_vitals_struct.cDonorSex == DONOR_SEX_IS_FEMALE)
   {
      // keep equation from dumping a partial solution.. very bad
      if (height * weight > 0.0f)
      {
         gui_donor_vitals_struct.fProcDonorTBVOgawa = 250 * height * height * height + 63 * weight - 662;
      }
   }
   else
   {

      // keep equation from dumping a partial solution.. very bad
      if (height * weight > 0.0f)
      {
         gui_donor_vitals_struct.fProcDonorTBVOgawa = 168 * height * height * height + 50 * weight + 444;
      }

   }
   //  return ogawaTbvLocal;
   DataLog (log_level_gui_info) << "Displaying Ogawa TBV of " << gui_donor_vitals_struct.fProcDonorTBVOgawa << endmsg;
   return gui_donor_vitals_struct.fProcDonorTBVOgawa;

}


// This function is used to determine whether or not the clamp pas line
// screen should be displayed.  It should not be called before a
// cassette has been selected.
bool guiglobs::shouldShowClampPas ()
{
#if CPU==SIMNT
   // This is the same stuff as below, but allows for no cassette having been selected yet.
   bool cassetteIsPas  = (!cassette_mgr->HasCurrentCassetteSet()) || cassette_mgr->currentSetIsPas();
   bool splitNotifOn   = cdsData.config.Procedure.Get().key_plt_mss_split_notif;
   bool masterPasOn    = cdsData.config.Procedure.Get().key_mss_plt_on;
   bool procedurePasOn = (!procedure_selected) || cdsData.procedures.getProcedure(CURR).metered_platelet;

   return cassetteIsPas && splitNotifOn && masterPasOn && procedurePasOn;

#else

   // Show the clamp platelet bag screen iff this is a metered set and the
   // split notification option is turned on in config.
   return
      (
      // Cassette is MSS PLT
      cassette_mgr->currentSetIsPas()
      // PLT Split notification is on.
      && cdsData.config.Procedure.Get().key_plt_mss_split_notif
      // and the master PAS switch is on (IT 7786)
      && cdsData.config.Procedure.Get().key_mss_plt_on
      // and either no procedure is selected or the selected procedure calls for PAS
      && (!procedure_selected || cdsData.procedures.getProcedure(CURR).metered_platelet)
      )
      ||
      (
      // The Japanese show this screen whether or not PAS is involved.
      Software_CDS::isJapaneseWorkflow()
      && cassette_mgr->currentSetIsWhiteStamp()
      );
#endif // CPU==SIMNT
}

bool guiglobs::shouldShowClampPlasmaAirBag ()
{
   // Today, only 1 set (80537) is a plasma-only set with an airbag,
   // and it's only used in Japanese region. Future sets like this
   // may require querying a cassette function bit instead of a
   // regionalization feature.
   return ( Software_CDS::isJapaneseWorkflow() &&
            cassette_mgr->currentSetIsPlasmaOnly() );
}

// This function is used to determine whether or not the clamp pas line
// screen should be displayed.  It should not be called before a
// cassette has been selected.
bool guiglobs::shouldShowRbcSplitInit ()
{
#if CPU==SIMNT
   return true;
#else
   // Show the clamp platelet bag screen iff this is a metered set and the
   // split notification option is turned on in config.
   return
      (
      // The DRBC alert is ON.
      cdsData.config.Procedure.Get().key_drbc_alert &&

      // The set is DRBC.
      cassette_mgr->currentSetIsDrbc() &&

      // The set is NOT RAS. (For RAS sets, we already
      // prompted the split on the PTF setup screen.
      (!cassette_mgr->currentSetIsRas())
      );
#endif // CPU==SIMNT
}



bool guiglobs::isDRBC ()
{
   return ( cdsData.config.Procedure.Get().key_drbc_alert &&
            cassette_mgr->currentSetIsDrbc() );
}


// This function is used to determine whether or not the clamp pas line
// screen should be displayed.  It should not be called before a
// cassette has been selected.
bool guiglobs::shouldShowClampCrossover ()
{
   // Show Clamp Crossover if the cassette is MSS RBC.
   return cassette_mgr->currentSetIsRas();
}



//
// Scan memory for specified character pattern.  Used to find ID strings
// in BIOS ROM and expansion ROM memory areas.
//
const char* guiglobs::findIdString (const char* memPtr,        // start of memory block
                                    unsigned int memLength,    // length of memory block (in bytes)
                                    const char* pattern,       // pattern to search for
                                    unsigned int& memStart,    // starting byte within memory block for search
                                    unsigned int& stringLength // length of found string
                                    )
{
   enum { MaxIDStringLength = 256 };

   int         patternLength = strlen(pattern);
   const char* resultString  = NULL;

   while ( memStart < memLength - patternLength &&
           !resultString )
   {
      if ( memPtr[memStart] == pattern[0] &&
           memcmp(&memPtr[memStart], pattern, patternLength) == 0 )
      {
         resultString = &memPtr[memStart];
      }
      else
      {
         memStart += 1;
      }
   }

   if ( resultString )
   {
      //
      // Found the specified pattern.  Extend the string to get the printable
      // characters surrounding it.
      //
      unsigned int stringStart = memStart;
      while ( stringStart > 0 &&
              isprint(memPtr[stringStart - 1]) &&
              (memPtr[stringStart - 1] & 0x80) == 0 )
      {
         stringStart -= 1;
      }

      stringLength = patternLength + (memStart - stringStart);
      while ( stringStart + stringLength < memLength &&
              stringLength < MaxIDStringLength &&
              isprint(memPtr[stringStart + stringLength]) &&
              (memPtr[stringStart + stringLength] & 0x80) == 0 )
      {
         stringLength += 1;
      }

      resultString = &memPtr[stringStart];
      memStart     = stringStart + stringLength;

      if ( stringStart + stringLength < memLength &&
           resultString[stringLength] != '\0' &&
           resultString[stringLength] != '\r' &&
           resultString[stringLength] != '\n' )
      {
         //
         // Reject strings that do not end at the end of the ROM, or do not
         // end with \0, \n, or \r.
         //
         resultString = NULL;
      }
      else
      {
         //
         // Strip trailing spaces from string
         //
         while ( resultString[stringLength] == ' ' && stringLength > 0 )
         {
            stringLength -= 1;
         }
      }
   }

   return resultString;
}



bool guiglobs::amapPlatelets ()
{
   return Software_CDS::GetInstance().getFeature(AmapPlateletsAvailable);
}



bool guiglobs::complementaryPlasmaEnabled ()
{
   return ( (bool)cdsData.config.Predict.Get().key_complementary_plasma
            && Software_CDS::GetInstance().getFeature(ComplementaryPlasmaAvailable) );
}


bool guiglobs::airToDonorMitigationEnabled ()
{
   return Software_CDS::GetInstance().getFeature(AiroutMitigation);
}



bool guiglobs::isSalineRinseback ()
{
   return screen_status.get_current_system_state() == BLOOD_RINSEBACK &&
          (cdsData.config.Procedure.Get().key_saline_rinseback &&
           (!cassette_mgr->currentSetIsWhiteStamp()));
}

bool guiglobs::isPlasmaRinseback ()
{
   return screen_status.get_current_system_state() == BLOOD_RINSEBACK &&
          (cdsData.config.Procedure.Get().key_plasma_rinseback && // Plasma rinseback configured
           cassette_mgr->currentSetIsWhiteStamp());               // Only on platelet collection sets
}

//
// Are we currently in either saline or plasma rinseback?
//
bool guiglobs::isSpecialRinseback ()
{
   return isSalineRinseback() || isPlasmaRinseback();
}




bool guiglobs::setIntDataItem (const char* name, int newValue)
{
   TrimaDataItemInteger* item = (TrimaDataItemInteger*)TrimaVariableDatabase::getDataItem(name);

   if ( item )
   {
      item->setValue(newValue);
      return true;
   }

   DataLog(log_level_gui_error) << "Data item " << name << " is NULL" << endmsg;
   return false;
}



bool guiglobs::setTextDataItem (const char* name, const char* newValue)
{
   TrimaDataItemText* item = (TrimaDataItemText*)TrimaVariableDatabase::getDataItem(name);

   if ( item )
   {
      item->setValue(newValue);
      return true;
   }

   DataLog(log_level_gui_error) << "Data item " << name << " is NULL" << endmsg;
   return false;
}




#if 0
bool guiglobs::setTextItemDataItem (const char* name, char* newValue)
{
   TrimaDataItemTextItem* item = (TrimaDataItemTextItem*)TrimaVariableDatabase::getDataItem(name);

   if ( item )
   {
      item->setValue(newValue);
      return true;
   }

   DataLog(log_level_gui_error) << "Data item " << name << " is NULL" << endmsg;
   return false;
}



bool guiglobs::setClockDataItem (const char* name, int newValue)
{
   TrimaDataItemClock* item = (TrimaDataItemClock*)TrimaVariableDatabase::getDataItem(name);

   if ( item )
   {
      item->setValue(newValue);
      return true;
   }

   DataLog(log_level_gui_error) << "Data item " << name << " is NULL" << endmsg;
   return false;
}



bool guiglobs::setDoubleDataItem (const char* name, double newValue)
{
   TrimaDataItemDouble* item = (TrimaDataItemDouble*)TrimaVariableDatabase::getDataItem(name);

   if ( item )
   {
      item->setValue(newValue);
      return true;
   }

   DataLog(log_level_gui_error) << "Data item " << name << " is NULL" << endmsg;
   return false;
}



bool guiglobs::setTextItemDataItem (const char* name, TrimaTextItem* newValue)
{
   TrimaDataItemTextItem* item = (TrimaDataItemTextItem*)TrimaVariableDatabase::getDataItem(name);

   if ( item )
   {
      item->setValue(newValue);
      return true;
   }

   DataLog(log_level_gui_error) << "Data item " << name << " is NULL" << endmsg;
   return false;
}
#endif



void guiglobs::setStatBar (bool withAas)
{
   STATUS_BAR_TYPE desiredType = STATUS_BAR_TYPE_NO_ADDITION;
   if (withAas) desiredType = STATUS_BAR_TYPE_WITH_ADDITION;

   if (apheresis_status_bar)
   {
      if (apheresis_status_bar->getType() == desiredType) return;
      else
      {
         delete apheresis_status_bar;
         apheresis_status_bar = NULL;
      }
   }

   DataLog (log_level_gui_info) << "setStatBar(withAas=" << withAas
                                << "): creating new status bar." << endmsg;

   switch (desiredType)
   {
      case STATUS_BAR_TYPE_NO_ADDITION :
         apheresis_status_bar = new StatBarNoSolution;
         break;

      case STATUS_BAR_TYPE_WITH_ADDITION :
         apheresis_status_bar = new StatBarWithSolution;
         break;

      default :
         DataLog(log_level_gui_error) << "Status bar deletion failed." << endmsg;
         return;
   }

#if CPU==SIMNT
   if (scrnComm && screen_status.get_current_system_state() >= STARTUP_TESTS)
   {
      Base_Apheresis_Window* currScrnPtr = (Base_Apheresis_Window*)scrnComm->getScreenInstancePointer();

      if (currScrnPtr) currScrnPtr->refresh_resources();
      else DataLog(log_level_gui_error) << "Unable to get current screen pointer." << endmsg;
   }
   else DataLog(log_level_gui_error) << "Not at startup test yet." << endmsg;
#endif // #if CPU==SIMNT

   apheresis_status_bar->allocate_resources();
}

void guiglobs::initStatBar ()
{
   setStatBar(cdsData.config.Procedure.Get().key_mss_plt_on ||
              cdsData.config.Procedure.Get().key_mss_rbc_on);
}

#if CPU==SIMNT
void guiglobs::swapStatBar ()
{
   setStatBar((apheresis_status_bar->getType() == STATUS_BAR_TYPE_NO_ADDITION) ?
              STATUS_BAR_TYPE_WITH_ADDITION : STATUS_BAR_TYPE_NO_ADDITION);
}
#endif



bool guiglobs::showCompPlsSymbol ()
{
   return guiglobs::cdsData.run.ShowCompPlsSymbol.Get();
}


void guiglobs::showCompPlsSymbol (bool val)
{

   guiglobs::cdsData.run_RW.ShowCompPlsSymbol.Set(val);

}

// bool guiglobs::isCompatible(const ProductOptionStruct& product, const CatalogNumber *catalogNumber)
// {
//    cassette cs;
//
//    double pltTarg = product.estimated_platelets;
//    double plsTarg = product.estimated_plasma;
//    double rbcTarg = product.estimated_rbc_dose - 0.5f;
//
//    bool targIsPas = product.metered_platelet;
//    bool targIsRas = product.metered_rbc;
//    bool targIsRepFluid = product.replacement_fluid_required;
//
//    bool targIsDrbc = guiglobs::cdsData.config.isPromptedDrbcSplit(product.procedure_index);
//
//    bool targIsPlt = pltTarg > 0.0f;
//    bool targIsPls = plsTarg > 0.0f;
//    bool targIsRbc = rbcTarg > 0.0f;
//
//    /////////////////////////////////////////////////////////////////////
//    // Test the physical volume capabilities of the set.
//    /////////////////////////////////////////////////////////////////////
//
//    if ( (catalogNumber->isWhiteStamp()) &&
//         (!cs.compatible( cassette::PLTPLSRBC, pltTarg, plsTarg, rbcTarg))
//       )
//    { // CASSETTE IS WHITE, PROCEDURE NEEDS BLACK.
//       DataLog (log_level_gui_info) << "Cassette " << catalogNumber->RefNum() << " disqualified. Procedure incompatible with PLT-PLS-RBC set." << endmsg;
//       return false;
//    }
//
//    if ( (!catalogNumber->isWhiteStamp()) &&
//         (!cs.compatible( cassette::RBCPLS, pltTarg, plsTarg, rbcTarg))
//       )
//    { // CASSETTE IS BLACK, PROCEDURE NEEDS WHITE.
//       DataLog (log_level_gui_info) << "Cassette " << catalogNumber->RefNum() << " disqualified. Procedure incompatible with RBC-PLS set." << endmsg;
//       return false;
//    }
//
//    /////////////////////////////////////////////////////////////////////
//    // Test the AAS capabilities of the set.
//    /////////////////////////////////////////////////////////////////////
//    if (targIsDrbc && (!catalogNumber->isDrbc()))
//    {
//       DataLog (log_level_gui_info) << "Cassette " << catalogNumber->RefNum() << " disqualified. Procedure requires DRBC capability." << endmsg;
//       return false;
//    }
//
//    if (targIsPas && (!catalogNumber->isPas()))
//    {
//       DataLog (log_level_gui_info) << "Cassette " << catalogNumber->RefNum() << " disqualified. Procedure requires PAS capability." << endmsg;
//       return false;
//    }
//
//    if (targIsRas && (!catalogNumber->isRas()))
//    {
//       DataLog (log_level_gui_info) << "Cassette " << catalogNumber->RefNum() << " disqualified. Procedure requires RAS/PTF cassette." << endmsg;
//       return false;
//    }
//
//    if ((rbcTarg > 0) && (!targIsRas) && catalogNumber->isRas())
//    {
//       DataLog (log_level_gui_info) << "Cassette " << catalogNumber->RefNum()
//       << " disqualified. Procedure requires non-RAS/PTF cassette." << "." << endmsg;
//
//       return false;
//    }
//
//    if (targIsPlt && (!catalogNumber->isPlt()))
//    {
//       DataLog (log_level_gui_info) << "Cassette " << catalogNumber->RefNum() << " disqualified. Procedure requires a set with platelet bags." << endmsg;
//       return false;
//    }
//
//    if (targIsPls && (!catalogNumber->isPls()))
//    {
//       DataLog (log_level_gui_info) << "Cassette " << catalogNumber->RefNum() << " disqualified. Procedure requires a set with a plasma bag." << endmsg;
//       return false;
//    }
//
//    if (targIsRbc && (!catalogNumber->isRbc()))
//    {
//       DataLog (log_level_gui_info) << "Cassette " << catalogNumber->RefNum() << " disqualified. Procedure requires a set with an RBC bag." << endmsg;
//       return false;
//    }
//
//    /////////////////////////////////////////////////////////////////////
//    // Test Replacement Fluid
//    /////////////////////////////////////////////////////////////////////
//
//    if (targIsRepFluid && (catalogNumber->isWhiteStamp()))
//    {
//       DataLog (log_level_gui_info) << "Cassette " << catalogNumber->RefNum() << " disqualified. Procedure requires replacement fluid; cassette is white stamp." << endmsg;
//       return false;
//    }
//
//    /////////////////////////////////////////////////////////////////////
//    // Test DRBC Special Case (Don't allow white stamp sets non-PLT procedures). ---> See IT 8066 for more info.
//    /////////////////////////////////////////////////////////////////////
//
//    if ((catalogNumber->isWhiteStamp()) &&
//        ((pltTarg == 0.0f) &&
//         ((plsTarg > 0.0f) ||
//          (rbcTarg > 0.0f))))
//    {
//       DataLog (log_level_gui_info) << "Cassette " << catalogNumber->RefNum() << " disqualified. Procedure is non-PLT, and cassette is white stamp." << endmsg;
//       return false;
//    }
//
//    /////////////////////////////////////////////////////////////////////
//
//    // If we haven't returned false by now, all tests have passed.
//    return true;
// }
//
//

/* FORMAT HASH 6e745a0ec71f14872189ee2fc3f0576d */
