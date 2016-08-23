/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rbcdetectorinit.cpp
 *
 */

#ifndef _RBCDETECTORINIT_H_
#define _RBCDETECTORINIT_H_

#include <stddef.h>

#include "auxclock.h"
#include "an_alarm.h"
#include "guiproc.h"
#include "trimamessages.h"


// Local constants
const int MIN_DRIVE_VALUE = 0;            // Lowest possible drive value
const int MAX_DRIVE_VALUE = 255;          // Highest possible drive value
const int LOGGING_SAMPLES = 5;


class RBCDetectorInit
{
public:

   enum CalibrateState
   {
      CheckCalibration,
      DisableGreenCheck,
      DisableCalCheck
   };

public:

   //
   // Default constructor
   RBCDetectorInit();

   //
   // Destructor
   virtual ~RBCDetectorInit();

   //
   // Function called to initialize the object
   //
   int init ();

   //
   // Function called to do the processing of the object
   //
   int calibrate ();

   //
   // Function called during the run to check the calibration
   //
   void checkCalibration (CalibrateState calCheckDisabled);

   //
   // Callback function for the calibrate request and restore
   // calibration messages
   //
   void calibrateRequest ();
   void restoreCalibration ();

   //
   // When I am done I have use this method to set the detector calibrated
   //  or send out the appropriate alarm.
   //
   void      calibrated (const int value);
   const int calibrated () const { return _Calibrated; }

protected:

   //
   // Function to clear all tracking members to constructor-default values.
   //
   void restart ();

   //
   // Function which checks calibration
   //
   int checkCalibration (int sample,
                         int min,
                         int max,
                         int consecutiveSamples);

   //
   // Function called to delete any dynamic memory created by the object
   //
   void cleanup ();


   //
   // Callback for Cassette position change messages
   //
   void cassetteChange ();

protected:


   //
   // Message sent to request or restore a calibration
   //
   RBCCalibrationRequestMsg* _CalRequestMsg;
   RBCRestoreCalibrationMsg* _CalRestoreMsg;

   //
   // Message monitored for Cassette position changes in order to re-calibrate.
   //
   CassettePositionStatusMsg* _CalCassStatMsg;

   //
   // Alarm for the RBC Calibration failing
   //
   anAlarmMsg _RBCCalAlarm;

   //
   // Flag to signify that we have been calibrated
   //
   int _CalibrateRequest;

   //
   // Flag to signify that we have been calibrated
   //
   int _Calibrated;

   //
   // Value of red LED drive command (0-255)
   //
   unsigned char _RedDriveValue;

   //
   // Value of green LED drive command (0-255)
   //
   unsigned char _GreenDriveValue;

   //
   // Value of Red Reflectance Value (0-~32000 raw counts)
   //
   short _RedReflectanceValue;

   //
   // Value of Green Reflectance Value (0-~32000 raw counts)
   //
   short _GreenReflectanceValue;

   //
   // tolerance target (set dependent)
   //
   float _DesiredTolerance;

   //
   // Number of samples outside of range during the
   //  reasonablness test
   //
   int _ConsecutiveBadSamples;

   int _RedSample[LOGGING_SAMPLES];
   int _GreenSample[LOGGING_SAMPLES];

private:
   void doBracket (void);
   void doSample (void);

private:
   enum { RBCTarget   = 1000 };
   enum { RBCRange    = 50 };
   enum { SampleRange = 1 };  // Number of 1000 millisecond periods to wait for rbc readings

   int _Target;
   int _Range;

   enum CalStatus { Initialize, Bracket, Sample, Done, Abort };

   CalStatus      _status;

   unsigned long  _lastReflectanceCounts;
   unsigned long  _lastDriveCounts;

   unsigned short _lowRedDrive;
   unsigned short _highRedDrive;
   unsigned short _lowGreenDrive;
   unsigned short _highGreenDrive;

   unsigned short _redSample[2 * SampleRange + 1];
   unsigned short _greenSample[2 * SampleRange + 1];

   int            _sampleCount;
   int            _minRed;
   int            _maxRed;
   int            _minGreen;
   int            _maxGreen;
   int            _testFreq;
   int            _testSamples;

   int            _bestMatchRed;
   int            _bestMatchGreen;
   int            _bestMatch;

   float          _red;
   float          _green;

   rawTime        _lastUpdateTime;
};

#endif /* ifndef _RBCDETECTORINIT_H_ */

/* FORMAT HASH 27afa9893cb199ef35a0a24c9c2765a2 */
