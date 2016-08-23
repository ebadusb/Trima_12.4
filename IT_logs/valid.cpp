// valid.cpp : Defines the entry point for the console application.
//

/*
 * Copyright (c) 1999 by Cobe BCT, Inc.  All rights reserved.
 *
 *      validateIT
 *              A PC (DOS) program that is called by the MKS checkin block code
 *              to search the IT data base and return the status of the IT.  The
 *              IT(s) are input by the person checking in the module and passed
 *              to this program.  Also passed to this program is the accepted
 *              release level(s) for this specific build.  Finally, the path to
 *              the IT data base is passed to this program.
 *              The args are keyword based and are not order sensitive.  Any of the
 *              following sample inputs are allowed:
 *                      validateIT -p r:\trac32\focussed\fcs1.bts -i 2900 -r 3.2
 *                      validateIT -p r:\trac32\focussed\fcs1.bts -i 2900 2905 -r 3.2
 *                      validateIT -p r:\trac32\focussed\fcs1.bts -i 2900, 2905 -r 3.2
 *                      validateIT -p r:\trac32\focussed\fcs1.bts -i 3698 -r 3.3b
 *                      validateIT -p r:\trac32\focussed\fcs1.bts -i 3698 3689 -r 3.3b
 *                      validateIT -p r:\trac32\focussed\fcs1.bts -i 3698, 3689 -r 3.3b 3.3c
 *                      validateIT -i 3698 -r 3.3b, 3.3c -p r:\trac32\focussed\fcs1.bts
 *                      validateIT -r 3.3b, 3.3c -i 3698 -p r:\trac32\focussed\fcs1.bts
 *                      etc., etc., etc.
 *              The exit codes are described in the function 'checkRecord'.
 *
 * $Header: I:/BCT_Development/Trima/change_control/rcs/valid.cpp 1.3 2001/11/14 15:12:39 sb07663 Exp $
 * $Log: valid.cpp $
 * Revision 1.3  2001/11/14 15:12:39  sb07663
 * IT5447: change to valid.cpp for more robust header length determination.
 * The skipheader function did not include checking to skip binary data.
 * Revision 1.2  2001/10/25 16:25:14  jl11312
 * - modified for Track32 database changes
 * - updated to use binary search to speed up check-in process
 * Revision 1.1  2000/02/08 15:16:49  JR08014
 * Initial revision
 * Revision 1.1  1999/04/20 21:09:13  BS04481
 * Initial revision
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PRINT_DEBUG

enum STATUS
{
   OK             = 0,                  // OK to checkin
   NOT_DETERMINED = 1,                  // Don't know yet because we are still searching
   IT_NOT_FOUND   = 2,                  // IT is not in the Issue Tracker system
   WRONG_TARGET   = 3,                  // IT found but not for the given release
   IT_CLOSED      = 4,                  // IT found and release is right but it is already closed
   FILE_ERROR     = 5,                  // Some kind of problem getting to the Issue Tracker system
   PARAM_ERROR    = 6                   // Some kind of problem with command line parameters
};

#define MIN_ARGS 7              // we expect three keywords and at least 3 data fields
#define MAX_ARGS 32             // had to set the limit somethere
#define MAX_ITS 10              // there can be up to 10 ITs listed after the '-i' keyword
#define MAX_RELEASES 10         // there can be up to 10 releases listed after the '-r' keyword
#define MAX_STRING 256          // max length of any string (including *argv) is 256
#define MAX_DELINEATORS 4       // there can be up to 4 deliminator character defined for the lists


//      searchFile
//
//      A class object that knows about Issue Tracker data.
//              It can open and close the IT data file.
//              It can find the first valid record after the header.
//              It knows the format of the IT data file and can
//                      recognize a corrupt record and repair it.
//                      (The repair is in memory only and is not written
//                      back to the file.)
//              It can search for a record that matches a specific IT
//                      and release level.
//
class searchFile
{
public:
   searchFile(){}
   ~searchFile(){}

   STATUS open (char* filename);
   STATUS close ();
   STATUS doSearch (char* release, int IT);

private:
   FILE* fd;
   // the structure of a record in the Issue
   // tracker system
   struct it_record
   {
      char formid[20];
      char atrail[10];
      char locker[32];
      char locktime[28];
      char description[128];
      char status[254];
      char domain[24];
      char ssystem[60];
      char severity[32];
      char frequency[12];
      char submitter[32];
      char opendate[8];
      char detbuild[10];
      char testhrs[10];
      char testid[25];
      char machine[12];
      char index[25];
      char disp[12];
      char copy[128];
      char target[16];
      char resbuild[15];
      char restartplus[84];
      char logfile[253];
      char closedate[8];
      char daysopen[16];
      char reportno[16];
      char assign[155];
      char vidplus[74];
      char userdoc[3];
      char writer[24];
   };

   void   skipHeader ();
   STATUS checkRecord (char* release, int IT, int& foundRecordNo);

};


//      open
//
//      Opens the file passed in
//
//      Returns FILE_ERROR if the name is null or the file
//      doesn't open
//
STATUS searchFile::open (char* filename)
{
   if (strcmp(filename, "") != 0)
   {
      fd = fopen(filename, "rb");
      if (fd != NULL)
         return(OK);
      else
         return(FILE_ERROR);
   }
   else
      return(FILE_ERROR);
}


//      close
//
//      Closes the file pointed at by member fd
//
//      Returns FILE_ERROR if the file doesn't close
//
STATUS searchFile::close ()
{
   if (fclose(fd) == 0)
      return(OK);
   else
      return(FILE_ERROR);
}


//      doSearch
//
//      Finds the start of the valid records and kicks
//      off the search for the passed in release and IT
//
//      Returns the result of the search for that specific
//  release and IT pair.  See the function "checkRecord"
//      for information on the results of the search
//
STATUS searchFile::doSearch (char* release, int IT)
{
   STATUS status = NOT_DETERMINED;

   // set the file pointer to the first valid record
   skipHeader();

   //
   // get information on current file position and on the length of the file
   //
   long         filePos = ftell(fd);
   int          fileNo  = fileno(fd);
   struct _stat fileStat;

   _fstat(fileNo, &fileStat);
   long numRec   = (fileStat.st_size - filePos) / sizeof(it_record);

   long firstRec = 0;
   long lastRec  = numRec - 1;
   while ( firstRec <= lastRec && status == NOT_DETERMINED )
   {
      int foundRecNo;
      int seekRec = (lastRec + firstRec) / 2;
      fseek(fd, filePos + seekRec * sizeof(it_record), SEEK_SET);

      status = checkRecord(release, IT, foundRecNo);
      if ( status == NOT_DETERMINED )
      {
         if ( IT < foundRecNo )
         {
            lastRec = seekRec - 1;
         }
         else
         {
            firstRec = seekRec + 1;
         }
      }
   }

   return(status);
}


//      skipHeader
//
//      Reads file, searching for the data byte 0x0d which marks the end of the header
//      note first 8 words are binary and may contain spurious 0x0d characters
//
void searchFile::skipHeader ()
{
   int  pos = 0;
   char ch  = '\0';
   while ((pos < 32 || ch != 0x0d) && ch != EOF)
   {
      ch = fgetc(fd);
      pos++;
   }
}


//      checkRecord
//
//      Reads in a record and sends it out to be checked
//      for corruption.  Then locates the fields we care about
//      and compares them to the release and IT that
//      we are searching for.
//
//      Returns OK (0) if the IT and target from the record
//      match the IT and release level that we are looking
//      for and the IT is not closed.
//
//      Returns NOT_DETERMINED (1) if the IT in the record
//      does not match the IT we are looking for.
//
//      Returns IT_NOT_FOUND (2) if we have reached the end
//      of the Issue Tracker data without finding a matching
//      IT
//
//      Returns WRONG_TARGET (3) if the IT in the record matches
//      the one we are looking for but the target does not
//      match the release we are looking for.
//
//      Returns IT_CLOSED (4) if the IT and target in the record
//      match the IT and release we are looking for but the
//      IT has already been closed.
//
STATUS searchFile::checkRecord (char* release, int IT, int& foundRecordNo)
{
   STATUS    result = NOT_DETERMINED;
   it_record rec;

   // clear the record buffer
   memset((void*)&rec, 0x0, sizeof(rec));

   // if we reached End-of-File on the last read,
   // we won't find the record.  but if there is
   // more data, read it.
   if (!feof(fd))
      fread((void*)&rec, sizeof(rec), 1, fd);
   else
      return(IT_NOT_FOUND);

   // carve out the parts of the record that we care about
   char reportno[MAX_STRING];
   strncpy(reportno, rec.reportno, sizeof(rec.reportno));
   reportno[sizeof(rec.reportno)] = 0x0;
   int  report = atoi(reportno);

   char status[MAX_STRING];
   strncpy(status, rec.status, sizeof(rec.status));
   status[sizeof(rec.status)] = 0x0;

   char target[MAX_STRING];
   strncpy(target, rec.target, sizeof(rec.target));
   target[sizeof(rec.target)] = 0x0;

   // compare them to what we are looking for
   if (report == IT)
   {
      if (strncmp(release, target, strlen(release)) == 0)
      {
         if (strncmp(status, "Closed", sizeof("Closed") - 1) != 0)
            result = OK;
         else
            result = IT_CLOSED;
      }
      else
         result = WRONG_TARGET;
   }

#ifdef PRINT_DEBUG
   if (result != NOT_DETERMINED)
   {
      printf("IT: %d, release %s %d \n", IT, release, strlen(release));
      printf("reportno %s %d \n", reportno, report);
      status[40] = 0x0;
      printf("status %s\n", status);
      printf("target %s %d\n", target, strlen(target));
   }
#endif

   foundRecordNo = report;
   return(result);
}


//      validator
//
//      A class object that knows about parsing command lines
//              It looks for the keyword switches '-p', '-r' and '-i'
//              After finding a keyword, it scoops up all data following
//                      the keyword until if finds another keyword.  The data
//                      following the keyword can be a list of items deliminated
//                      with any of the deliminator characters (comma, space or
//                      semicolon).  Up to 10 items are accepted after '-r' (release)
//                      keyword and after the '-i' (ITs) keyword.  Only one data
//                      item is accepted after the '-p' (path to IT data) keyword.
//                      If more than the expected number of items are included in the
//                      list following the keyword, they are ignored.
//              It informs the user of the expected syntax if a parsing error occurs
//              It kicks off the search after parsing the command line.
//
class validator
{
public:
   validator(int min, int max);
   ~validator();

   STATUS getArgs (int count, char** args);
   STATUS parseFile ();
   STATUS parseRel ();
   STATUS parseIT ();
   STATUS searchdb ();
   void   displaySyntax ();

private:
   char        argString[MAX_STRING];
   char*       pString;
   char        currentKeyword[MAX_STRING];
   int         min_args;
   int         max_args;
   char        delineators[MAX_DELINEATORS];
   int         IT[MAX_ITS];
   char        release[MAX_RELEASES][MAX_STRING];
   char        dbFile[MAX_STRING];
   searchFile* trac;

   char* pickOff (char* pString, char* nextArg);
};

//      validator constructor
//
//      Sets up the deliminator characters.  There can
//      be duplicates in the deliminator list.
//
validator::validator(int min, int max)
   : min_args(min), max_args(max)
{
   strcpy(argString, "");
   strcpy(currentKeyword, "");
   delineators[0] = ' ';
   delineators[1] = ',';
   delineators[2] = ';';
   delineators[3] = ' ';
   trac           = new searchFile;
}

validator::~validator()
{
   delete trac;
   trac = NULL;
}


//      displaySyntax()
//
//      If the line can't be parsed or the keywords are missing,
//      displays the expected syntax
//
void validator::displaySyntax ()
{
   cout << "Syntax Error \n";
   cout << "\tMinimum args: " << min_args << "\n";
   cout << "\tMaximum args: " << max_args << "\n";
   cout << "\tDelineators: " << delineators[0] << " "
        << delineators[1] << " "
        << delineators[2] << " "
        << delineators[3] << "\n";
   cout << "\tExpected keywords -p 'path' -r 'release' -i 'IT'\n";
   if (strcmp(currentKeyword, "") != 0)
      cout << "\tExpected keyword and/or data not found: " << currentKeyword << "\n";
}

//      getArgs
//
//      Collects the complete argv array into one big string
//      to prepare for keyword searching
//
//      Returns OK if the number of args are within the expected
//      range
//      Returns PARAM_ERROR if args are out of range
//
STATUS validator::getArgs (int count, char** argv)
{

   if (count < min_args)
      return(PARAM_ERROR);
   if (count > max_args)
      return(PARAM_ERROR);

   for (int i = 0; i<count; i++)
   {
      strcat(argString, argv[i]);
      strcat(argString, " ");
   }
   pString = &argString[0];
   return(OK);
}


//      pickOff
//
//      Fills the char array nextArg with all characters
//      found up to the next deliminator character
//
//      Returns a point to the next non-deliminator character
//      in the string
//      Returns a NULL pointer in the following cases:
//              reached end of string
//              length of data in nextArg is MAX_STRINGS size
//
char* validator::pickOff (char* pS, char* nextArg)
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


//      parseFile
//
//      Looks for the -p keyword and gets the following
//      string under the assumption that it is the path
//      to the file to be searched.
//
//      Returns PARAM_ERROR if the keyword is not found or if
//      there is no string following the keyword
//      Otherwise, returns OK
//
STATUS validator::parseFile ()
{
   char  nextArg[256] = "finder";
   char* p            = pString;
   strcpy(currentKeyword, "-p");

   // find the keyword...
   while (strcmp(&nextArg[0], "-p") != 0)
   {
      p = pickOff(p, &nextArg[0]);
   }

   // if we are not at end-of-args, collect the listed
   // item following the keyword
   if (p==NULL)
      return(PARAM_ERROR);
   else
   {
      pickOff(p, &dbFile[0]);
      if (strcmp(dbFile, "") != 0)
         return(OK);
      else
         return(PARAM_ERROR);
   }
}


//      parseRel
//
//      Looks for the -r keyword and gets following release
//      levels.  There can be a list of up to ten release
//      levels following the -r keyword.
//
//      Returns PARAM_ERROR if the keyword is not found or
//      if there is not data following the keyword.
//      Otherwise, returns OK
//
STATUS validator::parseRel ()
{
   char   nextArg[MAX_STRING] = "finder";
   char*  p                   = pString;
   STATUS status              = PARAM_ERROR;
   strcpy(currentKeyword, "-r");

   // initialize the releases to null strings
   for (int i = 0; i<MAX_RELEASES; i++)
      release[i][0] = 0x0;

   // find the keyword...
   while (strcmp(&nextArg[0], "-r") != 0)
      p = pickOff(p, &nextArg[0]);

   // if we are not at end-of-args, collect
   // the string items listed after the keyword
   if (p!=NULL)
   {
      for (int i = 0; i<MAX_RELEASES; i++)
      {
         p = pickOff(p, release[i]);
         if (release[i][0] == '-')
         {
            strcpy(release[i], "");
            break;
         }
         else if (p==NULL)
            break;
      }

      // if we found at least one item in the list, return ok
      if (strcmp(release[0], "") != 0)
         status = OK;
      else
         status = PARAM_ERROR;
   }
   return(status);
}


//      parseIT
//
//      Looks for the -i keyword and gets the following IT
//      list.  There can be a list of up to ten ITs
//      following the -i keyword.
//
//      Returns PARAM_ERROR if the keyword is not found or
//      if there is not data following the keyword.
//      Otherwise, returns OK
//
STATUS validator::parseIT ()
{
   char   nextArg[MAX_STRING] = "finder";
   char*  p                   = pString;
   STATUS status              = PARAM_ERROR;
   strcpy(currentKeyword, "-i");

   // initialize ITs to 0
   for (int i = 0; i<MAX_ITS; i++)
      IT[i] = 0;

   // find the keyword...
   while (strcmp(&nextArg[0], "-i") != 0)
      p = pickOff(p, &nextArg[0]);

   // if we are not at end-of-args, collect the
   // integer items following the keyword
   if (p!=NULL)
   {
      for (int i = 0; i<MAX_ITS; i++)
      {
         p = pickOff(p, &nextArg[0]);
         if (nextArg[0] == '-')
         {
            IT[i] = 0;
            break;
         }
         else
            IT[i] = atoi(nextArg);
         if (p==NULL)
            break;
      }

      if (IT[0] != 0)
         status = OK;
      else
         status = PARAM_ERROR;
   }
   return(status);
}


//      searchdb
//
//      Tells the searchFile object to search for each comination of i ITs
//      and j releases.  For a search to be successful, the searcher must
//      find that each IT is associated with at least one of the j release
//      levels and that the IT is not already closed.
//
//      Returns OK (0) if the search criteria (see paragraph above) is met.
//      Returns the specific search failure if the search criteria is not met.
//  See the searchFile member function checkRecord for information on
//  he specific search failure values.
//
STATUS validator::searchdb ()
{
   STATUS status = OK;
   int    i, j;

   for (i = 0; i<MAX_ITS; i++)
   {
      if (IT[i] != 0)
      {
         for (j = 0; j<MAX_RELEASES; j++)
         {
            if (strcmp(release[j], "") != 0)
            {
               if ( (status = trac->open(dbFile)) == OK )
               {
                  status = trac->doSearch(release[j], IT[i]);
                  trac->close();
                  if (status == OK)
                     break;
               }

            }
            else
               break;
         }
      }
      else
         break;
      if (status != OK)
         break;
   }
#ifdef PRINT_DEBUG
   if (status != OK)
      cout << "No Good.  Status: " << status << "\n" ;
   else
      cout << "OK \n";
#endif
   return(status);
}



//      main
//
//      Parse the args.  Search the IT data base.  Return
//      the result to the calling program
//
int main (int argc, char* argv[])
{
   validator* finder;

   finder = new validator(MIN_ARGS, MAX_ARGS);

   // grab the entire *argv
   if (finder->getArgs(argc, argv) == PARAM_ERROR)
   {
      finder->displaySyntax();
      return(PARAM_ERROR);
   }
   // parse out the data after the -p keyword
   if (finder->parseFile() == PARAM_ERROR)
   {
      finder->displaySyntax();
      return(PARAM_ERROR);
   }
   // parse out the data after the -r keyword
   if (finder->parseRel() == PARAM_ERROR)
   {
      finder->displaySyntax();
      return(PARAM_ERROR);
   }
   // parse out the data after the -i keyword
   if (finder->parseIT() == PARAM_ERROR)
   {
      finder->displaySyntax();
      return(PARAM_ERROR);
   }

   // search for the data
   int i = (int)finder->searchdb();
   return(i);
}

/* FORMAT HASH ccc8c0cdc5683e6faee0b515e0940b65 */
