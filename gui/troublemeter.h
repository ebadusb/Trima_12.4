/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  TroubleMeter

*     File name:   troublemeter.h

*     Contents:    Definition of class methods and attributes

*     Description: Class header file for trouble.cpp

*     Documents:
          Requirements:    I:\....
          Select OMT:      I:\....
          Test:            I:\....

*************************< FOCUSSED GUI DOMAIN >****************************

*/

// Single include define guardian
#ifndef TROUBLEMETER_H
#define TROUBLEMETER_H

#include "bitmap.h"

class TroubleMeter
{
public:
   enum MeterType { Vertical, Horizontal };
   enum CapType { CapEnabled, CapDisabled };

   TroubleMeter();
   virtual ~TroubleMeter();

   //
   //
   //   I must be initialized before you can use any methods.  After
   //   initialization I am inactive (and undrawn).
   //
   int Initialize (OSWindow& parent,
                   short X,
                   short Y,
                   short H,
                   short W,
                   OSColor color,
                   CapType capType = CapEnabled,
                   MeterType meterType = Vertical);
   //
   //
   //   Turn meter on or off
   //
   void Active (bool flag);
   bool Active () const;

   //
   //
   //   Update my values.  I redraw if I am active.
   //
   void Update (float minscale,
                float maxscale,
                float currentValue,
                float capValue,
                bool forceRedraw = false);

protected:
   void allocate_resources ();
   void deallocate_resources ();
   void update_cap (Bitmap* bitmap, float height, float value);

protected:

   bool         _active;
   OSColor      _color;
   CapType      _capType;
   MeterType    _meterType;

   short        _meterX;
   short        _meterY;
   short        _meterH;
   short        _meterW;
   float        _maxScale;
   float        _minScale;

   float        _currentReading;
   float        _capValue;

   float        _lastReading;
   float        _lastCap;

   Bitmap*      _bitmapCap;
   OSRectangle* _meterFillRegion;
   OSWindow*    _parent;

private:

   // noncopyable and non-assignable
   TroubleMeter(const TroubleMeter&);              // not defined
   TroubleMeter& operator = (const TroubleMeter&); // not defined

};

// End of single include define guardian
#endif /* ifndef TROUBLEMETER_H */

/* FORMAT HASH 4cafa9913ce20455526eaca9fced1010 */
