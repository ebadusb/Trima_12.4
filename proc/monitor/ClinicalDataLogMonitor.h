#ifndef ClinicalDataLogMonitor_H
#define ClinicalDataLogMonitor_H

#include <strstream.h>
#include <math.h>


#include "statsContainer.h"

#include "datalog.h"
#include "monitorbase.h"
#include "procdata.h"
#include "timekeeper.h"

class ClinicalDataLogMonitor
   : public MonitorBase
{
   DECLARE_OBJ(ClinicalDataLogMonitor);

public:

   ClinicalDataLogMonitor();

   // Destructor
   virtual ~ClinicalDataLogMonitor();

   // Initialization
   virtual void Initialize ();

   virtual void Monitor ();


protected:
   virtual void enable ();
   virtual void disable ();

   void writeData (Stats& theStats,
                   const string& theSubstate,
                   const string& DataExplaination);

private:
   Stats CPSStats;
   Stats APSStats;


   Stats CPSStatsState;
   Stats InletPumpStatsState;
   Stats APSStatsState;


   Stats       CPSStatsCollectState;
   Stats       APSStatsCollectState;

   string      lastSubstate;
   string      lastState;

   State_names lastCollectSubstate;

   Stats       ACPumpStats;
   Stats       InletPumpStats;
   Stats       ReturnPumpStats;

};

#endif

/* FORMAT HASH 7d95261791874a7140c8f03b8b2d8d8f */
