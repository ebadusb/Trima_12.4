/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      clearline.h
 *
 */

#ifndef _CLEARLINE_H_
#define _CLEARLINE_H_

#include "clearlineabs.h"

class ClearLine
   : public ClearLineAbs
{
   DECLARE_OBJ(ClearLine);
public:

   //
   // Constructor
   ClearLine();

   //
   // Copy constructor
   ClearLine(const ClearLine& rec);


protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int shouldRecover ();

   virtual bool disabled () const;

   virtual void setPumpSpeeds (ProcData& pd);
};

#endif

/* FORMAT HASH 00d84d51fc511834304301aee0e853da */
