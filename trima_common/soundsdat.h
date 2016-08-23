#ifndef __SOUNDS_DAT_H__
#define __SOUNDS_DAT_H__

#include "sounds.hpp"
#include "datfile.h"
#include "alarms.h"
//
// Individual tones for a particular sound type
//
struct ToneData
{
   short dur;        // duration (msec)
   short freq;       // frequency (Hz)
};

static const char* CrcTokens [] = { "sequence", "delay", "END_TOKEN" };

class SoundsDat
{
public:
   struct SoundsDatStruct
   {
      //
      // The sounds in toneData must appear in the same order as in
      // the SOUND_CODE enumeration.
      //
      ToneData* toneData[MAX_SOUNDS];

   };  /* SoundsDatStruct*/

   static ALARM_VALUES           initialize (bool logError = false);
   static const SoundsDatStruct& data ();
private:
   static short     getNote (const char* note);
   static ToneData* initSound (const char* section, CDatFileReader& config, ALARM_VALUES& status);
   static bool      checkCRC (CDatFileReader& config);
   static bool      checkRange (const char* section, short dur, short octave, const char* note, int count);
   static bool      checkDelayRange (const char* section, int delay);
   static unsigned long _calculatedCrc;
protected:
   SoundsDat(){}
   virtual ~SoundsDat(){}
   static SoundsDatStruct _data;
};
#endif /* __SOUNDS_DAT_H__*/

/* FORMAT HASH f12e3ad9a4d4a86a5b04da6b79528acb */
