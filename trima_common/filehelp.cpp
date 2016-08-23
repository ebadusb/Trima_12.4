/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      filehelp.cpp
 *             CFileHelper class is a low overhead and efficient helper
 *             to accumulate separate disk writes into memory before
 *             writing to disk.  Its purpose is to minimize file open
 *             and write time to minimize the risk of power loss
 *             losing the file.
 *             It starts with minimal overhead and allows data to be
 *             appended by string or by buffer/length.  The memory buffer
 *             grows as needed in large blocks to minimize mallocs.
 *             A write actually performs an open, write block, and close
 *             back to back.
 *             Other helper functions include a read from disk, calculate
 *             CRC, obtain a text string of the CRC, and compare to a file
 *             containing a text string CRC.
 *             This may find other uses but is primarily designed to aid
 *             in config.dat writes and crc checks.  May be used for
 *             other configuration files that have a separate crc file and
 *             any other disk writing from within the trima soaftware.
 *
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/
#include <vxworks.h>
#include <usrLib.h>

#include "filehelp.h"
#include "error.h"
#include <cstdio>
#include <string.h>
#include <iostream>
#include "crcgen.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "fcntl.h"
#include "filenames.h"

#ifndef _MSC_VER
#include <unistd.h>
#endif


#define BLOCKSIZE  10000

CFileHelper::CFileHelper()
{
   m_pData      = 0;
   m_nAllocated = m_nLength = 0;
}

CFileHelper::~CFileHelper()
{
   Empty();
}

void CFileHelper::Empty ()
{
   delete [] m_pData;
   m_pData = 0;
}


void CFileHelper::Append (const char* str)
{
   Append(str, strlen(str) );
}

void CFileHelper::Append (const char* str, int length)
{
   ReSize(length);
   memcpy(&m_pData[m_nLength], str, length);
   m_nLength += length;
}

// This routine checks if the needed size is available
// and does nothing if it is.  If not, it grows the buffer
// and copies the old data over.
void CFileHelper::ReSize (int sz)
{
   if ( ( m_nLength + sz ) >= m_nAllocated )
   {
      m_nAllocated = m_nLength + sz + BLOCKSIZE;
      char* ptr = new char[m_nAllocated];
      memcpy(ptr, m_pData, m_nLength);
      Empty();
      m_pData = ptr;
   }
}

unsigned long CFileHelper::CRC () const
{
   unsigned long lcrc = INITCRC_DEFAULT;
   crcgen32(&lcrc, (unsigned char*)m_pData, m_nLength);
   return lcrc;
}

unsigned long CFileHelper::CRC (char* str) const
{
   unsigned long lcrc = CRC();
   sprintf (str, "0x%08x", (unsigned int)lcrc);   // Force 8 character output
   return lcrc;
}

int CFileHelper::Length () const
{
   return m_nLength;
}

char* CFileHelper::pData ()
{
   return m_pData;
}

const char* CFileHelper::cpData () const
{
   return m_pData;
}

bool CFileHelper::FastWrite (const char* fname, bool readOnlyFile)
{
   bool bFlag = true;

   attrib(fname, "-R");

   const int fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0666);

   if ( fd == -1 )
   {
      perror(__FILE__);
      std::cerr << __FILE__ << " " << __LINE__ << " Unable to open file" << std::endl;

      return 0;
   }

   taskDelay(0);  // Make sure other tasks get a chance to interupt (avoid priority inversion)
   const int ln = write (fd, m_pData, m_nLength);
   taskDelay(0);  // Make sure other tasks get a chance to interupt (avoid priority inversion)

   if (ln != m_nLength)
   {
      perror(__FILE__);
      std::cerr << __FILE__ << " " << __LINE__ << " Write: " << m_nLength << " bytes, ret = " << ln
                << ": " << fname << std::endl;
      bFlag = false;
   }

   if (close(fd) == -1)
   {
      perror(__FILE__);
      std::cerr << __FILE__ << " " << __LINE__ << " Close: " << fname << std::endl;
      bFlag =  false;
   }

   if (readOnlyFile)
      attrib(fname, "+R");

   return bFlag;
}




bool CFileHelper::Read (const char* fname)
{
   long sz = FileSize(fname);

   if ( sz == 0 )
      return 0;

   Empty();
   m_nAllocated = m_nLength = 0;
   ReSize(sz);

   // regular read is fine for reading
   FILE* fp = fopen(fname, "r");

   if ( fp == 0 )
      return 0;

   int  szret;
   bool bFlag = true;

   if ( ( szret = fread(m_pData, sizeof( char ), sz, fp)) != sz )
   {
      taskDelay(0);   // Make sure other tasks get a chance to interupt (avoid priority inversion)

      perror(__FILE__);
      std::cerr << __FILE__ << " " << __LINE__ << " Read Error on file " << fname
                << ". ret = " << szret << std::endl;
      bFlag = false;
   }

   m_nLength = sz;

   if ( fclose(fp) != 0 )
   {
      perror(__FILE__);
      std::cerr << __FILE__ << " " << __LINE__ << " FClose Error on file " << fname
                << "." << std::endl;
      bFlag = false;
   }

   return bFlag;
}




long CFileHelper::FileSize (const char* fname)
{
   struct stat sbuf;
   if ( stat( (char*)fname, &sbuf) != 0 )
   {
      perror(__FILE__);
      std::cerr << __FILE__ << " " << __LINE__ << " Error on file " << fname << "." << std::endl;
      return 0;
   }
   return sbuf.st_size;
}

bool CFileHelper::CompareCRC (const char* crcFile)
{
   char                FileCRC[20];

   const unsigned long thisCRC = CRC();

   FILE*               fp      = fopen (crcFile, "r");

   if (fp == 0)
      return 0;

   if (fgets (FileCRC, 15, fp) == 0)
   {
      FileCRC[0] = 0;
   }

   fclose (fp);

   const unsigned long intFileCRC  = strtoul(FileCRC, 0, 0);

   bool                returnValue = (thisCRC == intFileCRC);

   if ( returnValue == false )
   {
      std::cerr << __FILE__ << " " << __LINE__ << " CRC Compare on " << crcFile << ": "
                << std::hex << "[" << thisCRC << "] crc file vs computed of [" << intFileCRC << "]." << std::dec << std::endl;
   }

   return returnValue;
}



////////////////////////////////////////////////////////////
// config file name helper

CCfgFilenames::CCfgFilenames(const char* str)
{
   // make sure each string has enough space
   int len = strlen(str) + strlen(CONFIG_CRC_PATH) +
             strlen(CONFIG_PATH) + 50;

   const char* pSlash = "/";
   const char* pBk    = ".bk";

   m_pFile      = new char[len];
   m_pCRC       = new char[len];
   m_pBackup    = new char[len];
   m_pCRCBackup = new char[len];

   strcpy(m_pFile, CONFIG_PATH);
   strcat(m_pFile, pSlash);
   strcat(m_pFile, str);

   strcpy(m_pBackup, m_pFile);
   strcat(m_pBackup, pBk);

   strcpy(m_pCRC, CONFIG_CRC_PATH);
   strcat(m_pCRC, pSlash);
   strcat(m_pCRC, str);
   // replace ".dat" extension with ".crc"
   char* p = strrchr(m_pCRC, '.');

   if (p != 0)
      *p = 0;

   strcat(m_pCRC, ".crc");

   strcpy(m_pCRCBackup, m_pCRC);
   strcat(m_pCRCBackup, pBk);
}

CCfgFilenames::~CCfgFilenames()
{
   delete [] m_pFile;
   delete [] m_pCRC;
   delete [] m_pBackup;
   delete [] m_pCRCBackup;
}

/* FORMAT HASH 224ff4e4d678a91451c18cd9fa8bc6d3 */
