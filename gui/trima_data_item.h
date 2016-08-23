/*
 *	Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 *
 *
 * Revision 1.1  2008/02/01 21:56:43Z  dslausb
 * Initial revision
 * Revision 1.6  2007/11/15 21:02:37Z  rm10919
 * Create new data item class for clock type numeric strings.
 * Revision 1.5  2007/06/04 22:04:20Z  wms10235
 * IT83 - Updates for the common GUI project to use the unicode string class
 * Revision 1.4  2007/04/05 17:37:17Z  pn02526
 * Make TrimaStringChar * arg a const TrimaStringChar *
 * Revision 1.3  2007/03/28 12:03:12  pn02526
 * Add ability to specify a decimal separator.
 * Revision 1.2  2007/03/01 12:09:55  rm10919
 * Add DataItemText to file reader and setText(char *) to DITextItem.
 * Revision 1.1  2005/04/27 13:40:44Z  rm10919
 * Initial revision
 *
 */

#ifndef _DATA_ITEM_INCLUDE
#define _DATA_ITEM_INCLUDE

#include "osgraphics.h"
#include "trima_text_item.h"

class TrimaDataItem
{
public:
   TrimaDataItem(void);
   TrimaDataItem(bool valueChanged);
   virtual ~TrimaDataItem();

   virtual const TrimaStringChar* convertToString (void) = 0;

protected:
   TrimaUnicodeString _string;
   bool               _valueChanged;

private:
   TrimaDataItem(const TrimaDataItem& object);
   TrimaDataItem& operator = (const TrimaDataItem& object);
};

class TrimaDataItemInteger
   : public TrimaDataItem
{
public:
   TrimaDataItemInteger(void);
   TrimaDataItemInteger(int value);
   virtual ~TrimaDataItemInteger();

   virtual const TrimaStringChar* convertToString (void);

   void setValue (int value);

   friend DataLog_Stream& operator << (DataLog_Stream& os, const TrimaDataItemInteger& dataItem);

protected:
   int _value;

private:
   TrimaDataItemInteger(const TrimaDataItemInteger& object);
   TrimaDataItemInteger& operator = (const TrimaDataItemInteger& object);
};

class TrimaDataItemDouble
   : public TrimaDataItem
{
public:
   TrimaDataItemDouble(void);
   TrimaDataItemDouble(double value, int precision);
   TrimaDataItemDouble(double value, int precision, TrimaTextItem* separator);
   virtual ~TrimaDataItemDouble();

   virtual const TrimaStringChar* convertToString (void);

   void setValue (double value);
   void setPrecision (int precision);
   void setDecimalSeparator (TrimaTextItem* separator) { _separator = separator; _valueChanged = true; }

protected:
   double         _value;
   int            _precision;
   TrimaTextItem* _separator;

private:
   TrimaDataItemDouble(const TrimaDataItemDouble& object);
   TrimaDataItemDouble& operator = (const TrimaDataItemDouble& object);
};

class TrimaDataItemTextItem
   : public TrimaDataItem
{
public:
   TrimaDataItemTextItem(TrimaTextItem* value);
   virtual ~TrimaDataItemTextItem();

   virtual const TrimaStringChar* convertToString (void);

   void setValue (TrimaTextItem* value);
   void setValue (const TrimaStringChar* value);
   void setValue (char* value);

protected:
   TrimaTextItem* _value;

private:
   TrimaDataItemTextItem(void);
   TrimaDataItemTextItem(const TrimaDataItemTextItem& object);
   TrimaDataItemTextItem& operator = (const TrimaDataItemTextItem& object);
};

class TrimaDataItemText
   : public TrimaDataItem
{
public:
   TrimaDataItemText(void);
   TrimaDataItemText(const char* value);
   virtual ~TrimaDataItemText();

   virtual const TrimaStringChar* convertToString (void);

   void setValue (const char* value);

   friend DataLog_Stream& operator << (DataLog_Stream& os, const TrimaDataItemText& dataItem);

private:
   TrimaDataItemText(const TrimaDataItemText& object);
   TrimaDataItemText& operator = (const TrimaDataItemText& object);
};

// Used to insert a leading 0 for clock time minutes and seconds.
class TrimaDataItemClock
   : public TrimaDataItem
{
public:
   TrimaDataItemClock(void);
   TrimaDataItemClock(int value);
   virtual ~TrimaDataItemClock();

   virtual const TrimaStringChar* convertToString (void);

   void setValue (int value);

protected:
   int _value;

private:
   TrimaDataItemClock(const TrimaDataItemClock& object);
   TrimaDataItemClock& operator = (const TrimaDataItemClock& object);
};

#endif /* ifndef _DATA_ITEM_INCLUDE */

/* FORMAT HASH 2edd8a604af7f362b70a1d62af967952 */
