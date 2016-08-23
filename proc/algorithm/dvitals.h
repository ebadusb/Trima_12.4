/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      dvitals.h
 *             Donor Vitals Object to support round robin of donor
 *             information between GUI-->PROC-->Safety-->GUI.
 *             This object is used within proc.  When the donor vitals
 *             are confirmed, the data are passed to the CDonor object
 *             passed as a parameter and the ProcDonor message is sent
 *             with the confirmed data.
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/


#if !defined( INCLUDE_DVITALS )
#define INCLUDE_DVITALS

#include "configdef.h"
#include "trimamessages.h"
#include "donor.h"
#include "datastore.h"

class ProcData;

class CDVitals
{

public:
   CDVitals();
   virtual ~CDVitals();

   // Initialize with loaction to store accepted donor
   // information.
   void Initialize ();

   // behavior changes if the donor is connected.  Object is created
   // with assumption donor is not connected.  When the donor is
   // connected, call this member to indicate donor is connected.
   // Currently do not allow transition from connect to not connected.
   void SetDonorIsConnected ();
   int  DonorIsConnected () const;

   //
   // Function used to output the donor data to the periodic log ...
   void forceLogging (void);

protected:
   void ProcessGuiMessage (void);
   int  SameAsCDS (const CDonor& donor) const;

   static const char* convertGender (const BaseElement<int>& female);
   static const char* convertHeight (const BaseElement<float>& height);
   static const char* convertWeight (const BaseElement<float>& weight);

private:

   // incoming messages
   DVitalsFromGUI _msgFromGui;

   // outgoing messages
   DVitalsFromProc     _msgToSafety;
   DoPredictionMsg     _doPredictionMsg;
   PredictRespondedMsg _predictResponded;

   CDonor              _tempDonor;
   CDonor              _final;
   CDonor              _previousFinal;
   bool                _donorConnected;

private:
   static DataLog_SetHandle _DonorSet;
   static ProcData*         _ProcData;

};



#endif

/* FORMAT HASH bddf6071f3c8101d2b0ed0bcb69a66a2 */
