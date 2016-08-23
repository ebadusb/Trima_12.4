#ifndef INCLUDE_VISTA_COMMON
#define INCLUDE_VISTA_COMMON

/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      vip_common.h
 *             External interface process
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file contains helper functions for the conversion of donor
 *             and configuration data from the interface types to the internal ones
 *
 ******************************************************************************/
#include "vipif.h"

#include "donor.h"        // CDonor Class
#include "config_cds.h"   // Offline Config CDS


class VistaConfig
{

// Member Functions
public:
   VistaConfig(Config_CDS* cds);
   virtual ~VistaConfig();

   //
   // These routine manages a snapshot of the config
   //
   void Snapshot ();
   void Rollback ();

   string errorMsg () const { return _errorMsg.str(); }

   //
   // This routine processes a donor sent by Vista.
   // This routine returns a 0 for success or the error return code if invalid
   //
   int ProcessDonorVitals (CDonor& donor, const donorInfoMsg& donorInfo);


   //
   // These routines copy over the Language section to the offline configuration
   // (config.dat)
   //
   void CopyLanguageSection (const donorInfoMsg& donorInfo);
   void CopyLanguageSection (const setOfflineConfigurationMsg& newConfig);


   //
   // This routine copies over the Machine section of the offline configuration
   // (config.dat)
   //
   void CopyMachineSection (const donorInfoMsg& donorInfo);
   void CopyMachineSection (const setOfflineConfigurationMsg& newConfig);


   //
   // This routine copies over the Procedure section of the offline configuration
   // (config.dat)
   //
   void CopyProcedureSection (const donorInfoMsg& donorInfo);
   void CopyProcedureSection (const setOfflineConfigurationMsg& newConfig);


   //
   // This routine copies over the Predict section of the offline configuration
   // (config.dat)
   //
   void CopyPredictSection (const donorInfoMsg& donorInfo);
   void CopyPredictSection (const setOfflineConfigurationMsg& newConfig);


   //
   // This routine copies over the Procedure List section of the offline
   // configuration
   //
   void CopyProcedureList (const donorInfoMsg& donorInfo);
   void CopyProcedureList (const setOfflineConfigurationMsg& newConfig);


   //
   // This routine copies over the Procedure Templates section of the offline
   // Configuration.  It returns a true if the settings were successfully validated.
   // The error string contains the validation failure reason when the status returns
   // false.
   //
   bool CopyProcedureTemplates (const setOfflineConfigurationMsg& newConfig);


   //
   // This routine validates the field ranges for the Vista Procedure section of the
   // donor download message.
   // This routine returns a 0 for success or the error return code if invalid
   //
   int ValidateVistaProcedureSection (const donorInfoMsg& donorInfo);


   // Helper routines that validate specific items.
   // These routines should be folded into the config class if possible.
   bool ValidMaxDonorTime (int value);

protected:
   // Helper routines that validate specific items.
   // These routines should be folded into the config class if possible.
   bool ValidProcedure (float value, float lowRange, float highRange, bool allowZero, bool allowOdd, const char* s, int index);
   bool ValidRasItems (bool meteringRas, float rbcDose, float rbcCrit, int rasVolume, int drbcThreshold, bool splitAlert, int index);
   bool ValidPasItems (bool meteringPas, float plateletVolume, int pco, int index);

   //
   // This routine does a range check on a value given its range constraints
   // Note that lower and upper bounds are checked inclusive
   //
   bool notInRange (int value, int lowRange, int highRange);
   bool notInRange (float value, float lowRange, float highRange);

// Member Functions.
private:
   VistaConfig();   // Hide base constructor


// Member Data
private:
   Config_CDS*        _config;

   std::ostringstream _errorMsg;

   // Snapshot data
   LangUnitCfg            _lang;
   BasicMachineCfg        _machine;
   ProcedureConfig        _procedure;
   PredictionConfig       _predict;
   ProductTemplates       _productTemplates;
   ProductDefinitionArray _productDefinition;
   ProductListArray       _procedureList;
   int                    _numberOfProcedures;

   // Used for range checking procedure list.
   const CCfgVarInfo _pltYield;
   const CCfgVarInfo _pltVolume;
   const CCfgVarInfo _meterPAS;
   const CCfgVarInfo _pco;
   const CCfgVarInfo _plsVolume;
   const CCfgVarInfo _rbcDose;
   const CCfgVarInfo _rbcCrit;
   const CCfgVarInfo _maxProcTime;
   const CCfgVarInfo _rbcMss;
   const CCfgVarInfo _rbcMssVolume;

   const CCfgVarInfo _amapPltIndex;
   const CCfgVarInfo _amapPltYield;
   const CCfgVarInfo _amapPltConcentration;
   const CCfgVarInfo _amapPltPasPco;

   bool              _complementaryPlasmaOn;
   int               _rbcMaxProductVolume;

};

#endif

/* FORMAT HASH 7c8366e92ae34208a345fa8d05bda035 */
