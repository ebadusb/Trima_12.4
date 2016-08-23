/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Default_Screen_Table

*     File name:    defscrn.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class will be passed the ID of a System State and return the ID of
         the default screen for that state.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: K:/BCT_Development/vxWorks/Trima/gui/rcs/defscrn.hpp 1.6 2002/07/05 21:14:31Z sb07663 Exp jl11312 $
$Log: defscrn.hpp $
Revision 1.6  2002/07/05 21:14:31Z  sb07663
independent screen / state tables for service and normal modes
Revision 1.5  2002/05/07 13:55:49  sb07663
First cut at VxWorks compatibility
Revision 1.4  1998/07/02 17:35:59  TD10216
IT3374
Revision 1.3  1996/07/01 20:20:53  tw09453
Update the file header

*/


// Single include define guardian
#ifndef DEFSCRN_HPP
#define DEFSCRN_HPP



// System #INCLUDES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// CLASS INCLUDES

#include "gui.h"


// PROCEDURE'S INCLUDES

#include "states.h"

#define TABLE_END ((State_names) - 1000)

// the structure definition of the default screen table
struct DEFAULT_SCREEN_TABLE
{
   State_names   system_state;
   GUI_SCREEN_ID screen_name ;
};



class Default_Screen_Table
{


protected:



public:


   // CONSTRUCTOR
   Default_Screen_Table();


   // DESTRUCTOR
   virtual ~Default_Screen_Table();


   // GET_SYSTEM_STATE_DEFAULT_SCREEN
   // This method returns the ID of the default screen for a specific
   // system state.
   GUI_SCREEN_ID get_system_state_default_screen (State_names current_system_state);

   // Return current screen if acceptable for system state, otherwise
   // returns default
   GUI_SCREEN_ID get_system_state_default_screen (State_names current_system_state, GUI_SCREEN_ID current_screen);



};  // END of Default_Screen_Table Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 4e0946c6b2f500c8a8b49dbe1c677a2d */
