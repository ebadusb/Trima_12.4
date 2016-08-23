/*****************************************************************************\
**
** SpoofGenerator.h
**
** Spoof generator interface.
**
\*****************************************************************************/

#ifndef SPOOFGENERATOR_H
#define SPOOFGENERATOR_H

/* Includes ******************************************************************/

#include "LogReader.h"
#include "SourceStream.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/* Classes *******************************************************************/

class ImportGenerator {
public:

	/**
	 *
	 * Register
	 *
	 * Registers the necessary handlers with the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Register(DECODER::LogReader &reader) = 0;

	/**
	 *
	 * Unregister
	 *
	 * Unregisters the handlers from the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Unregister(DECODER::LogReader &reader) = 0;

	/**
	 *
	 * PreImportProcess
	 *
	 * Perform the procedures before the import process.
	 *
	 */
	virtual void PreImportProcess(void) = 0;

	/**
	 *
	 * PostImportProcess
	 *
	 * Perform the procedures after the import process.
	 *
	 */
	virtual void PostImportProcess(void) = 0;
};

class ExportGenerator {
public:

	/**
	 *
	 * Register
	 *
	 * Registers the output stream with the generator.
	 *
	 * @param stream The output stream.
	 *
	 */
	virtual void Register(SourceStream &stream) = 0;

	/**
	 *
	 * Unregister
	 *
	 * Unregisters the output stream with the generator.
	 *
	 * @param stream The output stream.
	 *
	 */
	virtual void Unregister(SourceStream &stream) = 0;

	/**
	 *
	 * PreExportProcess
	 *
	 * Perform the procedures before the export process.
	 *
	 */
	virtual void PreExportProcess(void) = 0;

	/**
	 *
	 * PostExportProcess
	 *
	 * Perform the procedures after the export process.
	 *
	 */
	virtual void PostExportProcess(void) = 0;
};

} // namespace BUILDER

#endif // SPOOFGENERATOR_H

