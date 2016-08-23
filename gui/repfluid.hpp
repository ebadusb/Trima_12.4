/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1999 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_REPFLUID

*     File name:    repfluid.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the screen that prompts for the connection
         of replacement fluid. This screen through buttons will send out
         a message stating that the operator has connected (or not) replacement
         fluid to the DRBC set. This screen is invoked as a special "alarm"
         screen and therefore must perform the handshaking required of an
         alarm screen. As an alarm screen it may be invoked as a "normal"
         replacement fluid (RF) connection prompt or as an RF prompt due to
         the request to inject a bolus of RF via the adjust screen. The
         difference lies in how the screen responds to the "cancel" button
         option. For normal mode, the pressing of that button will cause the
         invocation of the predict screen. In the bolus mode the screen will
         just dismiss itself.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.15  2008/10/03 16:14:44Z  dslausb
Move away from dynamic memory where possible
Revision 1.14  2008/09/25 21:01:17Z  dslausb
IT 8814 - Add beep
Revision 1.13  2008/09/24 22:07:51Z  dslausb
IT 8850 - Add empty alarm for saline
Revision 1.12  2008/06/13 16:47:28Z  dslausb
IT 7819 - Tweaks to alarm layout based on localization request
Revision 1.11  2008/06/05 22:41:22Z  dslausb
IT 8673 - Solution connection confirmation boxes
Revision 1.10  2007/02/12 20:13:45Z  dslausb
IT 7869 - Combined RAS and PTF options and icons
Revision 1.9  2003/11/24 19:26:53Z  rm70006
IT 6624.  Fix bug in GUI where alarm screens weren't setting globvar variable for currently displayed alarm.
Revision 1.8  2003/01/06 18:31:18Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.7  2002/11/11 07:13:26  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.6  2002/08/15 22:17:17  sb07663
compiling version for Vx
Revision 1.5  2002/05/07 14:00:01  sb07663
First cut at VxWorks compatibility
Revision 1.4  2000/03/14 16:08:36  MS10234
Changed include files to remove paths, and case sensitive file
names.  (All files will be specified in lower case)  The makefile was
updated with appropriate dependencies
Revision 1.3  2000/02/28 21:58:10  SB07663
IT4604: changes to include directives for new MKS structure
Revision 1.2  1999/05/18 00:36:54  TW09453
Add more handshaking between Repfluid and Proc: in order to
dismiss the alarm and reset GUI alarmhdlr; have proc send predict
info to Predict.
Revision 1.1  1999/05/05 21:58:20  TW09453
Initial revision
*/



#ifndef REPFLUID_HPP
#define REPFLUID_HPP



// CLASS INCLUDE


#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "timermessage.h"

// alarm include
#include "alarms.h"

// GUI <-> procedure screen include
#include "guiproc.h"

// proc includes
#include "predict_msgs.h"
#include "trimamessages.h"


// CLASS DECLARES

class Screen_REPFLUID
   : public Base_Apheresis_Window
{

private:

   Bitmap _bmpConnectGraphic,
            _bmpBackground;

   Display_Text _txtBodyNormal,
                _txtBodyBolus,
                _txtBodyNewBag,
                _txtBodyRinseback,
                _txtConnectOnlySaline,
                _txtNaclBagLabel;

   // The RF buttons

   // The RF continue bitmap button
   Button_Bitmap _btnRfContinue;

   // The RF cancel bitmap button
   Button_Bitmap _btnRfCancel;

   // The invocation type flag
   enum INVOCATION_MODE
   {
      INVOCATION_NORMAL,
      INVOCATION_BOLUS,
      INVOCATION_NEW_BAG,
      INVOCATION_RINSEBACK
   };

   // How this allocation was invoked: either BOLUS or NORMAL
   INVOCATION_MODE _rfInvocation;

   // Structure with which to respond to PROC
   Alarm_response_struct alarm_set_struct;

   // Current state of the alarm as communicated to GUI by PROC
   int alarm_state;

   // Current alarm enum as determined by the parameter value passed at allocation
   ALARM_VALUES current_alarm;

   // Screen Event Handler pointers
   AlarmStateChangeMsg _msgAlarmStateChange;

   // Screen Timer Handler pointers
   TimerMessage _muteTimeout ;

   void alarm_state_change ();

protected:
   ConfirmBoxConfirmMsg _msgConnectionConfirmed;
   ConfirmBoxCancelMsg  _msgConnectionCanceled;

   void createConfirmCallbacks ();
   void destroyConfirmCallbacks ();

   void confirmCB ();
   void cancelCB ();

   void callConfirmBox ();


public:
   // CONSTRUCTOR
   Screen_REPFLUID();


   // DESTRUCTOR
   virtual ~Screen_REPFLUID();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // PROCESS_RF_CANCEL_BUTTON
   // This method is called when the RF cancel button is pressed
   void process_RF_cancel_button ();


   // PROCESS_RF_CONTINUE_BUTTON
   // This method is called when the RF continue button is pressed
   void process_RF_continue_button ();


   virtual void confirmConnection ();


   // PROCESS_CANCEL_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the cancel button.
   virtual void process_cancel_button ();


   // ALARM_SET_STRUCT_MANAGER
   // This method sets the command codes into a structure and sends it to the Proc
   // handler.
   void alarm_set_struct_manager ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

   void startBeepingSound ();
   void muteBeepingSound ();

};

#endif

/* FORMAT HASH 953f05a0f1672833062f5f4b53cda597 */
