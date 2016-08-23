/*
 * Copyright (C) 2012 Gambro BCT, Inc.  All rights reserved.
 *
 *	cal.dat access class
 *
 * $Header: //depot/main/embedded/Trima/Main/trima_common/autocaldat.h#1 $
 *
 */

#ifndef AUTOCAL_DAT_H
#define AUTOCAL_DAT_H

//
// This class encapsulates the autocal.dat calibration file.  It is designed
// as a global static class, i.e., there is only one autocal.dat class resident in
// the system.
//
class AutoCalDat
{
public:
   AutoCalDat();

   virtual ~AutoCalDat();

   //
   // Reads autocal.dat file and set the appropriate status
   //
   static void read ();

   static bool update (float apsRaw, float apsZeroOffset);

   static const bool  dataOK () { return _dataOK; }
   static const float rawReference () {return _rawReference; }
   static const float lastZeroOffset () {return _lastZeroOffset; }

protected:
   //
   // Static data
   //
   static float _rawReference;
   static float _lastZeroOffset;
   static bool  _dataOK;
};

#endif /* ifndef AUTOCAL_DAT */

/* FORMAT HASH 9e8c73f360e62360ee9bb79c8b9c7ee4 */
