/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 *	cal.dat access class
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/trima_common/rcs/caldat.h 1.1 2002/10/11 16:42:23Z jl11312 Exp dslausb $
 * $Log: caldat.h $
 * Revision 1.1  2002/10/11 16:42:23Z  jl11312
 * Initial revision
 *
 */

#ifndef caldat_h
#define caldat_h

#include "alarms.h"
#include "datfile.h"
//
//  This class encapsulates the cal.dat calibration file.  It is designed
//  as a static class, i.e., there is only one cal.dat file per task
//  and each task has a single instance of the file.
//
//  At the start of task execution, it is the task's responsibility to
//  initialize() this class.  From that point on, any object within
//  the task can access a value via a reference:
//     float tmp = CalDat::data().APSRawPositive*gain;
//
//  alternately, after the initialization, the task could create a singleton
//  data reference for use:
//     const CalDat::CalDatStruct &caldat = CalDat::data();
//  at which point the object would reference the values like:
//     float tmp = caldat.APSRawPositive*gain;
//
//
//  To add a value to the cal.dat file:
//    1: Change cal.dat to include the value
//    2: Add the equivalent value to this structure
//    3: Add an initialization statement in initialize() in caldat.cpp
//
class CalDat
{

public:
   struct CalDatStruct
   {

      float APSRawPositive;
      float APSmmHgPositive;
      float APSRawReference;
      float APSmmHgReference;
      float APSRawNegative;
      float APSmmHgNegative;

      float CPSOffset;
      float CPSGain;

      float RBCInitRedDrive;
      float RBCInitGreenDrive;

      float touchScreenCalA;
      float touchScreenCalB;
      float touchScreenCalC;
      float touchScreenCalD;
      float touchScreenCalE;
      float touchScreenCalF;
   };


public:
   //
   //   reads file and returns an alarm value if something goes wrong.  I do
   //   not actually raise an alarm.  If everything initializes properly
   //   I return NULL_ALARM.
   //
   static ALARM_VALUES        initialize (bool logError = false);
   static const CalDatStruct& data ();
   static ALARM_VALUES        convertAlarm(DFR_ERROR);

protected:
   //
   //
   //   My static singleton instance of the data
   //
   static CalDatStruct _data;

//
//
//   These methods are currently protected since we do not expect
//   anyone to actually instantiate a CalDat class.
//
protected:
   CalDat();
   virtual ~CalDat();

};

#endif /* ifndef caldat_h */

/* FORMAT HASH affd57c79ef9b0de725d30773b7ec9ef */
