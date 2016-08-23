/*****************************************************************************\
**
** GeneratorSystemStates.h
**
** Provides the implementation for the system state generator.
**
\*****************************************************************************/

#ifndef GENERATORSYSTEMSTATES_H
#define GENERATORSYSTEMSTATES_H

/* Includes ******************************************************************/

#include <vector>

#include "SpoofGenerator.h"
#include "SpoofTypes.h"
#include "SpoofUtil.h"
#include "LogReaderUtil.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/* Classes *******************************************************************/

class GeneratorSystemStates : public ImportGenerator {
public:
	typedef ProcessGetPeriodicLogs::LogDataVector SystemStateValues;

	/**
	 *
	 * GeneratorSystemStates
	 *
	 * Constructor
	 *
	 */ 
	GeneratorSystemStates(void);

	/**
	 *
	 * ~GeneratorSystemStates
	 *
	 * Destructor
	 *
	 */ 
	virtual ~GeneratorSystemStates(void);

	/**
	 *
	 * Register
	 *
	 * Registers the necessary handlers with the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Register(DECODER::LogReader &reader);

	/**
	 *
	 * Register
	 *
	 * Unregisters the handlers from the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Unregister(DECODER::LogReader &reader);

	/**
	 *
	 * PreImportProcess
	 *
	 * Perform the procedures before the import process.
	 *
	 */
	virtual void PreImportProcess(void);

	/**
	 *
	 * PostImportProcess
	 *
	 * Perform the procedures after the import process.
	 *
	 */
	virtual void PostImportProcess(void);

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
	SystemStateValues::const_iterator GetSystemStateValuesBegin(void) const;

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
	SystemStateValues::const_iterator GetSystemStateValuesEnd(void) const;

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
	Uint32 GetTimeForState(const String &state) const;

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
	String GetStateForTime(Uint32 time) const;

private:
	// Record data collected.
	SystemStateValues mSystemStateValues;

	// Record handler.
	ProcessGetPeriodicLogs mProcessGetSystemStates;
};

} // namespace BUILDER

#endif // GENERATORREDGREEN_H

