/*
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      BasinTemp.hpp
 *
 * ABSTRACT:   This header file declares the class used for examining
 *             the basin temperature.
 *
 */


#ifndef BASINTEMP_HPP
#define BASINTEMP_HPP

// Include files
#include "monitorbase.h"
#include "basin.hpp"
#include "objdictionary.h"
#include "an_alarm.h"
#include "trimamessages.h"

// This is the maximum number of samples we will use
// in our average.  We through away all samples
// more than our BT_SAMPLE_PERIOD old.
#define BT_MAXIMUM_SAMPLES      60

// must have at least this number of samples in the period
#define BT_MINIMUM_SAMPLES      4

// must have num samples in this time period
#define BT_SAMPLE_PERIOD        3   // minutes

class BasinTempAlert
   : public MonitorBase
{
   DECLARE_OBJ(BasinTempAlert);
public:

   // Default constructor
   BasinTempAlert();
   // Destructor
   virtual ~BasinTempAlert();

   // Initialize the application
   virtual void Initialize ();

   virtual void Monitor ();

   // Called as the callback function for the
   //  Temperature data message
   virtual void newTempData ();

protected:

   typedef struct
   {
      int   bValid;
      float time;
      float temperature;
   } sample_t;

   // Cleanup all application memory
   void cleanup ();

   // inserts the reading into the array and calculates the average
   float movingAverage (float temp);

   void ClearHistory (void);

   // increment index through cirsular buffer
   int IncIndex (int index);

   // remove samples too old
   void RemoveSamples (float CurTime);

   // Calculate the average on good samples
   float CalAverage (void);



protected:

   Message<basinTemp> m_msgNewData;
   BasinTempMsg       m_msgBasinTemp;

   anAlarmMsg         m_AlmOvertempWarning;
   anAlarmMsg         m_AlmOvertempFailure;

   // store our temp sample history here
   sample_t* m_pHistory;
   int       m_nCurIndex;    // index into current reading
   int       m_bValid;       // we have 3 full valid readings here
   float     m_fAverage;     // recent average
   int       m_nNumSamples;  // a record of how many produced the average
   float     m_fCurrentTime; // current system time

   bool      m_DataSeen;
};


#endif

/* FORMAT HASH a4a9dfebf8e89feb87a3cf62c2a3eb7f */
