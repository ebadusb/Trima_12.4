/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include "guiglobs.hpp"

#if CPU!=SIMNT
extern "C"
#endif // #if CPU!=SIMNT
void guimain (void)
{
   DataLog(log_level_gui_info) << "Starting GUI task" << endmsg;
   guiglobs::guiMain = new GuiMain();
}

/* FORMAT HASH 96ec459dc6750a1e87a08218c2964c16 */
