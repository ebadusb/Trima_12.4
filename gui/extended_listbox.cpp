/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      extended_listbox.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See header file
 *
 ******************************************************************************
 *
 *	$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/extended_listbox.cpp 1.1 2006/05/23 18:56:28Z dslausb Exp dslausb $
 *
 *	$Log: extended_listbox.cpp $
 *	Revision 1.1  2006/05/23 18:56:28Z  dslausb
 *	Initial revision
 *
 ******************************************************************************/

#include "plt_listbox.hpp"
#include "guiglobs.hpp"
#include "cfg_look.h"
#include "screen_config.hpp"

Screen_EXTENDED_LISTBOX::Screen_EXTENDED_LISTBOX(GUI_SCREEN_ID screenID)
   : Screen_LISTBOX()
{
   set_screen_id(screenID);
}

void Screen_EXTENDED_LISTBOX::allocate_resources (const char* allocation_parameter)
{
   Screen_LISTBOX::allocate_resources(allocation_parameter);
   addBtnExtras();
}

void Screen_EXTENDED_LISTBOX::process_up_button ()
{
   Screen_LISTBOX::process_up_button();
   addBtnExtras();
}

void Screen_EXTENDED_LISTBOX::process_down_button ()
{
   Screen_LISTBOX::process_down_button();
   addBtnExtras();
}

/* FORMAT HASH 6ce5c798fac5b31ab9bcc08d2446f536 */
