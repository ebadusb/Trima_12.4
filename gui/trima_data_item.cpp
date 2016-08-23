/*
 *	Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/trima_data_item.cpp 1.1 2008/02/01 21:56:49Z dslausb Exp $
 * $Log: trima_data_item.cpp $
 * Revision 1.1  2008/02/01 21:56:49Z  dslausb
 * Initial revision
 * Initial revision
 *
 */

#include <vxworks.h>
#include "trima_data_item.h"
#include <iostream>
#include <sstream>

//
// Void Constructor
//
TrimaDataItem::TrimaDataItem(void)
   : _valueChanged(false)
{}

TrimaDataItem::TrimaDataItem(bool valueChanged)
   : _valueChanged(valueChanged)
{}


TrimaDataItem::~TrimaDataItem()
{}

//
// Void Constructor
//
TrimaDataItemInteger::TrimaDataItemInteger(void)
   : _value(0)
{}

TrimaDataItemInteger::TrimaDataItemInteger(int value)
   : TrimaDataItem(true),
     _value(value)
{}

TrimaDataItemInteger::~TrimaDataItemInteger()
{}

const TrimaStringChar* TrimaDataItemInteger::convertToString (void)
{
   if ( _valueChanged )
   {
      const char*   intString = NULL;
      ostringstream textStream;

      textStream.setf(ios::fixed);
      textStream.precision(0);
      textStream << _value;

      intString = textStream.str().c_str();

      //
      // Copy value (string) into _string
      //
      if ( intString )
         _string = intString;
      else
         _string.empty();

      _valueChanged = false;
   }

   return _string.getString();
}

void TrimaDataItemInteger::setValue (int value)
{
   if (_value != value)
   {
      _value        = value;
      _valueChanged = true;
   }
}

//
// Void Constructor
//
TrimaDataItemDouble::TrimaDataItemDouble(void)
   : _separator(NULL),
     _value(0.0),
     _precision(0)
{}

TrimaDataItemDouble::TrimaDataItemDouble(double value, int precision)
   : TrimaDataItem(true),
     _value(value),
     _precision(precision),
     _separator(NULL)
{}

TrimaDataItemDouble::TrimaDataItemDouble(double value, int precision, TrimaTextItem* separator)
   : TrimaDataItem(true),
     _value(value),
     _precision(precision),
     _separator(separator)
{}

TrimaDataItemDouble::~TrimaDataItemDouble()
{}

const TrimaStringChar* TrimaDataItemDouble::convertToString (void)
{
   if ( _valueChanged )
   {
      ostringstream textStream;

      textStream.setf(ios::fixed);
      if (_precision < 0)
         textStream.precision(0);
      else
         textStream.precision(_precision);

      textStream << _value;

      _string = textStream.str().c_str();

      if ( _separator != NULL )
      {
         // replace 1st period with the given separator.
         int seperatorPos = _string.find( (TrimaStringChar)'.');

         if ( seperatorPos >= 0 )
         {
            _string.deleteChar(seperatorPos, 1);
            _string.insert(_separator->getTextObj(), seperatorPos);
         }
      }

      _valueChanged = false;
   }

   return _string.getString();
}

void TrimaDataItemDouble::setValue (double value)
{
   if (_value != value)
   {
      _value        = value;
      _valueChanged = true;
   }
}

void TrimaDataItemDouble::setPrecision (int precision)
{
   _precision    = precision;
   _valueChanged = true;
}

//
// Void Constructor
//
TrimaDataItemTextItem::TrimaDataItemTextItem(void)
   : _value(NULL)
{}

TrimaDataItemTextItem::TrimaDataItemTextItem(TrimaTextItem* value)
   : TrimaDataItem(true),
     _value(value)
{}

TrimaDataItemTextItem::~TrimaDataItemTextItem()
{}

const TrimaStringChar* TrimaDataItemTextItem::convertToString ()
{
   if (_valueChanged && _value != NULL)
   {
      _string       = _value->getTextObj();
      _valueChanged = false;
   }

   return _string.getString();
}

void TrimaDataItemTextItem::setValue (TrimaTextItem* value)
{
   if (_value != value)
   {
      _value        = value;

      _valueChanged = true;
   }
}

void TrimaDataItemTextItem::setValue (const TrimaStringChar* value)
{
   if (value != NULL && _value != NULL)
   {
      _value->setText(value);

      _valueChanged = true;
   }
}

void TrimaDataItemTextItem::setValue (char* value)
{
   if (value != NULL && _value != NULL)
   {
      _value->setText(value);

      _valueChanged = true;
   }
}

//
// Void Constructor
//
TrimaDataItemText::TrimaDataItemText(void)
{}

TrimaDataItemText::TrimaDataItemText(const char* value)
   : TrimaDataItem(true)
{
   _string = value;
}

TrimaDataItemText::~TrimaDataItemText()
{}

const TrimaStringChar* TrimaDataItemText::convertToString ()
{
   return _string.getString();
}

void TrimaDataItemText::setValue (const char* value)
{
   TrimaUnicodeString uValue = value;

   if ( _string != uValue )
   {
      _string       = value;
      _valueChanged = true;
   }
}

//
// Void Constructor
//
TrimaDataItemClock::TrimaDataItemClock(void)
   : _value(0)
{}

TrimaDataItemClock::TrimaDataItemClock(int value)
   : TrimaDataItem(true),
     _value(value)
{}

TrimaDataItemClock::~TrimaDataItemClock()
{}

const TrimaStringChar* TrimaDataItemClock::convertToString (void)
{
   if ( _valueChanged )
   {
      const char*   intString = NULL;
      ostringstream textStream;

      textStream.setf(ios::fixed);
      textStream.precision(0);

      if ( _value < 10 )
         textStream << "0" << _value;
      else
         textStream << _value;

      intString = textStream.str().c_str();

      //
      // Copy value (string) into _string
      //
      if ( intString )
         _string = intString;
      else
         _string.empty();

      _valueChanged = false;
   }

   return _string.getString();
}

void TrimaDataItemClock::setValue (int value)
{
   if (_value != value)
   {
      _value        = value;
      _valueChanged = true;
   }
}

DataLog_Stream& operator << (DataLog_Stream& os, const TrimaDataItemInteger& dataItem)
{
   return os << dataItem._value;
}

DataLog_Stream& operator << (DataLog_Stream& os, const TrimaDataItemText& dataItem)
{
   string utfString = dataItem._string.getUTF8();
   return os << utfString;
}

/* FORMAT HASH b29a01e9642de2aa353a32be9178b2e5 */
