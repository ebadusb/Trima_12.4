#ifndef STATS__CONTAINER_1111__H  //  Prevents multiple inclusions.
#define STATS__CONTAINER_1111__H
class Stats
{
public:
   Stats();
   ~Stats();

   void  addValue (float val);
   float average ()     const;
   float high ()        const { return myHigh; }
   float low ()         const { return myLow; }
   float sum ()         const { return mySum; }
   float entryValue ()  const { return myEntryValue; }
   float exitValue ()   const { return myExitValue;  }
   int   numOfValues () const { return myNumber;  }
   float sigma ();

   void cleanData ();

private:

   int   myNumber;
   float mySum;
   float mySumSquared;
   float myEntryValue;
   float myExitValue;
   float myHigh;
   float myLow;
   float mySigma;

};

#endif

/* FORMAT HASH 2d38ca83c6f796b3bc43e87e43984c52 */
