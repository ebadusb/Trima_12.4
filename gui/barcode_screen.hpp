/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_BARCODE

*     File name:    barcode_screen.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the screen that is raised when the barcode
         wand (scanner) scans a barcode. It will allow the operator to review
         the scanned barcode (or concatenated barcode segments) and assign them
         to categories for future review or deletion.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/barcode_screen.hpp 1.8 2008/10/03 16:13:45Z dslausb Exp dslausb $
$Log: barcode_screen.hpp $
Revision 1.8  2008/10/03 16:13:45Z  dslausb
Move away from dynamic memory where possible
Revision 1.7  2003/11/24 19:26:45Z  rm70006
IT 6624.  Fix bug in GUI where alarm screens weren't setting globvar variable for currently displayed alarm.
Revision 1.6  2003/04/25 16:49:03Z  rm70006
IT 5835.

Port errno calls.
Revision 1.5  2003/01/06 18:30:22Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.4  2002/11/11 05:16:24  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.3  2002/08/15 22:14:20  sb07663
Compiling Vx version
Revision 1.2  2001/05/03 13:24:58  rm70006
Added barcode code from V4.2.  IT 5083.
Revision 1.1  2000/11/06 19:18:14  tw09453
Initial revision

*/



// Single include define guardian
#ifndef BARCODESCR_HPP
#define BARCODESCR_HPP



// CLASS INCLUDE


#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"

// Complex Widgets

#include "barcode_cartouche.hpp"
#include "category_button.hpp"
#include "category_button_collection.hpp"
#include "scan_structure.h"
#include "barcode_object_list.h"
#include "metadirectory.hpp"


// alarm include
#include "alarms.h"

// MESSAGE DECLARES
#include "message.h"

// GUI <-> procedure screen include
#include "guiproc.h"

// proc includes
#include "predict_msgs.h"


// CLASS DECLARES

class Screen_BARCODE
   : public Base_Apheresis_Window
{

private:
   // Screen_BARCODE ();
   Screen_BARCODE(Screen_BARCODE const&);
   Screen_BARCODE& operator = (Screen_BARCODE const&);


   // Category placard
   Bitmap category_placard;


   // The display text objects
   Display_Text cartouche_input_label;

   // The Clear cartouche_input button
   Button_Bitmap btn_clear_input_cartouche;

   // The Clear cartouche category
   Button_Bitmap btn_clear_category_cartouche;

   // The Category button Rank switching button
   Button_Bitmap btn_category_rank_switch;

   // Structure with which to respond to PROC
   Alarm_response_struct alarm_set_struct;

   // Current state of the alarm as communicated to GUI by PROC
   int alarm_state;

   // Screen Event Handler pointers
   AlarmStateChangeMsg _msgAlarmStateChange;
   void alarm_state_change ();

   // The widget directory object
   MetaDirectory barcode_metadirectory;

   // The category collection object
   Category_Button_Collection* cat_collection;

   // The input cartouche
   Barcode_Input_Cartouche* cartouche_input;

   // The category cartouche
   Barcode_Category_Cartouche* cartouche_category;

public:
   // CONSTRUCTOR
   Screen_BARCODE();


   // DESTRUCTOR
   virtual ~Screen_BARCODE();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();


   // PROCESS_CLEAR_INPUT_CARTOUCHE
   // This method is called when the btn_clear_input_cartouche button is pressed
   void process_clear_input_cartouche ();


   // PROCESS_CLEAR_CATEGORY_CARTOUCHE
   // This method is called when the btn_clear_category_cartouche button is pressed
   void process_clear_category_cartouche ();


   // PROCESS_CATEGORY_RANK_SWITCH
   // This method is called when the btn_category_rank_switch button is pressed
   void process_category_rank_switch ();


   // ALARM_SET_STRUCT_MANAGER
   // This method sets the command codes into a structure and sends it to the Proc
   // handler.
   void alarm_set_struct_manager ();

}; // END of Screen_BARCODE Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 582dd4cfb4f53a43533c856d33348a1b */
