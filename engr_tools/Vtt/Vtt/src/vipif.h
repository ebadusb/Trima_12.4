/*******************************************************************************
 *
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      vip.h
 *             Vista Interface Process process
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the interface between Vista and Trima version 5.
 *
 *
 * HISTORY:    $Log: vipif.h $
 * HISTORY:    Revision 1.55  2009/06/16 15:52:42Z  rm70006
 * HISTORY:    IT 9239.  Remove unused status fields
 * HISTORY:    Revision 1.54  2009/05/15 16:26:49Z  rm70006
 * HISTORY:    IT 9087.  Add new info to sw.dat interface.
 * HISTORY:    Revision 1.53  2009/04/20 19:23:21Z  rm70006
 * HISTORY:    IT 9087.  Add new sw.dat request message to the Vista Interface.
 * HISTORY:    Revision 1.52  2009/03/11 21:44:07Z  rm70006
 * HISTORY:    IT 8019.  Update IF file to fix TRALI and Label flags.
 * HISTORY:    Revision 1.51  2009/02/20 17:35:30Z  rm70006
 * HISTORY:    IT 8019.  Interim checkin for Jon to finish.
 * HISTORY:    Revision 1.50  2008/05/08 19:46:24Z  dslausb
 * HISTORY:    IT 7973 - Add config button for saline rinseback
 * HISTORY:    Revision 1.49  2008/01/23 17:34:34Z  dslausb
 * HISTORY:    IT 8435 - VxWorks 5.5 Checkin
 * HISTORY:    Revision 1.48  2007/03/22 14:46:06Z  rm70006
 * HISTORY:    IT 7807.  Add PCO support to Vista interface.
 * HISTORY:    Revision 1.49  2006/12/05 17:12:05Z  rm70006
 * HISTORY:    IT 7808.  Added missing scan category.
 * HISTORY:    Revision 1.48  2006/11/14 22:11:25Z  rm70006
 * HISTORY:    IT 7722.  Add missing status codes to interface.
 * HISTORY:    Revision 1.47  2006/09/13 21:48:44Z  rm70006
 * HISTORY:    IT 7722.  Added new items per Spec review.
 * HISTORY:    Revision 1.46  2006/08/23 15:43:57Z  rm70006
 * HISTORY:    IT 7599.  If PFR doesn't restore donor data, then allow Vista to download new donor.
 * HISTORY:    Revision 1.45  2006/08/14 20:15:27Z  rm70006
 * HISTORY:    Make max catalog entries 10.
 * HISTORY:    Revision 1.44  2006/08/14 20:06:31Z  rm70006
 * HISTORY:    IT 7429.  Add new PPC fields to Vista interface.
 * HISTORY:    Revision 1.43  2003/10/06 22:49:27Z  rm70006
 * HISTORY:    IT 5841.  Make decoding PIN message easier.
 * HISTORY:    Revision 1.42  2003/10/06 19:41:12Z  rm70006
 * HISTORY:    IT 5841.  Add bio signature to bio auth event message.
 * HISTORY:    Revision 1.41  2003/08/19 15:15:08Z  rm70006
 * HISTORY:    IT 6366.  Get rid of floats for product values to avoid rounding issue.  Add remaining time field to periodic status message.  removed AC infusion rate from periodic status.
 * HISTORY:    Revision 1.40  2003/08/14 16:15:00Z  rm70006
 * HISTORY:    IT 5841.  Add CFR11 functionality.
 * HISTORY:    Revision 1.39  2003/07/30 16:02:23Z  rm70006
 * HISTORY:    IT 6238.  Changed version string to be unique incase others are needed for broadcast message.
 * HISTORY:    Revision 1.38  2003/05/21 22:10:47Z  rm70006
 * HISTORY:    IT 5967.
 * HISTORY:    
 * HISTORY:    Change reason code logic.
 * HISTORY:    Revision 1.37  2003/05/14 15:33:16Z  rm70006
 * HISTORY:    change revision number to be string.
 * HISTORY:    Revision 1.36  2003/05/14 15:31:37Z  rm70006
 * HISTORY:    Add file revision number to help Vista managing the changes to the interface.
 * HISTORY:    Revision 1.35  2003/04/25 16:41:19Z  rm70006
 * HISTORY:    IT 5841.
 * HISTORY:    
 * HISTORY:    Change #defines to enums to help Vista.
 * HISTORY:    Revision 1.34  2003/04/22 16:58:38Z  rm70006
 * HISTORY:    IT 5831.
 * HISTORY:    
 * HISTORY:    Add pragma for Vista to remove annoying warnings.
 * HISTORY:    Revision 1.33  2003/04/07 23:20:11Z  rm70006
 * HISTORY:    IT 5841.
 * HISTORY:    
 * HISTORY:    Remove datalog reference for Vista.
 * HISTORY:    Revision 1.32  2003/04/07 23:12:59Z  rm70006
 * HISTORY:    IT 5841.
 * HISTORY:    
 * HISTORY:    Add biometric info.
 * HISTORY:    Revision 1.31  2003/04/03 20:26:01Z  rm70006
 * HISTORY:    IT 5841.
 * HISTORY:    
 * HISTORY:    Add CFR Part 11 messages.
 * HISTORY:    Revision 1.30  2003/02/04 18:05:03Z  rm70006
 * HISTORY:    Add check for status messages.  Fix bug in prepare_msg
 * HISTORY:    Revision 1.29  2003/01/31 21:09:35Z  jl11312
 * HISTORY:    - updated for new datalog stream format
 * HISTORY:    Revision 1.28  2003/01/31 21:06:09  jl11312
 * HISTORY:    - update for new datalog stream format
 * HISTORY:    Revision 1.27  2003/01/30 17:51:19  rm70006
 * HISTORY:    Complete changes needed for 5.A interface.
 * HISTORY:    Revision 1.26  2003/01/27 14:55:58Z  rm70006
 * HISTORY:    Make changes for 5.X version of interface.
 * HISTORY:    Revision 1.25  2003/01/02 22:27:59Z  rm70006
 * HISTORY:    Added operator button push message (add-in from 5.0 baseline creep).
 * HISTORY:    Revision 1.24  2002/12/24 14:34:07Z  rm70006
 * HISTORY:    Update changes made from split from 5.0 baseline.
 * HISTORY:    Revision 1.23  2002/12/20 21:52:55Z  rm70006
 * HISTORY:    Update file.
 * HISTORY:    Revision 1.22  2002/10/28 21:19:58Z  rm70006
 * HISTORY:    Interim checkin.
 * HISTORY:    Revision 1.21  2002/04/09 16:56:39Z  rm70006
 * HISTORY:    Initial port to VxWorks comlete.  Code compiles.  Changed som field in header to be an unsigned int.
 * HISTORY:    Revision 1.20  2002/01/15 22:18:32Z  rm70006
 * HISTORY:    IT 5532.
 * HISTORY:    Made changes to allow Vista group to reuse code.
 * HISTORY:    Revision 1.19  2001/12/05 23:18:23Z  rm70006
 * HISTORY:    IT 5151.
 * HISTORY:    Added barcode feedback mesage to interface.
 * HISTORY:    Revision 1.18  2001/10/05 17:54:04  rm70006
 * HISTORY:    It 5352.
 * HISTORY:    Fixed bug where barcode length-of constant was pointing to itself 
 * HISTORY:    instead of the message struct.
 * HISTORY:    Revision 1.17  2001/10/04 19:05:44  rm70006
 * HISTORY:    IT 5349.
 * HISTORY:    Changed Actual HCT to float in End Run Summary.
 * HISTORY:    
 * HISTORY:    IT 4608.
 * HISTORY:    Add new status code for multiple donor download.
 * HISTORY:    Revision 1.16  2001/10/02 21:14:58  rm70006
 * HISTORY:    IT 4766.
 * HISTORY:    Changed interface to match internal types in offline config.
 * HISTORY:    Revision 1.15  2001/10/02 15:21:41  rm70006
 * HISTORY:    IT 4557, 4560, 4562.
 * HISTORY:    Fixed problems with Offline - Config issues.  GUI uses a different
 * HISTORY:    system to check for changes than the rest of Trima.  Had to
 * HISTORY:    make many changes to accommodate that.  GUI should be fixed in
 * HISTORY:    next release.
 * HISTORY:    Revision 1.14  2001/09/19 16:04:45  rm70006
 * HISTORY:    IT 4652.
 * HISTORY:    Added donor sample volume
 * HISTORY:    
 * HISTORY:    IT 4766.
 * HISTORY:    Fixed bug with End Run Summary message.
 * HISTORY:    Revision 1.13  2001/09/10 19:23:53  rm70006
 * HISTORY:    IT 4766.
 * HISTORY:    Added code to ensure that message contents would not be changed.
 * HISTORY:    Revision 1.12  2001/09/07 15:54:57  rm70006
 * HISTORY:    IT 4766.
 * HISTORY:    Fix bugs found during testing.
 * HISTORY:    Revision 1.11  2001/08/30 16:18:00  rm70006
 * HISTORY:    It 4766.
 * HISTORY:    Made changes to interface to match redlines from interface spec.
 * HISTORY:    Revision 1.10  2001/08/27 21:24:03  rm70006
 * HISTORY:    IT 4766.
 * HISTORY:    Added new field to periodic status message.
 * HISTORY:    Added some new types for the event messages.
 * HISTORY:    Revision 1.9  2001/08/23 18:53:22  rm70006
 * HISTORY:    IT 4766
 * HISTORY:    Added PFR status message.
 * HISTORY:    Revision 1.8  2001/08/23 16:32:41  rm70006
 * HISTORY:    IT 4766.
 * HISTORY:    Added some message id's.
 * HISTORY:    Revision 1.7  2001/08/22 14:16:47  rm70006
 * HISTORY:    IT 4766.
 * HISTORY:    Changed SOM token per spec.
 * HISTORY:    Changed routine that does checksum.  One more change will be 
 * HISTORY:    required to complete this change, however.
 * HISTORY:    Revision 1.6  2001/08/15 16:51:49  rm70006
 * HISTORY:    IT 4766.
 * HISTORY:    Change definintion of Request File List Msg.
 * HISTORY:    Removed more magic numbers.
 * HISTORY:    Revision 1.5  2001/08/10 20:05:25  rm70006
 * HISTORY:    Change ID's of Status messages to be different.
 * HISTORY:    Revision 1.4  2001/08/08 19:23:00  rm70006
 * HISTORY:    It 4766.
 * HISTORY:    Add bug fixes for Donor Download and Offline Config.
 * HISTORY:    Revision 1.3  2001/08/06 17:17:54  rm70006
 * HISTORY:    IT 4766.
 * HISTORY:    Removed magic numbers from IF file.
 * HISTORY:    Revision 1.2  2001/07/25 19:49:04  rm70006
 * HISTORY:    IT 4766
 * HISTORY:    Check in interim version of code for Kent to check out in lab.
 *
 ******************************************************************************/
#ifndef __VIPIF__
#define __VIPIF__


#ifndef WIN32

#include <sys/types.h>
#include <time.h>
#include "datalog.h"

#else

#pragma warning(disable : 4091)  // Added to remove Microsofts warning about "typedef struct typename"

#endif


const char VISTA_IF_VERSION_NUMBER[] = "$Revision: 1.55 $";


// Define port that VIP_CMD listens on.
const int VIP_LISTEN_PORT = 7010;

// Define the SOM value
const unsigned int VIP_SOM_ID = (0xCAFEBABE);

typedef struct trimaVistaHeader
{
   unsigned int  som;               // Start of Message identifier
   int           messageType;       // Message Type
   int           sequenceNumber;    // Number generated by Vista to correlate commands to responses
   int           status;            // Used by Trima only.  Indicates success or failure of commands
   int           lengthInBytes;     // Length of body message in bytes (excludes header)
   int           timestamp;         // Time message was sent (used for recording / playback only).
   unsigned long bodyCrc;           // CRC computed against entire message (header + body)
   unsigned long headerCrc;         // CRC computed against message header.
};


const int trimaVistaHeaderLength = sizeof (trimaVistaHeader);


const int VIP_OK                 =  1;
const int VIP_BAD_START_OF_MSG   =  2;
const int VIP_HEADER_CRC_INVALID =  3;
const int VIP_MSG_CRC_INVALID    =  4;
const int VIP_BAD_LENGTH         =  5;
const int VIP_NOT_PRIVILEGED     =  6;
const int VIP_PFR_NOT_COMPLETE   =  7;
const int VIP_MESSAGE_ID_INVALID =  8;
const int VIP_BUFFER_UNDERRUN    =  9;
const int VIP_BUFFER_OVERRUN     = 10;
const int VIP_BAD_VALUE          = 11;
const int VIP_ILLEGAL_FORMAT     = 12;
const int VIP_FAILED_TO_COMMIT   = 13;
const int VIP_DONOR_DOWNLOADED   = 14;
const int VIP_BAD_CATALOG_ENTRY  = 15;


const int VIP_MAX_TRIMA_MSG_SIZE = 8192;

const int trimaVistaMsgLength = VIP_MAX_TRIMA_MSG_SIZE - sizeof(trimaVistaHeader);

typedef char trimaVistaBuffer[trimaVistaMsgLength];

//
// Define the standard message header
//
typedef struct trimaVistaMsg
{
   trimaVistaHeader hdr;
   trimaVistaBuffer buf;
};



//
// Command Messages
//
const int VIP_FIRST_CMD_MSG                 = 101;
const int VIP_SET_LOCAL_TIME                = VIP_FIRST_CMD_MSG;
const int VIP_START_PERIODIC_STATUS         = 102;
const int VIP_SET_OFFLINE_CONFIG            = 103;
const int VIP_REQUEST_FILE_LIST             = 104;
const int VIP_REQUEST_FILE_LIST_BY_SN       = 105;
const int VIP_UPLOAD_FILE                   = 106;
const int VIP_DOWNLOAD_DONOR_INFO           = 107;
const int VIP_PING                          = 108;
const int VIP_BARCODE_RESPONSE              = 109;
const int VIP_INTERFACE_STATE               = 110;
const int VIP_START_MACHINE_STATUS          = 111;
const int VIP_DOWNLOAD_BIOMETRIC_SIGNATURES = 112;
const int VIP_BIOMETRIC_SIGNATURE_RESPONSE  = 113;
const int VIP_DOWNLOAD_CATALOG_INFO         = 114;
const int VIP_REQUEST_OPT_SETTING_VALUES    = 115;
const int VIP_LAST_CMD_MSG                  = VIP_REQUEST_OPT_SETTING_VALUES;



//
// Common Message definitions.
//

const int VIP_MAX_DONOR_NAME       =   129;
const int VIP_DATE_LENGTH          =     8;
const int VIP_BITMAP_SIZE          = 40294; /* Bitmap 171x228 pixels (256 colors)*/
const int VIP_MAX_VISTA_PROCEDURES =    50;


const int VIP_LANG_ENGLISH    = 0;
const int VIP_LANG_FRENCH     = 1;
const int VIP_LANG_GERMAN     = 2;
const int VIP_LANG_ITALIAN    = 3;
const int VIP_LANG_JAPANESE   = 4;
const int VIP_LANG_SPANISH    = 5;
const int VIP_LANG_DUTCH      = 6;
const int VIP_LANG_PORTUGUESE = 7;
const int VIP_LANG_SWEDISH    = 8;
const int VIP_LANG_KOREAN     = 9;

const int VIP_UNITS_IN = 1;
const int VIP_UNITS_CM = 0;

const int VIP_UNITS_LB = 1;
const int VIP_UNITS_KG = 0;

const int VIP_DATE_MMDDYY = 0;
const int VIP_DATE_DDMMYY = 1;
const int VIP_DATE_YYMMDD = 2;

const int VIP_TIME_24H = 0;
const int VIP_TIME_12H = 1;

const int VIP_DELIM_COMMA   = 0;
const int VIP_DELIM_DECIMAL = 1;

const int VIP_AUDIO_HIGH = 0;
const int VIP_AUDIO_MED  = 1;
const int VIP_AUDIO_LOW  = 3;

const int VIP_RBC_HEMATOCRIT = 1;
const int VIP_RBC_HEMAGLOBIN = 2;

const int VIP_BLOOD_UNKNOWN =   0;
const int VIP_BLOOD_O_NEG   =   2;
const int VIP_BLOOD_O_POS   =   4;
const int VIP_BLOOD_AB_NEG  =   8;
const int VIP_BLOOD_AB_POS  =  16;
const int VIP_BLOOD_A_NEG   =  32;
const int VIP_BLOOD_A_POS   =  64;
const int VIP_BLOOD_B_NEG   = 128;
const int VIP_BLOOD_B_POS   = 256;
const int VIP_BLOOD_ALL     = 510;

const int VIP_TBV_PERCENT = 1;
const int VIP_TBV_GTLT    = 2;
const int VIP_TBV_MLKG    = 4;

const int VIP_DRAW_SLOW = 0;
const int VIP_DRAW_MED  = 1;
const int VIP_DRAW_FAST = 2;

const int VIP_TRALI_MALE_ONLY      = 0;
const int VIP_TRALI_MALE_AB_FEMALE = 1;
const int VIP_TRALI_ALL            = 2;

//
// Set Local Time Msg
//
typedef struct setLocalTimeMsg
{
   trimaVistaHeader  hdr;
   time_t            localTime;   // Local Time in UTC.
};

const int setLocalTimeMsgSize = sizeof (setLocalTimeMsg);


//
// Start Periodic Status Msg
//
typedef struct startPeriodicStatusMsg
{
   trimaVistaHeader  hdr;
   int               repeatPeriod;
   unsigned int      portNumber;
   unsigned int      receiveTimeout;
};

const int startPeriodicStatusMsgSize = sizeof (startPeriodicStatusMsg);

   

//
// Start Machine Status Msg
//
typedef struct startMachineStatusMsg
{
   trimaVistaHeader hdr;
   int              repeatPeriod;
   int              portNumber;
};

const int startMachineStatusMsgSize = sizeof (startMachineStatusMsg);

   


typedef struct rbcCritDoseType
{
   float rbcHematocrit;
   float rbcDose;
   int   meterRas;
   int   rasVolume;
};

typedef struct plateletYVType
{
   float plateletYield;
   float plateletVolume;
   int   meterPAS;
   int   pco;
};

typedef struct procedureType
{
   int   platelet;
   int   plasma;
   int   rbc;
   int   bloodType;
   float maxProcedureTime;
};

//
// Set Offline Configuration Msg
//
const int VIP_MAX_CRIT_DOSE      =  3;
const int VIP_MAX_PLATELETYV     = 10;
const int VIP_MAX_PLASMA_VOLUME  =  6;
const int VIP_MAX_PROCEDURE_LIST = 20;


typedef struct setOfflineConfigurationMsg
{
   trimaVistaHeader  hdr;
   int               languageOnTrima;
   int               heightUnits;
   int               weightUnits;
   int               dateFormat;
   int               timeFormat;
   int               decimalDelimeter;
   int               audioLevel;
   int               rbcMeasurementType;
   int               maximumProcedureTime;
   float             maximumReturnPressure;
   float             maximumDrawPressure;
   float             customRatio;
   int               acRate;
   float             postHematocrit;
   int               tbvSetting;
   float             tbvPercent;
   float             weightSetting;
   float             noLessThanVolume;
   float             noGreaterThanVolume;
   float             mlPerKg;
   float             drbcTbvLimit;
   float             postProcedurePlateletCount;
   int               maxDrawFlow;
   int               minReplacementVolume;
   int               plateletDivertPrompts;
   int               bloodDivertPrompts;
   int               solutionsBagVolOverride;
   int               drbcSplitNotification;
   int               drbcThreshold;
   int               plasmaRinseback;
   int               salineRinseback;
   int               auditTrailLevel;
   int               airRemovalEnabled;
   int               pasEnabled;
   int               rasEnabled;
   int               showPreAasFlags;
   int               pasDefaultBagVolume;
   int               rasDefaultBagVolume;
   int               rbcRatio;
   int               rbcReplacementFluid;
   float             rbcFluidPercent;
   int               amapMin;
   int               amapMax;
   float             ysf;
   int               guaranteedYield;
   float             coefficientOfVariation;
   int               confidenceLevel;
   int               inletFlowRamp;
   int               inletManagement;
   int               returnManagement;
   float             rsf;
   int               maleOnlyPlatelet;
   int               maleOnlyPlasma;
   rbcCritDoseType   critDose[VIP_MAX_CRIT_DOSE];
   plateletYVType    plateletYV[VIP_MAX_PLATELETYV];
   float             plasmaVolume[VIP_MAX_PLASMA_VOLUME];
   procedureType     procedureList[VIP_MAX_PROCEDURE_LIST];
};

const int setOfflineConfigurationMsgSize = sizeof (setOfflineConfigurationMsg);


//
// Request File List Msg
//

// Max defined by QNX
const int VIP_MAX_PATHNAME_LENGTH = 256;
const int VIP_MAX_PATTERN_LENGTH  =  49;

typedef struct requestFileListMsg
{
   trimaVistaHeader  hdr;
   char              pathName[VIP_MAX_PATHNAME_LENGTH];
   char              pattern[VIP_MAX_PATTERN_LENGTH];
};

const int requestFileListMsgSize = sizeof (requestFileListMsg);


const int VIP_MAX_SEQUENCE_NUMBER = 999999;

//
// Request File List Msg By Sequence Number
//
typedef struct requestFileListBySeqNumMsg
{
   trimaVistaHeader  hdr;
   char              directoryName[VIP_MAX_PATHNAME_LENGTH];
   int               sequenceNumber;
};

const int requestFileListBySeqNumMsgSize = sizeof (requestFileListBySeqNumMsg);


//
// Upload Log File Msg
//

const int VIP_MAX_FILENAME_LENGTH = 81;

typedef struct uploadFileMsg
{
   trimaVistaHeader  hdr;
   char              fileName[VIP_MAX_FILENAME_LENGTH];
};

const int uploadFileMsgSize = sizeof (uploadFileMsg);


//
// Donor Info Msg
//

const int VIP_HEIGHT_UNITS_IN = 1;
const int VIP_HEIGHT_UNITS_CM = 2;

const int VIP_WEIGHT_UNITS_LB = 1;
const int VIP_WEIGHT_UNITS_KG = 2;

const int VIP_DONOR_GENDER_MALE   = 1;
const int VIP_DONOR_GENDER_FEMALE = 2;

const int VIP_VOLUME_REMOVAL_TBV = 1;
const int VIP_VOLUME_REMOVAL_WEIGHT = 2;
const int VIP_VOLUME_REMOVAL_ML_PER_KG = 4;
const int VIP_VOLUME_REMOVAL_DRBC_BODY_VOL = 8;


typedef struct procedureGoalType
{
   float    plateletYield;
   float    plateletVolume;
   int      meterPAS;
   int      pco;
   float    plasmaVolume;
   float    rbcDose;
   float    rbcHematocrit;
   int      meterRas;
   int      rasVolume;
   int      bloodType;
   float    maxProcedureTime;
   int      designator;
};



typedef struct donorInfoMsg
{
   trimaVistaHeader  hdr;
   // Donor Vital's section 
   char              donorName[VIP_MAX_DONOR_NAME];
   char              donorDOB[VIP_DATE_LENGTH + 1];      // (YYYYMMDD) format
   char              donorGender;
   char              filler01;
   float             donorHeight;
   float             donorWeight;
   int               donorBloodType;
   float             donorHematocrit;
   float             donorPrecount;
   float             donorSampleVolume;
   float             donorTBV;

   // Trima Configuration section 
   int               heightUnits;
   int               weightUnits;
   int               languageOnTrima;
   int               dateFormat;
   int               timeFormat;
   int               decimalDelimeter;
   int               audioLevel;
   int               rbcMeasurementType;
   int               maximumDonorTime;
   int               maximumProcedureTime;
   float             maximumReturnPressure;
   float             maximumDrawPressure;
   float             customRatio;
   int               acRate;
   float             postHematocrit;
   int               tbvSetting;
   float             tbvPercent;
   float             weightSetting;
   float             noLessThanVolume;
   float             noGreaterThanVolume;
   float             mlPerKg;
   float             drbcTbvLimit;
   float             postProcedurePlateletCount;
   int               maxDrawFlow;
   int               minReplacementVolume;
   int               plateletDivertPrompts;
   int               bloodDivertPrompts;
   int               solutionsBagVolOverride;
   int               drbcSplitNotification;
   int               drbcThreshold;
   int               plasmaRinseback;
   int               salineRinseback;
   int               auditTrailLevel;
   int               airRemovalEnabled;
   int               pasEnabled;
   int               rasEnabled;
   int               showPreAasFlags;
   int               pasDefaultBagVolume;
   int               rasDefaultBagVolume;
   int               rbcRatio;
   int               rbcReplacementFluid;
   float             rbcFluidPercent;
   int               amapMin;
   int               amapMax;
   float             ysf;
   int               guaranteedYield;
   float             coefficientOfVariation;
   int               confidenceLevel;
   int               inletFlowRamp;
   int               inletManagement;
   int               returnManagement;
   float             rsf;
   int               maleOnlyPlatelet;
   int               maleOnlyPlasma;

   // Procedure List section
   int               numberOfProcedures;
   procedureGoalType procedureList[VIP_MAX_VISTA_PROCEDURES];
   
   unsigned char     donorPicture[VIP_BITMAP_SIZE];  // Bitmap 171x228 pixels (256 colors)
};

const int donorInfoMsgSize = sizeof (donorInfoMsg);




const int VIP_BARCODE_FIRST                      = 1;
const int VIP_BARCODE_VALID                      = VIP_BARCODE_FIRST;
const int VIP_BARCODE_INVALID                    = 2;
const int VIP_BARCODE_WRONG_CATEGORY             = 3;
const int VIP_BARCODE_PROCEDURE_DATA_CONSISTENCY = 4;
const int VIP_BARCODE_LAST                       = VIP_BARCODE_PROCEDURE_DATA_CONSISTENCY;

//
// Barcode Invalid Msg
// This message is sent by Vista when they determine that a
// barcoded item is invalid.
//
typedef struct barcodeFeedbackMsg
{
   trimaVistaHeader  hdr;
   int               scanCategory;
   int               scanSequence;
   int               reasonCode;       // action code for Trima
};

const int barcodeFeedbackMsgSize = sizeof (barcodeFeedbackMsg);



//
// Interface State Msg
//
typedef struct getInterfaceStateMsg
{
   trimaVistaHeader hdr;
};

const int getInterfaceStateMsgSize = sizeof (getInterfaceStateMsg);


typedef struct interfaceStateMsg
{
   trimaVistaHeader hdr;
   int             privilegedState;
   int             donorDownloaded;
   int             pfrStatus;
   int             originalSequenceNumber;
   int             biometricConfig;
};


const int interfaceStateMsgSize = sizeof (interfaceStateMsg);


//
// Download Biometric Signatures
//


const int VIP_BIO_MAX_BADGE_LENGTH = 81; // Includes null char

const int VIP_BIO_MAX_PIN_LENGTH = 7;   // Includes null char

const int VIP_BIO_MAX_SIGNATURES = 1000;


typedef struct downloadBiometricSignaturesMsg
{
   trimaVistaHeader hdr;
   int              biometricDeviceId;
   int              numberOfSignatures;
   union
   {
      char          signatureSection[1];   // Placeholder for start of biometric signatures
      int           temp1;                 // Used to enforce consistent sizing between platforms.
   };
};

const int downloadBiometricSignaturesMsgSize = sizeof (downloadBiometricSignaturesMsg);



//
// Biometric Signature Response
//

const int VIP_BIO_SIGNATURE_INVALID   = 0;
const int VIP_BIO_SIGNATURE_VALID     = 1;
const int VIP_BIO_SIGNATURE_VALID_ADD = 2;

typedef struct biometricSignatureResponseMsg
{
   trimaVistaHeader hdr;
   int              biometricDeviceId;
   int              scanSequenceNumber;
   int              response;
   union
   {
      char          signatureSection[1];   // Placeholder for start of biometric signatures
      int           temp1;                 // Used to enforce consistent sizing between platforms.
   };
};

const int biometricSignatureResponseMsgSize = sizeof (biometricSignatureResponseMsg);


//
// Catalog Number Download Request
//

const int VIP_CATALOG_BARCODE_SIZE = 80;
const int VIP_CATALOG_LIST_SIZE    = 10;

typedef struct catalogItem
{
   int  catalogNumber;
   int  tubingSetCode;
   char barcode[VIP_CATALOG_BARCODE_SIZE];
};


typedef struct downloadCatalogListMsg
{
   trimaVistaHeader hdr;
   int              numberOfItems;
   catalogItem      itemList[VIP_CATALOG_LIST_SIZE];   
};

const int downloadCatalogListMsgSize = sizeof (downloadCatalogListMsg);


typedef struct getOptionalSettingsValuesMsg
{
   trimaVistaHeader hdr;
};

const int getOptionalSettingsValuesMsgSize = sizeof (getOptionalSettingsValuesMsg);


typedef struct optionalSettingsValuesMsg
{
   trimaVistaHeader hdr;
   int confirmAcConnectionPrompt;
   int dontSealSampleBagPrompt;
   int dontConnectAcAtSetLoadPrompt;
   int displayResidualVolume;
   int allowFlowAdjustments;
   int alternateNextProcedureButton;
   int enableDisplayingOgawaTbv;
   int fourWayVolumeCutoff;
   int displayPlateletVolume;
   int postcountiSpleenMobilizationEnabled;
   int lockTrimaConfig;
   int lowPlateletConcentration;
   int highPlateletConcentration;
   int rbcMaximumProductVolume;
   int maximumPasMeteringDuration;
};


const int optionalSettingsValuesMsgSize = sizeof(optionalSettingsValuesMsg);



//
// Status Messages
//
const int VIP_FIRST_STATUS_MSG                  = 201;
const int VIP_BARCODE_CONTENTS_MSG              = VIP_FIRST_STATUS_MSG;
const int VIP_POWER_FAIL_RECOVER_MSG            = 202;
const int VIP_PROCEDURE_PARAMETER_ADJ_MSG       = 203;
const int VIP_DONOR_VITALS_UPDATED_MSG          = 204;
const int VIP_END_RUN_SUMMARY_MSG               = 205;
const int VIP_ALARM_MSG                         = 206;
const int VIP_ALARM_ACK_MSG                     = 207;
const int VIP_PERIODIC_STATUS_MSG               = 208;
const int VIP_DONOR_COMMIT_MSG                  = 209;
const int VIP_DONOR_REJECTED_MSG                = 210;
const int VIP_OPERATOR_BUTTON_PUSH_MSG          = 212;
const int VIP_MACHINE_STATUS_MSG                = 213;
const int VIP_BIOMETRIC_AUTHORIZATION_EVENT_MSG = 214;
const int VIP_BIOMETRIC_LOG_EVENT_MSG           = 215;
const int VIP_LAST_STATUS_MSG                   = VIP_BIOMETRIC_LOG_EVENT_MSG;


//
// Barcode Verification Msg
// This message is sent from Trima to Vista when an item is
// barcoded.
//

const int VIP_SCAN_STRING_LENGTH = 120;


const int VIP_SCAN_CATEGORY_FIRST             =  0;
const int VIP_SCAN_CATEGORY_OPERATOR          =  VIP_SCAN_CATEGORY_FIRST;
const int VIP_SCAN_CATEGORY_DONOR             =  1;
const int VIP_SCAN_CATEGORY_TUBING_SET        =  2;
const int VIP_SCAN_CATEGORY_DONATION_ID       =  3;
const int VIP_SCAN_CATEGORY_AC                =  4;
const int VIP_SCAN_CATEGORY_REPLACEMENT_FLUID =  5;
const int VIP_SCAN_CATEGORY_SAMPLE_BAG        =  6;
const int VIP_SCAN_CATEGORY_NOTE              =  7;
const int VIP_SCAN_CATEGORY_MISC1             =  8;
const int VIP_SCAN_CATEGORY_MISC2             =  9;
const int VIP_SCAN_CATEGORY_MISC3             = 10;
const int VIP_SCAN_CATEGORY_MISC4             = 11;
const int VIP_SCAN_CATEGORY_RBC_STORAGE_SOL   = 12;
const int VIP_SCAN_CATEGORY_PLT_STORAGE_SOL   = 13;
const int VIP_SCAN_CATEGORY_DATA              = 14;
const int VIP_SCAN_CATEGORY_NONE              = 15;
const int VIP_SCAN_CATEGORY_LAST              = VIP_SCAN_CATEGORY_NONE;


const int VIP_SCAN_STATUS_ADDED   = 0;
const int VIP_SCAN_STATUS_DELETED = 1;


typedef struct barcodeContentsMsg
{
   trimaVistaHeader  hdr;
   int               scanCategory;
   int               scanSequence;
   int               scanStatus;
   int               scanStringLength;
   char              scanString[VIP_SCAN_STRING_LENGTH];
   char              scanSymbol;
};


const int barcodeContentsMsgLength = sizeof (barcodeContentsMsg);



const int PFR_FAILED_OR_NO_PFR      = 0;
const int PFR_COMPLETE_ALL_RESTORED = 1;
const int PFR_COMPLETE_NOT_RESTORED = 2;


//
// Power Fail Recovery Indication Msg
// This message tells Vista whether or not Trima performed a PFR.
//
typedef struct powerFailRecoveryIndicationMsg
{
   trimaVistaHeader  hdr;
   int               powerFailPerformed;
};

const int powerFailRecoveryIndicationMsgLength = sizeof (powerFailRecoveryIndicationMsg);



//
// Alarm Msg
//
typedef struct alarmMsg
{
   trimaVistaHeader  hdr;
   int               alarmId;
};

const int alarmMsgLength = sizeof (alarmMsg);



const int VIP_RESPONSE_ENDRUN    = 0;
const int VIP_RESPONSE_RINSEBACK = 1;
const int VIP_RESPONSE_CONTINUE  = 2;
const int VIP_RESPONSE_CONFIRM   = 3;
const int VIP_RESPONSE_UNLOAD    = 4;
 

//
// Alarm Acknowledge Msg
//
typedef struct alarmAckMsg
{
   trimaVistaHeader  hdr;
   int               alarmId;
   int               alarmResponse;
};

const int alarmAckMsgLength = sizeof (alarmAckMsg);



//
// End Run Summary Msg
//
// As spec'ed against proc/algorithm/productqualityinformation.cpp
//   ProductQualityInformation::Initialize
//
const int VIP_MAX_REASONS = 21;

const int VIP_RINSEABCK_UNSUCCESSFUL      = 0;
const int VIP_RINSEBACK_SUCCESSFUL_NORMAL = 1;
const int VIP_RINSEBACK_SUCCESSFUL_PLASMA = 2;
const int VIP_RINSEBACK_SUCCESSFUL_SALINE = 3;






typedef struct endRunSummaryMsg
{
   trimaVistaHeader  hdr;
   int               totalAcUsed;
   int               ActualAcToDonor;
   int               bloodVolumeProcessed;
   int               bloodVolumeProcessedPlatelet;
   int               plateletVolumeActual;
   int               plateletYieldActual; 
   int               acVolumeInPlatelets;
   int               PASVolume;
   int               offlinePASVolume;
   int               plasmaActualVolume;
   int               acVolumeInPlasma;
   int               rbcIndividualProductReporting;
   int               rbc1ActualVolume;
   int               rbc2ActualVolume;
   int               acVolumeInRBC1;
   int               acVolumeInRBC2;
   int               rbc1RasVolume;
   int               rbc2RasVolume;
   struct tm         startofAasTime;
   int               offlineRas1Volume;
   int               offlineRas2Volume;
   float             rbc1ActualHematocrit;
   float             rbc2ActualHematocrit;
   int               postHematocrit;
   int               postPlateletCount;
   int               replacementFluidVolume;
   struct tm         endOfRunTime;
   int               lengthOfRunWithPauseRecover;
   int               lengthOfRunWithoutPauseRecover;
   int               labelPlateletsLeukoreduced;
   int               labelPlasmaLeukoreduced;
   int               labelRBC1Leukoreduced;
   int               labelRBC2Leukoreduced;
   int               numberOfRbc1VolumeReasons;
   int               rbc1VolumeReason[VIP_MAX_REASONS];
   int               numberOfRbc2VolumeReasons;
   int               rbc2VolumeReason[VIP_MAX_REASONS];
   int               numberOfRbc1WbcReasons;
   int               rbc1WbcReason[VIP_MAX_REASONS];
   int               numberOfRbc2WbcReasons;
   int               rbc2WbcReason[VIP_MAX_REASONS];
   int               numberOfRbc1QualityReasons;
   int               rbc1QualityReason[VIP_MAX_REASONS];
   int               numberOfRbc2QualityReasons;
   int               rbc2QualityReason[VIP_MAX_REASONS];
   int               numberOfPlasmaWbcReasons;
   int               plasmaWbcReason[VIP_MAX_REASONS];
   int               numberOfPlateletWbcReasons;
   int               plateletWbcReason[VIP_MAX_REASONS];
   int               numberOfPlateletConcentrationReasons;
   int               plateletConcentrationReason[VIP_MAX_REASONS];
   int               numberOfPlasmaVolumeReasons;
   int               plasmaVolumeReason[VIP_MAX_REASONS];
   int               numberOfPlateletVolumeReasons;
   int               plateletVolumeReason[VIP_MAX_REASONS];
   int               rinsebackCompleteFlag;
   int               packedRbcResidualInSet;
   int               plasmaResidualInSet;
};

const int endRunSummaryMsgLength = sizeof (endRunSummaryMsg);


//
// Procedure Parameter Adjustment
//
enum PROCEDURE_PARAMETER_ADJ_ENUMS
{
	VIP_ACCESS_PRESSURE_UP	=	0,
	VIP_ACCESS_PRESSURE_DOWN,
	VIP_RETURN_PRESSURE_UP,
	VIP_RETURN_PRESSURE_DOWN,
	VIP_TINGLING_UP,
	VIP_TINGLING_DOWN,
	VIP_CLUMPING_UP,
	VIP_CLUMPING_DOWN,
	VIP_SPILLOVER,
	VIP_AIR_IN_PLASMA_LINE,
	VIP_SALINE_BOLUS
};

typedef struct procedureParameterAdjustmentMsg
{
   trimaVistaHeader  hdr;
   int               adjustment;    // Enumerated type containing both parameter and direction.
};

const int procedureParameterAdjMsgLength = sizeof (procedureParameterAdjustmentMsg);


//
// Donor Vitals Updated
//
typedef struct donorVitalsUpdatedMsg
{
   trimaVistaHeader  hdr;
   int               donorVitalsStatus;
   int               donorInfoStatus;
   int               donorDownloadStatus;
   float             hematocrit;
   float             precount;
   float             height;
   float             weight;
   int               donorGender;
   float             procDonorTBV;
   float             safetyDonorTBV;
   int               donorBloodType;
};

const int donorVitalsUpdatedMsgLength = sizeof (donorVitalsUpdatedMsg);


const int VIP_MAX_SUBSTATE_LENGTH     = 32;
const int VIP_MAX_RECOVERSTATE_LENGTH = 32;


//
// Trima Periodic Status Msg
//
typedef struct trimaPeriodicStatusMsg
{
   trimaVistaHeader hdr;
   float            targetProcedureTime;
   float            currentProcedureTime;
   float            remainingProcedureTime;
   int              targetPlateletYield;
   int              currentPlateletYield;
   int              targetPlasmaVolume;
   int              currentPlasmaVolume;
   int              targetRbcVolume;
   int              currentRbcVolume;
   float            targetRbcCollectHct;
   float            currentRbcCollectHct;
   int              targetPlateletVolume;
   int              currentPlateletVolume;
   int              targetPASVolume;
   int              currentPASVolume;
   int              targetRAS1Volume;
   int              currentRAS1Volume;
   int              targetRAS2Volume;
   int              currentRAS2Volume;
   int              currentPlateletAcVolume;
   int              currentPlasmaAcVolume;
   int              currentRbc1AcVolume;
   int              currentRbc2AcVolume;
   int              totalBloodProcessed;
   int              donorTbv;
   int              trimaSystemState;
   char             subState[VIP_MAX_SUBSTATE_LENGTH + 1];
   char             recoveryState[VIP_MAX_RECOVERSTATE_LENGTH + 1];
   int              cassetteType;
   int              cassettePosition;
   int              selectedProcedure;
   int              catalogNumber;
};

const int trimaPeriodicStatusMsgLength = sizeof (trimaPeriodicStatusMsg);



//
// Individual Pump status
//
typedef struct pumpStatus
{
   float commandedRate;
   float actualRate;
   float accumulatedVolume;
};



//
// Machine Status Message
//
typedef struct trimaMachineStatusMsg
{
   trimaVistaHeader  hdr;
   pumpStatus        acPump;
   pumpStatus        inletPump;
   pumpStatus        plasmaPump;
   pumpStatus        plateletPump;
   pumpStatus        returnPump;
   pumpStatus        replacementPump;
};


const int trimaMachineStatusMsgLength = sizeof(trimaMachineStatusMsg);



//
// Operator button push.
//
typedef struct operatorButtonPushMsg
{
   trimaVistaHeader  hdr;
   short             screenId;
   short             buttonId;
};

const int operatorButtonPushLength = sizeof (operatorButtonPushMsg);



//
// Vista Donor Rejected Msg
//
typedef struct donorRejectedMsg
{
   trimaVistaHeader hdr;
};

const int donorRejectedMsgLength = sizeof(donorRejectedMsg);




//
// Vista Donor Committed
//
typedef struct donorCommittedMsg
{
   trimaVistaHeader hdr;
};

const int donorCommittedMsgLength = sizeof(donorCommittedMsg);



//
// Biometric Authorization Event
//

//
// Generic Definition
typedef struct biometricAuthorizationEventMsg
{
   trimaVistaHeader hdr;
   int              biometricDeviceId;
   int              scanSequenceNumber;
   char             badge[VIP_BIO_MAX_BADGE_LENGTH];
   union
   {
      char          signatureSection[1];   // Placeholder for start of biometric signatures
      int           temp1;                 // Used to enforce consistent sizing between platforms.
   };
};

const int biometricAuthorizationEventMsgSize = sizeof(biometricAuthorizationEventMsg);



//
// PIN Specific definition
typedef struct PinAuthorizationEventMsg
{
   trimaVistaHeader hdr;
   int              biometricDeviceId;
   int              scanSequenceNumber;
   char             badge[VIP_BIO_MAX_BADGE_LENGTH];
   char             pin[VIP_BIO_MAX_PIN_LENGTH];
};

const int PinAuthorizationEventMsgSize = sizeof(PinAuthorizationEventMsg);



//
// Biometric Log Event
//

typedef union TriggerData
{
   struct {
      int height;  // CM
      int weight;  // KG
      int gender;  // 0 = MALE, 1 = FEMALE
   } donor_vitals_a;

   struct {
      int bloodType;
      int plateletCount;
      int hematocrit;
   } donor_vitals_b;

   int               value;
};

// This list is here for Vista convenience.  It mirrors the real def'n in TED IF.
const int VIP_BIO_SIGNATURE_PIN = 1;


const int VIP_TRIGGER_NONE                  = 0;
const int VIP_TRIGGER_DONOR_VITALS_A_CHANGE = 1; // Donor Entry Vitals (Gender, Height, Weight)
const int VIP_TRIGGER_DONOR_VITALS_B_CHANGE = 2; // Adjustment Vitals (Blood Type, Hct, Plt Count)
const int VIP_TRIGGER_PROCEDURE_SELECTED    = 3;
const int VIP_TRIGGER_ADJUSTMENT            = 4;
const int VIP_TRIGGER_ALARM                 = 5;


const int VIP_BIO_AUTH_NONE    = 0;
const int VIP_BIO_AUTH_SUCCESS = 1;
const int VIP_BIO_AUTH_FAILED  = 2;
const int VIP_BIO_AUTH_UNKNOWN = 3;


typedef struct biometricLogEventMsg
{
   trimaVistaHeader hdr;
   unsigned int     sequenceNumber;                   // Sequence Number of event
   time_t           timestamp;                        // Time event happened
   int              trigger;                          // Trigger that caused event
   TriggerData      data;                             // Data from trigger
   int              status;                           // Status of event
   char             badge[VIP_BIO_MAX_BADGE_LENGTH];  // Badge ID
   int              device;
};

const int biometricLogEventMsgSize = sizeof(biometricLogEventMsg);






//
//
//
class CVipIf
{
public:

   CVipIf();

   virtual ~CVipIf();

   //
   //
   // This routine checks to see that the message is valid.  The following
   // Checks are made:
   // 1.  Check that the message header CRC is correct
   // 2.  Check that the SOM is correct
   // 3.  Check that the message ID is a valid range
   // 4.  Check that the message length is valid
   // 5.  Check that the message body checksum is correct
   //
   // Usage:
   //    (1) pass in pointer to message buffer and the size received.
   //    (2) routine will return 0 if message passed validation.  Otherwise
   //        routine will pass back the appropriate response status code.  Also,
   //        routine will set a descriptive text string that can be gotten by calling
   //        errorString;
   //    (3) In the case that the message passed in does not have enough bytes to satisfy the
   //        message header, the value of VIP_BUFFER_INCOMPLETE will be returned.  Assuming
   //        that the message length is valid, the message should be stored and glued with the
   //        next received piece and revalidated.
   //
   int validHeader (const trimaVistaHeader &hdr, int sizeInBytes);


   //
   // This routine prepares the VIP message for transmission.
   // CRC's are calculated and all of the message fields are properly filled in.
   //
   void prepareMsg (void *msg, int messageId, int sequenceNumber, int status = 0, int sizeInBytes = trimaVistaHeaderLength);


   //
   // Returns the last generated error string.
   //
   char *errorString (void);


#ifndef WIN32
   //
   // Prints the header to the stream.
   //
   friend DataLog_Stream & operator << (DataLog_Stream &os, const trimaVistaHeader &hdr);
#endif


   //
   // Returns the current sequence number.
   //
   int sequenceNumber (void);


private:
   char *m_ErrorString;
   char *m_SerialNumber;
};
   
#endif
