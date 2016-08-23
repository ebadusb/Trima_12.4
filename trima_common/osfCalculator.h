////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2005 Gambro BCT, Inc.  All rights reserverd
//
// Author:       Jerry Heintz
//
// Class Name:   OSFCalculator
//
// Description:
//  Gathers run data for online QC calulations.
//  Calculates the values needed for Online QC.
//
// $Log $
////////////////////////////////////////////////////////////////////////////
#ifndef _OSFCALCULATOR_H
#define _OSFCALCULATOR_H


#include <vxWorks.h>
#include "trima_datalog.h"

// #include <iostream>
#include <strstream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

///  one of the data points containing :
struct OSF_data
{
   float val;  // val is (Ytarg/Yrbcd)
   float cv;   // current sigma of the collection of values including this one
};


class OSFCalculator
{

public:
   OSFCalculator ();
   virtual ~OSFCalculator ();

   // returns the OSF value, either default or calculated
   float osf ();

   // returns T/F if the run was added to the list
   // val is (Ytarg/Yrbcd)  ForceAdd = true will bypass the acceptance conditions
   bool addrun (const float val, bool forceAdd = false);

   // check the sigma vs limits to see if we need to alert the operator
   // that the RBC detector is out of spec.
   bool raiseRBCMaintAlert () { return haveAlert; }

   // private:

   // dump all the values out (to log)... debug only
   void dumpValues () const;

private:

   // returns the current sigma of the collection of values
   // need this for the recal alert if sigma > 0.2 (CVia)
   float sigma ()  ;

   // clean up data ( do this if  (2*sigma < |Ryc-1| ) two out of three times
   void restart (const float sigma);

   // returns number of data(runs) persisted
   int persistOSFData ();

   // returns number of data(runs) restored
   int restoreOSFData ();

   float calculateOSF ();

   int              numberValuesRetained;
   int              minValuesForCalc;
   bool             doNotPersist;

   bool             haveAlert;

   vector<OSF_data> osfData;

};

#endif

/* FORMAT HASH c829e53d19b6d7f4def3b04d9183ba8e */
