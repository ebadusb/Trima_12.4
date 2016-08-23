/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Bitmap

*     File name:    bitmap.h

*     Contents:     See prototype header descriptions below...

*     Description:

          This class is the base class for all bitmap objects.

*************************< FOCUSSED GUI DOMAIN >***************************/



// Single include define guardian
#ifndef BITMAP_HPP
#define BITMAP_HPP


// CLASS INCLUDE

#include "bitmap_info.h"
#include "osbitmap.hpp"

class Bitmap
{

   // Noncopyable, nonassignable; what are copy semantics for bitmaps?
   Bitmap(const Bitmap&);
   Bitmap& operator = (const Bitmap&);

protected:
   OSBitmap* _bitmap ;
   BITMAP_ID _id ;
   int       _xcoord, _ycoord ;
   bool      _visible;
   bool      _extraLogging;

public:


   // CONSTRUCTOR
   // The constructor will take bitmap file name and initial X,Y coordinates
   // in anticipation of allocation.
   Bitmap (int cons_x_coord = 0,
           int cons_y_coord = 0,
           BITMAP_ID id = BITMAP_NULL,
           bool extraLogging = false);


   // DESTRUCTOR
   virtual ~Bitmap ();


   // ALLOCATE_RESOURCES
   // This method will allocate a widget and attach to the specified
   // parent window.
   virtual void allocate_resources (OSWindow& parent);
   virtual bool allocated () const { return _bitmap != NULL; }

   // DEALLOCATE_RESOURCES
   // This method will delete the photon widget and any other manageable
   // resources.
   virtual void deallocate_resources ();

   // SET_ID
   // This method will load a bitmap and attach it to the bitmap widget
   virtual void set_id (BITMAP_ID id);

   // GET_ID
   // This method will return the bitmap file ID to the
   // caller.
   virtual BITMAP_ID get_id () const { return _id; }

   // SET_POSITION
   // virtual void     get_dimensions (OSRegion& region) const;
   virtual OSRegion get_dimensions () const;
   virtual void     set_position (short x, short y);

   // SET_VISIBLE
   virtual bool get_visible () const { return _visible; }
   virtual void set_visible (bool newVisible);

   // MOVE
   virtual void move_to_front ();
   virtual void move_to_back ();

   virtual void copy_to_display () { if (_bitmap) _bitmap->copy_to_display(); }

   // The usual alternative is to grant friendship to the op<< function; but
   // this way preserves encapsulation.
   DataLog_Stream& print (DataLog_Stream&) const;


} ; // END of Bitmap Class Declaration

inline
DataLog_Stream& operator << (DataLog_Stream& os, const Bitmap& bmp) { return bmp.print(os); }


// End of single include define guardian
#endif

/* FORMAT HASH 6af01615480d1b87c1cd9b360d6ecfe4 */
