// ProcessLog.cpp : Defines the entry point for the console application.
//
/*
 * Copyright (c) 1999 by Cobe BCT, Inc.  All rights reserved.
 *
 *	ProcessLog
 *		A PC (DOS) program that manipulates the log.all file created by the
 *		MKS checkin block code.  The output file log.tab is ready to be
 *		read in by Access.  The format of log.tab is space deliminated
 *		and has a single IT entry on each line.
 *		Format:
 *			Action BaseBuild Author SourceFileName FileRevision IT
 *		Example input line:
 *			CheckIn 1.188 SB07663 Q:\home1\SDB\focus\PROC\cobe_config.dat 1.5 IT3312,3815,3859,2393
 *		Example output lines:
 *			CheckIn 1.188 SB07663 ...\PROC\cobe_config.dat 1.5 3312
 *			CheckIn 1.188 SB07663 ...\PROC\cobe_config.dat 1.5 3815
 *			CheckIn 1.188 SB07663 ...\PROC\cobe_config.dat 1.5 3859
 *			CheckIn 1.188 SB07663 ...\PROC\cobe_config.dat 1.5 2393
 *
 * $Header: I:/BCT_Development/Trima/change_control/rcs/processlog.cpp 1.4 2001/03/14 02:06:54 kw10128 Exp $
 * $Log: processlog.cpp $
 * Revision 1.4  2001/03/14 02:06:54  kw10128
 * Remove path names from the input and output files.
 * Revision 1.3  2000/03/06 22:29:44  BS04481
 * Add bin, include, lib to the directory list in leastPath
 * Revision 1.2  2000/03/03 21:01:17  BS04481
 * Change for structure to projects and structure of log.all
 * Revision 1.1  1999/04/20 21:09:12  BS04481
 * Initial revision
 */

/////////////////////
// #include "stdafx.h"
#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <stdlib.h>

// #define PRINT_DEBUG

#define MAX_STRING 256      // an input line can be up to 256 characters long
#define MAX_DELINEATORS 4   // there can be up to 4 deliminator character defined for the lists
#define MIN_ELEMENTS 8      // an input line must have 8 fields
#define MAX_ELEMENTS 17     // an input line can have 17 fields
#define NUM_DIRS 26



// #define INPUT_FILE "I:/Trima_Project/log.all"
// #define OUTPUT_FILE "I:/Trima_Project/log.tab"
// #define INPUT_FILE "//bctlak2/focused/QNX_disk_tools/log.all"
// #define OUTPUT_FILE "//bctlak2/focused/QNX_disk_tools/log.csv"
// #define INPUT_FILE "//bctquad3/home/BCT_Development/Trima/it_logs/log.all"
// #define OUTPUT_FILE "//bctquad3/home/BCT_Development/Trima/it_logs/log.csv"
#define INPUT_FILE "log.all"
#define OUTPUT_FILE "log.csv"


enum STATUS
{
   OK         = 0,
   FILE_ERROR = 1,          // some kind of problem getting to the log.all file
   FILE_EOF   = 2           // reached end-of-file
};

//	Log
//
//	A class object that knows about Log.all data.
//		It can open and close the Log.all and Log.tab files.
//		It can read in a line with one to ten ITs
//		It can write a new file Log.tab with one IT
//			per line and header data which is ready for
//			import to Access
class Log
{
public:
   Log();
   ~Log(){}

   STATUS open (char* filename, char* mode);
   STATUS close ();
   STATUS readRecord ();
   STATUS writeRecords ();
   STATUS findLeastPath ();
   STATUS findProjectPath ();
   STATUS removeQuotes ();
   void   writeHeader ();

private:
   char* pickOff (char* pS, char* next);
   FILE* fin;
   FILE* fout;
   int   _count;
   char  _elements[MAX_ELEMENTS][MAX_STRING];
   char  delineators[MAX_DELINEATORS];

};

Log::Log()
{
   delineators[0] = ' ';
   delineators[1] = ' ';
   delineators[2] = ',';
   delineators[3] = ';';
}

//	open
//
//	Opens the file passed in
//
//	Returns FILE_ERROR if the name is null or the file
//	doesn't open
//

STATUS Log::open (char* filename, char* mode)
{
   FILE* fd = NULL;
   if (strcmp(filename, "") != 0)
   {
      fd = fopen(filename, mode);
      if (fd == NULL)
         return(FILE_ERROR);
      else
      {
         if (stricmp(mode, "r") == 0)
            fin = fd;
         else if (stricmp(mode, "w") == 0)
            fout = fd;
         return(OK);
      }
   }
   else
      return(FILE_ERROR);
}



//	close
//
//	Closes the file pointed at by member fd
//
//	Returns FILE_ERROR if the file doesn't close
//
STATUS Log::close ()
{
   if (fclose(fin) == 0)
   {
      if (fclose(fout) == 0)
         return(OK);
      else
         return(FILE_ERROR);
   }
   else
      return(FILE_ERROR);
}



//	pickOff
//
//	Fills the char array nextArg with all characters
//	found up to the next deliminator character
//
//	Returns a pointer to the next non-deliminator character
//	in the string
//	Returns a NULL pointer in the following cases:
//		reached end of string
//		length of data in nextArg is MAX_STRINGS size
//
char* Log::pickOff (char* pS, char* nextArg)
{
   *nextArg = 0x0;
   char* pArg     = nextArg;
   int   truncate = 0;
   int   count    = 0;
   int   valid;

   // until we hit end of data...
   while (*pS != 0x0)
   {
      // compare the byte to each of the deliminator characters
      // and stop gathering bytes if it matches.  but if it is
      // a deliminator, read until we find a non-deliminator byte
      // compare the byte to each of the deliminator characters
      // and stop gathering bytes if it matches.  but if it is
      // a deliminator, read until we find a non-deliminator byte
      for (int i = 0; i<MAX_DELINEATORS; i++)
      {
         valid = 1;
         if (*pS == delineators[i])
         {
            truncate = 1;
            valid    = 0;
            break;
         }
      }
      // if it is not a deliminator, add the byte to our data
      if (!truncate)
      {
         *pArg = *pS;
         pArg++;
         pS++;
         if (++count > MAX_STRING)
            return(NULL);
      }
      // if we have found a non-deliminator character following
      // a deliminator, break out of the while loop because we
      // have set the pointer where we want it for the next access
      else if ( (truncate) && (valid) )
         break;
      else
         pS++;
   }
   // put a null character on our data to truncate the string
   *pArg = 0x0;

   // if we are at end-of-data, return NULL, otherwise return
   // pointer to next valid data
   if (*pS != 0x0)
      return(pS);
   else
      return(NULL);
}


//	readRecord
//
//	Reads a line from the input file and parses it into
//  elements as delineated by one of the delineator characters
//
//	Returns FILE_EOF if we reached end-of-file
//			OK is we have valid data
//
STATUS Log::readRecord ()
{
   STATUS result = FILE_ERROR;
   char   buf[MAX_STRING];
   char*  p;

   // clear the record buffer
   memset((void*)buf, 0x0, sizeof(buf));
   // set the elements null
   for (int i = 0; i<MAX_ELEMENTS; i++)
      _elements[i][0] = 0x0;

   // if we reached End-of-File on the last read,
   // we won't find the record.  but if there is
   // more data, read it.
   if (!feof(fin))
      fgets(buf, MAX_STRING, fin);
   else
      return(FILE_EOF);
   p = buf;

   // if we just read the end-fo-file, quit
   if (buf[0] == 0x0)
      return(FILE_EOF);

   // break the line into elements as delineated
   if (p!=NULL)
   {
      for (_count = 0; _count<MAX_ELEMENTS; _count++)
      {
         p = pickOff(p, _elements[_count]);
         if (p == NULL)
            break;
      }
   }


#ifdef PRINT_DEBUG
   for (int j = 0; j<_count; j++)
      printf("%s, ", _elements[j]);
   printf("\n");
#endif
   return(OK);
}

//	findProjectPath
//
//	Removes the xxxx.pj from the project path element and
//	reverses the direction of the slashes
//
//	Returns OK is all cases
//
STATUS Log::findProjectPath ()
{
   char* end;
   int   i, slash_index = 0;
   end = _elements[1];
   i   = 0;
   while (*end != 0x0)
   {
//		cout << *end;
      if (*end == '/')
      {
         *end        = '\\';
         slash_index = i;
      }
      i++;
      end++;
   }
   end  = _elements[1];
   end += slash_index;
   *end = 0x0;
   return(OK);
}

//	removeQuotes
//
//	Removes the quotes surrounding the number fields.
//
//	Always returns OK

STATUS Log::removeQuotes ()
{
   char temp[MAX_STRING];
   int  i, j, length;
   strcpy(temp, _elements[2]);
   length = sizeof(_elements[2]);
   memset((void*)_elements[2], 0x0, length);
   j      = 0;
   for (i = 0; i<length; i++)
   {
      if (temp[i] != 0x22)
         _elements[2][j++] = temp[i];
   }
   _elements[2][++j] = 0x0;

   strcpy(temp, _elements[6]);
   length = sizeof(_elements[6]);
   memset((void*)_elements[6], 0x0, length);
   j      = 0;
   for (i = 0; i<length; i++)
   {
      if (temp[i] != 0x22)
         _elements[6][j++] = temp[i];
   }
   _elements[6][++j] = 0x0;
   return(OK);
}


//	findLeastPath
//
//	Parses the source element of the line into the parts
//  of the path and attempts to delete the start of the path
//	which is above the sandbox level.  The upper part is
//	replaced with ...\.
//
//	Returns OK in all cases
//
STATUS Log::findLeastPath ()
{
   // FIX ME.  This list should contain only directories and NOT subprojects
   char* dirs[NUM_DIRS] = { "cfg_bin", "cfg_build", "cfg_etc",
                            "cfg_licenses", "cfg_password",
                            "cfg_photon", "cfg_tcpip",
                            "build_objects", "test",
                            "guitest", "guitools",
                            "my_tools", "low_level_test",
                            "alarm", "algorithm", "archiver",
                            "hal", "interface", "predict",
                            "gooddog", "src", "wgt",
                            "bin", "include", "lib",
                            "test_saf"};

   char  pathElements[MAX_ELEMENTS][MAX_STRING];
   char* p;
   int   count;
   int   found = 0;
   char  fullPath[MAX_STRING];

   // set the path elements null
   for (int i = 0; i<MAX_ELEMENTS; i++)
      pathElements[i][0] = 0x0;

   strcpy(fullPath, _elements[5]);
   // break the source line elements into path elements
   // as delineated by a slash
   int length = strlen(fullPath);
   for (int m = 0; m<length; m++)
   {
      if (fullPath[m] == '\\')
         fullPath[m] = ' ';
   }
   p = fullPath;
   if (p!=NULL)
   {
      for (count = 0; count<MAX_ELEMENTS; count++)
      {
         p = pickOff(p, pathElements[count]);
         if (p == NULL)
            break;
      }
   }

   // look for one of our directory paths
   found = 0;
   for (int j = count; j>=0; j--)
   {
      for (int k = 0; k<NUM_DIRS; k++)
      {
         if ( (stricmp(pathElements[j], dirs[k]) ) == 0)
         {
#ifdef PRINT_DEBUG
            printf("\nFound %s\n", pathElements[j]);
#endif
            found = j;
            break;
         }
      }
//		if (found != 0)
//			break;
   }

   // clear the source element (_element[5]) and preface
   // it with ...
   for (int n = 0; n<MAX_STRING; n++)
      _elements[5][n] = 0x0;
   strcpy(_elements[5], "...");

   // if one of our directories was found, fill in the
   // source name starting at the found directory
   if (found != 0)
   {
      for (int o = found; o<count + 1; o++)
      {
         strcat(_elements[5], "\\");
         strcat(_elements[5], pathElements[o]);
      }
   }
   else
   // if not found, just use the actual module name
   {
      strcat(_elements[5], "\\");
      strcat(_elements[5], pathElements[count]);
   }


#ifdef PRINT_DEBUG
   printf("%s\n", _elements[5]);
#endif

   return(OK);
}





//	writeRecords
//
//	Using the data from readRecord, write as many lines
//	as it takes to create a record for each IT in the
//  read in record
//
//	Returns FILE_ERROR if the record(s) can't be written
//			OK if the record(s) were written
//
STATUS Log::writeRecords ()
{
   int it;
   it = atoi(&_elements[7][2]);
   findProjectPath();
   findLeastPath();
//	removeQuotes();
   if (fprintf(fout, "%s, %s, %s, %s, %s, %s, %s, %4d\n"
               , _elements[0]
               , _elements[1]
               , _elements[2]
               , _elements[3]
               , _elements[4]
               , _elements[5]
               , _elements[6]
               , it) < 0)
      return(FILE_ERROR);

   for (int i = MIN_ELEMENTS; i<_count; i++)
   {
      if (fprintf(fout, "%s, %s, %s, %s, %s, %s, %s, %s\n"
                  , _elements[0]
                  , _elements[1]
                  , _elements[2]
                  , _elements[3]
                  , _elements[4]
                  , _elements[5]
                  , _elements[6]
                  , _elements[i]) < 0)
         return(FILE_ERROR);
   }
   return(OK);
}

//	writeHeader
//
//	Add the header line to the output file
//
void Log::writeHeader ()
{
   fprintf(fout, "%s, %s, %s, %s, %s, %s, %s, %s\n"
           , "Action"
           , "Project"
           , "BaseBuild"
           , "Date/Time"
           , "Author"
           , "Source"
           , "Revision"
           , "IssueTracker");
}


int main (int argc, char* argv[])
{
   class Log* single;
   single = new Log();
   STATUS     result = OK;

   result = single->open(INPUT_FILE, "r");
   if (result != OK)
   {
      cout << "Open failed on input file " << result << "\n" << endl;
      return(result);
   }

   result = single->open(OUTPUT_FILE, "w");
   if (result != OK)
   {
      cout << "Open failed on output file " << result << "\n" << endl;
      return(result);
   }

   single->writeHeader();

   while (1)
   {
      result = single->readRecord();
      if (result == OK)
      {
         result = single->writeRecords();
         if (result != OK)
         {
            cout << "Write record failed " << result << "\n" << endl;
            return(result);
         }
      }
      else if (result == FILE_EOF)
         break;
      else
      {
         cout << "Read record failed " << result << "\n" << endl;
         return(result);
      }
   }
   single->close();

   return 0;
}

/* FORMAT HASH 29914bff62498cb015934a6633ef75bf */
