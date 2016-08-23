/*****************************************************************************\
**
** GeneratorRecoveryStates.cpp
**
** Provides the implementation for the red green code generator.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <iostream>

#include "GeneratorRecoveryStates.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/**
 *
 * GeneratorRecoveryStates
 *
 * Constructor
 *
 */ 
GeneratorRecoveryStates::GeneratorRecoveryStates(void) :
		mProcessGetRecoveryStates("Recovery", mRecoveryStateVector)
{
}

/**
 *
 * ~GeneratorRecoveryStates
 *
 * Destructor
 *
 */ 
GeneratorRecoveryStates::~GeneratorRecoveryStates(void)
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
void GeneratorRecoveryStates::Register(DECODER::LogReader &reader)
{
	reader.RegisterPeriodicSetItem(mProcessGetRecoveryStates);
	reader.RegisterPeriodicLogOutput(mProcessGetRecoveryStates);
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
void GeneratorRecoveryStates::Unregister(DECODER::LogReader &reader)
{
	reader.UnregisterPeriodicSetItem(mProcessGetRecoveryStates);
	reader.UnregisterPeriodicLogOutput(mProcessGetRecoveryStates);
}

/**
 *
 * PreImportProcess
 *
 * Perform the procedures before the import process.
 *
 */
void GeneratorRecoveryStates::PreImportProcess(void)
{
	mRecoveryStateVector.clear();
}

/**
 *
 * PostImportProcess
 *
 * Perform the procedures after the import process.
 *
 */
void GeneratorRecoveryStates::PostImportProcess(void)
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
GeneratorRecoveryStates::RecoveryStateVector::const_iterator GeneratorRecoveryStates::GetSystemStateValuesBegin(void) const
{
	return mRecoveryStateVector.begin();
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
GeneratorRecoveryStates::RecoveryStateVector::const_iterator GeneratorRecoveryStates::GetSystemStateValuesEnd(void) const
{
	return mRecoveryStateVector.end();
}

} // namespace BUILDER
