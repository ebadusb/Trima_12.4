/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      vtargetchecker.h
 *
 * ABSTRACT:   This class checks volumes to determine if the selected targets have been achieved
 *
 */

#include <vxworks.h>
#include "vtargetchecker.h"
#include "trima_datalog.h"
#include "an_alarm.h"

DataLog_Stream & operator << (DataLog_Stream& os, VTarget::VolTargetDetail value)
{
   switch (value)
   {
      case VTarget::NOCOLLECT :
         os << "NOCOLLECT";
         break;

      case VTarget::VIN :
         os << "VIN";
         break;

      case VTarget::VINDELTA :
         os << "VINDELTA";
         break;

      case VTarget::VBP :
         os << "VBP";
         break;

      case VTarget::VPBAG :
         os << "VPBAG";
         break;

      case VTarget::VRBCBAG :
         os << "VRBCBAG";
         break;

      case VTarget::VCOLBAG :
         os << "VCOLBAG";
         break;

      default :
         os << "UNKNOWN";
         break;
   }

   os << "(" << (int)value << ")";

   return os;
}



#define BUFFER_VOLUME 2.0f

VTargetChecker::VTargetChecker()
   : _VTargetArraySize(0),
     _ProcVolumesCDS(ROLE_RO)
{}

VTargetChecker::~VTargetChecker()
{}

void VTargetChecker::addTarget (const VTarget::VolTargetDetail tt, const float t)
{
   float v = getVolume(tt);
   addTarget(tt, t, v);
}

void VTargetChecker::addTarget (const VTarget::VolTargetDetail tt, const float t, const float v)
{
   _VTargetArray[ _VTargetArraySize ].targetType(tt);
   _VTargetArray[ _VTargetArraySize ].target(t);
   _VTargetArray[ _VTargetArraySize ].volume(v);
   _VTargetArraySize++;
   DataLog(log_level_proc_debug) << "Add target: target type->" << tt << " target->" << t << " volume->" << v << " size->" << _VTargetArraySize << endmsg;
}

void VTargetChecker::clearTargets ()
{
   _VTargetArraySize = 0;
   DataLog(log_level_proc_debug) << "Cleared Targets size->" << _VTargetArraySize << endmsg;
}

int VTargetChecker::evaluateTargets (const State_names state)
{
   Status status = VTargetChecker::ReachedTarget;
   int    i;

   //
   // Find out if there are any real targets to check ...
   for ( i = 0; ( i < _VTargetArraySize && status == VTargetChecker::ReachedTarget ) ; i++ )
   {
      //
      // Check the target for any real target type ...
      if (    _VTargetArray[ i ].targetType() != VTarget::NOCOLLECT
              && _VTargetArray[ i ].target() > 0.0f
              )
      {
         //
         // There is at least one real target ...
         status = VTargetChecker::StillWaiting;
         break;
      }
   }

   if ( status == VTargetChecker::ReachedTarget )
   {
      DataLog(log_level_proc_info) << "No targets or targets reached for current state->" << state << endmsg;
   }

   //
   // Check all volume targets first ...
   //
   //  If any collection volume target is within the 'close enough range' we will make
   //   ourselves run to the collection volume target
   for ( i = 0; ( i < _VTargetArraySize && status != VTargetChecker::ReachedTarget ) ; i++ )
   {
      //
      // Is the target type a collection volume target ...
      //
      if (    bagTarget(_VTargetArray[ i ].targetType() )
              && _VTargetArray[ i ].target() > 0.0f
              )
      {

         //
         // Check for the volume targets being reached ...
         if ( _VTargetArray[ i ].target() <= _VTargetArray[ i ].volume() )
         {
            //
            // We have reached target volume
            status = VTargetChecker::ReachedTarget;

            DataLog(log_level_proc_info) << "Checking bag target: #" << i << " "
                                         << "Bag Target->" << _VTargetArray[ i ].targetType() << " "
                                         << "Target Volume->" << _VTargetArray[ i ].target() << " "
                                         << "Volume->" << _VTargetArray[ i ].volume() << " "
                                         << "Target reached->" << status << " state->" << state << endmsg;
         }
         //
         // else check for the target being 'close enough' to override other targets
         else if ( _VTargetArray[ i ].target() <= _VTargetArray[ i ].volume() + BUFFER_VOLUME )
         {
            //
            // Don't check the other targets, we are running to this volume target ...
            //
            status = VTargetChecker::OnlyUseBagVolumeTargets;
         }

      }
   }

   //
   //  If all bag volume targets are out of range, then check the rest of the targets ...
   //
   for ( i = 0 ; ( i<_VTargetArraySize && status==StillWaiting ) ; i++ )
   {
      if (    !bagTarget(_VTargetArray[ i ].targetType() )
              && _VTargetArray[ i ].targetType() != VTarget::NOCOLLECT
              && _VTargetArray[ i ].target() > 0.0f
              )
      {
         //
         // Check for the volume targets being reached ...
         if ( _VTargetArray[ i ].target() <= _VTargetArray[ i ].volume() )
         {
            //
            // We have reached target volume
            status = VTargetChecker::ReachedTarget;

            DataLog(log_level_proc_info) << "Checking target: #" << i << " "
                                         << "Target->" << _VTargetArray[ i ].targetType() << " "
                                         << "Target Volume->" << _VTargetArray[ i ].target() << " "
                                         << "Volume->" << _VTargetArray[ i ].volume() << " "
                                         << "Target reached->" << status << " state->" << state << endmsg;
         }

      }
   }

   //
   // Clear all targets after checking ...
   //
   clearTargets();

   return ( status==VTargetChecker::ReachedTarget );
}

float VTargetChecker::getVolume (const VTarget::VolTargetDetail tt)
{
   float vol = 0.0f;
   switch ( tt )
   {

      case VTarget::NOCOLLECT :
         DataLog(log_level_proc_debug) << "NO COLLECT Volume->";
         break;

      case VTarget::VIN :
         DataLog(log_level_proc_debug) << "Vin Volume->";
         vol = _ProcVolumesCDS.Vin.Get();
         break;

      case VTarget::VINDELTA :
         DataLog(log_level_proc_debug) << "VIN Delta Volume->";
         vol = _ProcVolumesCDS.VinCurrentSubstate.Get();
         break;

      case VTarget::VBP :
         DataLog(log_level_proc_debug) << "VBP Volume->";
         vol = _ProcVolumesCDS.VBP.Get();
         break;

      case VTarget::VPBAG :
         DataLog(log_level_proc_debug) << "VPlasma Bag Volume->";
         vol = _ProcVolumesCDS.PlasmaVolume.Get();
         break;

      case VTarget::VRBCBAG :
         DataLog(log_level_proc_debug) << "VRBC Bag Volume->";
         vol = _ProcVolumesCDS.RBCVolume.Get();
         break;

      case VTarget::VCOLBAG :
         DataLog(log_level_proc_debug) << "VCollect Bag Volume->";
         vol = _ProcVolumesCDS.PlateletVolume.Get();
         break;

      default :
         anAlarmMsg alarm(VTARGETCHECKER_DEFAULT);
         alarm.setAlarm();

   }
   DataLog(log_level_proc_debug) << vol << " " << tt << endmsg;
   return vol;
}

int VTargetChecker::bagTarget (const VTarget::VolTargetDetail tt)
{
   //
   // Check for bag volume target ...
   if (    tt == VTarget::VCOLBAG
           || tt == VTarget::VPBAG
           || tt == VTarget::VRBCBAG )
   {
      //
      // It is a bag volume target ...
      return 1;
   }
   return 0;
}

void VTargetChecker::cleanup ()
{}

/* FORMAT HASH 9b56a64013242c54a1150915d8e8f011 */
