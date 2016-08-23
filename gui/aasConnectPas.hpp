/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      aasConnectPas.hpp
 *
 */

/*
$Log: aasConnectPas.hpp $
Revision 1.8  2008/10/03 16:12:53Z  dslausb
Move away from dynamic memory where possible
Revision 1.7  2008/08/16 19:11:00Z  dslausb
Mark's Screenshot Code
Revision 1.7  2008/08/15 20:30:37Z  mereusb
IT 8107: Minor change for screenshot scripts.
Revision 1.6  2008/06/17 15:35:44  mereusb
Updated handleTraverseMessage a little bit and changed appropriate screens to reflect changes.
Revision 1.5  2008/06/10 11:40:01  dslausb
IT 8704 - Tweak min/max styling records on PAS connect screen
Revision 1.4  2008/04/24 22:16:16Z  dslausb
IT 8588 - Add confirm box to PAS connection screen
Revision 1.3  2008/04/03 22:14:24Z  dslausb
IT 8555 - Update PAS connect graphic
Revision 1.2  2007/03/06 23:51:06Z  dslausb
- Changed MSS progress bar to a one-bar system.
- Changed the order of the connect screens
- Changed the way proc updates GUI of the MSS volumes
Revision 1.1  2006/12/04 20:53:54Z  dslausb
Initial revision
*/

// Single include define guardian
#ifndef __AAS_CONNECT_PAS_HPP__
#define __AAS_CONNECT_PAS_HPP__

#include <vxworks.h> // Needed here for #if CPU==SIMNT.

// CLASS INCLUDE
#include "aphwind.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "trimamessages.h"
#include "keypad.h"

// MESSAGE DECLARES
#include "message.h"
#include "timermessage.h"

class Screen_AASCONNECTPAS
   : public Base_Apheresis_Window
{
public:
   // CONSTRUCTOR
   Screen_AASCONNECTPAS();


   // DESTRUCTOR
   virtual ~Screen_AASCONNECTPAS();


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

   // REFRESH_RESOURCES
   // Handles the case for refreshing the barcode scan pop-up (optional feature)
   virtual void refresh_resources ();

   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();

   // virtual void process_goback_button ();

   void displayKeypadLimits (const float min, const float max, const char* format);

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

protected:

   void createConfirmCallbacks ();
   void destroyConfirmCallbacks ();
   void gotoNextScreen ();

   void callConfirmBox ();
   void confirmCB ();
   void cancelCB ();

   void callConfirmBarcodeBox ();
   void callInvalidBarcodeBox ();
   void barcodeConfirmed ();
   void barcodeCanceledOrInvalid ();

   void hdlCancelMssAll ();
   void hdlCancelMssPartial ();

   void choosePasBagVolOverride (bool overrideEnabled);

   void reminder_timer_handler ();

   // helper funcs
   void turnOffKeypadLimits ();
   void update_screen_with_keypad_data ();

  #if (CPU==SIMNT)
   virtual void handleTraverseMessage ();
  #endif // #if (CPU==SIMNT)

private:

   void setNumBags (float defBagVol);

   Bitmap        _bmpConnectPas;
   Button_Bitmap _btnPasBagVol;

   Display_Text  _txtFirstPanelConnect,
                 _txtSecondPanelFrang,
                 _txtThirdPanelUnclamp,
                 _txtBagVolLbl,
                 _txtNumBagsLbl,
                 _txtConnectPasOnly,
                 _minText,
                 _maxText;

   // Timer to drive the reminder sound
   TimerMessage _timerReminder;

   // callbacks setup here for these messages
   Message<KEYPAD_RETURN_STRUCT> _msgKeypadReturn;
   ConfirmBoxConfirmMsg          _connectionConfirmed;
   ConfirmBoxCancelMsg           _connectionCanceled;
   CancelMssMsg                  _msgCancelMssAll;
   PartialCancellationMsg        _msgCancelMssPartial;

   // Scanning for PAS bag barcode is a feature
   bool _isPasBarcodeFeatureOn;
   bool _isPasBarcodePending;

};  // END


// End of single include define guardian
#endif

/* FORMAT HASH 1b6519cff81e5dbcf0bb96877bb81a34 */
