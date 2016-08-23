/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plt_listbox.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION:
 * This class inherits from Screen_EXTENDED_LISTBOX and adds more information
 * about a platelet procedure to its corresponding button in the procedure
 * priority config screen platelet listbox.
 *
 ******************************************************************************
 *
 *	$Header: E:/BCT_Development/Trima5.1x/Trima/gui/rcs/plt_listbox.hpp 1.4 2006/06/29 16:10:53Z dslausb Exp dslausb $
 *
 *	$Log: plt_listbox.hpp $
 *	Revision 1.4  2006/06/29 16:10:53Z  dslausb
 *	IT 7614 - Cleaned up procedure priority screen (took off yield, volume, and dose labels)
 *	Revision 1.3  2006/06/09 22:35:14Z  dslausb
 *	Platelet listbox has units with label.
 *	Revision 1.2  2006/05/25 22:28:21Z  dslausb
 *	- Made text changes from 5-25 gui review
 *	- Added config option for blood diversion
 *	Revision 1.1  2006/05/23 18:56:38Z  dslausb
 *	Initial revision
 *
 ******************************************************************************/


#ifndef __PLT_LISTBOX_HPP__
#define __PLT_LISTBOX_HPP__

#include "extended_listbox.hpp"

class Screen_PLT_LISTBOX
   : public Screen_EXTENDED_LISTBOX
{
private:
   Display_Text _txtYieldValue[MAX_BUTTONS_IN_LIST + 1];
   Display_Text _txtYieldx10[MAX_BUTTONS_IN_LIST + 1];
   Display_Text _txtYieldtoThe11th[MAX_BUTTONS_IN_LIST + 1];

protected:
   virtual void addBtnExtras ();

public:
   Screen_PLT_LISTBOX();
   virtual void deallocate_resources ();
};


#endif

/* FORMAT HASH e2f54b776f75ec74a3da9f57e7e8d751 */
