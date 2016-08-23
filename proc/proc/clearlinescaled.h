/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      clearlinescaled.h
 *
 */

#ifndef __CLEAR_LINE_SCALED_H__
#define __CLEAR_LINE_SCALED_H__

#include "clearlineabs.h"

class ClearLineScaled
   : public ClearLineAbs
{
   DECLARE_OBJ(ClearLineScaled);
public:

   //
   // Constructor
   ClearLineScaled();

   //
   // Copy constructor
   ClearLineScaled(const ClearLineScaled& rec);


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

/* FORMAT HASH 2efd3d7b9ed9c079c2eb57a47191cb7c */
