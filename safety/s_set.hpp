/*
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/HOME/BCT_Development/vxWorks/Trima/safety/rcs/s_set.hpp 1.5 2001/08/27 21:37:29Z ms10234 Exp pn02526 $
 * $Log: s_set.hpp $
 * Revision 1.5  2001/08/27 21:37:29Z  ms10234
 * 5271 -  Remove references to stamp color
 * Revision 1.4  2000/07/22 00:33:02  bs04481
 * Remove compile flag and support of dual stage sets
 * Revision 1.3  2000/07/07 18:06:40  bs04481
 * Go back to using a five member enumeration for set type in order
 * to match procedure.
 * Revision 1.2  2000/06/22 16:46:47  bs04481
 * Plumbing changes for single-stage.  1) Change to RBC flow.
 * 2) Add valve position monitoring during Blood Run and Blood
 * Rinseback. 3)  Add compile flag to support dual-stage
 * Revision 1.1  1998/06/24 15:17:56  bs04481
 * Initial revision
 *
 *
 * TITLE:      s_set.hpp, set type constants
 *
 * ABSTRACT:   Safety's definitions for set type.
 *
 *
 */


#ifndef S_SET_HPP  //  Prevents multiple compilations.
#define S_SET_HPP

#include "chw.hpp"


// the set type is defined by the color of its stamp
// the number of cassette types defined here
#define NUM_SET_TYPES 4
// dual stage sets
enum SET_TYPE
{
   SET_NOT_LOADED = 0,
   SET_UNKNOWN    = 1,
   SET_PLTPLSRBC  = 2,       // four bag set
   SET_RBCPLS     = 3
};

#define SET_TYPE_STRING(t) ( (t==SET_NOT_LOADED) ? "SET_NOT_LOADED" : \
                             (t==SET_UNKNOWN) ? "SET_UNKNOWN" : \
                             (t==SET_PLTPLSRBC) ? "SET_PLTPLSRBC" : \
                             (t==SET_RBCPLS) ? "SET_RBCPLS" : \
                             "UNKNOWN" )


// possible directions of flow
enum FLOW_DIRECTION
{
   FLOW_UNKNOWN     = 0,      // we haven't set the flow because we don't know the set type
   FLOW_INTO_SET    = 1,      // set type dictates "normal" flow is into the set
   FLOW_INTO_BAG    = 2,      // set type dictates "normal" flow is into the bag
   FLOW_INTO_BAG_RC = 3,      // set type dictates "normal" flow is into the bag but the
                              // "other" flow is to recirculate to the collect -- not the reservoir
   FLOW_ERROR       = 4       // we can't determine flow because we don't know the set type
};

#define FLOW_DIRECTION_STRING(d) ( (d==FLOW_UNKNOWN) ? "FLOW_UNKNOWN" : \
                                   (d==FLOW_INTO_SET) ? "FLOW_INTO_SET" : \
                                   (d==FLOW_INTO_BAG) ? "FLOW_INTO_BAG" : \
                                   (d==FLOW_INTO_BAG_RC) ? "FLOW_INTO_BAG_RC" : \
                                   (d==FLOW_ERROR) ? "FLOW_ERROR" : \
                                   "UNKNOWN" )

// direction and condition of the flow in a set
struct set_flow
{
   SET_TYPE        set;                      // stamp color id
   FLOW_DIRECTION  InletFlow;                // normal flow dir from donor to set
   FLOW_DIRECTION  ReturnFlow;               // normal flow dir from set to donor
   FLOW_DIRECTION  ACBagFlow;                // normal flow dir for AC bag
   FLOW_DIRECTION  PlateletBagFlow;          // normal flow dir for Platelet bag
   FLOW_DIRECTION  PlasmaBagFlow;            // normal flow dir for Plasma bag
   FLOW_DIRECTION  RBCBagFlow;               // normal flow dir for RBC bag
   HW_VALVE_STATES InletValveFlowAllowed;    // a pseudo valve, always HW_VALVE_COLLECT
   HW_VALVE_STATES ReturnValveFlowAllowed;   // a pseudo valve, always HW_VALVE_COLLECT
   HW_VALVE_STATES ACValveFlowAllowed;       // a pseudo valve, always HW_VALVE_COLLECT
   HW_VALVE_STATES PlateletValveFlowAllowed; // normal valve position that allows flow
   HW_VALVE_STATES PlasmaValveFlowAllowed;   // normal valve position that allows flow
   HW_VALVE_STATES RBCValveFlowAllowed;      // normal valve position that allows flow
};

extern struct set_flow Set[NUM_SET_TYPES];


#endif  // S_SET_HPP

/* FORMAT HASH 4cf31541c7a49a4165494fd5ac198922 */
