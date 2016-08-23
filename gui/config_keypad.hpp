#ifndef CONFIG_KEYPAD_HPP
#define CONFIG_KEYPAD_HPP

#include "aphwind.hpp"
#include "cfg_ids.h"
#include "keypad.h"
#include "message.h"


class Config_Keypad
   : public Message<KEYPAD_RETURN_STRUCT>
{

private:
   Config_Keypad();  // force use of nondefault constructor

public:
   typedef void (Base_Apheresis_Window::* KeypadCallback)(KEYPAD_RETURN_STRUCT& buffer);

   //
   // Constructor, destructor
   //
   Config_Keypad (Base_Apheresis_Window* parent, KeypadCallback callback);
   virtual ~Config_Keypad();

   //
   //
   //  Message notification
   //
   virtual void notify ();


protected:

   Base_Apheresis_Window* _parent;
   KeypadCallback         _callback;
   KEYPAD_RETURN_STRUCT   _localBuffer;
};

#endif

/* FORMAT HASH 1cd0fa7ebfd9b8b835d2bb7c2370ffb8 */
