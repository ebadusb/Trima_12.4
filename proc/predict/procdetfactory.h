#ifndef PROCDETFACTORY_H
#define PROCDETFACTORY_H

#include "cassette.h"
#include "procdet.h"
#include "predict_types.h"

namespace ProcDetFactory
{
procedureDetail* CreateNewProcedure (cassette::cassetteType currentCassette,
                                     bool isCollectingPlt,
                                     PIRType pir);

cassette::cassetteType calcRecommendedCassette (cassette::cassetteType currentCassette,
                                                const procedureDetail& proc);

// Note: if this looks like an odd assortment of functions to group together...
// you're right.  This code is ripe for refactoring, but it's late in a
// development cycle, and further refactoring is a step too far right now. --DGS
}

#endif

/* FORMAT HASH 324288dc198f28caa765c524ecba98f0 */
