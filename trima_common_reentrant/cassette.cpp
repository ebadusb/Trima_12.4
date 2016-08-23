/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cassette.cpp
 *             Focussed system cassette functions.
 *
 * ABSTRACT:   This file contains the encapsulation for the cassette.
 *
 */
#include <vxworks.h>

#include <cmath>
#include <cstdio>

#include "cassette.h"
#include "guiproc.h"
#include "trima_datalog.h"
#if (_MSC_VER == 1200)
// If you are using Microsoft Visual Studio 6.0
// create your own minmax.h that defines the 'max' function
// and add it to your path.
#include "minmax.h"
#else
using std::max;
#endif

static const float plateletBagVolume = 1300.0;
static const float plasmaBagVolume   = 1000.0;
static const float rbcBagDose        =
   MaxRBCProductDose + ( MaxRBCProductDose * 0.10 );       // dose, not collect volume ( allow 10% over max storage range )


//
//
// -----------------------------------------------------------------------------
cassette::cassette()
{}

cassette::~cassette()
{}

const int cassette::compatible (cassetteType type,
                                const float plateletVolume,
                                const float plasmaVolume,
                                const float rbcDose) const
{
   int conditionTrue;
   //
   //
   //  Special conditions: not loaded or unknown type.
   //
   if (type == UNKNOWN)
   {
      DataLog(log_level_trima_library_error) << "Unknown cassette type." << endmsg;
      return 0;
   }

   float maxplatelet = 0.0;
   float maxplasma   = 0.0;
   float maxrbcs     = 0.0;

   if (type != NOT_LOADED)
   {
      volumes(type, maxplatelet, maxplasma, maxrbcs);
   }
   else // not loaded, see if compatible with any set
   {
      maxplatelet = maxplasma = maxrbcs = 0.0;
      type        = PLTPLSRBC;
      volumes(type, maxplatelet, maxplasma, maxrbcs);

      conditionTrue =     ( floor(maxplatelet - plateletVolume) >= 0 )
                      && ( floor(maxplasma - plasmaVolume) >= 0 )
                      && ( floor(maxrbcs - rbcDose) >= 0 );

      if (!conditionTrue)
      {
         maxplatelet = maxplasma = maxrbcs = 0.0;
         type        = RBCPLS;
         volumes(type, maxplatelet, maxplasma, maxrbcs);

      }

   }

   conditionTrue =    ( floor(maxplatelet - plateletVolume) >= 0 )
                   && ( floor(maxplasma - plasmaVolume) >= 0 )
                   && ( floor(maxrbcs - rbcDose) >= 0 );
   if ( !conditionTrue )
   {
      DataLog(log_level_trima_library_info).flags(std::ios::showpoint | std::ios::fixed);
      DataLog(log_level_trima_library_info).precision(2);
      DataLog(log_level_trima_library_info) << "Cassette type " << type << " limits: "
                                            << "maxplt->" << maxplatelet << " (targ->" << plateletVolume << "), "
                                            << "maxpls->" << maxplasma << " (targ->" << plasmaVolume << "), "
                                            << "maxrbc->" << maxrbcs << " (targ->" << rbcDose << ")" << endmsg;
   }
   return conditionTrue;
}

const cassette::cassetteType cassette::recommendedType (const float plateletVolume,
                                                        const float plasmaVolume,
                                                        const float rbcDose) const
{
   if (compatible(RBCPLS, plateletVolume, plasmaVolume, rbcDose)) return RBCPLS;
   if (compatible(PLTPLSRBC, plateletVolume, plasmaVolume, rbcDose)) return PLTPLSRBC;
   return UNKNOWN;
}

const float cassette::maxPlasma (cassetteType type,
                                 const float plateletVolume,
                                 const float rbcDose) const
{
   float maxplatelet = 0.0;
   float maxplasma   = 0.0;
   float maxrbcs     = 0.0;

   //
   //
   //   If the current cassette type is "UNKNOWN" then we need to get
   //   the max for each type and return the largest volume.
   //
   if ( type == UNKNOWN || type == NOT_LOADED )
   {
      float pltplsrbc = 0.0f;
      float rbcpls    = 0.0f;
      if ( compatible(PLTPLSRBC, plateletVolume, 0.0f, rbcDose) )
      {
         volumes(PLTPLSRBC, maxplatelet, pltplsrbc, maxrbcs);
      }
      if ( compatible(RBCPLS, plateletVolume, 0.0f, rbcDose) )
      {
         volumes(RBCPLS, maxplatelet, rbcpls, maxrbcs);
      }
      return ((pltplsrbc > rbcpls) ? pltplsrbc : rbcpls);

   }
   //
   //
   //   If the cassette is loaded and we know the type return the max for the
   //   current cassette type only.
   //
   else if ( type != UNKNOWN )
   {
      //
      //  Special condition: not compatible with zero plasma
      //
      if ( !compatible(type, plateletVolume, 0.0, rbcDose) )
         return 0.0;

      volumes(type, maxplatelet, maxplasma, maxrbcs);

      return maxplasma;
   }

   return 0.0;
}


const float cassette::maxRbcDose (cassetteType type) const
{
   float maxplatelet = 0.0;
   float maxplasma   = 0.0;
   float maxrbcs     = 0.0;

   //
   //
   //   If the current cassette type is "UNKNOWN" then we need to get
   //   the max for each type and return the largest volume.
   //
   if ( type == UNKNOWN || type == NOT_LOADED )
   {
      float pltplsrbc = 0.0f;
      float rbcpls    = 0.0f;

      volumes(PLTPLSRBC, maxplatelet, maxplasma, pltplsrbc);
      volumes(RBCPLS, maxplatelet, maxplasma, rbcpls);

      maxrbcs = max(pltplsrbc, rbcpls);
   }
   else volumes(type, maxplatelet, maxplasma, maxrbcs);

   return maxrbcs;
}


void cassette::volumes (cassetteType type,
                        float& platelet,
                        float& plasma,
                        float& rbc) const
{
   platelet = 0.0;
   plasma   = 0.0;
   rbc      = 0.0;

   if ((type != NOT_LOADED) && (type != UNKNOWN))
   {

      switch (type)
      {
         case PLTPLSRBC : // 4-bag set with RBCs
            platelet = 2.0f * plateletBagVolume;
            plasma   = 1.0f * plasmaBagVolume;
            rbc      = 1.0f * rbcBagDose;
            break;
         case RBCPLS : // DRBC set
            platelet = 0.0f * plateletBagVolume;
            plasma   = 1.0f * plasmaBagVolume;
            rbc      = 2.0f * rbcBagDose;
            break;
         default :
            _FATAL_ERROR(__FILE__, __LINE__,
                         "Bad enumeration passed to cassette::volumes.");
      }
   }
}



//
DataLog_Stream& operator << (DataLog_Stream& os, cassette::cassetteType value)
{
   switch (value)
   {
      case cassette::NOT_LOADED :
         os << "NOT LOADED";
         break;

      case cassette::UNKNOWN :
         os << "UNKNOWN";
         break;

      case cassette::PLTPLSRBC :
         os << "PLT-PLS-RBC";
         break;

      case cassette::RBCPLS :
         os << "RBC-PLS";
         break;

      case cassette::END_OF_LIST :
         os << "END OF LIST";
         break;

      default :
         break;
   }

   os << "(" << int(value) << ")";

   return os;

}


CatalogNumber::CatalogNumber(const char* refNum, unsigned int adminCode, const char* barcodeString)
   : _adminCode(adminCode)
{
   strncpy(_refNum, refNum, REF_NUM_SIZE);
   strncpy(_barcode, barcodeString, TUBING_SET_BARCODE_SIZE);

   unsigned int   newRefNum = atoi(refNum);
   CatalogChecker checker(newRefNum, adminCode);

   _functBits = checker.functionCodes();
}



CatalogNumber::CatalogNumber(unsigned int funcBits)
   : _adminCode(0),
     _functBits(funcBits)
{
   // During PFR, the reference number and barcode are not available
   _refNum[0]  = '\0';
   _barcode[0] = '\0';
}

/* FORMAT HASH c4f405b616e603daff1b721985ffcccf */
