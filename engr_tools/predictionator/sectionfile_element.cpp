/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      sectionfile_element.cpp
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
#include "Sectionfile_element.hpp"

IMPLEMENT_DYNAMIC(Sectionfile_element,CObject)

//
//
//   This class uses its internal data as follows:
//
//   _section = 0 implies that this element represents a comment
//   _variable = 0 implies that this element represents a section header
//   If both _section and _variable are zero this is a comment representing
//       a blank line.
//
//-----------------------------------------------------------------------------
//                 Constructor, destructor
//
Sectionfile_element::Sectionfile_element() :
   CObject(),
   _variable(0),
   _section(0),
   _value(0.0)
{
}

Sectionfile_element::~Sectionfile_element()
{
   cleanup();
}

//-----------------------------------------------------------------------------
//                    Initialization
//
//
const int Sectionfile_element::initialize(const char *section, const char *line)
{
   int status = 0;
   if (line) {
      char *workline = strdup(line); // since strtok may modify its arguments
      if (line[0] == '[') {
         _section = strdup(strtok(workline, "[]"));
         _variable = 0;
      } else if ((line[0] == '#') || (line[0] == ' ') || (line[0] == '\0')) { // this is a comment
         _section = 0;
         _variable = strdup(line);
      } else { // this is a variable
         _section = strdup(section);
         _variable = strdup(strtok(workline,"="));
         _value = (float)atof(strtok(0,"="));
      }
      delete workline;
   }
   return status;
}
//-----------------------------------------------------------------------------
//
//
void Sectionfile_element::put(std::fstream &str) const
{
   if (_section && _variable) { // this is a variable line
      str << _variable << "=" << _value;
   } else if (_section) { // this is a section line
      str << "[" << _section << "]";
   } else if (_variable) { // this is a comment
      str << _variable;
   } // else a blank line
   str << std::endl;
}
//-----------------------------------------------------------------------------
//
//
//   You can ask me if I match a given variable and section name. I return
//   nonzero if I match and zero if I do not match.
//
const int Sectionfile_element::match(const char *variable, const char *section) const
{
   int matched = 0;
   if (_variable && _section) {
      if ((!section) || (!strcmp(_section,section))) {
         matched = (!strcmp(_variable,variable));
      }
   }
   return matched;
}

//-----------------------------------------------------------------------------
//            get / set my data value
//
const float Sectionfile_element::get()
{
   return _value;
}
void Sectionfile_element::set(const float value)
{
   _value = value;
}
//-----------------------------------------------------------------------------
//                Accessors for my section and variable
//
const char *Sectionfile_element::variable() const
{
	return _variable;
}

const char *Sectionfile_element::section() const
{
	return _section;
}

//-----------------------------------------------------------------------------
// copy constructor and op= 
//
Sectionfile_element::Sectionfile_element(const Sectionfile_element &orig)
   // : CObject(orig) // private in base class; use default constructor instead
{
   copyover(orig);
}

Sectionfile_element &Sectionfile_element::operator=(const Sectionfile_element &orig)
{
   // CObject::operator=(orig);  private in base class; use default constructor
   if (this != &orig) {
      cleanup();
      copyover(orig);
   }
   return *this;
}
//
//
//-----------------------------------------------------------------------------
//    cleanup and copyover methods
//

void Sectionfile_element::cleanup()
{
   delete _variable;
   _variable = 0;
   delete _section;
   _section = 0;
   _value = 0;
}

void Sectionfile_element::copyover(const Sectionfile_element &orig)
{
   if (orig._variable) _variable = strdup(orig._variable);
   if (orig._section) _section = strdup(orig._section);
   _value = orig._value;
}
