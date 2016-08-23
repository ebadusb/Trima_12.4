#include "cfr_list.h"
#include "guiglobs.hpp"

void dumpCfrList (const char* file, const int line)
{
   DataLog (log_level_gui_cfr11_info) << "----- CFR list: called from " << file << "(" << line << ") -----" << endmsg;
   for (CFR_LIST::iterator it = guiglobs::cfr_list.begin(); it != guiglobs::cfr_list.end(); it++)
      DataLog (log_level_gui_cfr11_info) << "---> key=" << it->key << "; trigger=" << it->trigger
                                         << "; authStatus=" << (int)it->authStatus << "; selected=" << it->selected
                                         << "; txt=\"" << it->btn_text << "\"" << endmsg;
}

/* FORMAT HASH 7dcac5d4af4fc27490f457efc73747c8 */
