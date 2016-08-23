/*******************************************************************************
 *
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      runsumGlobs.cpp
 *             Run Summary Globals
 *
 * ABSTRACT:   This is a place for data and functions common to all the run summary screens
 *				and classes.
 *
 * HISTORY:    $Log: runsumGlobs.cpp $
 * HISTORY:    Revision 1.16  2009/06/24 20:36:58Z  dslausb
 * HISTORY:    IT 9075 - Pre-PAS flags are now on and not configureable.
 * HISTORY:    Revision 1.15  2009/02/19 23:07:21Z  dslausb
 * HISTORY:    Remove CPS check.  That's now handled on the procdata side.
 * HISTORY:    Revision 1.14  2008/10/03 16:19:40Z  dslausb
 * HISTORY:    Move away from dynamic memory where possible
 * HISTORY:    Revision 1.13  2008/10/01 16:07:36Z  dslausb
 * HISTORY:    Change CDS objects so they're not pointers
 * HISTORY:    Revision 1.12  2008/08/16 19:12:13Z  dslausb
 * HISTORY:    Mark's Screenshot Code
 * HISTORY:    Revision 1.12  2008/08/15 20:36:53Z  mereusb
 * HISTORY:    IT 8107: Change for screenshot scripts.
 * HISTORY:    Revision 1.11  2008/07/25 13:04:44  dslausb
 * HISTORY:    - Overhaul the way procedure and cassette data are seen to be more robust to PFR
 * HISTORY:    - Change UGL makefile so that it's not built so frequently
 * HISTORY:    Revision 1.10  2008/03/24 21:13:11Z  dslausb
 * HISTORY:    IT 8374 - Change the offline addition screen
 * HISTORY:    Revision 1.9  2008/02/19 17:17:28Z  dslausb
 * HISTORY:    Add reboot confirm dialog
 * HISTORY:    Revision 1.8  2008/02/18 20:20:28Z  dslausb
 * HISTORY:    IT 8456 - Changes for string.info files
 * HISTORY:    Revision 1.7  2007/12/04 18:43:15Z  dslausb
 * HISTORY:    IT 8015 - Removed pre-metering RAS flags
 * HISTORY:    Revision 1.6  2007/11/01 17:04:34Z  dslausb
 * HISTORY:    IT 8285 - Don't show PLT leukoreduction message before metering
 * HISTORY:    Revision 1.5  2007/10/19 16:28:06Z  dslausb
 * HISTORY:    - Added AAS start time to the run summary screen.
 * HISTORY:    - Add AAS icons to progress bars on runproc screen
 * HISTORY:    - Fix spurious air removal message when air removal off
 * HISTORY:    - Fix MSS disconnect screen transition
 * HISTORY:    Revision 1.4  2007/09/14 22:52:33Z  dslausb
 * HISTORY:    IT 8212 - Fixed run summary transition table
 * HISTORY:    Revision 1.3  2007/09/11 21:34:54Z  dslausb
 * HISTORY:    Update to run summary and flagging functionality
 * HISTORY:    Revision 1.2  2007/08/30 21:13:01Z  dslausb
 * HISTORY:    Fixed displaying of RBC salvage volume
 * HISTORY:    Revision 1.1  2007/08/29 21:38:54Z  dslausb
 * HISTORY:    Initial revision
 *
 *******************************************************************************/


#include <vxworks.h>
#include "runsumGlobs.hpp"
#include "guiglobs.hpp"
#include "aphwind.hpp"

PQInformation RunsumGlobs::_pqa;

bool RunsumGlobs::shouldShowAasSalvage (const RUN_SUMMARY_STRUCT& runSummary)
{
   bool correctSystemState =

    #if CPU==SIMNT  // If this is the simulator, let's say we're always in the correct system state.
      TRUE;         // This is necessary because in the simulator, we just call 'em by enumeration value.
    #else
      (guiglobs::screen_status.get_current_system_state() == METERED_STORAGE_DISCONNECT);
    #endif

   return correctSystemState && (rasHasSalvageVol(runSummary) || pasHasSalvageVol(runSummary));
}


bool RunsumGlobs::rasHasSalvageVol (const RUN_SUMMARY_STRUCT& runSummary)
{
   return rasHasSalvageVolProd1(runSummary) || rasHasSalvageVolProd2(runSummary);
}


bool RunsumGlobs::rasHasSalvageVolProd1 (const RUN_SUMMARY_STRUCT& runSummary)
{
   bool rbcsCollected    = runSummary.rs2_RBC_volume > 0;
   bool salvageVolExists = runSummary.rbc_salvage_volume > 0;

   bool show             = (rbcsCollected && salvageVolExists);

   DataLog (log_level_gui_info) << ((show) ? "S" : "Do NOT s") << "how RBC prod1: (rbcCol="
                                << rbcsCollected << "; salvVol=" << salvageVolExists << ")." << endmsg;

   return show;
}


bool RunsumGlobs::rasHasSalvageVolProd2 (const RUN_SUMMARY_STRUCT& runSummary)
{
   bool rbcsCollected    = runSummary.rs2_RBC_volume_2 > 0;
   bool salvageVolExists = runSummary.rbc_salvage_volume_2 > 0;

   bool show             = (rbcsCollected && salvageVolExists);

   DataLog (log_level_gui_info) << ((show) ? "S" : "Do NOT s") << "how RBC prod2: (rbcCol=" << rbcsCollected
                                << "; salvVol=" << salvageVolExists << ")." << endmsg;

   return show;
}


bool RunsumGlobs::pasHasSalvageVol (const RUN_SUMMARY_STRUCT& runSummary)
{
   bool pltsCollected    = runSummary.rs2_platelet_volume > 0;
   bool salvageVolExists = runSummary.platelet_salvage_volume > 0;

   bool show             = (pltsCollected && salvageVolExists);

   DataLog (log_level_gui_info) << ((show) ? "S" : "Do NOT s") << "how PLT prod: (pltCol=" << pltsCollected
                                << "; salvVol=" << salvageVolExists << ")." << endmsg;

   return show;
}

int RunsumGlobs::getPasSalvageVol (const RUN_SUMMARY_STRUCT& runSummary)
{
   if (pasHasSalvageVol(runSummary))
   {
      DataLog (log_level_gui_info) << "Platelet product requires salvage volume of "
                                   << runSummary.platelet_salvage_volume << "mL." << endmsg;

      return runSummary.platelet_salvage_volume;
   }

   return 0;
}

int RunsumGlobs::getRasSalvageVolProd1 (const RUN_SUMMARY_STRUCT& runSummary)
{
   if (rasHasSalvageVolProd1(runSummary))
   {
      DataLog (log_level_gui_info) << "RBC product 1 requires salvage volume of "
                                   << runSummary.rbc_salvage_volume << "mL." << endmsg;
      return runSummary.rbc_salvage_volume;
   }

   return 0;
}

int RunsumGlobs::getRasSalvageVolProd2 (const RUN_SUMMARY_STRUCT& runSummary)
{
   if (rasHasSalvageVolProd2(runSummary))
   {
      DataLog (log_level_gui_info) << "RBC product 2 requires salvage volume of "
                                   << runSummary.rbc_salvage_volume_2 << "mL." << endmsg;
      return runSummary.rbc_salvage_volume_2;
   }

   return 0;
}


/////////////////////////////////////////////////////////////////////////////
// This is a really complicated function that chooses the next run summary
// screen based on the current screen, the current system state, the config
// settings, and the run summary structure.
//
// Prior to creating this function, most of this stuff was scattered around in about
// five different screens, with big chunks of it copy-and-pasted and the functionality
// duplicated. Personally, I find it easier to maintain one central function, even
// if it is a little more complicated, than to try to maintain a dozen similar, yet somewhat
// less complicated, functions.  Sorry.
//
void RunsumGlobs::gotoNextRunSummaryScreen (const RUN_SUMMARY_STRUCT& runSummary)
{
   bool pltProdCollected    = (runSummary.rs2_platelet_volume > 0.0f);
   bool plasmaProdCollected = (runSummary.rs2_plasma_volume > 0.0f);
   bool rbcProd1Collected   = (runSummary.rs2_RBC_volume > 0.0f);
   bool rbcProd2Collected   = (runSummary.rs2_RBC_volume_2 > 0.0f);

   bool plasmaProdHasFlags  =
      _pqa.AtLeastOneReason(VERIFY_PLASMA_VOL)
      || _pqa.AtLeastOneReason(VERIFY_PLASMA_WBCS);

   bool pltProdHasFlags =
      _pqa.AtLeastOneReason(CONC_OUT_OF_RANGE)
      || _pqa.AtLeastOneReason(VERIFY_PLATELET_WBCS)
      || _pqa.AtLeastOneReason(VERIFY_PLATELET_YIELD);

   bool rbcProd1HasFlags =
      _pqa.AtLeastOneReason(VERIFY_RBC_VOL)
      || _pqa.AtLeastOneReason(VERIFY_RBC_WBCS)
      || _pqa.AtLeastOneReason(VERIFY_RBC_QUALITY);

   bool rbcProd2HasFlags =
      _pqa.AtLeastOneReason(VERIFY_RBC_VOL_PROD2)
      || _pqa.AtLeastOneReason(VERIFY_RBC_WBCS_PROD2)
      || _pqa.AtLeastOneReason(VERIFY_RBC_QUALITY_PROD2);

   bool          labelPltProd        = _pqa.LabelPlatelets();
   bool          labelPlasmaProd     = _pqa.LabelPlasma();
   bool          labelRbcProd1       = _pqa.LabelRbcs();
   bool          labelRbcProd2       = _pqa.LabelRbcsProd2();

   bool          procedureIsRas      = guiglobs::aasRunInfo.isRasProcedure();
   bool          procedureIsPas      = guiglobs::aasRunInfo.isPasProcedure();
   bool          procedureHasBeenRas = guiglobs::aasRunInfo.isRasProcedure(true);
   bool          procedureHasBeenPas = guiglobs::aasRunInfo.isPasProcedure(true);

   bool          isDrbcAutoSplit     = runSummary.drbcAutoSplit;

   State_names   currentSystemState  = guiglobs::screen_status.get_current_system_state();
   GUI_SCREEN_ID currentScreen       = guiglobs::screen_status.get_current_screen();

   DataLog(log_level_gui_info) << "RunsumTransition [state=" << currentSystemState
                               << "; screen=" << currentScreen << "]; "
                               << "PLT(C=" << (int)pltProdCollected << ";F=" << (int)pltProdHasFlags << ";L=" << (int)labelPltProd << ") "
                               << "PLAS(C=" << (int)plasmaProdCollected << ";F=" << (int)pltProdHasFlags << ";L=" << (int)labelPlasmaProd << ") "
                               << "RBC1(C=" << (int)rbcProd1Collected << ";F=" << (int)rbcProd1HasFlags << ";L=" << (int)labelRbcProd1 << ") "
                               << "RBC2(C=" << (int)rbcProd2Collected << ";F=" << (int)rbcProd2HasFlags << ";L=" << (int)labelRbcProd2 << ") "
                               << "RAS=" << (int)procedureIsRas << "-" << (int)procedureHasBeenRas << "; PAS=" << (int)procedureIsPas << "-"
                               << (int)procedureHasBeenPas << "; autoSplit=" << (int)isDrbcAutoSplit << ";" << endmsg;

   // In the top-level part of this switch statement (but not the nested switches),
   // fallthrough is expected, and thus there are no break statements.
   // This may seem like a very complicated decision tree, but believe me when I say
   // that it's far less maintenence than making these decisions individually from each
   // of these individual screens. -Dan
   switch (currentScreen)
   {
      case GUI_SCREEN_RUNPROC :
         // If necessary, show the MSS INCOMPLETE screen to instruct for
         // offline addition.  Otherwise, fall through to transition to
         // run summary 1.
         if (RunsumGlobs::shouldShowAasSalvage(runSummary))
         {
            Base_Apheresis_Window::goto_screen(GUI_SCREEN_AAS_RUN_SUMMARY, "GUI_SCREEN_AAS_RUN_SUMMARY");
            return;
         }

      case GUI_SCREEN_AAS_ADDITION :
      case GUI_SCREEN_AAS_RUN_SUMMARY :
         gotoNextRunSummaryScreen();
         return;

      case GUI_SCREEN_RUNSUMM1 :

         if (currentSystemState == METERED_STORAGE || currentSystemState == BLOOD_RINSEBACK)
         {
            Base_Apheresis_Window::goto_screen(GUI_SCREEN_RUNSUMM2, "GUI_SCREEN_RUNSUMM2");
            return;
         }

      case GUI_SCREEN_AAS_TRANSITION_DUMMY :

         // see if we need to go to PLT RS
         switch (currentSystemState)
         {
            case METERED_STORAGE :
               if (procedureIsPas &&
                   pltProdCollected &&
                   pltProdHasFlags)
               {
                  Base_Apheresis_Window::goto_screen(GUI_SCREEN_RUNSUM_PLT, "GUI_SCREEN_PRE_MSS_PLT_FLAGS");
                  return;
               }
               break;

            case POST_RUN :
               if (pltProdCollected && (pltProdHasFlags || labelPltProd))
               {
                  Base_Apheresis_Window::goto_screen (GUI_SCREEN_RUNSUM_PLT, "GUI_SCREEN_RUNSUM_PLT");
                  return;
               }
               break;

            default :
               break; // do nothing; fall through.
         }

      case GUI_SCREEN_RUNSUM_PLT :
         // see if we need to go to PLS RS
         if ((currentSystemState == POST_RUN)
             && plasmaProdCollected
             && (plasmaProdHasFlags || labelPlasmaProd))
         {
            Base_Apheresis_Window::goto_screen (GUI_SCREEN_RUNSUM_PLASMA, "GUI_SCREEN_RUNSUM_PLASMA");
            return;
         }

      case GUI_SCREEN_RUNSUM_PLASMA :
         // see if we need to go to RBC RS or RBC RS1
         switch (currentSystemState)
         {
            /*  IT 8015 : Don't show RBC flags before metering.
                    case METERED_STORAGE:
                        if (procedureIsRas &&
                            rbcProd1Collected &&
                            rbcProd1HasFlags)
                        {
                            if (isDrbcAutoSplit) Base_Apheresis_Window::goto_screen(GUI_SCREEN_RUNSUM_RBC_PROD1, "GUI_SCREEN_RUNSUM_RBC_PROD1");
                            else Base_Apheresis_Window::goto_screen(GUI_SCREEN_RUNSUM_RBC, "GUI_SCREEN_RUNSUM_RBC");
                            return;
                        }
                        break;
            */
            case POST_RUN :
               if (rbcProd1Collected
                   && (rbcProd1HasFlags || (procedureHasBeenRas && labelRbcProd1))) // note: labeling stuff doesn't apply to non-metered procedures.
               {
                #if CPU==SIMNT
                  Base_Apheresis_Window::goto_screen(GUI_SCREEN_RUNSUM_RBC, "GUI_SCREEN_RUNSUM_RBC");
                #else
                  if (isDrbcAutoSplit) Base_Apheresis_Window::goto_screen(GUI_SCREEN_RUNSUM_RBC_PROD1, "GUI_SCREEN_RUNSUM_RBC_PROD1");
                  else Base_Apheresis_Window::goto_screen(GUI_SCREEN_RUNSUM_RBC, "GUI_SCREEN_RUNSUM_RBC");
                #endif /* if CPU==SIMNT */
                  return;
               }
               break;
            default :
               break;
         }
    #if CPU==SIMNT
      case GUI_SCREEN_RUNSUM_RBC :
         switch (currentSystemState)
         {
            case POST_RUN :
               if (rbcProd1Collected
                   && (rbcProd1HasFlags || (procedureHasBeenRas && labelRbcProd1))) // note: labeling stuff doesn't apply to non-metered procedures.
               {
                  Base_Apheresis_Window::goto_screen(GUI_SCREEN_RUNSUM_RBC_PROD1, "GUI_SCREEN_RUNSUM_RBC_PROD1");
                  return;
               }
               break;
            default :
               break;
         }
    #endif /* if CPU!=SIMNT */

      case GUI_SCREEN_RUNSUM_RBC_PROD1 :
         // see if we need to go to RBC RS2.
         switch (currentSystemState)
         {
            /*  IT 8015 : Don't show RBC flags before metering.
                    case METERED_STORAGE:
                        if (rbcProd2Collected &&
                            procedureIsRas &&
                            isDrbcAutoSplit &&
                            rbcProd2HasFlags)
                        {
                            Base_Apheresis_Window::goto_screen(GUI_SCREEN_RUNSUM_RBC_PROD2, "GUI_SCREEN_RUNSUM_RBC_PROD2");
                            return;
                        }
                        break;
            */
            case POST_RUN :
               if (isDrbcAutoSplit && rbcProd2Collected
                   && (rbcProd2HasFlags || (procedureHasBeenRas && labelRbcProd2))) // note: labeling stuff doesn't apply to non-metered procedures.
               {
                  Base_Apheresis_Window::goto_screen(GUI_SCREEN_RUNSUM_RBC_PROD2, "GUI_SCREEN_RUNSUM_RBC_PROD2");
                  return;
               }
               break;
            default :
               break;
         }

      case GUI_SCREEN_RUNSUM_RBC_PROD2 :
    #if CPU!=SIMNT
      case GUI_SCREEN_RUNSUM_RBC :
    #endif /* if CPU!=SIMNT */

         if (currentSystemState == METERED_STORAGE)
         {
            Base_Apheresis_Window::goto_screen(GUI_SCREEN_AAS_CLAMP_CHANNEL, "GUI_SCREEN_AAS_CLAMP_CHANNEL");
            return;
         }

      default :
         // If, at this point, we haven't figured out a better place to go,
         // then go to runsummary 2.
         Base_Apheresis_Window::goto_screen (GUI_SCREEN_RUNSUMM2, "GUI_SCREEN_RUNSUMM2");
         return;
   }
}

void RunsumGlobs::gotoNextRunSummaryScreen ()
{
   GUI_SCREEN_ID currentScreen = guiglobs::screen_status.get_current_screen();

   // Note: no "break" statements in this switch, because fallthrough
   // is expected and desired.
   switch (currentScreen)
   {
      case GUI_SCREEN_RUNPROC :
      case GUI_SCREEN_AAS_ADDITION :
      case GUI_SCREEN_AAS_RUN_SUMMARY :
         // From the run procedure screen or the AAS addition screen, we always go to runsummary 1 (whether post-run or rinseback).
         Base_Apheresis_Window::goto_screen(GUI_SCREEN_RUNSUMM1, "GUI_SCREEN_RUNSUMM1");
         return;

      default :
         DataLog (log_level_gui_error) << "This version of the function should never be called from screen " << currentScreen << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad screen transition");
   }
}


void RunsumGlobs::requestRunsumStats ()
{
   RequestEndRunStatsMsg request(MessageBase::SEND_LOCAL);
   request.send(0);
}

void RunsumGlobs::completeRunsumStatsProcessing ()
{
    #if CPU==SIMNT
   SimntGuiProcessedRunsumStruct msg(MessageBase::SEND_LOCAL);
   msg.send();
    #endif // #if CPU==SIMNT
}

/* FORMAT HASH 5979b3fc1ad36d30b52f0a91eb6cf915 */
