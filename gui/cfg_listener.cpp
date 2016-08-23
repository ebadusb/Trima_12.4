/************************> FOCUSSED GUI DOMAIN <****************************
*
* Copyright(c) 2003 Gambro BCT, Inc.  All rights reserved.
*
* History:
* $Log: cfg_listener.cpp $
* Revision 1.2  2008/06/19 15:16:41Z  dslausb
* IT 8568 - Working towards functional drop-in language selection
* Revision 1.1  2003/10/30 17:09:02Z  rm70006
* Initial revision
*
*************************< FOCUSSED GUI DOMAIN >****************************/

#include "cfg_listener.h"

#include "trima_datalog_levels.h"
#include "display_text.h"
#include "guiglobs.hpp"


//
//
//
Config_Listener::Config_Listener()
{}



//
//
//
Config_Listener::~Config_Listener()
{}



//
//
//
void Config_Listener::Initialize ()
{
   _configChangedMsg.init(Callback<Config_Listener>(this, &Config_Listener::HandleConfigChangedMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
}



//
//
//
void Config_Listener::HandleConfigChangedMsg ()
{
   const CONFIG_DATA_LISTBOX_STRINGS language = (CONFIG_DATA_LISTBOX_STRINGS)guiglobs::cdsData.config.LangUnit.Get().key_lang;

   DataLog(log_level_gui_info) << "Received New config.  Setting language to " << language << endmsg;

   // Display_Text::set_configuration_language(language);
}

/* FORMAT HASH 36b2d89ec30a9c88c992eeb6776b0bd4 */
