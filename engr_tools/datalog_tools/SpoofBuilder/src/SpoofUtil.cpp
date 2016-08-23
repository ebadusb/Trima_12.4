/*****************************************************************************\
**
** SpoofUtil.cpp
**
** Spoof utility class implementation.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <fstream>
#include <ctime>

#include "SpoofUtil.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/* Classes *******************************************************************/

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
Bool SpoofUtil::InputData(const String &file, String &data)
{
	Bool result = false;
	std::fstream in;

	in.open(file.c_str(), std::fstream::in);

	if(in.good()) {
		std::string buffer;

		while(std::getline(in, buffer)) {
			data += buffer;
			data += "\n";
		}
		in.close();
		result = true;
	}

	return result;
}

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
Bool SpoofUtil::OutputData(const String &file, const String &data)
{
	Bool result = false;
	std::fstream out;
	
	out.open(file.c_str(), std::fstream::out);

	if(out.good()) {
		out << data << std::endl;
		out.close();
		result = true;
	}

	return result;
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
void SpoofUtil::ReplaceTagWithValue(String &data, const String &tag, const String &value)
{
	std::size_t len = std::strlen(tag.c_str());
	std::string::size_type loc = data.find(tag, 0);

	while (loc != std::string::npos) {
		data.replace(loc, len, value);
		loc = data.find(tag, loc);
	}
}

/**
 *
 * GetTimeStamp
 *
 * Creates a time stamp.
 *
 * @return A time stamp.
 *
 */
String SpoofUtil::GetTimeStamp(void)
{
	std::time_t time = std::time(NULL);
	const char *buffer = std::asctime(std::localtime(&time));

	return String(buffer, std::strlen(buffer) - 1);
}

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
String SpoofUtil::GetFilename(const String &filename)
{
	std::string name = filename;
	std::string::size_type loc = 0;

	loc = name.find_last_of("\\");
	if (loc != std::string::npos) {
		name.erase(0, loc + 1);
	}

	loc = name.find_last_of("/");
	if (loc != std::string::npos) {
		name.erase(0, loc + 1);
	}

	return name;
}

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
String SpoofUtil::StripExtension(const String &filename)
{
	std::string name = filename;
	std::string::size_type loc = 0;

	loc = name.find_last_of(".");
	if (loc != std::string::npos) {
		name.erase(loc, std::string::npos);
	}

	return name;
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
String SpoofUtil::ConvertNameForOutput(const String &text)
{
	String convertedText;

	String::const_iterator iter = text.begin();
	String::const_iterator end = text.end();

	for (; iter != end; ++iter) {
		char character = *iter;

		if (isdigit(character) || isalpha(character)) {
			convertedText += character;
		}
	}

	return convertedText;
}

/**
 *
 * ConvertTextForOutput
 *
 * Converts the given string for output.
 *
 * @param text The string to format.
 *
 * @return The updated string.
 *
 */
String SpoofUtil::ConvertTextForOutput(const String &text)
{
	String convertedText;

	String::const_iterator iter = text.begin();
	String::const_iterator end = text.end();

	for (; iter != end; ++iter) {
		char character = *iter;

		if (character == '\n') {
			convertedText += "\\n";
		}
		else if (character == '\"') {
			convertedText += "\\\"";
		}
		else if (character == '\\') {
			convertedText += "\\\\";
		}
		else if (isprint(character)) {
			convertedText += character;
		}
	}

	return convertedText;
}

/**
 *
 * FilterAlarmStates
 *
 * Filters alarm states from the manager.
 *
 */
void SpoofUtil::FilterAlarmStates(TimeFrameMgr &mgr,  
		LogDataVector::const_iterator iter, 
		LogDataVector::const_iterator end) 
{
	TimeFrameMgr::TimeFrame timeFrame(0, 0);

	for (; iter != end; ++iter) {
		if ((*iter).second == "true") {
			timeFrame.first = (*iter).first;
		}
		else if ((*iter).second == "false") {
			timeFrame.second = (*iter).first;
			mgr.ExcludeTimeFrame(timeFrame);
		}
	}
}

/**
 *
 * FilterRecoveryStates
 *
 * Filters recovery states from the manager.
 *
 */
void SpoofUtil::FilterRecoveryStates(TimeFrameMgr &mgr, 
		LogDataVector::const_iterator iter, 
		LogDataVector::const_iterator end)
{
	TimeFrameMgr::TimeFrame timeFrame(0, 0);

	for (; iter != end; ++iter) {
		if ((*iter).second == "None") {
			timeFrame.second = (*iter).first;
			mgr.ExcludeTimeFrame(timeFrame);
			timeFrame.first = 0;
			timeFrame.second = 0;
		}
		else if (timeFrame.first == 0) {
			timeFrame.first = (*iter).first;
		}
	}
}

} // namespace BUILDER


