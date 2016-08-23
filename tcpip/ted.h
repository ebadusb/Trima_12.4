/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ted.h
 *             Trima External Diagnostic Interface
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the interface between Trima and EMS.
 *
 * HISTORY:    $Log: ted.h $
 * HISTORY:    Revision 1.7  2003/10/14 21:54:36Z  rm70006
 * HISTORY:    IT 5830.  Add current cursor position message.  Fix bug with buffer logic.
 * HISTORY:    Revision 1.6  2003/10/08 18:31:43Z  rm70006
 * HISTORY:    IT 5830.  Put more fixes in TED.
 * HISTORY:    Revision 1.5  2003/07/01 15:59:05Z  rm70006
 * HISTORY:    IT 5830.  Interim checkin.  Got periodic status working.
 * HISTORY:    Revision 1.4  2003/06/16 21:58:21Z  rm70006
 * HISTORY:    IT 5830.
 * HISTORY:    Add more functionality to TED.
 * HISTORY:    Revision 1.3  2003/04/07 23:09:38Z  rm70006
 * HISTORY:    IT 5818.
 * HISTORY:
 * HISTORY:    Change logging.
 * HISTORY:    Revision 1.2  2002/12/20 21:49:54Z  rm70006
 * HISTORY:    Incremental checkin.
 * HISTORY:    Revision 1.1  2002/10/29 16:13:43Z  rm70006
 * HISTORY:    Initial revision
 *
 *******************************************************************************/

#ifndef __TED__
#define __TED__

#include "messagesystem.h"
#include "sockinet.h"
#include "tedif.h"
#include "ted_hw_periodic_msg.h"
#include "ted_message.h"
#include "cursor_msgs.h"
#include "xif_cds.h"


class TED
{
// Class Methods
public:
   TED();
   virtual ~TED();

   bool Initialize ();

   void ProcessMessages ();

   bool IsConnectionActive () {return _connectionActive; }

private:
   bool HandleCurrentCursorPositionMsg (const CURRENT_CURSOR_POSITION_MSG& in, TED_CurrentCursorPositionMsg& out);

   void ProcessOneMessage (const void* msg);

   void ProcessInternalMessages ();

   void* GetMessageFromBuffer (const sockinetaddr& client, TED_BUFFER_STATUS msgStatus, char* buffer, int& bufferIndex, int packetSize);

   void ManageConnection ();

   void DeRegister ();

private:
   MessageSystem   _ms;

   sockbuf         _commandSocket;
   sockinetaddr    _listenAddress;
   sockinetaddr    _connectedHost;

   struct timeval  _timeoutValue;

   bool            _connectionActive;

   TedIf           _tedIf;

   External_IF_CDS _xifCds;

   // External Messages
   TedHwPeriodicStatus* _controlPeriodicStatus;
   TedHwPeriodicStatus* _safetyPeriodicStatus;

   TedOneToOneStatusMsg <TED, CURRENT_CURSOR_POSITION_MSG, TED_CurrentCursorPositionMsg>* _currentCursorPositionMsg;

   // Declare private copy constructor and copy assignment operator but don't define
   // to prevent use
   TED& operator = (const TED& source);
   TED(const TED& source);
};

#endif

/* FORMAT HASH 389942165309a97c0e070fb97049599b */
