/*****************************************************************************\
**
** GeneratorSystemStates.cpp
**
** Provides the implementation for the red green code generator.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <iostream>

#include "GeneratorSystemStates.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/**
 *
 * GeneratorSystemStates
 *
 * Constructor
 *
 */ 
GeneratorSystemStates::GeneratorSystemStates(void) :
		mProcessGetSystemStates("SystemState", mSystemStateValues)
{
}

/**
 *
 * ~GeneratorSystemStates
 *
 * Destructor
 *
 */ 
GeneratorSystemStates::~GeneratorSystemStates(void)
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
void GeneratorSystemStates::Register(DECODER::LogReader &reader)
{
	reader.RegisterPeriodicSetItem(mProcessGetSystemStates);
	reader.RegisterPeriodicLogOutput(mProcessGetSystemStates);
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
void GeneratorSystemStates::Unregister(DECODER::LogReader &reader)
{
	reader.UnregisterPeriodicSetItem(mProcessGetSystemStates);
	reader.UnregisterPeriodicLogOutput(mProcessGetSystemStates);
}

/**
 *
 * PreImportProcess
 *
 * Perform the procedures before the import process.
 *
 */
void GeneratorSystemStates::PreImportProcess(void)
{
	mSystemStateValues.clear();
}

/**
 *
 * PostImportProcess
 *
 * Perform the procedures after the import process.
 *
 */
void GeneratorSystemStates::PostImportProcess(void)
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
GeneratorSystemStates::SystemStateValues::const_iterator GeneratorSystemStates::GetSystemStateValuesBegin(void) const
{
	return mSystemStateValues.begin();
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
GeneratorSystemStates::SystemStateValues::const_iterator GeneratorSystemStates::GetSystemStateValuesEnd(void) const
{
	return mSystemStateValues.end();
}

/**
 *
 * GetTimeForState
 *
 * Gets the time for the given state.
 *
 * @param state The system state.
 *
 * @return The time the state occured. Retuns -1 if not found.
 *
 */
Uint32 GeneratorSystemStates::GetTimeForState(const String &state) const
{
	SystemStateValues::const_iterator iter = mSystemStateValues.begin();
	SystemStateValues::const_iterator end = mSystemStateValues.end();

	for (; iter != end; ++iter) {
		if ((*iter).second == state) {
			return (*iter).first;
		}
	}

	return -1;
}

/**
 *
 * GetStateForTime
 *
 * Gets the state for the given time.
 *
 * @param time The state time.
 *
 * @return The state for the time. Retuns empty string if not found.
 *
 */
String GeneratorSystemStates::GetStateForTime(Uint32 time) const
{
	SystemStateValues::const_iterator iter = mSystemStateValues.begin();
	SystemStateValues::const_iterator end = mSystemStateValues.end();

	for (; iter != end; ++iter) {
		if ((*iter).first == time) {
			return (*iter).second;
		}
	}

	return "";
}

} // namespace BUILDER
