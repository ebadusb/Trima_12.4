/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG

*     File name:   cfg_main.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/cfg_main.hpp 1.27 2008/02/20 19:37:25Z dslausb Exp $
$Log: cfg_main.hpp $
Revision 1.27  2008/02/20 19:37:25Z  dslausb
More string layout changes
Revision 1.26  2006/03/03 17:28:54Z  dslausb
First attempt at bringing over MSS stuff
Revision 1.25  2003/01/06 18:31:04Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.24  2002/11/11 05:20:14  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.23  2002/08/16 16:29:35  sb07663
removed obsolete help button
Revision 1.22  2002/07/19 16:17:50  rm70006
Revision 1.21  2002/05/07 13:55:47Z  sb07663
First cut at VxWorks compatibility
Revision 1.20  2002/02/08 20:57:07  sb07663
IT5582: ignore screen refresh requests
Revision 1.19  2000/10/04 16:43:30  sb07663
Config.dat modifications for new volume removal limits
Revision 1.18  2000/08/14 18:34:06  sb07663
IT4564: consolidation of config.dat parameter tables
Revision 1.17  2000/06/15 15:46:28  bd10648
Remove dependency on reply.hpp
Revision 1.16  2000/06/02 20:42:22  BD10648
Removed run_init update complete message
Revision 1.15  2000/05/31 16:55:31  BD10648
Removed Agent Interface Access.
Revision 1.14  2000/03/14 16:07:59  MS10234
Changed include files to remove paths, and case sensitive file
names.  (All files will be specified in lower case)  The makefile was
updated with appropriate dependencies
Revision 1.13  2000/02/28 21:54:29  SB07663
IT4612: initial checkin for single stage user configuration
Revision 1.12  1999/05/07 16:18:34  SB07663
Added new "replacement fluid" configuration screen
Revision 1.11  1998/10/22 06:17:49  BF02858
- reset the flags when leaving the screen for the agent update
Revision 1.10  1998/10/20 21:57:36  BF02858
- do not leave the main config screen until both the agent and config.dat file
have been updated
Revision 1.9  1998/10/02 23:04:55  tw09453
Moved the Proc config update message handling to cfg_main
where the logic could manage the elimination of multiple messages
and the deletion of the screen_event_handler object.
Revision 1.8  1997/05/05 17:04:47  Barry_Fulkerson
- moved password code from sys_donor screen to main config screen.
Revision 1.7  1997/03/11 16:13:25  Barry_Fulkerson
- removed all references to the old cfg_repo & cfg_serv screens. They are
  do not exist.
Revision 1.6  1997/02/26 02:33:17  Barry_Fulkerson
- changed reference from the cfg_adju file to the cfg_priority file.  Renamed
 cfg_adju.cpp/hpp to cfg_priority to better represent what it is actually doing
 also in preperation for changing the PROCEDURE PRIORITY screen to include
 more than 5 procedures.
Revision 1.5  1996/10/24 19:15:23  Barry_Fulkerson
- removed SERVICE button
Revision 1.4  1996/10/10 04:12:30  Barry_Fulkerson
- removed #include 'cfg_writ.hpp'.  Not needed
Revision 1.3  1996/10/09 23:38:25  Barry_Fulkerson
- added file write capabilities
Revision 1.2  1996/10/07 19:52:45  Barry_Fulkerson
added File Title Block comment section and MKS keywords

*/

// Single include define guardian
#ifndef SYSCONFIG_HPP
#define SYSCONFIG_HPP

/** include files **/

// CLASS INCLUDE
#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"

// Config includes
#include "cfg_kstr.hpp"
#include "keypad.h"




//
///////////////////////////////////////////////////////////////////
//
class Screen_CONFIG
   : public Base_Apheresis_Window
{

private:

   // The display text objects

   TEXT_STRING_ID text_screen_title ;

   Button_Bitmap  _cfgBtnLang,
                  _cfgBtnPlatelet,
                  _cfgBtnDonor,
                  _cfgBtnMachine,
                  _cfgBtnPlasma,
                  _cfgBtnVolLimits,
                  _cfgBtnProcedure,
                  _cfgBtnRbc,
                  _cfgBtnRepFluid,
                  _cfgBtnAutoProc,
                  _cfgBtnSwOpt,
                  _cfgBtnProcPrio;

   Message<KEYPAD_RETURN_STRUCT> _msgKeypadData;

public:

   // CONSTRUCTOR
   Screen_CONFIG();

   // DESTRUCTOR
   virtual ~Screen_CONFIG();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // PROCESS_CANCEL_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the cancel button.

   // virtual void process_cancel_button ();

   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();

   // PROCESS_GOBACK_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the goback button.

   // virtual void process_goback_button ();


   virtual void process_config_machine_button ();
   virtual void process_config_language_button ();
   virtual void process_config_plasma_button ();
   virtual void process_config_platelet_button ();
   virtual void process_config_rbc_button ();
   virtual void process_config_donor_button ();
   virtual void process_config_volume_limits_button ();
   virtual void process_config_procedure_button ();
   virtual void process_config_priority_button ();
   virtual void process_replacement_fluid_button ();
   virtual void process_config_mss_button ();
   virtual void process_config_swOpt_button ();

   // MESSAGE NOTIFIES
   void respond_to_keypad_return (void);

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

}; // END of Screen_CONFIG Class Declaration


// End of single include define guardian
#endif


//
///////////////////////////////////////////////////////////////////
// end of cfg_main.hpp file

/* FORMAT HASH 98647275fece76ebbbd056671b1af312 */
