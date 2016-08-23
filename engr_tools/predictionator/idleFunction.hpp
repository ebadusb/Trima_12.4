/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      idleFunction.hpp
 *             wrapper for idle loop functions.
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
 *     This file encapsulates a function that will be called during "idle"
 *     processing time for the main application.  To define a function that
 *     you want called in the idle loop, do the following:
 *      1) define a void function with no parameters in your class
 *      2) instantiate an IdleFunction object:
 *             IdleFunction idle(this,(IdleFunction::idle_loop_function)&class::function);
 *     The function class::function will then be called when the main application
 *     is idle.  The function call is automatically removed when the IdleFunction
 *     object is destructed.
 *
 *     Restrictions:  Note that due to the way this class is designed, your
 *     class must be derived from CObject (most MFC classes are already).  Also,
 *     virtual behaviors are not available since the constructor actually uses
 *     the class::function address sent.
 *
 *
 */
#ifndef IDLEFUNCTION_HPP
#define IDLEFUNCTION_HPP

#include <afx.h>

class IdleFunction : public CObject
{

   DECLARE_DYNAMIC(IdleFunction)

   //
   //
   //   The following methods are available for general use.
   //
   public:

      typedef void (CObject::*idle_loop_function)();

      //
      //
      //   You can ask me to call the next function in sequence.  I return
      //   TRUE if I performed any actions or FALSE if no requests exist.
      //
      static const BOOL do_idle_function();

      //
      //
      // Constructor (default constructor protected to force usage of this one)
      // I do not own either the object or function (duh).
      //
      IdleFunction(CObject *parent, void (CObject::*func)());

      // Destructor
      virtual ~IdleFunction();

      //
      //
      //   Accessors
      //
      idle_loop_function function() const;
      CObject *parent() const;

      //
      //
      //   Copy constructor, op= are valid for this class
      //
      IdleFunction(const IdleFunction &orig);
      IdleFunction &operator=(const IdleFunction &orig);

   //
   //
   //   These methods are for internal use only.
   //
   protected:

      IdleFunction(); // protected to force use of variant
     
      //
      //   methods to invoke internally for construction / destruction
      //   (note non-virtual)
      void cleanup();
      void copyover(const IdleFunction &orig);

//
//
//-----------------------------------------------------------------------------
//                      Member data
//
   protected:
      //
      //
      //   My function pointers
      void (CObject::*_function)();
      CObject *_parent;

      //
      //
      //        Static member data
      //
      static CObList  __idle_list;
      static POSITION __idle_position;
};

#endif
