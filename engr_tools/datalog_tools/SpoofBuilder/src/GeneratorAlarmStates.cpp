/*****************************************************************************\
**
** GeneratorAlarmStates.cpp
**
** Provides the implementation for the red green code generator.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <iostream>

#include "GeneratorAlarmStates.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/**
 *
 * GeneratorAlarmStates
 *
 * Constructor
 *
 */ 
GeneratorAlarmStates::GeneratorAlarmStates(void) :
		mProcessGetAlarmStates("AlarmStateFlag", mAlarmStateVector)
{
}

/**
 *
 * ~GeneratorAlarmStates
 *
 * Destructor
 *
 */ 
GeneratorAlarmStates::~GeneratorAlarmStates(void)
{
}

/**
 *
 * Register
 *
 * Registers the necessary handlers with the reader.
 *
 * @param reader The file reader.
 *
 */
void GeneratorAlarmStates::Register(DECODER::LogReader &reader)
{
	reader.RegisterPeriodicSetItem(mProcessGetAlarmStates);
	reader.RegisterPeriodicLogOutput(mProcessGetAlarmStates);
}

/**
 *
 * Register
 *
 * Unregisters the handlers from the reader.
 *
 * @param reader The file reader.
 *
 */
void GeneratorAlarmStates::Unregister(DECODER::LogReader &reader)
{
	reader.UnregisterPeriodicSetItem(mProcessGetAlarmStates);
	reader.UnregisterPeriodicLogOutput(mProcessGetAlarmStates);
}

/**
 *
 * PreImportProcess
 *
 * Perform the procedures before the import process.
 *
 */
void GeneratorAlarmStates::PreImportProcess(void)
{
	mAlarmStateVector.clear();
}

/**
 *
 * PostImportProcess
 *
 * Perform the procedures after the import process.
 *
 */
void GeneratorAlarmStates::PostImportProcess(void)
{
}

/**
 *
 * GetSystemStateValuesBegin
 *
 * Gets an iterator to the system state values. The iterator will point
 * to the beginning of the set. The pair contains state and the time.
 *
 * @return The iterator to the beginning of the values.
 *
 */
GeneratorAlarmStates::AlarmStateVector::const_iterator GeneratorAlarmStates::GetSystemStateValuesBegin(void) const
{
	return mAlarmStateVector.begin();
}

/**
 *
 * GetSystemStateValuesEnd
 *
 * Gets an iterator to the system state values. The iterator will point
 * to the end of the set. The pair contains state and the time.
 *
 * @return The iterator to the end of the values.
 *
 */
GeneratorAlarmStates::AlarmStateVector::const_iterator GeneratorAlarmStates::GetSystemStateValuesEnd(void) const
{
	return mAlarmStateVector.end();
}

} // namespace BUILDER
