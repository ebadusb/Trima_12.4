#include <vxworks.h>
#include "config_keypad.hpp"


Config_Keypad::Config_Keypad(Base_Apheresis_Window* parent, KeypadCallback callback)
   : Message<KEYPAD_RETURN_STRUCT>(Callback<Config_Keypad>(this, &Config_Keypad::notify), MessageBase::SNDRCV_RECEIVE_ONLY),
     _parent(parent)
{
   _callback = callback;
}

Config_Keypad::~Config_Keypad()
{
   _parent   = 0;
   _callback = 0;
}


void Config_Keypad::notify ()
{
   _localBuffer = this->getData();

   if (_parent && _callback)
   {
      (_parent->*_callback)(_localBuffer);
   }
}

/* FORMAT HASH 984524b41ebb40621195d7d0f64a2d3c */
