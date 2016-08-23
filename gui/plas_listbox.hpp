/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plas_listbox.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION:
 * This class inherits from Screen_EXTENDED_LISTBOX and adds more information
 * about a plasma procedure to its corresponding button in the procedure
 * priority config screen plasma listbox.
 *
 ******************************************************************************
 *
 *	$Header: E:/BCT_Development/Trima5.1x/Trima/gui/rcs/plas_listbox.hpp 1.1 2006/06/13 17:39:47Z dslausb Exp dslausb $
 *
 *	$Log: plas_listbox.hpp $
 *	Revision 1.1  2006/06/13 17:39:47Z  dslausb
 *	Initial revision
 *
 ******************************************************************************/


#ifndef __PLAS_LISTBOX_HPP__
#define __PLAS_LISTBOX_HPP__

#include "extended_listbox.hpp"

class Screen_PLAS_LISTBOX
   : public Screen_EXTENDED_LISTBOX
{
private:
   Display_Text _txtVolValue[MAX_BUTTONS_IN_LIST + 1];
   Display_Text _txtVolUnits[MAX_BUTTONS_IN_LIST + 1];

protected:
   virtual void addBtnExtras ();

public:
   Screen_PLAS_LISTBOX();
   virtual void deallocate_resources ();
};


#endif

/* FORMAT HASH cb6a90ad1e63706f571a8fc138eb1a49 */
