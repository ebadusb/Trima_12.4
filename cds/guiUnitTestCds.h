#ifndef __GUI_UNIT_TEST_CDS_H__
#define __GUI_UNIT_TEST_CDS_H__

#include "datastore.h"

#ifndef WIN32
#pragma interface
#endif

class GuiUnitTest_CDS
   : public MultWriteDataStore
{
public:

   BaseElement<bool> autoScreenshots;

public:

   GuiUnitTest_CDS(Role role = ROLE_RW);
};

#endif

/* FORMAT HASH 4984a613c7f7a36067544742ce3373a0 */
