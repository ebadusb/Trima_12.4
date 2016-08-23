/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_PTFMOUNT

*     File name:    ptfMount.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

*     This class represents the screen that directs the operator to
      load the press through leukoreduction filter(s) on RAS-capable
*     disposable sets.  This class is derived from the base apheresis
      class.

*************************< FOCUSSED GUI DOMAIN >****************************
*
*	Revision 1.8  2009/04/13 21:38:38Z  dslausb
*	IT 9006 - PTF load prompts
*	Revision 1.7  2009/01/05 22:51:16Z  dslausb
*	IT 8990 - Move RBC initial split screen to after air evac, but before disposables test.
*	Revision 1.6  2008/08/16 19:12:10Z  dslausb
*	Mark's Screenshot Code
*	Revision 1.6  2008/08/15 20:36:47Z  mereusb
*	IT 8107: Change for screenshot scripts.
*	Revision 1.5  2008/06/17 15:36:24  mereusb
*	Updated handleTraverseMessage a little bit and changed appropriate screens to reflect changes.
*	Revision 1.4  2008/06/13 13:06:46  dslausb
*	More tweaks for new alarm layout
*	Revision 1.3  2008/05/29 19:59:49Z  dslausb
*	ITs 8507, 8663, and 8665 - Fiddle with set load screens
*	- Fix set load stat line message junk
*	- Add slide clamp instruction for non-RAS drbc sets
*	Revision 1.2  2008/04/17 15:33:04Z  dslausb
*	IT 8576 - text not updating correctly when styling record changes
*	Revision 1.1  2007/11/29 23:48:14Z  dslausb
*	Initial revision
*
****************************************************************************/



// Single include define guardian
#ifndef __PTF_MOUNT_HPP__
#define __PTF_MOUNT_HPP__



// CLASS INCLUDE

#include <vxworks.h> // Needed for SIMNT compiler definition.
#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"

// CLASS DECLARES



class Screen_PTFMOUNT
   : public Base_Apheresis_Window
{
private:

   // The disposable setup placard bitmap object
   Bitmap
            _bmpMountOnePtf,
            _bmpMountTwoPtf;

   // The display text objects

   Display_Text
   // For Mount PTF State
      _txtMountOnePtf1,
      _txtMountOnePtf2,
   // _txtMountOnePtf3,

      _txtMountTwoPtf1,
      _txtMountTwoPtf2,
      _txtMountTwoPtf3,

      _txtDontConnectAc;

public:

   // CONSTRUCTOR
   Screen_PTFMOUNT();


   // DESTRUCTOR
   virtual ~Screen_PTFMOUNT();


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

   // chooseTextAndGraphic
   // This method takes decides what version of this screen to show.
   // The variables are passed to make it easy to unit test.
   void chooseTextAndGraphic (bool setIsDrbc, bool showDontConnectAc);

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

#if CPU==SIMNT
protected:
   virtual void handleTraverseMessage ();
#endif // #if CPU==SIMNT
};  // END of Screen_PTFMOUNT Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 0d43cf768b28e10dae3eea924b5214a9 */
