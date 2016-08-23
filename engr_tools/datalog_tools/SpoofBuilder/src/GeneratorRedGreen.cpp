/*****************************************************************************\
**
** GeneratorRedGreen.cpp
**
** Provides the implementation for the red green code generator.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <ctime>
#include <iostream>

#include "GeneratorRedGreen.h"
#include "SpoofTags.h"

namespace BUILDER {

/* Constants *****************************************************************/

static const char* kIncludeStatement = "#include \"SpoofRedGreen.h\"";
static const char* kPopulateStatement = "mAssignmentList.push_back(new SpoofRedGreen(API));";

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/**
 *
 * GeneratorRedGreen
 *
 * Constructor
 *
 */ 
GeneratorRedGreen::GeneratorRedGreen(void) :
		mTimeFrameMgrP(NULL),
		mProcessGetRedGreenData(mRedRegister, mGreenRegister, mRedGreenVector),
		mNoiseFactor(0),
		mInterpolate(false),
		mSourceStreamP(NULL)
{
	std::srand(static_cast<Uint32>(std::time(NULL)));
}

/**
 *
 * GeneratorRedGreen
 *
 * Constructor
 *
 */ 
GeneratorRedGreen::GeneratorRedGreen(const TimeFrameMgr *mgr) :
		mTimeFrameMgrP(mgr),
		mProcessGetRedGreenData(mRedRegister, mGreenRegister, mRedGreenVector),
		mNoiseFactor(0),
		mInterpolate(false),
		mSourceStreamP(NULL)
{
	std::srand(static_cast<Uint32>(std::time(NULL)));
}

/**
 *
 * ~GeneratorRedGreen
 *
 * Destructor
 *
 */ 
GeneratorRedGreen::~GeneratorRedGreen(void)
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
void GeneratorRedGreen::Register(DECODER::LogReader &reader)
{
	reader.RegisterPeriodicSetItem(mProcessGetRedGreenData);
	reader.RegisterPeriodicLogOutput(mProcessGetRedGreenData);
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
void GeneratorRedGreen::Unregister(DECODER::LogReader &reader)
{
	reader.UnregisterPeriodicSetItem(mProcessGetRedGreenData);
	reader.UnregisterPeriodicLogOutput(mProcessGetRedGreenData);
}

/**
 *
 * Register
 *
 * Registers the output stream with the generator.
 *
 * @param stream The output stream.
 *
 */
void GeneratorRedGreen::Register(SourceStream &stream)
{
	mSourceStreamP = &stream;
}

/**
 *
 * Unregister
 *
 * Unregisters the output stream with the generator.
 *
 * @param stream The output stream.
 *
 */
void GeneratorRedGreen::Unregister(SourceStream &stream)
{
	if (mSourceStreamP == &stream) {
		mSourceStreamP = NULL;
	}
}

/**
 *
 * PreImportProcess
 *
 * Perform the procedures before the import process.
 *
 */
void GeneratorRedGreen::PreImportProcess(void)
{
	this->ClearData();
}

/**
 *
 * PostImportProcess
 *
 * Perform the procedures after the import process.
 *
 */
void GeneratorRedGreen::PostImportProcess(void)
{
	this->ProcessData();
}

/**
 *
 * PreExportProcess
 *
 * Perform the procedures before the export process.
 *
 */
void GeneratorRedGreen::PreExportProcess(void)
{
	if (mSourceStreamP != NULL && mTimeFrameMgrP != NULL) {
		TimeFrameMgr::TimeFrameList::const_iterator timeIter = mTimeFrameMgrP->GetTimeFrameListBegin();
		TimeFrameMgr::TimeFrameList::const_iterator timeEnd = mTimeFrameMgrP->GetTimeFrameListEnd();
		Int32 count = 0;  

		mSourceStreamP->GetStream(kTagConstants) << "const int kRedGreenValuesArray[][2] = {" << std::endl;

		for (; timeIter != timeEnd; ++timeIter) {
			RedGreenValues::const_iterator iter = mRedGreenValues.lower_bound((*timeIter).first);
			RedGreenValues::const_iterator end = mRedGreenValues.upper_bound((*timeIter).second);

			for (; iter != end; ++iter, ++count) {
				if ((count % 5) == 0) {
					mSourceStreamP->GetStream(kTagConstants) << "\t";
				}

				mSourceStreamP->GetStream(kTagConstants) 
						<< "{ " 
						<< iter->second.red 
						<< "," 
						<< iter->second.green 
						<< " }, ";

				if ((count % 5) == 4) {
					mSourceStreamP->GetStream(kTagConstants) << std::endl;
				}
			}
		}

		mSourceStreamP->GetStream(kTagConstants) << "};" << std::endl << std::endl;
		mSourceStreamP->GetStream(kTagConstants) 
				<< "const unsigned int kRedGreenValuesCount = " << count << ";" << std::endl;
		mSourceStreamP->GetStream(kTagIncludes) << kIncludeStatement << std::endl;
		mSourceStreamP->GetStream(kTagPopulate) << kPopulateStatement << std::endl;
	}
}

/**
 *
 * PostExportProcess
 *
 * Perform the procedures after the export process.
 *
 */
void GeneratorRedGreen::PostExportProcess(void)
{
}

/**
 *
 * SetTimeFrameMgr
 *
 * Sets the time frames of the red/green data that will be processed.
 *
 * @param mgr The time frame manager.
 *
 */
void GeneratorRedGreen::SetTimeFrameMgr(const TimeFrameMgr *mgr)
{
	mTimeFrameMgrP = mgr;
}

/**
 *
 * GetTimeFrameMgr
 *
 * Gets the time frames of the red/green data that will be processed.
 *
 * @return The time frame manager.
 *
 */
const TimeFrameMgr *GeneratorRedGreen::GetTimeFrameMgr(void)
{
	return mTimeFrameMgrP;
}

/**
 *
 * SetInterpolate
 *
 * Sets the interpolate state. True signifies that the values will be interpolated.
 *
 * @param The interpolate state.
 *
 */
void GeneratorRedGreen::SetInterpolateState(Bool state)
{
	mInterpolate = state;
	this->ProcessData();
}

/**
 *
 * GetInterpolateState
 *
 * Gets the interpolate state. True signifies that the values will be interpolated.
 *
 * @return The interpolate state.
 *
 */
Bool GeneratorRedGreen::GetInterpolateState(void)
{
	return mInterpolate;
}

/**
 *
 * SetNoiseFactor
 *
 * Sets the noise factor that will be used to generate the red/green values.
 *
 * @param noise The noise factor.
 *
 */
void GeneratorRedGreen::SetNoiseFactor(Int32 noise)
{
	mNoiseFactor = noise;
	this->ProcessData();
}

/**
 *
 * GetNoiseFactor
 *
 * Gets the noise factor that will be used to generate the red/green values.
 *
 * @return The noise factor.
 *
 */
Int32 GeneratorRedGreen::GetNoiseFactor(void)
{
	return mNoiseFactor;
}

/**
 *
 * GetRedGreenValuesBegin
 *
 * Gets an iterator to the red green values. The iterator will point
 * to the beginning of the set. 
 *
 * @return The iterator to the beginning of the values.
 *
 */
GeneratorRedGreen::RedGreenValues::const_iterator GeneratorRedGreen::GetRedGreenValuesBegin(void) const
{
	return mRedGreenValues.begin();
}

/**
 *
 * GetRedGreenValuesEnd
 *
 * Gets an iterator to the red green values. The iterator will point
 * to the end of the set. 
 *
 * @return The iterator to the end of the values.
 *
 */
GeneratorRedGreen::RedGreenValues::const_iterator GeneratorRedGreen::GetRedGreenValuesEnd(void) const
{
	return mRedGreenValues.end();
}

/**
 *
 * FindRedGreenValues
 *
 * Gets an iterator to the red green values. The iterator will point
 * to the given time. The pair contains the time and the 
 * reflector data.
 *
 * @param The time value.
 *
 * @return The iterator to the values.
 *
 */
GeneratorRedGreen::RedGreenValues::const_iterator GeneratorRedGreen::FindRedGreenValues(Uint32 time) const
{
	return mRedGreenValues.find(time);
}

/**
 *
 * ClearData
 *
 * Clears the data from the internal structures.
 *
 */
void GeneratorRedGreen::ClearData(void)
{
	RedGreenRegister clearRecord;

	mRedRegister = clearRecord;
	mGreenRegister = clearRecord;
	mRedGreenVector.clear();
	mRedGreenValues.clear();
}

/**
 *
 * ProcessData
 *
 * Populates the internal structures with the data from the records.
 *
 */
void GeneratorRedGreen::ProcessData(void)
{
	RedGreenVector::iterator recordIter = mRedGreenVector.begin();
	RedGreenVector::iterator recordEnd = mRedGreenVector.end();

	Uint32 firstRecordTime = 0;
	Int32 firstRedValue = 0;
	Int32 firstGreenValue = 0;
	Uint32 secondRecordTime = 0;
	Int32 secondRedValue = 0;
	Int32 secondGreenValue = 0;
	Float redSlope = 0;
	Float greenSlope = 0;

	for (; recordIter != recordEnd; ++recordIter) {
		DECODER::RecordPeriodicLogOutput &record = *recordIter;

		DECODER::RecordPeriodicLogOutput::Map::const_iterator redIter = record.mData.find(mRedRegister.mKeyCode);
		DECODER::RecordPeriodicLogOutput::Map::const_iterator greenIter = record.mData.find(mGreenRegister.mKeyCode);
		DECODER::RecordPeriodicLogOutput::Map::const_iterator dataEnd = record.mData.end();

		secondRecordTime = record.mTimeSeconds;

		if (redIter != dataEnd) {
			secondRedValue = (*reinterpret_cast<const Int32*>(&redIter->second.front()));
		}

		if (greenIter != dataEnd) {
			secondGreenValue = (*reinterpret_cast<const Int32*>(&greenIter->second.front()));
		}

		if (mInterpolate) {
			redSlope = SpoofUtil::GetLineSlope(
					firstRecordTime, firstRedValue,
					secondRecordTime, secondRedValue);
			greenSlope = SpoofUtil::GetLineSlope(
					firstRecordTime, firstGreenValue, 
					secondRecordTime, secondGreenValue);
			}

		for (Uint32 time = firstRecordTime; time < secondRecordTime; ++time) {
			RedGreenValuePair values = { 0 };

			values.red = SpoofUtil::GetValueWithNoise(redSlope, firstRecordTime, firstRedValue, time, mNoiseFactor);
			values.green = SpoofUtil::GetValueWithNoise(greenSlope, firstRecordTime, firstGreenValue, time, mNoiseFactor);

			mRedGreenValues[time] = values;
		}

		firstRecordTime = secondRecordTime;
		firstRedValue = secondRedValue;
		firstGreenValue = secondGreenValue;
	}
}

} // namespace BUILDER


