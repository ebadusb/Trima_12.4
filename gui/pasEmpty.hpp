/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Caridian BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_PAS_EMPTY

*     File name:    pasEmpty.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

    This class represents the alarm for replacing the empty bag
    of PAS during addition.  It's overridden to provide the functionality
    of showing remaining delivery volume on the screen.

*************************< FOCUSSED GUI DOMAIN >****************************
*/


// Single include define guardian
#ifndef __PAS_EMPTY_HPP__
#define __PAS_EMPTY_HPP__


// CLASS INCLUDE
#include "oipAlarmBase.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "textDataField.hpp"
#include "trimamessages.h"


// CLASS DECLARES

class Screen_PAS_EMPTY
   : public OipAlarmBase
{
public:
   // CONSTRUCTOR
   Screen_PAS_EMPTY();

   // DESTRUCTOR
   virtual ~Screen_PAS_EMPTY();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // REFRESH_RESOURCES
   // Handles the case for refreshing the barcode scan pop-up (optional feature)
   virtual void refresh_resources ();

   // PROCESS_RF_CANCEL_BUTTON
   // This method is called when the RF cancel button is pressed
   void processCancelBtn ();


   // PROCESS_RF_CONTINUE_BUTTON
   // This method is called when the RF continue button is pressed
   void processContinueBtn ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

protected:
   virtual void setUpOipInfo (const char* allocation_parameter);

   virtual void gotPasRemainingMsg ();

   void callConfirmBox ();
   void connectionConfirmed ();
   void connectionCanceled ();

   void callConfirmBarcodeBox ();
   void callInvalidBarcodeBox ();
   void barcodeConfirmed ();
   void barcodeCanceledOrInvalid ();

   void hdlCancelMssAll ();
   void hdlCancelMssPartial ();

   void createConfirmCallbacks ();
   void destroyConfirmCallbacks ();

private:

   // The text box
   Bitmap _bmpTxtCartouche;

   // The display text objects
   Display_Text _txtPasEmptyInstruction;

   // The button objects
   Button_Bitmap
      _btnContinue,
      _btnCancel;

   TextDataField _pasRemainingData;
   Proc2GuiSendRemainingPasVolDataMsg _pasVolMsg;

   // Messages associated with Confirmation pop-ups
   ConfirmBoxConfirmMsg   _connectionConfirmed;
   ConfirmBoxCancelMsg    _connectionCanceled;
   CancelMssMsg           _msgCancelMssAll;
   PartialCancellationMsg _msgCancelMssPartial;

   // Scanning for PAS bag barcode is a feature
   bool _isPasBarcodeFeatureOn;
   bool _isPasBarcodePending;
   bool _isAdditionEnded;

}; // END of Screen_PAS_EMPTY Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 7a61febcd79ff601e8d12107e9d054c5 */
