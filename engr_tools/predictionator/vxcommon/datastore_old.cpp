/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      datastore.cpp
 *             Common Data Store Interface
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the Data Store primitives from which Common Data
 *             Stores are made.
 *
 * HISTORY:    $Log: datastore_old.cpp $
 * HISTORY:    Revision 1.1  2005/12/16 21:49:55Z  rm70006
 * HISTORY:    Initial revision
 * HISTORY:    Revision 1.21  2002/11/18 18:29:10Z  jl11312
 * HISTORY:    - modifications to improve compile speed, reduce inline function sizes
 * HISTORY:    Revision 1.20  2002/11/06 15:43:56  rm70006
 * HISTORY:    Remove unnecessary new's.
 * HISTORY:    remove inline functions to fix compiler bug.
 * HISTORY:    fix bug in register call.
 * HISTORY:    Revision 1.19  2002/10/31 19:22:31Z  rm70006
 * HISTORY:    Changed internal structure to use less symbols which improved speed of datastore creation.
 * HISTORY:    Revision 1.18  2002/10/25 20:46:36Z  td07711
 * HISTORY:    support spoofer caching mechanism
 * HISTORY:    Revision 1.17  2002/10/18 19:58:14  rm70006
 * HISTORY:    Added new type of CDS for proc.
 * HISTORY:    Revision 1.16  2002/09/25 16:05:00Z  rm70006
 * HISTORY:    Fixed bug in fatal logging.
 * HISTORY:    Revision 1.15  2002/09/24 16:48:39Z  rm70006
 * HISTORY:    Add extra debugging ability
 * HISTORY:    Revision 1.14  2002/09/19 21:46:07Z  jl11312
 * HISTORY:    - added initialization to avoid compiler warning for BUILD_TYPE=PRODUCTION
 * HISTORY:    Revision 1.13  2002/09/18 22:13:57  rm70006
 * HISTORY:    Changed get and set to have built in locking.
 * HISTORY:    Revision 1.12  2002/09/13 20:09:23Z  rm70006
 * HISTORY:    Fix bug with lock/unlock.
 * HISTORY:    Revision 1.10  2002/09/04 18:33:24Z  rm70006
 * HISTORY:    Added missing destructor for SingleWriteDataStore.
 * HISTORY:    Added code to SingleWriteDataStore to allow writer to be created and deleted as many times as needed.  Check now ensures only one at a time, not one ever.
 * HISTORY:    Revision 1.9  2002/09/03 14:36:19Z  rm70006
 * HISTORY:    Added new SingleWrite class.
 * HISTORY:    Revision 1.8  2002/08/23 14:55:08Z  rm70006
 * HISTORY:    Changed binditem to work with 486 compiler bug
 * HISTORY:    Revision 1.7  2002/07/17 14:03:22Z  sb07663
 * HISTORY:    latest latest fix for multiple writer check
 * HISTORY:    Revision 1.6  2002/07/16 22:22:13  rm70006
 * HISTORY:    Fix problem with uninitialized variable.
 * HISTORY:    Revision 1.5  2002/07/16 21:02:44Z  rm70006
 * HISTORY:    Fix bug in check for multiple writers.
 * HISTORY:    Revision 1.4  2002/07/05 16:36:46Z  rm70006
 * HISTORY:    Added destructor.
 * HISTORY:    Revision 1.3  2002/07/02 16:00:20Z  rm70006
 * HISTORY:    Added new non-write-restrictive class to CDS.
 * HISTORY:    Revision 1.2  2002/07/01 16:38:41Z  sb07663
 * HISTORY:    More descriptive logs and fatal errors
 * HISTORY:    Revision 1.1  2002/06/24 20:33:48  rm70006
 * HISTORY:    Initial revision
 * HISTORY:    Revision 1.1  2002/06/24 19:24:25Z  rm70006
 * HISTORY:    Initial revision
 * HISTORY:    Revision 1.3  2002/06/17 18:40:39Z  rm70006
 * HISTORY:    Change design to make data store instance based instead of static.  Changed from taskVars to using the symbol table library.
 * HISTORY:    Revision 1.2  2002/06/04 17:18:17Z  rm70006
 * HISTORY:    Added logging and new const ref get function.
 * HISTORY:    Revision 1.1  2002/05/28 19:00:28Z  rm70006
 * HISTORY:    Initial revision
*******************************************************************************/

#include <vxWorks.h>
#include <cstdio>

#include "datastore.h"

#define DS_LOCK_RO_EVENT   10
#define DS_LOCK_RW_EVENT   20
#define DS_UNLOCK_RO_EVENT 30
#define DS_UNLOCK_RW_EVENT 40
#define DS_CREATE_EVENT    50


// Set to turn on event logging for timing info.
#undef EVENT_TRACE

static const char DATASTORE_SYMBOL_CONTAINER[]    = "_DataStore_%s_container";
static const char BASE_ELEMENT_SYMBOL_CONTAINER[] = "_Base_Element_%s_ref_%d";
enum { DATASTORE_SYMTBL_SIZE = 6 };   // Create Max 64 datastore derived classes


////////////////////////////////////////////////////////////////
// ElementType
////////////////////////////////////////////////////////////////


//
// Default Constructor
//
ElementType::ElementType()
{
   _ds      = NULL;
   _pfrType = PfrType(-1);
}



//
// Destructor
//
ElementType::~ElementType()
{
}



//
// Register
//
bool ElementType::Register(DataStore *ds, PfrType pfr)
{
   _ds      = ds;
   _pfrType = pfr;

   return false;
}



////////////////////////////////////////////////////////////////
// DataStore
////////////////////////////////////////////////////////////////

DataStore::DATASTORE_LISTTYPE DataStore::_datastoreList;

SYMTAB_ID DataStore::_datastoreTable = NULL;

bool DataStore::_logging = false;

//
// Default Constructor
//
DataStore::DataStore()
{
   DataLog_Critical _fatal;

   // Assert an error.
   DataLog(_fatal) << "Datastore default constructor was called." << endmsg;
   _FATAL_ERROR(__FILE__, __LINE__, "Datastore default constructor called.");
}



//
// CreateSymbolTableEntry
//
void DataStore::CreateSymbolTableEntry()
{
//   const int MUTEX_SEM_FLAGS = SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE;


   //
   // Keep track of how many writers there are.  There should only be 1.
   //
   _handle->_writerDeclared = false;

   //
   // Create mutex semaphore
   //

/*
   // Create the semaphore.
   _handle->_mutexSemaphore = semMCreate(MUTEX_SEM_FLAGS);
      
   if (_handle->_mutexSemaphore == NULL)
   {
      // Fatal Error
      DataLog(_fatal) << "_mutexSemaphore could not be created for " << _name 
                       << ".  Errno " << errnoMsg << "." << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "_mutexSemaphore could not be created.");
   }
   else
      DataLog(_debug) << "_mutexSemaphore value(" << std::hex << _handle->_mutexSemaphore << std::dec << ")." << endmsg;


   // Create the semaphore.
   _handle->_readSemaphore = semBCreate(SEM_Q_PRIORITY, SEM_FULL);

   // Fatal if _writeSemaphore = NULL
   if (_handle->_readSemaphore == NULL)
   {
      DataLog(_fatal) << "_readSemaphore could not be created for " << _name 
                          << ".  Errno " << errnoMsg << "." << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "_readSemaphore could not be created.");
   }
   else
      DataLog(_debug) << "_readSemaphore value(" << std::hex << _handle->_readSemaphore << std::dec << ")." << endmsg;

   
   // Create the semaphore.
   _handle->_writeSemaphore = semBCreate(SEM_Q_PRIORITY, SEM_FULL);

   // Fatal if _writeSemaphore = NULL
   if (_handle->_writeSemaphore == NULL)
   {
      DataLog(_fatal) << "_writeSemaphore could not be created for " << _name 
                       << ".  Errno " << errnoMsg << "." << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "_writeSemaphore could not be created.");
   }
   else
      DataLog(_debug) << "_writeSemaphore value(" << std::hex << _handle->_writeSemaphore << std::dec << ")." << endmsg;

*/
   //
   // Create symbol names for mutex control flags and assign their values to the member variables.
   //
   _handle->_signalRead = false;

   _handle->_signalWrite = false;
   
   _handle->_readCount = 0;
}



//
// DataStore Constructor
//
// This constructor creates the following resources:
// 1) allocates the _writerDeclared and environment variable
// 1) allocates the _pfrList and environment variable
// 2) allocates the _mutexSemaphore and environment variable
// 3) allocates the _readSemaphore and environment variable
// 3) allocates the _writeSemaphore and environment variable
// 4) allocates the _signalWrite and environment variable
// 4) allocates the _signalRead and environment variable
// 6) allocates the _readCount and environment variable
//
//
DataStore::DataStore(const char *name, Role role) :
   _role(role),
   _debug("log datastore"),
   _refCount(0)
{
   strcpy(_name,name);
   bool created;

#if EVENT_TRACE == 1
   event_type = DS_CREATE_EVENT;
   wvEvent(event_type, (char *)_name.c_str(), _name.length());
#endif

   // Create the Symbol table.
   if (_datastoreTable == NULL)
   {
//      _datastoreTable = symTblCreate(DATASTORE_SYMTBL_SIZE, false, memSysPartId);   // Create a symbol table that does NOT allow duplicate items.
      _datastoreTable = 1;
   }

   //
   // Create the symbol table entry for this datastore instance
   //
   BindItem(this, &_handle, ITEM_DATASTORE_SYMBOL_CONTAINER, created);

   // If first time, populate handle
   if (created)
   {
      CreateSymbolTableEntry();
      
      // First derived datastore instance.  Save it in the std::list of datastores.
      // The std::list of datastores is used during by PFR to restore all datastores
      // (element values) to their last state.
//      _datastoreList.push_back(this);

      if (_logging)
      {
         DataLog(_debug) << "First instance of " << _name << " created.  Saving datastore." << endmsg;
      }
   }

#if EVENT_TRACE == 1
   wvEvent(++event_type, (char *)_name.c_str(), _name.length());
#endif
}



//
// Base Destructor
//
DataStore::~DataStore()
{
   // Don't delete anything.  The rest of the system is still using it.
}




//
// Semaphore Take macro
//
#define SEM_TAKE(sem, wait)                                                           \
//{                                                                                     \
//   STATUS status = semTake(sem, wait);                                                \
//   if (status != OK)                                                                  \
//   {                                                                                  \
//      DataLog(_fatal) << "SemTake failed.  Errno " << errnoMsg << ".  " << endmsg;    \
//      _FATAL_ERROR(__FILE__, __LINE__, "semTake failed");                             \
//   }                                                                                  \
//}



//
// Semaphore Give macro
//
#define SEM_GIVE(sem)                                                                 \
//{                                                                                     \
//   STATUS status = semGive(sem);                                                      \
//   if (status != OK)                                                                  \
//   {                                                                                  \
//      DataLog(_fatal) << "SemGive failed.  Errno " << errnoMsg << ".  " << endmsg;    \
//      _FATAL_ERROR(__FILE__, __LINE__, "semGive failed");                             \
//   }                                                                                  \
//}



//
// Semaphore Flush macro
//
#define SEM_FLUSH(sem)                                                                 \
//{                                                                                      \
//   STATUS status = semFlush(sem);                                                      \
//   if (status != OK)                                                                   \
//   {                                                                                   \
//      DataLog(_fatal) << "SemFlush failed.  Errno " << errnoMsg << ".  " << endmsg;    \
//      _FATAL_ERROR(__FILE__, __LINE__, "semFlush failed");                             \
//   }                                                                                   \
//}



//
// Begin Critical Section macro
//
//#define BEGIN_CRITICAL_SECTION() SEM_TAKE(*_mutexSemaphore, WAIT_FOREVER);
#define BEGIN_CRITICAL_SECTION() ;



//
// End Critical Section macro
//
//#define END_CRITICAL_SECTION() SEM_GIVE(*_mutexSemaphore);
#define END_CRITICAL_SECTION() ;



//
// Lock
//
void DataStore::Lock()
{
   bool crit_section_released = false;
   int event_type = 0;

   // If instance is RO, perform RO semaphore lock
   if (_role == ROLE_RO || _role == ROLE_SPOOFER )
   {
#if EVENT_TRACE == 1
      event_type = DS_LOCK_RO_EVENT;
      wvEvent(event_type, (char *)_name.c_str(), _name.length());
#endif

      BEGIN_CRITICAL_SECTION();

      // If a writer has signaled to write, readers should block until after 
      // writer releases semaphore.
      if (_handle->_signalWrite)
      {
         
         END_CRITICAL_SECTION();

         crit_section_released = true;

         // Block waiting for writer
         SEM_TAKE(_handle->_readSemaphore, WAIT_FOREVER);
         
         // Reset semaphore for next writer
         SEM_GIVE(_handle->_readSemaphore);

         event_type += 0x1;
      }
      
      if (crit_section_released)
      {
         BEGIN_CRITICAL_SECTION();
      }

      // Increment the read count
      ++(_handle->_readCount);

      if (_handle->_signalRead == false)
      {
         // Signal Writers to block for read
         _handle->_signalRead = true;

         SEM_TAKE(_handle->_writeSemaphore, WAIT_FOREVER);   // Non-blocking

         event_type += 0x2;
      }

      END_CRITICAL_SECTION();
   }
   else  // Do RW semaphore lock
   {
#if EVENT_TRACE == 1
      event_type = DS_LOCK_RW_EVENT;
      wvEvent(event_type, (char *)_name.c_str(), _name.length());
#endif

      BEGIN_CRITICAL_SECTION();

      // Set write count.
      _handle->_signalWrite = true;

      if (_handle->_signalRead)
      {
         // Block future readers (RBOW).  At this point, we shouldn't block.
         SEM_TAKE(_handle->_readSemaphore, WAIT_FOREVER);    // Non-blocking

         END_CRITICAL_SECTION();

         // Block on the Read semaphore (WBOR)
         SEM_TAKE(_handle->_writeSemaphore, WAIT_FOREVER);    // Blocking

         BEGIN_CRITICAL_SECTION();

         // Give back read semaphore.
         SEM_GIVE(_handle->_writeSemaphore);    // Reset for next reader.

         END_CRITICAL_SECTION();
      }
      else
      {

         // Block future writers (RBOW).  At this point, we shouldn't block.
         SEM_TAKE(_handle->_readSemaphore, WAIT_FOREVER);    // Non-blocking

         END_CRITICAL_SECTION();
      }
   }

#if EVENT_TRACE == 1
   wvEvent(event_type, (char *)_name.c_str(), _name.length());
#endif
}



//
// Unlock
//
void DataStore::Unlock()
{
   int event_type = 0;

   // If instance is RO, perform RO semaphore lock
   if (_role == ROLE_RO || _role == ROLE_SPOOFER )
   {
#if EVENT_TRACE == 1
      event_type = DS_UNLOCK_RO_EVENT;
      wvEvent(event_type, (char *)_name.c_str(), _name.length());
#endif

      BEGIN_CRITICAL_SECTION();

      if (_handle->_readCount > 0)
      {
         --(_handle->_readCount);
      }

      if (_handle->_signalWrite && _handle->_signalRead)   // Only do the first time.
      {
         event_type += 0x1;

         // Unlock the writer
         SEM_GIVE(_handle->_writeSemaphore);

         // Clear/Reset read flag
         _handle->_signalRead = false;

      }
      else if (_handle->_readCount == 0)   // No more readers.
      {
         event_type += 0x2;

         // Unlock the writer
         SEM_GIVE(_handle->_writeSemaphore);

         // Clear/Reset read flag
         _handle->_signalRead = false;

      }
      else
      {
         event_type += 0x4;

      }

      END_CRITICAL_SECTION();
   }
   else  // Do RW semaphore lock
   {
#if EVENT_TRACE == 1
      event_type = DS_UNLOCK_RW_EVENT;
      wvEvent(event_type, (char *)_name.c_str(), _name.length());
#endif

      BEGIN_CRITICAL_SECTION();

      // Clear the Write signal
      _handle->_signalWrite = false;

      END_CRITICAL_SECTION();

      // Unlock the semaphore
      SEM_GIVE(_handle->_readSemaphore);
      SEM_FLUSH(_handle->_readSemaphore);

      event_type += 0x1;
   }

#if EVENT_TRACE == 1
   wvEvent(event_type, (char *)_name.c_str(), _name.length());
#endif
}



//
// symbolName
//
void DataStore::GetSymbolName(char *s, const BIND_ITEM_TYPE item)
{
   int size = 0;
   const int s_len = strlen(s);

   // Create the Symbol name to search for.
   switch (item)
   {
   case ITEM_DATASTORE_SYMBOL_CONTAINER:
      size = sprintf(s, DATASTORE_SYMBOL_CONTAINER, _name);
      break;

   case ITEM_BASE_ELEMENT_SYMBOL_CONTAINER:
      size = sprintf(s, BASE_ELEMENT_SYMBOL_CONTAINER, _name, _refCount);
      ++_refCount;
      break;

   default:
      DataLog(_fatal) << "Unknown symbol type(" << item << ")" << "in " << Name() << "." << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "Unknown symbol type.");
   }

   if (size > s_len)
   {
      // Dude.  You just wiped out something.
      DataLog(_fatal) << "datastore item(" << item << ") name size mismatch in " << Name() 
                       << ".  Generated size is " << size << ".  Available size is " << s_len << "." << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "Unknown symbol type.");
   }

}



//
// SingleWriteDataStore
//



//
// Base Constructor
// 
SingleWriteDataStore::SingleWriteDataStore(const char * name, Role role) :
   DataStore (name, role)
{
   // Ensure no multiple writers
   CheckForMultipleWriters();
}



//
// Class Destructor
//
SingleWriteDataStore::~SingleWriteDataStore()
{
   if (GetRole() == ROLE_RW)
   {
      _handle->_writerDeclared = false;
      DataLog(_debug) << "Writer exited for CDS " << Name() << "." << endmsg;
   }
}



//
// CheckForMultipleWriters
//
void SingleWriteDataStore::CheckForMultipleWriters()
{
   if (GetRole() == ROLE_RW)
   {
      // The base implementation of DataStore fatal errors when multiple writers are declared.
      if (_handle->_writerDeclared)
      {
         // This is an error.
         DataLog(_fatal) << "Error.  Multiple Writers Declared for CDS " << Name() << ".  Abort!!!!!!" << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Datastore multiple writers");
         return;
      }
      else
         _handle->_writerDeclared = true;
   }
}



//
// MultWriteDataStrore
//



//
// Base Constructor
// 
MultWriteDataStore::MultWriteDataStore(const char * name, Role role) :
   DataStore (name, role)
{
   // Empty check.  Included for completeness.
   CheckForMultipleWriters();
}



//
// Class Destructor
// 
MultWriteDataStore::~MultWriteDataStore()
{
}



//
// DynamicSingleWriteDataStrore
//



//
// Base Constructor
// 
DynamicSingleWriteDataStore::DynamicSingleWriteDataStore(const char * name, Role role) :
   SingleWriteDataStore (name, role)
{
}



//
// Class Destructor
// 
DynamicSingleWriteDataStore::~DynamicSingleWriteDataStore()
{
}



//
// SetRead
//
void DynamicSingleWriteDataStore::SetRead()
{
   // If you are currently the writer, then unset the _writerDeclared flag
   if (_role == ROLE_RW)
   {
      _handle->_writerDeclared = false;
   }

   _role = ROLE_RO;
}



//
// SetWrite
//
void DynamicSingleWriteDataStore::SetWrite()
{
   _role = ROLE_RW;

   CheckForMultipleWriters();
}



int dump_table_choice;

