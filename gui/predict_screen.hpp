/************************> FOCUSSED GUI DOMAIN <****************************

 *     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

 *     Author:      Michael Cobb

 *     Class name:  Screen_PREDICT

 *     File name:   predict.hpp

 *************************< FOCUSSED GUI DOMAIN >****************************/

// Single include define guardian
#ifndef PREDICT_SCREEN_HPP
#define PREDICT_SCREEN_HPP

// GUI global element include
#include "guiglobs.hpp"

// GUI class includes
#include "aphwind.hpp"
#include "display_text.h"
#include "gui.h"
#include "scrnstat.hpp"

// state definitions
#include "states.h"

// message include
#include "message.h"

// keypad includes
#include "keypad.h"

#include "alarms.h"
#include "an_alarm.h"
#include "predict_msgs.h"
#include "procedure_status.h"
#include "trimamessages.h"
#include "timermessage.h"

// GUI <-> safety screen include
#include "safproc.h"

// sound include
#include "sounds.hpp"


// alarms
#include "clearablealarm.h"

#include "predict_screen_display.hpp"

class Screen_PREDICT
   : public Base_Apheresis_Window
{

private: // Data

   enum AMAP_STATE {SHOW_ONE, SHOW_ALL};
   AMAP_STATE       _amapState;

   predict_buttons  _buttons;
   predict_keypad   _keypad;
   predict_units    _units;

   PlateletText     _plt_prev_text;
   PlateletText     _plt_curr_text;
   PlateletText     _plt_next_text;

   display_plt_vol  _pltVol_prev_text;
   display_plt_vol  _pltVol_curr_text;
   display_plt_vol  _pltVol_next_text;

   display_plasma   _plas_prev_text;
   display_plasma   _plas_curr_text;
   display_plasma   _plas_next_text;

   display_rbc      _rbc_prev_text;
   display_rbc      _rbc_curr_text;
   display_rbc      _rbc_next_text;

   display_time     _time_prev_text;
   display_time     _time_curr_text;
   display_time     _time_next_text;

   display_flowrate _flowrate_prev_text;
   display_flowrate _flowrate_curr_text;
   display_flowrate _flowrate_next_text;

   display_rbc      _rbc_prev_4AMAP_text;
   display_rbc      _rbc_curr_4AMAP_text;
   display_rbc      _rbc_next_4AMAP_text;

   display_value    _compPlsStatement;

   // PREDICT DATA
   const ProcedureStatus::ProcSnapshot& _procData;

   // pump pause alarm
   ClearableAlarm _predictPause;

   // invocation structure
   PREDICT_INVOKE_TYPE _invocationType;

   // startup synchronization conditions
   bool _initial_repredict; // true when we're trying to come up, but have to
                            // wait for PredictManager to complete a transaction
                            // before starting our initial repredict.

   // shutdown synch conditions
   bool _goback;            // true when the 'continue' button has been
                            // pressed.  Affects whether or not we restart
                            // the pumps.


   // MESSAGE POINTERS ------------------------------------
   TimerMessage                  _msgTimer;
   Message<KEYPAD_RETURN_STRUCT> _msgPtrKeypadData;

   ConfirmBoxConfirmMsg          _procedureConfirmed;
   ConfirmBoxCancelMsg           _procedureCanceled;

   // BOOLEAN OBJECTS -----------------------------
   bool _allowAdjustFlowRate,
        _amapPlatelets,
        _currentlyConfirming, // Are we currently showing the confirm box?
        _nextPls_AllComp,
        _currPls_AllComp,
        _prevPls_AllComp ;



   int  _currentProcNumber;
   int  _selectedProcNumber;
   int  _donor_time;            // Donor time, set by donor time button

   bool _no_change;             // set to true when transitioning back to this screen after a keypad
                                // that didn't change anything.

   // FLOAT OBJECTS -----------------------------

   float _floatLastFlowAdj,   // Flow adj delta
         _floatCompPlsPASVolCurr;

   // Procedure display

   procedure_text _procedure_prev;
   procedure_text _procedure_curr;
   procedure_text _procedure_next;

   // BITMAP OBJECTS -----------------------------

   Bitmap _bmpSelectionBox, // The background placard

   // The disabled previous and next buttons
          _bmpDisabledPrev,     // NOTE: these two should go into buttons next.
          _bmpDisabledNext,

   // The replacement fluid icons
          _bmpFluidPrev,
          _bmpFluidCurr,
          _bmpFluidNext,

          _bmpFluidPrev_AMAP,
          _bmpFluidCurr_AMAP,
          _bmpFluidNext_AMAP,


   // The Platelet Additive Solution Icons
          _bmpPasPrev,
          _bmpPasCurr,
          _bmpPasNext,

   // The RBC Additive Solution Icons
          _bmpRasPrev,
          _bmpRasCurr,
          _bmpRasNext,

   // The WBC Count Icons (Magnifying Glass)
          _bmpWbcCountPrev,
          _bmpWbcCountCurr,
          _bmpWbcCountNext,

   // The Hyperconcentrate Icons (Asterisk)
          _bmpHyperPrev,
          _bmpHyperCurr,
          _bmpHyperNext,

   // comp_pls
          _bmpCompPls,
          _bmpCompPlsNext,
          _bmpCompPlsPrev;

   // DISPLAY TEXT OBJECTS ------------------

   // the display text objects
   Display_Text _txtNoProduct,
                _txtBetterAmapPltAvailable,
   // These are the labels for the product.
                _txtLblPlt,
                _txtLblPlas,
                _txtLblRbc,
                _txtLblTime,

                _txtLblAMAPPPlt,
                _txtLblAMAPPPlas,
                _txtLblAMAPPRbc,


                _txtDataAMAPPSizePrev,
                _txtDataAMAPPSizeCurr,
                _txtDataAMAPPSizeNext,

                _txtDataCompPlsVolm,

                _txtDataACPlsVolmNext,
                _txtACPlsUnitsNext,
                _txtDataACPlsVolmCurr,
                _txtACPlsUnitsCurr,
                _txtDataACPlsVolmPrev,
                _txtACPlsUnitsPrev ;


   TimerMessage _muteTimeout ;


private: // Member functions

   void systemStateNotify ();

   void timeoutResetMgr ();

   void enable_callbacks ();
   void disable_callbacks ();

   // set/clear the pump pause alarm (== stop / start the pumps)
   void setPumpPause ();
   void clearPumpPause ();

   // callback for incoming messages
   void predictResponseNotify ();

   void screenTimerHandler ();

   // FREEZE_SCREEN
   // This method disables all screen button objects.
   void freezeScreen (const char* from_);

   // THAW_SCREEN
   // This method enables all screen button objects.
   void thawScreen (const char* from_);

   // SCREEN_EXIT
   // This method manages all screen exits
   void screenExit (GUI_SCREEN_ID, const char*);

   // RESET_PRODUCT_SELECTION
   // This method manages the product selection updated prior to and via
   // messaging
   void resetProductSelection ();

   // RESET_INTERNAL_SCREEN_STATE
   // This method will reset the PREDICT internal screen state
   virtual void reset_internal_screen_state ();


   // UPDATE_SCREEN_DATA
   // Refresh the screen's data. (Experimental: don't do anything else)
   void updateScreenData ();

   void keypadStructMgr ();

   void select_current_procedure ();

   // Stuff to interact with confirm box
   void confirmCB ();
   void cancelCB ();
   void createConfirmCallbacks ();
   void destroyConfirmCallbacks ();


public:

   // METHODS

   Screen_PREDICT();

   virtual ~Screen_PREDICT();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   void clearDisplayedProcedures ();

   virtual void allocate_resources (const char* allocation_parameter);

   virtual void refresh_resources ();


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // PROCESS_CANCEL_BUTTON
   // This method is called from the C callback stub and executes "exit"
   virtual void process_cancel_button ();


   // PROCESS_GOBACK_BUTTON
   // This method is called from the vitual C callback stub and executes
   // "go back"
   virtual void process_goback_button ();


   // PROCESS_CONTINUE_BUTTON
   // This method is called from the virtual C callback stub and executes
   // "continue"
   virtual void process_continue_button ();


   // PROCESS_PREVIOUS_OPTIONS
   // This method is called from the C callback stub and executes
   // the up press on the user options bar
   void processPrevBtn ();


   // PROCESS_NEXT_OPTIONS
   // This method is called from the C callback stub and executes
   // the down press on the user options bar
   void processNextBtn ();


   void processAFAdjustBtn ();


   // PROCESS_DONOR_TIME_BUTTON
   // This method is called from the C callback stub and executes
   // "donor time"
   void processTimeBtn ();

   // PROCESS_MORE_OR_LESS  AMAP PLT only
   // This method is called from the C callback stub and executes
   // "Welcome to the Jungle" by Guns'n'Roses.
   virtual void processMoreOrLess ();


   // PROCESS_ADJUST_FLOW_RATE_BUTTON
   // This method is called from the C callback stub and executes "flow rate"
   void processAdjustFlowRateBtn ();

   void startBeepingSound ();
   void muteBeepingSound ();


#if CPU==SIMNT
protected:
   virtual void handleTraverseMessage ();
#endif

}; // END of Screen_PREDICT Class Declaration


#endif /* ifndef PREDICT_HPP */

/* FORMAT HASH 01ac3281c838ae2989b22746d85ffc34 */
