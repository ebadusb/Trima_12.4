#ifndef CONFIG_LISTBOX_HPP
#define CONFIG_LISTBOX_HPP

#include "aphwind.hpp"
#include "cfg_ids.h"
#include "listbox.hpp"


class Config_Listbox
   : public Message<GUI_LISTBOX_STRUCT>
{

private:
   Config_Listbox();  // force use of nondefault constructor

public:
   typedef void (Base_Apheresis_Window::* ListboxCallback)(GUI_LISTBOX_STRUCT& buffer);


   // CONSTRUCTOR
   Config_Listbox (Base_Apheresis_Window* parent, ListboxCallback callback);

   // DESTRUCTOR
   virtual ~Config_Listbox();

   // NOTIFY
   virtual void notify ();

protected:

   Base_Apheresis_Window* _parent;
   ListboxCallback        _callback;
   GUI_LISTBOX_STRUCT     _localBuffer;

};

#endif

/* FORMAT HASH 9c3f4fe1a51dd9dc9e3b533b66cee983 */
