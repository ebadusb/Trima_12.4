/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      operatorresponse.h
 *
 */


#ifndef OPERATORRESPONSE_H  //  Prevents multiple inclusions.
#define OPERATORRESPONSE_H

#include <stateabs.h>
#include "trimamessages.h"
#include "guiproc.h"


// Class Declaration
class OperatorResponse
   : public StateAbs
{
   DECLARE_STATE(OperatorResponse);
public:

   // Default constructor
   OperatorResponse();

   // Copy constructor
   OperatorResponse(const OperatorResponse& state);

   // Default destructor
   virtual ~OperatorResponse();

   virtual StateAbs* clone () const { return ( new OperatorResponse(*this)); }

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
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const OperatorResponse& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();


   void cassetteFuctMsg (void* data);

   void clearFuctMsg (void* data);

protected:

   double _LoggingInterval;

   // message from GUI that has the cassette functions
   Message<CASSETTE_MESSAGE>* _cassetteMsg;

   ClearRefNumberMsg*         _clearBitsMsg;
};


#endif

/* FORMAT HASH 54f5bfd411fb5376f5c02c40b1cb102b */
