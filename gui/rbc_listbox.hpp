/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rbc_listbox.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION:
 * This class inherits from Screen_EXTENDED_LISTBOX and adds more information
 * about an RBC procedure to its corresponding button in the procedure
 * priority config screen RBC listbox.
 *
 ******************************************************************************
 *
 *	$Header: E:/BCT_Development/Trima5.1x/Trima/gui/rcs/rbc_listbox.hpp 1.1 2006/06/13 17:40:03Z dslausb Exp dslausb $
 *
 *	$Log: rbc_listbox.hpp $
 *	Revision 1.1  2006/06/13 17:40:03Z  dslausb
 *	Initial revision
 *
 ******************************************************************************/


#ifndef __RBC_LISTBOX_HPP__
#define __RBC_LISTBOX_HPP__

#include "extended_listbox.hpp"

class Screen_RBC_LISTBOX
   : public Screen_EXTENDED_LISTBOX
{
private:
   Display_Text _txtDoseValue[MAX_BUTTONS_IN_LIST + 1];
   Display_Text _txtDoseUnits[MAX_BUTTONS_IN_LIST + 1];

protected:
   virtual void addBtnExtras ();

public:
   Screen_RBC_LISTBOX();
   virtual void deallocate_resources ();
};


#endif

/* FORMAT HASH 46b8c9a389a41acda71afa3bbcb3bd66 */
