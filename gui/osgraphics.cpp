/*
 *	Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 */

#include <vxWorks.h>
#include <ugl/uglInput.h>
#include <ugl/winWindow.h>
#include <ugl/winMsg.h>
#include <ugl/private/uglWinP.h> /* need to get access to the internals of the WIN_APP_ID */



#include "an_alarm.h"
#include "bitmap_info.h"
#include "error.h"
#include "osgraphics.h"
#include "trima_datalog.h"
#include "zlib.h"
#include "messagesystem.h"
#include "guiglobs.hpp"
#include "guipalette.h"
#include "cgui_pool_allocator.hpp"
#include "fontdefs.h"
#include "../os/discover_hw.h"
#include "language_loader.h"
#include "trima_tasks.h"

#include "osdib.hpp"

#include <stdio.h>

#if CPU==SIMNT
static UGL_ARGB deviceClut[OSPaletteSize];
#endif /* if CPU==SIMNT */

extern void         (* winAppStartupTask)();
extern void         (* winAppIdleTask)();
extern int          winAppIdleTimeoutMSec;

static CallbackBase guiStartCB;
static CallbackBase guiWakeupCB;

static void guiWinAppStartupTask ()
{
   guiglobs::message_system.initNonBlock("winApp", 300);
   guiStartCB();
}

static void guiWinAppIdleTask ()
{
   guiglobs::message_system.dispatchMessages();
   guiWakeupCB();
}


OSDisplay::OSDisplay(const CallbackBase& startCallback, const CallbackBase& wakeupCallback, unsigned int timeoutMsec)
{
   DataLog_Critical criticalLog;

   //
   // Perform basic UGL initialization
   //
   DataLog (log_level_gui_startup_debug) << "Initialize UGL." << endmsg;

   UGL_STATUS status = uglInitialize();

   if ( status != UGL_STATUS_OK )
   {
      printf("uglInitialize failed status=%d\n", status);
      taskSuspend(taskIdSelf());
   }
   else
      DataLog (log_level_gui_startup_debug) << "UGL initialized OK." << endmsg;

   guiStartCB  = startCallback;
   guiWakeupCB = wakeupCallback;

   DataLog (log_level_gui_startup_debug) << "Initialize display driver." << endmsg;
   //
   // Get an ID for the display driver, then check that we are in the proper
   // color mode (other parts of the driver assume 8-bit color).
   //
   UGL_STATUS_MONITOR(uglDriverFind(UGL_DISPLAY_TYPE, 0, (UGL_UINT32*)&_uglDisplay)   );

   winAppStartupTask     = guiWinAppStartupTask;
   winAppIdleTask        = guiWinAppIdleTask;
   winAppIdleTimeoutMSec = timeoutMsec;

   UGL_MODE_INFO modeInfo;

   DataLog (log_level_gui_startup_debug) << "Set display info." << endmsg;

   UGL_STATUS_MONITOR(uglInfo(_uglDisplay, UGL_MODE_INFO_REQ, &modeInfo)   );

   _height = modeInfo.height;
   _width  = modeInfo.width;

   if ( modeInfo.colorModel != UGL_INDEXED_8 )
   {
      DataLog(criticalLog) << "UGL reports color model " << modeInfo.colorModel << " - expected " << (int)UGL_INDEXED_8 << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "gui startup failed");
   }

   DataLog (log_level_gui_startup_debug) << "Create graphics context." << endmsg;

   _uglGc = uglGcCreate(_uglDisplay);

   DataLog (log_level_gui_startup_debug) << "Initialize event service." << endmsg;

   UGL_STATUS_MONITOR(uglDriverFind(UGL_EVENT_SERVICE_TYPE, 0, (UGL_UINT32*)&_uglEventService)   );

   DataLog (log_level_gui_startup_debug) << "Create winApp." << endmsg;

   TrimaTaskInfo info;
   trimaGetTaskInfo(TTID_WinApp, &info);

   // Starts the UGL task and set the router queue for debugging purposes
   _uglApp = winAppCreate((char*)info.taskName, info.priority, info.stackSize, 0, UGL_NULL);
   trimaSetTaskID (TTID_WinApp, _uglApp->taskId);
   trimaSetRouterQ(_uglApp->taskId, guiglobs::message_system.MsgSystem());
   DataLog(log_level_startup_info) << "started task " << info.taskName << " task id " << hex << _uglApp->taskId << dec << endmsg;

   _uglRootWindow = winCreate(_uglApp, UGL_NULL_ID,
                              WIN_ATTRIB_VISIBLE,              // list window attributes
                              0, 0, _width, _height, UGL_NULL, 0, UGL_NULL);

   DataLog (log_level_gui_startup_debug) << "Set root window data." << endmsg;

   winDataSet(_uglRootWindow, NULL, NULL, 0);

   DataLog (log_level_gui_startup_debug) << "Attach root window." << endmsg;

   UGL_STATUS_MONITOR(winAttach(_uglRootWindow, UGL_NULL_ID, UGL_NULL_ID)   );

   DataLog (log_level_gui_startup_debug) << "Find font driver." << endmsg;

   UGL_STATUS_MONITOR(uglDriverFind(UGL_FONT_ENGINE_TYPE, 0, (UGL_UINT32*)&_uglFontDriver)   );

   DataLog (log_level_gui_startup_debug) << "Initialize cursor." << endmsg;

   cursor_init();

   DataLog (log_level_gui_startup_debug) << "Initialize bitmap data structure." << endmsg;

   for ( int i = 0; i<BITMAP_ID_COUNT; i++ )
   {
      _bitmapStatus[i]._uglId     = UGL_NULL_ID;
      _bitmapStatus[i]._loadCount = 0;
   }

   initFonts();

   _option = UGL_FONT_ANTI_ALIASING_16;

   DataLog (log_level_gui_startup_debug) << "OSDisplay initialized." << endmsg;
}

OSDisplay::~OSDisplay()
{}


void OSDisplay::flush ()
{
   //
   // Redraw all dirty windows
   //
   list<OSWindow*>::iterator windowIter = _windowList.begin();

   while ( windowIter != _windowList.end() )
   {
      if (*windowIter) (*windowIter)->draw(_uglGc);
      else
      {
         DataLog (log_level_gui_error) << "We've got a null pointer somewhere in the window list." << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Null window pointer");
      }

      ++windowIter;
   }

   drawRootWindow();
}

void OSDisplay::set_color_entry (OSColor index, const OSPaletteEntry& entry)
{
   //
   // Free color first, in case it has already been allocated.  If not allocated,
   // the function will simply do nothing and return an error status.
   //
   UGL_COLOR uglColor = index;

   // No UGL_STATUS_MONITOR here, because this one will return some errors (it's expected).
   uglColorFree(_uglDisplay, &uglColor, 1);

   //
   // Allocate the color for the associated palette entry
   //
   UGL_ARGB uglARGB       = UGL_MAKE_RGB(entry.red, entry.green, entry.blue);
   UGL_ORD  uglColorIndex = index;
   UGL_STATUS_MONITOR(uglColorAlloc(_uglDisplay, &uglARGB, &uglColorIndex, &uglColor, 1)   );

#if CPU==SIMNT
   deviceClut[index] = uglARGB;
#endif
}

void OSDisplay::initFonts ()
{
   for (int i = 0; i < MAX_FONTS; i++)
      _font[i] = NULL;
}

OSFontId OSDisplay::get_fontWithFontId (const int FontIndex, const unsigned char pixelSize, const unsigned char weight = 0)
{
   const char* familyName = getFontFamilyName(FontIndex);
   return get_font(familyName, FontIndex, pixelSize, weight);
}



OSFontId OSDisplay::get_font (const char* familyName, int fontIndex, unsigned char pixelSize, unsigned char weight = 0)
{
   unsigned char fontSize = pixelSize;

   if (!familyName || strlen(familyName) < 1)
   {
      DataLog (log_level_gui_error) << "NULL font name passed to getFont function." << endmsg;
      return UGL_NULL;
   }

   if (fontSize < 6)
   {
      DataLog (log_level_gui_error) << "Font size " << (int)fontSize << " too small. Set to 6." << endmsg;
      fontSize = 6;
   }

   if (fontSize > 32)
   {
      DataLog (log_level_gui_error) << "Font size " << (int)fontSize << " too big. Set to 32." << endmsg;
      fontSize = 32;
   }

   int calcFontIndex = fontSize + (fontIndex * 50);

   if (_font[calcFontIndex] == NULL)
   {
      static const char fontStringTemplate[] = "familyName=%s; pixelSize=%d";

      // Allocate memory and check for success
      OSFontId font       = UGL_NULL;
      char*    fontString = (char*)GUI_POOL_MALLOC((strlen(familyName) + sizeof(fontStringTemplate) + 3) * sizeof(char));

      if (fontString != NULL)
      {
         UGL_FONT_DEF fontDef = {};

         sprintf(fontString, fontStringTemplate, familyName, fontSize);
         UGL_STATUS_MONITOR(uglFontFindString(_uglFontDriver, fontString, &fontDef)   );
         fontDef.pixelSize = fontSize;

         if ( weight > 0 )
            fontDef.weight =  weight;

         font = uglFontCreate(_uglFontDriver, &fontDef);
         uglFontInfo(font, UGL_FONT_ANTI_ALIASING_SET, &_option);

         GUI_POOL_FREE(fontString);
      }

      if (font == UGL_NULL)
      {
         DataLog (log_level_gui_error) << "Unable to create font: name=\"" << familyName
                                       << "\"; size=" << (int)fontSize
                                       << "; index=" << fontIndex << ")" << endmsg;
      }
     #if 0 /* Leave commented out because dlog statements do take time even if turned off */
      else
      {
         DataLog (log_level_gui_startup_debug) << "Font created: name=\"" << familyName << "\"; size="
                                               << (int)fontSize << "; index=" << fontIndex << "; uglId=" << font << ")" << endmsg;
      }
     #endif

      _font[calcFontIndex] = font;
   }

   return _font[calcFontIndex];
}

void OSDisplay::delete_font (OSFontId font)
{
   DataLog (log_level_gui_info) << "uglFontDestroy()" << endmsg;
   UGL_STATUS_MONITOR(uglFontDestroy(font)   );

   if (font != UGL_NULL)
      font = UGL_NULL;
}

void OSDisplay::cleanupFonts ()
{
   for ( int fontTableIndex = 0 ; fontTableIndex < MaxMyFonts ; fontTableIndex++ )
   {
      int                   fontFamilyIndex = getFontIndexFromTableIndex(fontTableIndex);
      const unsigned int    languageIndex   = guiglobs::cdsData.rwConfig.getLanguageIndex();
      const LanguageHeader* currentLang     = LanguageList::getLanguageHeader(languageIndex);

      if (fontFamilyIndex >= 0 &&
          currentLang != NULL &&
          fontFamilyIndex != currentLang->fontIndex)
      {
         for (int fontSize = 6; fontSize <= 32; fontSize++)
         {
            OSFontId fontId = get_font(currentLang->fontName.c_str(), fontFamilyIndex, fontSize);

            if (fontId != UGL_NULL)
               delete_font(fontId);
         }
      }
   }
}



OSBitmapId OSDisplay::load_bitmap (BITMAP_ID guiId, bool& transparency)
{
   OSBitmapId result = UGL_NULL_ID;

   if ( guiId <= BITMAP_NULL || guiId >= BITMAP_ID_COUNT )
   {
      // requested bitmap does not exist
      DataLog_Critical critical;

      DataLog(critical) << "Attempt to load invalid bitmap " << (int)guiId <<
         "[" << (int)BITMAP_NULL << "," << (int)BITMAP_ID_COUNT << "]" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "Invalid bitmap ID");
   }
   else if (bitmap_data_table[guiId].absolutePath)
   {
      // dynamic file must be loaded at runtime
      load_bitmap_from_file(guiId);
      transparency                     = false; // No transparency for bmps loaded from file.

      result                           = _bitmapStatus[guiId]._uglId;
      _bitmapStatus[guiId]._loadCount += 1;
   }
   else
   {
      if ( _bitmapStatus[guiId]._loadCount > 0 )
      {
         // bitmap already loaded
         result = _bitmapStatus[guiId]._uglId;
         _bitmapStatus[guiId]._loadCount += 1;
      }
      else if ( bitmap_data_table[guiId].dataSize > 0 )
      {
         // compressed image data for bitmap is already in memory
         transparency                     = load_bitmap_from_compressed_data(guiId);

         result                           = _bitmapStatus[guiId]._uglId;
         _bitmapStatus[guiId]._loadCount += 1;
      }
      else
      {
         DataLog_Critical critical;
         DataLog(critical) << "load_bitmap(" << guiId << "; " << transparency << ") not sure how to handle." << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Unable to handle bitmap load.");
      }
   }

   if ( result == UGL_NULL_ID )
   {
      DataLog_Critical critical;
      DataLog(critical) << "Bitmap load failure " << (int)guiId << endmsg;

      anAlarmMsg badload(BITMAP_LOAD_FAILURE);
      badload.setAlarm();
   }

   return result;
}

void OSDisplay::unload_bitmap (BITMAP_ID guiId)
{
   if ( guiId <= BITMAP_NULL || guiId >= BITMAP_ID_COUNT )
   {
      // requested bitmap does not exist
      DataLog_Critical critical;

      DataLog(critical) << "Attempt to unload invalid bitmap " << (int)guiId <<
         "[" << (int)BITMAP_NULL << "," << (int)BITMAP_ID_COUNT << "]" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "Invalid bitmap ID");
   }
   else
   {
      if ( _bitmapStatus[guiId]._loadCount > 0 )
      {
         _bitmapStatus[guiId]._loadCount -= 1;
      }

      if ( _bitmapStatus[guiId]._loadCount == 0 && _bitmapStatus[guiId]._uglId != UGL_NULL_ID )
      {
         UGL_STATUS_MONITOR(uglBitmapDestroy(_uglDisplay, _bitmapStatus[guiId]._uglId)   );
         _bitmapStatus[guiId]._uglId = UGL_NULL_ID;
      }
   }
}

void OSDisplay::displayCursor ()
{
   UGL_CDIB pointerDib;

   /* bitmap of cursor image */
   static UGL_UINT8 pointerData[] =
   {
#define T UGL_CURSOR_COLOR_TRANSPARENT,
#define B 0,
#define W 1,
// 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6
//
      B B B T T T T T T T T T T T T T
      B W W B T T T B B B T T T T T T
      B W W W B T B W W W B B T T T T
      T B W W W B W W W W W W B T T T
      T T B W W W W W W W W W B T T T
      T T T B W W W W W W W W W B T T
      T T B W B W W W W W W W W B T T
      T T T B W W W W W W W W W B B T
      T T T T B W W W W W W W B W W B
      T T T T T B B W W W W B W W B T
      T T T T T T T B B W B W W W W B
      T T T T T T T T T B W W W W B T
      T T T T T T T T T T B W W B T T
      T T T T T T T T T T T B B T T T
#undef T
#undef B
#undef W
   };

   /* color of cursor */
   static UGL_ARGB cursorClut[] =
   {
      UGL_MAKE_ARGB (0xff, 0x00, 0x00, 0x00),
      UGL_MAKE_ARGB (0xff, 0xFF, 0xFF, 0xFF)
   };


   if ( !_cursorDisplayed )
   {
      pointerDib.width     = 16;
      pointerDib.stride    = 16;
      pointerDib.height    = 14;
      pointerDib.hotSpot.x = 0;
      pointerDib.hotSpot.y = 0;
      pointerDib.pClut     = cursorClut;
      pointerDib.clutSize  = 2;
      pointerDib.pImage    = pointerData;

      UGL_CDDB_ID pointerImage = UGL_NULL_RESULT_MONITOR(uglCursorBitmapCreate(_uglDisplay, &pointerDib));
      // if (pointerImage == UGL_NULL)
      //    _FATAL_ERROR(__FILE__, __LINE__, "UGL ERROR: failed to create cursor bitmap");
      UGL_STATUS_MONITOR(uglCursorImageSet(_uglDisplay, pointerImage)   );
      UGL_STATUS_MONITOR(uglCursorOn(_uglDisplay)   );
   }
}

void OSDisplay::cursor_init ()
{
   _cursorDisplayed = false;
   UGL_STATUS_MONITOR(uglCursorInit(_uglDisplay, 20, 20, _width / 2, _height / 2)   );

#if CPU==SIMNT
   displayCursor();
#endif /* if CPU==SIMNT */

}

bool OSDisplay::load_bitmap_from_compressed_data (BITMAP_ID guiId)
{
   BITMAP_DATA_ENTRY& entry    =  bitmap_data_table[guiId];

   unsigned long      bmpSize  = entry.height * entry.width * sizeof(OSColor);
   unsigned long      maskSize = entry.height * entry.width * sizeof(unsigned char);

#if CPU==SIMNT
   // Simulator bitmap:
   OSDib Dib (entry.width,
              entry.height,
              sizeof(OSColor),
              UGL_INDEXED_8,    // simulator display type
              UGL_ARGB8888,     // simulator color format
              OSPaletteSize,    // simulator clut size
              deviceClut);      // simulator clut buffer
#else
   OSDib Dib (entry.width,
              entry.height,
              sizeof(OSColor),
              UGL_DIRECT,       // Trima display type
              UGL_DEVICE_COLOR, // Trima color format
              0, NULL);         // no clut
#endif

   bool bmpHasMask     = (entry.maskData != NULL);

   int  bmpUncmpRetVal = uncompress(Dib.image(), &bmpSize, entry.data, entry.dataSize);

   // On exit from uncompress, the bmpSize parameter contains the actual
   // uncompressed size of the data.  That had better match the size of the
   // memory we set aside for holding it:
   if (bmpSize != entry.height * entry.width)
   {
      DataLog (log_level_gui_error) << "ERROR: Bitmap  " << guiId
                                    << " is " << entry.height
                                    << 'x'    << entry.width
                                    << " = " << entry.height * entry.width
                                    << " but uncompressed to " << bmpSize
                                    << endmsg;

      _FATAL_ERROR(__FILE__, __LINE__, "BITMAP ALLOCATION ERROR: compressed data size mismatch");
   }

   if (bmpUncmpRetVal < Z_OK)
   {
      DataLog (log_level_gui_error) << "Unable to uncompress bitmap (" << bmpUncmpRetVal << ")." << endmsg;

      return false;
   }

   // Set up the mask if necessary.
   OSDibMask mask;

   if (bmpHasMask)
   {
      int byteWidth = (entry.width + 7) / 8;

      mask.load(entry.width, entry.height, 8 * byteWidth);

      int maskUncmpRetVal = Z_OK;
      maskUncmpRetVal = uncompress(mask.image(), &maskSize, entry.maskData, entry.maskDataSize);
      if (maskUncmpRetVal < Z_OK)
      {
         DataLog (log_level_gui_error) << "Unable to uncompress mask (" << maskUncmpRetVal << ")." << endmsg;
         // Don't fail here, just skip the mask.
         bmpHasMask = false;
      }
   }

   if (bmpHasMask)
   {
      _bitmapStatus[guiId]._uglId = uglTransBitmapCreate(_uglDisplay, Dib, mask, UGL_DIB_INIT_DATA, 0, UGL_DEFAULT_MEM);

      if (_bitmapStatus[guiId]._uglId == UGL_NULL)
      {
         DataLog (log_level_gui_error) << "UGL ERROR, can't create transparent bitmap " << guiId << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "UGL ERROR, can't create transparent bitmap");
      }


   }
   else
   {
      _bitmapStatus[guiId]._uglId = uglBitmapCreate(_uglDisplay, Dib, UGL_DIB_INIT_DATA, 0, UGL_DEFAULT_MEM);

      if (_bitmapStatus[guiId]._uglId == UGL_NULL)
      {
         DataLog (log_level_gui_error) << "UGL ERROR, can't create bitmap " << guiId << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "UGL ERROR, can't create bitmap");
      }

   }

   // error checking
   UGL_SIZE result_width, result_height;

   UGL_STATUS_MONITOR (uglBitmapSizeGet (_bitmapStatus[guiId]._uglId, &result_width, &result_height) );

   if ( (result_width  != entry.width ) ||
        (result_height != entry.height) )
   {
      DataLog (log_level_gui_error) << "UGL BITMAP ERROR, bitmap " << guiId << " was supposed to be "
                                    << entry.width << 'x' << entry.height
                                    << " but was found to be "
                                    << result_width << 'x' << result_height
                                    << endmsg;
   }

   return bmpHasMask;

}


void OSDisplay::load_bitmap_from_file (BITMAP_ID guiId)
{
   typedef unsigned char BMP_BYTE;
   typedef unsigned short BMP_WORD;
   typedef unsigned long BMP_DWORD;

   struct BITMAPFILEHEADER
   {
      BMP_WORD  bfType;
      BMP_DWORD bfSize;
      BMP_WORD  bfReserved1;
      BMP_WORD  bfReserved2;
      BMP_DWORD bfOffBits;
   } __attribute__((packed));

   struct BITMAPINFOHEADER
   {
      BMP_DWORD biSize;
      BMP_DWORD biWidth;
      BMP_DWORD biHeight;
      BMP_WORD  biPlanes;
      BMP_WORD  biBitCount;
      BMP_DWORD biCompression;
      BMP_DWORD biSizeImage;
      BMP_DWORD biXPelsPerMeter;
      BMP_DWORD biYPelsPerMeter;
      BMP_DWORD biClrUsed;
      BMP_DWORD biClrImportant;
   } __attribute__((packed));

   BITMAPFILEHEADER bmpFileHeader;
   BITMAPINFOHEADER bmpInfoHeader;

   // Open the bmp file.
   FILE* bmpFile = fopen((const char*)bitmap_data_table[guiId].data, "rb");
   if ( !bmpFile ) return;

   int readSize = fread((void*)&bmpFileHeader, sizeof(bmpFileHeader), 1, bmpFile);
   if ( readSize != 1 ||
        bmpFileHeader.bfType != 0x4d42 )
   {
      fclose(bmpFile);
      return;
   }

   // Read the header
   fread((void*)&bmpInfoHeader, sizeof(bmpInfoHeader), 1, bmpFile);
   if ( bmpInfoHeader.biBitCount != 8 )
   {
      fclose(bmpFile);
      return;
   }

   // Allocate the memory we'll need and check for success
   size_t   bmpSize = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight;

   UGL_DIB* pDib    = (UGL_DIB*)GUI_POOL_MALLOC(sizeof(UGL_DIB) + bmpSize + 256 * sizeof(UGL_RGB));
   if ( pDib == NULL )
   {
      fclose(bmpFile);
      return;
   }

   pDib->width       = bmpInfoHeader.biWidth;
   pDib->height      = bmpInfoHeader.biHeight;
   pDib->stride      = bmpInfoHeader.biWidth;

   pDib->colorFormat = UGL_ARGB8888;
   pDib->imageFormat = UGL_INDEXED_8;
   pDib->clutSize    = 256;

   pDib->pClut       = (UGL_UINT8*)pDib + sizeof(UGL_DIB);
   pDib->pImage      = (UGL_UINT8*)pDib + sizeof(UGL_DIB) + 256 * sizeof(UGL_RGB);

   for ( int color = 0; color<256; color++ )
   {
      int red, green, blue, reserved;

      blue     = fgetc(bmpFile);
      green    = fgetc(bmpFile);
      red      = fgetc(bmpFile);
      reserved = fgetc(bmpFile);  // this is needed, otherwise color table will be messed up

      taskDelay(0);

      ((UGL_RGB*)pDib->pClut)[color] = UGL_MAKE_RGB(red, green, blue);
   }

   //
   // BMP data is aligned on 32-bit words and is upside down
   // compared to WindMl data
   //
   unsigned int stride = (bmpInfoHeader.biWidth + 3) / 4 * 4;
   BMP_BYTE*    pPix   = (BMP_BYTE*)pDib->pImage +
                         bmpInfoHeader.biWidth * (bmpInfoHeader.biHeight - 1);

   fseek(bmpFile, bmpFileHeader.bfOffBits, SEEK_SET);
   for (unsigned int y = 1; y <= bmpInfoHeader.biHeight; y++)
   {
      fread((void*)pPix, bmpInfoHeader.biWidth, 1, bmpFile);

      taskDelay(0);

      if ( stride > bmpInfoHeader.biWidth )
      {
         fseek(bmpFile, stride - bmpInfoHeader.biWidth, SEEK_CUR);
         taskDelay(0);
      }

      pPix -= bmpInfoHeader.biWidth;
   }

   fclose(bmpFile);

   _bitmapStatus[guiId]._uglId     = uglBitmapCreate(_uglDisplay, pDib, UGL_DIB_INIT_DATA, 0, UGL_DEFAULT_MEM);

   bitmap_data_table[guiId].width  = _bitmapStatus[guiId]._uglId->width;
   bitmap_data_table[guiId].height = _bitmapStatus[guiId]._uglId->height;

   UGL_STATUS_MONITOR(GUI_POOL_FREE(pDib)   );

}


void OSDisplay::setCursorPos (int x, int y)
{
   UGL_STATUS_MONITOR(uglCursorMove (_uglDisplay, x, y)   );
}



void OSDisplay::getCursorPos (int& x, int& y)
{
   UGL_STATUS_MONITOR(uglCursorPositionGet (_uglDisplay, &x, &y)   );
}

void OSDisplay::drawRootWindow ()
{
   if (_uglRootWindow != UGL_NULL_ID)
   {
      winDrawStart(_uglRootWindow, _uglGc, true);

      UGL_STATUS_MONITOR(uglLineWidthSet(_uglGc, 0)   );
      UGL_STATUS_MONITOR(uglBackgroundColorSet(_uglGc, 0)   );
      UGL_STATUS_MONITOR(uglRectangle(_uglGc, 0, 0, _width - 1, _height - 1)   );
      UGL_STATUS_MONITOR(winDrawEnd(_uglRootWindow, _uglGc, true)   );
   }
}


void dumpOsColor (DataLog_Stream& os, OSColor colorIndex)
{
   if (colorIndex >= OSPaletteSize)
   {
      DataLog (log_level_gui_error) << "ColorIndex " << colorIndex << " is out of bounds." << endmsg;
      return;
   }

   os << "(r" << (int)guipalette::palette[colorIndex].red << ";g" << (int)guipalette::palette[colorIndex].green
      << ";b" << (int)guipalette::palette[colorIndex].blue << ")";
}

/* FORMAT HASH 98c0611c61b61116cda1384a79495fc5 */
