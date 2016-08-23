#ifndef __LOG_CONVERT_H__
#define __LOG_CONVERT_H__
/* for msvc only */
//#ifndef __MINGW__
//#pragma warning(disable: 4786)
//#endif
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>
#include <set>


using namespace std;


/* Create an iterator for log records. 
  Perform time based, sequential iteration 
*/
struct DLOG_Record
{
	enum DLOG_RecordType
	{
		DLOG_HeaderId           = 0x5500, 
		DLOG_LogLevelId			= 0x5501,
		DLOG_PrintOutputId		= 0x5502,
		DLOG_StreamOutputId     = 0x5503, 
		DLOG_PeriodicLogId		= 0x5504,
		DLOG_PeriodicSetId		= 0x5505,
		DLOG_PeriodicSetItemId	= 0x5506,
		DLOG_TaskCreatedId		= 0x5507,      
		DLOG_TaskDeletedId		= 0x5508,      
		DLOG_NetworkLogHeaderId	= 0x5509,
		DLOG_BinaryId			= 0x55f0,
		DLOG_EndNetworkLogId    = 0x55fc,
		DLOG_EndLogId           = 0x55fd,
		DLOG_TimeStampId        = 0x55fe,
		DLOG_MissedDataId		= 0x55ff
	};
	/* Record Type Id (0x5500), 2 bytes */
	unsigned short headerType;    
	/* Time since subsystem startup when record was created (4 bytes seconds, 4 bytes nanoseconds */
	unsigned int timeSeconds;
	unsigned int timeNanoSeconds;
};
/* Start of Log file */
struct DLOG_Header : DLOG_Record
{
	/* Confidential and proprietary message, terminated with a ctrl-Z (end of file for DOS/Windows), ctrl-D (end of file for Unix), and null (in that order). */
	string startMessage;
	/* The value 0x12345678, 4 bytes */
	int byteOrder;
	/* sizeof(char), 2 bytes */
	unsigned short sizeOfChar;
	/* sizeof(int), 2 bytes */
	unsigned short sizeOfInt;
	/* sizeof(long), 2 bytes */
	unsigned short sizeOfLong;
	/* sizeof(float), 2 bytes */
	unsigned short sizeOfFloat;
	/* sizeof(double), 2 bytes */
	unsigned short sizeOfDouble;
	/* sizeof(task ID) */
	unsigned short sizeOfTaskId;
	/* task id */
	unsigned int taskId;
	/* sizeof(NodeID) */
	unsigned short sizeOfNodeId;
	/* Version information (high byte = major version, low byte = minor version).  
	A major version change identifies changes that affect the format of the log 
	file (and thus require accompanying changes to the decoder).  Minor version 
	change identifies changes that do not impact the format of the log file.*/
	unsigned short version;
	/* Size of platform name (not including NULL byte). */
	unsigned short sizeOfPlatformName;
	/* The platform name */
	string platformName;
	/* Node ID */
	unsigned int nodeId;
	/* Day log was created (1-31). */
	unsigned char day;
	/* Month log was created (1-12).*/
	unsigned char month;
	/* Year log was created (e.g. 2002). */
	unsigned short year;
	/* Hour log was created (0-23). */
	unsigned char hour;
	/* Minute log was created (0-59). */
	unsigned char minute;
	/* Second log was created (0-59). */
	unsigned char second;
	/* Size of node name (not including NULL byte) */
	unsigned short sizeOfNodeName;
	/* Node name, in ASCII, not including NULL terminator */
	string nodeName;
	
};


struct DLOG_LogLevel : DLOG_Record
{
	
	/* LevelID */
	unsigned short levelId;

	/* Node ID */
	unsigned int nodeId;

	/* Size of level name (not including NULL byte) */
	unsigned short sizeOfLevelName;

	/* level name, in ASCII, not including NULL terminator */
	string levelName;

};

struct DLOG_PrintOutput : DLOG_Record
{
	
	/* LevelID */
	unsigned short levelId;

	/* task id */
	unsigned int taskId;

	/* Node ID */
	unsigned int nodeId;

	/* length of format (not including NULL byte) */
	unsigned short lenFormat;

	/* length of file (not including NULL byte) */
	unsigned short lenFile;

	/* line no.*/
	unsigned short lineNo;
	
	/* format contents, in ASCII, not including NULL terminator */
	string formatString;

	/* file name, in ASCII, not including NULL terminator */
	string fileName;

	/* print argument values.  Arguments are determined by parsing the
	format string, and are stored in the order specified in that string.
	Most arguments are stored directly in binary.  The exception is that
	arguments for %s are stored similar to other strings, with 2 bytes
	for length, followed by the ASCII string data, not including NULL terminator. */
	string argValues; /* stored as space separated string */
};

struct DLOG_StreamOutput : DLOG_Record
{
	
	/* LevelID */
	unsigned short levelId;

	/* task id */
	unsigned int taskId;

	/* Node ID */
	unsigned int nodeId;

	/* length of file (not including NULL byte) */
	unsigned short lenFile;

	/* line no. */
	unsigned short lineNo;

	/* file name, in ASCII, not including NULL terminator */
	string fileName;

	/* Initial format flag setting (bitwise-OR of values from Table 11). */
	unsigned short flags;

	/* Initial precision setting (number of digits displayed after the
	decimal point for floating point values). */
	unsigned char precision;

	/* Total length (in bytes) of the stream argument output records.  */
	unsigned short lenStream;

	/* Stream argument output records.  Each argument is stored as a 1 byte
	type code followed by the argument data (see Table 12). */
	string streamContents;
};
struct PeriodicLogItem
{
	unsigned short itemSize;
	unsigned short keyCode;
	string data;
};

struct DLOG_PeriodicLog : DLOG_Record
{
	/* set ID */
	unsigned short setId;

	/* nodeID (this field is present only if network support is enabled). */
	unsigned int nodeId;

	/* item count */
	unsigned short itemCount;

	
	vector<PeriodicLogItem> periodicItemList;
};

struct DLOG_PeriodicSet : DLOG_Record
{
	/* 0x5505 */
	
	/* set ID */
	unsigned short setId;

	/* nodeID (this field is present only if network support is enabled). */
	unsigned int nodeId;

	/* item count */
	unsigned short itemCount;

	string setName;
};

struct DLOG_PeriodicSetItem : DLOG_Record
{
	/* 0x5506 (tag for task created record) */
	unsigned short keyCode;

	/* nodeID (this field is present only if network support is enabled). */
	unsigned int nodeId;

	unsigned short lenKeyName;
	unsigned short lenDescription;
	unsigned short lenFormat;

	string keyName;
	string description;
	string format;
};

struct DLOG_TaskCreated : DLOG_Record
{
	/* 0x5507 (tag for task created record) */

	/* LevelID */
	unsigned short levelId;

	/* TaskID */
	unsigned int taskId;

	/* nodeID (this field is present only if network support is enabled). */
	unsigned int nodeId;

	/* Length of task name, in characters, not including NULL terminator */
	unsigned short lenTaskName;

	/* task name, in ASCII, not including NULL terminator. */
	string taskName;
};

struct DLOG_TaskDeleted : DLOG_Record
{
	/* 0x5508 (tag for task created record) */
	
	/* LevelID */
	unsigned short levelId;

	/* TaskID */
	unsigned int taskId;

	/* nodeID (this field is present only if network support is enabled). */
	unsigned int nodeId;

};

struct DLOG_NetworkLogHeader : DLOG_Record
{
	/*   0x5509 (tag for time stamp record) */
	/* Node ID */
	unsigned int nodeId;
		/* Day log was created (1-31). */
	char day;
	/* Month log was created (1-12).*/
	char month;
	/* Year log was created (e.g. 2002). */
	unsigned short year;
	/* Hour log was created (0-23). */
	char hour;
	/* Minute log was created (0-59). */
	char minute;
	/* Second log was created (0-59). */
	char second;
	/* Size of node name (not including NULL byte) */
	unsigned short lenNodeName;
	/* Node name, in ASCII, not including NULL terminator */
	string nodeName;
};

struct DLOG_Binary : DLOG_Record
{
	/*   0x55f0 */
	unsigned int nItems;
	string binaryData;
};
//
struct DLOG_EndNetworkLog : DLOG_Record
{
	/* 0x55fc (tag for task created record) */

	
	/* nodeID (this field is present only if network support is enabled). */
	unsigned int nodeId;
};

struct DLOG_EndLog : DLOG_Record
{
	/* 0x55fd (tag for task created record) */

	
};

struct DLOG_TimeStamp : DLOG_Record
{
	/* 0x55fe (tag for task created record) */
};

struct DLOG_MissedData : DLOG_Record
{
	/* 0x55ff (tag for task created record) */
};

// enum DLOG_FormatFlags
// {
// 	/* Print explicit '+' on output of positive values. */
//     showpos		= 	0x0001,
//
// 	/* Print integers as decimal values. */
// 	dec			= 	0x0010,
//
// 	/* Print integers as hexadecimal values */
// 	hex			=	0x0020,
//
// 	/* Print integers as octal values */
// 	oct			=	0x0040,
//
// 	/* Show integer output base (e.g. 0x for hex) */
// 	showbase	=	0x0100,
//
// 	/* Show trailing zeroes after decimal point */
// 	showpoint	=	0x1000,
//
// 	/* Print floating point values in scientific notation (e.g. 1.234E02) */
// 	scientific	=	0x2000,
//
// 	/* Print floating point values in fixed point notation (e.g. 123.4) */
// 	fixed		=	0x4000
//
// };
//
// enum DLOG_StreamOutputTypes
// {
// 	/* DSO -> Datalog Stream Output*/
// 	/* 1	signed char	Binary data */
// 	DSO_SIGNED_CHAR = 1,
// 	/* 2	unsigned char	Binary data */
// 	DSO_UNSIGNED_CHAR,
// 	/* 3	signed int	Binary data */
// 	DSO_SIGNED_INT,
// 	/* 4	unsigned int	Binary data */
// 	DSO_UNSIGNED_INT,
// 	/* 5	signed long	Binary data */
// 	DSO_SIGNED_LONG,
// 	/* 6	unsigned long	Binary data */
// 	DSO_UNSIGNED_LONG,
// 	/* 7	string (char *)	2 bytes for string length, followed
// 	by the ASCII string data, not including NULL terminator */
// 	DSO_STRING,
// 	/* 8	float	Binary data */
// 	DSO_FLOAT,
// 	/* 9	double	Binary data */
// 	DSO_DOUBLE,
// 	/* 10	bool	1 byte (0=false, 1=true) */
// 	DSO_BOOL,
// 	/* 100	Change to flag settings	2 bytes for bitwise-OR of new settings (see Table 11) */
// 	DSO_CHANGE_TO_FLAG_SETTINGS = 100,
// 	/* 101	Change to precision setting	1 byte for new setting */
// 	DSO_CHANGE_TO_PRECISION_SETTING
// };
//
//
//
enum DLOG_Filter_Type
{
	
};
class DLOG_Filter
{
private:
	DLOG_PeriodicSet _currentPeriodicSet;
	/* store the set ids we need for easier search of periodic log records */
	std::set<int> _setIds; 
	/* periodic item map. Format - ( Description, ) */
	std::map<std::string, DLOG_PeriodicSetItem > _periodicItemMap; 
public:
	DLOG_Filter();
	~DLOG_Filter();
	/* Records will be excluded by default if include type is specified */
	/* exclude list will be ignored if include list is specified */
	std::vector<DLOG_Record::DLOG_RecordType> includeRecordTypes;
	/* Records will be included by default if exclude type is specified */
	std::vector<DLOG_Record::DLOG_RecordType> excludeRecordTypes;

	/* searches for the sub-string in stream records */
	std::vector<string> includeStrings;
	std::vector<string> excludeStrings;

	/* terminate on particular record Types reaching a certain recordCount */
	std::map<DLOG_Record::DLOG_RecordType, unsigned int> recordCountMap;

	/* Match all=0, one=1*/
	unsigned int recordCount;

	/* Read in the following periodic set items from periodic log records*/
	/* Eg. DonorHeight DonorBMI */
    std::vector<std::string> periodicItemNames;

	/* All the periodic logs corresponding to periodic item names */
	std::map<std::string, PeriodicLogItem*> periodicLogItems;

	bool add(DLOG_Record* record);
	bool DLOG_Filter::addPeriodicLog(DLOG_Record* record);

	/* Restrict search to the following Periodic sets */
	std::vector<std::string> periodicSetNames;
};

typedef vector<DLOG_Record*> DLOGREC_VECTOR;
typedef map<DLOG_Record::DLOG_RecordType, vector<DLOG_Record*> > DLOGREC_MAP;

class DLOG_Reader
{
public:
	/* path to dlog file */
	DLOG_Reader();
	virtual ~DLOG_Reader();
	/* MUST be invoked to initialize filter*/
	int initialize();
	bool terminate();
protected:
	bool add(DLOG_Record* dlogRecord);

	/* Ordered by time stamp */
	DLOGREC_VECTOR _dlogRecords;
	DLOGREC_MAP _recordMap;

	const char* _dlogName;
	DLOG_Filter _dlogFilter;
	bool _initialized;
};

#endif /* __LOG_CONVERT_H__ */

