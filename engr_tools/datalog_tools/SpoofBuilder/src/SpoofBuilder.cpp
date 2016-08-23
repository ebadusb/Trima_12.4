/*****************************************************************************\
**
** SpoofBuilder.h
**
** Spoof builder implementation.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <algorithm>

#include "SpoofBuilder.h"
#include "SpoofUtil.h"
#include "SpoofTags.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/**
 *
 * SpoofBuilder
 *
 * Constructor
 *
 */
SpoofBuilder::SpoofBuilder(void) 
{
	mInfo.timeout = 0;
}

/**
 *
 * ~SpoofBuilder
 *
 * Destructor
 *
 */
SpoofBuilder::~SpoofBuilder(void)
{
}

/**
 *
 * IncludeImportGenerator
 *
 * Includes the given generator in the read process.
 *
 * @param generator The generator to include.
 *
 */
void SpoofBuilder::IncludeImportGenerator(ImportGenerator &generator)
{
	if (mImportGeneratorList.find(&generator) == mImportGeneratorList.end()) {
		mImportGeneratorList.insert(&generator);
		generator.Register(mLogReader);
	}
}

/**
 *
 * ExcludeImportGenerator
 *
 * Excludes the given generator from the read process.
 *
 * @param generator The generator to include.
 *
 */
void SpoofBuilder::ExcludeImportGenerator(ImportGenerator &generator)
{
	if (mImportGeneratorList.find(&generator) != mImportGeneratorList.end()) {
		mImportGeneratorList.erase(&generator);
		generator.Unregister(mLogReader);
	}
}

/**
 *
 * IncludeExportGenerator
 *
 * Includes the given generator in the read process.
 *
 * @param generator The generator to include.
 *
 */
void SpoofBuilder::IncludeExportGenerator(ExportGenerator &generator)
{
	if (mExportGeneratorList.find(&generator) == mExportGeneratorList.end()) {
		mExportGeneratorList.insert(&generator);
		generator.Register(mSourceStream);
	}
}

/**
 *
 * ExcludeExportGenerator
 *
 * Excludes the given generator from the read process.
 *
 * @param generator The generator to include.
 *
 */
void SpoofBuilder::ExcludeExportGenerator(ExportGenerator &generator)
{
	if (mExportGeneratorList.find(&generator) != mExportGeneratorList.end()) {
		mExportGeneratorList.erase(&generator);
		generator.Unregister(mSourceStream);
	}
}

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
Bool SpoofBuilder::IsImportGeneratorIncluded(ImportGenerator &generator)
{
	return (mImportGeneratorList.find(&generator) != mImportGeneratorList.end());
}

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
Bool SpoofBuilder::IsExportGeneratorIncluded(ExportGenerator &generator)
{
	return (mExportGeneratorList.find(&generator) != mExportGeneratorList.end());
}

/**
 *
 * ImportLogData
 *
 * Inports the data from the data log file.
 *
 * @param filename The filename of the data log file.
 *
 */
SpoofBuilder::Result SpoofBuilder::ImportLogData(const char *filename)
{
	Result results = kResultNone;

	mDatalog = SpoofUtil::GetFilename(filename);

	// Do pre-process operations.
	for_each(mImportGeneratorList.begin(), mImportGeneratorList.end(), PreImportProcess);

	// Read the records from the file.
	results = mLogReader.Open(filename);
	while (results == kResultNone && !mLogReader.IsEndOfFile()) {
		results = mLogReader.Read(1);
	}
	mLogReader.Close();
	
	// Do post-process operations.
	for_each(mImportGeneratorList.begin(), mImportGeneratorList.end(), PostImportProcess);

	return results;
}

/**
 *
 * ExportSpoofData
 *
 * Exports the spoof data to the file.
 *
 * @param filename The filename of the spoof.
 *
 */
SpoofBuilder::Result SpoofBuilder::ExportSpoofData(const char *filename)
{
	Result results = kResultNone;
	std::string data;

	// Clear all data
	for_each(mSourceStream.Begin(), mSourceStream.End(), ClearStream);

	// Do pre-process operations.
	for_each(mExportGeneratorList.begin(), mExportGeneratorList.end(), PreExportProcess);

	// Read in the template
	results = SpoofUtil::InputData(mConfig.templateFilename, data) ? kResultNone : kResultTemplateReadFailed;

	if (results == kResultNone) {
		String souceFilename = SpoofUtil::StripExtension(SpoofUtil::GetFilename(filename));
		String namespaceName = souceFilename;

		// Create output data
		mSourceStream.GetStream(kTagName) << mInfo.name;
		mSourceStream.GetStream(kTagDatalog) << mDatalog;
		mSourceStream.GetStream(kTagComment) << mInfo.comment;
		mSourceStream.GetStream(kTagHelp) << mInfo.help;
		mSourceStream.GetStream(kTagDate) << SpoofUtil::GetTimeStamp();
		mSourceStream.GetStream(kTagFilename) << souceFilename;
		mSourceStream.GetStream(kTagNamespace) << namespaceName;
		mSourceStream.GetStream(kTagTimeout) << mInfo.timeout;

		// Replace spoof tags
		for_each(mSourceStream.Begin(), mSourceStream.End(), ReplaceTags(data));

		// Export the data
		results = SpoofUtil::OutputData(filename, data) ? kResultNone : kResultSourceWriteFailed;
	}

	// Do post-process operations.
	for_each(mExportGeneratorList.begin(), mExportGeneratorList.end(), PostExportProcess);

	// Clear all data
	for_each(mSourceStream.Begin(), mSourceStream.End(), ClearStream);

	return results;
}

/**
 *
 * SetSpoofConfig
 *
 * Sets the spoof configurations.
 *
 * @param config The spoof config.
 *
 */
void SpoofBuilder::SetSpoofConfig(const SpoofConfig &config)
{
	mConfig = config;
}

/**
 *
 * SetSpoofInfo
 *
 * Sets the spoof info.
 *
 * @param info The spoof info.
 *
 */
void SpoofBuilder::SetSpoofInfo(const SpoofInfo &info)
{
	mInfo.name = SpoofUtil::ConvertNameForOutput(info.name);
	mInfo.comment = SpoofUtil::ConvertTextForOutput(info.comment);
	mInfo.help = SpoofUtil::ConvertTextForOutput(info.help);
	mInfo.timeout = info.timeout;
}

} // namespace BUILDER

