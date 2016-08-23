/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      filehelp.h
 *             CFileHelper class definition for accumulating writes into
 *             one large write for the purpose of blasting a faster single
 *             write to disk.
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/


#if !defined( INCLUDE_FILEHELP )
#define INCLUDE_FILEHELP

class CFileHelper
{

public:
   CFileHelper();
   ~CFileHelper();

   // clear our memory copy, zero length
   void Empty (void);
   // Add a zero terminated string to the end of the file (in memory)
   // The zero is not copied.
   void Append (const char* str);
   // Append 'length' bytes to the end.
   void Append (const char* pBuff, int length);

   // Calculate the crc of the current memory copy
   unsigned long CRC (void) const;
   // return a hex string representing the CRC
   unsigned long CRC (char* hexstring) const;

   // current length of the file.
   int Length (void) const;

   // do an open, write and close on this file.
   bool FastWrite (const char* filename, bool readOnlyFile = true);

   // do an open read and close.
   bool Read (const char* filename);

   // This compares the memory crc to the hexstring crc
   // located in the specified file.
   bool CompareCRC (const char* crcfile);

   // Obtain an existing file's size on disk.
   // Return -1 if not exist.
   // Otherwise, return the number of bytes.
   long FileSize (const char* fname);


   // access to data
   char*       pData (void);
   const char* cpData (void) const;

protected:
   void ReSize (int sz);

private:
   char* m_pData;
   int   m_nAllocated;
   int   m_nLength;


};

// This is a filename helper for configuration backup
// filenames.  Create it with the base filename (eg.
//  "hw.dat", or "config.dat" etc.
// It provides methods for the four filenames:
//      base file, base file backup, crc file, crc backup file
//  These are complete pathnames using the default paths
//  defined in filenames.h.
// With this interface, multiple users of files will
// not inadvertently access the wrong file name.

class CCfgFilenames
{

public:
   CCfgFilenames(const char* basename);

   ~CCfgFilenames();

   const char* PNameFile (void) const { return m_pFile; }
   const char* PNameBackup (void) const { return m_pBackup; }
   const char* PNameCRC (void) const { return m_pCRC; }
   const char* PNameCRCBackup (void) const { return m_pCRCBackup; }

private:
   CCfgFilenames();
   char* m_pFile;
   char* m_pCRC;
   char* m_pBackup;
   char* m_pCRCBackup;
};


#endif

/* FORMAT HASH 55868d1d524a581f158e93e3282d4332 */
