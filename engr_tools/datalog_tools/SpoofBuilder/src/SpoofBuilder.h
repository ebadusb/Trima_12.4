/*****************************************************************************\
**
** SpoofBuilder.h
**
** Spoof builder implementation.
**
\*****************************************************************************/

#ifndef SPOOFBUILDER_H
#define SPOOFBUILDER_H

/* Includes ******************************************************************/

#include <set>

#include "LogReader.h"
#include "SpoofGenerator.h"
#include "SpoofTypes.h"
#include "SourceStream.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/* Classes *******************************************************************/

class SpoofBuilder {
public:
	typedef DECODER::Result Result;

	enum {
		kResultNone = DECODER::LogReader::kResultNone,
		kResultDLogFileAlreadyOpen = DECODER::LogReader::kResultFileAlreadyOpen,
		kResultDLogFileNotOpen = DECODER::LogReader::kResultFileNotOpen,
		kResultDLogFileOpenFailed = DECODER::LogReader::kResultFileOpenFailed,
		kResultDLogInvalidHeaderType = DECODER::LogReader::kResultInvalidHeaderType,
		kResultDLogInvalidVersion = DECODER::LogReader::kResultInvalidVersion,
		kResultDLogUnexpectedEOF = DECODER::LogReader::kResultUnexpectedEOF,
		kResultDLogUnknownRecordType = DECODER::LogReader::kResultUnknownRecordType,
		kResultDLogUnknownResult = DECODER::LogReader::kResultLast,
		kResultTemplateReadFailed,
		kResultSourceWriteFailed
	};

	struct SpoofConfig {
		String templateFilename;
	};

	struct SpoofInfo {
		String name;
		String comment;
		String help;
		Int32 timeout;
	};

	/**
	 *
	 * SpoofBuilder
	 *
	 * Constructor
	 *
	 */
	SpoofBuilder(void);

	/**
	 *
	 * ~SpoofBuilder
	 *
	 * Destructor
	 *
	 */
	~SpoofBuilder(void);

	/**
	 *
	 * IncludeImportGenerator
	 *
	 * Includes the given generator in the read process.
	 *
	 * @param generator The generator to include.
	 *
	 */
	void IncludeImportGenerator(ImportGenerator &generator);

	/**
	 *
	 * ExcludeImportGenerator
	 *
	 * Excludes the given generator from the read process.
	 *
	 * @param generator The generator to include.
	 *
	 */
	void ExcludeImportGenerator(ImportGenerator &generator);

	/**
	 *
	 * IncludeExportGenerator
	 *
	 * Includes the given generator in the read process.
	 *
	 * @param generator The generator to include.
	 *
	 */
	void IncludeExportGenerator(ExportGenerator &generator);

	/**
	 *
	 * ExcludeExportGenerator
	 *
	 * Excludes the given generator from the read process.
	 *
	 * @param generator The generator to include.
	 *
	 */
	void ExcludeExportGenerator(ExportGenerator &generator);

	/**
	 *
	 * IsImportGeneratorIncluded
	 *
	 * Checks if hte given generator is included.
	 *
	 * @param generator The generator.
	 *
	 * @return True if the generator is included.
	 *
	 */
	Bool IsImportGeneratorIncluded(ImportGenerator &generator);

	/**
	 *
	 * IsExportGeneratorIncluded
	 *
	 * Checks if hte given generator is included.
	 *
	 * @param generator The generator.
	 *
	 * @return True if the generator is included.
	 *
	 */
	Bool IsExportGeneratorIncluded(ExportGenerator &generator);

	/**
	 *
	 * ImportLogData
	 *
	 * Inports the data from the data log file.
	 *
	 * @param filename The filename of the data log file.
	 *
	 */
	Result ImportLogData(const char *filename);

	/**
	 *
	 * ExportSpoofData
	 *
	 * Exports the spoof data to the file.
	 *
	 * @param filename The filename of the spoof.
	 *
	 */
	Result ExportSpoofData(const char *filename);

	/**
	 *
	 * SetSpoofConfig
	 *
	 * Sets the spoof configurations.
	 *
	 * @param config The spoof config.
	 *
	 */
	void SetSpoofConfig(const SpoofConfig &config);

	/**
	 *
	 * SetSpoofInfo
	 *
	 * Sets the spoof info.
	 *
	 * @param info The spoof info.
	 *
	 */
	void SetSpoofInfo(const SpoofInfo &info);

private:
	typedef std::set<ImportGenerator*> ImportGeneratorList;
	typedef std::set<ExportGenerator*> ExportGeneratorList;

	DECODER::LogReader mLogReader;
	ImportGeneratorList mImportGeneratorList;
	ExportGeneratorList mExportGeneratorList;
	SourceStream mSourceStream;

	SpoofConfig mConfig;
	SpoofInfo mInfo;

	String mDatalog;
};

} // namespace BUILDER

#endif // SPOOFBUILDER_H

