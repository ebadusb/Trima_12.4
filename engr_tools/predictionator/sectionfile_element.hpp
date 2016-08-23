/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      sectionfile_element.hpp
 *             Focussed system prediction functions - wrapper for
 *             data file reader abstractions.
 *
 * AUTHOR:     R.E. Edwards
 *
 * ABSTRACT:
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 * EXAMPLE:
 *
 *
 */
#ifndef SECTIONFILE_ELEMENT_HPP
#define SECTIONFILE_ELEMENT_HPP

#include <afx.h>
#include <fstream>

class Sectionfile_element : public CObject
{

   DECLARE_DYNAMIC(Sectionfile_element)

   //
   //
   //   The following methods are available for general use.
   //
   public:

      // Constructor
      Sectionfile_element();

      // Destructor
      virtual ~Sectionfile_element();

      //
      //
      //  Once I have been constucted I must be initialized.  I return
      //  zero on success.  The line sent is assumed to be \0 terminated.
      //
      virtual const int initialize(const char *section, const char *line);

      //
      //
      //   You can ask me to output myself to a file stream
      //
      void put(std::fstream &str) const;

      //
      //
      //   You can ask me if I match a given variable and section name.
      //   I return nonzero if I match and zero if I do not match.
      //
      virtual const int match(const char *variable, const char *section=0) const;

      //
      //
      //  You can get or set my data value.
      //
      virtual const float get();
      virtual void set(const float value);

      //
      //
      //   Accessors for my section and variable
      //
      virtual const char *variable() const;
      virtual const char *section() const;

      //
      //
      //   Copy constructor, op= are valid for this class
      //
      Sectionfile_element(const Sectionfile_element &orig);
      Sectionfile_element &operator=(const Sectionfile_element &orig);

   //
   //
   //   These methods are for internal use only.
   //
   protected:

      //
      //   methods to invoke internally for construction / destruction
      //   (note non-virtual)
      void cleanup();
      void copyover(const Sectionfile_element &orig);

//
//
//-----------------------------------------------------------------------------
//                      Member data
//
   protected:
      char *_variable;
      char *_section;
      float _value;

};

#endif
