/*******************************************************************************
 *
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cassUtils.hpp
 *             CassetteUtils
 *
 * AUTHOR:     Shawn Price
 *
 * ABSTRACT:   Safety utility class providing ad-hoc read/search
 *             capabilities for target both setconfig.dat - supervisor
 *             configured cassette catalog - and cassette.dat.
 *******************************************************************************/

#ifndef CASSETTE_UTILS_HPP  //  Prevents multiple compilations.
#define CASSETTE_UTILS_HPP

#include <fstream>
#include <sys/stat.h>

#include "alarms.h"                   // Get alarm defns

/*****************************************************************************
*
*  CassetteUtils CLASS DEFINITION
*
******************************************************************************/
class CassetteUtils
{
public:
   // Retieve admin code from cassette catalog with
   // REF number 'ref'
   unsigned int adminCodeByRef (unsigned int ref);

private:
   /*****************************************************************************
   *
   *  CassetteRecord CLASS DEFINITION
   *
   * CassetteRecord is a simple, immutable object we use as the intermediate
   * representation for a cassette entry in the date file.  We'll use these
   * to pass entries back from the file reader so that CassetteUtils remains
   * flexible.
   ******************************************************************************/
   class CassetteRecord
   {
   public:
      CassetteRecord(unsigned int ref, unsigned int code)
         : m_ref(ref),
           m_adminCode(code){}

      ~CassetteRecord() {}

      // getters
      unsigned int refNumber () const { return m_ref; }
      unsigned int adminCode () const { return m_adminCode; }

      // match functions
      bool matchRefNumber (const unsigned int& ref) const { return (ref == m_ref); }
      bool matchAdminCode (const unsigned int& code) const { return (code == m_adminCode); }

   private:
      // not implemented
      CassetteRecord();
      CassetteRecord(CassetteRecord const&);
      CassetteRecord& operator = (CassetteRecord const&);

      unsigned int m_ref;
      unsigned int m_adminCode;

   };       // End CassetteRecord definition

private:
   // file management operations
   bool openFile (const char* datFilePath, ifstream& inFile);
   bool checkFile (char* datFilePath, char* crcFilePath);
   void fileError (const char* msg, ALARM_VALUES alrmVal);

   // internal search methods
   unsigned int adminCodeFromRef (unsigned int ref, char* filePath);

   // line reader
   bool nextRecord (ifstream& inFile, const char* datFilePath, CassetteRecord** record);

};  // End CassetteUtils definition

#endif // CASSETTE_UTILS_HPP

/* FORMAT HASH 5e8735e49d1bbc605ec7c54101e85e83 */
