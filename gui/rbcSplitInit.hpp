/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 CaridianBCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_RBCSPLITINIT

*     File name:    rbcSplitInit.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

*     This class represents the screen that directs the operator to
      close the line to one of the RBC bags after air removal.

*************************< FOCUSSED GUI DOMAIN >****************************
*
*	$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/rbcSplitInit.hpp 1.1 2009/01/05 19:59:48Z dslausb Exp dslausb $
*	$Log: rbcSplitInit.hpp $
*	Revision 1.1  2009/01/05 19:59:48Z  dslausb
*	Initial revision
*
****************************************************************************/



// Single include define guardian
#ifndef __RBC_SPLIT_INIT__
#define __RBC_SPLIT_INIT__



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



class Screen_RBCSPLITINIT
   : public Base_Apheresis_Window
{
private:

   // The disposable setup placard bitmap object
   Bitmap _bmpRbcSplitInit;

   // The display text objects

   Display_Text _txtRbcSplitInit;


   bool _a2donorMitigation;

public:

   // CONSTRUCTOR
   Screen_RBCSPLITINIT();


   // DESTRUCTOR
   virtual ~Screen_RBCSPLITINIT();


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

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

};  // END of Screen_RBCSPLITINIT Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 38b9c62d2ee7cda883cedf08874c8b8c */
