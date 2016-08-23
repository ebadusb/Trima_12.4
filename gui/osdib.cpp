#include "osdib.hpp"
#include "trima_datalog.h"


// clut memory is external
OSDib::OSDib(UGL_SIZE width,
             UGL_SIZE height,
             size_t sizeof_pixel,
             UGL_INT8 imageFormat,
             UGL_COLOR_FORMAT colorFormat,
             UGL_SIZE clutSize,
             void* clut)
   : free_clut(false)
{
   _dib.height      = height;
   _dib.width       = width;
   _dib.stride      = width;
   _dib.imageFormat = imageFormat;
   _dib.colorFormat = colorFormat;
   _dib.clutSize    = clutSize;
   _dib.pClut       = clut;

   _bmpSize         = height * width * sizeof_pixel;

   _dib.pImage      = GUI_POOL_MALLOC(_bmpSize);
}


// we manage the clut memory
OSDib::OSDib(UGL_SIZE width,
             UGL_SIZE height,
             size_t sizeof_pixel,
             UGL_INT8 imageFormat,
             UGL_COLOR_FORMAT colorFormat,
             UGL_SIZE clutSize)
   : free_clut(true)
{
   _dib.height      = height;
   _dib.width       = width;
   _dib.stride      = width;
   _dib.imageFormat = imageFormat;
   _dib.colorFormat = colorFormat;
   _dib.clutSize    = clutSize;

   _bmpSize         = height * width * sizeof_pixel;

   _dib.pImage      = GUI_POOL_MALLOC(_bmpSize);
   _dib.pClut       = GUI_POOL_MALLOC(clutSize * sizeof(UGL_RGB));
}

OSDib::~OSDib()
{
   GUI_POOL_FREE(_dib.pImage);
   if (free_clut)
      GUI_POOL_FREE(_dib.pClut);
}

//////////////////////////////////////////////////////////////////////

OSDibMask::OSDibMask()
{
   _tdib.pImage = NULL;
}


void OSDibMask::load (UGL_SIZE width,
                      UGL_SIZE height,
                      UGL_SIZE stride)
{
   _tdib.width  = width;
   _tdib.height = height;
   _tdib.stride = stride;
   // each element of the image is of size unsigned int ( == 1 )
   _tdib.pImage =  (UGL_UINT8*)GUI_POOL_MALLOC(height * width);
}

OSDibMask::~OSDibMask()
{
   GUI_POOL_FREE(_tdib.pImage);
}

/* FORMAT HASH b5e59976edd0995dbf49296b00d9c17b */
