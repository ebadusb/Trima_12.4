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
 *
 ******************************************************************************/


#ifndef __BIO_MGR__
#define __BIO_MGR__

#include <map>
#include <list>


#include "message.h"                  // Include message system stuff
#include "trimamessages.h"            // Trima Int messages

#include "bio_signatures.h"           // Include Bio Sig message defns
#include "alarms.h"                   // Get alarm defns
#include "gui_cds.h"                  // Get log item sequence number
#include "safproc.h"                  // Get Donor Vitals Msg
#include "guistring.h"                // To make text for cfr_list.
#include "pfrmsg.h"                   // Gets PFR Status msg


// forward declare guiglobs
class guiglobs;
class Random_file_storage;


//
// class to hold onto adjustments made on adjust screen.
//
class AdjustmentContainer
{
public:
   AdjustmentContainer()
      : drawAdjustment(false), drawUp(0), drawDown(0), returnAdjustment(false), returnUp(0), returnDown(0), tinglingAdjustment(false), tinglingUp(0), tinglingDown(0), clumpingAdjustment(false), clumpingUp(0), clumpingDown(0) {}

   bool drawAdjustment;
   int  drawUp;
   int  drawDown;
   bool returnAdjustment;
   int  returnUp;
   int  returnDown;
   bool tinglingAdjustment;
   int  tinglingUp;
   int  tinglingDown;
   bool clumpingAdjustment;
   int  clumpingUp;
   int  clumpingDown;

public:
   void LogItems ();
};



class BioSignaturesMgr
{
   friend class AdjustmentContainer;

   friend class guiglobs; // guiglobs is the only object allowed to make a BioSignaturesMgr

public:
   enum PinValidType
   {
      BIO_PIN_VALID,
      BIO_PIN_INVALID,
      BIO_NO_MATCH
   };

private:
   static const int CFR11_LOG_STRING_SIZE = 256;

   typedef struct CFR11_PFR_Storage_Type
   {
      CFR11_Log_Type logItem;
      char           logString[CFR11_LOG_STRING_SIZE];
   };


public:
   void Initialize ();

   // Pin File methods
   PinValidType PinValid (const char* badge, const char* pin);
   void         AddPin (const char* badge, const char* pin);

   void UpdateItem (const char* badge, AuthorizationStatusType status, BiometricSignatureType deviceId, long key, const void* authData);

   void invoke_cfr11_screen ();

private:
   void SetAuditLevel ();

   void RequestPinSignature (const char* badge, const char* pin, int sequenceNumber);

// We need these functions to be public for screenshots.
#if CPU==SIMNT
public:
#endif /* if CPU==SIMNT */
   // Save Event Information.
   // Returns sequence number of logged item
   int LogEvent (ComplianceTriggerType trigger, const TriggerDataType& data, const guistring& list_text);

   // Helper routines
   void BuildDonorVitalsAMsg (const SDonorVitals& vitals, guistring& msg);
   void BuildDonorVitalsBMsg (const SDonorVitals& vitals, guistring& msg);

private:
   BioSignaturesMgr();          // only friends are allowed to construct
   virtual ~BioSignaturesMgr();

   // noncopyable and nonassignable
   BioSignaturesMgr(const BioSignaturesMgr&);              // not defined
   BioSignaturesMgr& operator = (const BioSignaturesMgr&); // not defined

   void add_time_of_day (guistring& list_text);

   // Message Handlers
   void HandlePinAddMsg ();
   void HandleAlarmResponseMsg ();
   void HandlePfrToOccurMsg ();
   void HandleConfigChangedMsg ();
   void HandleAdjustmentMsg ();
   void HandleDonorVitalsChangeMsg ();
   void HandleProcedureSelectedMsg ();
   void HandleAirInPlasmLineMsg ();
   void HandleSpilloverMsg ();
   void HandleErasePinFileMsg ();
   void HandleNumberOfNewPinsMsg ();
   void HandleWritePinFileMsg ();


private:
   // Received Messages
   Message<PinSignatureListMsg> _pinAddMsg;
   VistaWritePinFileMsg         _writePinFileMsg;
   VistaNumberOfNewPinsMsg      _numberOfNewPinsMsg;

   Message<PFR_Status>          _pfrMsg;
   ConfigChangedMsg             _configChangedMsg;
   ErasePinFileMsg              _erasePinFileMsg;

   // CFR 11 Trigger Alarms
   Message<Alarm_response_struct> _alarmResponseMsg;
   ProcedureSelectedMsg           _procedureSelectedMsg;
   ProcedureAdjustmentMsg         _adjustMsg;
   DVitalsFromGUI                 _donorVitalsChangeMsg;

   Random_file_storage*           _biometricPfrFile;


   bool      _cantPfr;

   bool      _standard_time; // true for standard (AM/PM), false for military time
   guistring time_text;

   // CDS objects
   Gui_CDS _guiCds;

   // Donor Vitals Tracking
   int                 bio_gender;
   float               bio_height;
   float               bio_weight;

   float               bio_hematocrit;
   float               bio_platelet;
   BLOODTYPE           bio_bloodtype;

   AdjustmentContainer _adjustmentContainer;
};

#endif

/* FORMAT HASH e7803e219766886abf18ac96526f3fbd */
