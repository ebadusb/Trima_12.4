/*                                    -
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      powertest.cpp
 *
 */


#include <powertest.h>

DEFINE_STATE(PowerTest) PowerTest::PowerTest()
   : StateAbs()
{}

PowerTest::~PowerTest()
{
   cleanup();
}

int PowerTest::preProcess ()
{
   return NORMAL;
}

int PowerTest::postProcess ()
{
   return NORMAL;
}

int PowerTest::preEnter ()
{
   return NORMAL;
}

void PowerTest::cleanup ()
{}

/* FORMAT HASH 33288eeba4124c83280dfe293aa9b258 */
