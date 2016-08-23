#ifndef __CONTINUE_CONFIRM_CYCLE_HPP__
#define __CONTINUE_CONFIRM_CYCLE_HPP__

#include "aphwind.hpp"
#include "timermessage.h"

class ContinueConfirmCycle
{
private:
   // Disable default constructor.
   ContinueConfirmCycle();

   class BtnLocks
   {
   private:

      bool
      // These two flags act like semaphores to lock out access to their buttons.
         _continueBtnLockout,
         _confirmBtnLockout;

   public:

      // Line Numbers are for logging.
      bool continueBtnGetLock (int lineNum);
      void continueBtnReleaseLock (int lineNum);
      bool confirmBtnGetLock (int lineNum);
      void confirmBtnReleaseLock (int lineNum);

      void forceLocks (int lineNum);
      void forceReset (int lineNum);

      BtnLocks();
   };

   ////////////////

   Base_Apheresis_Window*      _parentWindow;
   Button_Continue&            _btnContinue; // The parent window's continue button
   Button_Bitmap               _btnConfirm;  // The confirm button
   BtnLocks                    _btnLocks;

   TimerMessage                _msgConfirmTimer; // Timeout
   Button_Bitmap::CallbackFunc _cycleCompleteCallback;

   void armTimer (int lineNum);
   void disarmTimer (int lineNum);

   void confirmTimeoutCallback ();

public:

   // Crazy Big Constructor
   ContinueConfirmCycle(Base_Apheresis_Window*  parentWindow,

                        Button_Bitmap::CallbackFunc cycleCompleteCallback,

                        Button_Continue& parentContinue,

                        GUI_BUTTON_ID confirmBtnId,
                        short confirmBtnX,
                        short confirmBtnY
                        );

   void allocate_resources ();
   void deallocate_resources ();
   void resetCycle (const char* filename, int lineNum);

   void continueBtnCallback ();
   void confirmBtnCallback ();

};

#endif // #ifndef __CONTINUE_CONFIRM_CYCLE_HPP__

/* FORMAT HASH fa695aab149d5ae1b753c508b75b63b2 */
