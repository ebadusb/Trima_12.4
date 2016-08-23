/*****************************************************************************\
**
** GeneratorRedGreen.h
**
** Provides the implementation for the red green code generator.
**
\*****************************************************************************/

#ifndef GENERATORREDGREEN_H
#define GENERATORREDGREEN_H

/* Includes ******************************************************************/

#include <vector>

#include "SpoofGenerator.h"
#include "SpoofTypes.h"
#include "SpoofUtil.h"
#include "LogReaderUtil.h"
#include "TimeFrameMgr.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/* Classes *******************************************************************/

class ProcessGetRedGreenData {
public:
	typedef DECODER::RecordPeriodicSetItem RedGreenRegister;
	typedef std::vector<DECODER::RecordPeriodicLogOutput> RedGreenVector;

	ProcessGetRedGreenData(RedGreenRegister &red, RedGreenRegister &green, RedGreenVector &vector) :
			mRedRegister(red),
			mGreenRegister(green),
			mRedGreenVector(vector),
			mProcessSetRegisterRed(mRedRegister),
			mProcessSetRegisterGreen(mGreenRegister),
			mProcessIncludeRedGreen(mRedGreenVector),
			mFilterPeriodicSetItemRed(mProcessSetRegisterRed, "Red"),
			mFilterPeriodicSetItemGreen(mProcessSetRegisterGreen, "Green")
	{
	}

	void operator()(const DECODER::RecordPeriodicSetItem &data) 
	{ 	
		mFilterPeriodicSetItemRed(data);
		mFilterPeriodicSetItemGreen(data);
	}

	void operator()(const DECODER::RecordPeriodicLogOutput &data) 
	{ 	
		if (data.mData.find(mRedRegister.mKeyCode) != data.mData.end() 
				|| data.mData.find(mGreenRegister.mKeyCode) != data.mData.end() ) {
			mProcessIncludeRedGreen(data);
		}
	}

private:
	typedef DECODER::ProcessSet<DECODER::RecordPeriodicSetItem> ProcessSetRegisterRedGreen;
	typedef DECODER::ProcessInclude<DECODER::RecordPeriodicLogOutput, RedGreenVector> ProcessIncludeRedGreen;
	typedef FilterPeriodicSetItemByName<ProcessSetRegisterRedGreen> FilterPeriodicSetItemByName;

	RedGreenRegister &mRedRegister;
	RedGreenRegister &mGreenRegister;
	RedGreenVector &mRedGreenVector;

	ProcessSetRegisterRedGreen mProcessSetRegisterRed;
	ProcessSetRegisterRedGreen mProcessSetRegisterGreen;
	ProcessIncludeRedGreen mProcessIncludeRedGreen;

	FilterPeriodicSetItemByName mFilterPeriodicSetItemRed;
	FilterPeriodicSetItemByName mFilterPeriodicSetItemGreen;
};

class GeneratorRedGreen : public ImportGenerator, public ExportGenerator {
public:

	struct RedGreenValuePair {
		Int32 red;
		Int32 green;
	};

	typedef std::map<Uint32, RedGreenValuePair> RedGreenValues;

	/**
	 *
	 * GeneratorRedGreen
	 *
	 * Constructor
	 *
	 */ 
	GeneratorRedGreen(void);

	/**
	 *
	 * GeneratorRedGreen
	 *
	 * Constructor
	 *
	 */ 
	GeneratorRedGreen(const TimeFrameMgr *mgr);

	/**
	 *
	 * ~GeneratorRedGreen
	 *
	 * Destructor
	 *
	 */ 
	virtual ~GeneratorRedGreen(void);

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
	 * Register
	 *
	 * Registers the output stream with the generator.
	 *
	 * @param stream The output stream.
	 *
	 */
	virtual void Register(SourceStream &stream);

	/**
	 *
	 * Unregister
	 *
	 * Unregisters the output stream with the generator.
	 *
	 * @param stream The output stream.
	 *
	 */
	virtual void Unregister(SourceStream &stream);

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
	 * PreExportProcess
	 *
	 * Perform the procedures before the export process.
	 *
	 */
	virtual void PreExportProcess(void);

	/**
	 *
	 * PostExportProcess
	 *
	 * Perform the procedures after the export process.
	 *
	 */
	virtual void PostExportProcess(void);

	/**
	 *
	 * SetTimeFrameMgr
	 *
	 * Sets the time frames of the red/green data that will be processed.
	 *
	 * @param mgr The time frame manager.
	 *
	 */
	void SetTimeFrameMgr(const TimeFrameMgr *mgr);

	/**
	 *
	 * GetTimeFrameMgr
	 *
	 * Gets the time frames of the red/green data that will be processed.
	 *
	 * @return The time frame manager.
	 *
	 */
	const TimeFrameMgr *GetTimeFrameMgr(void);

	/**
	 *
	 * SetInterpolate
	 *
	 * Sets the interpolate state. True signifies that the values will be interpolated.
	 *
	 * @param The interpolate state.
	 *
	 */
	void SetInterpolateState(Bool state);

	/**
	 *
	 * GetInterpolateState
	 *
	 * Gets the interpolate state. True signifies that the values will be interpolated.
	 *
	 * @return The interpolate state.
	 *
	 */
	Bool GetInterpolateState(void);

	/**
	 *
	 * SetNoiseFactor
	 *
	 * Sets the noise factor that will be used to generate the red/green values.
	 *
	 * @param noise The noise factor.
	 *
	 */
	void SetNoiseFactor(Int32 noise);

	/**
	 *
	 * GetNoiseFactor
	 *
	 * Gets the noise factor that will be used to generate the red/green values.
	 *
	 * @return The noise factor.
	 *
	 */
	Int32 GetNoiseFactor(void);

	/**
	 *
	 * GetRedGreenValuesBegin
	 *
	 * Gets an iterator to the red green values. The iterator will point
	 * to the beginning of the set. The pair contains the time and the 
	 * reflector data.
	 *
	 * @return The iterator to the beginning of the values.
	 *
	 */
	RedGreenValues::const_iterator GetRedGreenValuesBegin(void) const;

	/**
	 *
	 * GetRedGreenValuesEnd
	 *
	 * Gets an iterator to the red green values. The iterator will point
	 * to the end of the set. The pair contains the time and the 
	 * reflector data.
	 *
	 * @return The iterator to the end of the values.
	 *
	 */
	RedGreenValues::const_iterator GetRedGreenValuesEnd(void) const;

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
	RedGreenValues::const_iterator FindRedGreenValues(Uint32 time) const;

private:
	typedef ProcessGetRedGreenData::RedGreenRegister RedGreenRegister;
	typedef ProcessGetRedGreenData::RedGreenVector RedGreenVector;

	void ClearData(void);
	void ProcessData(void);

	// The time frame to process
	const TimeFrameMgr *mTimeFrameMgrP;

	// Record data collected.
	RedGreenRegister mRedRegister;
	RedGreenRegister mGreenRegister;
	RedGreenVector mRedGreenVector;

	// Generator settings.
	Int32 mNoiseFactor;
	Bool mInterpolate;

	// Spoof output data 
	RedGreenValues mRedGreenValues;

	// The streams for data output.
	SourceStream *mSourceStreamP;

	// Record handler.
	ProcessGetRedGreenData mProcessGetRedGreenData;
};

} // namespace BUILDER

#endif // GENERATORREDGREEN_H

