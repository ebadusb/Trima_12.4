#include "soundsdat.h"
#include "sounds.hpp"
#include "trima_datalog.h"
#include "filenames.h"
#include "filehelp.h"
#include "crcgen.h"
#include <cstdlib>

#define NOTE_A      440
#define NOTE_A_SHARP (short)((NOTE_A) * 1.059)
#define NOTE_B      (short)((NOTE_A) * 1.122)
#define NOTE_C      (short)((NOTE_A) * 1.189)
#define NOTE_C_SHARP (short)((NOTE_A) * 1.260)
#define NOTE_D      (short)((NOTE_A) * 1.335)
#define NOTE_D_SHARP (short)((NOTE_A) * 1.414)
#define NOTE_E      (short)((NOTE_A) * 1.498)
#define NOTE_F      (short)((NOTE_A) * 1.587)
#define NOTE_F_SHARP (short)((NOTE_A) * 1.682)
#define NOTE_G      (short)((NOTE_A) * 1.782)
#define NOTE_G_SHARP (short)((NOTE_A) * 1.888)

#define FREQ(octave, note)   ((1 << (octave)) * (note) / 2)

//
//  My singleton data instance
//
SoundsDat::SoundsDatStruct SoundsDat::_data = {0};

short SoundsDat::getNote (const char* note)
{
   // sharp?
   if (strlen(note) > 1 && note[1] == '^')
   {
      switch (note[0])
      {
         case 'A' :
            return NOTE_A_SHARP;
         case 'C' :
            return NOTE_C_SHARP;
         case 'D' :
            return NOTE_D_SHARP;
         case 'F' :
            return NOTE_F_SHARP;
         case 'G' :
            return NOTE_G_SHARP;
         default :
            DataLog(log_level_datfile_error) << "Invalid Note: " << note << endmsg;
            return -1;
      }
   }
   else
   {
      switch (note[0])
      {
         case 'A' :
            return NOTE_A;
         case 'B' :
            return NOTE_B;
         case 'C' :
            return NOTE_C;
         case 'D' :
            return NOTE_D;
         case 'E' :
            return NOTE_E;
         case 'F' :
            return NOTE_F;
         case 'G' :
            return NOTE_G;
         case '0' :
            return 0;
         default :
            DataLog(log_level_datfile_error) << "Invalid Note: " << note << endmsg;
            return -1;
      }
   }
}

/*
    Reads one entry from the sounds.dat config file and initializes
    the appropriate ToneData structure.
*/
ToneData* SoundsDat::initSound (const char* section, CDatFileReader& config, ALARM_VALUES& status)
{
   ToneData* td = NULL;  // return value
   status = NULL_ALARM;  // no error state

   string seq = config.GetString(section, "sequence");
   if (seq.empty())
   {
      DataLog(log_level_datfile_error)
         << "sounds.dat: missing section: " << section << " or sequence data" << endmsg;
      status = SOUNDS_DAT_VALUE_NOT_FOUND;
      return NULL;
   }

   char* c_seq = new char[seq.length() + 1];
   seq.copy(c_seq, seq.length());

   int nTriples = 1;  // count no. of triples in the sequence
   for (string::iterator iter = seq.begin(); iter != seq.end(); iter++)
      if (*iter == ',') nTriples++;

   nTriples = nTriples + 2;  // add one for delay and another for end of record
   td       = new ToneData[nTriples];

   char* triple = strtok(c_seq, ",");
   int   i      = 0;
   while (triple != NULL)
   {
      short temp_octave  = 0;
      char  temp_note[3] = {0};

      sscanf(triple, "%hd %hd %2s", &td[i].dur, &temp_octave, temp_note);
      if (!checkRange(section, td[i].dur, temp_octave, temp_note, i))
      {
         status = SOUNDS_DAT_VALUE_OUT_OF_RANGE;
         break;
      }
      td[i].freq = FREQ(temp_octave, getNote(temp_note));
      triple     = strtok(NULL, ",");
      i++;
   }

   if (NULL_ALARM == status)
   {
      // delay
      td[i].dur  = config.GetInt(section, "delay");
      td[i].freq = 0;
      if (!checkDelayRange(section, td[i].dur))
      {
         status = SOUNDS_DAT_VALUE_OUT_OF_RANGE;
      }
   }

   if (NULL_ALARM == status)
   {
      // End of record
      i++;
      td[i].dur  = 0;
      td[i].freq = 0;
   }
   else
   {
      // return NULL on failure
      delete[] td;
      td = NULL;
   }

   delete[] c_seq;
   return td;
}

ALARM_VALUES SoundsDat::initialize (bool logError)
{
   ALARM_VALUES status = NULL_ALARM;  // default

   //
   // Create the dat file reader to retrieve the configuration data.
   //
   CDatFileReader config;
   if (config.Initialize(PNAME_SOUNDSDAT, logError) != DFR_OK)
   {
      return SOUNDS_DAT_BAD_FILE;
   }
   if (!checkCRC(config))
      return SOUNDS_DAT_CRC_CHECK_FAILURE;

   int i = 0;
   while (strncmp(SoundCodeStrings[i], "MAX_SOUNDS", strlen(SoundCodeStrings[i])) != 0 )
   {
      ToneData* td = initSound(SoundCodeStrings[i], config, status);
      if (td == NULL)
         return status;
      _data.toneData[i] = td;
      i++;
   }

   //
   //  Final status check to ensure that all values were read in properly.
   //
   DFR_ERROR configStatus = config.Error();
   if (configStatus == DFR_PARSE)
   {
      status = SOUNDS_DAT_PARSE_ERROR;
   }
   return status;
}

bool SoundsDat::checkCRC (CDatFileReader& config)
{
   int           i          = 0, j = 0;
   bool          status     = true;
   unsigned long runningCrc = INITCRC_DEFAULT;
   while (strncmp(SoundCodeStrings[i], "MAX_SOUNDS", strlen(SoundCodeStrings[i])) != 0 )
   {
      while (strncmp(CrcTokens[j], "END_TOKEN", strlen(CrcTokens[j])) != 0 )
      {
         string s = config.GetString(SoundCodeStrings[i], CrcTokens[j]);
         crcgen32 (&runningCrc, (unsigned char*)s.c_str(), strlen(s.c_str()));
         j++;
      }
      i++; j = 0;
   }
   string        sCrc      = config.GetString("CRC", "crc");
   unsigned long storedCrc = strtoul(sCrc.c_str(), NULL, 0);
   if (runningCrc != storedCrc)
   {
      DataLog(log_level_datfile_error) << "Sounds dat CRC mismatch. Calculated:0x" << hex << runningCrc
                                       << " Stored:0x" << hex << storedCrc << dec << endmsg;
      status = false;
   }
   return status;
}

const SoundsDat::SoundsDatStruct& SoundsDat::data ()
{
   return _data;
}

bool SoundsDat::checkRange (const char* section, short dur, short octave, const char* note, int count)
{
   short s_note = getNote(note);
   if ((count == 0)
       && ((octave == 0) || (s_note == 0)))
   {
      DataLog(log_level_datfile_error) << "In section: " << section
                                       << " Cannot begin sequence with value 0 for Octave or Note." << endmsg;
      return false;
   }

   // dur 10 - 5000
   if (!(dur >= 10  && dur <=5000))
   {
      DataLog(log_level_datfile_error) << "In section: " << section
                                       << " duration: " << dur << " out of range (10-5000) " << endmsg;
      return false;
   }

   // Check invalid note
   if ( s_note == -1)
   {
      DataLog(log_level_datfile_error) << "In section: " << section
                                       << " Invalid Note: " << note << endmsg;
      return false;
   }

   // octave 1-3
   if (!(octave >= 0  && octave <=3))
   {
      DataLog(log_level_datfile_error) << "In section: " << section
                                       << " octave: " << octave << " out of range (0-3) " << endmsg;
      return false;
   }

   return true;
}

bool SoundsDat::checkDelayRange (const char* section, int delay)
{
   if (!(delay >= 200  && delay <=5000))
   {
      DataLog(log_level_datfile_error) << "In section: " << section
                                       << " delay: " << delay << " out of range (200-5000) " << endmsg;
      return false;
   }
   return true;
}

/* FORMAT HASH 5c3430eaed433febc124afb2a14dbeda */
