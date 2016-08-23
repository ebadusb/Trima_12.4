#include "rbcdat.h"
#include "datfile.h"
#include "filenames.h"

static const char PATHNAME[] = PNAME_RBCDAT; // the one and only

//
// -----------------------------------------------------------------------------
//                      Constructor, destructor
//
//
RbcDat::RbcDat()
{}

RbcDat::~RbcDat()
{}

//
// -----------------------------------------------------------------------------
//
//   Return a constant reference to my data
//
const RbcDat::RbcDatStruct& RbcDat::data ()
{
   return _data;
}


//
// -----------------------------------------------------------------------------
//
ALARM_VALUES RbcDat::initialize (bool logError)
{
   ALARM_VALUES status = NULL_ALARM; // default

   //
   //
   // Create the dat file reader to retrieve the Cobe configuration data.
   //
   CDatFileReader config;

   if (config.Initialize(PATHNAME, logError) != DFR_OK)
   {
      return RBC_DAT_BAD_FILE;
   }

   _data.InitRedReturn             = config.get_float("RBC_DETECTOR", "init_red_return", 0.0f, 255.0f);
   _data.DesiredRBCRatio           = config.get_float("RBC_DETECTOR", "desired_rbc_ratio", 0.1f, 10.0f);
   _data.RBCRatioTolerance         = config.get_float("RBC_DETECTOR", "rbc_ratio_tolerance", 0.0f, 10.0f);
   _data.RBCRatioFilter            = config.get_float("RBC_DETECTOR", "rbc_ratio_filter_periods", 0.0f, 100.0f);
   _data.RedLowPltPlsRBCStamp      = config.get_float("RBC_DETECTOR", "red_low_pltplsrbc_stamp", 0.0f, 10000.0f);
   _data.RedHighPltPlsRBCStamp     = config.get_float("RBC_DETECTOR", "red_high_pltplsrbc_stamp", 0.0f, 10000.0f);
   _data.GreenLowPltPlsRBCStamp    = config.get_float("RBC_DETECTOR", "green_low_pltplsrbc_stamp", 0.0f, 10000.0f);
   _data.GreenHighPltPlsRBCStamp   = config.get_float("RBC_DETECTOR", "green_high_pltplsrbc_stamp", 0.0f, 10000.0f);
   _data.RedLowRBCPlsStamp         = config.get_float("RBC_DETECTOR", "red_low_rbcpls_stamp", 0.0f, 10000.0f);
   _data.RedHighRBCPlsStamp        = config.get_float("RBC_DETECTOR", "red_high_rbcpls_stamp", 0.0f, 10000.0f);
   _data.GreenLowRBCPlsStamp       = config.get_float("RBC_DETECTOR", "green_low_rbcpls_stamp", 0.0f, 10000.0f);
   _data.GreenHighRBCPlsStamp      = config.get_float("RBC_DETECTOR", "green_high_rbcpls_stamp", 0.0f, 10000.0f);

   _data.SpilloverTriggerRatio     = config.get_float("RBC_DETECTOR", "spillover_trigger_ratio", 0.0, 25.0);
   _data.SpilloverTriggerSamples   = (int)config.get_float("RBC_DETECTOR", "spillover_trigger_samples", 0.0, 5000.0);

   _data.RBCTestMinRed             = (int)config.get_float("RBC_DETECTOR", "rbc_test_min_red", 0.0f, 10000.0f);
   _data.RBCTestMaxRed             = (int)config.get_float("RBC_DETECTOR", "rbc_test_max_red", 0.0f, 10000.0f);
   _data.RBCTestMinGreen           = (int)config.get_float("RBC_DETECTOR", "rbc_test_min_green", 0.0f, 10000.0f);
   _data.RBCTestMaxGreen           = (int)config.get_float("RBC_DETECTOR", "rbc_test_max_green", 0.0f, 10000.0f);
   _data.RBCTestFreq               = (int)config.get_float("RBC_DETECTOR", "rbc_test_freq", 0.0f, 5000.0f);
   _data.RBCTestSamples            = (int)config.get_float("RBC_DETECTOR", "rbc_test_samples", 0.0f, 5000.0f);

   _data.MinimumCassetteReflection = config.get_float("RBC_DETECTOR", "minimum_cassette_reflection", 0.0f, 5000.0f);
   _data.MaximumCassetteReflection = config.get_float("RBC_DETECTOR", "maximum_cassette_reflection", 0.0f, 5000.0f);

   // # [VERIFY_FLAGS]
   // Number of Peak-to-Peak RG ratio samples constituting a full set for
   // Platelet Contaminant detection.
   _data.RGP2Psamples = (int)config.get_float("RBC_DETECTOR", "color_ratio_peak_to_peak_samples", 0.0f, 5000.0f);
   // Frequency of Peak-to-Peak RG ratio sampling for Platelet Contaminant detection,
   // expressed as a number of .5 second intervals.
   _data.RGP2Pfrequency = (int)config.get_float("RBC_DETECTOR", "color_ratio_peak_to_peak_frequency", 0.0f, 5000.0f);
   // Red/Green Ratio trigger for declaring Platelet Contamination.
   _data.RGP2Ptrigger   = config.get_float("RBC_DETECTOR", "color_ratio_peak_to_peak_trigger", 0.01f, 0.20f);
   // Factor of actual platelet yield from which the RBC-detector integrated yield
   // must deviate before a PLATELET_VOLUME_ERROR reason is declared.
   _data.YieldCutoffConst = config.get_float("RBC_DETECTOR", "yield_cutoff_constant", 0.0f, 0.9f);

   // # [INTEGRATED_YIELD]
   // Yield Integration constants used to compute RBC Detector's Yield
   _data.YieldReflectancePltPlsRBCSet = config.get_float("RBC_DETECTOR", "yield_reflectance_pltplsrbc_set", 0.1f, 999.9f);
   _data.VinStartSamplingREDpl        = config.get_float("RBC_DETECTOR", "vin_start_red_platelet_baseline_sampling", 200.0f, 500.0f);
   _data.dVinSamplingREDpl            = config.get_float("RBC_DETECTOR", "delta_vin_red_platelet_baseline_sampling", 20.0f, 100.0f);


   _data.LowConcentrationTriggerJapan         = config.get_float("RBC_DETECTOR", "low_concentration_alert_trigger_japan", 0.0f, 10.0f);
   _data.LowConcentrationTrigger              = config.get_float("RBC_DETECTOR", "low_concentration_alert_trigger", 0.0f, 10.0f);
   _data.LowConcentrationThreshold            = config.get_float("RBC_DETECTOR", "low_concentration_alert_threshold", 0.0f, 1.0f);
   _data.LowConcentrationShutoffConcentration = config.get_float("RBC_DETECTOR", "low_concentration_alert_shutoff_concentration", 0.0f, 10000.0f);

   // [CHAMBER_SATURATION]
   _data.ChamberSatFsat  = config.get_float("RBC_DETECTOR", "chamber_sat_fsat", 0.2f, 5.0f);
   _data.ChamberSatSWs3a = (bool)config.get_int("RBC_DETECTOR", "chamber_sat_sws3a", 0, 1);
   _data.ChamberSatSWs3b = (bool)config.get_int("RBC_DETECTOR", "chamber_sat_sws3b", 0, 1);
   _data.ChamberSatSWs4a = (bool)config.get_int("RBC_DETECTOR", "chamber_sat_sws4a", 0, 1);
   _data.ChamberSatSWs4b = (bool)config.get_int("RBC_DETECTOR", "chamber_sat_sws4b", 0, 1);

   // [PRIME_HEMOLYSIS]
   _data.CheckForPrimeHemolysis = (bool)config.get_int("RBC_DETECTOR", "check_for_prime_hemolysis", 0, 1);

   //
   //
   //  Final status check to ensure that all values were read in properly.
   //
   DFR_ERROR configStatus = config.Error();

   if (configStatus == DFR_NOTFOUND)
   {
      status = RBC_DAT_VALUE_NOT_FOUND;
   }
   else if (configStatus == DFR_VALUE)
   {
      status = RBC_DAT_VALUE_OUT_OF_RANGE;
   }
   else if (configStatus == DFR_PARSE)
   {
      status = RBC_DAT_PARSE_ERROR;
   }

   return status;
}

//
//
//  My singleton data instance
//
RbcDat::RbcDatStruct RbcDat::_data = {0};

/* FORMAT HASH c2ba2280b1c1bdf54d742cd6de7388bb */
