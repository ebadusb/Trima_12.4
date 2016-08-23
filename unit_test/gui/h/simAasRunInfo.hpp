/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simAasRunInfo.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This file contains overridden data for the GUI aasRunInfo.
 *
 *******************************************************************************/

#ifdef __AAS_RUN_INFO_SIMNT_HEADER_EXTRAS__
enum
{
	AAS_INFO_NO_ADDITION = 0,
	AAS_INFO_RAS_SINGLE = 1,
	AAS_INFO_RAS_DOUBLE = 2,
	AAS_INFO_PAS=4
};

float pasTargeted;
float rasTargeted;
float percentCompletion;
void setAasInfo(unsigned int additionOptions, float pctComplete=0.6f);
void setAasInfo(float pasTarg, float rasTarg, float pctComplete=0.6f);
#endif

/*********************************************************************************/

#ifdef __AAS_RUN_INFO_SIMNT_CONSTRUCTOR_EXTRAS__
pasTargeted(200.0),
rasTargeted(200.0),
percentCompletion(0.6),
#endif

/*********************************************************************************/

#ifdef __AAS_RUN_INFO_SIMNT_FUNCTION_IMPLEMENTATIONS__
#define PAS_PRIME_VOLUME 10.0f
float AasRunInfo::getPasNeeded() { return pasTargeted; }
float AasRunInfo::getPasTotalVolNeeded() { return getPasNeeded()+PAS_PRIME_VOLUME; }
float AasRunInfo::getRasNeeded() { return rasTargeted; }
float AasRunInfo::getAasNeeded() { return getPasNeeded() + getRasNeeded(); }
bool AasRunInfo::isRasProcedure(bool ignoreAlarms=false) { return pasTargeted > 0.0f; }
bool AasRunInfo::isPasProcedure(bool ignoreAlarms=false) { return rasTargeted > 0.0f; }
float AasRunInfo::getAasAdded() { return percentCompletion * getAasNeeded(); }
bool AasRunInfo::readyForAddition() { return isPasProcedure() || isRasProcedure(); }
bool AasRunInfo::productTargetedAsDrbcPtf() { return rasTargeted > 150.0; }
bool AasRunInfo::rasBlockedDueToCpsAlarms() { return false; }
bool AasRunInfo::mssSetupComplete() { return true; }

void AasRunInfo::setAasInfo(unsigned int additionOptions, float pctComplete=0.6f)
{
	if (additionOptions & AAS_INFO_RAS_SINGLE)	rasTargeted = 100.0f;
	else if (additionOptions & AAS_INFO_RAS_DOUBLE)	rasTargeted = 200.0f;
	else rasTargeted = 0.0f;

	if (additionOptions & AAS_INFO_PAS)	pasTargeted = 84.0f;
	else pasTargeted = 0.0f;

	percentCompletion = pctComplete;
}

void AasRunInfo::setAasInfo(float pasTarg, float rasTarg, float pctComplete=0.6f)
{
	pasTargeted = pasTarg;
	rasTargeted = rasTarg;
	percentCompletion = pctComplete;
}

#endif

