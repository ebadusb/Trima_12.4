#include "xif_cds.h"

#include "trima_datalog_levels.h"




External_IF_CDS::External_IF_CDS(Role role)
   : MultWriteDataStore("External_IF_CDS", role)
{
   vistaStatusReady.Register    (this, NO_PFR,      false);
   vistaConnected.Register      (this, NO_PFR,      false);
   vistaHasConnected.Register   (this, PFR_RECOVER, false);
   vistaInControl.Register      (this, NO_PFR,      false);
   pfrComplete.Register         (this, NO_PFR,      false);
   broadcastComplete.Register   (this, NO_PFR,      false);
   donorDownloaded.Register     (this, PFR_RECOVER, false);
   loadSetReady.Register        (this, PFR_RECOVER, false);
   cassetteLowered.Register     (this, PFR_RECOVER, false);
   donorCommitted.Register      (this, PFR_RECOVER, false);
   safetyStarted.Register       (this, NO_PFR,      false);

   replyToAddress.Register      (this, NO_PFR,      0);

   firstSequenceNumber.Register (this, PFR_RECOVER, -1);   // Initialize to illegal value to test against for PFR
}



bool External_IF_CDS::restrictedAccess (unsigned int access) const
{
   if ( (access & XIF_CASSETTE_LOWERED) && cassetteLowered.Get())
   {
      return true;
   }
   else if ( (access & XIF_DONOR_COMMITTED) && donorCommitted.Get())
   {
      return true;
   }
   else if ( (access & XIF_DONOR_DOWNLOADED) && donorDownloaded.Get())
   {
      return true;
   }
   else
      return false;
}

/* FORMAT HASH 396235268aa2bd2b30843c9cfa88c738 */
