/*
 *   Wrapper around the UGL_DIB structure -- this is really meant as a way to
 *   bring some sanity to the resource management involved in these things.  The
 *   wrappers around the DIB and the MDIB manage their own resources,
 *   guaranteeing that memory is properly allocated and freed.
 *
 */

#ifndef DIB_H_INCLUDED
#define DIB_H_INCLUDED

#include <ugl/ugl.h>
#include "trima_ugl_mem.h"


class OSDib
{

public:

   // clut memory is owned by an external party
   OSDib(UGL_SIZE width,
         UGL_SIZE height,
         size_t sizeof_pixel,
         UGL_INT8 imageFormat,
         UGL_COLOR_FORMAT colorFormat,
         UGL_SIZE clutSize,
         void* clut);

   // clut memory is managed by us
   OSDib(UGL_SIZE width,
         UGL_SIZE height,
         size_t sizeof_pixel,
         UGL_INT8 imageFormat,
         UGL_COLOR_FORMAT colorFormat,
         UGL_SIZE clutSize);

   operator UGL_DIB* () { return &_dib; }

   unsigned char* image () { return (unsigned char*)_dib.pImage; }

   UGL_RGB& color (int i) {return ((UGL_RGB*)_dib.pClut)[i]; }

   ~OSDib();

private:

   UGL_DIB       _dib;
   unsigned long _bmpSize;

   bool          free_clut;
};


// mono bitmaps, used as masks
class OSDibMask
{
public:

   OSDibMask ();

   void load (UGL_SIZE width,
              UGL_SIZE height,
              UGL_SIZE stride);

   ~OSDibMask();

   unsigned char* image () { return (unsigned char*)_tdib.pImage; }

   operator UGL_MDIB* () { return &_tdib; }


private:
   UGL_MDIB _tdib;

};


#endif // include guard

/* FORMAT HASH a908683c2cd4854e7ad50d89c765df21 */
