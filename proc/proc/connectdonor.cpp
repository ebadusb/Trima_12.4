/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      connectdonor.cpp
 *
 */

#include "connectdonor.h"
#include "states.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_STATE(ConnectDonor);

ConnectDonor::ConnectDonor()
   : StateAbs(),
     _RunInfo()
{}

ConnectDonor::ConnectDonor(const ConnectDonor& state)
   : StateAbs(state),
     _RunInfo()
{}

ConnectDonor::~ConnectDonor()
{}

int ConnectDonor::transitionStatus ()
{
   return NO_TRANSITION;
}

int ConnectDonor::preProcess ()
{
   return NORMAL;
}

int ConnectDonor::postProcess ()
{
   ProcData pd;

   //
   //
   //  The "first cycle pressure meter" is active during this state
   //
   _RunInfo.Pressure(CobeConfig::data().PressureMeterMinLimit,
                     CobeConfig::data().PressureMeterMaxLimit,
                     pd.Status().APS(),
                     1
                     );
   _RunInfo.SendMsg();

   return NORMAL;
}

int ConnectDonor::preEnter ()
{
   ProcData pd;
   //
   // Send the state message for system wide notification
   //  that the donor should now be considered connected ...
   //
   pd.SystemState(DONOR_CONNECTED);

   //
   // Set the cassette state ...
   //
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::BLOOD);

   //
   // Initialize the Run Info object ...
   //
   pd.MakeRunWritable();
   _RunInfo.Initialize();
   pd.MakeRunReadable();

   return NORMAL;
}

int ConnectDonor::postExit ()
{
   return NORMAL;
}

void ConnectDonor::reset ()
{}

void ConnectDonor::copyOver (const ConnectDonor&)
{}

void ConnectDonor::cleanup ()
{}

/* FORMAT HASH a84984c46d68e9ff44bbfbc751ad0df9 */
