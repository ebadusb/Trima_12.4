/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      vtargetchecker.h
 *
 * ABSTRACT:   This class checks volumes to determine if the selected targets have been achieved
 *
 */


#ifndef _VTARGET_CHECKER_H_  //  Prevents multiple inclusions.
#define _VTARGET_CHECKER_H_

#include "states.h"
#include "procvolumes_cds.h"

class VTarget
{
public:

   enum VolTargetDetail
   {
      NOCOLLECT,
      VIN,
      VINDELTA,
      VBP,
      VPBAG,
      VRBCBAG,
      VCOLBAG
   };

   // Default constructor
   VTarget()
      : _TargetType(NOCOLLECT), _Target(0.0f), _Volume(0.0f) { }
   // Default destructor
   virtual ~VTarget() { }

   //
   // Set/Get target type
   void                  targetType (const VolTargetDetail t) { _TargetType = t; }
   const VolTargetDetail targetType () const { return _TargetType; }

   //
   // Set/Get target
   void        target (const float t) { _Target = t; }
   const float target () const { return _Target; }

   //
   // Set/Get volume
   void        volume (const float v) { _Volume = v; }
   const float volume () const { return _Volume; }

protected:

   VolTargetDetail _TargetType;
   float           _Target;
   float           _Volume;
};


class VTargetChecker
{
protected:
   enum Status { StillWaiting, ReachedTarget, OnlyUseBagVolumeTargets };

public:

   // Default constructor
   VTargetChecker();

   // Default destructor
   virtual ~VTargetChecker();

   //
   // Function to add another target to evaluate
   //
   void addTarget (const VTarget::VolTargetDetail tt,
                   const float targetVolume);
   void addTarget (const VTarget::VolTargetDetail tt,
                   const float targetVolume,
                   const float currentVolume);

   //
   // Function to clear all targets out of the array
   //
   void clearTargets ();

   //
   // Function used to determine whether the target has been reached
   //  given the target type
   //
   int evaluateTargets (const State_names state = NOCOLLECT_STATE);

protected:

   //
   // Get the volume given the target type
   //
   float getVolume (const VTarget::VolTargetDetail tt);

   //
   // Check the target type for being a bag volume target
   int bagTarget (const VTarget::VolTargetDetail tt);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Access to procedure volumes
   ProcVolumes_CDS _ProcVolumesCDS;

   //
   // Array of all targets to be evaluated ( no more than 10 )...
   //  the list get cleared after every evaluation.
   VTarget _VTargetArray[ 10 ];
   int     _VTargetArraySize;

};


DataLog_Stream& operator << (DataLog_Stream& os, VTarget::VolTargetDetail value);

#endif

/* FORMAT HASH 90f6b0badd2f71135f19c9b0a54934b1 */
