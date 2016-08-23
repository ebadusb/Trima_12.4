/************************> FOCUSSED GUI DOMAIN <****************************
*
*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved
*
*     Author:       Dan Slauson
*
*     Class name:   Screen_MSS_END
*
*     File name:    mssEnd.hpp
*
*     Contents:     See prototype header descriptions below...
*
*     Description:
*
*	    This class represents the screen after mss addition where the
*		operator seals and disconnects product and solution bags
*		and END_MSS_SCREEN_STATE_REMOVEs the set.
*
*************************< FOCUSSED GUI DOMAIN >***************************
*
*	$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/mssEnd.hpp 1.6 2008/08/16 19:11:58Z dslausb Exp dslausb $
*	$Log: mssEnd.hpp $
*	Revision 1.6  2008/08/16 19:11:58Z  dslausb
*	Mark's Screenshot Code
*	Revision 1.6  2008/08/15 20:36:23Z  mereusb
*	IT 8107: Change for screenshot scripts.
*	Revision 1.5  2008/06/10 14:45:20  mereusb
*	IT 8107: Lots of little foundational changes to help ease writing of screenshot scripts.
*	Revision 1.4  2008/05/06 12:34:38  dslausb
*	IT 8390
*	Revision 1.3  2007/05/11 23:03:54Z  dslausb
*	IT 7970 - Fixed disconnect graphics
*	Revision 1.2  2006/09/25 19:49:22Z  dslausb
*	IT 7764 - Don't put back continue button until proc says cassette is raised.
*	Revision 1.1  2006/08/25 18:31:23Z  dslausb
*	Initial revision
*
*
****************************************************************************/

// Single include define guardian
#ifndef __MSSEND_HPP__
#define __MSSEND_HPP__

// CLASS INCLUDE
#include <vxworks.h> // for SIMNT compiler directives
#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "timermessage.h"
#include "trimamessages.h"
#include "keypad.h"

// MESSAGE DECLARES

#include "message.h"

class Screen_MSS_END
   : public Base_Apheresis_Window
{
private:

   BITMAP_ID
            _bmpIdSeal,
            _bmpIdRemove;

   Bitmap       _bmpGraphic;

   Display_Text _txtSealDetach;
   Display_Text _txtRemoveSet;

   // these are the "states" for the MSS GUI screens.
   // these are needed to vary functionality of the continue button
   // associated with the screens
   typedef enum
   {
      END_MSS_SCREEN_STATE_NULL,
      END_MSS_SCREEN_STATE_SEAL_DETACH,
      END_MSS_SCREEN_STATE_REMOVE
   } END_MSS_STATE;

   const char* printState (END_MSS_STATE state);


   END_MSS_STATE _currentEndMssState;

   MSSUnloadMsg  _msgCassetteRaised;

   void selectGraphics ();
   void selectGraphics (bool isWhiteStamp, bool isPas, bool isRas);

public:


   // CONSTRUCTOR
   Screen_MSS_END();


   // DESTRUCTOR
   virtual ~Screen_MSS_END();


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

   virtual void procSaysCassetteIsRaised ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

protected:
   // transitions to MSS Screens
   void confirmUnload ();
   void transitionGraphicsByScreenState (END_MSS_STATE newState);
   void completionAck ();

    #if (CPU==SIMNT) // For SIMNT unit testing
protected:
   virtual void handleTraverseMessage ();
    #endif // #if (CPU==SIMNT)
};  // END


// End of single include define guardian
#endif

/* FORMAT HASH 5dae684749ad6d136ab46a20026b95b8 */
