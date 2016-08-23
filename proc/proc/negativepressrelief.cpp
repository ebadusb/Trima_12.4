#include "negativepressrelief.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "software_cds.h"     // Get Optional sw.dat settings

DEFINE_STATE(NegativePressRelief);

const float npr_Qret         = 90.0f;
const float high_nominal_aps = -45.0f;
const float low_nominal_aps  = -55.0f;


NegativePressRelief::NegativePressRelief()
   : StateAbs(),
     _NegativePressureReliefAlarm(FAILURE_TO_RELIEVE_PRESSURE),
     _TestVolume(0.0f),
     _transition(false),
     _direction(1.0f),
     _lastCheckAPS(0.0f),
     _slowingFactor(1.0f),
     _entryAPS(0.0f)
{}

NegativePressRelief::NegativePressRelief(const NegativePressRelief& state)
   : StateAbs(state),
     _NegativePressureReliefAlarm(FAILURE_TO_RELIEVE_PRESSURE),
     _TestVolume(0.0f),
     _transition(false),
     _direction(1.0f),
     _lastCheckAPS(0.0f),
     _slowingFactor(1.0f),
     _entryAPS(0.0f)

{}

NegativePressRelief::~NegativePressRelief()
{}

int NegativePressRelief::transitionStatus ()
{
   ProcData pd;

   if (    _transition
           && (_NegativePressureReliefAlarm.getState() == CLEARED )
           && !pd.AlarmActive()
           )
   {
      pd.Orders().UnlockDoor();
      DataLog(log_level_proc_disposable_test_info) << "NegativePressRelief ( APS=" << pd.Status().APS() << "mmHg) volm processes = "  << _TestVolume << endmsg;
      return 1;  // goto AC Connect
   }

   return NO_TRANSITION;
}

int NegativePressRelief::preProcess ()
{
   ProcData pd;
   float    apsReading = pd.Status().APS();
   //
   // Command all other pumps off ...
   //

   // ideal case:
   if ((apsReading  > low_nominal_aps) && (apsReading < high_nominal_aps))
   {
      DataLog(log_level_proc_disposable_test_info) << "Ideal range APS=" << pd.Status().APS()  << endmsg;
      _transition = true;
   }
   if ((_lastCheckAPS < low_nominal_aps) &&  (apsReading > high_nominal_aps))
   {
      checkDirection();
   }
   if ((apsReading < low_nominal_aps) &&  (_lastCheckAPS > high_nominal_aps))
   {
      checkDirection();
   }

   if (!_transition)
   {
      pd.Orders().Qrp(npr_Qret * _direction * _slowingFactor);
   }

   _lastCheckAPS = apsReading;

   return NORMAL;
}

int NegativePressRelief::postProcess ()
{


   ProcData pd;

   checkDirection();

   _TestVolume +=  (pd.Status().ReturnPump.dV() * _direction );

   //
   // If the pressure wasn't relieved after the
   //  pump has processed 50 mls ...
   //
   if ( _TestVolume >= CobeConfig::data().DispTestVolumeFail  )
   {
      DataLog(log_level_proc_disposable_test_error) << " Exceeded test Volume: volume change = " << _TestVolume << "mls" << endmsg;

      //
      // Set the alarm ...
      //
      _NegativePressureReliefAlarm.setAlarm();
      DataLog(log_level_proc_disposable_test_info) << "Set negative pressure relief alarm" << FAILURE_TO_RELIEVE_PRESSURE << endmsg;

      //
      // Reset the accumulated volume ...
      //
      _TestVolume = 0.0f;

   }

   // we should end disp test with slight neg pressure on the donor line.

   return NORMAL;
}

int NegativePressRelief::preEnter ()
{

   ProcData pd;

   pd.Orders().Qin(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);

   _entryAPS     = pd.Status().APS();
   _lastCheckAPS = _entryAPS;

   _TestVolume   = 0.0f;

   if ( pd.Status().APS() > -50.0f ) _direction = -1.0f;
   else _direction =  1.0f;

   _slowingFactor = 1.0f;

   if ((_entryAPS  > low_nominal_aps) && (_entryAPS < high_nominal_aps))
      _transition = true;

   DataLog(log_level_proc_disposable_test_info) << "aps at entry: " << _entryAPS << endmsg;
   DataLog(log_level_proc_disposable_test_info) << "return dir: "   << _direction << endmsg;

   return NORMAL;
}

void NegativePressRelief::checkDirection ()
{

   ProcData pd;
   float    old_direction = _direction;

   if ( pd.Status().APS() > -50.0f ) _direction = -1.0f;
   else _direction =  1.0f;


   if ( (old_direction < 0.0f) &&  (_direction > 0.0f) )
   {

      _slowingFactor = _slowingFactor * 0.5f;
      DataLog(log_level_proc_disposable_test_info) << "change in Qrp direction: "   << _direction << endmsg;
      DataLog(log_level_proc_disposable_test_info) << "APS at change: "   << pd.Status().APS() << endmsg;
      DataLog(log_level_proc_disposable_test_info) << "Slowed Qrp by : "   << _slowingFactor << endmsg;

   }

}


int NegativePressRelief::postExit ()
{
   ProcData pd;

   //
   // Stop the pump ...
   //
   pd.Orders().Qrp(0.0f);

   DataLog(log_level_proc_disposable_test_info) << "aps at exit: "
                                                << pd.Status().APS()
                                                << ", Last APS check was: "
                                                << _lastCheckAPS
                                                << endmsg;
   //
   // Delete my messages ...
   //
   cleanup();


   _slowingFactor = 1.0f;
   _transition    = false;

   return NORMAL;
}

void NegativePressRelief::copyOver (const NegativePressRelief&)
{}

void NegativePressRelief::cleanup ()
{}

/* FORMAT HASH 29c91c2ae5ee75d6b0fec4d818584ccf */
