#include "procdetfactory.h"

#include "procdetpltplsrbc.h"
#include "procdetrbcpls.h"

namespace ProcDetFactory
{

procedureDetail* CreateNewProcedure (cassette::cassetteType currentCassette, bool isCollectingPlt, PIRType pir)
{
   // DataLog(log_level_predict_info) << __FUNCTION__  << endmsg;

   cassette::cassetteType recommendedCassette = currentCassette;
   if ((currentCassette == cassette::NOT_LOADED) || (currentCassette == cassette::UNKNOWN))
      recommendedCassette = isCollectingPlt ?  cassette::PLTPLSRBC :  cassette::RBCPLS ;

   switch (recommendedCassette)
   {
      case cassette::PLTPLSRBC : return new procedureDetailPltPlsRBC(pir);
      case cassette::RBCPLS    : return new procedureDetailRBCPls;
      default                  : return new procedureDetailPltPlsRBC(pir);
   }

   return NULL;
}



cassette::cassetteType calcRecommendedCassette (cassette::cassetteType currentCassette,
                                                const procedureDetail& proc)
{
   // DataLog(log_level_predict_info) << __FUNCTION__  << endmsg;

   if ((currentCassette == cassette::NOT_LOADED) || (currentCassette == cassette::UNKNOWN))
      return proc.getRecommendedCassette();

   return currentCassette;
}

} // namespace ProcDetFactory

/* FORMAT HASH b3be0f973205d885fea8be290d1357af */
