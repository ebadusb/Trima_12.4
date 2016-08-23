#ifndef __PROCADJUST_CDS
#define __PROCADJUST_CDS

#include <vxworks.h> // Needed for CPU-based compiler directives in header
#include "adjust.h"
#include "datastore.h"
#include "run_defs.h"

#ifndef WIN32
#pragma interface
#endif

class ProcAdjustBar_CDS
   : public MultWriteDataStore
{
public:

   BaseElement<float> Maximum;            // value top of the scale
   BaseElement<float> Minimum;            // value bottom of the scale
   BaseElement<float> CurrentCap;         // Position of the cap
   BaseElement<float> CurrentValue;       // actual present value
   BaseElement<bool>  Active;             // 'true' if meter is active

public:

   ProcAdjustBar_CDS(const char* name, Role role);
   virtual ~ProcAdjustBar_CDS();

private:

   ProcAdjustBar_CDS(); // Base constructor not available

};

class ProcAdjust_CDS
   :  public MultWriteDataStore
{
public:

   BaseElement<BUTTON_VISIBILITY_STATUS> Spillover;
   BaseElement<BUTTON_VISIBILITY_STATUS> AirBlock;
   BaseElement<BUTTON_VISIBILITY_STATUS> SalineBolus;
   ProcAdjustBar_CDS                     Draw;
   ProcAdjustBar_CDS                     Return;
   ProcAdjustBar_CDS                     Tingling;
   ProcAdjustBar_CDS                     Clumping;
   BaseElement<float>                    MaxQin;

public:

   ProcAdjust_CDS(Role role);
   virtual ~ProcAdjust_CDS();

private:

   ProcAdjust_CDS();  // Base Constructor not available
};

#endif

/* FORMAT HASH 4db4613d03363ca1ff791516473874f0 */
