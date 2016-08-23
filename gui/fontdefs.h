/*
 *	Copyright (c) 2008 by CaridianBCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#ifndef __FONT_DEFS_H__
#define __FONT_DEFS_H__

#include <ctype.h>
#include "../src/ugl/driver/font/agfa/itype/object.h"

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

enum { MaxMyFonts = 24 };    //  CGUIGraphics - MAX_FONTS = 1200, 1200 / 50 = 24

void dumpFontTable (void);

void initializeFontTable (void);

void loadFontFileToMemory (const char* fontName);
void unloadFontFileFromMemory (int index);

int getFontIndex (const char* fontName);
int getFontIndexFromTableIndex (int tableIndex);

int         checkFileCRC (const char* fileName, const char* crcName);
const char* getFontFamilyName (const int fontId);

const MY_FONT* getFontStruct (int tableIndex);

#ifdef __cplusplus
}  // extern "C"
#endif /* ifdef __cplusplus */

#endif // #ifndef __FONT_DEFS_H__

/* FORMAT HASH 6d3b1f974f41504e30e1ce8423c71d2d */
