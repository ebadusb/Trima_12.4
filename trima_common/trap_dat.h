/*
 * Copyright (c) 2012 by TerumoBCT, Inc.  All rights reserved.
 *
 * File: trap_dat.h
 *
 * The purpose of this module is to pre-load all of the Trima data files before the
 * application tasks start up.  The intent of the entry point readDataFiles is to
 * run in the context of sysinit.  This will prevent any priority inversion issues
 * during start-up and remove the need for inserting task delays in the file reading
 * code.
 *
 */
#ifndef _TRAP_DAT_H_
#define _TRAP_DAT_H_


//
// Define config file tokens
//
const char COMMENT_LINE[]       = "#";
const char DAYS_OF_WEEK[]       = "DAYS_OF_WEEK";
const char BEGIN_WINDOW[]       = "BEGIN_WINDOW";
const char END_WINDOW[]         = "END_WINDOW";
const char NUMBER_OF_ATTEMPTS[] = "NUMBER_OF_ATTEMPTS";
const char NUMBER_OF_TIMEOUTS[] = "NUMBER_OF_TIMEOUTS";
const char FEATURE[]            = "FEATURE";
const char CONTROL_IP[]         = "CONTROL_IP";
const char CONTROL_PORT[]       = "CONTROL_PORT";
const char REQUEST_DELAY[]      = "REQUEST_DELAY";
const char CRC_VALUE[]          = "CRC_VALUE";


static const char TOKENS[] = " ,=\t\n\r";


class TrapDat
{
public:
   TrapDat();
   virtual ~TrapDat();

   static const int BUFFER_SIZE = 80;

   // public accessors
   int daysOfWeek () const { return _daysOfWeek; }
   int beginWindow () const { return _beginWindow; }
   int endWindow () const { return _endWindow; }
   int maxRetries () const { return _maxRetries; }
   int maxTimeouts () const { return _maxTimeouts; }
   int feature () const { return _feature; }
   int requestDelay () const { return _requestDelay; }

   const char* controlIp () const { return _controlIp; }
   short       controlPort () const { return _controlPort; }

   unsigned long calculatedCrc () const { return _calculatedCrc; }
   unsigned long overrideFileCrc () const { return _overrideFileCrc; }

   int requestDelayInMilliSeconds () const { return _requestDelayInMilliSeconds; }

   bool isOverrideFile () const { return _isOverrideFile; }

   // Reads the contents of the file
   bool read (const char* filename, bool ramCopy);

   // RAM file operations
   void BOF ();   // beginning of file.
   bool isEOF () { return _isEOF; }

   const char*  nextLine ();
   void         replaceLine (const char* s);
   unsigned int lineNumber () const { if (_lineInRamFile) return _lineInRamFile->lineCount; else return 0; }
   bool         fileHasChanged () const { return _fileHasChanged; }


// Used for accessing the file Online
protected:
   class LINE
   {
   public:
      unsigned int lineCount;
      LINE*        next;
      char         line[BUFFER_SIZE];

      LINE(){ line[0] = 0; next = NULL; lineCount = 0; }
      LINE(const char* x, int count){ strncpy(line, x, BUFFER_SIZE); lineCount = count; next = NULL; }
      virtual ~LINE() {}
   };

   LINE* _topOfFile;
   LINE* _lineInRamFile;

   bool  _fileHasChanged;
   bool  _isEOF;

protected:
   int              _daysOfWeek;   // Day(s) which connection will be attempted.
   int              _beginWindow;  // Time of day to start
   int              _endWindow;    // Time of day to end
   int              _maxRetries;   // Number of retries per session.
   int              _maxTimeouts;  // Number of timeouts per session
   int              _feature;      // Turn on or off interface.
   int              _requestDelay; // Seconds to delay between Cadence commands

   static const int IP_ADDR_SIZE = 16;
   char             _controlIp[IP_ADDR_SIZE]; // Control Server IP Address
   short            _controlPort;             // Control Server Port #

   int              _requestDelayInMilliSeconds; // milliseconds to delay between Cadence commands

   unsigned long    _calculatedCrc;
   unsigned long    _overrideFileCrc;

   bool             _isOverrideFile;

private:
   TrapDat(const TrapDat&);
   TrapDat& operator = (TrapDat& rhs);
};



class OverrideTrapDat
{
public:
   OverrideTrapDat();
   virtual ~OverrideTrapDat();

   static const char* filename () { return _filename; }
   static bool        fileOK () { return _fileOK; }
   static void        read ();

   static unsigned long calculatedCrc () { return _trapDat.calculatedCrc(); }
   static unsigned long overrideFileCrc () { return _trapDat.overrideFileCrc(); }

   static int daysOfWeek ()   { return _trapDat.daysOfWeek(); }
   static int beginWindow ()  { return _trapDat.beginWindow(); }
   static int endWindow ()    { return _trapDat.endWindow(); }
   static int maxRetries ()   { return _trapDat.maxRetries(); }
   static int maxTimeouts ()  { return _trapDat.maxTimeouts(); }
   static int feature ()      { return _trapDat.feature(); }
   static int requestDelay () { return _trapDat.requestDelay(); }

   static const char* controlIp () { return _trapDat.controlIp(); }
   static short       controlPort () { return _trapDat.controlPort(); }

   static int requestDelayInMilliSeconds () { return _trapDat.requestDelayInMilliSeconds(); }

   // Update operations
   static bool updateConfigEntry (const char* token, const char* value);
   static bool write ();   // writes the file to disk and fix the CRC file


protected:
   static TrapDat     _trapDat;
   static bool        _fileOK;
   static const char* _filename;
};



class DefaultTrapDat
{
public:
   DefaultTrapDat();
   virtual ~DefaultTrapDat();

   static const char* filename () { return _filename; }
   static bool        fileOK () { return _fileOK; }
   static void        read () { _fileOK = _trapDat.read(_filename, false); }

   static int daysOfWeek ()   { return _trapDat.daysOfWeek(); }
   static int beginWindow ()  { return _trapDat.beginWindow(); }
   static int endWindow ()    { return _trapDat.endWindow(); }
   static int maxRetries ()   { return _trapDat.maxRetries(); }
   static int maxTimeouts ()  { return _trapDat.maxTimeouts(); }
   static int feature ()      { return _trapDat.feature(); }
   static int requestDelay () { return _trapDat.requestDelay(); }

   static const char* controlIp () { return _trapDat.controlIp(); }
   static short       controlPort () { return _trapDat.controlPort(); }

   static int requestDelayInMilliSeconds () { return _trapDat.requestDelayInMilliSeconds(); }

protected:
   static TrapDat     _trapDat;
   static bool        _fileOK;
   static const char* _filename;
};



#endif

/* FORMAT HASH 49b6757269b63a4d36708648a17a76dc */
