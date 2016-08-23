/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      IdleFunction.cpp
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
#include "IdleFunction.hpp"

IMPLEMENT_DYNAMIC(IdleFunction,CObject)

//
//-----------------------------------------------------------------------------
//                 Constructors, destructor
//
IdleFunction::IdleFunction() :
   CObject(),
   _parent(0),
   _function(0)
{
}

   IdleFunction::IdleFunction(CObject *parent, void (CObject::*func)()) :
   CObject(),
   _parent(parent),
   _function(func)
{
      __idle_position = NULL;
      __idle_list.AddTail(this);
}

IdleFunction::~IdleFunction()
{
   POSITION pos = __idle_list.Find(this);
   __idle_list.RemoveAt(pos);
   __idle_position = NULL;
   cleanup();
}

//-----------------------------------------------------------------------------
// copy constructor and op= 
//
IdleFunction::IdleFunction(const IdleFunction &orig)
   // : CObject(orig) // private in base class; use default constructor instead
{
   copyover(orig);
}

IdleFunction &IdleFunction::operator=(const IdleFunction &orig)
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

void IdleFunction::cleanup()
{
   _parent = 0;
   _function = 0;
}

void IdleFunction::copyover(const IdleFunction &orig)
{
   _parent = orig._parent;
   _function = orig._function;
}
//
//
//-----------------------------------------------------------------------------
//    Accessor functions
//

IdleFunction::idle_loop_function IdleFunction::function() const
{
   return _function;
}
CObject *IdleFunction::parent() const
{
   return _parent;
}
//
//
//-----------------------------------------------------------------------------
//    List traversal
//
const BOOL IdleFunction::do_idle_function()
{
   IdleFunction *idle = 0;
   BOOL status = FALSE;

   if (!__idle_list.IsEmpty()) {
      if (__idle_position == NULL) {
         __idle_position = __idle_list.GetHeadPosition();
         idle = (IdleFunction *)__idle_list.GetAt(__idle_position);
      } else {
         idle = (IdleFunction *)__idle_list.GetNext(__idle_position);
      }
   } else {
      __idle_position = NULL;
   }

   //
   //
   //   Call the next idle function in the list
   //
   if (idle) {
      CObject *parent = idle->parent();
      idle_loop_function func = idle->function();
      (parent->*func)();
      status = TRUE;
   }
   return status;
}
//
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//    Initialization of static members
//
CObList IdleFunction::__idle_list;
POSITION IdleFunction::__idle_position = NULL;