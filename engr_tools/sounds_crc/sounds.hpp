#ifndef _SOUNDS_INCLUDE
#define _SOUNDS_INCLUDE

// typedefs

enum SOUND_CODE
{
   ALARM_SOUND,                  // safety alarms
   ALERT_SOUND,                  // control alerts
   BUTTON_CLICK_SOUND,           // button clicks
   END_PROCEDURE_SOUND,          // end of procedure
   END_AAS_SOUND,
   RINSE_BACK_SOUND,             // start of rinse back
   SQUEEZE_SOUND,                // tell donor to squeeze
   FIRST_RETURN_SOUND,           // start of first return
   BARCODE_INVALID,              // Invalid Barcode

   MAX_SOUNDS                    // entry for sound manager [must be last in enum]
};
//These strings correspond to the section names in the sounds.dat file
const char* SoundCodeStrings[] = { 
	"ALARM",                  // safety alarms
	"ALERT",                  // control alerts
	"BUTTON_CLICK",           // button clicks
	"END_PROCEDURE",          // end of procedure
    "END_AAS",
	"RINSE_BACK",             // start of rinse back
	"SQUEEZE",                // tell donor to squeeze
	"FIRST_RETURN",           // start of first return
	"BARCODE_INVALID",        // Invalid Barcode
	"MAX_SOUNDS"

};

enum SOUND_MODE
{
   SOUND_CLEAR,                  // clear continous sound
   SOUND_CONTINUOUS,             // schedule sound continously
   SOUND_IMMEDIATE,              // send now, override any other tone
   SOUND_ONCE,                   // schedule sound once

   SOUND_MAX_MODES               // entry for sound manager [must be last in enum]
};

// SPECIFICATION:    orders used for sound request
//
// ERROR HANDLING:   none.

typedef struct
{
   SOUND_CODE     soundCode;     // sound code, see above
   SOUND_MODE     mode;          // scheduling mode
} SoundOrders;

#endif /* ifndef _SOUNDS_INCLUDE */


