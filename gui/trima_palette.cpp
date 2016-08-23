/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include "guipalette.h"

OSPaletteEntry guipalette::palette[OSPaletteSize] =
{
   {   0,    0,   0 },
   {  247, 247, 247 },
   {  239, 239, 239 },
   {  231, 231, 231 },
   {  222, 222, 222 },
   {  181, 181, 181 },
   {  173, 173, 173 },
   {  165, 165, 165 },
   {  156, 156, 156 },
   {  148, 148, 148 },
   {  140, 140, 140 },
   {  132, 132, 132 },
   {  123, 123, 123 },
   {  115, 115, 115 },
   {  107, 107, 107 },
   {   99,  99,  99 },
   {   90,  90,  90 },
   {   74,  74,  74 },
   {   57,  57,  57 },
   {   49,  49,  49 },
   {   16,  16,  16 },
   {  222, 214, 214 },
   {  206, 198, 198 },
   {  189, 181, 181 },
   {  173, 165, 165 },
   {  115, 107, 107 },
   {  181, 165, 165 },
   {  156,  90,  90 },
   {  247, 132, 132 },
   {  173,  49,  49 },
   {  156,  33,  33 },
   {  156,  24,  24 },
   {  189,  16,  16 },
   {  239,   8,   0 },
   {  222,   8,   0 },
   {  132,  16,   8 },
   {  173, 107,  99 },
   {  239,  33,   0 },
   {  115,  82,  74 },
   {  239,  74,  16 },
   {  165, 107,  82 },
   {  115,  82,  66 },
   {  206, 148, 115 },
   {  247, 107,  24 },
   {  247, 123,  33 },
   {  148,  99,  57 },
   {  156, 107,  57 },
   {  247, 140,  33 },
   {  140, 107,  66 },
   {  198, 148,  82 },
   {  156, 115,  57 },
   {  156, 123,  66 },
   {  123, 107,  74 },
   {  247, 181,  49 },
   {  255, 198,  49 },
   {  165, 148,  90 },
   {  255, 239, 181 },
   {  206, 173,  49 },
   {  239, 198,  41 },
   {  198, 173,  66 },
   {  181, 156,  49 },
   {  222, 189,  41 },
   {  247, 206,  41 },
   {  206, 189, 107 },
   {  206, 189,  99 },
   {  206, 181,  66 },
   {  222, 214, 165 },
   {  206, 189,  90 },
   {  247, 222,  66 },
   {  255, 222,  41 },
   {  173, 165, 107 },
   {  222, 206,  82 },
   {  198, 189, 107 },
   {  189, 181,  99 },
   {  239, 222,  74 },
   {  231, 214,  66 },
   {  247, 231,  66 },
   {  214, 198,  57 },
   {  255, 247, 132 },
   {  206, 198,  99 },
   {  181, 173,  57 },
   {  255, 239,  66 },
   {  140, 132,  33 },
   {  214, 206,  74 },
   {  255, 247,  57 },
   {  173, 173, 165 },
   {  165, 165, 156 },
   {  148, 148, 140 },
   {  132, 132, 123 },
   {  107, 107,  99 },
   {  132, 132, 115 },
   {  214, 214, 173 },
   {  255, 255, 189 },
   {  222, 222, 115 },
   {  255, 255, 123 },
   {  222, 222,  90 },
   {  132, 132,  49 },
   {  231, 231,  82 },
   {  222, 231, 140 },
   {  206, 214, 140 },
   {  140, 148,  99 },
   {  140, 156,  82 },
   {  123, 140,  57 },
   {  181, 198, 148 },
   {  115, 132,  90 },
   {  123, 140,  99 },
   {  181, 206, 148 },
   {  173, 198, 148 },
   {  115, 132,  99 },
   {   90,  99,  82 },
   {  173, 214, 165 },
   {  165, 173, 165 },
   {  165, 214, 165 },
   {  140, 198, 156 },
   {  140, 173, 156 },
   {  140, 189, 173 },
   {  123, 198, 181 },
   {  107, 189, 181 },
   {  206, 214, 214 },
   {  189, 198, 198 },
   {  140, 148, 148 },
   {  123, 132, 132 },
   {  165, 181, 181 },
   {  206, 239, 239 },
   {   49,  57,  57 },
   {  173, 206, 206 },
   {  107, 132, 132 },
   {  148, 198, 198 },
   {   82, 115, 115 },
   {  140, 198, 198 },
   {  115, 198, 198 },
   {  107, 189, 189 },
   {   90, 181, 181 },
   {   66, 132, 132 },
   {   82, 173, 173 },
   {   82, 181, 181 },
   {   66, 165, 165 },
   {   57, 156, 156 },
   {   41, 132, 132 },
   {   33, 132, 132 },
   {   33, 140, 140 },
   {   24, 132, 132 },
   {   16, 132, 132 },
   {    0, 115, 115 },
   {  107, 165, 173 },
   {   82, 140, 148 },
   {   41,  82,  90 },
   {    8,  49,  57 },
   {    8,  82,  99 },
   {    0,  74,  90 },
   {   66, 132, 148 },
   {   41, 107, 123 },
   {   90, 148, 165 },
   {   57, 115, 132 },
   {    0,  57,  74 },
   {  107, 156, 173 },
   {  198, 222, 231 },
   {   16,  82, 107 },
   {  123, 140, 148 },
   {  140, 173, 189 },
   {  173, 189, 198 },
   {   74,  82,  90 },
   {   41,  74, 107 },
   {    0,  33,  66 },
   {   33,  74, 132 },
   {  165, 173, 189 },
   {   41,  66, 115 },
   {   49,  66, 107 },
   {  148, 165, 214 },
   {  123, 140, 189 },
   {  148, 156, 181 },
   {  140, 148, 181 },
   {   49,  57,  90 },
   {   99, 107, 148 },
   {   41,  49,  90 },
   {   49,  66, 156 },
   {  115, 123, 173 },
   {  107, 115, 165 },
   {   90,  99, 156 },
   {   74,  82, 132 },
   {   66,  74, 123 },
   {   57,  66, 115 },
   {   57,  66, 123 },
   {  115, 123, 181 },
   {  107, 115, 173 },
   {   99, 107, 165 },
   {   82,  90, 148 },
   {   74,  82, 140 },
   {   66,  74, 132 },
   {   49,  57, 115 },
   {   57,  66, 140 },
   {   49,  57, 132 },
   {  181, 181, 189 },
   {  140, 140, 148 },
   {  123, 123, 132 },
   {  148, 148, 165 },
   {  140, 140, 165 },
   {  148, 148, 181 },
   {   82,  82, 140 },
   {   74,  74, 132 },
   {   49,  49, 115 },
   {   49,  41, 123 },
   {  165, 156, 198 },
   {   82,  66, 132 },
   {   66,  41, 140 },
   {   57,  24, 132 },
   {   74,  41, 140 },
   {  107,  82, 156 },
   {   49,   8, 123 },
   {   74,  33, 140 },
   {   57,   8, 132 },
   {  189, 181, 198 },
   {  156, 140, 173 },
   {   99,  74, 115 },
   {   57,  24,  74 },
   {   66,  33,  82 },
   {   49,   8,  66 },
   {  115,  82, 123 },
   {   99,  66, 107 },
   {   82,  49,  90 },
   {   57,  16,  66 },
   {   74,  33,  82 },
   {   49,   0,  57 },
   {  198, 181, 198 },
   {  165, 148, 165 },
   {   74,  24,  66 },
   {  123,  82, 115 },
   {  107,  57,  82 },
   {  107,  16,  57 },
   {  181, 156, 165 },
   {  165,  90, 115 },
   {  140,  57,  82 },
   {  222, 189, 198 },
   {  156, 123, 132 },
   {  231, 173, 189 },
   {  148,  57,  82 },
   {  206, 173, 181 },
   {  173, 140, 148 },
   {  115,  82,  90 },
   {  107,  74,  82 },
   {  189, 123, 140 },
   {  181, 115, 132 },
   {  173, 107, 123 },
   {  148,  82,  99 },
   {  132,  66,  82 },
   {  107,  41,  57 },
   {  214, 140, 156 },
   {  206, 132, 148 },
   {  198, 123, 140 },
   {  156,  82,  99 },
   {   82,  41,  49 },
   {  156,  57,  74 },
   {  140,  33,  49 },
   {  148,  33,  41 },
   {  165,   8,  16 },
   {    0,   0,   0 }
};

/* FORMAT HASH ec3b084dd320bdeb233cf3b79bc2f559 */
