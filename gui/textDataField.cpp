/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  TextDataField

*     File name:   textDataField.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************
* $Header$
*
*/


#include <vxworks.h>
#include "textDataField.hpp"
#include "guiglobs.hpp"
#include "frmtdec.hpp"

TextDataField::TextDataField(TEXT_STRING_ID labelTxtId,
                             TEXT_STRING_ID dataTxtId,
                             TEXT_STRING_ID units1TxtId = TEXT_NULL,
                             TEXT_STRING_ID units2TxtId = TEXT_NULL) :

   _origTxtLblId(labelTxtId),
   _origTxtDataId(dataTxtId),
   _origTxtUnits1Id(units1TxtId),
   _origTxtUnits2Id(units2TxtId),

   _txtLabel(labelTxtId),
   _txtData(dataTxtId),
   _txtUnits1(units1TxtId),
   _txtUnits2(units2TxtId),
   _useUnits1(!(units1TxtId==TEXT_NULL||units1TxtId==textEmpty)),
   _useUnits2(!(units2TxtId==TEXT_NULL||units2TxtId==textEmpty)),
   _currentlyAllocated(false),
   _dashesForZeros(false),
   _myParentPtr(NULL)
{
   _origColor = _txtData.getColor();
}

TextDataField::~TextDataField()
{
   // Parent pointer is somebody else's, so don't delete, just
   // set our copy to NULL.
   _myParentPtr = NULL;

   // Make sure we're deallocated.
   TextDataField::deallocate_resources();
}

void TextDataField::updateStringIds (TEXT_STRING_ID labelTxtId,
                                     TEXT_STRING_ID dataTxtId,
                                     TEXT_STRING_ID units1TxtId = TEXT_NULL,
                                     TEXT_STRING_ID units2TxtId = TEXT_NULL)
{
   _origTxtLblId    = labelTxtId;
   _origTxtDataId   = dataTxtId;
   _origTxtUnits1Id = units1TxtId;
   _origTxtUnits2Id = units2TxtId;

   _origColor       = _txtData.getColor();

   _txtLabel.set_string_id(labelTxtId);
   _txtData.set_string_id(dataTxtId);
   _txtUnits1.set_string_id(units1TxtId);
   _txtUnits2.set_string_id(units2TxtId);

   bool previouslyUsedUnits1 = _useUnits1;
   bool previouslyUsedUnits2 = _useUnits2;

   _useUnits1 = (!(units1TxtId==TEXT_NULL||units1TxtId==textEmpty));
   _useUnits2 = (!(units2TxtId==TEXT_NULL||units2TxtId==textEmpty));

   // See if anything changed as far as what units we display.
   // If so, update screen.
   if (_currentlyAllocated && _myParentPtr &&
       ((previouslyUsedUnits1!=_useUnits1)||
        (previouslyUsedUnits2!=_useUnits2)))
   {
      deallocate_resources();
      allocate_resources(*_myParentPtr);
   }
}


void TextDataField::updateLocations (TEXT_STRING_ID labelTxtId,
                                     TEXT_STRING_ID dataTxtId,
                                     TEXT_STRING_ID units1TxtId = TEXT_NULL,
                                     TEXT_STRING_ID units2TxtId = TEXT_NULL)
{
   _txtLabel.set_location_via_id(labelTxtId);
   _txtData.set_location_via_id(dataTxtId);
   _txtUnits1.set_location_via_id(units1TxtId);
   _txtUnits2.set_location_via_id(units2TxtId);

   bool previouslyUsedUnits1 = _useUnits1;
   bool previouslyUsedUnits2 = _useUnits2;

   _useUnits1 = (!(units1TxtId==TEXT_NULL||units1TxtId==textEmpty));
   _useUnits2 = (!(units2TxtId==TEXT_NULL||units2TxtId==textEmpty));

   // See if anything changed as far as what units we display.
   // If so, update screen.
   if (_currentlyAllocated && _myParentPtr &&
       ((previouslyUsedUnits1!=_useUnits1)||
        (previouslyUsedUnits2!=_useUnits2)))
   {
      deallocate_resources();
      allocate_resources(*_myParentPtr);
   }
}

void TextDataField::updateLabelStringId (TEXT_STRING_ID labelTxtId)
{
   _origTxtLblId = labelTxtId;
   _txtLabel.set_string_id(labelTxtId);
}

void TextDataField::updateLabelLocation (TEXT_STRING_ID labelTxtId)
{
   _txtLabel.set_location_via_id(labelTxtId);
}

void TextDataField::updateDataStringId (TEXT_STRING_ID dataTxtId)
{
   _origTxtDataId = dataTxtId;
   _txtData.set_string_id(dataTxtId);
   _origColor     = _txtData.getColor();
}

void TextDataField::updateDataLocation (TEXT_STRING_ID dataTxtId)
{
   _txtData.set_location_via_id(dataTxtId);
}

void TextDataField::updateUnits1StringId (TEXT_STRING_ID units1TxtId)
{
   _origTxtUnits1Id = units1TxtId;
   _txtUnits1.set_string_id(units1TxtId);

   bool previouslyUsedUnits1 = _useUnits1;
   _useUnits1 = (!(units1TxtId==TEXT_NULL||units1TxtId==textEmpty));

   // See if anything changed as far as what units we display.
   // If so, update screen.
   if (_currentlyAllocated && _myParentPtr && previouslyUsedUnits1 != _useUnits1)
   {
      deallocate_resources();
      allocate_resources(*_myParentPtr);
   }
}

void TextDataField::updateUnits1Location (TEXT_STRING_ID units1TxtId)
{
   _txtUnits1.set_location_via_id(units1TxtId);


   bool previouslyUsedUnits1 = _useUnits1;
   _useUnits1 = (!(units1TxtId==TEXT_NULL||units1TxtId==textEmpty));

   // See if anything changed as far as what units we display.
   // If so, update screen.
   if (_currentlyAllocated && _myParentPtr && previouslyUsedUnits1 != _useUnits1)
   {
      deallocate_resources();
      allocate_resources(*_myParentPtr);
   }
}

void TextDataField::updateUnits2StringId (TEXT_STRING_ID units2TxtId)
{
   _origTxtUnits2Id = units2TxtId;
   _txtUnits2.set_string_id(units2TxtId);

   bool previouslyUsedUnits2 = _useUnits2;
   _useUnits2 = (!(units2TxtId==TEXT_NULL||units2TxtId==textEmpty));

   // See if anything changed as far as what units we display.
   // If so, update screen.
   if (_currentlyAllocated && _myParentPtr && previouslyUsedUnits2 != _useUnits2)
   {
      deallocate_resources();
      allocate_resources(*_myParentPtr);
   }
}

void TextDataField::updateUnits2Location (TEXT_STRING_ID units2TxtId)
{
   _txtUnits2.set_location_via_id(units2TxtId);

   bool previouslyUsedUnits2 = _useUnits2;
   _useUnits2 = (!(units2TxtId==TEXT_NULL||units2TxtId==textEmpty));

   // See if anything changed as far as what units we display.
   // If so, update screen.
   if (_currentlyAllocated && _myParentPtr && previouslyUsedUnits2 != _useUnits2)
   {
      deallocate_resources();
      allocate_resources(*_myParentPtr);
   }
}

void TextDataField::allocate_resources (OSWindow& parent)
{
   _currentlyAllocated = true;
   _myParentPtr        = &parent;
   _txtLabel.allocate_resources(parent);
   _txtData.allocate_resources(parent);
   if (_useUnits1) _txtUnits1.allocate_resources(parent);
   if (_useUnits2) _txtUnits2.allocate_resources(parent);
}

void TextDataField::deallocate_resources ()
{
   _currentlyAllocated = false;
   _myParentPtr        = NULL;
   _txtLabel.deallocate_resources();
   _txtData.deallocate_resources();
   if (_useUnits1) _txtUnits1.deallocate_resources();
   if (_useUnits2) _txtUnits2.deallocate_resources();
}

// setData function for a float (you can specify number of decimals, too).
void TextDataField::setData (float newData, int numDecimals = 0)
{
   if (numDecimals > 9)
   {
      // Greater than 9 would cause a buffer overflow when we create the format string.
      DataLog (log_level_gui_error) << "Too many digits requested. Misuse of function." << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "Misuse of setData function in textDataField");
   }

   guistring dataString;

   if (_dashesForZeros)
   {
      if (newData == 0.0f)
      {
         _txtData.set_text_direct(strlen(DASH_STR), DASH_STR);
         _txtUnits1.set_text_direct(strlen(DASH_STR), DASH_STR);
         _txtUnits2.deallocate_resources();
         _useUnits2 = false;
         return;
      }
      else
      {
         _txtUnits1.set_text_via_id(_origTxtUnits1Id);
         _txtUnits2.set_text_via_id(_origTxtUnits2Id);
      }
   }

   char formatString[8];  // = "%0.1f";
   sprintf(formatString, "%%0.%df", numDecimals);
   sprintf_l10n(dataString, formatString, newData);

   _txtData.set_text_direct(dataString.length(), dataString);
}

// setData function for an int
void TextDataField::setData (int newData)
{
   guistring dataString;

   if (_dashesForZeros)
   {
      if (newData == 0)
      {
         _txtData.set_text_direct(strlen(DASH_STR), DASH_STR);
         _txtUnits1.set_text_direct(strlen(DASH_STR), DASH_STR);
         _txtUnits2.deallocate_resources();
         return;
      }
      else
      {
         _txtUnits1.set_text_via_id(_origTxtUnits1Id);
         _txtUnits2.set_text_via_id(_origTxtUnits2Id);
      }
   }

   // Make sure we get delimiters in there, if necessary.
   sprintf_l10n(dataString, "%d", newData);
   _txtData.set_text_direct(dataString.length(), dataString);
}

// Note: currently this must be called prior to setting the data.
// That is, you won't get the effect until setData has been called.
void TextDataField::setDashesForZeros () { _dashesForZeros = true; }


void TextDataField::blinkInit ()
{
   _txtData.set_color(YELLOW);
   _txtData.set_text_direct(strlen(DASH_STR), DASH_STR);

   if (_useUnits1)
   {
      _txtUnits1.set_color(YELLOW);
      _txtUnits1.set_text_direct(strlen(DASH_STR), DASH_STR);
   }

   if (_useUnits2)
   {
      _txtUnits2.set_color(YELLOW);
      _txtUnits2.set_text_direct(strlen(DASH_STR), DASH_STR);
   }

   _txtUnits2.deallocate_resources();
}

void TextDataField::blink (bool visible)
{
   _txtData.blink(visible);
   if (_useUnits1) _txtUnits1.blink(visible);
   if (_useUnits2) _txtUnits2.blink(visible);
}

void TextDataField::reset ()
{
   // Data will remain dashes until it gets reset by caller.
   _txtData.setColor(_origColor);

   if (_useUnits1)
   {
      _txtUnits1.setColor(_origColor);
      _txtUnits1.set_text_via_id(_origTxtUnits1Id);
   }

   if (_useUnits2)
   {
      _txtUnits2.setColor(_origColor);
      _txtUnits2.set_text_via_id(_origTxtUnits2Id);
   }

   blink(true);  // Make stuff visible
}

DataLog_Stream& operator << (DataLog_Stream& os, TextDataField& dataField)
{
   if (dataField._currentlyAllocated) // If this isn't allocate, don't print anything.
   {
      // Get the data string so we can parse the number
      guistring dataStr;
      dataField._txtData.get_text_direct(dataStr);
#if 0                                 // For debug only.  Language translation is very expensive call to make in a production release
      os << guiglobs::getEnglishText(dataField._origTxtLblId) << ": ";

      // If dashes for zeroes is set and the data is 0, then print dashes
      if (dataField._dashesForZeros && atof(dataStr.getBufPtr()) == 0.0)
         os << " --";
      // Otherwise, just print the data.
      else
      {
         os << dataField._txtData;
         if (dataField._useUnits1) os << " " << guiglobs::getEnglishText(dataField._origTxtUnits1Id);
         if (dataField._useUnits2) os << " " << guiglobs::getEnglishText(dataField._origTxtUnits2Id);
      }

      os << endmsg;
#else  // production version, only print data without translation
      if (dataField._dashesForZeros && atof(dataStr.getBufPtr()) == 0.0)
      {
         os << " --";
         // Otherwise, just print the data.
      }
      else
      {
         os << dataStr;
      }
#endif
   }
   return os;
}

/* FORMAT HASH bdcd0748626969090efb4a1a5276d28b */
