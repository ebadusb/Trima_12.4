#ifndef MSS_COMMON_H
#define MSS_COMMON_H

////////////////////////////////////////////////////////////////////////////////////////////
/*
 *   this contains some common functions/classes that are
 *   used in metered storgae code
 */
////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// is this a MSS PLT setup
//    the ignoreAlarms = true;  will NOT consider MSS cancelling alarms in the decision
//    the ignoreAlarms = false;  will consider MSS cancelling alarms in the decision
bool readyForPLTMSS (bool ignoreAlarms = false);

///////////////////////////////////////////////////////////////////////////////////////
// is this a MSS RBC setup
//    the ignoreAlarms = true;  will NOT consider MSS cancelling alarms in the decision
//    the ignoreAlarms = false;  will consider MSS cancelling alarms in the decision
bool readyForRBCMSS (bool ignoreAlarms = false);
bool RASBlockedDueToCPSalarms ();


///////////////////////////////////////////////////////////////////////////////////////
// this algorithm will calculate the correct PAS required for a given collect volume.
// it attempts to hold Final Concentration constant
// This function returns the adjusted SS volm in mL as a convienence.
// This function sets the CDS "PltSSVolumeFactor" which adjust the metered SS all around.
// This fuction pulls all input data from CDS's
//     the peekOnly param is just to see what the PAS volume (returned)
//      is without setting anything in CDS's
float getAdjustedPAS (bool peekOnly = false);


//////////////////////////////////////////////////////////////////////////////////////////
class GuiAasUpdater
{
private:
   static int _lastAasUpdateValue;
   static int _loggingCount;

public:
   static void sendUpdate ();
};

#endif

/* FORMAT HASH 37012bff4e0f4188d9a0548b854811b7 */
