/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 GAMBRO BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_SYSCASSETTE

*     File name:    syscassette.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the screen that directs the operator to
         enter a cassette ID for cassette identification.

*************************< FOCUSSED GUI DOMAIN >****************************
*
*	Revision 1.11  2008/08/14 19:09:05Z  dslausb
*	IT 8801 - Initialize message pointers to NULL
*	Revision 1.10  2008/05/09 20:43:14Z  dslausb
*	Moved cassette.h header to source file so reduce dependencies
*	Revision 1.9  2006/08/16 18:42:47Z  dslausb
*	IT 7625 - Made sure you can't get past this screen with a cassette that doesn't match your selected procedure.
*	Revision 1.8  2006/08/03 22:53:14Z  dslausb
*	Fixed cassette deletion
*	Revision 1.7  2006/07/28 23:29:56Z  dslausb
*	Redesigned cassette entry
*	Revision 1.6  2006/06/30 15:08:59Z  dslausb
*	IT 7618 - Cleaned up catalog entry
*	Revision 1.5  2006/06/27 22:53:29Z  dslausb
*	Changes before gui freeze:
*	- Revert to old tyvec picture off to the side for catalog entry
*	- Remove frangible 'bubbles' from PAS connection screen
*	Revision 1.4  2006/05/30 21:29:53Z  dslausb
*	Changed graphic on cassette entry screen.
*	Revision 1.3  2006/04/26 21:35:41Z  dslausb
*	IT 7505 - Fixed stuff that was screwing up the status bar for everybody else.
*	Revision 1.2  2006/04/21 20:41:41Z  dslausb
*	IT 7272 - Smoothed out functionality for admin and operator cassette entry.
*	Revision 1.1  2006/04/11 16:01:45Z  dslausb
*	Initial revision
*
*
***************************************************************************/



// Single include define guardian
#ifndef SYSCASSETTE_HPP
#define SYSCASSETTE_HPP

// CLASS INCLUDE
#include "aphwind.hpp"
#include "button_bitmap.h"
#include "keypad.h"

// MESSAGE DECLARES
#include "trimamessages.h"

// CLASS DECLARES

class Screen_SYSCASSETTE
   : public Base_Apheresis_Window
{

private:

   // The disposable setup placard bitmap object
   Button_Bitmap  _bitmapCassetteEntryButton;
   Bitmap         _bitmapCassetteLabel;

   guistring      _setIdListboxAllocationString;
   guistring      _setIdKeypadAllocationString;

   TEXT_STRING_ID _alert;

   void initListboxString ();
   void initCalc ();
   void displayStatusNormal (TEXT_STRING_ID msg);
   void clearStatusAttention ();
   void setStatusLine (TEXT_STRING_ID stringID);
   void clearCurrentCassette ();
   void checkCassetteMgr ();
   void setCassetteButtonFace (const guistring& cassetteRef);

public:

   void initListbox ();
   void showCalc ();

   // CONSTRUCTOR
   Screen_SYSCASSETTE();


   // DESTRUCTOR
   virtual ~Screen_SYSCASSETTE();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // REFRESH_RESOURCES
   virtual void refresh_resources ();


   // PROCESS_CANCEL_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the cancel button.
   virtual void process_cancel_button ();


   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();


   // PROCESS_GOBACK_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the goback button.
   virtual void process_goback_button ();

   // GET_KEYPAD_STRUCT_MANAGER
   // This method is deals with the information returned from the keypad.
   virtual void get_keypad_struct_manager ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

protected:
   Message<KEYPAD_RETURN_STRUCT> _msgKeypadData;

   SetListboxItemChosen          _msgListboxData;

   void listboxCB ();

};  // END of Screen_SYSCASSETTE Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH b724f8b930b43f5004ddff80608a880f */
