/*****************************************************************************\
**
** DatabaseConverterUtil.h
**
** Extracts the datalog data into an database using ADO.
** Requirements: To use this class, you must initialize and uninitialize
** the COM interface using CoInitialize and CoUninitialize.
**
\*****************************************************************************/

#ifndef DATABASECONVERTERUTIL_H
#define DATABASECONVERTERUTIL_H

/* Includes ******************************************************************/

#import <c:\\Program Files\\Common Files\\system\\ado\\msadox.dll>
#import <C:\\Program Files\\Common Files\\System\\ado\\msado15.dll> rename( "EOF", "AdoNSEOF" )

#include <string>
#include <set>
#include <list>
#include <icrsint.h>
#include <sys/stat.h> 

#include "LogReader.h"

using namespace ADODB;

namespace CONVERT {

/* Constants *****************************************************************/

const int kMaxStringSize = 100;
const int kMaxTextSize = 1024;
const int kMaxBinarySize = 100;

enum {
	kTableLogInfo = 0,
	kTableLogHeader,
	kTableTaskCreated,
	kTableTaskDeleted,
	kTableTimeStamps,
	kTableEndOfFile,
	kTablePeriodicSet,
	kTableLogLevel,
	kTableStartNetCon,
	kTablePeriodicSetItem,
	kTablePeriodicLogInt,
	kTablePeriodicLogFloat,
	kTablePeriodicLogString,
	kTableTextOutput,
	kTableBinaryData,
	kTableCount
};

static const char *kStatementTableNames[kTableCount] = {
		"LogInfo",
		"LogHeader",
		"TaskCreated",
		"TaskDeleted",
		"TimeStamps",
		"EndOfFile",
		"PeriodicSet",
		"LogLevel",
		"StartNetCon",
		"PeriodicSetItem",
		"PeriodicLogInt",
		"PeriodicLogFloat",
		"PeriodicLogString",
		"TextOutput",
		"BinaryData"
};

static const char *kStatementCreateTable[kTableCount] = {
		"CREATE TABLE LogInfo ("
			"RecordIndex Integer,"
			"Message text"
		");",
		"CREATE TABLE LogHeader ("
			"RecordIndex Integer,"
			"SizeOfChar Integer," 
			"SizeOfInt Integer,"
			"SizeOfLong Integer,"
			"SizeOfFloat Integer,"
			"SizeOfDouble Integer,"
			"SizeOfTaskID Integer,"
			"SizeOfNodeID Integer,"
			"LogVersion Integer,"
			"PlatformName varchar(100),"
			"NodeName varchar(100),"
			"LogTaskID Long,"
			"LogNodeID Long,"
			"TimeDay Long,"
			"TimeMonth Long,"
			"TimeYear Integer,"
			"TimeHour Long,"
			"TimeMin Long,"
			"TimeSec Long"
		");",
		"CREATE TABLE TaskCreated ("
			"RecordIndex Integer,"
			"TimeSeconds Long," 
			"TimeNanosec Long,"
			"LevelID Integer,"
			"TaskID Long,"
			"NodeID Long,"
			"TaskName varchar(100)"
		");",
		"CREATE TABLE TaskDeleted ("
			"RecordIndex Integer,"
			"TimeSeconds Long," 
			"TimeNanosec Long,"
			"LevelID Integer,"
			"TaskID Long,"
			"NodeID Long"
		");",
		"CREATE TABLE TimeStamps ("
			"RecordIndex Integer,"
			"TimeSeconds Long," 
			"TimeNanosec Long"
		");",
		"CREATE TABLE EndOfFile ("
			"RecordIndex Integer,"
			"TimeSeconds Long," 
			"TimeNanosec Long"
		");",
		"CREATE TABLE PeriodicSet ("
			"RecordIndex Integer,"
			"TimeSeconds Long," 
			"TimeNanosec Long,"
			"SetID Integer,"
			"NodeID Long,"
			"SetName varchar(100)"
		");",
		"CREATE TABLE LogLevel ("
			"RecordIndex Integer,"
			"TimeSeconds Long," 
			"TimeNanosec Long,"
			"LevelID Integer,"
			"NodeID Long,"
			"LevelName varchar(100)"
		");",
		"CREATE TABLE StartNetCon ("
			"RecordIndex Integer,"
			"NodeID Long," 
			"TimeDay Long,"
			"TimeMonth Long,"
			"TimeYear Integer,"
			"TimeHour Long,"
			"TimeMin Long,"
			"TimeSec Long,"
			"Name varchar(100)"
		");",
		"CREATE TABLE PeriodicSetItem ("
			"RecordIndex Integer,"
			"TimeSeconds Long," 
			"TimeNanosec Long,"
			"KeyCode Integer,"
			"NodeID Long,"
			"Name varchar(100),"
			"Description varchar(100),"
			"Format varchar(100)"
		");",
		"CREATE TABLE PeriodicLogInt ("
			"RecordIndex Integer,"
			"TimeSeconds Long," 
			"TimeNanosec Long,"
			"SetID Integer,"
			"NodeID Long,"
			"KeyCode Integer,"
			"Data Long"
		");",
		"CREATE TABLE PeriodicLogFloat ("
			"RecordIndex Integer,"
			"TimeSeconds Long," 
			"TimeNanosec Long,"
			"SetID Integer,"
			"NodeID Long,"
			"KeyCode Integer,"
			"Data Float"
		");",
		"CREATE TABLE PeriodicLogString ("
			"RecordIndex Integer,"
			"TimeSeconds Long," 
			"TimeNanosec Long,"
			"SetID Integer,"
			"NodeID Long,"
			"KeyCode Integer,"
			"Data varchar(100)"
		");",
		"CREATE TABLE TextOutput ("
			"RecordIndex Integer,"
			"TimeSeconds Long," 
			"TimeNanosec Long,"
			"LevelID Integer,"
			"TaskID Long,"
			"NodeID Long,"
			"File varchar(100),"
			"Line Integer,"
			"Message text"
		");",
		"CREATE TABLE BinaryData ("
			"RecordIndex Integer,"
			"SectionIndex Integer,"
			"SectionSize Integer,"
			"Data varbinary(100)"
		");"
};

static const char *kStatementClearTable[kTableCount] = {
		"DELETE * FROM LogInfo",
		"DELETE * FROM LogHeader",
		"DELETE * FROM TaskCreated",
		"DELETE * FROM TaskDeleted",
		"DELETE * FROM TimeStamps",
		"DELETE * FROM EndOfFile",
		"DELETE * FROM PeriodicSet",
		"DELETE * FROM LogLevel",
		"DELETE * FROM StartNetCon",
		"DELETE * FROM PeriodicSetItem",
		"DELETE * FROM PeriodicLogInt",
		"DELETE * FROM PeriodicLogFloat",
		"DELETE * FROM PeriodicLogString",
		"DELETE * FROM TextOutput",
		"DELETE * FROM BinaryData"
};

static const char *kStatementDropTable[kTableCount] = {
		"DROP TABLE LogInfo",
		"DROP TABLE LogHeader",
		"DROP TABLE TaskCreated",
		"DROP TABLE TaskDeleted",
		"DROP TABLE TimeStamps",
		"DROP TABLE EndOfFile",
		"DROP TABLE PeriodicSet",
		"DROP TABLE LogLevel",
		"DROP TABLE StartNetCon",
		"DROP TABLE PeriodicSetItem",
		"DROP TABLE PeriodicLogInt",
		"DROP TABLE PeriodicLogFloat",
		"DROP TABLE PeriodicLogString",
		"DROP TABLE TextOutput",
		"DROP TABLE BinaryData"
};

/* Types *********************************************************************/

/* Functions *****************************************************************/

inline void TESTHR(HRESULT x) 
{ 
	if FAILED(x) _com_issue_error(x); 
};

inline void COPYSTRING(char *dest, const char *src)
{
	const char escapeCharList[] = { 4, 10, 13, 26, 0 };

	char *results = std::strncpy(dest, src, kMaxStringSize);
   results[kMaxStringSize - 1] = NULL;
	char *pointer = std::strpbrk(results, escapeCharList);

	while(pointer != NULL) {
		*pointer = 32;
		pointer = std::strpbrk(++pointer, escapeCharList);
	}
}

inline void COPYTEXT(char *dest, const char *src)
{
	const char escapeCharList[] = { 4, 10, 13, 26, 0 };

	char *results = std::strncpy(dest, src, kMaxTextSize);
   results[kMaxTextSize - 1] = NULL;
	char *pointer = std::strpbrk(results, escapeCharList);

	while(pointer != NULL) {
		*pointer = 32;
		pointer = std::strpbrk(++pointer, escapeCharList);
	}
}

inline void COPYCHARS(char *dest, const char *src, int count, int max)
{
	const char escapeCharList[] = { 4, 10, 13, 26, 0 };

	if (count > max - 2) {
		count = max - 2;
	}

	char *results = std::strncpy(dest, src, count);
   results[count] = NULL;
	char *pointer = std::strpbrk(results, escapeCharList);

	while(pointer != NULL) {
		*pointer = 32;
		pointer = std::strpbrk(++pointer, escapeCharList);
	}
}

/* Classes *******************************************************************/

class RecordLogInfo : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordLogInfo)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(2, ADODB::adVarChar, Message, sizeof(Message), MessageStatus, TRUE)
   END_ADO_BINDING()

public:
 	INT RecordIndex;
   CHAR Message[kMaxTextSize];

	ULONG RecordIndexStatus;
   ULONG MessageStatus;
};

class RecordLogHeader : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordLogHeader)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, SizeOfChar, 10, 10, SizeOfCharStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, SizeOfInt, 10, 10, SizeOfIntStatus, TRUE)
	ADO_NUMERIC_ENTRY(4, ADODB::adInteger, SizeOfLong, 10, 10, SizeOfLongStatus, TRUE)
	ADO_NUMERIC_ENTRY(5, ADODB::adInteger, SizeOfFloat, 10, 10, SizeOfFloatStatus, TRUE)
	ADO_NUMERIC_ENTRY(6, ADODB::adInteger, SizeOfDouble, 10, 10, SizeOfDoubleStatus, TRUE)
	ADO_NUMERIC_ENTRY(7, ADODB::adInteger, SizeOfTaskID, 10, 10, SizeOfTaskIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(8, ADODB::adInteger, SizeOfNodeID, 10, 10, SizeOfNodeIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(9, ADODB::adInteger, LogVersion, 10, 10, LogVersionStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(10, ADODB::adVarChar, PlatformName, sizeof(PlatformName), PlatformNameStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(11, ADODB::adVarChar, NodeName, sizeof(NodeName), NodeNameStatus, TRUE)
	ADO_NUMERIC_ENTRY(12, ADODB::adInteger, LogTaskID, 10, 10, LogTaskIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(13, ADODB::adInteger, LogNodeID, 10, 10, LogNodeIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(14, ADODB::adInteger, TimeDay, 10, 10, TimeDayStatus, TRUE)
	ADO_NUMERIC_ENTRY(15, ADODB::adInteger, TimeMonth, 10, 10, TimeMonthStatus, TRUE)
	ADO_NUMERIC_ENTRY(16, ADODB::adInteger, TimeYear, 10, 10, TimeYearStatus, TRUE)
	ADO_NUMERIC_ENTRY(17, ADODB::adInteger, TimeHour, 10, 10, TimeHourStatus, TRUE)
	ADO_NUMERIC_ENTRY(18, ADODB::adInteger, TimeMin, 10, 10, TimeMinStatus, TRUE)
	ADO_NUMERIC_ENTRY(19, ADODB::adInteger, TimeSec, 10, 10, TimeSecStatus, TRUE)
   END_ADO_BINDING()

public:
 	INT RecordIndex;
 	INT SizeOfChar;
	INT SizeOfInt;
	INT SizeOfLong;
	INT SizeOfFloat;
	INT SizeOfDouble;
	INT SizeOfTaskID;
	INT SizeOfNodeID;
	INT LogVersion;
   CHAR PlatformName[kMaxStringSize];
   CHAR NodeName[kMaxStringSize];
	LONG LogTaskID;
	LONG LogNodeID;
	LONG TimeDay;
	LONG TimeMonth;
	INT TimeYear;
	LONG TimeHour;
	LONG TimeMin;
	LONG TimeSec;

	ULONG RecordIndexStatus;
   ULONG SizeOfCharStatus;
	ULONG SizeOfIntStatus;
	ULONG SizeOfLongStatus;
	ULONG SizeOfFloatStatus;
	ULONG SizeOfDoubleStatus;
	ULONG SizeOfTaskIDStatus;
	ULONG SizeOfNodeIDStatus;
	ULONG LogVersionStatus;
	ULONG PlatformNameStatus;
	ULONG NodeNameStatus;
	ULONG LogTaskIDStatus;
	ULONG LogNodeIDStatus;
	ULONG TimeDayStatus;
	ULONG TimeMonthStatus;
	ULONG TimeYearStatus;
	ULONG TimeHourStatus;
	ULONG TimeMinStatus;
	ULONG TimeSecStatus;
};

class RecordTaskCreated : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordTaskCreated)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, TimeSeconds, 10, 10, TimeSecondsStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, TimeNanosec, 10, 10, TimeNanosecStatus, TRUE)
	ADO_NUMERIC_ENTRY(4, ADODB::adInteger, LevelID, 10, 10, LevelIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(5, ADODB::adInteger, TaskID, 10, 10, TaskIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(6, ADODB::adInteger, NodeID, 10, 10, NodeIDStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(7, ADODB::adVarChar, TaskName, sizeof(TaskName), TaskNameStatus, TRUE)
   END_ADO_BINDING()

public:
 	INT RecordIndex;
 	LONG TimeSeconds;
	LONG TimeNanosec;
	INT LevelID;
	LONG TaskID;
	LONG NodeID;
   CHAR TaskName[kMaxStringSize];

	ULONG RecordIndexStatus;
   ULONG TimeSecondsStatus;
   ULONG TimeNanosecStatus;
   ULONG LevelIDStatus;
   ULONG TaskIDStatus;
   ULONG NodeIDStatus;
   ULONG TaskNameStatus;
};

class RecordTaskDeleted : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordTaskDeleted)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, TimeSeconds, 10, 10, TimeSecondsStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, TimeNanosec, 10, 10, TimeNanosecStatus, TRUE)
	ADO_NUMERIC_ENTRY(4, ADODB::adInteger, LevelID, 10, 10, LevelIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(5, ADODB::adInteger, TaskID, 10, 10, TaskIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(6, ADODB::adInteger, NodeID, 10, 10, NodeIDStatus, TRUE)
	END_ADO_BINDING()

public:
 	INT RecordIndex;
 	LONG TimeSeconds;
	LONG TimeNanosec;
	INT LevelID;
	LONG TaskID;
	LONG NodeID;

	ULONG RecordIndexStatus;
   ULONG TimeSecondsStatus;
   ULONG TimeNanosecStatus;
   ULONG LevelIDStatus;
   ULONG TaskIDStatus;
   ULONG NodeIDStatus;
};

class RecordTimeStamps : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordTimeStamps)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, TimeSeconds, 10, 10, TimeSecondsStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, TimeNanosec, 10, 10, TimeNanosecStatus, TRUE)
	END_ADO_BINDING()

public:
 	INT RecordIndex;
 	LONG TimeSeconds;
	LONG TimeNanosec;

	ULONG RecordIndexStatus;
   ULONG TimeSecondsStatus;
   ULONG TimeNanosecStatus;
};

class RecordEndOfFile : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordEndOfFile)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, TimeSeconds, 10, 10, TimeSecondsStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, TimeNanosec, 10, 10, TimeNanosecStatus, TRUE)
	END_ADO_BINDING()

public:
 	INT RecordIndex;
 	LONG TimeSeconds;
	LONG TimeNanosec;

	ULONG RecordIndexStatus;
   ULONG TimeSecondsStatus;
   ULONG TimeNanosecStatus;
};

class RecordPeriodicSet : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordPeriodicSet)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, TimeSeconds, 10, 10, TimeSecondsStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, TimeNanosec, 10, 10, TimeNanosecStatus, TRUE)
	ADO_NUMERIC_ENTRY(4, ADODB::adInteger, SetID, 10, 10, SetIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(5, ADODB::adInteger, NodeID, 10, 10, NodeIDStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(6, ADODB::adVarChar, SetName, sizeof(SetName),SetNameStatus, TRUE)
   END_ADO_BINDING()

public:
 	INT RecordIndex;
 	LONG TimeSeconds;
	LONG TimeNanosec;
	INT SetID;
	LONG NodeID;
   CHAR SetName[kMaxStringSize];

	ULONG RecordIndexStatus;
   ULONG TimeSecondsStatus;
   ULONG TimeNanosecStatus;
   ULONG SetIDStatus;
   ULONG NodeIDStatus;
   ULONG SetNameStatus;
};

class RecordLogLevel : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordLogLevel)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, TimeSeconds, 10, 10, TimeSecondsStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, TimeNanosec, 10, 10, TimeNanosecStatus, TRUE)
	ADO_NUMERIC_ENTRY(4, ADODB::adInteger, LevelID, 10, 10, LevelIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(5, ADODB::adInteger, NodeID, 10, 10, NodeIDStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(6, ADODB::adVarChar, LevelName, sizeof(LevelName),LevelNameStatus, TRUE)
   END_ADO_BINDING()

public:
 	INT RecordIndex;
 	LONG TimeSeconds;
	LONG TimeNanosec;
	INT LevelID;
	LONG NodeID;
   CHAR LevelName[kMaxStringSize];

	ULONG RecordIndexStatus;
   ULONG TimeSecondsStatus;
   ULONG TimeNanosecStatus;
   ULONG LevelIDStatus;
   ULONG NodeIDStatus;
   ULONG LevelNameStatus;
};

class RecordStartNetCon : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordStartNetCon)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, NodeID, 10, 10, NodeIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, TimeDay, 10, 10, TimeDayStatus, TRUE)
	ADO_NUMERIC_ENTRY(4, ADODB::adInteger, TimeMonth, 10, 10, TimeMonthStatus, TRUE)
	ADO_NUMERIC_ENTRY(5, ADODB::adInteger, TimeYear, 10, 10, TimeYearStatus, TRUE)
	ADO_NUMERIC_ENTRY(6, ADODB::adInteger, TimeHour, 10, 10, TimeHourStatus, TRUE)
	ADO_NUMERIC_ENTRY(7, ADODB::adInteger, TimeMin, 10, 10, TimeMinStatus, TRUE)
	ADO_NUMERIC_ENTRY(8, ADODB::adInteger, TimeSec, 10, 10, TimeSecStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(9, ADODB::adVarChar, Name, sizeof(Name), NameStatus, TRUE)
   END_ADO_BINDING()

public:
 	INT RecordIndex;
 	LONG NodeID;
	LONG TimeDay;
	LONG TimeMonth;
	INT TimeYear;
	LONG TimeHour;
	LONG TimeMin;
	LONG TimeSec;
   CHAR Name[kMaxStringSize];

	ULONG RecordIndexStatus;
   ULONG NodeIDStatus;
   ULONG TimeDayStatus;
   ULONG TimeMonthStatus;
   ULONG TimeYearStatus;
   ULONG TimeHourStatus;
   ULONG TimeMinStatus;
   ULONG TimeSecStatus;
   ULONG NameStatus;
};

class RecordPeriodicSetItem : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordPeriodicSetItem)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, TimeSeconds, 10, 10, TimeSecondsStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, TimeNanosec, 10, 10, TimeNanosecStatus, TRUE)
	ADO_NUMERIC_ENTRY(4, ADODB::adInteger, KeyCode, 10, 10, KeyCodeStatus, TRUE)
	ADO_NUMERIC_ENTRY(5, ADODB::adInteger, NodeID, 10, 10, NodeIDStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(6, ADODB::adVarChar, Name, sizeof(Name), NameStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(7, ADODB::adVarChar, Description, sizeof(Description), DescriptionStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(8, ADODB::adVarChar, Format, sizeof(Format), FormatStatus, TRUE)
   END_ADO_BINDING()

public:
 	INT RecordIndex;
 	LONG TimeSeconds;
	LONG TimeNanosec;
	INT KeyCode;
	LONG NodeID;
   CHAR Name[kMaxStringSize];
   CHAR Description[kMaxStringSize];
   CHAR Format[kMaxStringSize];

	ULONG RecordIndexStatus;
   ULONG TimeSecondsStatus;
   ULONG TimeNanosecStatus;
   ULONG KeyCodeStatus;
   ULONG NodeIDStatus;
   ULONG NameStatus;
   ULONG DescriptionStatus;
   ULONG FormatStatus;
};

class RecordPeriodicLogInt : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordPeriodicLogInt)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, TimeSeconds, 10, 10, TimeSecondsStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, TimeNanosec, 10, 10, TimeNanosecStatus, TRUE)
	ADO_NUMERIC_ENTRY(4, ADODB::adInteger, SetID, 10, 10, SetIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(5, ADODB::adInteger, NodeID, 10, 10, NodeIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(6, ADODB::adInteger, KeyCode, 10, 10, KeyCodeStatus, TRUE)
	ADO_NUMERIC_ENTRY(7, ADODB::adInteger, Data, 10, 10, DataStatus, TRUE)
   END_ADO_BINDING()

public:
 	INT RecordIndex;
 	LONG TimeSeconds;
	LONG TimeNanosec;
	INT SetID;
	LONG NodeID;
	INT KeyCode;
	LONG Data;

	ULONG RecordIndexStatus;
   ULONG TimeSecondsStatus;
   ULONG TimeNanosecStatus;
   ULONG SetIDStatus;
   ULONG NodeIDStatus;
   ULONG KeyCodeStatus;
   ULONG DataStatus;
};

class RecordPeriodicLogFloat : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordPeriodicLogFloat)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, TimeSeconds, 10, 10, TimeSecondsStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, TimeNanosec, 10, 10, TimeNanosecStatus, TRUE)
	ADO_NUMERIC_ENTRY(4, ADODB::adInteger, SetID, 10, 10, SetIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(5, ADODB::adInteger, NodeID, 10, 10, NodeIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(6, ADODB::adInteger, KeyCode, 10, 10, KeyCodeStatus, TRUE)
	ADO_NUMERIC_ENTRY(7, ADODB::adSingle, Data, 10, 10, DataStatus, TRUE)
   END_ADO_BINDING()

public:
 	INT RecordIndex;
 	LONG TimeSeconds;
	LONG TimeNanosec;
	INT SetID;
	LONG NodeID;
	INT KeyCode;
	FLOAT Data;

	ULONG RecordIndexStatus;
   ULONG TimeSecondsStatus;
   ULONG TimeNanosecStatus;
   ULONG SetIDStatus;
   ULONG NodeIDStatus;
   ULONG KeyCodeStatus;
   ULONG DataStatus;
};

class RecordPeriodicLogString : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordPeriodicLogString)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, TimeSeconds, 10, 10, TimeSecondsStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, TimeNanosec, 10, 10, TimeNanosecStatus, TRUE)
	ADO_NUMERIC_ENTRY(4, ADODB::adInteger, SetID, 10, 10, SetIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(5, ADODB::adInteger, NodeID, 10, 10, NodeIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(6, ADODB::adInteger, KeyCode, 10, 10, KeyCodeStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(7, ADODB::adVarChar, Data, sizeof(Data), DataStatus, TRUE)
   END_ADO_BINDING()

public:
 	INT RecordIndex;
 	LONG TimeSeconds;
	LONG TimeNanosec;
	INT SetID;
	LONG NodeID;
	INT KeyCode;
   CHAR Data[kMaxStringSize];

	ULONG RecordIndexStatus;
   ULONG TimeSecondsStatus;
   ULONG TimeNanosecStatus;
   ULONG SetIDStatus;
   ULONG NodeIDStatus;
   ULONG KeyCodeStatus;
   ULONG DataStatus;
};

class RecordTextOutput : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordTextOutput)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, TimeSeconds, 10, 10, TimeSecondsStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, TimeNanosec, 10, 10, TimeNanosecStatus, TRUE)
	ADO_NUMERIC_ENTRY(4, ADODB::adInteger, LevelID, 10, 10, LevelIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(5, ADODB::adInteger, TaskID, 10, 10, TaskIDStatus, TRUE)
	ADO_NUMERIC_ENTRY(6, ADODB::adInteger, NodeID, 10, 10, NodeIDStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(7, ADODB::adVarChar, File, sizeof(File), FileStatus, TRUE)
	ADO_NUMERIC_ENTRY(8, ADODB::adInteger, Line, 10, 10, LineStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(9, ADODB::adVarChar, Message, sizeof(Message), MessageStatus, TRUE)
   END_ADO_BINDING()

public:
 	INT RecordIndex;
 	LONG TimeSeconds;
	LONG TimeNanosec;
	INT LevelID;
	LONG TaskID;
	LONG NodeID;
   CHAR File[kMaxStringSize];
	INT Line;
   CHAR Message[kMaxTextSize];
	
	ULONG RecordIndexStatus;
   ULONG TimeSecondsStatus;
   ULONG TimeNanosecStatus;
   ULONG LevelIDStatus;
   ULONG TaskIDStatus;
   ULONG NodeIDStatus;
   ULONG FileStatus;
   ULONG LineStatus;
   ULONG MessageStatus;
};

class RecordBinaryData : public CADORecordBinding {
   BEGIN_ADO_BINDING(RecordBinaryData)
	ADO_NUMERIC_ENTRY(1, ADODB::adInteger, RecordIndex, 10, 10, RecordIndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(2, ADODB::adInteger, Index, 10, 10, IndexStatus, TRUE)
	ADO_NUMERIC_ENTRY(3, ADODB::adInteger, Size, 10, 10, SizeStatus, TRUE)
	ADO_VARIABLE_LENGTH_ENTRY2(4, ADODB::adVarBinary, Data, sizeof(Data), DataStatus, TRUE)
   END_ADO_BINDING()

public:
 	INT RecordIndex;
	INT Index;
	INT Size;
   BYTE Data[kMaxBinarySize];

	ULONG RecordIndexStatus;
   ULONG IndexStatus;
   ULONG SizeStatus;
   ULONG DataStatus;
};

class DatalogDatabaseConverter {
public:

	/**
	 *
	 * DatalogDatabaseConverter
	 *
	 * Constructor
	 *
	 */
	DatalogDatabaseConverter(const DECODER::String &file) :
			mDatabaseFile(file), 
			mConnectionP("ADODB.Connection"),
			mRecordIndex(0)
	{
	}

	/**
	 *
	 * ConnectToDatabase
	 *
	 * Connects to the database.
	 *
	 */
	bool ConnectToDatabase(void)
	{
		bool result = false;

		if (this->IsDatabaseCreationRequired()) {
			try {
				ADOX::_CatalogPtr catalog = NULL;
				std::string command = this->GetConnectionString();
				ADODB::_ConnectionPtr connectionP;
				
				catalog.CreateInstance(__uuidof (ADOX::Catalog));
				catalog->Create(command.c_str());
				connectionP = catalog->GetActiveConnection();
				connectionP->Close();
				catalog.Release();
			}
			catch(_com_error &e) {
				std::cout << e.ErrorMessage();
			}
		}

		try {
			std::string command = this->GetConnectionString();
			std::set<std::string> tableList;
			ADODB::_RecordsetPtr tableSchemaPtr = NULL;

			TESTHR(mConnectionP->Open(command.c_str(), "", "", ADODB::adConnectUnspecified));
			tableSchemaPtr = mConnectionP->OpenSchema(ADODB::adSchemaTables);

			while (!tableSchemaPtr->GetAdoNSEOF()) {
				_bstr_t name = tableSchemaPtr->Fields->GetItem("TABLE_NAME")->Value;
				tableList.insert((LPCSTR)(name));
				tableSchemaPtr->MoveNext();      
			}

			if (tableSchemaPtr != NULL) {
				tableSchemaPtr->Close();
			}

			for (int i = 0; i < kTableCount; ++i) {
				if (tableList.find(kStatementTableNames[i]) == tableList.end()) {
					mConnectionP->Execute(kStatementCreateTable[i], NULL, 1);
				}
				else if (this->IsRecordDeletionSupported()){
					mConnectionP->Execute(kStatementClearTable[i], NULL, 1);
				}
				else {
					mConnectionP->Execute(kStatementDropTable[i], NULL, 1);
					mConnectionP->Execute(kStatementCreateTable[i], NULL, 1);
				}
			}

			this->CreateBinding(kStatementTableNames[kTableLogInfo], 
					mTableLogInfo, mBindingLogInfo, mRecordLogInfo);
			this->CreateBinding(kStatementTableNames[kTableLogHeader], 
					mTableLogHeader, mBindingLogHeader, mRecordLogHeader);
			this->CreateBinding(kStatementTableNames[kTableTaskCreated], 
					mTableTaskCreated, mBindingTaskCreated, mRecordTaskCreated);
			this->CreateBinding(kStatementTableNames[kTableTaskDeleted], 
					mTableTaskDeleted, mBindingTaskDeleted, mRecordTaskDeleted);
			this->CreateBinding(kStatementTableNames[kTableTimeStamps], 
					mTableTimeStamps, mBindingTimeStamps, mRecordTimeStamps);
			this->CreateBinding(kStatementTableNames[kTableEndOfFile], 
					mTableEndOfFile, mBindingEndOfFile, mRecordEndOfFile);
			this->CreateBinding(kStatementTableNames[kTablePeriodicSet], 
					mTablePeriodicSet, mBindingPeriodicSet, mRecordPeriodicSet);
			this->CreateBinding(kStatementTableNames[kTableLogLevel], 
					mTableLogLevel, mBindingLogLevel, mRecordLogLevel);
			this->CreateBinding(kStatementTableNames[kTableStartNetCon], 
					mTableStartNetCon, mBindingStartNetCon, mRecordStartNetCon);
			this->CreateBinding(kStatementTableNames[kTablePeriodicSetItem], 
					mTablePeriodicSetItem, mBindingPeriodicSetItem, mRecordPeriodicSetItem);
			this->CreateBinding(kStatementTableNames[kTablePeriodicLogInt], 
					mTablePeriodicLogInt, mBindingPeriodicLogInt, mRecordPeriodicLogInt);
			this->CreateBinding(kStatementTableNames[kTablePeriodicLogFloat], 
					mTablePeriodicLogFloat, mBindingPeriodicLogFloat, mRecordPeriodicLogFloat);
			this->CreateBinding(kStatementTableNames[kTablePeriodicLogString], 
					mTablePeriodicLogString, mBindingPeriodicLogString, mRecordPeriodicLogString);
			this->CreateBinding(kStatementTableNames[kTableTextOutput], 
					mTableTextOutput, mBindingTextOutput, mRecordTextOutput);
			this->CreateBinding(kStatementTableNames[kTableBinaryData], 
					mTableBinaryData, mBindingBinaryData, mRecordBinaryData);

			mItemFormats.Clear();
			mRecordIndex = 0;

			result = true;
		}
		catch(_com_error &e) {
			this->LogInformation(e.ErrorMessage());
		}

		return result;
	}

	/**
	 *
	 * DisconnectFromDatabase
	 *
	 * Connects to the database.
	 *
	 */
	bool DisconnectFromDatabase(void)
	{
		bool result = false;

		try {
			this->ReleaseBinding(kStatementTableNames[kTableLogInfo], 
					mTableLogInfo, mBindingLogInfo);
			this->ReleaseBinding(kStatementTableNames[kTableLogHeader], 
					mTableLogHeader, mBindingLogHeader);
			this->ReleaseBinding(kStatementTableNames[kTableTaskCreated], 
					mTableTaskCreated, mBindingTaskCreated);
			this->ReleaseBinding(kStatementTableNames[kTableTaskDeleted], 
					mTableTaskDeleted, mBindingTaskDeleted);
			this->ReleaseBinding(kStatementTableNames[kTableTimeStamps], 
					mTableTimeStamps, mBindingTimeStamps);
			this->ReleaseBinding(kStatementTableNames[kTableEndOfFile], 
					mTableEndOfFile, mBindingEndOfFile);
			this->ReleaseBinding(kStatementTableNames[kTablePeriodicSet], 
					mTablePeriodicSet, mBindingPeriodicSet);
			this->ReleaseBinding(kStatementTableNames[kTableLogLevel], 
					mTableLogLevel, mBindingLogLevel);
			this->ReleaseBinding(kStatementTableNames[kTableStartNetCon], 
					mTableStartNetCon, mBindingStartNetCon);
			this->ReleaseBinding(kStatementTableNames[kTablePeriodicSetItem], 
					mTablePeriodicSetItem, mBindingPeriodicSetItem);
			this->ReleaseBinding(kStatementTableNames[kTablePeriodicLogInt], 
					mTablePeriodicLogInt, mBindingPeriodicLogInt);
			this->ReleaseBinding(kStatementTableNames[kTablePeriodicLogFloat], 
					mTablePeriodicLogFloat, mBindingPeriodicLogFloat);
			this->ReleaseBinding(kStatementTableNames[kTablePeriodicLogString], 
					mTablePeriodicLogString, mBindingPeriodicLogString);
			this->ReleaseBinding(kStatementTableNames[kTableTextOutput], 
					mTableTextOutput, mBindingTextOutput);
			this->ReleaseBinding(kStatementTableNames[kTableBinaryData], 
					mTableBinaryData, mBindingBinaryData);

			if (mConnectionP && (mConnectionP->GetState() == ADODB::adStateOpen)) {
				mConnectionP->Close();		
			}

			mItemFormats.Clear();

			result = true;
		}
		catch(_com_error &e) {
			this->LogInformation(e.ErrorMessage());
		}

		return result;
	}

	void operator()(const DECODER::RecordLogInfo &data) 
	{
		try {
			mRecordLogInfo.RecordIndex = ++mRecordIndex;
			COPYTEXT(mRecordLogInfo.Message, data.mMessage.c_str());

			mRecordLogInfo.RecordIndexStatus = 0;
			mRecordLogInfo.MessageStatus = 0;

			TESTHR(mBindingLogInfo->AddNew(&mRecordLogInfo));
		}
		catch (_com_error &e) {
			this->LogInformation("Error: Unable to add LogInfo record");
		}
	}

	void operator()(const DECODER::RecordLogHeader &data) 
	{
		try {
			mRecordLogHeader.RecordIndex = ++mRecordIndex;
			mRecordLogHeader.SizeOfChar = data.mSizeOfChar;
			mRecordLogHeader.SizeOfInt = data.mSizeOfInt;
			mRecordLogHeader.SizeOfLong = data.mSizeOfLong;
			mRecordLogHeader.SizeOfFloat = data.mSizeOfFloat;
			mRecordLogHeader.SizeOfDouble = data.mSizeOfDouble;
			mRecordLogHeader.SizeOfTaskID = data.mSizeOfTaskID;
			mRecordLogHeader.SizeOfNodeID = data.mSizeOfNodeID;
			mRecordLogHeader.LogVersion = data.mLogVersion;
			COPYSTRING(mRecordLogHeader.PlatformName, data.mPlatformName.c_str());
			COPYSTRING(mRecordLogHeader.NodeName, data.mNodeName.c_str());
			mRecordLogHeader.LogTaskID = data.mLogTaskID;
			mRecordLogHeader.LogNodeID = data.mLogNodeID;
			mRecordLogHeader.TimeDay = data.mDay;
			mRecordLogHeader.TimeMonth = data.mMonth;
			mRecordLogHeader.TimeYear = data.mYear;
			mRecordLogHeader.TimeHour = data.mHour;
			mRecordLogHeader.TimeMin = data.mMin;
			mRecordLogHeader.TimeSec = data.mSec;

			mRecordLogHeader.RecordIndexStatus = 0;
			mRecordLogHeader.SizeOfCharStatus = 0;
			mRecordLogHeader.SizeOfIntStatus = 0;
			mRecordLogHeader.SizeOfLongStatus = 0;
			mRecordLogHeader.SizeOfFloatStatus = 0;
			mRecordLogHeader.SizeOfDoubleStatus = 0;
			mRecordLogHeader.SizeOfTaskIDStatus = 0;
			mRecordLogHeader.SizeOfNodeIDStatus = 0;
			mRecordLogHeader.LogVersionStatus = 0;
			mRecordLogHeader.PlatformNameStatus = 0;
			mRecordLogHeader.NodeNameStatus = 0;
			mRecordLogHeader.LogTaskIDStatus = 0;
			mRecordLogHeader.LogNodeIDStatus = 0;
			mRecordLogHeader.TimeDayStatus = 0;
			mRecordLogHeader.TimeMonthStatus = 0;
			mRecordLogHeader.TimeYearStatus = 0;
			mRecordLogHeader.TimeHourStatus = 0;
			mRecordLogHeader.TimeMinStatus = 0;
			mRecordLogHeader.TimeSecStatus = 0;

			TESTHR(mBindingLogHeader->AddNew(&mRecordLogHeader));
		}
		catch (_com_error &e) {
			this->LogInformation("Error: Unable to add LogHeader record");
		}
	}

	void operator()(const DECODER::RecordTaskCreated &data) 
	{
		try {
			mRecordTaskCreated.RecordIndex = ++mRecordIndex;
			mRecordTaskCreated.TimeSeconds = data.mTimeSeconds;
			mRecordTaskCreated.TimeNanosec = data.mTimeNanosec;
			mRecordTaskCreated.LevelID = data.mLevelID;
			mRecordTaskCreated.TaskID = data.mTaskID;
			mRecordTaskCreated.NodeID = data.mNodeID;
			COPYSTRING(mRecordTaskCreated.TaskName, data.mTaskName.c_str());

			mRecordTaskCreated.RecordIndexStatus = 0;
			mRecordTaskCreated.TimeSecondsStatus = 0;
			mRecordTaskCreated.TimeNanosecStatus = 0;
			mRecordTaskCreated.LevelIDStatus = 0;
			mRecordTaskCreated.TaskIDStatus = 0;
			mRecordTaskCreated.NodeIDStatus = 0;
			mRecordTaskCreated.TaskNameStatus = 0;

			TESTHR(mBindingTaskCreated->AddNew(&mRecordTaskCreated));
		}
		catch (_com_error &e) {
			this->LogInformation("Error: Unable to add TaskCreated record");
		}
	}

	void operator()(const DECODER::RecordTaskDeleted &data) 
	{
		try {
			mRecordTaskDeleted.RecordIndex = ++mRecordIndex;
			mRecordTaskDeleted.TimeSeconds = data.mTimeSeconds;
			mRecordTaskDeleted.TimeNanosec = data.mTimeNanosec;
			mRecordTaskDeleted.LevelID = data.mLevelID;
			mRecordTaskDeleted.TaskID = data.mTaskID;
			mRecordTaskDeleted.NodeID = data.mNodeID;

			mRecordTaskDeleted.RecordIndexStatus = 0;
			mRecordTaskDeleted.TimeSecondsStatus = 0;
			mRecordTaskDeleted.TimeNanosecStatus = 0;
			mRecordTaskDeleted.LevelIDStatus = 0;
			mRecordTaskDeleted.TaskIDStatus = 0;
			mRecordTaskDeleted.NodeIDStatus = 0;

			TESTHR(mBindingTaskDeleted->AddNew(&mRecordTaskDeleted));
		}
		catch (_com_error &e) {
			this->LogInformation("Error: Unable to add TaskDeleted record");
		}
	}

	void operator()(const DECODER::RecordTimeStamp &data) 
	{
		try {
			mRecordTimeStamps.RecordIndex = ++mRecordIndex;
			mRecordTimeStamps.TimeSeconds = data.mTimeSeconds;
			mRecordTimeStamps.TimeNanosec = data.mTimeNanosec;

			mRecordTimeStamps.RecordIndexStatus = 0;
			mRecordTimeStamps.TimeSecondsStatus = 0;
			mRecordTimeStamps.TimeNanosecStatus = 0;

			TESTHR(mBindingTimeStamps->AddNew(&mRecordTimeStamps));
		}
		catch (_com_error &e) {
			this->LogInformation("Error: Unable to add TimeStamps record");
		}
	}

	void operator()(const DECODER::RecordEndOfFile &data) 
	{
		try {
			mRecordEndOfFile.RecordIndex = ++mRecordIndex;
			mRecordEndOfFile.TimeSeconds = data.mTimeSeconds;
			mRecordEndOfFile.TimeNanosec = data.mTimeNanosec;

			mRecordEndOfFile.RecordIndexStatus = 0;
			mRecordEndOfFile.TimeSecondsStatus = 0;
			mRecordEndOfFile.TimeNanosecStatus = 0;

			TESTHR(mBindingEndOfFile->AddNew(&mRecordEndOfFile));
		}
		catch (_com_error &e) {
			this->LogInformation("Error: Unable to add TimeStamps record");
		}
	}

	void operator()(const DECODER::RecordPeriodicSet &data) 
	{
		try {
			mRecordPeriodicSet.RecordIndex = ++mRecordIndex;
			mRecordPeriodicSet.TimeSeconds = data.mTimeSeconds;
			mRecordPeriodicSet.TimeNanosec = data.mTimeNanosec;
			mRecordPeriodicSet.SetID = data.mSetID;
			mRecordPeriodicSet.NodeID = data.mNodeID;
			COPYSTRING(mRecordPeriodicSet.SetName, data.mSetName.c_str());

			mRecordPeriodicSet.RecordIndexStatus = 0;
			mRecordPeriodicSet.TimeSecondsStatus = 0;
			mRecordPeriodicSet.TimeNanosecStatus = 0;
			mRecordPeriodicSet.SetIDStatus = 0;
			mRecordPeriodicSet.NodeIDStatus = 0;
			mRecordPeriodicSet.SetNameStatus = 0;

			TESTHR(mBindingPeriodicSet->AddNew(&mRecordPeriodicSet));
		}
		catch (_com_error &e) {
			this->LogInformation("Error: Unable to add PeriodicSet record");
		}
	}

	void operator()(const DECODER::RecordLogLevel &data) 
	{
		try {
			mRecordLogLevel.RecordIndex = ++mRecordIndex;
			mRecordLogLevel.TimeSeconds = data.mTimeSeconds;
			mRecordLogLevel.TimeNanosec = data.mTimeNanosec;
			mRecordLogLevel.LevelID = data.mLevelID;
			mRecordLogLevel.NodeID = data.mNodeID;
			COPYSTRING(mRecordLogLevel.LevelName, data.mLevelName.c_str());

			mRecordLogLevel.RecordIndexStatus = 0;
			mRecordLogLevel.TimeSecondsStatus = 0;
			mRecordLogLevel.TimeNanosecStatus = 0;
			mRecordLogLevel.LevelIDStatus = 0;
			mRecordLogLevel.NodeIDStatus = 0;
			mRecordLogLevel.LevelNameStatus = 0;

			TESTHR(mBindingLogLevel->AddNew(&mRecordLogLevel));
		}
		catch (_com_error &e) {
			this->LogInformation("Error: Unable to add LogLevel record");
		}
	}

	void operator()(const DECODER::RecordStartNetCon &data) 
	{
		try {
			mRecordStartNetCon.RecordIndex = ++mRecordIndex;
			mRecordStartNetCon.NodeID = data.mNodeID;
			mRecordStartNetCon.TimeDay = data.mDay;
			mRecordStartNetCon.TimeMonth = data.mMonth;
			mRecordStartNetCon.TimeYear = data.mYear;
			mRecordStartNetCon.TimeHour = data.mHour;
			mRecordStartNetCon.TimeMin = data.mMin;
			mRecordStartNetCon.TimeSec = data.mSec;
			COPYSTRING(mRecordStartNetCon.Name, data.mName.c_str());

			mRecordStartNetCon.RecordIndexStatus = 0;
			mRecordStartNetCon.NodeIDStatus = 0;
			mRecordStartNetCon.TimeDayStatus = 0;
			mRecordStartNetCon.TimeMonthStatus = 0;
			mRecordStartNetCon.TimeYearStatus = 0;
			mRecordStartNetCon.TimeHourStatus = 0;
			mRecordStartNetCon.TimeMinStatus = 0;
			mRecordStartNetCon.TimeSecStatus = 0;
			mRecordStartNetCon.NameStatus = 0;

 			TESTHR(mBindingStartNetCon->AddNew(&mRecordStartNetCon));
		}
		catch (_com_error &e) {
			this->LogInformation("Error: Unable to add StartNetCon record");
		}
	}

	void operator()(const DECODER::RecordPeriodicSetItem &data) 
	{
		try {
			mItemFormats.Set(data.mNodeID, data.mKeyCode, data.mFormat); 

			mRecordPeriodicSetItem.RecordIndex = ++mRecordIndex;
			mRecordPeriodicSetItem.TimeSeconds = data.mTimeSeconds;
			mRecordPeriodicSetItem.TimeNanosec = data.mTimeNanosec;
			mRecordPeriodicSetItem.KeyCode = data.mKeyCode;
			mRecordPeriodicSetItem.NodeID = data.mNodeID;
			COPYSTRING(mRecordPeriodicSetItem.Name, data.mName.c_str());
			COPYSTRING(mRecordPeriodicSetItem.Description, data.mDescription.c_str());
			COPYSTRING(mRecordPeriodicSetItem.Format, data.mFormat.c_str());

			mRecordPeriodicSetItem.RecordIndexStatus = 0;
			mRecordPeriodicSetItem.TimeSecondsStatus = 0;
			mRecordPeriodicSetItem.TimeNanosecStatus = 0;
			mRecordPeriodicSetItem.KeyCodeStatus = 0;
			mRecordPeriodicSetItem.NodeIDStatus = 0;
			mRecordPeriodicSetItem.NameStatus = 0;
			mRecordPeriodicSetItem.DescriptionStatus = 0;
			mRecordPeriodicSetItem.FormatStatus = 0;

			TESTHR(mBindingPeriodicSetItem->AddNew(&mRecordPeriodicSetItem));
		}
		catch (_com_error &e) {
			this->LogInformation("Error: Unable to add PeriodicSetItem record");
		}
	}

	void operator()(const DECODER::RecordPeriodicLogOutput &data) 
	{
		DECODER::RecordPeriodicLogOutput::Map::const_iterator iter = data.mData.begin();
		DECODER::RecordPeriodicLogOutput::Map::const_iterator end = data.mData.end();

		for (; iter != end; ++iter) {
			DECODER::String format = mItemFormats.Get(data.mNodeID, iter->first);
			DECODER::Int16 length = format.size();

			if (length == 0) {
				this->LogInformation("Error: Format does not exist");
			}
			else if (format[length - 1] == 'd') {
				try {
					DECODER::Int32 value = 0;

					if (iter->second.size() == sizeof(DECODER::Int32) / sizeof(DECODER::Uint8)) {
						value = *reinterpret_cast<const DECODER::Int32*>(&iter->second.front());
					}
					else if (iter->second.size() == sizeof(DECODER::Int16) / sizeof(DECODER::Uint8)) {
						value = *reinterpret_cast<const DECODER::Int16*>(&iter->second.front());
					}
					else if (iter->second.size() == sizeof(DECODER::Int8) / sizeof(DECODER::Uint8)) {
						value = *reinterpret_cast<const DECODER::Int8*>(&iter->second.front());
					}

					mRecordPeriodicLogInt.RecordIndex = ++mRecordIndex;
					mRecordPeriodicLogInt.TimeSeconds = data.mTimeSeconds;
					mRecordPeriodicLogInt.TimeNanosec = data.mTimeNanosec;
					mRecordPeriodicLogInt.SetID = data.mSetID;
					mRecordPeriodicLogInt.NodeID = data.mNodeID;
					mRecordPeriodicLogInt.KeyCode = iter->first;
					mRecordPeriodicLogInt.Data = value;

					mRecordPeriodicLogInt.RecordIndexStatus = 0;
					mRecordPeriodicLogInt.TimeSecondsStatus = 0;
					mRecordPeriodicLogInt.TimeNanosecStatus = 0;
					mRecordPeriodicLogInt.SetIDStatus = 0;
					mRecordPeriodicLogInt.NodeIDStatus = 0;
					mRecordPeriodicLogInt.KeyCodeStatus = 0;
					mRecordPeriodicLogInt.DataStatus = 0;

					TESTHR(mBindingPeriodicLogInt->AddNew(&mRecordPeriodicLogInt));
				}
				catch (_com_error &e) {
					this->LogInformation("Error: Unable to add PeriodicLogInt record");
				}
			}
			else if (format[length - 1] == 'u') {
				try {
					DECODER::Uint32 value = 0;

					if (iter->second.size() == sizeof(DECODER::Uint32) / sizeof(DECODER::Uint8)) {
						value = *reinterpret_cast<const DECODER::Uint32*>(&iter->second.front());
					}
					else if (iter->second.size() == sizeof(DECODER::Uint16) / sizeof(DECODER::Uint8)) {
						value = *reinterpret_cast<const DECODER::Uint16*>(&iter->second.front());
					}
					else if (iter->second.size() == sizeof(DECODER::Uint8) / sizeof(DECODER::Uint8)) {
						value = *reinterpret_cast<const DECODER::Uint8*>(&iter->second.front());
					}

					mRecordPeriodicLogInt.RecordIndex = ++mRecordIndex;
					mRecordPeriodicLogInt.TimeSeconds = data.mTimeSeconds;
					mRecordPeriodicLogInt.TimeNanosec = data.mTimeNanosec;
					mRecordPeriodicLogInt.SetID = data.mSetID;
					mRecordPeriodicLogInt.NodeID = data.mNodeID;
					mRecordPeriodicLogInt.KeyCode = iter->first;
					mRecordPeriodicLogInt.Data = value;

					mRecordPeriodicLogInt.RecordIndexStatus = 0;
					mRecordPeriodicLogInt.TimeSecondsStatus = 0;
					mRecordPeriodicLogInt.TimeNanosecStatus = 0;
					mRecordPeriodicLogInt.SetIDStatus = 0;
					mRecordPeriodicLogInt.NodeIDStatus = 0;
					mRecordPeriodicLogInt.KeyCodeStatus = 0;
					mRecordPeriodicLogInt.DataStatus = 0;

					TESTHR(mBindingPeriodicLogInt->AddNew(&mRecordPeriodicLogInt));
				}
				catch (_com_error &e) {
					this->LogInformation("Error: Unable to add PeriodicLogInt record");
				}
			}
			else if (format[length - 1] == 'f') {
				try {
					DECODER::Double value = 0;
					
					if (iter->second.size() == sizeof(DECODER::Float) / sizeof(DECODER::Uint8)) {
						value = *reinterpret_cast<const DECODER::Float*>(&iter->second.front());
					}
					else if (iter->second.size() == sizeof(DECODER::Double) / sizeof(DECODER::Uint8)) {
						value = *reinterpret_cast<const DECODER::Double*>(&iter->second.front());
					}

					mRecordPeriodicLogFloat.RecordIndex = ++mRecordIndex;
					mRecordPeriodicLogFloat.TimeSeconds = data.mTimeSeconds;
					mRecordPeriodicLogFloat.TimeNanosec = data.mTimeNanosec;
					mRecordPeriodicLogFloat.SetID = data.mSetID;
					mRecordPeriodicLogFloat.NodeID = data.mNodeID;
					mRecordPeriodicLogFloat.KeyCode = iter->first;
					mRecordPeriodicLogFloat.Data = static_cast<DECODER::Float>(value);

					mRecordPeriodicLogFloat.RecordIndexStatus = 0;
					mRecordPeriodicLogFloat.TimeSecondsStatus = 0;
					mRecordPeriodicLogFloat.TimeNanosecStatus = 0;
					mRecordPeriodicLogFloat.SetIDStatus = 0;
					mRecordPeriodicLogFloat.NodeIDStatus = 0;
					mRecordPeriodicLogFloat.KeyCodeStatus = 0;
					mRecordPeriodicLogFloat.DataStatus = 0;

					TESTHR(mBindingPeriodicLogFloat->AddNew(&mRecordPeriodicLogFloat));
				}
				catch (_com_error &e) {
					this->LogInformation("Error: Unable to add PeriodicLogFloat record");
				}
			}
			else if (format[length - 1] == 'c') {
				try {
					mRecordPeriodicLogInt.RecordIndex = ++mRecordIndex;
					mRecordPeriodicLogInt.TimeSeconds = data.mTimeSeconds;
					mRecordPeriodicLogInt.TimeNanosec = data.mTimeNanosec;
					mRecordPeriodicLogInt.SetID = data.mSetID;
					mRecordPeriodicLogInt.NodeID = data.mNodeID;
					mRecordPeriodicLogInt.KeyCode = iter->first;
					mRecordPeriodicLogInt.Data = *reinterpret_cast<const DECODER::Char*>(&iter->second.front());

					mRecordPeriodicLogInt.RecordIndexStatus = 0;
					mRecordPeriodicLogInt.TimeSecondsStatus = 0;
					mRecordPeriodicLogInt.TimeNanosecStatus = 0;
					mRecordPeriodicLogInt.SetIDStatus = 0;
					mRecordPeriodicLogInt.NodeIDStatus = 0;
					mRecordPeriodicLogInt.KeyCodeStatus = 0;
					mRecordPeriodicLogInt.DataStatus = 0;

					TESTHR(mBindingPeriodicLogInt->AddNew(&mRecordPeriodicLogInt));
				}
				catch (_com_error &e) {
					this->LogInformation("Error: Unable to add PeriodicLogInt record");
				}
			}
			else if (format[length - 1] == 's') {
				try {
					const char* characters = NULL;
					int count = 0;

					if (!iter->second.empty()) {
						characters = reinterpret_cast<const char*>(&iter->second.front());
						count = iter->second.size();
					}

					mRecordPeriodicLogString.RecordIndex = ++mRecordIndex;
					mRecordPeriodicLogString.TimeSeconds = data.mTimeSeconds;
					mRecordPeriodicLogString.TimeNanosec = data.mTimeNanosec;
					mRecordPeriodicLogString.SetID = data.mSetID;
					mRecordPeriodicLogString.NodeID = data.mNodeID;
					mRecordPeriodicLogString.KeyCode = iter->first;
					COPYCHARS(mRecordPeriodicLogString.Data, characters, count, kMaxStringSize);
					mRecordPeriodicLogString.Data[iter->second.size()] = NULL;

					mRecordPeriodicLogString.RecordIndexStatus = 0;
					mRecordPeriodicLogString.TimeSecondsStatus = 0;
					mRecordPeriodicLogString.TimeNanosecStatus = 0;
					mRecordPeriodicLogString.SetIDStatus = 0;
					mRecordPeriodicLogString.NodeIDStatus = 0;
					mRecordPeriodicLogString.KeyCodeStatus = 0;
					mRecordPeriodicLogString.DataStatus = 0;

					TESTHR(mBindingPeriodicLogString->AddNew(&mRecordPeriodicLogString));
				}
				catch (_com_error &e) {
					this->LogInformation("Error: Unable to add PeriodicLogString record");
				}
			}
			else {
				this->LogInformation("Error: Unknown Format");
			}
		}
	}

	void operator()(const DECODER::RecordTextOutput &data) 
	{
		try {
			mRecordTextOutput.RecordIndex = ++mRecordIndex;
			mRecordTextOutput.TimeSeconds = data.mTimeSeconds;
			mRecordTextOutput.TimeNanosec = data.mTimeNanosec;
			mRecordTextOutput.LevelID = data.mLevelID;
			mRecordTextOutput.TaskID = data.mTaskID;
			mRecordTextOutput.NodeID = data.mNodeID;
			COPYSTRING(mRecordTextOutput.File, data.mFile.c_str());
			mRecordTextOutput.Line = data.mLine;
			COPYTEXT(mRecordTextOutput.Message, data.mMessage.c_str());

			mRecordTextOutput.RecordIndexStatus = 0;
			mRecordTextOutput.TimeSecondsStatus = 0;
			mRecordTextOutput.TimeNanosecStatus = 0;
			mRecordTextOutput.LevelIDStatus = 0;
			mRecordTextOutput.TaskIDStatus = 0;
			mRecordTextOutput.NodeIDStatus = 0;
			mRecordTextOutput.FileStatus = 0;
			mRecordTextOutput.LineStatus = 0;
			mRecordTextOutput.MessageStatus = 0;

			TESTHR(mBindingTextOutput->AddNew(&mRecordTextOutput));
		}
		catch (_com_error &e) {
			this->LogInformation("Error: Unable to add TextOutput record");
		}
	}

	void operator()(const DECODER::RecordBinary &data) 
	{
		if (this->IsBinaryRecordsSupported()) {
			try {
				int size = data.mData.size();

				if (size >= sizeof(mRecordBinaryData.Data)) {
					size = sizeof(mRecordBinaryData.Data) - 1;
				}

				mRecordBinaryData.RecordIndex = ++mRecordIndex;
				std::memcpy(mRecordBinaryData.Data, &data.mData.front(), size);
				mRecordBinaryData.Data[size + 1] = NULL;

				mRecordBinaryData.RecordIndexStatus = 0;
				mRecordBinaryData.IndexStatus = 0;
				mRecordBinaryData.SizeStatus = 0;
				mRecordBinaryData.DataStatus = 0;

				TESTHR(mBindingBinaryData->AddNew(&mRecordBinaryData));
			}
			catch (_com_error &e) {
				this->LogInformation("Error: Unable to add BinaryData record");
			}
		}
	}

private:
	class ItemFormats {
	public:
		void Set(DECODER::Uint32 node, DECODER::Uint16 key, DECODER::String val)
		{
			mNodeFormats[node][key] = val;
		}

		DECODER::String Get(DECODER::Uint32 node, DECODER::Uint16 key)
		{
			return mNodeFormats[node][key];
		}

		void Clear(void)
		{
			mNodeFormats.clear();
		}

	private:
		typedef std::map<DECODER::Uint16, DECODER::String> ItemFormatsSet;
		typedef std::map<DECODER::Uint32, ItemFormatsSet> NodeFormatsSet;

		NodeFormatsSet mNodeFormats;
	};

	template <class _RECORDTYPE>
	void CreateBinding(const char *name, 
			ADODB::_RecordsetPtr &table, 
			IADORecordBinding *&binding, 
			_RECORDTYPE &record)
	{
		TESTHR(table.CreateInstance(__uuidof(ADODB::Recordset)));
		TESTHR(table->Open(name, 
				_variant_t((IDispatch *)mConnectionP, true), 
				ADODB::CursorTypeEnum::adOpenForwardOnly, 
				ADODB::LockTypeEnum::adLockOptimistic, 
				ADODB::adCmdTable)); 
		TESTHR(table->QueryInterface(__uuidof(IADORecordBinding),
				(LPVOID*)&binding));
		TESTHR(binding->BindToRecordset(&record)); 
	}

	void ReleaseBinding(const char *name, 
			ADODB::_RecordsetPtr table, 
			IADORecordBinding *binding)
	{
		try {
			if (table != NULL ) {
				if (binding != NULL) {
					binding->Release();
				}

				if (table->GetState() == ADODB::adStateOpen) {
					table->Close();		
				}
			}
		}
		catch (_com_error &e) {
			this->LogInformation("Error: Could not release binding for " + std::string(name));
		}
	}
	
	void LogInformation(const DECODER::String &text)
	{
		std::cout << text << std::endl;
	}

	std::string GetConnectionString(void)
	{
		std::string extension(mDatabaseFile.substr(mDatabaseFile.find_last_of('.')));
		std::string command;

		if (extension == ".accdb") {
			command += "Provider=Microsoft.ACE.OLEDB.12.0;";
			command += "Data Source=" + mDatabaseFile + ";";
		}
		else if (extension == ".mdb") {
			command += "Provider=Microsoft.Jet.OLEDB.4.0;";
			command += "Data Source=" + mDatabaseFile + ";";
		}
		else if (extension == ".xlsx") {
			command += "Provider=Microsoft.ACE.OLEDB.12.0;";
			command += "Data Source=" + mDatabaseFile + ";";
			command += "Extended Properties=\"Excel 12.0 Xml;HDR=YES\";";
		}
		else if (extension == ".xlsb") {
			command += "Provider=Microsoft.ACE.OLEDB.12.0;";
			command += "Data Source=" + mDatabaseFile + ";";
			command += "Extended Properties=\"Excel 12.0;HDR=YES\";";
		}
		else if (extension == ".xlsm") {
			command += "Provider=Microsoft.ACE.OLEDB.12.0;";
			command += "Data Source=" + mDatabaseFile + ";";
			command += "Extended Properties=\"Excel 12.0 Macro;HDR=YES\";";
		}
		else {
			command += "Provider=Microsoft.ACE.OLEDB.12.0;";
			command += "Data Source=" + mDatabaseFile + ";";
		}

		return command;
	}

	bool IsRecordDeletionSupported(void)
	{
		std::string extension(mDatabaseFile.substr(mDatabaseFile.find_last_of('.')));

		if (extension == ".accdb") {
			return true;
		}
		if (extension == ".mdb") {
			return true;
		}
		else if (extension == ".xlsx") {
			return false;
		}
		else if (extension == ".xlsb") {
			return false;
		}
		else if (extension == ".xlsm") {
			return false;
		}

		return false;
	}

	bool IsBinaryRecordsSupported(void)
	{
		return false;
	}

	bool IsDatabaseCreationRequired(void)
	{
		std::string extension(mDatabaseFile.substr(mDatabaseFile.find_last_of('.')));

		if (extension == ".accdb" || extension == ".mdb") {
			struct stat stFileInfo; 

			return (stat(mDatabaseFile.c_str(), &stFileInfo) != 0);
		}
		
		return false;
	}

	DECODER::String mDatabaseFile;

	ADODB::_ConnectionPtr mConnectionP;

	ADODB::_RecordsetPtr mTableLogInfo;
	ADODB::_RecordsetPtr mTableLogHeader;
	ADODB::_RecordsetPtr mTableTaskCreated;
	ADODB::_RecordsetPtr mTableTaskDeleted;
	ADODB::_RecordsetPtr mTableTimeStamps;
	ADODB::_RecordsetPtr mTableEndOfFile;
	ADODB::_RecordsetPtr mTablePeriodicSet;
	ADODB::_RecordsetPtr mTableLogLevel;
	ADODB::_RecordsetPtr mTableStartNetCon;
	ADODB::_RecordsetPtr mTablePeriodicSetItem;
	ADODB::_RecordsetPtr mTablePeriodicLogInt;
	ADODB::_RecordsetPtr mTablePeriodicLogFloat;
	ADODB::_RecordsetPtr mTablePeriodicLogString;
	ADODB::_RecordsetPtr mTableTextOutput;
	ADODB::_RecordsetPtr mTableBinaryData;

   IADORecordBinding *mBindingLogInfo;
	IADORecordBinding *mBindingLogHeader;
	IADORecordBinding *mBindingTaskCreated;
	IADORecordBinding *mBindingTaskDeleted;
	IADORecordBinding *mBindingTimeStamps;
	IADORecordBinding *mBindingEndOfFile;
	IADORecordBinding *mBindingPeriodicSet;
	IADORecordBinding *mBindingLogLevel;
	IADORecordBinding *mBindingStartNetCon;
	IADORecordBinding *mBindingPeriodicSetItem;
	IADORecordBinding *mBindingPeriodicLogInt;
	IADORecordBinding *mBindingPeriodicLogFloat;
	IADORecordBinding *mBindingPeriodicLogString;
	IADORecordBinding *mBindingTextOutput;
  	IADORecordBinding *mBindingBinaryData;

	RecordLogInfo mRecordLogInfo;
	RecordLogHeader mRecordLogHeader;
	RecordTaskCreated mRecordTaskCreated;
	RecordTaskDeleted mRecordTaskDeleted;
	RecordTimeStamps mRecordTimeStamps;
	RecordEndOfFile mRecordEndOfFile;
	RecordPeriodicSet mRecordPeriodicSet;
	RecordLogLevel mRecordLogLevel;
	RecordStartNetCon mRecordStartNetCon;
	RecordPeriodicSetItem mRecordPeriodicSetItem;
	RecordPeriodicLogInt mRecordPeriodicLogInt;
	RecordPeriodicLogFloat mRecordPeriodicLogFloat;
	RecordPeriodicLogString mRecordPeriodicLogString;
	RecordTextOutput mRecordTextOutput;
	RecordBinaryData mRecordBinaryData;

	ItemFormats mItemFormats;
	DECODER::Int32 mRecordIndex;
};

} // namespace CONVERT

#endif // DATABASECONVERTERUTIL_H

