#ifndef __SOUNDS_DAT_H__
#define __SOUNDS_DAT_H__

#include "sounds.hpp"
#include "datfile.h"
#include "alarms.h"
#include <vector>
//
// Individual tones for a particular sound type
//
struct ToneData
{
   short	dur;     // duration (msec)
   short	freq;		// frequency (Hz)
};
static const char* _CrcTokens [] = { "sequence", "delay", "END_TOKEN" };

class SoundsDat
{
public:
	struct SoundsDatStruct {
		//
		// The sounds in toneData must appear in the same order as in
		// the SOUND_CODE enumeration.
		//
		ToneData *	toneData[MAX_SOUNDS];
	
	}; /* SoundsDatStruct*/

	static ALARM_VALUES initialize(int logError = 0);
	static void initservice();
	static const SoundsDatStruct &data();
private:
	static short getNote(const char* note);
	static ToneData* initSound(const char* section, CDatFileReader &config);
	static bool checkCRC(CDatFileReader& config);
	static bool checkRange(const char* section, short dur, short octave, const char* note, int count);
	
	static unsigned long _calculatedCrc;
protected:
	SoundsDat(){};
	virtual ~SoundsDat(){};
	static SoundsDatStruct _data;
};
#endif /* __SOUNDS_DAT_H__*/
