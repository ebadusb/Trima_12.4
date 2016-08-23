/*******************************************************************************
 *
 * Copyright (c) 2003 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      bio_signatures.h
 *             biometric signatures
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file provides utility functions for the processing of biometric
 *             signatures.
 *
 ******************************************************************************/


#ifndef __BIO_SIGNATURES__
#define __BIO_SIGNATURES__

#include <vxWorks.h>
#include <string.h>

#include "datalog.h"


//
// Types of Biometric devices
//
// (This list should match the one defined in tedif.h)
//
enum BiometricSignatureType
{
   BIO_DEVICE_NONE = 0,
   BIO_DEVICE_PIN  = 1
};



//
// Trigger Types
//
enum ComplianceTriggerType
{
   TRIGGER_NONE = 0,
   TRIGGER_DONOR_VITALS_A_CHANGE,
   TRIGGER_DONOR_VITALS_B_CHANGE,
   TRIGGER_PROCEDURE_SELECTED,
   TRIGGER_ADJUSTMENT,
   TRIGGER_ALARM,
   TRIGGER_SPILLOVER,
   TRIGGER_AIR_IN_PLASMA_LINE
};


//
// Authorization Status Defns
//
enum AuthorizationStatusType
{
   BIO_AUTH_NONE = 0,
   BIO_AUTH_SUCCESS,
   BIO_AUTH_FAILED,
   BIO_AUTH_UNKNOWN
};

//
// Signature Request Responses from Vista
//

// Note:  Enum values are tied to Vista interface.
enum BioSignatureResponse
{
   BIO_SIGNATURE_INVALID = 0,
   BIO_SIGNATURE_VALID,
   BIO_SIGNATURE_VALID_ADD,
};



//
// PIN Device Type
//
#define BARCODE_ID_SIZE 80 + 1
#define PIN_SIZE         6 + 1

typedef struct PinSignature
{
   char badge[BARCODE_ID_SIZE];
   char pin[PIN_SIZE];
};



//
// Sent by the Biometric Device Mgr to VIP asking for verification of unknown user
//
typedef struct PinSignatureRequestMsg
{
   int  biometricDeviceId;
   int  scanSequenceNumber;
   char badge[BARCODE_ID_SIZE];
   char pin[PIN_SIZE];
};



//
// Sent by VIP to Biometric Device Mgr with action to take on unknown user.
//
typedef struct PinSignatureResponseMsg
{
   int                  scanSequenceNumber;
   BioSignatureResponse action;
   PinSignature         signature;
};



enum { BIO_MAX_PIN_SIGNATURES = 100,   // The max number to transfer via message system
       BIO_MAX_PIN_LIST       = 400 }; // Max size of PIN file.


//
// Sent by VIP to Biometric Device Mgr with a list of signatures to add to file.
//
typedef struct PinSignatureListMsg
{
   int          numberOfSignatures;
   PinSignature signatureList[BIO_MAX_PIN_SIGNATURES];
};


typedef union TriggerDataType
{
   struct // Donor Vitals A
   {
      int height;    // cm
      int weight;    // kg
      int gender;    // 0 = MALE, 1 = FEMALE
   } donor_vitals_a;

   struct // Donor Vitals B
   {
      int   bloodType;
      float plateletCount;
      float hematocrit;
   } donor_vitals_b;

   int value;
};

typedef struct CFR11_Log_Type
{
   unsigned int sequenceNumber;                        // Sequence Number of event
   time_t       timestamp;                             // Time event happened

   union
   {
      ComplianceTriggerType trigger;                     // Trigger that caused event
      int                   temp1;
   };

   TriggerDataType data;                               // Data from trigger

   union
   {
      AuthorizationStatusType status;                  // Status of event
      int                     temp2;
   };

   union
   {
      BiometricSignatureType device;                    // Device
      int                    temp3;
   };

   char badge[BARCODE_ID_SIZE];                        // Badge ID

   CFR11_Log_Type()
      : timestamp(0), trigger(TRIGGER_NONE), status(BIO_AUTH_NONE), device(BIO_DEVICE_NONE) { memset(badge, 0, BARCODE_ID_SIZE); data.value = 0; }
};

typedef unsigned int CFR11_Key_Type;


//
// Datalog stream manipulators
//
DataLog_Stream& operator << (DataLog_Stream& os, const AuthorizationStatusType& status);
DataLog_Stream& operator << (DataLog_Stream& os, const CFR11_Log_Type& item);
DataLog_Stream& operator << (DataLog_Stream& os, const ComplianceTriggerType& item);
DataLog_Stream& operator << (DataLog_Stream& os, const BioSignatureResponse& item);


#endif

/* FORMAT HASH 0301ed20ae2244bfa173264250b8d4bd */
