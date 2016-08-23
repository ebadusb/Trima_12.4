/*
 * Copyright (C) 2008 CaridianBCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R_screenshots/Trima/cds/rcs/guiUnitTestCds.cpp 1.1 2008/09/04 17:14:27Z dslausb Exp $
 * $Log: guiUnitTestCds.cpp $
 * Revision 1.1  2008/09/04 17:14:27Z  dslausb
 * Initial revision
 *
 */

#include <vxWorks.h>

#ifndef WIN32
#pragma implementation "guiUnitTestCds.h"
#endif

#include "guiUnitTestCds.h"

GuiUnitTest_CDS::GuiUnitTest_CDS(Role role = ROLE_RW)
   : MultWriteDataStore ("GuiUnitTest_CDS", role)
{
   autoScreenshots.Register     (this, NO_PFR, false);
}

/* FORMAT HASH 43f4517eb79c4d4ab4f939a27792fc10 */
