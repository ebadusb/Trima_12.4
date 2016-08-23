#include <math.h>
#include "statsContainer.h"


Stats::Stats()
   : mySum(0.0f),
     myEntryValue(0.0f),
     myExitValue(0.0f),
     myNumber(0),
     myHigh(-10000.0f),
     myLow(10000.0f),
     mySumSquared(0.0f),
     mySigma(0.0f)
{}

/////////////////////////////////////////////////////
Stats::~Stats()
{}

/////////////////////////////////////////////////////
void Stats::addValue (float val)
{

   myNumber++;
   // first and last.
   if (myNumber == 1)
      myEntryValue = val;

   myExitValue = val;

   // high and low..
   if (val > myHigh)
      myHigh = val;

   if (val < myLow)
      myLow = val;

   mySum        = mySum + val;
   mySumSquared = mySumSquared + (val * val);

}


/////////////////////////////////////////////////////
float Stats::average ()  const
{

   if (myNumber < 1)
   {
      return 0.0f;
   }

   double ans = mySum / myNumber;
   return ans;
}


/////////////////////////////////////////////////////
void Stats::cleanData ()
{

   mySum        = 0.0f;
   myEntryValue = 0.0f;
   myExitValue  = 0.0f;
   myNumber     = 0;
   myHigh       = -10000.0f;
   myLow        = 10000.0f;
   mySumSquared = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


float Stats::sigma ()
{

   if (numOfValues() < 2)
      return 0.0f;

   if (mySumSquared <= (mySum * mySum) / numOfValues())
   {
      mySigma =  0.0f;
   }
   else
   {
      mySigma = sqrt( (mySumSquared - (mySum * mySum) / numOfValues() ) / (numOfValues() - 1)  );
   }

   return mySigma;
}

/* FORMAT HASH bb67beb1f0e7e1047f257c9a439980b9 */
