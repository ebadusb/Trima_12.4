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

#include <fstream>
#include <cassert>

#include "Sectionfile.hpp"
#include "Sectionfile_element.hpp"

IMPLEMENT_DYNAMIC(Sectionfile,CObject)

//-----------------------------------------------------------------------------
//                Constructor, destructor
//
Sectionfile::Sectionfile() :
   CObject(),
   _pathname(0),
   _elements()
{
}

Sectionfile::~Sectionfile()
{
   delete _pathname;
   _pathname = 0;

   //
   //
   //  Dump my list.  The CObList class does not include a method to destruct
   //  objects within the std::list.
   //
   Sectionfile_element *elem;
   while (!_elements.IsEmpty()) {
      elem = (Sectionfile_element *)_elements.RemoveHead();
      delete elem;
   }
}

//-----------------------------------------------------------------------------
//                Initialization
//
const int Sectionfile::initialize(const char *pathname)
{
   assert(pathname != 0);
   int status = 0;
   Sectionfile_element *elem;

   //
   //
   //  Start by restoring myself to a null state.
   //
   delete _pathname; // in case it already exists
   while (!_elements.IsEmpty()) {
      elem = (Sectionfile_element *)_elements.RemoveHead();
      delete elem;
   }

   //
   //
   //   Copy the pathname for future reference and attempt to open the file.
   //
   _pathname = strdup(pathname);
   std::ifstream datfile(_pathname,std::ios::in);
   if (!datfile.fail()) {
      static const int MAXBUFFERLENGTH = 255; // by TRIMA definition
      char buffer[MAXBUFFERLENGTH]; 
      char *last_section = 0;
	  char *this_section = 0;
      //
      //
      //   Go through the file and create a Sectionfile_element for each
      //   line in the file.  As we are reading through we keep track of
	  //   the last "section" since all lines after that are assumed to
	  //   belong to that section until a new section delimiter is found.
	  //   Note that comments do not have a valid section so we can only
	  //   update the last section when it is valid.
     //
      datfile.getline(buffer,MAXBUFFERLENGTH); // istream call
      while (!datfile.eof()) {
         elem = new Sectionfile_element();
         elem->initialize(last_section,buffer);
         _elements.AddTail(elem);
         this_section = (char *)elem->section(); // deconstify for compiler
         if (this_section) last_section = this_section;
         datfile.getline(buffer,MAXBUFFERLENGTH);
      }
      datfile.close();
   } else { // could not open file
      status = 1;
   }
   return status;
}


//-----------------------------------------------------------------------------
//                    Get/set my pathname.
//
const char *Sectionfile::pathname() const
{
   return _pathname;
}

void Sectionfile::pathname(const char *path)
{
   delete _pathname;
   _pathname = strdup(path);
}

//-----------------------------------------------------------------------------
//               Writing functionality
//
const int Sectionfile::write(const char *path)
{
   pathname(path);
   return write();
}

const int Sectionfile::write() const
{
   int status = 1;
   Sectionfile_element *elem;

   std::fstream datfile(_pathname,std::ios::out);
   if (!datfile.fail()) {
      datfile.flags(std::ios::fixed | std::ios::showpoint);
      datfile.precision(2);
      if (!_elements.IsEmpty()) {
         status = 0;
         POSITION pos;
         for (pos = _elements.GetHeadPosition(); pos != NULL;) {
            elem = (Sectionfile_element *)_elements.GetNext(pos);
            elem->put(datfile);
         }
      }
      datfile.close();
   }
   return status;
}

//-----------------------------------------------------------------------------
//                   Get / set data
//
const int Sectionfile::get(const char *variable,
                           float      &value,
                           const char *section) const
{
   int status = 0;
   Sectionfile_element *elem = find_element(variable,section);
   if (elem) {
      value = elem->get();
   } else {
      ASSERT(0);
      value = 0.0;
      status = 1;
   }
   return status;
}

const int Sectionfile::set(const char *variable,
                           const float value,
                           const char *section)
{
   int status = 0;
   Sectionfile_element *elem = find_element(variable,section);
   if (elem) {
      elem->set(value);
   } else {
      ASSERT(0);
      status = 1;
   }
   return status;
}

//-----------------------------------------------------------------------------
//                   Return a copy of my internal list
//
CObList *Sectionfile::elements() const
{
   CObList *new_list = new CObList;
   if (!_elements.IsEmpty()) {
     POSITION pos;
     Sectionfile_element *elem;
     for( pos = _elements.GetHeadPosition(); pos != NULL; ) {
        elem = (Sectionfile_element *)_elements.GetNext(pos);
        new_list->AddTail(new Sectionfile_element(*elem));
     }
   }
   return new_list;
}

CObList *Sectionfile::variables() const
{
   CObList *new_list = new CObList;
   if (!_elements.IsEmpty()) {
      POSITION pos;
      Sectionfile_element *elem;
      for (pos = _elements.GetHeadPosition(); pos != NULL;) {
         elem = (Sectionfile_element *)_elements.GetNext(pos);
         if (elem->variable()) {
            new_list->AddTail(new Sectionfile_element(*elem));
         }
      }
   }
   return new_list;
}

//-----------------------------------------------------------------------------
//                   Find an element given the variable and section name
//
Sectionfile_element *Sectionfile::find_element(const char *variable, 
                                               const char *section) const
{
   Sectionfile_element *result = 0;
   if (!_elements.IsEmpty()) {
      POSITION pos;
      Sectionfile_element *elem;
      for (pos = _elements.GetHeadPosition(); pos != NULL;) {
         elem = (Sectionfile_element *)_elements.GetNext(pos);
         if (elem->match(variable,section)) {
            result = elem;
            break;
         }
      }
   }
   return result;
}

//-----------------------------------------------------------------------------
//          Copy constructor, op= are not valid for this class
//       
Sectionfile::Sectionfile(const Sectionfile &orig)
   // : Cobject(orig) // private in base class; use default
{
}

Sectionfile &Sectionfile::operator=(const Sectionfile &orig)
{
   // Cobject::operator=(&orig); // private in base class; use default
   if (this != &orig) {
   }
   return *this;
}
