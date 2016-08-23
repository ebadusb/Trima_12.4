/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //depot/main/embedded/Trima/Main/gui/guipalette.cpp#1 $
 */

#include <vxworks.h>
#include "guipalette.h"

OSColor guipalette::meter_green  = 0;
OSColor guipalette::meter_yellow = 0;
OSColor guipalette::meter_red    = 0;


OSColor guipalette::lookupColor (unsigned char red, unsigned char green, unsigned char blue)
{
   OSColor match      = 0;
   int     matchScore = 999999;

   for (int i = 0; i<OSPaletteSize && matchScore!=0; i++)
   {
      int redScore   = red - palette[i].red;
      int greenScore = green - palette[i].green;
      int blueScore  = blue - palette[i].blue;
      int score      = redScore * redScore + greenScore * greenScore + blueScore * blueScore;

      if (i==0 || score < matchScore)
      {
         match      = i;
         matchScore = score;
      }
   }

   return match;
}

/* FORMAT HASH 5c473c15427a1eabf8391aa1f291f9c9 */
