// FILENAME: spoofercontrol.h
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/spoofercontrol.h 6.13 2003/11/11 18:49:14Z td07711 Exp td07711 $
// PURPOSE: defines SpooferControl message data
// CHANGELOG:
//   $Log: spoofercontrol.h $
//   Revision 6.13  2003/11/11 18:49:14Z  td07711
//   IT6091 - support bssOverflow
//   Revision 6.12  2003/10/24 17:57:08Z  td07711
//   IT6091 - add support for memory protection testing
//   Revision 6.11  2003/09/05 19:50:02Z  td07711
//   IT6394 - support for taskSuspend on safety
//   Revision 6.10  2003/07/22 22:28:27Z  td07711
//   IT5303 - support turning spoof_server debug msgs on and off from a spoof_client.
//   Revision 6.9  2003/05/29 18:42:18Z  td07711
//   support FATAL_ERROR spoofs
//   Revision 6.8  2003/03/14 23:46:29Z  td07711
//   support msg multiplier (to stop or flood msgs)
//   Revision 6.7  2002/11/12 00:49:40Z  td07711
//   remove .data from SpooferAck
//   Revision 6.6  2002/10/31 22:47:58  td07711
//   add SCM_SPOOF_ENABLE and SCM_SPOOF_DISABLE commands
//   Revision 6.5  2002/10/10 21:28:57  td07711
//   add SCM_SHOW_SPOOFS and SCM_UNSPOOF_DATA
//   Revision 6.4  2002/10/08 00:15:39  td07711
//   remove SCM_SPOOF_SEND
//   Revision 6.3  2002/09/26 21:53:43  td07711
//   added SCM_SPOOF_LIMIT command for setting spoof limit
//   added SCM_SPOOF_EXIT command to force spoofing tasks to exit
//   Revision 6.2  2002/09/17 00:12:52  td07711
//   Revision 6.1  2002/09/13 23:47:16  td07711
//   Revision 1.1  2002/09/12 22:55:58  td07711
//   Initial revision


#ifndef SpooferControl_H // prevents multiple inclusion
#define SpooferControl_H


enum SCM_COMMAND
{
    SCM_SPOOF_EXIT, // end spoofing and exit
    SCM_SPOOF_END, // ends spoofing and cleanup
    SCM_SPOOF_ACTIVATE, // enable pending spoofs
    SCM_SPOOF_DATA, // spoof specific field
    SCM_ACK,  // sent to acknowledge successful processing of received msg
    SCM_NACK, // sent to notify of unsuccessful processing of received msg
    SCM_SPOOF_LIMIT, // end spoofing after n spoofs
    SCM_SHOW_SPOOFS,
    SCM_UNSPOOF_DATA, // quit spoofing specific field
    SCM_SPOOF_ENABLE,
    SCM_SPOOF_DISABLE,
    SCM_MSG_MULTIPLIER,
    SCM_FATAL_ERROR,
    SCM_ENABLE_DEBUG,
    SCM_DISABLE_DEBUG,
    SCM_SUSPEND_TASK,
    SCM_NULLPTR_READ,
    SCM_TEXT_WRITE,
    SCM_STACK_OVERFLOW,
    SCM_DATA_OVERFLOW,
    SCM_BSS_OVERFLOW,
    SCM_LOGGING_TEST
};


typedef struct
{
    SCM_COMMAND command; 
    int taskid; // uniquely identifies source task
    int seqnum; // sequence number used for ack/nack

    char structName[40];
    char fieldName[40];
    char valueName[40];
    float multiplier;
    float offset;
    int data;

} SpooferControl;


typedef struct
{
    SCM_COMMAND command;
    int taskid; // task sending the ACK/NACK
    int ackTaskid; // taskid of msg being acked
    int ackSeqnum; // seqnum of msg being acked

} SpooferAck;


#endif // SpooferControl_H
