/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simConfig.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 *
 *******************************************************************************
 *
 * REVISION HISTORY:
 * $Log: simConfig.hpp $
 *
 ******************************************************************************/
class SimConfig
{
public:
   static void walkThroughConfig ();
   static void walkThroughConfigAutomatedProcessesPlatelets ();
   static void walkThroughConfigMachine ();
   static void walkThroughConfigLanguageUnit ();
   static void walkThroughConfigProcedure ();
   static void walkThroughConfigPlasma ();
   static void walkThroughConfigRedBloodCells ();
   static void walkThroughConfigDonor ();
   static void walkThroughConfigDonorVolumeLimits ();
   static void walkThroughConfigReplacementFluid ();
   static void walkThroughConfigProcedurePriority ();
   static void walkThroughConfigSoftwareOptions ();
   static void walkThroughConfigAmapPlatelets ();
   static void walkThroughConfigProcedurePriorityAmapPlt ();
   static void walkThroughAirOutMitigation ();

   static void setMinAmapPlasma (const char* stringNum);
   static void setAmapPlatelets ();
   static void setCompPlasma ();
   static void setAirOutMitigation ();
   static void setShowPlasmaVolume ();
   static void resetSoftwareConfiguration ();
   static void setupPrerunSimulatorConfig ();
   static void setupRunSimulatorConfig ();

   static void turnOffJapaneseFeatures();
   static void turnOnJapaneseFeatures();

   static void configureIgnoredStrings ();

   static void gotoConfig ();
   static void exitConfig ();
   static void utilWalkThrough ();
};

/* FORMAT HASH 939929eaf3c02b308a4a61a02df2d9ca */
