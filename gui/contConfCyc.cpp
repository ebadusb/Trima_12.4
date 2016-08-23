#include "contConfCyc.hpp"
#include "trima_datalog.h"

void contConfCycContinueCallback (void* data);
void contConfCycConfirmCallback (void* data);

ContinueConfirmCycle::BtnLocks::BtnLocks()
   : _continueBtnLockout(false),
     _confirmBtnLockout(true)
{}

bool ContinueConfirmCycle::BtnLocks::continueBtnGetLock (int lineNum)
{
   if (_continueBtnLockout)
   {
      DataLog (log_level_gui_info) << "Continue button lock request from line "
                                   << lineNum << " ignored (button is already locked)." << endmsg;
      return false;
   }
   else
   {
      _continueBtnLockout = true;
      DataLog (log_level_gui_info) << "Continue button lock request from line "
                                   << lineNum << " granted." << endmsg;
      return true;
   }
}

void ContinueConfirmCycle::BtnLocks::continueBtnReleaseLock (int lineNum)
{
   _continueBtnLockout = false;
   DataLog (log_level_gui_info) << "Continue button release lock request from line "
                                << lineNum << " granted." << endmsg;
}

bool ContinueConfirmCycle::BtnLocks::confirmBtnGetLock (int lineNum)
{
   if (_confirmBtnLockout)
   {
      DataLog (log_level_gui_info) << "Confirm button lock request from line "
                                   << lineNum << " ignored (button is already locked)." << endmsg;
      return false;
   }
   else
   {
      _confirmBtnLockout = true;
      DataLog (log_level_gui_info) << "Confirm button lock request from line "
                                   << lineNum << " granted." << endmsg;
      return true;
   }
}

void ContinueConfirmCycle::BtnLocks::confirmBtnReleaseLock (int lineNum)
{
   _confirmBtnLockout = false;
   DataLog (log_level_gui_info) << "Confirm button release lock request from line "
                                << lineNum << " granted." << endmsg;
}

void ContinueConfirmCycle::BtnLocks::forceLocks (int lineNum)
{
   _continueBtnLockout = true;
   _confirmBtnLockout  = true;
   DataLog (log_level_gui_info) << "Forcing both buttons to locked (called from line "
                                << lineNum << ".)" << endmsg;
}

void ContinueConfirmCycle::BtnLocks::forceReset (int lineNum)
{
   _continueBtnLockout = false;
   _confirmBtnLockout  = true;
   DataLog (log_level_gui_info) << "Forced both buttons to reset (called from line "
                                << lineNum << ".)" << endmsg;
}

// This is the constructor that should be used.
ContinueConfirmCycle::ContinueConfirmCycle(Base_Apheresis_Window*  parentWindow,

                                           Button_Bitmap::CallbackFunc cycleCompleteCallback,

                                           Button_Continue& parentContinue,

                                           GUI_BUTTON_ID confirmBtnId,
                                           short confirmBtnX,
                                           short confirmBtnY
                                           ) :

   _parentWindow(parentWindow),
   _btnContinue(parentContinue),
   _btnConfirm(confirmBtnId,
               confirmBtnX,
               confirmBtnY,
               textButtonLongDisconnectConfirm,
               BITMAP_BUTTON_CONFIRM_UP,
               BITMAP_BUTTON_CONFIRM_DN,
               true),
   _btnLocks(),

   _msgConfirmTimer(DISCONN_CONFIRM_PERIOD,
                    Callback<ContinueConfirmCycle>(this, &ContinueConfirmCycle::confirmTimeoutCallback), TimerMessage::DISARMED),

   _cycleCompleteCallback(cycleCompleteCallback)
{ }

void ContinueConfirmCycle::armTimer (int lineNum)
{
   DataLog (log_level_gui_info) << "Starting confirm timer (called from line "
                                << lineNum << ".)" << endmsg;

   _msgConfirmTimer.armTimer(TimerMessage::ARMED);
}

void ContinueConfirmCycle::disarmTimer (int lineNum)
{
   _msgConfirmTimer.armTimer(TimerMessage::DISARMED);

   DataLog (log_level_gui_info) << "Confirm timer reset. (called from line "
                                << lineNum << ".)" << endmsg;
}

void ContinueConfirmCycle::resetCycle (const char* filename, int lineNum)
{
   // Force both locks to prevent button presses.
   _btnLocks.forceLocks(__LINE__);

   // Reset continue button timeout timer
   disarmTimer(__LINE__);

   // Set up buttons
   DataLog (log_level_gui_info) << "Deallocating confirm button." << endmsg;
   _btnConfirm.deallocate_resources();

   DataLog (log_level_gui_info) << "Allocating continue button." << endmsg;
   _btnContinue.allocate_resources(*_parentWindow);

   // Reset the locks (confirm locked, continue unlocked).
   _btnLocks.forceReset(__LINE__);

   DataLog (log_level_gui_info)
      << "Continue / confirm cycle has been reset. (called from file "
      << filename << ", line " << lineNum << ".)" << endmsg;
}

void ContinueConfirmCycle::allocate_resources ()
{
   _btnConfirm.set_callback(contConfCycConfirmCallback, this);
   _btnContinue.set_callback(contConfCycContinueCallback, this);

   resetCycle(__FILE__, __LINE__);
}

void ContinueConfirmCycle::deallocate_resources ()
{
   // Prevent further button presses
   _btnLocks.forceLocks(__LINE__);

   disarmTimer(__LINE__);

   _btnConfirm.deallocate_resources();
   _btnContinue.deallocate_resources();
}

void contConfCycContinueCallback (void* data)
{
   ((ContinueConfirmCycle*)data)->continueBtnCallback();
}

void ContinueConfirmCycle::continueBtnCallback ()
{
   // Try to lock the continue button.  If you can't, just return.
   if (!_btnLocks.continueBtnGetLock(__LINE__)) return;

   // Unlock the confirm button
   _btnLocks.confirmBtnReleaseLock(__LINE__);

   // Set up the buttons.
   DataLog (log_level_gui_info) << "Deallocating continue button." << endmsg;
   _btnContinue.deallocate_resources();

   DataLog (log_level_gui_info) << "Allocating confirm button." << endmsg;
   _btnConfirm.allocate_resources(*_parentWindow);

   // Start the timeout timer.
   armTimer(__LINE__);

   DataLog (log_level_gui_info) << "Continue button callback complete" << endmsg;
}

void contConfCycConfirmCallback (void* data)
{
   ((ContinueConfirmCycle*)data)->confirmBtnCallback();
}

void ContinueConfirmCycle::confirmBtnCallback ()
{
   // Try to lock the confirm button.  If you can't, just return.
   if (!_btnLocks.confirmBtnGetLock(__LINE__)) return;

   // Reset the timeout timer.
   disarmTimer(__LINE__);

   // No more button presses allowed.
   _btnLocks.forceLocks(__LINE__);

   // Set up the button.
   DataLog (log_level_gui_info) << "Deallocating confirm button." << endmsg;
   _btnConfirm.deallocate_resources();

   // Call the client's callback now that the cycle is complete
   _cycleCompleteCallback((void*)_parentWindow);

   DataLog (log_level_gui_info) << "Confirm button callback complete" << endmsg;
}

void ContinueConfirmCycle::confirmTimeoutCallback ()
{
   resetCycle(__FILE__, __LINE__);

   DataLog (log_level_gui_info) << "Confirm timeout complete" << endmsg;
}

/* FORMAT HASH 4b679226a01abdd40000b8557deec83a */
