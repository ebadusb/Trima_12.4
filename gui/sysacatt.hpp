/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_SYSACATT

*     File name:    sysacatt.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the attach AC screen - directing the operator
         to connect the blood tubing set to the AC bag. This class is derived
         from the apheresis base class.

*************************< FOCUSSED GUI DOMAIN >****************************
*/



// Single include define guardian
#ifndef SYSACATT_HPP
#define SYSACATT_HPP



// CLASS INCLUDE


#include "aphwind.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "trimamessages.h"


// CLASS DECLARES



class Screen_SYSACATT
   : public Base_Apheresis_Window
{
private:

   // The AC attach placard bitmap objects
   Bitmap
            _bmpAcAttach,
            _bmpAcLuerAttach;

   // The display text objects
   Display_Text
      _txtAcConnect,
      _txtAcSqueeze,
      _txtAcLoad,
      _txtAcBag1,
      _txtAcBag2,

      _txtAcLuerConnect,
      _txtAcLuerBreak,
      _txtAcLuerSqueeze,
      _txtAcLuerLoad,
      _txtAcLuerBag1,
      _txtAcLuerBag2,
      _txtAcLuerBag3,

      _txtNoStorSol; // "Do NOT connect storage solution".

protected:

   ConfirmBoxConfirmMsg _msgAcConnectConfirmed;
   ConfirmBoxCancelMsg  _msgAcConnectCanceled;

   void createConfirmCallbacks ();
   void destroyConfirmCallbacks ();

   void confirmCB ();
   void cancelCB ();

   void callConfirmBox ();

   void confirmAcAttached ();


public:


   // CONSTRUCTOR
   Screen_SYSACATT();


   // DESTRUCTOR
   virtual ~Screen_SYSACATT();


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

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

   // Following the pattern used in other screesn, this is so
   // screen shot can be done with the handleTraverseMessage
   void updateScreedData (bool hasAcLuer);

    #if CPU==SIMNT
protected:
   virtual void handleTraverseMessage ();
    #endif /* if CPU==SIMNT */

};  // END of Screen_SYSACATT Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH b9e50af5446536a7a924a7b9020f492d */
