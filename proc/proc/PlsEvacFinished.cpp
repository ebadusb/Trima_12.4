#include "PlsEvacFinished.h"
#include "states.h"
#include "procdata.h"
#include "software_cds.h"     // Get Optional sw.dat settings
                              //
#include "cobeconfig.h"


DEFINE_STATE(PlsEvacFinished);

PlsEvacFinished::PlsEvacFinished()
   : StateAbs()
{}

PlsEvacFinished::PlsEvacFinished(const PlsEvacFinished& state)
   : StateAbs(state)
{

   copyOver(state);
}

PlsEvacFinished::~PlsEvacFinished()
{
   cleanup();
}

int PlsEvacFinished::preProcess ()
{
   return NORMAL;
}

int PlsEvacFinished::postProcess ()
{
   return NORMAL;
}

int PlsEvacFinished::transitionStatus ()
{

   ProcData pd;

   if (!Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      DataLog(log_level_proc_info) << "air_mitigation_disabled - exiting" << endmsg;
      return 1;
   }

//     if (pd.TrimaSet().Cassette.Get() == cassette::RBCPLS)
//     {
//         DataLog( log_level_proc_info ) << "PlsEvac disabled for RBCPLS sets"  << endmsg;
//          return 1;
//     }

   if (!pd.Config().Procedure.Get().key_air_removal)
   {
      DataLog(log_level_proc_info) << "airevac disabled "  << endmsg;
      return 1;
   }

   if (pd.TrimaSet().Cassette.Get() == cassette::PLTPLSRBC)
   {
      if (pd.Run().PlsBagEvacVc.Get() >= CobeConfig::data().EvacuationPlsEvacVc  + pd.Run().PlsBagEvacVin_done.Get() )
      {
         DataLog(log_level_proc_info) << "PlsEvac Limit reached at " << pd.Run().PlsBagEvacVc.Get() << endmsg;
         pd.Orders().Qcollect(0.0f);
         return 1;
      }
   }

   if (pd.TrimaSet().Cassette.Get() == cassette::RBCPLS)
   {
      if  (  pd.Run().PlsBagEvacVpls.Get() >=  CobeConfig::data().EvacuationBlackplsExtra + pd.Run().PlsBagEvacVin_done.Get()  )
      {
         pd.Orders().Qplasma(0.0f);
         DataLog(log_level_proc_info) << "RBC air processing: Limit reached at " << pd.Run().PlsBagEvacVpls.Get() << endmsg;
         DataLog(log_level_proc_info) << "RBC air processing: Plasma pump stopped " << endmsg;
         return 1;
      }
   }




   return NO_TRANSITION;
}

int PlsEvacFinished::preEnter ()
{


   return NORMAL;
}

int PlsEvacFinished::postExit ()
{
   ProcData pd;
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(0.0f);

   return NORMAL;
}

void PlsEvacFinished::copyOver (const PlsEvacFinished&)
{}

void PlsEvacFinished::cleanup ()
{}



/////////////////////////////////////////////////////////////////////////////

/* FORMAT HASH b098a0d0ef9a7132ba28a399d2313a52 */
