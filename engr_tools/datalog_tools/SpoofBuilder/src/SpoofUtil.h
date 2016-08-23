/*****************************************************************************\
**
** SpoofUtil.h
**
** Spoof utility class implementation.
**
\*****************************************************************************/

#ifndef SPOOFUTIL_H
#define SPOOFUTIL_H

/* Includes ******************************************************************/

#include "SpoofTypes.h"
#include "SpoofBuilder.h"
#include "TimeFrameMgr.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

static void PreImportProcess(ImportGenerator *generator) { generator->PreImportProcess(); }
static void PostImportProcess(ImportGenerator *generator) { generator->PostImportProcess(); }
static void PreExportProcess(ExportGenerator *generator) { generator->PreExportProcess(); }
static void PostExportProcess(ExportGenerator *generator) { generator->PostExportProcess(); }

static void ClearStream(const std::pair<String, SourceStream::Stream*> &data) { 	data.second->str(""); }

/* Classes *******************************************************************/

class SpoofUtil {
public:

	typedef std::vector<std::pair<Uint32, String>> LogDataVector;

	/**
	 *
	 * InputData
	 *
	 * Inputs the given file into the string object.
	 *
	 * @param file The filename including path.
	 * @param data The object to hold the file data. 
	 *
	 * @return TRUE if the opperation was successful.
	 *
	 */
	static Bool InputData(const String &file, String &data);

	/**
	 *
	 * OutputData
	 *
	 * Outputs the given data to the given file.
	 *
	 * @param file The filename including path.
	 * @param data The object containing the file data. 
	 *
	 * @return TRUE if the opperation was successful.
	 *
	 */
	static Bool OutputData(const String &file, const String &data);

	/**
	 *
	 * GetLineSlope
	 *
	 * Calculates the slope between two points.
	 *
	 * @return The slope between the two points.
	 *
	 */
	template <typename _TYPE1, typename _TYPE2>
	static inline Float GetLineSlope(_TYPE1 x1, _TYPE2 y1, _TYPE1 x2, _TYPE2 y2)
	{
		return static_cast<Float>(y2 - y1) / (x2 - x1);
	}

	/**
	 *
	 * GetValueBetweenPoints
	 *
	 * Calculates the value at given x between two points.
	 *
	 * @return The value of x.
	 *
	 */
	template <typename _TYPE1, typename _TYPE2>
	static inline _TYPE2 GetValueBetweenPoints(_TYPE1 x1, _TYPE2 y1, _TYPE1 x2, _TYPE2 y2, _TYPE1 x)
	{
		return static_cast<_TYPE2>(SpoofUtil::GetValueFromEquation(x1, y1, x2, y2) * (x - x1) + y1);
	}

	/**
	 *
	 * GetValueFromEquation
	 *
	 * Calculates the value at given x using line equation.
	 *
	 * @return The value of x.
	 *
	 */
	template <typename _TYPE1, typename _TYPE2>
	static inline _TYPE2 GetValueFromEquation(Float slope, _TYPE1 x1, _TYPE2 y1, _TYPE1 x)
	{
		return static_cast<_TYPE2>(slope * (x - x1) + y1);
	}

	/**
	 *
	 * GetValueWithNoise
	 *
	 * Calculates the value at given x using line equation and adds noise to the result.
	 *
	 * @return The value of x.
	 *
	 */
	template <typename _TYPE1, typename _TYPE2>
	static inline _TYPE2 GetValueWithNoise(Float slope, _TYPE1 x1, _TYPE2 y1, _TYPE1 x, _TYPE2 noise)
	{
		return SpoofUtil::AddNoise(SpoofUtil::GetValueFromEquation(slope, x1, y1, x), noise);
	}

	/**
	 *
	 * AddNoise
	 *
	 * Adds noise to the given value.
	 *
	 * @return The new values.
	 *
	 */
	template <typename _TYPE>
	static inline _TYPE AddNoise(_TYPE value, _TYPE noise)
	{
		return value + (noise * 2 * std::rand() / RAND_MAX - noise);
	}

	/**
	 *
	 * ReplaceTagWithValue
	 *
	 * Replaces all occurrences of the tag with the given value.
	 *
	 * @param data The string to modify.
	 * @param tag The tag to replace.
	 * @param data The tag value.
	 *
	 */
	static void ReplaceTagWithValue(String &data, const String &tag, const String &value);

	/**
	 *
	 * GetTimeStamp
	 *
	 * Creates a time stamp.
	 *
	 * @return A time stamp.
	 *
	 */
	static String GetTimeStamp(void);

	/**
	 *
	 * GetFilename
	 *
	 * Gets the filename the given pathname.
	 *
	 * @param filename The filename.
	 *
	 * @return A valid filename name.
	 *
	 */
	static String GetFilename(const String &filename);

	/**
	 *
	 * StripExtension
	 *
	 * Strips the extension from the given filename.
	 *
	 * @param filename The filename.
	 *
	 * @return A filename.
	 *
	 */
	static String StripExtension(const String &filename);

	/**
	 *
	 * ToString
	 *
	 * Converts the given item to a string form.
	 *
	 */
	template <typename _TYPE>
	static String ToString(const _TYPE &data)
	{
		std::ostringstream stream;

		stream << data;

		return stream.str();
	}

	/**
	 *
	 * ConvertNameForOutput
	 *
	 * Converts the given name for output.
	 *
	 * @param text The string to format.
	 *
	 * @return The updated string.
	 *
	 */
	static String SpoofUtil::ConvertNameForOutput(const String &text);

	/**
	 *
	 * ConvertStringForOutput
	 *
	 * Converts the given string for output.
	 *
	 * @param text The string to format.
	 *
	 * @return The updated string.
	 *
	 */
	static String ConvertTextForOutput(const String &text);

	/**
	 *
	 * FilterAlarmStates
	 *
	 * Filters alarm states from the manager.
	 *
	 */
	static void FilterAlarmStates(TimeFrameMgr &mgr, 
			LogDataVector::const_iterator iter, 
			LogDataVector::const_iterator end); 

	/**
	 *
	 * FilterRecoveryStates
	 *
	 * Filters recovery states from the manager.
	 *
	 */
	static void FilterRecoveryStates(TimeFrameMgr &mgr, 
			LogDataVector::const_iterator iter, 
			LogDataVector::const_iterator end); 
};


class ReplaceTags {
public:
	ReplaceTags(String &str) : mStr(str) {}

	void operator()(const std::pair<String, SourceStream::Stream*> &data)
	{
		SpoofUtil::ReplaceTagWithValue(mStr, data.first, data.second->str());
	}

private:
	String &mStr;
};

template <class _HANDLER>
class FilterPeriodicSetItemByName {
public:
	typedef _HANDLER Handler;

	FilterPeriodicSetItemByName(Handler &handler, String name) 
			: mHandler(handler), mName(name) {}

	void operator()(const DECODER::RecordPeriodicSetItem &data) 
	{ 	
		if (data.mName == mName) {
			mHandler(data);
		}
	}

private:
	Handler &mHandler;
	String mName;
};

template <class _HANDLER>
class FilterPeriodicLogOutputByKey {
public:
	typedef _HANDLER Handler;

	FilterPeriodicLogOutputByKey(Handler &handler, Uint16 key) 
			: mHandler(handler), mKey(key) {}

	void operator()(const DECODER::RecordPeriodicLogOutput &data) 
	{ 	
		if (data.mData.find(mKey) != data.mData.end()) {
			mHandler(data);
		}
	}

private:
	Handler &mHandler;
	Uint16 mKey;
};

template <class _HANDLER, class _TYPE>
class FilterRecordTimeSpan {
public:
	typedef _HANDLER Handler;
	typedef _TYPE Type;

	FilterRecordTimeSpan(Handler &handler, Uint32 begin, Uint32 end) 
			: mHandler(handler), mBegin(begin), mEnd(end) {}

	void operator()(const Type &data) 
	{ 	
		if (data.mTimeSeconds >= mBegin && data.mTimeSeconds <= mEnd) {
			mHandler(data);
		}
	}

private:
	Handler &mHandler;
	Uint32 mBegin;
	Uint32 mEnd;
};

template <class _HANDLERSETITEM, class _HANDLERLOGOUTPUT>
class FilterPeriodicLogOutputByName {
public:
	typedef _HANDLERSETITEM HandlerSetItem;
	typedef _HANDLERLOGOUTPUT HandlerLogOutput;

	FilterPeriodicLogOutputByName(HandlerSetItem &handlerSetItem, HandlerLogOutput &handlerLogOutput, String name) 
			: mHandlerSetItem(handlerSetItem), mHandlerLogOutput(handlerLogOutput), mName(name), mKey(0) {}

	void operator()(const DECODER::RecordPeriodicSetItem &data) 
	{ 	
		if (data.mName == mName) {
			mHandlerSetItem(data);
			mKey = data.mKeyCode;
		}
	}

	void operator()(const DECODER::RecordPeriodicLogOutput &data) 
	{ 	
		if (data.mData.find(mKey) != data.mData.end()) {
			mHandlerLogOutput(data);
		}
	}

private:
	HandlerSetItem &mHandlerSetItem;
	HandlerSetItem &mHandlerLogOutput;
	String mName;
	Uint16 mKey;
};

class ProcessGetPeriodicLogs {
public:
	typedef std::vector<std::pair<Uint32, String>> LogDataVector;

	ProcessGetPeriodicLogs(String name, LogDataVector &vector) : 
			mLogDataVector(vector), mName(name), mKey(0) {}

	void operator()(const DECODER::RecordPeriodicSetItem &data) 
	{ 	
		if (data.mName == mName) {
			mKey = data.mKeyCode;
		}
	}

	void operator()(const DECODER::RecordPeriodicLogOutput &data) 
	{ 	
		typedef DECODER::RecordPeriodicLogOutput::Map Map;

		Map::const_iterator iter = data.mData.find(mKey);
		Map::const_iterator end = data.mData.end();

		if (iter != end) {
			const char* text = reinterpret_cast<const char*>(&iter->second.front());
			Int32 count = iter->second.size();

			mLogDataVector.push_back(std::make_pair(data.mTimeSeconds, String(text, count)));
		}
	}

private:
	LogDataVector &mLogDataVector;

	String mName;
	Uint16 mKey;
};

} // namespace BUILDER

#endif // SPOOFUTIL_H

