/*
 *	Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#ifndef _VARIABLE_DATABASE_INCLUDE
#define _VARIABLE_DATABASE_INCLUDE

#include "trima_data_item.h"


// Member for storing variable values that appear in strings.
// The actual variable and value is controled by the project.
class TrimaVariableDatabase
{
public:
   TrimaVariableDatabase();
   virtual ~TrimaVariableDatabase(void);

   static const TrimaStringChar* variableLookUp (const string& name);

   static TrimaDataItem* getDataItem (const char* name);

   static bool read ();
   static void dumpDatabase ();

protected:
   static void addDataItem (const char* name, TrimaDataItem* dataItem);
   static void deleteDataItem (const char* name);

private:
   typedef map<string, TrimaDataItem*> VARIABLE_DICTIONARY_MAP;
   typedef VARIABLE_DICTIONARY_MAP::iterator VARIABLE_DICTIONARY_MAP_ITER;

   static VARIABLE_DICTIONARY_MAP _variableDictionary;
};

#endif /* ifndef _VARIABLE_DATABASE_INCLUDE */

/* FORMAT HASH 91bfb84988698febf6fae2e32f5ecaf9 */
