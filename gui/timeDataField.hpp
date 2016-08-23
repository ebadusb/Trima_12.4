/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:           Mark Eret

*     Class name:       TimeDataField

*     File name:        timeDataField.hpp

*     Contents:         Implementations of class methods

*     Description:      This class inherits from TextDataField and is a specialised subclass
                    for displaying time.
*************************< FOCUSSED GUI DOMAIN >****************************
* CHANGELOG * Revision 1.3  2008/06/12 23:37:29Z  dslausb
* CHANGELOG * IT 8679 - Dashes for zeroes
* CHANGELOG * Revision 1.2  2008/06/09 22:33:11Z  dslausb
* CHANGELOG * IT 8647 - Fixed missing symbol for new timeDataField
* CHANGELOG */

#ifndef __TIME_DATA_FIELD_HPP__
#define __TIME_DATA_FIELD_HPP__

#include "textDataField.hpp"

class TimeDataField
   : public TextDataField
{
public:
   friend DataLog_Stream& operator << (DataLog_Stream& os, TimeDataField& timeData);

protected:
   bool           _useAMUnits, _usePMUnits;
   Display_Text   _txtAMUnits, _txtPMUnits;
   TEXT_STRING_ID _origTxtAMUnitsId, _origTxtPMUnitsId;

private:
   // Make these guys private.  These don't make sense for TimeDataField.  Note,
   // this really indicates an OO design issue (see Meyers' "All birds can fly,
   // penguins are birds, penguins can't fly, uh-oh")
   virtual void setData (float newData, int numDecimals = 0)
   {_FATAL_ERROR(__FILE__, __LINE__, "setData(float,int) doesn't make sense for TimeDataField"); }
   virtual void setData (int newData)
   {_FATAL_ERROR(__FILE__, __LINE__, "setData(int) doesn't make sense for TimeDataField"); }

protected:
   void updateTimeUnits ();

protected:

public:
   TimeDataField(TEXT_STRING_ID labelTxtId,
                 TEXT_STRING_ID dataTxtId,
                 TEXT_STRING_ID txtIdAMUnits,
                 TEXT_STRING_ID txtIdPMUnits);

   virtual TimeDataField::~TimeDataField();

   virtual void allocate_resources (OSWindow& parent);
   virtual void deallocate_resources ();

   // set the hour to a negative number to get dashes instead of a time
   // also turns off the am/pm units
   virtual void setData (struct tm time);

   virtual void updateStringIds (TEXT_STRING_ID labelTxtId,
                                 TEXT_STRING_ID dataTxtId,
                                 TEXT_STRING_ID txtIdAMUnits = TEXT_NULL,
                                 TEXT_STRING_ID txtIdPMUnits = TEXT_NULL);

   virtual void updateLocations (TEXT_STRING_ID labelTxtId,
                                 TEXT_STRING_ID dataTxtId,
                                 TEXT_STRING_ID txtIdAMUnits = TEXT_NULL,
                                 TEXT_STRING_ID txtIdPMUnits = TEXT_NULL);

   virtual void updateAMUnitsStringId (TEXT_STRING_ID txtIdAMUnits);
   virtual void updateAMUnitsLocation (TEXT_STRING_ID txtIdAMUnits);

   virtual void updatePMUnitsStringId (TEXT_STRING_ID txtIdPMUnits);
   virtual void updatePMUnitsLocation (TEXT_STRING_ID txtIdPMUnits);

   virtual void blink (bool visible);
   virtual void reset ();
   virtual void blinkInit ();
};

#endif // #ifndef __TEXT_DATA_FIELD_HPP__

/* FORMAT HASH 67bde8f04de47eb6656c41069c5ec353 */
