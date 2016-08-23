/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * Corrections to pump monitoring : but for platelet pump due to two magnet rotor.
 *
 * stest.mak target changed to be  saf_exec.
 *
 * TITLE:      s_dev.hpp
 *             Safety devices class definitions.
 *
 *
 * ABSTRACT:   This file contains the class definitions for safety physical devices
 *             door, cassette, valves, and switches.  Also contained are the class
 *             definitions for the safety "soft clock" logical device.
 *
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/

#ifndef S_DEV_HPP  //  Prevents multiple compilations.
#define S_DEV_HPP

// Common includes
#include "timermessage.h"

// Trima includes
#include "alarms.h"

// Local includes
#include "s_type.h"
#include "procop.hpp"


/*******************************************************************************
 *
 *  aSoftTimer_Safety CLASS DEFINITION
 *
 ******************************************************************************/

// this cycle counter fires every 100ms and, when active, counts up to a variable limit.
// it does have the advantage of being insensitive to resets in the real-time
// clock
class aSoftTimer_Safety
{
public:
   aSoftTimer_Safety(short sTimerTickLimit, const char* name);
   virtual ~aSoftTimer_Safety() {}

   void StartTimer (void);
   void StopTimer (void);
   void ResetTimer (short sTimerTickLimit);

   inline char GetTimerStatus (void) { return _cTimer; }

   virtual void notify (void);

private:
   char         _cTimer;
   TimerMessage _myTimer;
   short        _sTimerTickCount;
   short        _sTimerTickLimit;
   char         _cName[MAX_CHAR];

   aSoftTimer_Safety();                                        // not implemented
   aSoftTimer_Safety(aSoftTimer_Safety const&);                // not implemented
   aSoftTimer_Safety& operator = (aSoftTimer_Safety const&);   // not implemented

}; // END aSoftTimer_Safety class definition



/*******************************************************************************
*
*  aSwitch_Safety CLASS DEFINITION
*
******************************************************************************/
class aSwitch_Safety
   : public aSoftTimer_Safety
{
public:
   aSwitch_Safety(const char* pszSwitchName,
                  short sTimerTickLimit,
                  ALARM_VALUES alarm_name,
                  aProcOpMonitor_Safety* paProcOpMonitor);

   virtual ~aSwitch_Safety();

   void SetSwitchTimerStatus (char cTimerStatus);

private:
   aSwitch_Safety();                                     // not implemented
   aSwitch_Safety(aSwitch_Safety const&);                // not implemented
   aSwitch_Safety& operator = (aSwitch_Safety const&);   // not implemented

protected:
   ALARM_VALUES           _alarm_name;
   aProcOpMonitor_Safety* _paProcOpMonitor;          // watches Procs commands

}; // END aSwitch_Safety class defintion


/*******************************************************************************
*
*  Switch classes derived from aSwitch_Safety
*
******************************************************************************/
class aPauseSwitch_Safety
   : public aSwitch_Safety
{
public:
   aPauseSwitch_Safety(aProcOpMonitor_Safety* paProcOpMonitor);
   virtual ~aPauseSwitch_Safety() {}

private:
   aPauseSwitch_Safety();                                            // not implemented
   aPauseSwitch_Safety(aPauseSwitch_Safety const&);                  // not implemented
   aPauseSwitch_Safety& operator = (aPauseSwitch_Safety const&);     // not implemented

   bool checkOperationalStatus ();

   virtual void notify (void);
};



class aStopSwitch_Safety
   : public aSwitch_Safety
{
public:
   aStopSwitch_Safety(aProcOpMonitor_Safety* paProcOpMonitor);
   virtual ~aStopSwitch_Safety() {}

private:
   aStopSwitch_Safety();                                        // not implemented
   aStopSwitch_Safety(aStopSwitch_Safety const&);               // not implemented
   aStopSwitch_Safety& operator = (aStopSwitch_Safety const&);  // not implemented

   bool checkOperationalStatus ();

   virtual void notify (void);
};



/*******************************************************************************
 *
 *  aCentrifuge_Safety CLASS DEFINITION
 *
 ******************************************************************************/
class aCentrifuge_Safety
{
public:
   aCentrifuge_Safety();
   virtual ~aCentrifuge_Safety() {}

   void UpdateT1Status (T1STATUS cCentrifugeT1);

   void Update (float RPM, unsigned char error);

   inline float WhatIsCurrentRPM () { return _RPM; }

   inline void SetCommandRPM (float RPM) { _commandRPM = RPM; }

   bool CentrifugeT1OK (void);

   void CentrifugeAtBloodPrimeT1 (void);

private:
   T1STATUS      _cT1Status;
   float         _RPM;
   float         _commandRPM;
   unsigned char _error;
   int           _iCentrifugeErrorCount;

private:
   aCentrifuge_Safety (aCentrifuge_Safety const&);                // not implemented
   aCentrifuge_Safety& operator = (aCentrifuge_Safety const&);    // not implemented

}; // END aCentrifuge_Safety class definition



/*******************************************************************************
 *
 *  aBlinkTest_Safety CLASS DEFINITION
 *
 ******************************************************************************/

class aBlinkTest_Safety
{
public:
   aBlinkTest_Safety();
   virtual ~aBlinkTest_Safety(){}

   // alarms if driver indicates the blink test is not working
   void Update (unsigned char test);

   bool BlinkT1OK (void);

private:
   unsigned char _test;

private:
   aBlinkTest_Safety (aBlinkTest_Safety const&);                // not implemented
   aBlinkTest_Safety& operator = (aBlinkTest_Safety const&);    // not implemented

}; // END aBlinkTest_Safety class definition



/*******************************************************************************
*
*  aCRC_Safety CLASS DEFINITION
*
******************************************************************************/
class aCRC_Safety
{
public:
   aCRC_Safety();
   virtual ~aCRC_Safety() {}

   // SPECIFICATION:     Checks that all files exists.
   //                    If all files are found, compares
   //                    the CRCs in the pairs.
   //
   // ERROR HANDLING:    None.
   // RETURN:            Sets _cT1Status to a bit significant
   //                    value depending on which CRCs match
   //
   void UpdateT1Status ();

   inline T1STATUS GetT1Status (void) { return _cT1Status; }

private:
   // SPECIFICATION:     Opens a file and reads a word.
   //
   // ERROR HANDLING:    None.
   // RETURN:            Var is filled with the word read.
   //
   bool ReadCRC (const char* filename, long* var);


   // SPECIFICATION:     Checks that files exist.
   //                    Returns after two are found to
   //                    avoid overloading the internal
   //                    ethernet with File I/O traffic.
   //
   // ERROR HANDLING:    None.
   // RETURN:            NA
   //
   void FindAllFiles ();


private:
   aCRC_Safety(aCRC_Safety const&);                // not implemented
   aCRC_Safety& operator = (aCRC_Safety const&);   // not implemented

private:
   T1STATUS _cT1Status;

   long     _allFilesFound;
   long     _machCRCGold;          // gold standard machine dependent CRC
   long     _machCRC;              // generated machine dependent CRC
   long     _softCRCGold;          // gold standard application CRC
   long     _softCRC;              // generated application CRC
   long     _safeCRCGold;          // gold standard safety CRC
   long     _safeCRC;              // generated safety CRC

}; // END aCRC_Safety class definition



/*******************************************************************************
*
*  aCRC_Timer CLASS DEFINITION
*
******************************************************************************/
class aCRC_Timer
{
public:
   aCRC_Timer();
   virtual ~aCRC_Timer() {}

   void CheckCRCs ();

   bool CRCT1OK (void);

   bool CRCDisconnectOK (void);

private:
   aCRC_Timer (aCRC_Timer const&);                // not implemented
   aCRC_Timer& operator = (aCRC_Timer const&);    // not implemented

private:
   aCRC_Safety _crc;
};


#endif /* S_DEV_HPP  */

/* FORMAT HASH 9b39e203a0b332f6c216c430904352a6 */
