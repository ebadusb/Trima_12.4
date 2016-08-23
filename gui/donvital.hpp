/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_DONVITAL

*     File name:   donvital.hpp

*     Contents:    Definition of class methods and attributes

*     Description: Class header file for donvital.cpp

*     Documents:
          Requirements:    I:\....
          Select OMT:      I:\....
          Test:            I:\....

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/donvital.hpp 1.52 2008/10/03 16:14:24Z dslausb Exp dslausb $
$Log: donvital.hpp $
Revision 1.52  2008/10/03 16:14:24Z  dslausb
Move away from dynamic memory where possible
Revision 1.51  2003/07/18 20:03:45Z  jl11312
- updated power response defines
- removed references to safproc.hpp
Revision 1.50  2003/01/06 18:31:11Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.49  2002/12/02 21:14:11  jl11312
- simplified handling of button text
Revision 1.48  2002/11/11 06:08:51  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.47  2002/08/15 22:16:37  sb07663
Compiling version for Vx
Revision 1.46  2002/05/07 13:56:27  sb07663
First cut at VxWorks compatibility
Revision 1.45  2000/03/14 16:08:16  MS10234
Changed include files to remove paths, and case sensitive file
names.  (All files will be specified in lower case)  The makefile was
updated with appropriate dependencies
Revision 1.44  2000/02/28 23:21:03  SB07663
IT4604: include directive changes for safproc.hpp
Revision 1.43  1998/07/14 22:36:34  MC03015
update of donor data object messaging protocol
Revision 1.42  1998/07/02 22:05:09  MC03015
addition of donor data object data management
Revision 1.41  1998/05/21 00:17:35  MC03015
removal of external tbv massaging and graphics and status line for APII
Revision 1.40  1998/02/25 21:19:43  MC03015
addition of string id's for button face pad for translation
Revision 1.38  1997/03/17 22:30:27  MC03015
Revisions for beta 2.5F text and graphics
Revision 1.37  1997/03/15 02:27:23  MC03015
Addition of new keypad table and parsing logic
Revision 1.36  1996/12/20 09:03:28  mc03015
Modified range checking to exclude resubmission of similar values
Revision 1.35  1996/12/15 04:33:47  mc03015
APII messaging modifications
Revision 1.34  1996/12/11 23:22:37  MC03015
Addition of virtual keypad write for APII
Revision 1.33  1996/12/09 00:10:08  MC03015
Addition of beta 2.3 bitmap functionality
Revision 1.32  1996/10/29 09:45:56  MC03015
Modifications for screen init and refresh
Revision 1.31  1996/10/17 09:11:54  MC03015
Removal of common header guitemp.h
Revision 1.30  1996/10/07 11:29:09  MC03015
Changes in messaging for beta 2.3 build
Revision 1.29  1996/09/30 23:47:41  MC03015
Changes for beta 2.3
Revision 1.28  1996/09/24 22:17:58  MC03015
Changes for Beta 2.3, specifically text entry name modifications and additions.
Revision 1.21  1996/08/17 19:50:19  MC03015
architecture change to facilitate hct entered before continuing on doninfo screen
per cft review 8/16/96
Revision 1.20  1996/07/10 18:33:19  MC03015
addition of new header (log  and header info), maigic number removal, and string function review
*/

// Single include define guardian
#ifndef DONVITAL_HPP
#define DONVITAL_HPP

// GUI global element include
#include "guiglobs.hpp"

// GUI class includes
#include "aphwind.hpp"
#include "display_text.h"
#include "gui.h"
#include "scrnstat.hpp"

// string definitions and methods
#include "frmtstr.hpp"

// state definitions
#include "states.h"

// keypad includes
#include "keypad.h"

// GUI <-> procedure screen include
#include "guiproc.h"

// GUI <-> safety screen include
#include "safproc.h"

#include "trimamessages.h"

// CLASS DECLARES

class Screen_DONVITAL
   : public Base_Apheresis_Window

{

private:

   // ATTRIBUTES
   int  tbv_confirmed;                 // flag to determine previous
                                       // confirmation of tbv
   bool sex_set;                       // flag for donor sex value set
   bool height_set;                    // flag for donor height value set
   bool weight_set;                    // flag for donor weight value set

   // The donor vitals bitmap buttons
   Button_Bitmap btn_donor_gender;
   Button_Bitmap btn_donor_height;
   Button_Bitmap btn_donor_weight;

   // The placard bitmap objects
   Bitmap bitmap_tbv;

   // The display text objects
   Display_Text text_tbv;
   Display_Text text_donor_tbv;

   // METHODS

   // RESET_DYNAMIC_BUTTON_PARAMETERS
   // This method initializes screen data
   void reset_dynamic_button_parameters ();


   // SCREEN_INITIALZATION
   // This method initializes screen components
   void screen_initialization ();

   // DONOR_VITALS_SET_STRUCT_MANAGER
   // This method manages the donor vitals struct sent via messaging
   void donor_vitals_set_struct_manager (SDonorVitals& donVitalsStruct);

   // Private Cleanup method
   void cleanup ();


protected:



public:

   // METHODS

   Screen_DONVITAL();
   virtual ~Screen_DONVITAL();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // RESET_INTERNAL_SCREEN_STATE
   // This method will reset the DONVITAL internal screen state
   virtual void reset_internal_screen_state ();


   // PROCESS_GOBACK_BUTTON
   // This method is called from the virtual C callback stub and executes
   // "go back"
   virtual void process_goback_button ();


   // PROCESS_GENDER_BUTTON
   // This method is called from the C callback stub and executes "toggle gender"
   void process_gender_button ();

   // PROCESS_HEIGHT_BUTTON
   // This method is called from the C callback stub and executes "height"
   void process_height_button ();


   // PROCESS_WEIGHT_BUTTON
   // This method is called from the C callback stub and executes "weight"
   void process_weight_button ();


   // PROCESS_CONTINUE_BUTTON
   // This method is called from the virtual C callback stub and executes
   // "continue" button.
   virtual void process_continue_button ();


//////////////// MESSAGING DECLARATIONS AND SUPPORTING POINTERS ////////////////
////////////////////////////////////////////////////////////////////////////////


protected:


   Message<KEYPAD_RETURN_STRUCT> _msgKeypadData;
   ACPrimeCompletedMsg           _msgAcPrimeComplete;

   void get_keypad_struct_manager ();
   void ac_prime_completed ();
   void donor_vitals_get_struct_manager ();


}; // END of Screen_DONVITAL Class Declaration

// End of single include define guardian
#endif

/* FORMAT HASH 406d06b796b1bdc7dce9a0109ea0efc7 */
