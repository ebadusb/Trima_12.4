/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      operatorresponse.cpp
 *
 */

#include <operatorresponse.h>
#include <periodic_log.h>
#include "procdata.h"


DEFINE_STATE(OperatorResponse);

OperatorResponse::OperatorResponse()
   : StateAbs(),
     _LoggingInterval(0.0)
{
   _cassetteMsg = new  Message<CASSETTE_MESSAGE>(Callback<OperatorResponse>(this, &OperatorResponse::cassetteFuctMsg),
                                                 MessageBase::SNDRCV_LOCAL);

   _clearBitsMsg = new  ClearRefNumberMsg(Callback<OperatorResponse>(this, &OperatorResponse::clearFuctMsg),
                                          MessageBase::SNDRCV_LOCAL);
}

OperatorResponse::OperatorResponse(const OperatorResponse& state)
   : StateAbs(state),
     _LoggingInterval(state._LoggingInterval)
{
   copyOver(state);
}

OperatorResponse::~OperatorResponse()
{
   cleanup();
}

int OperatorResponse::preProcess ()
{
   return NORMAL;
}

int OperatorResponse::postProcess ()
{
   return NORMAL;
}

int OperatorResponse::preEnter ()
{
   PeriodicLog::getInterval(_LoggingInterval);
   PeriodicLog::enablePeriodicity(30.0 /*seconds*/);

   return NORMAL;
}

int OperatorResponse::postExit ()
{
   PeriodicLog::enablePeriodicity(_LoggingInterval);

   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void OperatorResponse::copyOver (const OperatorResponse&)
{}

void OperatorResponse::cleanup ()
{}

void OperatorResponse::cassetteFuctMsg (void* data)
{
   ProcData         pd;
   CASSETTE_MESSAGE msgData = (CASSETTE_MESSAGE)_cassetteMsg->getData();

   pd.MakeRunWritable();
   pd.Run().CassetteFunctionBits.Set(msgData.functBits);
   pd.Run().CassetteRef.Set(msgData.ref);
   pd.MakeRunReadable();

   DataLog(log_level_proc_mss_info) << "Cassette received from GUI (REF=" << msgData.ref
                                    << "; BITS=" << msgData.functBits << ")" <<  endmsg;
}

void OperatorResponse::clearFuctMsg (void* data)
{

   ProcData pd;
   pd.MakeRunWritable();
   pd.Run().CassetteFunctionBits.Set(0);
   pd.Run().CassetteRef.Set(0);
   pd.MakeRunReadable();

   DataLog(log_level_proc_mss_info) << " Cassette Fuction Bits cleared by GUI goback -> "  << pd.Run().CassetteFunctionBits.Get() << endmsg;

}

/* FORMAT HASH d5374a6bfaa18d6b758e4252a751412a */
