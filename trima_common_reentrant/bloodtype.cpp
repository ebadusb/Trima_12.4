/*******************************************************************************
 *
 * Copyright (c) 2003 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      bloodtype.cpp
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file provides utility functions for the mapping of bloodtypes
 *
 * HISTORY:
 *
 ********************************************************************************/

#include "bloodtype.h"


void BloodTypeString (BLOODTYPE bt, char* btString)
{
   if (bt==BT_UNKNOWN)
      sprintf(btString, "UNKNOWN");
   else
   {
      bool first = true;

      if (bt & BT_O_NEG)
      {
         first = false;
         strcat(btString, "O-");
      }

      if (bt & BT_O_POS)
      {
         if (!first)
            strcat(btString, " ");
         else
            first = false;

         strcat(btString, "O+");
      }

      if (bt & BT_AB_NEG)
      {
         if (!first)
            strcat(btString, " ");
         else
            first = false;

         strcat(btString, "AB-");
      }

      if (bt & BT_AB_POS)
      {
         if (!first)
            strcat(btString, " ");
         else
            first = false;

         strcat(btString, "AB+");
      }

      if (bt & BT_A_NEG)
      {
         if (!first)
            strcat(btString, " ");
         else
            first = false;

         strcat(btString, "A-");
      }

      if (bt & BT_A_POS)
      {
         if (!first)
            strcat(btString, " ");
         else
            first = false;

         strcat(btString, "A+");
      }

      if (bt & BT_B_NEG)
      {
         if (!first)
            strcat(btString, " ");
         else
            first = false;

         strcat(btString, "B-");
      }

      if (bt & BT_B_POS)
      {
         if (!first)
            strcat(btString, " ");
         else
            first = false;

         strcat(btString, "B+");
      }
   }
}


DataLog_Stream& operator << (DataLog_Stream& os, const BLOODTYPE bt)
{
   char btStr[MAX_BLOOD_TYPE_STRLEN];

   bzero(btStr, MAX_BLOOD_TYPE_STRLEN);  // Must 0 the string because strcat depends on it.

   BloodTypeString(bt, btStr);

   return os << "(" << btStr << " [0x" << hex << (int)bt << dec << "])";
}

/* FORMAT HASH c2166237f560d20e206f6998685f354a */
