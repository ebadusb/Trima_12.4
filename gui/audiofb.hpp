/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Audio_Feedback

*     File name:    audiofb.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

        This object will accept sound parameters from the calling
        client and package them into a message that is sent to the
        Focussed sound process that then generates PC speaker sound.


*************************< FOCUSSED GUI DOMAIN >***************************/



// Single include define guardian
#ifndef AUDIOFB_HPP
#define AUDIOFB_HPP


// Sound includes

#include "sounds.hpp"


// MESSAGE DECLARES

#include "message.h"


// CLASS INCLUDES

#include "gui.h"


class Audio_Feedback
{


private:

   // The sound orders buffer
   SoundOrders gui_sound_order ;


protected:




public:



   // CONSTRUCTOR
   Audio_Feedback () ;


   // DESTRUCTOR
   virtual ~Audio_Feedback () ;


   // CREATE_FEEDBACK_BEEP
   // This method will generate a feedback sound from the PC sound process
   void create_feedback_sound (SOUND_CODE sound_type, SOUND_MODE sound_mode);


};  // END of Audio_Feedback Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 744e1f564db5c450a8f6083d94f2ae0e */
