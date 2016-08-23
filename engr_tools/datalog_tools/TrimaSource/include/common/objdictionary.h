/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      objdictionary.h
 *
 * ABSTRACT:   
 *
 */

#ifndef _OBJDICTIONARY_H
#define _OBJDICTIONARY_H

#include <list>
#include <string>
#include "datalog.h"


typedef void *(*NewObjFcnPtr)();

class ObjDictionaryEntry
{
public:

   ObjDictionaryEntry( const std::string &n, NewObjFcnPtr fcnPtr );
   ObjDictionaryEntry( const char* n, NewObjFcnPtr fcnPtr );

   ObjDictionaryEntry( const ObjDictionaryEntry &d );

   virtual ~ObjDictionaryEntry();

   ObjDictionaryEntry &operator=( const ObjDictionaryEntry &d );

   virtual void *newObject();
   
   const std::string &className() const { return _ClassName; }
   void className( const std::string &n ) { _ClassName = n; }
   void className( const char* n ) { _ClassName = n; }

   int operator==( const ObjDictionaryEntry &d );

   int operator==( const std::string &n );
   int operator==( const char *n );

   int operator<( const ObjDictionaryEntry &d );

   int operator<( const std::string &n );
   int operator<( const char *n );

   int compare( const std::string &n );
   int compare( const char *n );
   
protected:

   //
   // Delete the memory 
   //
   void cleanup();

protected:

   std::string _ClassName;

   NewObjFcnPtr _NewObjFcnPtr;
   
};


class ObjDictionary
{
public:

   static void add( ObjDictionaryEntry* d );

   static void *create( const char *n );

   static int valid( const char *n );

   static void dump( DataLog_Stream &outs );

protected:

   static ObjDictionaryEntry *findEntry( const char *n );

private:

   static std::list< ObjDictionaryEntry* > _ObjEntries;

};

#define DECLARE_OBJ( __objclassname__ ) \
   \
public: \
   \
   static void *newObject() \
   {  \
      return new __objclassname__( ); \
   }; \
   \
// End of DECLARE_OBJ macro


#define DEFINE_OBJ( __objclassname__ ) \
   \
   static ObjDictionaryEntry entry##__objclassname__\
                    ( #__objclassname__, &__objclassname__::newObject ); \
   \
// End of DEFINE_OBJ macro

#endif /* _OBJDICTIONARY_H */
