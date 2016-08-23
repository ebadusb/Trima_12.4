/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Base_Window

*     File name:   bwindow.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

****************************************************************************/

#include <vxworks.h>
#include "bwindow.hpp"
#include "guiMain.hpp"
#include "guipalette.h"
#include "trima_ugl_mem.h"
#include "frmtdate.hpp"


int     Base_Window::_snapNumber = 0;
rawTime Base_Window::_snapTime   = {0};

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Base_Window::Base_Window(BITMAP_ID background, DisplayOrderType displayOrder)
   : OSWindow(*GuiMain::getDisplay(), OSRegion(0, 0, GuiMain::getDisplay()->width(), GuiMain::getDisplay()->height())),
     background_bitmap(0, 0, background),
     backgrnd_bitmap_id(background),
     _displayOrder(displayOrder),

#if CPU==SIMNT
     _txtScreenshotName(textEmptyFormatSimScreenshotName),
     _txtScreenshotInfo(textEmptyFormatSimScreenshotInfo),
#endif

     screen_invocation_type(GUI_SCREEN_INVOKE_NOTACTIVE),
     window_ID(GUI_SCREEN_NULL_SCREEN),
     allocation_parameter(),
     _allocated(false)
{} // END of Base_Window CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Base_Window::~Base_Window()
{
   Base_Window::deallocate_resources();

#if CPU == SIMNT
   array[window_ID] = NULL;
#endif /* if CPU == SIMNT */
} // END of Base_Window DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// SET_BACKGROUND_BITMAP
void Base_Window::set_background_bitmap (BITMAP_ID background)
{
   if (backgrnd_bitmap_id != background)
   {
      backgrnd_bitmap_id = background;
      background_bitmap.set_id (backgrnd_bitmap_id);
   }
} // END of Base_Window SET_BACKGROUND_BITMAP


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Base_Window::allocate_resources (const char* allocation_parameter)
{
   if (_allocated)
   {
      DataLog(log_level_critical) << "Base_Window: invalid reallocation.  Window was " << window_ID << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "Base_Window: invalid reallocation");
   }

   // Save off any allocation parameters
   set_allocation_parameter (allocation_parameter);

   // Load the background bitmap
   background_bitmap.allocate_resources (*this);
   set_background_bitmap (backgrnd_bitmap_id);

   //
   //  Size window to background bitmap size
   //
   if (backgrnd_bitmap_id != BITMAP_NULL)
   {
      OSRegion bitmapreg, windowreg;
      bitmapreg = background_bitmap.get_dimensions();
      get_region(windowreg);

      windowreg.width  = bitmapreg.width;
      windowreg.height = bitmapreg.height;
      set_region(windowreg);
   }

   //
   // If requested by display order, clear background (by copying background bitmap to display) before
   // drawing screen
   //
   if (_displayOrder == DisplayOrder_ClearBackground)
   {
      background_bitmap.copy_to_display();
   }

   //
   //  Add to front of list to ensure visibility
   //
//   attach(NULL);
   _allocated = true;

}  // END of Base_Window ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Base_Window::deallocate_resources ()
{
   if (!_allocated)
      return;

   detach();

   // Deallocate the bitmap resources
   background_bitmap.deallocate_resources ();
   _allocated = false;

}  // END of Base_Window DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// REFRESH_RESOURCES
void Base_Window::refresh_resources  ()
{
   // This virtual method will be overriden by derived classes

}  // END of Base_Window REFRESH_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// RESET_INTERNAL_SCREEN_STATE
void Base_Window::reset_internal_screen_state  ()
{
   // This virtual method will be overriden by derived classes

}  // END of Base_Window RESET_INTERNAL_SCREEN_STATE



////////////////////////////////////////////////////////////////////////////////
// SET_SCREEN_ID
void Base_Window::set_screen_id (GUI_SCREEN_ID screen_id)
{
   window_ID = screen_id;

#if CPU == SIMNT
   array[screen_id] = this;
#endif /* if CPU == SIMNT */
}  // END of Base_Window SET_SCREEN_ID


////////////////////////////////////////////////////////////////////////////////
// SET_SCREEN_INVOCATION_TYPE
void Base_Window::set_screen_invocation_type (GUI_SCREEN_INVOCATION invocation_type)
{
   screen_invocation_type = invocation_type;

}  // END of Base_Window SET_SCREEN_INVOCATION_TYPE



////////////////////////////////////////////////////////////////////////////////
// GET_SCREEN_INVOCATION_TYPE
GUI_SCREEN_INVOCATION Base_Window::get_screen_invocation_type ()
{
   return screen_invocation_type;

}  // END of Base_Window GET_SCREEN_INVOCATION_TYPE



////////////////////////////////////////////////////////////////////////////////
// SET_ALLOCATION_PARAMETER
void Base_Window::set_allocation_parameter (const char* alloc_param)
{
   // Copy no more than MAX-1 characters and (insurance) cauterize the string
   //  with 0 in case we get n or more characters. If a null string is passed,
   //  then the allocation parameter is left untouched. This is done so screens that
   //  are covered/uncovered by child windows will retain their original allocation
   //  parameters on reallocation by the commutator.
   allocation_parameter.initialize();

   if (strlen(alloc_param) > 0)
   {
      strncpy (allocation_parameter, alloc_param, MAXSIZE_BWINDOW_PARAM - 1);
   }

}  // END of Base_Window SET_ALLOCATION_PARAMETER


////////////////////////////////////////////////////////////////////////////////
// GET_ALLOCATION_PARAMETER
const guistring& Base_Window::get_allocation_parameter ()
{
   return(allocation_parameter);

}  // END of Base_Window GET_ALLOCATION_PARAMETER



void Base_Window::screenshotName (guistring& filename)
{
   filename = "SS_";
   Format_date formatter;
   formatter.appendDateAndTime(filename, '_', '_', '_');
}

void Base_Window::screenshot (guistring& filename)
{
   if (filename.length() < 1)
   {
      screenshotName(filename);
   }

   UGL_DEVICE_ID devId = 0;
   UGL_STATUS_MONITOR(uglDriverFind(UGL_DISPLAY_TYPE, 0, (UGL_UINT32*)&devId));

   uglCursorOff(devId);   // Remove mouse from display for snapshot.

   const int bmpWidth  = 640;
   const int bmpHeight = 480;

   struct BITMAPFILEHEADER
   {
      unsigned short bfType;
      unsigned long  bfSize;
      unsigned short bfReserved1;
      unsigned short bfReserved2;
      unsigned long  bfOffBits;
   } __attribute__((packed));

   struct BITMAPINFOHEADER
   {
      unsigned long  biSize;
      unsigned long  biWidth;
      unsigned long  biHeight;
      unsigned short biPlanes;
      unsigned short biBitCount;
      unsigned long  biCompression;
      unsigned long  biSizeImage;
      unsigned long  biXPelsPerMeter;
      unsigned long  biYPelsPerMeter;
      unsigned long  biClrUsed;
      unsigned long  biClrImportant;
   } __attribute__((packed));

   BITMAPFILEHEADER bmpFileHeader;
   BITMAPINFOHEADER bmpInfoHeader;

   size_t           bmpSize = bmpWidth * bmpHeight;

   //
   //  Set fileheader information.
   //
   bmpFileHeader.bfType          = 0x4D42;
   bmpFileHeader.bfSize          = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) + bmpSize * 3;
   bmpFileHeader.bfReserved1     = 0x0000;
   bmpFileHeader.bfReserved2     = 0x0000;
   bmpFileHeader.bfOffBits       = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader);

   bmpInfoHeader.biSize          = sizeof(bmpInfoHeader);
   bmpInfoHeader.biWidth         = bmpWidth;
   bmpInfoHeader.biHeight        = bmpHeight;
   bmpInfoHeader.biPlanes        = 1;
   bmpInfoHeader.biBitCount      = 24;
   bmpInfoHeader.biCompression   = 0;
   bmpInfoHeader.biSizeImage     = bmpSize * 3;
   bmpInfoHeader.biXPelsPerMeter = 0x0EC4;
   bmpInfoHeader.biYPelsPerMeter = 0x0EC4;
   bmpInfoHeader.biClrUsed       = 0;
   bmpInfoHeader.biClrImportant  = 0;

   // Here's all the dynamic memory allocation.  You need a UGL_DIB object,
   // space for the image info in that UGL_DIB object, and space for the
   // image converted to the format in which it will be written to disk.
   UGL_DIB*       screenInfo = (UGL_DIB*)GUI_POOL_MALLOC(sizeof(UGL_DIB));
   screenInfo->pImage = GUI_POOL_MALLOC(bmpSize * 4);
   unsigned char* buffer     = (unsigned char*)GUI_POOL_MALLOC(bmpSize * 3);

   //
   //  Put current screen image information
   //  into a dib structure. (Device Independent Bitmap)
   //
   screenInfo->width       = bmpWidth;
   screenInfo->height      = bmpHeight;
   screenInfo->stride      = bmpWidth;
   screenInfo->colorFormat = UGL_DEVICE_COLOR_32;
   screenInfo->imageFormat = UGL_DIRECT;
   screenInfo->clutSize    = UGL_NULL;
   screenInfo->pClut       = NULL;

   UGL_STATUS_MONITOR(uglBitmapRead (devId, UGL_DISPLAY_ID, 0, 0, bmpWidth - 1, bmpHeight - 1, screenInfo, 0, 0));

   //
   //  Create and open the bmp file.
   //
   char destFileName[250];
   strcpy(destFileName, SNAPSHOTS_PATH);

   DIR* logDir = opendir(destFileName);

   if (logDir == NULL)
   {
      if (mkdir(destFileName) != OK)
      {
         printf("snap directory = \"%s\"\n", destFileName);
      }
   }
   else
   {
      closedir(logDir);
   }

   strcat(destFileName, "/");
   strcat(destFileName, filename);
   strcat(destFileName, ".bmp");

   //
   // Now convert color to RGB value and invert bitmap.
   //
   unsigned int   stride      = bmpWidth * 4;
   unsigned char* copyto      = buffer;
   unsigned char* copyfrom    = (unsigned char*)screenInfo->pImage + bmpWidth * (bmpHeight - 1) * 4;
   unsigned char* placeholder = copyfrom;

   for (int row =  0; row <= (bmpHeight - 1); row++)
   {
      for (int column = 0; column <= (bmpWidth - 1); column++)
      {
         OSPaletteEntry currentPixel = guipalette::palette[*copyfrom];

         copyto[0] = currentPixel.blue;
         copyto[1] = currentPixel.green;
         copyto[2] = currentPixel.red;

         copyto   += 3;
         copyfrom += 4;
      }

      placeholder -= stride;
      copyfrom     = placeholder;
   }

   //  Write bitmap image to file.
   FILE* bmpFile = fopen(destFileName, "wb");

   if (bmpFile)
   {
      fwrite(&bmpFileHeader, sizeof(bmpFileHeader), 1, bmpFile);
      fwrite(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, bmpFile);
      fwrite(buffer, bmpSize * 3, 1, bmpFile);
      fclose(bmpFile);  // Close file.
   }
   else
   {
      DataLog (log_level_gui_error) << "failed to open screenshot file: " << destFileName << endmsg;
   }

   UGL_STATUS_MONITOR(GUI_POOL_FREE(screenInfo->pImage));
   UGL_STATUS_MONITOR(GUI_POOL_FREE(screenInfo));
   UGL_STATUS_MONITOR(GUI_POOL_FREE(buffer));

   uglCursorOn(devId);  // Reactivate mouse.
}


#if CPU == SIMNT
Base_Window* Base_Window::array[GUI_SCREEN_TOTAL_SCREENS];

void Base_Window::setScreenshotName (const char* name)
{
   _txtScreenshotName.set_text_direct(sizeof(name), name);
   _txtScreenshotName.allocate_resources(*this);
}

void Base_Window::removeScreenshotName ()
{
   _txtScreenshotName.deallocate_resources();
}

void Base_Window::setScreenshotInfo (const char* info)
{
   _txtScreenshotInfo.set_text_direct(sizeof(info), info);
   _txtScreenshotInfo.allocate_resources(*this);
}

void Base_Window::removeScreenshotInfo ()
{
   _txtScreenshotInfo.deallocate_resources();
}

#endif /* if CPU == SIMNT */

/* FORMAT HASH bd09db47f34fc0f17bd03a5a2b1a8254 */
