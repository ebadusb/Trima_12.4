/*****************************************************************************\
**
** DatabaseConverter.cpp
**
** Extracts the datalog data into an database using ADO.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include "DatabaseConverter.h"

namespace CONVERT {

/* Functions *****************************************************************/

/**
 *
 * DatabaseConverter
 *
 * Constructor
 *
 */
DatabaseConverter::DatabaseConverter(const DECODER::String &file) : mDatalogDatabaseConverter(file)
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
void DatabaseConverter::Register(DECODER::LogReader &reader)
{
	reader.RegisterLogInfo(mDatalogDatabaseConverter);
	reader.RegisterLogHeader(mDatalogDatabaseConverter);
	reader.RegisterTaskCreated(mDatalogDatabaseConverter);
	reader.RegisterTaskDeleted(mDatalogDatabaseConverter);
	reader.RegisterTimeStamp(mDatalogDatabaseConverter);
	reader.RegisterEndOfFile(mDatalogDatabaseConverter);
	reader.RegisterPeriodicSet(mDatalogDatabaseConverter);
	reader.RegisterPeriodicLogOutput(mDatalogDatabaseConverter);
	reader.RegisterBinary(mDatalogDatabaseConverter);
	reader.RegisterLogLevel(mDatalogDatabaseConverter);
	reader.RegisterStartNetCon(mDatalogDatabaseConverter);
	reader.RegisterPeriodicSetItem(mDatalogDatabaseConverter);
	reader.RegisterPrintOutput(mDatalogDatabaseConverter);
	reader.RegisterStreamOutput(mDatalogDatabaseConverter);
}

/**
 *
 * Unregister
 *
 * Unregisters the handlers from the reader.
 *
 * @param reader The file reader.
 *
 */
void DatabaseConverter::Unregister(DECODER::LogReader &reader)
{
	reader.UnregisterLogInfo(mDatalogDatabaseConverter);
	reader.UnregisterLogHeader(mDatalogDatabaseConverter);
	reader.UnregisterTaskCreated(mDatalogDatabaseConverter);
	reader.UnregisterTaskDeleted(mDatalogDatabaseConverter);
	reader.UnregisterTimeStamp(mDatalogDatabaseConverter);
	reader.UnregisterEndOfFile(mDatalogDatabaseConverter);
	reader.UnregisterPeriodicSet(mDatalogDatabaseConverter);
	reader.UnregisterPeriodicLogOutput(mDatalogDatabaseConverter);
	reader.UnregisterBinary(mDatalogDatabaseConverter);
	reader.UnregisterLogLevel(mDatalogDatabaseConverter);
	reader.UnregisterStartNetCon(mDatalogDatabaseConverter);
	reader.UnregisterPeriodicSetItem(mDatalogDatabaseConverter);
	reader.UnregisterPrintOutput(mDatalogDatabaseConverter);
	reader.UnregisterStreamOutput(mDatalogDatabaseConverter);
}

/**
 *
 * PreProcess
 *
 * Perform the procedures before the import process.
 *
 */
void DatabaseConverter::PreProcess(void)
{
	mDatalogDatabaseConverter.ConnectToDatabase();
}

/**
 *
 * PostProcess
 *
 * Perform the procedures after the import process.
 *
 */
void DatabaseConverter::PostProcess(void)
{
	mDatalogDatabaseConverter.DisconnectFromDatabase();
}

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
DECODER::Result DatabaseConverter::ExtractLogData(const DECODER::String &logFile, const DECODER::String &datfile)
{
	DECODER::Result results = kResultNone;
	DECODER::LogReader reader;
	DatabaseConverter process(datfile);

	if (!process.mDatalogDatabaseConverter.ConnectToDatabase() && (results == kResultNone)) {
		results = kResultDatabaseConnectFailed;
	}

	if (results == kResultNone) {
		process.Register(reader);
		results = reader.Open(logFile.c_str());
		while (results == DECODER::LogReader::kResultNone && !reader.IsEndOfFile()) {
			results = reader.Read(1);
		}
		reader.Close();
		process.Unregister(reader);
	}

	if (!process.mDatalogDatabaseConverter.DisconnectFromDatabase() && (results == kResultNone)) {
		results = kResultDatabaseDisconnectFailed;
	}

	return results;
}

} // namespace CONVERT


