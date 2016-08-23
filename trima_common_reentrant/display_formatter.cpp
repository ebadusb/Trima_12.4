/*******************************************************************************
 *
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      display_formatter.cpp
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file contains common conversion routines used by GUI, VIP, and Datalogger
 *             to format common displayed values
 *
 * HISTORY:    $Log: display_formatter.cpp $
 * HISTORY:    Revision 1.4  2007/11/16 00:16:29Z  dslausb
 * HISTORY:    Added VxWorks.exe includes in source files where missing
 * HISTORY:    Revision 1.3  2003/08/28 15:53:56Z  rm70006
 * HISTORY:    IT 6366.  Delete unneeded functions and make fix to time remaining function.
 * HISTORY:    Revision 1.2  2003/08/22 20:12:26Z  rm70006
 * HISTORY:    IT 6366.  fix function name by adding it to class.
 * HISTORY:    Revision 1.1  2003/08/22 14:37:03Z  rm70006
 * HISTORY:    Initial revision
 *
 *******************************************************************************/

#include <vxworks.h>
#include "display_formatter.h"

#include <math.h>    // Gets floor function

#include "cobeconfig.h"

//
// Constructor
//
DisplayFormatter::DisplayFormatter()
{}



//
// Destructor
//
DisplayFormatter::~DisplayFormatter()
{}



//
// FormatPlateletYield
// converts to a scaled integer representing platelet yield at e10 (2 digit only)
//
int DisplayFormatter::FormatPlateletYield (float y)
{
   return (int)(floor( (y + 0.05f) * 10) );   // Put in E10, 1 significant digit
}



//
// CalculateRemainingProcedureTime
//
float DisplayFormatter::CalculateRemainingProcedureTime (float currentTime, float targetTime)
{
   return ceil(currentTime + targetTime) - (2 * currentTime);
}



//
// Calculate current RBC collect and estimated bag volumes
//
void DisplayFormatter::CalculateRbcs (bool isRasSet, bool isDrbc, bool isSplit, float collectedRbcs, float estimatedRbcs, float rbcProd1, float rbcProd2, int& finalCurrRbc, int& finalEstRbc)
{
   float currentRbcsCollected   = collectedRbcs;
   float estimatedRbcsPredicted = estimatedRbcs;

   // If this is a PTF set, we need to think about subtracting filter loss.
   if (isRasSet)
   {
      // If this is a DRBC set and we're collecting a double, subtract two filters' worth.
      // IT 8306 - Always determine whether this is prompted split based on configured
      // volumes with no scaling or adjustments
      if (isDrbc && isSplit)
      {
         // Do it this way to avoid volume anomolies from the PTF filter loss.
         float currentProd1Vol = rbcProd1 - CobeConfig::data().mssPtfFilterLoss;
         float currentProd2Vol = rbcProd2 - CobeConfig::data().mssPtfFilterLoss;

         // Keep values from going negative
         currentProd1Vol        = (currentProd1Vol < 0.0f) ? 0.0f : currentProd1Vol;
         currentProd2Vol        = (currentProd2Vol < 0.0f) ? 0.0f : currentProd2Vol;

         currentRbcsCollected   = currentProd1Vol + currentProd2Vol;
         estimatedRbcsPredicted = estimatedRbcsPredicted - (2 * CobeConfig::data().mssPtfFilterLoss);
      }
      else // Otherwise, just one filter's worth is fine.
      {
         currentRbcsCollected   = currentRbcsCollected - CobeConfig::data().mssPtfFilterLoss;
         estimatedRbcsPredicted = estimatedRbcsPredicted - CobeConfig::data().mssPtfFilterLoss;
      }

      // Make sure we didn't go negative.
      currentRbcsCollected   = (currentRbcsCollected < 0)   ? 0.0f : currentRbcsCollected;
      estimatedRbcsPredicted = (estimatedRbcsPredicted < 0) ? 0.0f : estimatedRbcsPredicted;
   }

   finalCurrRbc = (int)(currentRbcsCollected + 0.5);
   finalEstRbc  = (int)(estimatedRbcsPredicted + 0.5);
}

int DisplayFormatter::CalculateCollectVolForRAS (float collectVol)
{
   return( (int)(collectVol + 0.5));
}

/* FORMAT HASH 050d61b7655bb844d12a4d801532d8c9 */
