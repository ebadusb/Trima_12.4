/*
 * $Header: E:/BCT_Development/TrimaVxPort/Trima/trima_common/rcs/node_data.h 1.3 2007/08/01 22:42:15Z dslausb Exp $
 *
 *  Taos network node data for control, safety, and APC
 *
 * $Log: node_data.h $
 * Revision 1.3  2007/08/01 22:42:15Z  dslausb
 * Added guiMain class similar to Optia's
 * Revision 1.2  2007/07/10 16:34:26Z  dslausb
 * Fixed the declaration for a function
 * Revision 1.1  2007/07/09 20:04:38Z  dslausb
 * Initial revision
 * Revision 1.9  2005/01/18 18:11:22Z  ms10234
 * - added new node specific date functions
 * Revision 1.8  2005/01/11 15:46:40Z  wms10235
 * - Changed the APC node enum to APCBootNode and added the second APC processor as APCApplNode
 * Revision 1.7  2004/08/23 23:44:40Z  ms10234
 * - added new functions for getting node info
 * Revision 1.6  2004/07/02 19:19:14Z  ms10234
 * - changes to allow the message system to work over multiple subnets
 * Revision 1.5  2004/06/08 21:53:07Z  jl11312
 * - added support for simulator
 * Revision 1.4  2004/05/18 15:32:57Z  ms10234
 * changes for message system startup
 * Revision 1.3  2004/04/21 20:08:36Z  wms10235
 * Add PXE boot functionality and change IP addresses to 90.1.0.X and 90.2.0.X for the safety and APC subnets.
 * Revision 1.2  2004/04/20 15:46:42Z  jl11312
 * - change base node addresses
 * Revision 1.1  2004/03/04 17:49:49Z  jl11312
 * Initial revision
 *
 */

#ifndef _NODE_DATA_INCLUDE
#define _NODE_DATA_INCLUDE

#include <vxWorks.h>
#include "portnumbers.h"

/*
 *  Make sure this file uses only constructs valid in both C and C++.  Although
 * it is used mostly from .cpp files, the operating system image build
 * uses it from a .c file.
 */

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/*
 * Node information
 */
typedef enum
{
   /*
    *  If changing this enum, be sure to also change the nodeData
   * array defined below.
   */
   FirstNode   = 0,                    /* must be defined as 0 */
   ControlNode = FirstNode,
   SafetyNode,
   LocalNode,
   EndOfNodeEntries                    /* must be last entry */
} SysNodeID;

typedef struct
{
   const char*   name;                             /* name to be used in log messages */
   unsigned long ipAddress[EndOfNodeEntries];      /* ipAddress = network address in network byte order for interface to node i, 0 if not used */
   short         gatewayPort[EndOfNodeEntries];    /* gatewayPort = network port used by gateway task for inter-processor messaging to node i */
} SysNodeData;

SysNodeID   sysGetLocalNodeID (void);
const char* sysGetLocalNodeInitModule (void);
int         sysGetNodeData (SysNodeID id, SysNodeData* result);
SysNodeID   sysGetNodeID (unsigned long networkAddress);
SysNodeID   sysGetNodeIDFromString (const char* nodeName);
const char* sysGetNodeIDString (SysNodeID id);

#ifdef OS_BUILD

#if CPU==SIMNT
/*
 *  Internal node addresses for simulator are all on the
 * 90.0.0.x subnet.  The subnet base addresses below
 * are defined in network byte order.
*/
   #define SAFETY_SUBNET      0x0000005a  /* Safety subnet 90.0.0.0 */
   #define CONTROL_OCTET      0x01000000
   #define SAFETY_OCTET       0x02000000
#else /* if CPU==SIMNT */
      /*
       *  Internal node addresses are on the 90.1.0.X,
       * 90.2.0.X, and 90.3.0.X subnets.  The subnet
       * base addresses below are defined in network
       * byte order.
      */
   #define SAFETY_SUBNET      0x0000000a  /* Safety subnet 10.0.0.0 */
   #define CONTROL_OCTET      0x02000000
   #define SAFETY_OCTET       0x01000000
#endif /* if CPU==SIMNT */


/*
   Although defined here so that node information is contained completely within one file,
   this data is intended only for use by the Trima OS image.  All other applications should
   use the sysGetNodeData function.
*/

static SysNodeData nodeData[EndOfNodeEntries] =
{
   {
      /* Control has a network card connected to the safety processors */
      "ControlNode",
      {  0,                                  /* address of control from control - undeterminable*/
         SAFETY_SUBNET + SAFETY_OCTET,       /* address of safety from control 10.0.0.1 */
         0},
      { 0, Trima_SafetyGatewayPort, 0}
   },

   {
      /* Safety has a single network card connected to the control processor */
      "SafetyNode",
      {  SAFETY_SUBNET + CONTROL_OCTET,  /* address of control from safety 10.0.0.2 */
         0,                              /* address of safety from safety - undeterminable*/
         0},
      { Trima_ControlGatewayPort, 0, 0}
   },

   {
      /* default address used when the local node wants it's own address */
      "LocalNode",
      {  SAFETY_SUBNET + CONTROL_OCTET,    /* address of control from Safety 10.0.0.2 */
         SAFETY_SUBNET + SAFETY_OCTET,     /* address of safety from control 10.0.0.1 */
         0},                               /* local address doesn't make sense in this case */
      { Trima_ControlGatewayPort, Trima_SafetyGatewayPort, 0}
   }
};

#endif /* ifdef OS_BUILD */

#ifdef __cplusplus
}  // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _NODE_DATA_INCLUDE */

/* FORMAT HASH abe996aabf5b8c7794c3bb12c6ed4c06 */
