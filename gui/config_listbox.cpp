#include <vxworks.h>
#include "config_listbox.hpp"


Config_Listbox::Config_Listbox(Base_Apheresis_Window* parent, ListboxCallback callback)
   : Message<GUI_LISTBOX_STRUCT>(Callback<Config_Listbox>(this, &Config_Listbox::notify), MessageBase::SNDRCV_RECEIVE_ONLY),
     _parent(parent),
     _callback(callback)
{}

Config_Listbox::~Config_Listbox()
{
   _parent   = 0;
   _callback = 0;
}

void Config_Listbox::notify ()
{
   if (_parent && _callback)
   {
      _localBuffer = Message<GUI_LISTBOX_STRUCT>::getData();
      (_parent->*_callback)(_localBuffer);
   }
}

/* FORMAT HASH eaa0fd2585d003471c76df97375c8be2 */
