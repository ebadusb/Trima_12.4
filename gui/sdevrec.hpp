/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Sysdonor_Event_Recorder

*     File name:    sdevrec.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class receives notification of all the events and screen
         display states that contribute to the overall state of SYSDONOR.
         It will post these events to a local copy of the Sysdonor_state
         and if it detects that the posting of the event causes
         a change in SYSDONOR state, it will call the State_Manager's method,
         check_for_state_change, to attempt the execution of any actions
         associated with this state change. It also has an unconditional call
         for state check which is used when the screen is allocated.

*************************< TRIMA GUI DOMAIN >****************************

$Header: I:/BCT_Development/vxWorks/Trima/gui/rcs/sdevrec.hpp 1.2 2002/08/15 22:17:22Z sb07663 Exp rm70006 $
$Log: sdevrec.hpp $
Revision 1.2  2002/08/15 22:17:22Z  sb07663
compiling version for Vx
Revision 1.1  1998/08/08 00:04:49  tw09453
Initial revision

*/



// Single include define guardian
#ifndef SDEVREC_HPP
#define SDEVREC_HPP

#include "sdstate.hpp"   // include the Sysdonor state header
#include "statmgr.hpp"   // include the state manager header
#include "guistate.hpp"  // include the gui state header
#include "sysdninc.h"    // include sysdonor includes

class Sysdonor_Event_Recorder
   : public State_Manager
{
private:
   Sysdonor_State current_state; // the state collection object

public:
   // CONSTRUCTOR
   Sysdonor_Event_Recorder();

   // DESTRUCTOR
   virtual ~Sysdonor_Event_Recorder();


   // POST_EVENT
   // This method posts the various state elements that contribute
   // to the overall state of SYSDONOR

   // >>> the following signature is for true/false type state elements
   void post_event (SD_EVENT_TYPE ev_type, SD_T_F_D true_false_type);

   // >>> the following signature is for screen display
   void post_event (SD_EVENT_TYPE ev_type, SYSDONOR_STATE display_state);


   // POST_EVENT_NOCHECK
   // This method posts the various state elements that contribute
   // to the overall state of SYSDONOR. It does not, however, call state
   // change detection after it posts the new current state component.

   // >>> the following signature is for true/false type state elements
   void post_event_nocheck (SD_EVENT_TYPE ev_type, SD_T_F_D true_false_type);

   // >>> the following signature is for screen display
   void post_event_nocheck (SD_EVENT_TYPE ev_type, SYSDONOR_STATE display_state);


   // UNCONDITIONAL_STATE_CHECK
   // This method causes the current state to be unconditionally searched for
   // in the state collection. This would be used when the screen is allocated
   // and needs to re-establish its display environment based on the screen state.
   void unconditional_state_check ();

};  // END of Sysdonor_Event_Recorder Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 66b98a5f91953484084794762b0bb8f0 */
