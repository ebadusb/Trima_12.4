/*****************************************************************************\
**
** DatabaseConverter.h
**
** Extracts the datalog data into an database using ADO.
**
\*****************************************************************************/

#ifndef DATABASECONVERTER_H
#define DATABASECONVERTER_H

/* Includes ******************************************************************/

#include "LogReader.h"
#include "LogExtractor.h"
#include "DatabaseConverterUtil.h"

namespace CONVERT {

/* Constants *****************************************************************/

/* Types *********************************************************************/

/* Classes *******************************************************************/

class DatabaseConverter : public DECODER::LogExtractor {
public:

	enum {
		kResultNone = 0,
		kResultDatalogFileAlreadyOpen,
		kResultDatalogFileNotOpen,
		kResultDatalogFileOpenFailed,
		kResultDatalogInvalidHeaderType,
		kResultDatalogInvalidVersion,
		kResultDatalogUnexpectedEOF,
		kResultDatalogUnknownRecordType,
		kResultDatabaseConnectFailed,
		kResultDatabaseDisconnectFailed,
		kResultDatabaseQueryFailed,
		kResultProductUnregistered
	};

	/**
	 *
	 * DatabaseConverter
	 *
	 * Constructor
	 *
	 */
	DatabaseConverter(const DECODER::String &file);

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
	 * Unregister
	 *
	 * Unregisters the handlers from the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Unregister(DECODER::LogReader &reader);

	/**
	 *
	 * PreProcess
	 *
	 * Perform the procedures before the import process.
	 *
	 */
	virtual void PreProcess(void);

	/**
	 *
	 * PostProcess
	 *
	 * Perform the procedures after the import process.
	 *
	 */
	virtual void PostProcess(void);

	/**
	 *
	 * ExtractLogData
	 *
	 * Utility function that extracts the datalog data into the database.
	 *
	 * @param logFile The datalog filename.
	 * @param datfile The database filename.
	 *
	 * @return The results. 
	 *
	 */
	static DECODER::Result ExtractLogData(const DECODER::String &logFile, const DECODER::String &datfile);

private:
	DatalogDatabaseConverter mDatalogDatabaseConverter;
};

} // namespace CONVERT

#endif // DATABASECONVERTER_H

