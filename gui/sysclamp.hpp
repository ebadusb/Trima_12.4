/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_SYSCLAMP

*     File name:    sysclamp.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the screen that directs the operator to
         clamp the disposable prior to pressure testing. It is a class
         derived from the base apheresis class.


*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.15  2008/08/16 19:12:56Z  dslausb
Mark's Screenshot Code
Revision 1.15  2008/08/15 20:38:36Z  mereusb
IT 8107: Change for screenshot scripts.
Revision 1.14  2008/05/07 15:17:26  dslausb
IT 8296 - Add instruction to close sample bag prior to air removal
Revision 1.13  2007/03/07 23:46:47Z  dslausb
IT 7893 - Change cassette alarm button setup
Revision 1.12  2006/12/07 20:56:23Z  dslausb
- separated "Clamp PAS line" stuff from sysclamp (it's now pasInitSplit.)
- added the clamp crossover screen.
- added functions to guiglobs to determine which of these should be shown.
Revision 1.11  2006/06/20 16:10:35Z  dslausb
Changed PAS line to clamp instead of frangible
Revision 1.10  2006/03/07 22:13:38Z  dslausb
Revision 1.9  2006/03/03 20:59:34Z  dslausb
Revision 1.8  2003/01/06 18:32:31Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.7  2002/11/11 07:24:40  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.6  2002/08/15 22:17:26  sb07663
compiling version for Vx
Revision 1.5  2002/05/07 14:01:17  sb07663
Revision 1.4  2002/02/19 20:23:09  rm70006
IT 5596.
Add special handlers for language refresh.
Revision 1.3  1996/07/08 20:13:14Z  tw09453
Update file header

*/

// Single include define guardian
#ifndef SYSCLAMP_HPP
#define SYSCLAMP_HPP

// CLASS INCLUDE
#include "aphwind.hpp"
#include "bitmap.h"
#include "display_text.h"

class Screen_SYSCLAMP
   : public Base_Apheresis_Window
{

private:
   bool _a2donorMitigation;

   // The disposable clamp placard bitmap object
   Bitmap _bmpDispClamp;

   // The display text objects
   Display_Text _txtCloseClamps;

public:

   // CONSTRUCTOR
   Screen_SYSCLAMP();

   // DESTRUCTOR
   virtual ~Screen_SYSCLAMP();

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
   virtual void process_cancel_button ();

   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();

   // PROCESS_GOBACK_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the goback button.
   virtual void process_goback_button ();

   // This function disables standard buttons and informs proc that all necessary
   // clamping has been done.  It will likely trigger a state change that will
   // transition away from this screen.
   void sendClampsReadyMsg ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

#if (CPU==SIMNT)
protected:
   virtual void handleTraverseMessage ();
#endif // #if (CPU==SIMNT)

};  // END of Screen_SYSCLAMP Class Declaration


#endif /* SYSCLAMP_HPP */

/* FORMAT HASH 72c4c804b8ca73aa4fb43f374373f5a2 */
