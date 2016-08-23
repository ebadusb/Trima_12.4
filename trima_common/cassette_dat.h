/*
 * Copyright (C) 2012 Gambro BCT, Inc.  All rights reserved.
 *
 * This class encapsulates the cassette.dat and the setconfig.dat files.
 * The files are read during task initialization and are sentinel objects.
 *
 * $Header: //depot/main/embedded/Trima/Main/trima_common/cassette_dat.h#1 $
 *
 */


#ifndef _CASSETTE_DAT_H_
#define _CASSETTE_DAT_H_

#include <vector>

#include "alarms.h"                   // Get alarm defns
#include "filehelp.h"                 // Get CFileHelper
#include "cassette.h"                 // Gets the cassette container

// FOR TRAVERSING THE CASSETTE LIST OUTSIDE THIS CLASS
typedef vector<CatalogNumber*> CASSETTE_VECTOR_TYPE;
typedef CASSETTE_VECTOR_TYPE::iterator CASSETTE_VECTOR_ITERATOR;

typedef map<string, CatalogNumber*> CASSETTE_MAP_TYPE;
typedef CASSETTE_MAP_TYPE::iterator CASSETTE_MAP_ITERATOR;


class CassetteDat
{
public:
   CassetteDat();
   virtual ~CassetteDat();

protected:
   static void fileError (const char* msg, ALARM_VALUES alrmVal);
   static bool checkFile (const char* datFilePath, const char* crcFilePath, const char* datFileBkPath, const char* crcFileBkPath);
   static bool fixFile (CFileHelper& goodFile, const char* fname, const char* crcfname);
};



class MasterCassetteDat
   : public CassetteDat
{
public:
   MasterCassetteDat();
   virtual ~MasterCassetteDat();

   // Exported iterator methods
   static CASSETTE_MAP_ITERATOR find (const char* cassetteIdString);

private:
public:
   // READ THE MASTER CASSETTE FILE
   static void read ();
   static void clear ();

   static CASSETTE_MAP_ITERATOR begin ();
   static CASSETTE_MAP_ITERATOR end ();

   static bool fileOK (void) {return _fileOK; }

protected:
   static bool _fileOK;

   static char _fileVersion[80];
};



class AdminCassetteDat
   : public CassetteDat
{
public:
   AdminCassetteDat();
   virtual ~AdminCassetteDat();

   // READ THE ADMIN CASSETTE FILE
   static void read ();

   static bool fileOK (void){return _fileOK; }

   // Exported iterator methods
   static CASSETTE_VECTOR_ITERATOR begin ();
   static CASSETTE_VECTOR_ITERATOR end ();

   static void erase (CASSETTE_VECTOR_ITERATOR it);

   static unsigned int size (void);
   static void         clear (void);

   static void addCassette (const char* refNumber, unsigned int tubingSetCode, const char* barcode);

   // UPDATE THE CASSETTE FILE
   static bool updateCassetteFile ();
   static void updateIfChanged ();

protected:
   // ADD THE BEGINNING-FILE DECORATIONS TO THE ADMIN FILE
   static void decorateFile (ostream& o);

protected:
   static bool _fileOK;

   static char _fileVersion[80];
   static bool _fileHasChanged;
};



#endif

/* FORMAT HASH 24a6a2e2b446bbc76b974c47481aa678 */
