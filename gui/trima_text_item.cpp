/*
 *  Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>

#include "trima_text_item.h"
#include "trima_variable_database.h"
#include "trima_string_data.h"
#include "../cds/software_cds.h"

static const TrimaUnicodeString startTok("#!{");
static const TrimaUnicodeString endTok("}");

TrimaTextItem::TrimaTextItem(void)
   : _id(),
     _string(),
     _template(),
     _languageId(0),
     _stylingRecord(),
     _hasVariables(false),
     _wasLoaded(false),
     _englishOnly(false)
{}

TrimaTextItem::TrimaTextItem(const char* id, StylingRecord* stylingRecord)
   : _id(),
     _string(),
     _template(),
     _languageId(0),
     _stylingRecord(),
     _hasVariables(false),
     _wasLoaded(false),
     _englishOnly(false)
{
   setId(id);

   if (stylingRecord)
   {
      _stylingRecord = *stylingRecord;
   }
}



TrimaTextItem::TrimaTextItem(const TrimaTextItem& textItem)
   : _id(textItem._id),
     _string(textItem._string),
     _template(textItem._template),
     _languageId(textItem._languageId),
     _stylingRecord(textItem._stylingRecord),
     _hasVariables(textItem._hasVariables),
     _wasLoaded(textItem._wasLoaded),
     _englishOnly(textItem._wasLoaded)
{}



//
// This copy constructor is "special".  Since we don't read the localize bit from
// the text file but get it from the StringData array that is constructed from
// the string.info file, we cannot faithfully copy the _englishOnly flag since it is
// likely wrong.
//
// The englishOnly flag is set at time of StringData class construction and is
// fixed from then on.
//
TrimaTextItem& TrimaTextItem::operator = (const TrimaTextItem& textItem)
{
   if (this != &textItem) // Protect against self assignment
   {
      _id            = textItem._id;
      _string        = textItem._string;
      _template      = textItem._template;
      _languageId    = textItem._languageId;
      _stylingRecord = textItem._stylingRecord;
      _hasVariables  = textItem._hasVariables;
      _wasLoaded     = false; // Don't be fooled.  This needs to be this way for loading one language over another
      // _englishOnly = textItem._englishOnly;
   }

   return *this;
}



TrimaTextItem::~TrimaTextItem()
{}



void TrimaTextItem::setText (const char* string)
{
   if (string)
   {
      if (!_id.empty())
      {
         _template     = string;
         _string       = _template;
         _hasVariables = ( _string.find(startTok) >= 0 );
      }
   }
}



void TrimaTextItem::setText (const TrimaStringChar* string)
{
   if (!_id.empty())
   {
      _template     = string;
      _string       = string;
      _hasVariables = ( _string.find(startTok) >= 0 );
   }
}



const TrimaUnicodeString& TrimaTextItem::getTextObj ()
{
   handleVariableSubstitution();
   return _string;
}



void TrimaTextItem::getAscii (string& myString)
{
   handleVariableSubstitution();
   myString = _string.getUTF8();
}



TrimaTextItem* TrimaTextItem::getTextItem (const char* id)
{
   return TrimaStringData::findString(id);
}



void TrimaTextItem::setId (const char* id)
{
   _id.erase();

   if (id)
   {
      _id = id;
   }
}



const char* TrimaTextItem::getId (void) const
{
   return _id.c_str();
}



int TrimaTextItem::getLength (void) const
{
   return _string.getLength();
}



bool TrimaTextItem::isInitialized (void) const
{
   return !_id.empty();
}



int TrimaTextItem::getStringCharVariableLength (void)
{
   handleVariableSubstitution();
   return _string.getLength();
}



void TrimaTextItem::handleVariableSubstitution (void)
{
   if (_hasVariables)
   {
      _string = _template;

      int startIndex = _string.find(startTok);

      if (startIndex >= 0)
      {
         TrimaUnicodeString variableName;
         TrimaUnicodeString variableValue;
         string             name;

         while (startIndex >= 0)
         {
            const int endIndex = _string.find(endTok, startIndex);

            if (endIndex >= 0)
            {
               // Cut out the variable name substring, including the markup characters
               variableName = _string.mid(startIndex, endIndex - startIndex + 1);

               // Cut out the variable name without the markup characters
               name = variableName.mid(3, endIndex - startIndex - 3).getUTF8();

               // Look up the variable in the variable dictionary
               variableValue = TrimaVariableDatabase::variableLookUp(name);

               // If we got something from the variable dictionary, use that.
               if (variableValue.getLength() > 0)
               {
                  _string.replace(variableName, variableValue);
               }
               else if (name.empty())
               {
                  DataLog (log_level_gui_error) << "TrimaTextItem(" << _id << ") 0x" << hex << _string.getString() << dec
                                                << ": no variable found in " << _string.getUTF8() << endmsg;
               }

               // If the variable dictionary returned nothing, use the old Trima features.bin method.
               // Note that in order to use Jon's new sw cds, we had to stuff this into an int
               // instead of a string.  That's fine for the two places it's used right now, but
               // if we ever need something else from it (i.e. a string or a float), we'll have
               // to do something else, like the CGUI variable dictionary or something.
               else
               {
                  /*************************************************************************
                  ****                TODO: Jon- go ahead and replace             **********
                  ****                this stuff with your CDS function           **********
                  ****                to get an int, and put that int             **********
                  ****                into the settingVal variable                **********
                  *************************************************************************/
                  const char* settingString = Software_CDS::GetInstance().GetGUISetting(name.c_str());
                  int         settingVal    = (settingString) ? atoi(settingString) : 0;

                  /*************************************************************************
                  ****                The rest of this stuff can remain           **********
                  ****                the same.                                   **********
                  *************************************************************************/
                  DataLog (log_level_gui_info) << "TrimaTextItem(" << _id << ") 0x" << hex << _string.getString() << dec
                                               << ": resorting to features.bin to replace " << name << "=" << settingVal
                                               << " in " << _string.getUTF8() << endmsg;

                  char settingBuffer[16];
                  sprintf(settingBuffer, "%d", settingVal);
                  _string.replace(variableName, settingBuffer);
               }
            }
            else // no end token found
            {
               DataLog (log_level_gui_error) << "TrimaTextItem(" << _id << ") 0x" << hex << _string.getString() << dec
                                             << ": no END token found in " << _string.getUTF8() << endmsg;
            }

            startIndex = _string.find(startTok, startIndex + 1);
         }
      }
      else // no start token found
      {
         DataLog (log_level_gui_error) << "TrimaTextItem(" << _id << ") 0x" << hex << _template.getString() << dec
                                       << ": no START token found in '" << _template.getUTF8() << endmsg;
      }
   }
}

#ifndef __ALARM_TABLE_BUILD__ // Don't include this if this is an alarm table generator tool build
DataLog_Stream& operator << (DataLog_Stream& os, TrimaTextItem& txtItem)
{
   string        asciiString;
   txtItem.getAscii(asciiString);
   StylingRecord style = txtItem.getStylingRecord();

   return os << "[id=" << txtItem.getId() << "; lang=" << txtItem.getLanguageId() << "; fontIndex="
             << style.fontIndex << "; fontSize=" << style.fontSize << "; region=" << style.region << "; str="
             << '"' << asciiString << "\"]";
}
#endif // #ifndef __ALARM_TABLE_BUILD__

/* FORMAT HASH af0f588930d22e947366ee366004973b */
