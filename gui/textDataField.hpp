/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  TextDataField

*     File name:   textDataField.cpp

*     Contents:    Implementations of class methods

*     Description: This class just organizes text together for screens like the run summaries.
        It's ideal for screens that have a whole bunch of data that's formatted with a label,
        some numerical data, followed by units.
        All text items are still completely formatted by styling records for ease of localization.
        This class also provides functions for setting the data that are very helpful, as well.

*************************< FOCUSSED GUI DOMAIN >****************************
* CHANGELOG * $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/textDataField.hpp 1.6 2008/06/12 23:37:25Z dslausb Exp dslausb $
* CHANGELOG * $Log: textDataField.hpp $
* CHANGELOG * Revision 1.6  2008/06/12 23:37:25Z  dslausb
* CHANGELOG * IT 8679 - Dashes for zeroes
* CHANGELOG * Revision 1.5  2008/06/09 21:52:15Z  dslausb
* CHANGELOG * IT 8647 - Took setData(Date Struct) out of textDataField.  Use timeDataField instead.
* CHANGELOG * Revision 1.4  2008/06/06 20:04:16Z  mereusb
* CHANGELOG * Made parent pointer protected so it could be accessed by subclasses.
* CHANGELOG * Revision 1.3  2008/06/04 16:39:30  dslausb
* CHANGELOG * IT 8556 - More tweaks to flow adjustment screens
* CHANGELOG * Revision 1.2  2008/06/02 16:46:06Z  dslausb
* CHANGELOG * IT 8464 - Plasma rinseback operator notification
* CHANGELOG * Revision 1.1  2008/05/21 21:44:51Z  dslausb
* CHANGELOG * Initial revision
* CHANGELOG */

#ifndef __TEXT_DATA_FIELD_HPP__
#define __TEXT_DATA_FIELD_HPP__

#include "osgraphics.h"
#include "display_text.h"

#define DASH_STR "-"

// This class just organizes text together for screens like the run summaries.
// It's far easier to read code for a screen that has a half a dozen of these
// than for a screen with a million DisplayText objects, IMHO.
class TextDataField
   : public BlinkableObject
{
protected:
   bool           _useUnits1, _useUnits2, _currentlyAllocated, _dashesForZeros;
   Display_Text   _txtLabel, _txtUnits1, _txtUnits2;

   TEXT_STRING_ID _origTxtLblId, _origTxtDataId, _origTxtUnits1Id, _origTxtUnits2Id;
   OSColor        _origColor;

   Display_Text   _txtData;
   OSWindow*      _myParentPtr;

public:
   TextDataField(TEXT_STRING_ID labelTxtId,
                 TEXT_STRING_ID dataTxtId,
                 TEXT_STRING_ID units1TxtId = TEXT_NULL,
                 TEXT_STRING_ID units2TxtId = TEXT_NULL);

   virtual TextDataField::~TextDataField();

   virtual void allocate_resources (OSWindow& parent);
   virtual void deallocate_resources ();
   virtual void setData (float newData, int numDecimals = 0);
   virtual void setData (int newData);

   virtual void setDashesForZeros ();

   virtual void updateStringIds (TEXT_STRING_ID labelTxtId,
                                 TEXT_STRING_ID dataTxtId,
                                 TEXT_STRING_ID units1TxtId = TEXT_NULL,
                                 TEXT_STRING_ID units2TxtId = TEXT_NULL);

   virtual void updateLocations (TEXT_STRING_ID labelTxtId,
                                 TEXT_STRING_ID dataTxtId,
                                 TEXT_STRING_ID units1TxtId = TEXT_NULL,
                                 TEXT_STRING_ID units2TxtId = TEXT_NULL);

   virtual void updateLabelStringId (TEXT_STRING_ID labelTxtId);
   virtual void updateLabelLocation (TEXT_STRING_ID labelTxtId);

   virtual void updateDataStringId (TEXT_STRING_ID dataTxtId);
   virtual void updateDataLocation (TEXT_STRING_ID dataTxtId);

   virtual void updateUnits1StringId (TEXT_STRING_ID units1TxtId);
   virtual void updateUnits1Location (TEXT_STRING_ID units1TxtId);

   virtual void updateUnits2StringId (TEXT_STRING_ID units2TxtId);
   virtual void updateUnits2Location (TEXT_STRING_ID units2TxtId);

   virtual void blink (bool visible);
   virtual void reset ();
   virtual void blinkInit ();

   virtual bool allocated () { return _currentlyAllocated; }

   friend DataLog_Stream& operator << (DataLog_Stream& os, TextDataField& dataField);
};

#endif // #ifndef __TEXT_DATA_FIELD_HPP__

/* FORMAT HASH 24dd095c785e69efd81447a2ffaa0a20 */
