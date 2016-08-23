/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      sectionfile.hpp
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
#ifndef SECTIONFILE_HPP
#define SECTIONFILE_HPP

#include <afx.h>
#include <afxcoll.h>

class Sectionfile_element;

class Sectionfile : public CObject
{

   DECLARE_DYNAMIC(Sectionfile)
   //
   //
   //   The following methods are available for general use.
   //
   public:

      // Constructor
      Sectionfile();

      // Destructor
      virtual ~Sectionfile();

      //
      //
      //  Once I have been constucted I must be initialized.  I return
      //  zero on success.
      //
      virtual const int initialize(const char *pathname);

      //
      //
      //  Get/set my pathname.
      //
      virtual const char *pathname() const;
      virtual void pathname(const char *path);

      //
      //
      //  You can ask me to write my data to a file.  If you do not specify
      //  a pathname I use the pathname I currently have.  If you rewrite me
      //  using a filename then the filename given replaces any previous path.
      //  I return zero on success.
      //
      virtual const int write() const;
      virtual const int write(const char *path);

      //
      //
      //  You can get or set any piece of data in my file.  (Changes are only
      //  internal until you ask me to write).  Generally the variable name
      //  is unique so you do not need to specify the section name.  If I
      //  cannot find the variable name or section while getting data then I
      //  return zero for the value and a nonzero result.  I return a zero
      //  status if everything was successful.  If the variable name specified
      //  is not unique and the section name is not specified then I set or
      //  return the value for the first matching variable found.
      //
      virtual const int get(const char *variable,
                            float      &value,
                            const char *section=0) const;
      virtual const int set(const char *variable,
                            const float value,
                            const char *section=0);

      //
      //
      //  You can get the internal elements that I have created.  You can
      //  either get an element for each line in the file or have the std::list
      //  filtered so only valid variables are returned.  Each element in
      //  the std::list is of type Sectionfile_element.  The caller takes ownership
      //  of the returned std::list and is responsible for its deletion (including
      //  deletion of the elements within the std::list).
      //
      virtual CObList *elements() const;
      virtual CObList *variables() const;

   //
   //
   //   These methods are for internal use only.
   //
   protected:
      virtual Sectionfile_element *find_element(const char *variable, 
                                                const char *section=0) const;

      //
      //
      //   Copy constructor, op= are not valid for this class
      //
      Sectionfile(const Sectionfile &orig);
      Sectionfile &operator=(const Sectionfile &orig);
//
//
//-----------------------------------------------------------------------------
//                      Member data
//
   protected:
      char *_pathname;          // my pathname
      CObList _elements;        // internal std::list of data file elements

};

#endif
