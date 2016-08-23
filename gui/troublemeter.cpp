/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  TroubleMeter

*     File name:   troublemeter.cpp

*     Contents:    Implementation of class methods

*     Description: Class representaion of the ADJUST screen

*     Documents:
          Requirements:    I:\....
          Select OMT:      I:\....
          Test:            I:\....

*************************< FOCUSSED GUI DOMAIN >****************************
*/
// header file includes
#include <vxworks.h>
#include <math.h>

#include "troublemeter.h"
#include "an_alarm.h"
#include "guiglobs.hpp"

// define standard cap height for positioning algorithms
static const short CAP_HEIGHT = 13;

//
// -----------------------------------------------------------------------------
//
TroubleMeter::TroubleMeter()
   : _active(false),
     _capType(CapDisabled),
     _meterType(Horizontal),
     _meterX(0),
     _meterY(0),
     _meterH(0),
     _meterW(0),
     _maxScale(0.0f),
     _minScale(0.0f),
     _currentReading(0.0f),
     _capValue(0.0f),
     _lastReading(0.0f),
     _lastCap(0.0f),
     _bitmapCap(0),
     _meterFillRegion(0),
     _parent(0),
     _color(0)
{}

TroubleMeter::~TroubleMeter()
{
   if (_active)
      deallocate_resources();
}

//
// -----------------------------------------------------------------------------
//
int TroubleMeter::Initialize (OSWindow& parent,
                              short X,
                              short Y,
                              short H,
                              short W,
                              OSColor color,
                              CapType capType,
                              MeterType meterType)
{
   _parent    = &parent;

   _capType   = capType;
   _meterType = meterType;
   _meterX    = X;
   _meterY    = Y;
   _meterH    = H;
   _meterW    = W;
   _color     = color;

   //
   //  Rudimentary warm fuzzy feeling that initialization parameters are OK
   //
   return ((X >= 0) && (Y >= 0) && (H >= 0) && (W >= 0));
}
//
// -----------------------------------------------------------------------------
//
void TroubleMeter::Active (bool flag)
{
   if (_active != flag)
   {
      if (!_active)
      {
         allocate_resources();
         Update(_currentReading, _capValue, _minScale, _maxScale, true);
      }
      else
      {
         deallocate_resources();
      }
   }
}

bool TroubleMeter::Active () const { return _active; }

//
// -----------------------------------------------------------------------------
//
void TroubleMeter::Update (float minscale,
                           float maxscale,
                           float currentValue,
                           float capValue,
                           bool forceRedraw)
{

   const bool redraw = (forceRedraw || (minscale != _minScale) || (maxscale != _maxScale));

   _minScale = minscale;
   _maxScale = maxscale;

   if (_active && (maxscale > minscale))
   {
      //
      //   First update the meter widget itself
      //
      if (redraw || (_currentReading != currentValue))
      {
         _currentReading = currentValue;

         if (_currentReading < _minScale) _currentReading = _minScale;
         if (_currentReading > _maxScale) _currentReading = _maxScale;

         // assign height and position, the ceil function rounds up to cover the
         // pixel not accounted for because of potential round off error (short)
         OSRegion reg;
         double   fractionalPos = (_currentReading - _minScale) / (_maxScale - _minScale);

         if (_meterType == Vertical)
         {
            int size = (int)ceil(fractionalPos * _meterH);
            reg = OSRegion(_meterX, _meterY + (_meterH - size), _meterW, size);
         }
         else
         {
            int size = (int)ceil(fractionalPos * _meterW);
            reg = OSRegion(_meterX, _meterY, size, _meterH);
         }

         if ( !_meterFillRegion )
         {
            _meterFillRegion = new OSRectangle(*GuiMain::getDisplay(), reg, _color);
            _parent->add_object_to_front(_meterFillRegion);
         }
         else
         {
            _meterFillRegion->set_region(reg);
         }
      }

      //
      //   Update the cap widget
      //
      if (_capType == CapEnabled)
      {
         if (redraw || (_capValue != capValue))
         {
            update_cap(_bitmapCap, CAP_HEIGHT, capValue);
         }
      }

      _capValue = capValue;

   } // if _active

}

//
// -----------------------------------------------------------------------------
//
void TroubleMeter::update_cap (Bitmap* bitmap, float height, float value)
{
   //
   //   Limit  value to meter range
   //
   if (value < _minScale) value = _minScale;
   if (value > _maxScale) value = _maxScale;

   // remove existing bitmap
   bitmap->deallocate_resources();

   // allocate the bitmap
   bitmap->allocate_resources(*_parent);

   // establish location of the bitmap
   bitmap->set_position((short)(_meterX),
                        (short)ceil(_meterY + _meterH * (1 - (value - _minScale) / (_maxScale - _minScale)) - height));
}

//
// -----------------------------------------------------------------------------
//
void TroubleMeter::allocate_resources ()
{
   if ((!_parent) || _active) return;

   //
   //
   //   Create the cap indicators
   //
   if (_capType == CapEnabled)
   {
      _bitmapCap = new Bitmap(0, 0, BITMAP_DRAW_CAP);
   }

   _active = true;
   Update(_currentReading, _capValue, _minScale, _maxScale, true);
}

//
// -----------------------------------------------------------------------------
//
void TroubleMeter::deallocate_resources ()
{
   if ( _bitmapCap )
   {
      _bitmapCap->deallocate_resources();
      delete _bitmapCap;
      _bitmapCap = 0;
   }

   delete _meterFillRegion;
   _meterFillRegion = NULL;

   _active          = false;
}

/* FORMAT HASH c1bfc5976c9f3d80e1b7ba3e916b4dbb */
