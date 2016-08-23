#include <iostream>
#include "soundsdat.h"
#include "sounds.hpp"
#include "filehelp.h"
#include "crcgen.h"

using namespace std;
#define NOTE_A 		440
#define NOTE_A_SHARP (short)((NOTE_A) * 1.059)
#define NOTE_B 		(short)((NOTE_A) * 1.122) 
#define NOTE_C 		(short)((NOTE_A) * 1.189)
#define NOTE_C_SHARP (short)((NOTE_A) * 1.260)
#define NOTE_D 		(short)((NOTE_A) * 1.335)
#define NOTE_D_SHARP (short)((NOTE_A) * 1.414)
#define NOTE_E 		(short)((NOTE_A) * 1.498)
#define NOTE_F 		(short)((NOTE_A) * 1.587)
#define NOTE_F_SHARP (short)((NOTE_A) * 1.682)
#define NOTE_G 		(short)((NOTE_A) * 1.782)
#define NOTE_G_SHARP (short)((NOTE_A) * 1.888)

#define FREQ(octave,note)	((1 << (octave)) * (note)/2)

#define DataLog(x) cerr

#define endmsg endl

#define PNAME_SOUNDSDAT "sounds.dat"
//
//  My singleton data instance
//
SoundsDat::SoundsDatStruct  SoundsDat::_data;

short SoundsDat::getNote(const char* note)
{
    //sharp?
    if(strlen(note) > 1 && note[1] == '^')
    {    
        switch(note[0])
        {
            case 'A':
                return NOTE_A_SHARP; 
            case 'C':
                return NOTE_C_SHARP;
            case 'D':
                return NOTE_D_SHARP;
            case 'F':
                return NOTE_F_SHARP;
            case 'G':
                return NOTE_G_SHARP;
		default:
            return -1; 
        }
    }
    else
    {
        switch(note[0])
        {
            case 'A':
                return NOTE_A; 
            case 'B':
                return NOTE_B;
            case 'C':
                return NOTE_C;
            case 'D':
                return NOTE_D;
            case 'E':
                return NOTE_E;
            case 'F':
                return NOTE_F;
            case 'G':
                return NOTE_G;
		case '0':
			return 0;
            default:
				DataLog(log_level_sounds_error) << "Invalid Note: " << note << endmsg;
                return -1; 
        }
    }
}

/* 
	Reads one entry from the sounds.dat config file and initializes
	the appropriate ToneData structure. 
*/
ToneData* SoundsDat::initSound(const char* section, CDatFileReader& config)
{
    ToneData* td;
	string seq = config.GetString(section, "sequence");
    if(seq.empty()){ 
		DataLog(log_level_sounds_error)
			<< "sounds.dat: missing section: " << section << " or sequence data" << endmsg;
		return NULL;
	}
    char* c_seq = new char[seq.length() + 1]; memset(c_seq, '\0', seq.length()+1); 
	seq.copy(c_seq,seq.length());
	int nTriples = 1; //count no. of triples in the sequence
	for(string::iterator iter = seq.begin(); iter != seq.end(); iter++)
		if(*iter == ',') nTriples++;

	nTriples = nTriples + 2; //add one for delay and another for end of record
	td = (ToneData*)malloc(sizeof(ToneData) * nTriples);
    memset(td, '\0', sizeof(ToneData) * nTriples);
            
    short temp_octave = 0;
    char* temp_note = new char[3]; memset(temp_note, '\0', 3);
    char* triple = new char[20]; memset(triple, '\0', 20);
    char save_ptr;
    triple = strtok(c_seq, ",");
    int i = 0;
    while(triple != NULL)
    {
        sscanf(triple, "%hd %hd %2s", &td[i].dur, &temp_octave, temp_note);
		if(!checkRange(section, td[i].dur, temp_octave, temp_note, i)) {
			delete[] temp_note; delete[] triple; delete[] c_seq; 
			return NULL;
		}
        td[i].freq = FREQ(temp_octave, getNote(temp_note));
        triple = strtok(NULL, ",");
        i++;
    }

	//delay
	td[i].dur = config.GetInt(section, "delay"); td[i].freq = 0;

	//End of record
	i++; td[i].dur = 0; td[i].freq = 0; 

	delete[] temp_note; delete[] triple; delete[] c_seq; 
    return td;
}

ALARM_VALUES SoundsDat::initialize(int logError)
{
	ALARM_VALUES status = NULL_ALARM; // default

	//
	// Create the dat file reader to retrieve the configuration data.
	//
	CDatFileReader config;
	if (config.Initialize(PNAME_SOUNDSDAT, logError) != DFR_OK) {
		return SOUNDS_DAT_BAD_FILE;     
	}
	if(!checkCRC(config))
		return SOUNDS_DAT_CRC_CHECK_FAILURE;

	int i = 0;
	while(strncmp(SoundCodeStrings[i], "MAX_SOUNDS", strlen(SoundCodeStrings[i])) != 0 )
	{
		if((_data.toneData[i] = initSound(SoundCodeStrings[i], config)) == NULL)
			return SOUNDS_DAT_VALUE_NOT_FOUND; 
		ToneData* td = _data.toneData[i];
		//1. no empty sounds, range check. 
		i++;
	}
	
   //
   //  Final status check to ensure that all values were read in properly.
   //
   DFR_ERROR configStatus = config.Error();
   if (configStatus == DFR_NOTFOUND) {
      status = SOUNDS_DAT_VALUE_NOT_FOUND;
   } else if (configStatus == DFR_VALUE) {
      status = SOUNDS_DAT_VALUE_OUT_OF_RANGE;
   } else if (configStatus == DFR_PARSE) {
      status = SOUNDS_DAT_PARSE_ERROR;
   }
   return status;
}

bool SoundsDat::checkCRC(CDatFileReader& config)
{                                            
	int i=0, j=0;
	bool status = true;
	unsigned long runningCrc = INITCRC_DEFAULT;
	while(strncmp(SoundCodeStrings[i], "MAX_SOUNDS", strlen(SoundCodeStrings[i])) != 0 )
	{
		while(strncmp(_CrcTokens[j], "END_TOKEN", strlen(_CrcTokens[j])) != 0 ) 
		{
			string s = config.GetString(SoundCodeStrings[i], _CrcTokens[j]);
			crcgen32 (&runningCrc, (unsigned char *)s.c_str(), strlen(s.c_str()));
			j++;
		}
		i++; j=0;
	}
	string sCrc = config.GetString("CRC", "crc");
	unsigned long storedCrc = strtoul(sCrc.c_str(),NULL,0);
	if(runningCrc != storedCrc)
	{
		DataLog(log_level_sounds_error) << "CRC mismatch. Calculated: 0x" 
            << hex << runningCrc << " Stored: 0x"<< hex << storedCrc << endmsg;
		status = false;
	}
    else
    {
        cout << "CRCs Match: Calculated: 0x" 
            << hex << runningCrc << " Stored: 0x"<< hex << storedCrc << endmsg;
		status = true;
	}
	return status;
}

const SoundsDat::SoundsDatStruct &SoundsDat::data()
{
   return _data;
}

//Just for service mode.
static ToneData	buttonClickSound[] =
{
	{ 50, FREQ(3, NOTE_D) },
	{ 200, 0 },               	// delay between repeats
	{ 0, 0 }                   // end of list
};
void SoundsDat::initservice()
{
	//Initialization for service mode.
	_data.toneData[BUTTON_CLICK_SOUND] = buttonClickSound; 
}
bool SoundsDat::checkRange(const char* section, short dur, short octave, const char* note, int count)
{
	//User cannot specify a zero duration.
	if(dur == 0) {
		DataLog(log_level_sounds_error) << "In section: " << section
			<< " Cannot specify a zero duration " << endmsg;
		return false;
	}

	//Check invalid note 
	short s_note = getNote(note);
	if( s_note == -1){
		DataLog(log_level_sounds_error) << "In section: " << section
			<< " Invalid Note: " << note << endmsg; 
		return false;
	}

	if((count == 0) 
	   && ((octave == 0) || (s_note == 0))) {
		DataLog(log_level_sounds_error) << "In section: " << section
			<< " Cannot begin sequence with value 0 for Octave or Note." << endmsg;
		return false;
	}
	return true;
}

int main()
{
    SoundsDat::initialize( 1 );
    return 0;
}
