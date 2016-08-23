/*****************************************************************************\
**
** GeneratorAlarmStates.h
**
** Provides the implementation for the system state generator.
**
\*****************************************************************************/

#ifndef GENERATORALARMSTATES_H
#define GENERATORALARMSTATES_H

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

class GeneratorAlarmStates : public ImportGenerator {
public:
	typedef ProcessGetPeriodicLogs::LogDataVector AlarmStateVector;

	/**
	 *
	 * GeneratorAlarmStates
	 *
	 * Constructor
	 *
	 */ 
	GeneratorAlarmStates(void);

	/**
	 *
	 * ~GeneratorAlarmStates
	 *
	 * Destructor
	 *
	 */ 
	virtual ~GeneratorAlarmStates(void);

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
	AlarmStateVector::const_iterator GetSystemStateValuesBegin(void) const;

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
	AlarmStateVector::const_iterator GetSystemStateValuesEnd(void) const;

private:
	// Record data collected.
	AlarmStateVector mAlarmStateVector;

	// Record handler.
	ProcessGetPeriodicLogs mProcessGetAlarmStates;
};

} // namespace BUILDER

#endif // GENERATORALARMSTATES_H

