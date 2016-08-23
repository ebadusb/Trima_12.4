/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_REPL

*     File name:   cfg_rbc.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.8  2008/04/03 22:15:37Z  dslausb
IT 5725 - Add config setting for donor volume limit
Revision 1.7  2003/01/30 23:04:49Z  rm10919
Removed listbox references.
Revision 1.6  2002/11/11 05:23:26Z  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.5  2002/07/22 19:59:45  rm70006
VXWorks Port
Revision 1.4  2000/10/04 16:43:34Z  sb07663
Config.dat modifications for new volume removal limits
Revision 1.3  2000/08/14 18:34:34  sb07663
IT4564: consolidation of config.dat parameter tables
Revision 1.2  2000/02/28 21:54:52  SB07663
IT4612: initial checkin for single stage user configuration
Revision 1.1  1999/05/07 16:13:51  SB07663
Initial revision
Revision 1.15  1999/04/29 21:20:07  SB07663
Allow four digits for volume IT3898; RBC label positioning; storage
warning changes per IT3405
Revision 1.14  1999/04/16 15:00:38  SB07663
Temp V3.3a versions to allow head rev to build.
Revision 1.13  1999/04/07 22:15:49  SB07663
Initial version (incomplete) for V3.3a
Revision 1.12  1998/05/29 20:15:33  BF02858
- OK, to make this simple (not doing a varient), I commented out the
  DRBC code.
Revision 1.10  1998/05/29 02:12:06  BF02858
- added DRBC additions to the RBC and Proc Priority screens
Revision 1.8  1998/05/05 16:09:20  BF02858
- added REPLACEMENT FLUID buttons to the CONFIG RBC screen
Revision 1.7  1998/04/16 19:54:39  BF02858
- added rbc ratio for 3.2
- added custom rbc product for DRBC lab testing
Revision 1.6  1997/12/19 18:12:30  BF02858
- per IT 2798, added display of alert message to RBC screen when
  RBC Volume is < 150 or > 250.
Revision 1.5  1997/04/17 19:00:00  Barry_Fulkerson
- re-arranged screen to the new calculation look
*/

// Single include define guardian
#ifndef SYSCONFIGREPL_HPP
#define SYSCONFIGREPL_HPP

/** include files **/

// CLASS INCLUDE

#include "screen_config.hpp"
#include "cfg_textabove_button.h"
#include "toggleButton.hpp"


//
///////////////////////////////////////////////////////////////////
//
class Screen_CONFIG_REPL
   : public Screen_Config
{

private:

   // The display text objects
   ToggleButton             config_btn_rbc_replace_fluid;
   Config_textabove_button* config_btn_min_replace_vol;
   Config_textabove_button* config_btn_min_replace_vol_grey;
   Config_textabove_button* config_btn_rbc_fluid_percent;
   Config_textabove_button* config_btn_rbc_fluid_percent_grey;

protected:

public:

   // CONSTRUCTOR
   Screen_CONFIG_REPL();

   // DESTRUCTOR
   virtual ~Screen_CONFIG_REPL();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // CONFIG BUTTONS

   virtual void allocate_buttons ();
   virtual void process_min_replace_vol ();
   virtual void process_rbc_replace_fluid ();
   virtual void process_rbc_fluid_percent ();

   // MESSAGE NOTIFIES
   virtual void update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);

   void checkForTimeOnlyConfig ();

};

// End of single include define guardian
#endif

/* FORMAT HASH 682f2b8db752817357ed9b9d8a62a484 */
