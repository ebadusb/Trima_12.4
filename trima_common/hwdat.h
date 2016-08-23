#ifndef hwdat_h
#define hwdat_h

#include "alarms.h"
#include "list"

//
//  This class encapsulates the hw.dat data file.  It is designed
//  as a static class, i.e., there is only one hw.dat file per task
//  and each task has a single instance of the file.
//
//  At the start of task execution, it is the task's responsibility to
//  initialize() this class.  From that point on, any object within
//  the task can access a value via a reference:
//     float tmp = HWDat::data().MaxTemperature;
//
//  alternately, after the initialization, the task could create a singleton
//  data reference for use:
//     const HWDat::HWDatStruct &HWDat = HWDat::data();
//  at which point the object would reference the values like:
//     float tmp = HWDat.MaxTemperature;
//
//
//  To add a value to the hw.dat file:
//    1: Change hw.dat to include the value
//    2: Add the equivalent value to this structure
//    3: Add an initialization statement in initialize() in HWDat.cpp
//
class HWDat
{

public:
   struct HWDatStruct
   {
      float                 MaximumBasinTemperature;
      list< unsigned long > ValidControlHWCRC;
      list< unsigned long > ValidSafetyHWCRC;
   };


public:
   //
   //
   //   When you initialize me I will read in the file
   //   and report back with an alarm value if something goes wrong.  I do
   //   not actually raise an alarm.  If everything initializes properly
   //   I return NULL_ALARM.
   //
   static ALARM_VALUES       initialize (bool logError = false);
   static const HWDatStruct& data ();

   //
   // Check the computed hardware CRC against the list of acceptable CRC values.  Returns
   // non-zero if the CRC is on the acceptable list, otherwise returns 0.  See the proc
   // state CPUIDTest for details.
   //
   static int VerifyControlHWCRC (unsigned long crc);
   static int VerifySafetyHWCRC (unsigned long crc);

   //
   // Log the list of acceptable CRC values.
   //
   static void LogValidControlHWCRC (void);
   static void LogValidSafetyHWCRC (void);

protected:
   //
   //
   //   My static singleton instance of the data
   //
   static HWDatStruct _data;

//
//
//   These methods are currently protected since we do not expect
//   anyone to actually instantiate a HWDat class.
//
protected:
   HWDat();
   virtual ~HWDat();

};
#endif

/* FORMAT HASH 118c4d17bbaf6bcd2550b335a5734193 */
