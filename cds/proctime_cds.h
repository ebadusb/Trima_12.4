#ifndef __PROCTIME_CDS
#define __PROCTIME_CDS

#include "datastore.h"
#include <time.h>

#ifndef WIN32
#pragma implementation
#endif

//
//   Procedure common data store:  This class was created to hold
//   the miscellaneous pieces of data that are written throughout
//   the procedure code.
//
class ProcTime_CDS
   : public SingleWriteDataStore
{

public:
   BaseElement<struct timespec> tmAbsolute;   // absolute time
   BaseElement<struct timespec> tmProcTime;   // procedure time
   BaseElement<struct timespec> tmSysRun;     // system run time
   BaseElement<struct timespec> tmProcRun;    // procedure run time
   BaseElement<struct timespec> tmMSSRun;     // metered storage run time

// Class Methods
public:

   ProcTime_CDS (Role role);
   virtual ~ProcTime_CDS();

// Data Members
protected:



// Class Methods
public:
   static float ConvertToMinutes (const timespec& t);

   inline float AbsTime () const { return ConvertToMinutes(tmAbsolute.Get() ); }
   inline float ProcTime () const { return ConvertToMinutes(tmProcTime.Get() ); }
   inline float SysRunTime () const { return ConvertToMinutes(tmSysRun.Get() ); }
   inline float ProcRunTime () const { return ConvertToMinutes(tmProcRun.Get() ); }
   inline float MSSRunTime () const { return ConvertToMinutes(tmMSSRun.Get() ); }


private:
   ProcTime_CDS();  // Base Constructor not available
};

#endif

/* FORMAT HASH 1d58f97d6b90f88c3310f55328abe448 */
