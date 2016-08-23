/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/trima_common/rcs/safproc.h 1.14 2008/04/23 23:48:46Z dslausb Exp ssunusb $
 * $Log $
 *
 *
 * TITLE:      safproc.hpp, FS Safety System and Procedure common interface file.
 *
 * ABSTRACT:   Interface/Include file for FS Safety System and Procedure where common
 *             definitions are required,
 *
 */

#ifndef SAF_PROC_HPP
#define SAF_PROC_HPP

// include for config data portion of SDonorVitals struct
#include "units.h"
#include "bloodtype.h"


/*++++++++++++++++++++++++++++++++++++++++++++++++
        RequestSafetyPower Defines : Start
++++++++++++++++++++++++++++++++++++++++++++++++++*/

// possible values for the RequestSafetyPower message from proc to safety
// 03/12/96 note : TEST_BOTH_POWER is enumed, and implemented on the
// safety side....if proc request power ( 24,64, both)  enabled,
// if the safety T1 scoreboard does not indicate the test(s) has/have been
// performed , safety will test the power switches before enabling the
// power. This will occurr if proc requests power before having sent the
// TEST_BOTH_POWER message.

enum REQUEST_POWER_VALUES
{
   ENABLE_24V_POWER,
   ENABLE_64V_POWER,
   ENABLE_BOTH_POWER,
   ENABLE_SOLENOID_POWER,
   DISABLE_24V_POWER,
   DISABLE_64V_POWER,
   DISABLE_BOTH_POWER,
   DISABLE_SOLENOID_POWER,
   TEST_BOTH_POWER,
};

// if an error is detected in testing the safety power switches, an alarm will
// be genrated by safety.  The error responses listed below are defined simply so
// that a response message is generated to the RequestSafetyPowere even in the face
// of an alarm condidition.
// possible responses for failed power requests ( SafetyTestDone ).
// Safety will return one of these values as indicated for its response to the
// RequestSafetyPower message from procedure.  The expected response to the
// RequestSafetyPower message is SafetyTestDone.
#define PWR_CONTROL_24V_FAILED              -1     // T1 test for 24V power control failed
#define PWR_CONTROL_64V_FAILED              -2     // T1 test for 64V power control failed
#define PWR_CONTROL_BOTH_FAILED             -3     // T1 test for 24V and 64V power control failed
// possible responses for successful power requests (SafetyTestDone )
// these can indicate a passing test or an agreement to perform the request
#define PWR_CONTROL_24V_NOMINAL             1 << 0       // T1 test for 24V passed or agree to request
      #define PWR_CONTROL_64V_NOMINAL             1 << 1 // T1 test for 64V passed or agree to request
      #define PWR_CONTROL_SOLENOID_NOMINAL        1 << 2 // Agree to solenoid request
      #define PWR_CONTROL_BOTH_NOMINAL            (PWR_CONTROL_24V_NOMINAL | PWR_CONTROL_64V_NOMINAL)
// T1 test for 24V and 64V passed or agree to request
// possible responses when disagreeing with a power request (SafetyTestDone)
#define PWR_CONTROL_24V_IGNORED             1 << 3   // power request ignored
      #define PWR_CONTROL_64V_IGNORED             1 << 4
      #define PWR_CONTROL_SOLENOID_IGNORED        1 << 5
      #define PWR_CONTROL_BOTH_IGNORED            (PWR_CONTROL_24V_IGNORED | PWR_CONTROL_64V_IGNORED)

/*++++++++++++++++++++++++++++++++++++++++++++++++
        RequestSafetyPower Defines : End
++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*++++++++++++++++++++++++++++++++++++++++++++++++
        MidCycleSwitch Defines : Start
++++++++++++++++++++++++++++++++++++++++++++++++++*/
// possible values for mid-cycle switch.
// allows procedure to direct safety to switch to draw or switch to return
// without seeing a reservoir sensor event.

enum NEW_CYCLE
{
   SWITCH_TO_DRAW  =1,
   SWITCH_TO_RETURN=2
};

/*++++++++++++++++++++++++++++++++++++++++++++++++
        MidCycleSwitch Defines : End
++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*++++++++++++++++++++++++++++++++++++++++++++++++
        TBV Defines : Start
++++++++++++++++++++++++++++++++++++++++++++++++++*/

// SPECIFICATION:    structure associated with focusBufferMsg<SDonorVitals>
//
// ERROR HANDLING:   none.

// predefined char buffer sizes for SDonorVitals struct
#define DONOR_NAME_LENGTH                  129
#define DONOR_DOB_LENGTH                     9

struct SDonorVitals
{

   char          cDonorVitalsStatus;                   // gui will load this with either DONOR_VITAL_INCOMPLETE | DONOR_VITAL_NOMINAL_COMPLETE | DONOR_VITAL_OPERATOR_COMFIRMATION
   char          cDonorInfoStatus;                     // gui will load this with either DONOR_INFO_DEFAULT | DONOR_INFO_NOMINAL_COMPLETE | DONOR_INFO_OPERATOR_COMFIRMATION | PFR_DONOR_DATA
   char          cDonorDownloadStatus;                 // gui will load this with either DONOR_DOWNLOAD_DEFAULT | DONOR_DOWNLOAD_ALL_AP2AGENT_VALUES | DONOR_DOWNLOAD_OPERATOR_COMMIT | DONOR_DOWNLOAD_OPERATOR_REJECT
   char          cDonorSex;                            // donor sex
   char          cDonorHeightUnits;                    // height units ( supplied by GUI from configuration )
   char          cDonorWeightUnits;                    // weight units ( supplied by GUI from configuration )
   char          cDonorRbcCount;                       // RBC units (supplied by GUI from configuration )
   char          cDonorDateofBirth [DONOR_DOB_LENGTH]; // donor date of birth in (YYYYMMDD) format, 8 characters
   char          cDonorName [DONOR_NAME_LENGTH];       // donor name for display, 128 characters in UTF8 format
   float         fDonorWeight;                         // donor weight
   float         fDonorHeight;                         // donor height
   float         fDonorHct;                            // donor Hct
   float         fDonorPltPrecount;                    // donor platelet pre-count
   float         fDonorSampleVolume;                   // Donor Sample Bag Volume
   float         fProcDonorTBVOgawa;                   // donor TBV in ml calculated by procedure via Ogawa algorithm (cannot be used by safety!)
   float         fProcDonorTBV;                        // donor TBV in ml via Nadler calculation
   float         fSafetyDonorTBV;                      // donor TBV in ml calculated by safety
   unsigned long DonorDataCRC;                         // donor bitmap CRC value
   BLOODTYPE     DonorBloodType;                       // Donor Blood Type (bloodtype.h)

};

DataLog_Stream& operator << (DataLog_Stream& os, const SDonorVitals& vitals);



// TBV Defines Common to GUI,Procedure,and Safety

// Possible Values for cDonorSex
#define DONOR_SEX_UNKNOWN                 0      // INIT VALUE for donor sex prior to operator indication
#define DONOR_SEX_IS_MALE                 1      // operator indicates sex is male
#define DONOR_SEX_IS_FEMALE               2      // operator inidicates sex is female

// donor height units
#define HEIGHT_UNITS_UNKNOWN             -1         // INIT VALUE prior to operator indication
#define HEIGHT_UNITS_CM                   0         // donor height in centimeters
#define HEIGHT_UNITS_IN                   1         // donor height in inches

// donor weight units
#define WEIGHT_UNITS_UNKNOWN             -1         // INIT VALUE prior to operator indication
#define WEIGHT_UNITS_KG                   0         // donor weight in kilograms
#define WEIGHT_UNITS_LB                   1         // donor weight in pounds

// donor weight units
#define RBC_COUNT_UNKNOWN             -1         // INIT VALUE prior to operator indication
#define RBC_COUNT_HEMATOCRIT           0         // donor rbc count in hematocrit
#define RBC_COUNT_HEMOGLOBIN           1         // donor rbc count in hemaglobin

// Range Extrema for Height, Weight, and TBV
#define MIN_ALLOWABLE_WEIGHT_LB          50.f                                  // FS User Specification UIS-1070 min weight in lbs
#define MAX_ALLOWABLE_WEIGHT_LB         500.f                                  // FS User Specification UIS-1070 max weight in lbs
#define MIN_ALLOWABLE_WEIGHT_KG         MIN_ALLOWABLE_WEIGHT_LB / LB_PER_KG    // FS User Specification UIS-1070 min kg derived
#define MAX_ALLOWABLE_WEIGHT_KG         MAX_ALLOWABLE_WEIGHT_LB / LB_PER_KG    // FS User Specification UIS-1070 max kg derived

#define MIN_ALLOWABLE_HEIGHT_IN          48.f                                 // FS User Specification UIS-1060 min height in inches
#define MAX_ALLOWABLE_HEIGHT_IN          96.f                                 // FS User Specification UIS-1060 max height in inches
#define MIN_ALLOWABLE_HEIGHT_CM         MIN_ALLOWABLE_HEIGHT_IN * CM_PER_IN   // FS User Specification UIS-1070 min cm derived
#define MAX_ALLOWABLE_HEIGHT_CM         MAX_ALLOWABLE_HEIGHT_IN * CM_PER_IN   // FS User Specification UIS-1070 max cm derived

#define MIN_ALLOWABLE_TBV_FEMALE        1578.f                            // see c:\saf_foc\misc_support\fs1.xls for computations
#define MAX_ALLOWABLE_TBV_FEMALE       12845.f                            // calculations performed using in-lb units and eqns reference listed in comments.
#define MIN_ALLOWABLE_TBV_MALE          1999.f                            // ditto
#define MAX_ALLOWABLE_TBV_MALE         13223.f                            // ditto

// Range Extrema for Donor Hct and Platelet Precount
#define MIN_ALLOWABLE_HCT               30.f
#define MAX_ALLOWABLE_HCT               55.4f
#define MIN_ALLOWABLE_HGB               10.0f
#define MAX_ALLOWABLE_HGB               18.4f
#define MIN_ALLOWABLE_PRECOUNT          50.f
#define MAX_ALLOWABLE_PRECOUNT         600.f


// Possible values for cDonorVitalsStatus
#define DONOR_VITAL_INCOMPLETE               0                // INIT VALUE, as processing of the structure is ongoing, but no errors have been detected
#define DONOR_VITAL_NOMINAL_COMPLETE         1                // processing of all donor vitals including caluclated TBV is NOMINAL and COMPLETE
#define DONOR_VITAL_OPERATOR_COMFIRMATION    2                // operator has confirmed TBV data ( ht,wt,sex,TBV ), s/b supplied by GUI
#define DONOR_INFO_DEFAULT                   3                // INIT VALUE, as processing of the structure is ongoing, but no errors have been detected
#define DONOR_INFO_NOMINAL_COMPLETE          4                // processing of all donor info including hematocrit and platelet precount is NOMINAL and COMPLETE
#define DONOR_INFO_OPERATOR_COMFIRMATION     5                // operator has confirmed donor info data ( hct, plt precount ), s/b supplied by GUI
#define DONOR_DOWNLOAD_DEFAULT               6                // Incompleted SDonorVitals structure: some or all values are missing.
#define DONOR_DOWNLOAD_OPERATOR_COMMIT       7                // operator has commited donor download data
#define DONOR_DOWNLOAD_OPERATOR_REJECT       8                // operator has rejected donor download data
#define DONOR_DOWNLOAD_ALL_AP2AGENT_VALUES   10               // The SDonorVitals structure contains a full transciption of the donor parameters and Everest-calculated-TBV received from AP2Agent.

// 03/19/96
// only have a single error value for cDonorVitalsStatus eventhough there may be multiple error conditions
// ( DONOR_SEX_ERROR, HEIGHT_UNITS_ERROR, WEIGHT_UNITS_ERROR, DONOR_WEIGHT_ERROR, DONOR_HEIGHT_ERROR, DONOR_TBV_ERROR ).
// alarms will be generated by Safety for all of the error conditions....rather than just a single alarm.
// This field is superfluous, however, for completeness I have included it for now....
#define DONOR_VITAL_ERROR                   55                // there is an error in one or more of the donor vital fields.

/*++++++++++++++++++++++++++++++++++++++++++++++++
        TBV Defines : End
++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define DONOR_SAMPLE_VOLUME_INVALID -1.0

#ifdef __cplusplus
#include "message.h"
//
//
//   These classes included to allow differentiation of
//   SDonorVitals messages from different sources.
//
#define DONOR_MESSAGE(__name__) \
   class __name__\
      : public Message< SDonorVitals > \
   { \
   public: \
      __name__()\
         : Message<SDonorVitals>() { } \
      __name__(MessageBase::SendType t)\
         : Message<SDonorVitals>() { init(t); } \
      __name__(const CallbackBase &cb, \
               MessageBase::SendReceiveType t = MessageBase::SNDRCV_RECEIVE_ONLY)   \
         : Message<SDonorVitals>() { init(cb, t); } \
      virtual ~__name__() { }; \
   }

DONOR_MESSAGE(DVitalsFromGUI);
DONOR_MESSAGE(DVitalsFromProc);
DONOR_MESSAGE(DVitalsFromSafety);
DONOR_MESSAGE(DVitalsFromVista);

//
//
//   This number represents the instantaneous safety extracorporeal volume.  Changing this number changes
//   the maximum allowable limit in both safety and the proc-side safety mimic.  Number is percentage
//   of TBV using Nadler calculation.
//
static const float SAFETY_EC_LIMIT = 0.20f;

//
//
//   This number is the maximum allowable draw cycle in ml allowed by safety
//
static const float MAX_ALLOWABLE_SAFETY_DRAW_CYCLE_VOLUME = 75.0f; // from safety/hypovol.hpp

#endif  /* __cplusplus */

#endif  /* end SAF_PROC_HPP multiple include protection block    */

/* FORMAT HASH ecd2aa70cb669cb95f7460e4541b7f3c */
