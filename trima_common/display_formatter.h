/*******************************************************************************
 *
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      display_formatter.h
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file contains common conversion routines used by GUI, VIP, and Datalogger
 *             to format common displayed values
 *
 * HISTORY:    $Log: display_formatter.h $
 * HISTORY:    Revision 1.2  2003/08/28 15:52:58Z  rm70006
 * HISTORY:    IT 6366.  Get rid of unneeded functions.  Proc does all value rounding now.
 * HISTORY:    Revision 1.1  2003/08/22 14:36:44Z  rm70006
 * HISTORY:    Initial revision
 *
 *******************************************************************************/

class DisplayFormatter
{
public:
   DisplayFormatter();
   ~DisplayFormatter();

   static int FormatPlateletYield (float y);            // converts to e10 (2 digit only)

   static float CalculateRemainingProcedureTime (float currentTime, float targetTime);

   static void CalculateRbcs (bool isRasSet, bool isDrbc, bool isSplit, float collectedRbcs, float estimatedRbcs, float rbcProd1, float rbcProd2, int& finalCurrRbc, int& finalEstRbc);

   static int CalculateCollectVolForRAS (float collectVol);

};

/* FORMAT HASH e1cd2707bf1666c5b6665fd93e531bad */
