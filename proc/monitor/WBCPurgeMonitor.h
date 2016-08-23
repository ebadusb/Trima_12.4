#ifndef _WBCPURGEMONITOR_H_
#define _WBCPURGEMONITOR_H_

// Include files
#include <vxworks.h>
#include <vector>
#include <string>
using namespace std;

#include "aPQMonitor.h"
#include "procdata.h"
#include "rbcdat.h"
#include "states.h"
#include "timekeeper.h"

// HELPER class
/////////////////////////////////////////////////////////////
//  find 5 in a moving 20 window that violate the limits

#define MAX_VIOLATORS 5
#define WINDOW        20

class finder
{
public:
   finder();
   virtual ~finder() {}

   void finderFunc (float redValue);
   void setLimits (float boundry1, float boundry2,  bool inWindow = true);
   bool CheckVal (float redValue);
   bool flag () const { return _FLAGGED; }
   void shiftArray ();
   void dumpArray ();

private:

   float _violatorArray[WINDOW];
   int   _vileCount;
   int   _totalCount;
   bool  _FLAGGED;

   float _boundry1;
   float _boundry2;
   bool  _inWindow;

};
//////////////////////////////////////////////////////////////
class LocalStats
{
public:
   LocalStats();
   ~LocalStats();

   void   addValue (double val);
   double average ()     const;
   double high ()        const { return myHigh; }
   double low ()         const { return myLow; }
   double sum ()         const { return mySum; }
   double entryValue ()  const { return myEntryValue; }
   double exitValue ()   const { return myExitValue;  }
   int    numOfValues ()  const { return myNumber;  }
   double sigma ();

   void cleanData ();

   // private:

   int    myNumber;
   double mySum;
   double mySumSquared;
   double myEntryValue;
   double myExitValue;
   double myHigh;
   double myLow;
   double mySigma;

};
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

class WBCPurgeMonitor
   : public aPQMonitor
{
   DECLARE_OBJ(WBCPurgeMonitor);
public:
   enum WBCPurgeMonitor_states
   {
      GATHERING_DATA = 1,
      JUDGING_PURGE  = 2,
      BLACKEDOUT     = 3,
      DISABLED       = 4
   };

   // Default constructor
   WBCPurgeMonitor();

   // Destructor
   virtual ~WBCPurgeMonitor();

   // Initialize the class
   void Initialize ();

   void enable ();
   void disable ();

   //
   // Function called every status update ...
   void Monitor ();


protected:
   void                   getFilteredRed ();
   void                   judgeTheWBCPurge ();
   WBCPurgeMonitor_states setMyState ();
   void                   setAPSStatus ();
   void                   clearDataOnStateChange ();
   bool                   inWindow (float value, float boundry1, float boundry2);
   bool                   shouldWeSkipJudging (float redpls_base,
                                               float redplt_sigma,
                                               float redpls_low,
                                               float redplt_low);

   string dumpMyState (WBCPurgeMonitor_states s);

private:

   bool                   _collectingData;
   float                  _APSAlarmTime; // in minutes
   bool                   _blockJudgement;

   LocalStats             _RedPlt;
   bool                   _apsActive;

   WBCPurgeMonitor_states _myState;

   State_names            _currentSubstate;
   State_names            _lastsubstate;
   vector<string>         _recoverysToJudge;
   string                 _currentRecovery;

   finder                 _myAirFinder;
   finder                 _myDirtyPlasmaFinder;

   bool                   _productFlagged;
   float                  _vinDuringPurge;

};



#endif

/* FORMAT HASH 98765c3d0fd2afcde9c14f202ba62e50 */
