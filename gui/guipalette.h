/*
 * Copyright (c) 2012 by Terumo BCT, Inc.  All rights reserved.
 *
 * Author:  Regis McGarry
 *
 * Description:
 *    This class contains static objects and functions used by both GUI and the
 * LanguageLoader object.  Currently, this object is not global, but is copied
 * between the two tasks.  There are a great deal of dependencies within GUI that
 * makes separating all of the string stuff out a challenge.
 *
 */

#ifndef _GUIPALETTE_INCLUDE
#define _GUIPALETTE_INCLUDE

#include <vxworks.h> // Needed for SIMNT compiler directives
#include "styling_record.h"

class guipalette
{
public:
   // This static object is used by both GUI and the Language Loader
   // It is effectively a constant object that is initialized by the palette cpp file.
   // The init file is linked with the application.  Both normal and Service have their
   // own palette file.
   static OSPaletteEntry palette[OSPaletteSize];

   //
   // Returns closest match in guiglobs::palette for specified color
   //
   static OSColor lookupColor (unsigned char red, unsigned char green, unsigned char blue);

   //
   // Statically initialized colors used by various screens.
   //
   static OSColor meter_green;
   static OSColor meter_yellow;
   static OSColor meter_red;
};

#endif

/* FORMAT HASH 2cabd7d9558f92f26d99e7a7e5d79493 */
