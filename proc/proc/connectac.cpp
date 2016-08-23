/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      connectac.cpp
 *
 */

#include "connectac.h"
#include "trimamessages.h"
#include "guiproc.h"
#include "procdata.h"

DEFINE_STATE(ConnectAC);

ConnectAC::ConnectAC()
   : StateAbs()
{}

ConnectAC::ConnectAC(const ConnectAC& state)
   : StateAbs(state)
{
   copyOver(state);
}

ConnectAC::~ConnectAC()
{
   cleanup();
}

int ConnectAC::transitionStatus ()
{
   return NO_TRANSITION;
}

int ConnectAC::preProcess ()
{
   return NORMAL;
}

int ConnectAC::postProcess ()
{
   return NORMAL;
}

int ConnectAC::preEnter ()
{

   //
   // Send the gui handshake message ...
   //
   TestCompletedMsg guiMsg(MessageBase::SEND_LOCAL);
   guiMsg.send(1);
   DataLog(log_level_proc_info) << "GUI Message sent : Disposable Test Completed" << endmsg;

   //
   // Set the system state to AC_CONNECTED state ...
   //
   ProcData pd;
   pd.SystemState(AC_CONNECTED);


   if (Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      // Make sure valves are returning
      //
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
      pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);      //
   }
   //
   // The cassette can now be connected to AC and should be considered
   //  in a non-dry state ...
   //
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::AC);

   return NORMAL;
}

int ConnectAC::postExit ()
{
   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void ConnectAC::copyOver (const ConnectAC&)
{}

void ConnectAC::cleanup ()
{}

/* FORMAT HASH 68fd128be6356fbcf4673d8c0cd2728f */
