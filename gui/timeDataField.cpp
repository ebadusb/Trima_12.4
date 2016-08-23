/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:      Mark Eret

*     Class name:  TimeDataField

*     File name:   timeDataField.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >*****************************
* CHANGELOG * Revision 1.4  2008/10/01 16:08:10Z  dslausb
* CHANGELOG * Change CDS objects so they're not pointers
* CHANGELOG * Revision 1.3  2008/06/09 22:33:07Z  dslausb
* CHANGELOG * IT 8647 - Fixed missing symbol for new timeDataField
* CHANGELOG */


#include <vxworks.h>
#include "timeDataField.hpp"
#include "guiglobs.hpp"

typedef TEXT_STRING_ID StrId;

TimeDataField::TimeDataField(StrId labelTxtId, StrId dataTxtId, StrId txtIdAMUnits, StrId txtIdPMUnits)
   : TextDataField(labelTxtId, dataTxtId),
     _txtAMUnits(txtIdAMUnits),
     _txtPMUnits(txtIdPMUnits),
     _useAMUnits(false),
     _usePMUnits(false),
     _origTxtAMUnitsId(txtIdAMUnits),
     _origTxtPMUnitsId(txtIdPMUnits)
{
   _origColor = _txtData.getColor();
}

TimeDataField::~TimeDataField()
{
   TimeDataField::deallocate_resources();
}

void TimeDataField::allocate_resources (OSWindow& parent)
{
   TextDataField::allocate_resources(parent);

   if (_useAMUnits) _txtAMUnits.allocate_resources(parent);
   if (_usePMUnits) _txtPMUnits.allocate_resources(parent);
}

void TimeDataField::deallocate_resources ()
{
   TextDataField::deallocate_resources();

   if (_useAMUnits) _txtAMUnits.deallocate_resources();
   if (_usePMUnits) _txtPMUnits.deallocate_resources();
}

void TimeDataField::updateTimeUnits ()
{
   if (_useAMUnits == true && _usePMUnits == true)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "This should never occur.  How'd you manage to do this?");
   }
   else
   {
      deallocate_resources();
      allocate_resources(*_myParentPtr);
   }
}

void TimeDataField::updateStringIds (StrId labelTxtId, StrId dataTxtId, StrId txtIdAMUnits, StrId txtIdPMUnits)
{
   _origTxtAMUnitsId = txtIdAMUnits;
   _origTxtPMUnitsId = txtIdPMUnits;

   TextDataField::updateStringIds(labelTxtId, dataTxtId);
   _txtAMUnits.set_string_id(txtIdAMUnits);
   _txtPMUnits.set_string_id(txtIdPMUnits);
}


void TimeDataField::updateLocations (StrId labelTxtId, StrId dataTxtId, StrId txtIdAMUnits, StrId txtIdPMUnits)
{
   TextDataField::updateLocations(labelTxtId, dataTxtId);
   _txtAMUnits.set_location_via_id(txtIdAMUnits);
   _txtPMUnits.set_location_via_id(txtIdPMUnits);
}

void TimeDataField::updateAMUnitsStringId (StrId txtIdAMUnits)
{
   _origTxtAMUnitsId = txtIdAMUnits;
   _txtAMUnits.set_string_id(txtIdAMUnits);
}

void TimeDataField::updateAMUnitsLocation (StrId txtIdAMUnits)
{
   _txtAMUnits.set_location_via_id(txtIdAMUnits);
}

void TimeDataField::updatePMUnitsStringId (StrId txtIdPMUnits)
{
   _origTxtPMUnitsId = txtIdPMUnits;
   _txtPMUnits.set_string_id(txtIdPMUnits);
}

void TimeDataField::updatePMUnitsLocation (StrId txtIdPMUnits)
{
   _txtPMUnits.set_location_via_id(txtIdPMUnits);
}

void TimeDataField::setData (struct tm time)
{
   bool previouslyUsedAMUnits = _useAMUnits;
   bool previouslyUsedPMUnits = _usePMUnits;

   if (time.tm_hour < 0)
   {
      _txtData.set_text_direct(strlen(DASH_STR), DASH_STR);
      _txtAMUnits.set_text_direct(strlen(DASH_STR), DASH_STR);
      _txtPMUnits.set_text_direct(strlen(DASH_STR), DASH_STR);
      _useAMUnits = true;
      _usePMUnits = false;
      updateTimeUnits();
   }
   else
   {

      _txtAMUnits.set_text_via_id(_origTxtAMUnitsId);
      _txtPMUnits.set_text_via_id(_origTxtPMUnitsId);

      char         tmpTxt[6];
      unsigned int hour;

      strftime(tmpTxt, sizeof(tmpTxt), "%H", &time);
      sscanf(tmpTxt, "%d:%d", &hour);

      if (guiglobs::cdsData.config.LangUnit.Get().key_time_format == STANDARD_TIME_FORMAT)
      {
         // The %I gets the AM/PM hour; the %M gets the minutes after the hour
         strftime(tmpTxt, sizeof(tmpTxt), "%I:%M", &time);
         _txtData.set_text_direct(strlen(tmpTxt), tmpTxt);

         if (hour >= 12 && hour <= 23)
         {
            _usePMUnits = true;
            _useAMUnits = false;
         }
         else
         {
            _useAMUnits = true;
            _usePMUnits = false;
         }
      }
      else
      {
         // The %H gets the "24 hour" hour; the %M gets the minutes after the hour
         strftime (tmpTxt, sizeof(tmpTxt), "%H:%M", &time);
         _txtData.set_text_direct(strlen(tmpTxt), tmpTxt);

         _useAMUnits = false;
         _usePMUnits = false;
      }

      if (_useAMUnits != previouslyUsedAMUnits || _usePMUnits != previouslyUsedPMUnits)
         updateTimeUnits();
   }
}

void TimeDataField::blinkInit ()
{
   _txtData.set_color(YELLOW);
   _txtData.set_text_direct(strlen(DASH_STR), DASH_STR);

   if (_useAMUnits)
   {
      _txtAMUnits.set_color(YELLOW);
      _txtAMUnits.set_text_direct(strlen(DASH_STR), DASH_STR);
   }

   if (_usePMUnits)
   {
      _txtPMUnits.set_color(YELLOW);
      _txtPMUnits.set_text_direct(strlen(DASH_STR), DASH_STR);
   }

   _txtPMUnits.deallocate_resources();
}

void TimeDataField::blink (bool visible)
{
   _txtData.blink(visible);
   if (_useAMUnits) _txtAMUnits.blink(visible);
   if (_usePMUnits) _txtPMUnits.blink(visible);
}

void TimeDataField::reset ()
{
   // Data will remain dashes until it gets reset by caller.
   _txtData.setColor(_origColor);

   if (_useAMUnits)
   {
      _txtAMUnits.setColor(_origColor);
      _txtAMUnits.set_text_via_id(_origTxtAMUnitsId);
   }

   if (_usePMUnits)
   {
      _txtPMUnits.setColor(_origColor);
      _txtPMUnits.set_text_via_id(_origTxtPMUnitsId);
   }

   blink(true);  // Make stuff visible
}

DataLog_Stream& operator << (DataLog_Stream& os, TimeDataField& timeData)
{
#if 0   // Debug only
        // If the data object isn't allocated, don't print anything.
   if (timeData._currentlyAllocated)
      os << timeData._txtData << (timeData._useAMUnits ? " AM" : "")
         << (timeData._usePMUnits ? " PM" : "") << endmsg;
#endif
   return os;
}

/* FORMAT HASH e4d1d8c247b84da3314b9ea41c1ddc23 */
