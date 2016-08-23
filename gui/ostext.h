/*
 *	Copyright (c) 2004 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * This file contains the class used to wrap the underlying text of a
 * window.  The intent is to limit dependencies on the graphics
 * system to this header file and to the implementation files associated
 * with it. All interaction with the window text from the application
 * should be through the publically accessible functions and data defined
 * in this file.
 *
 */

#ifndef __OSTEXT_H__
#define __OSTEXT_H__

#include <vxworks.h>
#include <map>
#include <list>
#include <string>
#include <stdio.h>

#include <ugl/ugl.h>
#include <ugl/uglwin.h>
#include <ugl/uglinput.h>

#include "ostext_base.h"
#include "osgraphics.h"
#include "trima_text_item.h"
#include "trima_data_item.h"
#include "fontdefs.h"
#include "string_id.h"

class OSText
   : public OSTextBase, public OSWindowObject
{
public:
   //
   //  These are the attributes a text string can have.
   //  They specify how the text is justified or aligned
   //  and what style type it has in it's OSRegion.
   //
   enum { VJUSTIFY_TOP  = 0x0000, VJUSTIFY_CENTER = 0x0001, VJUSTIFY_BOTTOM = 0x0002 };    // vertical justification options
   enum { HJUSTIFY_LEFT = 0x0000, HJUSTIFY_CENTER = 0x0010, HJUSTIFY_RIGHT = 0x00020 };    // horizontal justification options

   enum { NTS           = 0x0000, BOLD = 0x0100, ITALIC = 0x0200, BOLD_ITALIC = 0x0300 }; // text style options, this may be controlled by the font or the font driver.
   // NTS - no text style, default attribute.

   //  The LEFTTORIGHT and RIGHTTOLEFT attributes are for
   //  specifing from which direction the text is read.
   //  This will only become an issue when a language is added
   //  that is read from right to left instead of left to right.
   //  The default IS LEFTTORIGHT.
   //
   enum { LEFT_TO_RIGHT = 0x0000, RIGHT_TO_LEFT = 0x1000 };      // text direction options

   // Constructors
   //
   OSText(OSDisplay& display);
   OSText(OSDisplay& display, TrimaTextItem* textItem, StylingRecord* stylingRecord = NULL);
   OSText(OSDisplay& display, TrimaTextItem* textItem, OSColor backgroundColor, StylingRecord* stylingRecord = NULL);

   // Destructor
   //
   virtual ~OSText();

   // SET_ATTRIBUTES
   // This methods set the attributes of the text.
   // First bit sets vertical justification. Default value is TOP.
   // Second bit sets the horizontal justification. Default value is LEFT.
   // Third bit sets the style (no text style (NTS), BOLD, ITALIC, BOLDITALIC.  Default value is NTS. Note: if BOLD and ITALIC are added together you still get BOLDITALIC.
   // Fourth bit is for text direction. Is it read LEFTTORIGHT or RIGHTTOLEFT? Default is LEFTTORIGHT.
   //
   void         setAttributes (unsigned int newAttributes);
   unsigned int getAttributes () const { return _stylingRecord.attributes; }

   // SET_BACKGROUND_COLOR
   // This method sets the background color for the text.
   //
   void setBackgroundColor (OSColor color);

   // SET_COLOR
   // These methods set the text color by sending on OSColor
   // or by sending the RGB numbers.
   //
   void          setColor (OSColor color);
   void          setColor (int red, int green, int blue);
   const OSColor getColor () const { return _stylingRecord.color; }

   // SET_CAPTURE_BACKGROUND_COLOR
   // This method determines if the background is determined by
   // by this class' draw method or if the background color
   // is set by the OSColor passed in the constructor.  This method
   // is primarily for determining the text background color for the
   // simulator.
   //
   void setCaptureBackgroundColor () { _captureBackgroundColor = true; _backgroundColorSet = false; }

   int getFontIndex (int currentChar);

   int getFontIndex () const { return _stylingRecord.fontIndex; }
   int getFontSize () const { return _stylingRecord.fontSize; }

   // SET_FONT_SIZE
   // This method sets the font size for the text.
   //
   void setFontIndex (int newIndex);
   void setFontSize (int newSize);
   void setFont (const int newFontIndex, const int newFontSize);

   // SET_LANGUAGE
   // This methods sets the language the text will use.
   // These is an interdependency between the font and the
   // language.
   //
   void       setLanguage (LanguageId configLanguage);
   LanguageId getLanguage () const { return _configLanguage; }

   // SET_REGION
   // This method sets the region by x, y, Width, and height
   // for the text.
   //
   void     setRegion (const OSRegion& region);
   OSRegion getRegion () const { return _stylingRecord.region; }

   void get_requested_region (OSRegion& requested_region) const;
   void set_requested_region (const OSRegion& set_text_requested_region);

   //
   // SET_STYLING_RECORD
   // This method sets the styling record of the
   // text.  Use this method if several members
   // of the styling record need to be set instead
   // calling each individual method.
   //
   void           setStylingRecord (StylingRecord* stylingRecord);
   StylingRecord* getStylingRecord () { return &_stylingRecord; }

   // SET_TEXT
   // This methods sets the text string.  When using this method
   // only use strings from the text database.
   //
   void setText (TrimaTextItem* textItem);   // Warning.  Item must stick around.  Reference to item remains
   void setText (const char* text);
   void setText (const TrimaStringChar* text);
   void setText (const TrimaUnicodeString& text);
   void setText ();  // Sets _textString to the string in the _textItem memeber.

   const TrimaStringChar*    getText ();
   const TrimaUnicodeString& getTextObj ();

   int getLength () const;

   void handleVariableSubstitution ();

   TEXT_STRING_ID getStringId () { return _currentId; }

protected:
   enum GetTokenResult
   {
      EndOfString,          // reached end of string - no token returned
      NormalToken,          // normal token to add to output record
      FormatToken           // format token - controls output format but is not included in output record
      // format tokens are only allowed at the start of a line
   };

   //
   // These methods are used to create the text lines
   // as they will appear in the window.  Together they
   // work as the text wrapping method for the text.
   //
   void           getSize (OSRegion& region, int startIndex = -1, int length = -1, OSFontId fontId = UGL_NULL_ID /*= _stylingRecord.fontId */);
   void           getTokenSize (OSRegion& region, int startIndex, int length);
   void           getPixelSize (OSRegion& pixelRegion, OSFontId fontId); // The entire string on one line, no text wrapping.
   GetTokenResult getToken (int start_index, bool start_of_line, int& length);
   void           convertTextToMultiline (OSRegion& region);
   bool           convertLinePosition (int width, int height, int indent_pixels, OSRegion& region);
   void           computeTextRegion ();

   // SET_FONT_ID
   // This methods sets the font for the text.
   // The font used may dictate some styling
   // attributes, e.g., BOLD and ITALIC, of the text.
   // The font is also dependent on the language
   // to be displayed.
   //
   void     setFontId (OSFontId newFontId);
   OSFontId getFontId ();
   OSFontId getFontId (int currentChar);

   virtual void pre_draw ();

   virtual void draw (UGL_GC_ID gc);

   // This structure holds the individual text
   // for each line.  It is used to wrap the
   // text in the display area.
   //
   struct LineData
   {
      short          x, y;              // offset from top left corner of text region to top left corner of line
      unsigned short index;             // index into text string for start of line
      unsigned short textLength;        // number of text characters on line
      // the ofsset for RIGHTTOLEFT text string may have a negative offset.
      OSFontId       fontId;            // the font associated with text
   };

   struct FormatData
   {
      unsigned short firstLineIndent;       // number of tab-stops for indenting first line
      unsigned short secondLineIndent;      // number of tab-stops for indenting second and subsequent lines

      FormatData()
         : firstLineIndent(0), secondLineIndent(0) { }
   };

   OSText::GetTokenResult getCharBasedToken (int start_index, bool start_of_line, int& length);
   bool                   checkIfEnglish (int index);
   bool                   checkIfForbiddenStart (int index);
   bool                   checkIfForbiddenEnd (int index);
   bool                   checkIfArabicNumeral (int index);

private:
   // Text is never clipped by another window object and
   // always appears on top.
   //
   virtual ClippingType clip_siblings () const { return NotClipped; }

   // Initialize values for text string object.
   //
   void initializeData (TrimaTextItem* textItem, StylingRecord* stylingRecord);

   // Disallow value semantics and the default constructor
   //
   OSText();
   OSText (const OSText& copy);
   OSText& operator = (const OSText& obj);


//
// Member data section
//
protected:
   TEXT_STRING_ID _currentId; // This isn't really needed, but it's nice to have so we can log it and such.
                              // It's only used through the Display_Text class.

   TrimaTextItem*     _textItem;        // Text item for strings from the string database
   StylingRecord      _stylingRecord;   // holds the styling record for an object
   TrimaUnicodeString _textString;      // Text string for dynamic user defined text

   TrimaUnicodeString _lastTextString;     // Last text string evaluated by computeTextRegion()
   bool               _forceCompute;       // if true, computeTextRegion() is forced to reevaluate string

   list<LineData>     _lineData;           // list of text lines for object
   FormatData         _formatData;         // current paragraph format options

   //
   // These are used to determine the background color for the text.
   //
   bool    _captureBackgroundColor;
   bool    _backgroundColorSet;
   OSColor _backgroundColor;

   bool    _languageSetByApp; // flag for determining if _textString needs updating based on language

private:
   OSRegion   _requestedRegion;    // used to determine area for LineData
   LanguageId _configLanguage;     // language of text string, need to know where to look for string. Not sure if this is need _textItem.languageId may be used.

};

#endif /* #ifndef __OSTEXT_H__ */

/* FORMAT HASH 241c3c5bce85c201c50469d2a3cd385e */
