#include <vxWorks.h>
#include <math.h>

#include "ostime.hpp"
#include "caldat.h"
#include "procdata.h"
#include "rbcdat.h"
#include "rbcdetectorinit.h"
#include "trima_datalog.h"


RBCDetectorInit::RBCDetectorInit()
   : _CalRequestMsg(0),
     _RBCCalAlarm(RBC_DETECTOR_CAL_FAILURE),
     _CalRestoreMsg(0),
     _CalCassStatMsg(0)
{

   restart();
}

RBCDetectorInit::~RBCDetectorInit()
{

   cleanup();
}

int RBCDetectorInit::init ()
{

   //
   // Initialize the Request Message ...
   //
   _CalRequestMsg = new RBCCalibrationRequestMsg(Callback<RBCDetectorInit>(this, &RBCDetectorInit::calibrateRequest));

   _CalRestoreMsg = new RBCRestoreCalibrationMsg(Callback<RBCDetectorInit>(this, &RBCDetectorInit::restoreCalibration));

   //
   // Initialize the Cassette Position ...
   //
   _CalCassStatMsg = new CassettePositionStatusMsg(Callback<RBCDetectorInit>(this, &RBCDetectorInit::cassetteChange));

   _sampleCount    = 0;

   _lowRedDrive    = 1;
   _highRedDrive   = 255;
   _lowGreenDrive  = 1;
   _highGreenDrive = 255;

   _testFreq       = 1000; // msecs

   osTime::snapshotRawTime(_lastUpdateTime);

   _lastReflectanceCounts = 0;
   _lastDriveCounts       = 0;

   _status                = Bracket;

   _Target                = RBCTarget;
   _Range                 = RBCRange;

   //
   //
   //   Snarf the appropriate values from the cal file.
   //
   _minRed           = RbcDat::data().RBCTestMinRed;
   _maxRed           = RbcDat::data().RBCTestMaxRed;
   _minGreen         = RbcDat::data().RBCTestMinGreen;
   _maxGreen         = RbcDat::data().RBCTestMaxGreen;
   _testSamples      = RbcDat::data().RBCTestSamples;
   _DesiredTolerance = RbcDat::data().RBCRatioTolerance;

   return 1;
}

void RBCDetectorInit::restart ()
{  // Restart calibration, called by constructor and
   // when cassette is up.


   _CalibrateRequest      = 0;
   _Calibrated            = 0;

   _RedDriveValue         = 0;
   _GreenDriveValue       = 0;

   _DesiredTolerance      = 0.0f;
   _ConsecutiveBadSamples = 0;

   return;
}

void RBCDetectorInit::cassetteChange ()
{  // Restart calibration if cassette raises.


   if (_CalCassStatMsg->getData() == CASSETTE_RAISING)
   {

      restart();
   }
}

int RBCDetectorInit::calibrate (void)
{
   ProcData pd;

   //
   //  We are done if the cassette is raised ...
   //
   if (!pd.Status().CassIsDown())
   {

      restart();

      return 0;
   }

   if (osTime::howLongMilliSec(_lastUpdateTime) >= _testFreq)
   {
      osTime::snapshotRawTime(_lastUpdateTime);


      if ( pd.Status().NewReflectanceCounts() > _lastReflectanceCounts
           &&
           pd.Status().NewDriveCounts() > _lastDriveCounts)
      {

         _lastReflectanceCounts = pd.Status().NewReflectanceCounts();

         _lastDriveCounts       = pd.Status().NewDriveCounts();

         if (_status == Bracket)
         {

            doBracket();
         }
         else if (_status == Sample)
         {

            doSample();
         }
         else if (_status == Done)
         {
            _RedReflectanceValue   =   (short)( pd.Status().RedValue() );
            _GreenReflectanceValue = (short)( pd.Status().GreenValue() );

            _RedDriveValue         = pd.TrimaSet().RBCRedDriveValue.Get();
            _GreenDriveValue       = pd.TrimaSet().RBCGreenDriveValue.Get();

            if (_RedReflectanceValue   < _Target - _Range ||
                _RedReflectanceValue   > _Target + _Range ||
                _GreenReflectanceValue < _Target - _Range ||
                _GreenReflectanceValue > _Target + _Range)
            {
               calibrated(0);

               DataLog(log_level_proc_info) << "Calibration Failed: Status = "
                                            << _Calibrated
                                            << "  Desired Red/Green Reflectance: "
                                            << _Target
                                            << "  Desired RedGreenRange: +/- "
                                            << _Range
                                            << endmsg;

               DataLog(log_level_proc_info) << "Actual redDrive->redReflectance: "
                                            << (int)_RedDriveValue
                                            << "->"
                                            << _RedReflectanceValue
                                            << "  Actual greenDrive->greenReflectance: "
                                            << (int)_GreenDriveValue
                                            << "->"
                                            << _GreenReflectanceValue
                                            << endmsg;

               _status = Abort;
            }
            else
            {
               calibrated(1);

               DataLog(log_level_proc_info) << "Calibration Done: Status = "
                                            << _Calibrated
                                            << "  Desired Red/Green Reflectance: "
                                            << _Target
                                            << "  Desired RedGreenRange: +/- "
                                            << _Range
                                            << endmsg;

               DataLog(log_level_proc_info) << "Actual redDrive->redReflectance: "
                                            << (int)_RedDriveValue
                                            << "->"
                                            << _RedReflectanceValue
                                            << "  Actual greenDrive->greenReflectance: "
                                            << (int)_GreenDriveValue
                                            << "->"
                                            << _GreenReflectanceValue
                                            << endmsg;
            }
         }
      }
   }

   return 1;
}


void RBCDetectorInit::doBracket (void)
{
   ProcData pd;

   _RedReflectanceValue   =   (short)( pd.Status().RedValue() );
   _GreenReflectanceValue = (short)( pd.Status().GreenValue() );

   _RedDriveValue         = pd.TrimaSet().RBCRedDriveValue.Get();
   _GreenDriveValue       = pd.TrimaSet().RBCGreenDriveValue.Get();

   DataLog(log_level_proc_info) << "Bracket start:  redDrive->redReflectance: "
                                << (int)_RedDriveValue
                                << "->"
                                << _RedReflectanceValue
                                << "  greenDrive->greenReflectance: "
                                << (int)_GreenDriveValue
                                << "->"
                                << _GreenReflectanceValue
                                << endmsg;

   if (_highRedDrive > _lowRedDrive + 1 && (_RedDriveValue != 0))
   {

      if (_RedReflectanceValue > _Target)
      {

         _highRedDrive  = _RedDriveValue;

         _RedDriveValue = ( _lowRedDrive + _highRedDrive ) / 2;

         if (_RedDriveValue > _highRedDrive - 1)
         {
            _RedDriveValue = _highRedDrive - 1;
         }
      }
      else
      {

         _lowRedDrive   = _RedDriveValue;

         _RedDriveValue = (_lowRedDrive + _highRedDrive) / 2;

         if (_RedDriveValue < _lowRedDrive + 1)
         {
            _RedDriveValue = _lowRedDrive + 1;
         }
      }
   }

   if (_highGreenDrive > _lowGreenDrive + 1 && (_GreenDriveValue != 0))
   {
      if (_GreenReflectanceValue > _Target)
      {

         _highGreenDrive  = _GreenDriveValue;

         _GreenDriveValue = ( _lowGreenDrive + _highGreenDrive ) / 2;

         if (_GreenDriveValue > _highGreenDrive - 1)
         {
            _GreenDriveValue = _highGreenDrive - 1;
         }
      }
      else
      {
         _lowGreenDrive   = _GreenDriveValue;

         _GreenDriveValue = ( _lowGreenDrive + _highGreenDrive ) / 2;

         if (_GreenDriveValue < _lowGreenDrive + 1)
         {
            _GreenDriveValue = _lowGreenDrive + 1;
         }
      }
   }

   if (_highRedDrive <= _lowRedDrive + 1 &&
       _highGreenDrive <= _lowGreenDrive + 1)
   {
      _status = Sample;    // We are now ready to sample

      if (_lowRedDrive < SampleRange + 1)
      {
         _lowRedDrive = SampleRange + 1;
      }

      if (_lowGreenDrive < SampleRange + 1)
      {
         _lowGreenDrive = SampleRange + 1;
      }

      if (_lowRedDrive > 255 - SampleRange)
      {
         _lowRedDrive = 255 - SampleRange;
      }

      if (_lowGreenDrive > 255 - SampleRange)
      {
         _lowGreenDrive = 255 - SampleRange;
      }

      _RedDriveValue   = _lowRedDrive - SampleRange;

      _GreenDriveValue = _lowGreenDrive - SampleRange;

      _sampleCount     = 0;
   }

   DataLog(log_level_proc_info) << "Bracket end:  red[ "
                                << _lowRedDrive
                                << " to "
                                << _highRedDrive
                                << " ] "
                                << "  green[ "
                                << _lowGreenDrive
                                << " to "
                                << _highGreenDrive
                                << " ]"
                                << endmsg;

   DataLog(log_level_proc_info) << "Actual redDrive: "
                                << (int)_RedDriveValue
                                << "  Actual greenDrive: "
                                << (int)_GreenDriveValue
                                << endmsg;

   //
   //   Set drives to desired values
   //
   pd.Orders().RBCGreen(_GreenDriveValue);
   pd.Orders().RBCRed(_RedDriveValue);

   pd.MakeTrimaSetWritable();
   pd.TrimaSet().RBCRedDriveValue.Set(_RedDriveValue);
   pd.TrimaSet().RBCGreenDriveValue.Set(_GreenDriveValue);
   pd.MakeTrimaSetReadable();
}



void RBCDetectorInit::doSample (void)
{
   ProcData pd;

   _RedReflectanceValue   =   (short)( pd.Status().RedValue() );
   _GreenReflectanceValue = (short)( pd.Status().GreenValue() );

   _RedDriveValue         = pd.TrimaSet().RBCRedDriveValue.Get();
   _GreenDriveValue       = pd.TrimaSet().RBCGreenDriveValue.Get();

   DataLog(log_level_proc_info) << "Sample Start:  redDrive->redReflectance: "
                                << (int)_RedDriveValue
                                << "->"
                                << _RedReflectanceValue
                                << "  greenDrive->greenReflectance: "
                                << (int)_GreenDriveValue
                                << "->"
                                << _GreenReflectanceValue
                                << endmsg;

   _redSample[_sampleCount]   = _RedReflectanceValue;

   _greenSample[_sampleCount] = _GreenReflectanceValue;

   _sampleCount              += 1;

   if (_sampleCount < 2 * SampleRange + 1)
   {
      _RedDriveValue   = _lowRedDrive - SampleRange + _sampleCount;

      _GreenDriveValue = _lowGreenDrive - SampleRange + _sampleCount;
   }
   else
   {
      _bestMatchRed   = 0;
      _bestMatchGreen = 0;
      _bestMatch      = 9999;

      for (int r = 0; r < 2 * SampleRange + 1; r++)
      {
         for (int g = 0; g < 2 * SampleRange + 1; g++)
         {
            if (abs(_redSample[r] - _greenSample[g]) < _bestMatch)
            {
               _bestMatchRed   = r;
               _bestMatchGreen = g;
               _bestMatch      = abs(_redSample[r] - _greenSample[g]);
            }
         }
      }

      _RedDriveValue   = _lowRedDrive - SampleRange + _bestMatchRed;

      _GreenDriveValue = _lowGreenDrive - SampleRange + _bestMatchGreen;

      _status          = Done;
   }

   DataLog(log_level_proc_info) << "Sample End:  New redDrive: "
                                << (int)_RedDriveValue
                                << "  New greenDrive: "
                                << (int)_GreenDriveValue
                                << endmsg;

   //
   //   Set drives to desired values
   //
   pd.Orders().RBCGreen(_GreenDriveValue);
   pd.Orders().RBCRed(_RedDriveValue);

   pd.MakeTrimaSetWritable();
   pd.TrimaSet().RBCRedDriveValue.Set(_RedDriveValue);
   pd.TrimaSet().RBCGreenDriveValue.Set(_GreenDriveValue);
   pd.MakeTrimaSetReadable();
}


void RBCDetectorInit::calibrateRequest ()
{

   ProcData pd;
   bool     blackStamp = false;

   // If stamp is black, do not calibrate and use the rbc.dat cal values
   if (pd.TrimaSet().Cassette.Get() == cassette::RBCPLS)
   {
      _CalibrateRequest      = 0;
      _Calibrated            = 1;
      _RedReflectanceValue   =   (short)( pd.Status().RedValue() );
      _GreenReflectanceValue = (short)( pd.Status().GreenValue() );
      blackStamp             = true;
   }
   else
   {
      //
      // We have received a request ...
      //
      _CalibrateRequest = 1;

      //
      // Set that we are not calibrated ...
      //
      _Calibrated = 0;
   }

   if (( _status == Done ) ||
       ( _status == Abort))
   {
      DataLog(log_level_proc_info) << "RBC calibrateRequest(): Starting again" << endmsg;

      _sampleCount    = 0;

      _lowRedDrive    = 1;
      _highRedDrive   = 255;
      _lowGreenDrive  = 1;
      _highGreenDrive = 255;

      osTime::snapshotRawTime(_lastUpdateTime);

      _lastReflectanceCounts = 0;
      _lastDriveCounts       = 0;

      _status                = Bracket;
   }

   //
   // Initialize the starting values ...
   //
   _RedDriveValue   = (char)((int)CalDat::data().RBCInitRedDrive);
   _GreenDriveValue = (char)((int)CalDat::data().RBCInitGreenDrive);

   //
   // Drive the leds at the starting values
   //
   pd.Orders().RBCRed(_RedDriveValue);
   pd.Orders().RBCGreen(_GreenDriveValue);

   pd.MakeTrimaSetWritable();
   pd.TrimaSet().RBCRedDriveValue.Set(_RedDriveValue);
   pd.TrimaSet().RBCGreenDriveValue.Set(_GreenDriveValue);
   pd.MakeTrimaSetReadable();

   DataLog(log_level_proc_info) << "RBC calibrateRequest(): Initial redDrive = "
                                << (int)_RedDriveValue
                                << "  Initial greenDrive = "
                                << (int)_GreenDriveValue
                                << endmsg;

   // Provide the final values so as not to confuse the GTS guy.
   if (blackStamp)
   {
      DataLog(log_level_proc_info) << "Set is RBC/PLS.  Using default calibration values" << endmsg;

      DataLog(log_level_proc_info) << "Calibration Done: Status = " << _Calibrated << endmsg;

      DataLog(log_level_proc_info) << "Actual redDrive->redReflectance: "
                                   << (int)_RedDriveValue
                                   << "->"
                                   << _RedReflectanceValue
                                   << "  Actual greenDrive->greenReflectance: "
                                   << (int)_GreenDriveValue
                                   << "->"
                                   << _GreenReflectanceValue
                                   << endmsg;
   }
}


void RBCDetectorInit::checkCalibration (CalibrateState calCheckDisabled)
{
   ProcData pd;

   int      i;

   //
   //  RBC detector "reasonableness test":
   //  Check to ensure that the reflection values are within the configured
   //  range and alarm if the reflections are outside.
   //
   if (    pd.TrimaSet().CassetteState.Get() > ProcTrimaSet_CDS::BLOOD
           && pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::RINSEBACK
           && calCheckDisabled != RBCDetectorInit::DisableCalCheck
           && _Calibrated)
   {
      _red   =   (float)pd.Status().RedValue();
      _green = (float)pd.Status().GreenValue();

      //
      //
      //   Update time since our last sample and see if it's time to sample again.
      //
      if (osTime::howLongMilliSec(_lastUpdateTime) >= _testFreq)
      {
         osTime::snapshotRawTime(_lastUpdateTime);

         if (!checkCalibration(_red, _minRed, _maxRed, _testSamples) ||
             ( calCheckDisabled == RBCDetectorInit::CheckCalibration &&
               !checkCalibration(_green, _minGreen, _maxGreen, _testSamples) )
             )
         {

            DataLog(log_level_proc_info) << "RBC reasonableness fail red=";

            for (i = 0; i < LOGGING_SAMPLES; i++)
            {
               DataLog(log_level_proc_info) << _RedSample[i] << " ";
            }

            DataLog(log_level_proc_info) << _red << " green=";

            for (i = 0; i < LOGGING_SAMPLES; i++)
            {
               DataLog(log_level_proc_info) << _GreenSample[i] << " ";
            }

            DataLog(log_level_proc_info) << _green
                                         << " red drive="
                                         << pd.Status().RedDriveValue()
                                         << " green drive="
                                         << pd.Status().GreenDriveValue()
                                         << endmsg;

            //
            // Set the alarm ...
            calibrated(0);
         }
      }

      //
      //
      //   Save our last few samples for logging purposes
      for (i = 0; i < LOGGING_SAMPLES - 1; i++)
      {
         _RedSample[i]   = _RedSample[i + 1];
         _GreenSample[i] = _GreenSample[i + 1];
      }

      _RedSample[LOGGING_SAMPLES - 1]   = pd.Status().RedValue();
      _GreenSample[LOGGING_SAMPLES - 1] = pd.Status().GreenValue();
   }
}


void RBCDetectorInit::restoreCalibration ()
{

   ProcData pd;

   DataLog(log_level_proc_info) << "RBC restoreCalibration()" << endmsg;
   //
   // Restore the saved values
   //
   _RedDriveValue   = pd.TrimaSet().RBCRedDriveValue.Get();
   _GreenDriveValue = pd.TrimaSet().RBCGreenDriveValue.Get();

   //
   // Drive the leds at the starting values
   //
   pd.Orders().RBCRed(_RedDriveValue);
   pd.Orders().RBCGreen(_GreenDriveValue);

   DataLog(log_level_proc_info) << "RBC restored drives red:"
                                << (int)_RedDriveValue
                                << " green:"
                                << (int)_GreenDriveValue;

   //
   // Restore calibration status
   //
   _CalibrateRequest = 0;

   _Calibrated       = _CalRestoreMsg->getData();

   DataLog(log_level_proc_info) << " status:" << _Calibrated << endmsg;
}


void RBCDetectorInit::calibrated (const int value)
{
   ProcData pd;

   if (value)
   {
      _Calibrated = 1;
      pd.MakeRunWritable();
      pd.Run().wbcAlarmPending.Set(false);
      pd.MakeRunReadable();
   }
   else
   {

      //
      // ALARM! -can't calibrate RBC detector
      // Send alarm message indicating rbc detector cal failure
      // Note this can happen when the cassette is unloaded (due to
      // test failure) so we don't put out the alarms in these cases
      // since we are going to restart / retry when the cassette is
      // lowered again anyway.
      //
      if ( _CalibrateRequest &&
           pd.Status().CassIsDown())
      {
         _RBCCalAlarm.setAlarm();
         pd.MakeRunWritable();
         pd.Run().wbcAlarmPending.Set(true);
         pd.MakeRunReadable();
      }

      _CalibrateRequest = 0;
      _Calibrated       = 0;
   }
}


int RBCDetectorInit::checkCalibration (int sample,
                                       int min, int max,
                                       int samples)
{

   //
   //   Time to sample.  Test the current values to see if they fall
   //   outside the configured range.
   //
   if ( ( sample < min ) || ( sample > max ) )
   {
      ProcData pd;

      if ( pd.Status().CassIsPltPlsRBC() ||
           pd.Status().CollectPump.CmdFlow() > 0.0f)
      {
         //
         //
         //  Bad sample found!  If we have found enough consecutive
         //  samples outside the configured range then fire the alarm.
         //  This uncalibrates the RBC detector so it will no longer
         //  be valid for spillover detection.
         //
         if (++_ConsecutiveBadSamples >= samples)
         {

            return 0;
         }
      }
   }
   else
   {  // values were within range, reset number bad samples

      _ConsecutiveBadSamples = 0;
   }

   return 1;
}

void RBCDetectorInit::cleanup ()
{
   //
   // Delete the calibration request message ...
   //
   delete _CalRequestMsg;
   _CalRequestMsg = 0;
}

/* FORMAT HASH f6b126ceeb4165c173097bbaaae61a55 */
