/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cassetteid.h
 *
 */


#ifndef CASSETTEID_H  //  Prevents multiple inclusions.
#define CASSETTEID_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"
#include "trimamessages.h"


// Class Declaration
class CassetteID
   : public StateAbs
{
   DECLARE_STATE(CassetteID);
public:

   // Default constructor
   CassetteID();

   // Copy constructor
   CassetteID(const CassetteID& state);

   // Default destructor
   virtual ~CassetteID();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback for the identify set msg
   //
   virtual void operatorSelectedCassette ();

   //
   // Callback for the wait timer.
   //  ( Function which determines the set type )
   //
   void initRBCDetector ();

   virtual StateAbs* clone () const { return ( new CassetteID(*this)); }

protected:

   //
   // Functions to do this states processing ...
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Function called upon first entrance to this state
   //
   virtual int preEnter ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Function which determines if the loaded cassette will
   //  work with the selected procedure
   //
   void checkCompatibility ();

   //
   // Function to log the calibration data
   //
   void logCalData ();

   //
   // Reset state upon termination
   //
   virtual void reset ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const CassetteID& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Timer used to signal when a detection should be
   //  attempted
   //
   TimerMessage _DetectionWaitTimer;

   //
   // Message used to return the set identified
   //  from GUI
   //
   OperatorSetIdentificationMsg _IdentifySetMsg;

   //
   // Alarms ...
   //
   anAlarmMsg _SetIdentificationAlarm;

   //
   // Stores the determined set type ...
   //
   int _CassetteType;

   //
   // Stores time dependent drive values
   //
   unsigned char _RedDriveValue;
   unsigned char _GreenDriveValue;

   //
   // Two attempts are given on an unsuccessful
   //  set identification ...
   //
   int _RetryAttempted;

   //
   // The disposable was successfully identified
   //
   int _DisposableSelected;


   // cassette bit has stamp color set to white (1)
   bool _CatalogNumberIsWhite;
   bool _CatalogNumberIsBlack;
   bool _CatalogNumberIsPLTMSS;
   bool _CatalogNumberIsRBCMSS;
   bool _CatalogNumberDoesNotSupportSaline;

   bool _driveValuesLogged;


private:

   //  Error flags.
   enum { CASSETTE_OK, CASSETTE_UNRECOGNIZED, CASSETTE_INVALID, UNABLE_TO_LOWER,
          OPERATOR_REQUESTED_UNLOAD } _LoadError;

};


#endif

/* FORMAT HASH 406913ef8dee841134189a3744b78879 */
