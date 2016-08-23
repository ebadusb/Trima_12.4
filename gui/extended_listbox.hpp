/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      extended_listbox.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION:
 * This class should not be instantiated.  It inherits from Screen_LISTBOX and
 * makes it so that allocate_resources(), process_up_button(), and process_down_button(),
 * all call the virtual addBtnExtras function, which must be defined in classes
 * that inherit this class.
 * This allows you make adjustments to button appearance and text after a listbox
 * has been created without messing with the other listbox functionality.
 *
 *
 ******************************************************************************
 *
 *	$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/extended_listbox.hpp 1.1 2006/05/23 18:56:31Z dslausb Exp dslausb $
 *
 *	$Log: extended_listbox.hpp $
 *	Revision 1.1  2006/05/23 18:56:31Z  dslausb
 *	Initial revision
 *
 ******************************************************************************/


#ifndef __EXTENDED_LISTBOX_HPP__
#define __EXTENDED_LISTBOX_HPP__

#include "listbox.hpp"

class Screen_EXTENDED_LISTBOX
   : public Screen_LISTBOX
{
protected:
   virtual void addBtnExtras () = 0;

public:
   Screen_EXTENDED_LISTBOX(GUI_SCREEN_ID screenID);

   virtual void allocate_resources (const char* allocation_parameter);
   virtual void process_up_button ();
   virtual void process_down_button ();
};


#endif

/* FORMAT HASH 97993048985ab6eb1363169d25913cd6 */
