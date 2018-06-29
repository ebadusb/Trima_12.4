/*
 *	$Header: E:/BCT_Development/Trima5.R/Trima/service/rcs/updateTrima.cpp 1.12 2009/03/20 15:31:41Z dslausb Exp dslausb $
 *
 * Update software used by EMS for installing new config files
 *   Now new and improved... and ubiqutous!
 *   by that i mean:
 *     it will copy any x.dat file in /machine/update to /config
 *     then it will generate a CRC from /trima/softcrc/filelist/ by
 *     taking any /trima/softcrc/filelist/x.file  and
 *     genetating a /config/crc/x.crc using softcrc.
 *
 * so for this to work, any new x.dat file needs a new x.file file .
 *  --jph
 *
 * $Log: updateTrima.cpp $
 * Revision 1.12  2009/03/20 15:31:41Z  dslausb
 * - Transparent bitmaps
 * - Stand-alone graphics packages
 * - String.info CRCs
 * - Font mapping for Asian languages
 * Revision 1.11  2008/11/20 20:28:46Z  dslausb
 * Change MFR update script so that logging is the same as what we had in 5.1.
 * Revision 1.10  2008/07/15 17:09:33Z  ssunusb
 * IT 8765 - install globvars file
 * Revision 1.9  2006/09/15 16:07:32Z  jheiusb
 * 7749 -- fix update to do any .dat file
 *
 */
#include <vxWorks.h>
#include <bootLib.h>
#include <sysLib.h>
#include <taskLib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cbioLib.h>
#include <dirent.h>
#include <dosFsLib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <usrLib.h>
#include <private/dosFsLibP.h>
#include <sys/stat.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>

#include "filenames.h"
#include "zlib.h"
#include "../os/targzextract.c"
#include "crcgen.h"
#include "env_names.h"

FILE* fp = NULL;

static bool copyFile (const char* sourceFileName, const char* destFileName, bool lock = true)
{
   attrib(destFileName, "-R");  // Unlock regardless of lock flag

   fprintf(fp, "copying file %s to %s\n", sourceFileName, destFileName);

   if ( cp(sourceFileName, destFileName) == ERROR )
   {
      fprintf(fp, "copy of %s failed\n", destFileName);
      if (lock) attrib(destFileName, "+R");
      return false;
   }

   if (lock) attrib(destFileName, "+R");  // Lock only if lock bit is set.
   return true;
}

static int zipFile (const char* from, const char* to)
{
   int    fromFD = open(from, O_RDONLY, 0644);
   gzFile toFD   = gzopen(to, "w");

   if ( fromFD >= 0 && toFD >= 0 )
   {
      int  bytesRead;
      int  bytesWritten = 0;
      char buffer[10 * 1024];

      while ( (bytesRead = read(fromFD, buffer, 10 * 1024)) > 0 )
      {
         bytesWritten += gzwrite(toFD, buffer, bytesRead);
      }

      gzclose(toFD);
      close(fromFD);

      return 1;
   }
   else
   {
      if ( fromFD < 0 )
      {
         perror(from);
      }
      else
      {
         close(fromFD);
      }

      if ( toFD < 0 )
      {
         perror(to);
      }
      else
      {
         gzclose(toFD);
      }
   }

   return 0;
}

static int unzipFile (const char* from, const char* to)
{
#ifdef WIN32
   gzFile fromFD = gzopen(from, "rb");
#else
   gzFile fromFD = gzopen(from, "r");
#endif
   int    toFD   = open(to, O_CREAT | O_RDWR, 0644);

   if ( fromFD >= 0 && toFD >= 0 )
   {
      int  bytesRead;
      int  bytesWritten  = 0;
      int  kBytesWritten = 0;
      char buffer[10 * 1024];

      while ( (bytesRead = gzread(fromFD, buffer, 10 * 1024)) > 0 )
      {
         bytesWritten += write(toFD, buffer, bytesRead);
         kBytesWritten = bytesWritten / 1024;
      }

      gzclose(fromFD);
      close(toFD);

      return 1;
   }
   else
   {
      if ( fromFD < 0 )
      {
         perror(from);
      }
      else
      {
         gzclose(fromFD);
      }

      if ( toFD < 0 )
      {
         perror(to);
      }
      else
      {
         close(toFD);
      }
   }

   return 0;
}

static string getSerialNumber (const char* sourceFileName)
{
   int      lineLength = 256;
   char     line[lineLength];
   ifstream globVars(sourceFileName);
   string   ret;
   char*    loc = NULL;
   if (globVars)
   {
      while (!globVars.eof())
      {
         globVars.getline(line, lineLength);
         loc = strstr(line, "MACHINE=");
         if (loc)
         {
            ret = loc + strlen("MACHINE=");
            break;
         }
      }
   }

   // Remove non-alphanumeric characters from serial number
   ret.erase(std::remove_if(ret.begin(), ret.end(), std::not1(std::ptr_fun(std::isalnum))), ret.end());

   return ret;
}

static void checkForSerialNumber (const char* sourceFileName)
{
   fprintf(fp, "Checking for Serial Number\n");
   char   tempFileName[NAME_MAX + 1];
   string source = "/machine/update/machine.id";
   sprintf(tempFileName, "%s.tmp", source.c_str());
   unzipFile(source.c_str(), tempFileName);
   string   line;
   ifstream machineFile(tempFileName);
   if (machineFile)
   {
      machineFile.seekg(0, machineFile.end);
      int length = machineFile.tellg();
      machineFile.seekg(0, machineFile.beg);

      char* buffer = new char[length];

      machineFile.read(buffer, length);

      machineFile.close();

      if (!strstr(buffer, "serial_number="))
      {
         std::stringstream serialNum ;
         serialNum << "serial_number=" << getSerialNumber(GLOBVARS_FILE);
         fprintf(fp, "serial: %s\n", serialNum.str().c_str());
         unsigned long crc = 0;
         crcgen32(&crc, (const unsigned char*)serialNum.str().c_str(), serialNum.str().length());
         serialNum << "," << hex << crc;

         ofstream outFile;
         outFile.open(tempFileName, ios::app);
         outFile << serialNum.str() << endl;
         outFile.close();

         zipFile(tempFileName, source.c_str());
      }

      delete[] buffer;
   }
}

static void updateMachineId ()
{
   char   tempMachineIdFile[NAME_MAX + 1];
   string machineIdFile = PNAME_MACHINE_ID;
   string globvarsFile = "/machine/update/globvars";
   string newMachineIdFile = "/machine/update/newmachine.tmp";

   attrib(PNAME_MACHINE_ID,"-R");
   sprintf(tempMachineIdFile, "%s.tmp", machineIdFile.c_str());
   unzipFile(machineIdFile.c_str(), tempMachineIdFile);

   // Get the serial number from globvars file being updated
   std::stringstream serialNum ;
   serialNum << "serial_number=" << getSerialNumber(globvarsFile.c_str());
   unsigned long crc = 0;
   crcgen32(&crc, (const unsigned char*)serialNum.str().c_str(), serialNum.str().length());
   serialNum << "," << hex << crc;

   // Open the Temp MachineID file for reading
   string   line;
   ifstream machineFile(tempMachineIdFile);
   ofstream tempNewFile;
   tempNewFile.open(newMachineIdFile.c_str());

   while (getline(machineFile, line))
   {
      if (line.find("serial_number=") == std::string::npos)
      {
         tempNewFile << line << endl;
      }
   }

   // Now copy the new serial number
   tempNewFile << serialNum.str() << endl;
   tempNewFile.close();
   remove(tempMachineIdFile);
   zipFile(newMachineIdFile.c_str(), machineIdFile.c_str());
   attrib(PNAME_MACHINE_ID,"+R");
}

static void fileProcess (const char* dirName, const char* fileName)
{
   int  nameLen = strlen(fileName);
   int  dirLen  = strlen(dirName);

   char sourceFileName [NAME_MAX + 1];
   char destFileName   [NAME_MAX + 1];

   sprintf(sourceFileName, "%s/%s", dirName, fileName);

   ////  Check for obvious ignore cases  ////////////////
   if ((nameLen < 5) ||
       (nameLen > NAME_MAX) ||
       (!strcmp(fileName, "trima.files")) ||
       (!strcmp(fileName, "safety.files")))
   {
      fprintf(fp, "ignored fileName: %s \n", fileName);
      return;
   }

   ////  PROCESS THE .CRC FILES ////////////////
   else if (nameLen > 6 && strcmp(&fileName[nameLen - 6], ".files") == 0)
   {
      char command[128];

      sprintf(destFileName, "/config/crc/%s", fileName);
      strcpy(&destFileName[strlen(destFileName) - 6], ".crc");
      sprintf(command, "-filelist /trima/softcrc/filelists/%s -update %s", fileName, destFileName);

      fprintf(fp, "generating crc from  %s for %s \n", fileName, destFileName);

      attrib(destFileName, "-R");    // should be unlocked... but just in case

      // do the softcrc....
      softcrc(command);

      return;   // Return, since nothing needs to be copied.
   }

   ////  PROCESS THE machine.id file ////////////////
   else if (nameLen > 4 &&
            (strcmp(fileName, FILE_MACHINE_ID) == 0))
   {
      sprintf(destFileName, "/config/%s", fileName);
      checkForSerialNumber(fileName);
   }

   ////  PROCESS THE .DAT FILES  ////////////////
   else if (nameLen > 4 &&
            (strcmp(&fileName[nameLen - 4], ".dat") == 0))
   {
      sprintf(destFileName, "/config/%s", fileName);
   }

   ////  PROCESS THE GLOBVARS FILE  ////////////
   else if (nameLen > 4 &&
            (strcmp(fileName, "globvars") == 0))
   {
      sprintf(destFileName, "/config/%s", fileName);
      updateMachineId();
   }
   ////  PROCESS THE GRAPHICS FILES
   else if (nameLen >= 20 &&
            strcmp(&fileName[nameLen - 20], "graphics_package.out") == 0)
   {
      sprintf(destFileName, "%s/%s", GRAPHICS_PATH, fileName);
   }

   ////  PROCESS THE .INFO FILES (String or Data)
   else if (nameLen >= 9 &&
            !strcmp(&fileName[nameLen - 5], ".info"))
   {
      if (strstr(fileName, "string") == fileName )
         sprintf(destFileName, "%s/%s", STRING_DIRECTORY, fileName);
      else if (strstr(fileName, "font"))
         sprintf(destFileName, "%s/%s", DROP_IN_FONTS_DIR, fileName);
      else if (strstr(fileName, "data"))
         sprintf(destFileName, "%s/%s", DATA_DIRECTORY, fileName);
      else
      {
         fprintf(fp, "ignored .info fileName: %s \n", fileName);
         return;
      }
   }

   ////  PROCESS THE FONT FILES
   else if (nameLen >= 5 &&
            ((strcmp(&fileName[nameLen - 4], ".ttf") == 0) ||
             (strcmp(&fileName[nameLen - 4], ".stf") == 0) ||
             (strcmp(&fileName[nameLen - 4], ".ccc") == 0)))
   {
      sprintf(destFileName, "%s/%s", DROP_IN_FONTS_DIR,   fileName);
   }

   else if (nameLen == 14 && !strcmp(fileName, "guiPackage.taz"))
   {
      sprintf(destFileName, "%s/guiPackage", dirName);

      if ( tarExtract(fileName, destFileName) == ERROR )
      {
         fprintf(fp, "Extraction of %s/guiPackage failed\n", dirName);
         return;
      }
      else if ( remove(fileName) == ERROR )
      {
         fprintf(fp, "Removal of %s failed\n", fileName);
         return;
      }
      else
      {
         DIR*           subdir      = opendir(destFileName);

         struct dirent* subDirEntry = NULL;
         struct stat    subDirFileStat;

         // Todo: we should delete all the old string and
         // font files.

         while ( (subDirEntry = readdir(subdir)) != NULL )
         {
            stat(subDirEntry->d_name, &subDirFileStat);

            if ( S_ISREG(subDirFileStat.st_mode) )
               fileProcess(destFileName, subDirEntry->d_name);
         }

         closedir(subdir);
      }

      return;
   }

   // IGNORE ALL OTHER FILES
   else
   {
      fprintf(fp, "ignored fileName: %s \n", fileName);
      return;
   }

   copyFile(sourceFileName, destFileName);
}

static void dirProcess (const char* dirName)
{
   char tmpDirName[NAME_MAX + 1];
   sprintf(tmpDirName, "%s", dirName);

   DIR* dir = opendir(tmpDirName);

   if ( dir )
   {
      fprintf(fp, "Processing directory: %s\n", tmpDirName);

      struct dirent* dirEntry = NULL;
      struct stat    fileStat;

      // First, go through and see if there are any archives that
      // need to be processed.
      while ( (dirEntry = readdir(dir)) != NULL )
      {
         /*
          *	Skip . and .. directory entries
          */
         if ( !strcmp(dirEntry->d_name, ".") || !strcmp(dirEntry->d_name, "..") ) continue;


         // skip the installer!
         if ( !strcmp(dirEntry->d_name, "updateTrima") ) continue;

         /*
          * Skip CRC directories
          */
         if ( !strcmp(dirEntry->d_name, "crc") ) continue;

         fprintf(fp, "Processing File: %s\n", dirEntry->d_name) ;
         char* fileName = new char[strlen(tmpDirName) + 1 + strlen(dirEntry->d_name) + 1];
         strcpy(fileName, tmpDirName);
         strcat(fileName, "/");
         strcat(fileName, dirEntry->d_name);

         stat(fileName, &fileStat);
         if ( S_ISDIR(fileStat.st_mode) )
         {
            strcpy(fileName, tmpDirName);
            strcat(fileName, "/");
            strcat(fileName, dirEntry->d_name);

            dirProcess(fileName);
         }
         else if ( S_ISREG(fileStat.st_mode) )
         {
            fileProcess(tmpDirName, dirEntry->d_name);
         }
      }

      closedir(dir);
   }
}

/* Define this to log the update output for debug */
// #define INSTALL_LOG_FILE "/machine/log/install.log"

extern "C" void updateTrima (void);
void            updateTrima (void)
{
   taskPrioritySet(taskIdSelf(), 250);

   #ifdef INSTALL_LOG_FILE
   fp = fopen(INSTALL_LOG_FILE, "w");

   bool logFileOpenedOk = true;

   if (!fp)
   {
      logFileOpenedOk = false;
      fp              = stderr;
      fprintf(fp, "Unable to open file handle for install.log. Using stderr.\n");
   }
   #else
   fp = stderr;
   #endif // #if INSTALL_LOG_FILE

   // open the dirs
   attrib("/config", "-R");
   attrib("/config/crc", "-R");  // should be unlocked... but just in case

   dirProcess("/machine/update");
   dirProcess("/trima/softcrc/filelists");
   // lock the dir
   attrib("/config", "+R");

   fprintf(fp, "Trima software update complete.\n");
   fflush(fp);

   remove(UPDATE_PATH "/updatetrima");

   // Only close the install file handle if it's
   // actually a file (not if it's standard error).
   #ifdef INSTALL_LOG_FILE
   if (logFileOpenedOk) fclose(fp);
   #endif // #if INSTALL_LOG_FILE
}

/* FORMAT HASH 1d4b11ab1623c8d423ee3273bea905d7 */
