/*****************************************************************************\
**
** SpoofRedGreen.h
**
** Provides the implementaion for generated red green spoofs.
**
\*****************************************************************************/

#ifndef SPOOFREDGREEN_H
#define SPOOFREDGREEN_H

/* Includes ******************************************************************/

#include <sstream>

#include "SpoofAssignment.h"
#include "timermessage.h"

namespace SPOOFNAME {

/* Constants *****************************************************************/

static const int kRedIndex = 0;
static const int kGreenIndex = 1;
static const unsigned long kTimerInterval = 1000;

/* Classes *******************************************************************/

class SpoofRedGreen : public SpoofAssignment {
public:
	explicit SpoofRedGreen(SpooferAPI &api);
	virtual ~SpoofRedGreen(void);

	virtual void Initialize(void);
	virtual void Execute(void);
	virtual void Finalize(void);

protected:
	 void SpoofData(void);

private:
    TimerMessage *mTimerMessageP;

	 unsigned int mRedGreenValuesIndex;
};

/**
 * 
 * SpoofRedGreen
 * 
 * Constructor
 * 
 * @param api The Spoof API for executing the spoof.
 * 
 */
SpoofRedGreen::SpoofRedGreen(SpooferAPI &api) : SpoofAssignment(api),
	   mTimerMessageP(NULL), 
	   mRedGreenValuesIndex(0) 
{
}

/** 
 * 
 * ~SpoofRedGreen
 * 
 * Destructor
 * 
 */
SpoofRedGreen::~SpoofRedGreen(void) 
{ 
	 this->Finalize(); 
}

/**
 * 
 * Initialize
 * 
 * Initializes the task. This function is called before executing the spoof.
 * This function sets up the timer and resources.
 * 
 */
void SpoofRedGreen::Initialize(void)
{
	 if (mTimerMessageP == NULL) {
		  mTimerMessageP = new TimerMessage(
				kTimerInterval, 
            Callback<SpoofRedGreen>(this, &SpoofRedGreen::SpoofData),
            TimerMessage::DISARMED);
	 }

	 mRedGreenValuesIndex = 0;
}

/**
 * 
 * Execute
 * 
 * Execute the spoof by staring the timer. 
 * 
 */
void SpoofRedGreen::Execute(void)
{
	mTimerMessageP->armTimer(TimerMessage::ARMED);
}

/**
 * 
 * Finalize
 * 
 * Finalizes the task. This function is called after executing the spoof.
 * This function stops the timer and cleans up resources.
 * 
 */
void SpoofRedGreen::Finalize(void)
{
	 if (mTimerMessageP->timerArmed() == TimerMessage::ARMED) {
		  mTimerMessageP->armTimer(TimerMessage::DISARMED);
	 }

	 delete mTimerMessageP;
	 mTimerMessageP = NULL;
	 mRedGreenValuesIndex = 0;
}

/**
 * 
 * SpoofData
 * 
 * Handles the timer events. This function sets the red/green values from the
 * array.  
 * 
 */
void SpoofRedGreen::SpoofData(void) 
{
	 if (mRedGreenValuesIndex < kRedGreenValuesCount) {
		  const int *kRGValue = kRedGreenValuesArray[mRedGreenValuesIndex];

		  std::ostringstream redStr;
		  std::ostringstream greenStr;

		  redStr << kRGValue[kRedIndex];
  		  greenStr << kRGValue[kGreenIndex];

		  if ((mRedGreenValuesIndex % 10) == 0) {
				DataLog(log_level_spoofer_info) << "Spoofing"
					 << " Index:" << mRedGreenValuesIndex
					 << " Red:" << redStr.str().c_str() 
					 << " Green:" << greenStr.str().c_str()
					 << endmsg;
		  }

		  API.spoofData("CHwStates", "red", redStr.str().c_str());
		  API.spoofData("CHwStates", "green", greenStr.str().c_str());

		  ++mRedGreenValuesIndex;
	 }
	 else {
		  mTimerMessageP->armTimer(TimerMessage::DISARMED);
		  API.unspoofData("CHwStates", "red");
		  API.unspoofData("CHwStates", "green");
	 }
}

}; // namespace SPOOFNAME

#endif SPOOFREDGREEN_H
