/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#ifndef _GUIGLOBS_INCLUDE
#define _GUIGLOBS_INCLUDE

#include <vxworks.h> // Needed for SIMNT compiler directives

#include "config_cds.h"
#include "procrun_cds.h"
#include "procruntargets_cds.h"
#include "procvolumes_cds.h"
#include "proctrimaset_cds.h"
#include "proctime_cds.h"
#include "procpqi_cds.h"
#include "xif_cds.h"
#include "guiProcedures_cds.h"
#include "procadjust_cds.h"

// TODO: Fix this compromise/workaround, as it's not strictly necessary here.
// It's no longer included by config_cds.h, but since so many gui modules use
// Software_CDS without providing this include, it's not practical to fix now.
#include "software_cds.h"

#include "alarmhdl.hpp"
#include "audiofb.hpp"
#include "barcode_repository.hpp"
#include "bio_mgr.h"
#include "cassette_mgr.hpp"
#include "donordat.hpp"
#include "messagesystem.h"
#include "osgraphics.h"
#include "procedure_status.h"
#include "scrnstat.hpp"
#include "trima_datalog.h"
#include "cfr_list.h"
#include "guiproc.h"
#include "aasRunInfo.hpp"
#include "guiMain.hpp"
#include "dynamicListboxItem.hpp"
#include "operator_config.h"
#include "trima_unicode_string_types.h"


#define __RUNSUM_TO_MSS_ADDITION_ALLOC_PARAM__ "RUNSUM_TO_ADDITION"

class StatBarBase;
class Status_Line;

class GuiCdsData
{
public:
   // Constructor
   GuiCdsData();

   // Cds Objects
   Config_CDS         config;
   ProcRun_CDS        run;
   ProcRun_CDS        run_RW;
   ProcRunTargets_CDS targets;
   ProcVolumes_CDS    volumes;
   ProcTrimaSet_CDS   set;
   ProcTime_CDS       time;
   External_IF_CDS    xif;
   GuiProcedures_CDS  procedures;
   Operator_Config    rwConfig;
   ProcPQI_CDS        pqi_cds;
   ProcAdjust_CDS     adjustments;
};


class guiglobs
{
private:

   static const char* findIdString (const char* memPtr,          // start of memory block
                                    unsigned int memLength,      // length of memory block (in bytes)
                                    const char* pattern,         // pattern to search for
                                    unsigned int& memStart,      // starting byte within memory block for search
                                    unsigned int& stringLength   // length of found string
                                    );

public:
   static bool isClinicalBuild ();
   static bool isTestBuild ();
   static bool screenshotsAreEnabled ();

   static bool amapPlatelets ();
   static bool complementaryPlasmaEnabled ();
   static bool airToDonorMitigationEnabled ();

   static GuiCdsData            cdsData;

   static Screen_Commutator*    scrnComm;
   static GuiMain*              guiMain;
   static StatBarBase*          apheresis_status_bar;
   static Status_Line*          apheresis_status_line;
   static Current_Screen_Status screen_status;
   static Audio_Feedback        button_audio_feedback;
   static Alarm_Handler         gui_alarm_manager;
   static Donor_Data_Repository gui_donor_data_repository;
   static MessageSystem         message_system;
   static CCfgVarList           config_var_list;
   static Barcode_Repository*   barcode_repo;
   static ProcedureStatus       proc_status;

   static BioSignaturesMgr&     bioMgr ();

   static CassetteMgr*          cassette_mgr;
   static int                   display_dont_connect_donor_icon;
   static Bitmap                dont_connect_icon;
   static int                   display_vista_connected_icon;
   static Bitmap                vista_connected_icon;
   static CFR_LIST              cfr_list;
   static AasRunInfo            aasRunInfo;

   static unsigned char         stack_screens_to_process;
   static ALARM_VALUES          current_displayed_alarm;

   static bool                  run_summary_display_active;
   static bool                  donor_disconnect_confirmed;

   static bool                  procedure_selected;

   static bool                  timeOnlyConfigMode;

   static bool                  currentlyInPreConnectDonorDataCheck; // Long m'er f'ing variable name says it all.

   static bool                  donor_platelet_precount_entered;

   // Proc sends gui the cobe config value (currently 4200) for
   // the threshold of PLT concentration at which we flag for
   // WBC contamination.  That value is stored here, and used
   // on the PLT config screen.
   static float concWbcFlagThreshold;


   static float PtfFilterLoss;
   static float ptfFilterVol;

   static float rasBagEmptyPercentage;
   static float pasBagEmptyPercentage;

   static float flowAdjMin;
   static float flowAdjMax;

   static float pasPrimeVol;
   static float pasVolBiasPercent;
   static float pasNoRinsebackPenalty;

   static float minPlsRinseVol;

   // for showing the symbol on the runproc display.
   static bool showCompPlsSymbol ();
   static void showCompPlsSymbol (bool val);

   static float donor_tbv ();

   // These functions are used to determine whether or not the clamp
   // pas line and clamp crossover screens must be displayed after
   // sysclamp.  They should not be called before a cassette has
   // been selected.
   static bool shouldShowClampPas ();
   static bool shouldShowClampPlasmaAirBag ();
   static bool shouldShowClampCrossover ();

   static bool shouldShowRbcSplitInit ();
   static bool isDRBC ();



   //
   // Are we currently in either saline or plasma rinseback?
   //
   static bool isSalineRinseback ();
   static bool isPlasmaRinseback ();
   static bool isSpecialRinseback ();

   static void initStatBar ();

   // RKM.  These items can probably be moved to somewhere else
   static bool setIntDataItem       (const char* name, int newValue);
   static bool setTextDataItem      (const char* name, const char* newValue);

#if 0  // These are unused
   static bool setDoubleDataItem    (const char* name, double newValue);
   static bool setTextItemDataItem  (const char* name, TrimaTextItem* newValue);
   static bool setTextItemDataItem  (const char* name, const TrimaStringChar* newValue);
   static bool setTextItemDataItem  (const char* name, char* newValue);
   static bool setClockDataItem     (const char* name, int newValue);
#endif

#if CPU==SIMNT
   static void swapStatBar ();
#else
private:
#endif // #if CPU != SIMNT

   static void setStatBar (bool withAas);
};


#endif /* ifndef _GUIGLOBS_INCLUDE */

/* FORMAT HASH efd04e17f83ebbe9c09eed7e94992285 */
