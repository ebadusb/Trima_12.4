/*
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/safety/rcs/s_set.cpp 1.4 2001/08/27 21:37:27Z ms10234 Exp $
 * $Log: s_set.cpp $
 * Revision 1.4  2001/08/27 21:37:27Z  ms10234
 * 5271 -  Remove references to stamp color
 * Revision 1.3  2000/07/10 15:24:55  bs04481
 * Set grey sets for illegal flows even though grey is allowed in the
 * enumeration for set type.
 * Revision 1.2  2000/07/07 18:06:37  bs04481
 * Go back to using a five member enumeration for set type in order
 * to match procedure.
 * Revision 1.1  2000/06/22 16:47:17  bs04481
 * Initial revision
 *
 *
 * TITLE:      s_set.cpp, set type constants
 *
 * ABSTRACT:   Safety's definitions for set type.
 *
 *
 */

#include "s_set.hpp"

// this structure is global to the other container objects
// Note that the Return container flow is set to FLOW_INTO_SET when
// it seems that it should be FLOW_INTO_BAG.  This is because the
// sign of the return pump volume set by the drivers is negative
// when the flow is away from the donor.
set_flow Set[NUM_SET_TYPES] =
{
// type,          Inlet,            Return            ac bag,           platelet bag,     plasma bag,       rbc bag,
   {SET_NOT_LOADED, FLOW_UNKNOWN,     FLOW_UNKNOWN,     FLOW_UNKNOWN,     FLOW_UNKNOWN,     FLOW_UNKNOWN,     FLOW_UNKNOWN,
    HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, },
// type,          Inlet,            Return            ac bag,           platelet bag,     plasma bag,       rbc bag,
   {SET_UNKNOWN,    FLOW_ERROR,       FLOW_ERROR,       FLOW_ERROR,       FLOW_ERROR,       FLOW_ERROR,       FLOW_ERROR,
    HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, },
// type,          Inlet,            Return            ac bag,           platelet bag,     plasma bag,       rbc bag,
   {SET_PLTPLSRBC,  FLOW_INTO_SET,    FLOW_INTO_SET,    FLOW_INTO_SET,    FLOW_INTO_BAG,    FLOW_INTO_BAG_RC, FLOW_INTO_BAG,
    HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, },
// type,          Inlet,            Return            ac bag,           platelet bag,     plasma bag,       rbc bag,
   {SET_RBCPLS,     FLOW_INTO_SET,    FLOW_INTO_SET,    FLOW_INTO_SET,    FLOW_INTO_SET,    FLOW_INTO_BAG,    FLOW_INTO_BAG,
    HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_COLLECT, HW_VALVE_RETURN,  HW_VALVE_COLLECT, HW_VALVE_COLLECT, }
};

/* FORMAT HASH 92f4bbb03312f573718ebf5580383570 */
