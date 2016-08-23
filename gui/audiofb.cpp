/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Audio_Feedback

*     File name:   audiofb.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#include <vxworks.h>


// BASE CLASS INCLUDES
#include "audiofb.hpp"
#include "trima_datalog.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Audio_Feedback::Audio_Feedback()
{}    // END of Audio_Feedback CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Audio_Feedback::~Audio_Feedback()
{}    // END of Audio_Feedback DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// CREATE_FEEDBACK_SOUND

void Audio_Feedback::create_feedback_sound (SOUND_CODE sound_type, SOUND_MODE sound_mode)
{
   // Do nothing if invalid parameter is passed in
   if (sound_mode >= SOUND_MAX_MODES)
   {
      DataLog(log_level_gui_error) << "Invalid Sound Mode "  << endmsg;
      return;
   }

   Message<SoundOrders> soundmsg(MessageBase::SEND_LOCAL);

   // Assign the sound values to the sound structure
   gui_sound_order.soundCode = sound_type ;
   gui_sound_order.mode      = sound_mode ;


   // Send the message
   soundmsg.send(gui_sound_order);

   DataLog(log_level_gui_info) << "Sound is played, sound type: " << SoundCodeStrings[sound_type] <<
      " sound mode: " << SoundModeStrings[sound_mode] << endmsg;
}     // END of Audio_Feedback CREATE_FEEDBACK_SOUND

/* FORMAT HASH 672a83635a2a05114b4ff28d1c353c4e */
