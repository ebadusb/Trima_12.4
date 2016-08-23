// log_convert.cpp : Defines the entry point for the console application.
//

#include <cstdlib>
#include "zlib.h"
#include "log_format.h"
#include <sys/types.h>
#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>

int egzread(gzFile file, void * buffer, size_t size)
{
	unsigned char	* cBuffer = (unsigned char *)buffer;

	int retVal = gzread(file, buffer, size);
	for ( size_t i=0; i<size; i++ )
	{
		cBuffer[i] ^= 0xa5;
	}

   return retVal;
}

int egzgetc(gzFile file)
{
	int	ch = gzgetc(file);
	if ( ch != EOF )
	{
		ch ^= 0xa5;
	}

	return ch;
}

enum PrintArg_Type
{
	PrintArg_Char,
	PrintArg_Int,
	PrintArg_Long,
	PrintArg_Float,
	PrintArg_Double,
	PrintArg_String,
	PrintArg_Done,
	PrintArg_Error
};

static PrintArg_Type getNextFormatArg(const char * format, size_t& currentIndex, unsigned int& starModifierCount)
{
	enum ParseState
	{
		WaitPercent,
		WaitType
	};

	PrintArg_Type	result = 	PrintArg_Done;
	ParseState		parseState = WaitPercent;
	bool	longModifierPresent = false;
	bool	parseDone = false;

	starModifierCount = 0;
	while ( format &&
			  format[currentIndex] != '\0' &&
			  !parseDone )
	{
		if ( parseState == WaitPercent &&
		     format[currentIndex] == '%' )
		{
			parseState = WaitType;
		}
		else if ( parseState == WaitType )
		{
			switch ( format[currentIndex] )
			{
			case '%':
				if ( format[currentIndex-1] == '%' )
				{
					//
					// %% combination corresponds to single % on output
					//
					parseState = WaitPercent;
			   }
			   else
			   {
					result = PrintArg_Error;
					parseDone = true;
			   }

				break;
  
			case '*':
				starModifierCount += 1;
				break;

			case 'l':
				longModifierPresent = true;
				break;

			case 'c':
				result = PrintArg_Char;
				parseDone = true;
				break;

			case 'd':
			case 'u':
			case 'x':
			case 'X':
				result = ( longModifierPresent ) ? PrintArg_Long : PrintArg_Int;
				parseDone = true;
				break;

			case 'f':
			case 'g':
				result = ( longModifierPresent ) ? PrintArg_Double : PrintArg_Float;
				parseDone = true;
				break;

			case 's':
				result = PrintArg_String;
				parseDone = true;
				break;
			}
		}

		currentIndex += 1;
	}

	if ( format[currentIndex] == '\0' &&
		  result == PrintArg_Done && 
		  parseState == WaitType )
	{
		//
		// Reached end of format string with incomplete format specifier
		//
		result = PrintArg_Error;
	}

	return result;
}

static void parsePrint(const char * format, gzFile zfp, DLOG_PrintOutput* printOutputRecord)
{
	unsigned int	starModifierCount;
	size_t			currentIndex = 0;
	PrintArg_Type	argType;
	bool				argsDone = false;
	stringstream argValues;
	while ( !argsDone )
	{
		argType = getNextFormatArg(format, currentIndex, starModifierCount);
		if ( argType == PrintArg_Done )
		{
			argsDone = true;
	   }
	   else if ( argType == PrintArg_Error )
	   {
			/*
			 *	Send error notification, but otherwise ignore error
			 */
			printf(" ** arg error **\n");
	   } 
	   else
	   {
			for ( unsigned int i=0; i<starModifierCount; i++ )
			{
				int	fieldLen;
				egzread(zfp, &fieldLen, sizeof(fieldLen));
				argValues << fieldLen << " "; 
			}

			switch ( argType )
			{
			case PrintArg_Char:
				{
					char	chArg;
					egzread(zfp, &chArg, sizeof(chArg));
					argValues << chArg << " ";
				}
				break;

			case PrintArg_Int:
				{
					int	intArg;
					egzread(zfp, &intArg, sizeof(intArg));
					argValues << intArg << " ";
				}
				break;

			case PrintArg_Long:
				{
					long	longArg;
					egzread(zfp, &longArg, sizeof(longArg));
					argValues << longArg << " ";

				}
				break;

			case PrintArg_Float:
				{
					float	floatArg;
					egzread(zfp, &floatArg, sizeof(floatArg));
					argValues << (double)floatArg << " ";
				}
				break;

			case PrintArg_Double:
				{
					double	doubleArg;
					egzread(zfp, &doubleArg, sizeof(doubleArg));
					argValues << doubleArg << " ";
				}
				break;

			case PrintArg_String:
				{
					unsigned short len;
					egzread(zfp, &len, sizeof(len));
					for (int i=0; i<len; i++)
					{
						char ch = egzgetc(zfp);
						argValues << ch;
					}
					argValues << " ";
				}
				break;

			default:
				argValues << " ** unknown arg type **\n";
				break;
		   }
		}
	}
	printOutputRecord->argValues = argValues.str();
}
/*int main(int argc, char* argv[])
{
	if ( argc != 2 )
	{
		fprintf(stderr, "Usage: log_convert <file_name>\n");
		exit(1);
	}
	DLOG_Filter filter;
	//read in only stream records. 
	filter.includeRecordTypes.push_back(DLOG_Record::DLOG_StreamOutputId);

	DLOG_Reader reader(argv[1],filter);
	int retVal = reader.initialize();
	return retVal;
}
*/
DLOG_Filter::DLOG_Filter()
{
	recordCount = 0;
}

DLOG_Filter::~DLOG_Filter()
{

}

bool DLOG_Filter::addPeriodicLog(DLOG_Record* record)
{
	bool result = false;

	switch(record->headerType) {
		/* Is it a periodic set record? */
		case DLOG_Header::DLOG_PeriodicSetId: {
			this->_currentPeriodicSet = *((DLOG_PeriodicSet*)record);
			if(std::find(this->periodicSetNames.begin(),
						 this->periodicSetNames.end(), _currentPeriodicSet.setName) 
			   != this->periodicSetNames.end()) {
				_setIds.insert(_currentPeriodicSet.setId);
				result = true;
			}

		}
		break;
		/* Is it a periodic set item record?*/
		case DLOG_Header::DLOG_PeriodicSetItemId: {
			DLOG_PeriodicSetItem* item = (DLOG_PeriodicSetItem*) record;
	
			if(std::find(this->periodicItemNames.begin(),
						 this->periodicItemNames.end(), item->keyName) 
			   != this->periodicItemNames.end()) {
				_periodicItemMap.insert(std::make_pair(item->keyName, *item));
				if(!(std::find(_setIds.begin(), _setIds.end(), _currentPeriodicSet.setId ) == _setIds.end())) {
					_setIds.insert(_currentPeriodicSet.setId);
				}                            
				result = true;
	//			_setIds.insert(item->keyCode);
			}

			if(std::find(this->periodicSetNames.begin(),
						 this->periodicSetNames.end(), _currentPeriodicSet.setName) 
			   != this->periodicSetNames.end()) {
				result = true;
			}
		}
		break;
		/* Is it a perioc log record? */
		case DLOG_Header::DLOG_PeriodicLogId: {
			DLOG_PeriodicLog* periodicLog = (DLOG_PeriodicLog*) record;
			if(find(_setIds.begin(), _setIds.end(), periodicLog->setId) != _setIds.end()) {
//			/* iterate over evey log record to see if this log item needs to be added */
//				std::vector<PeriodicLogItem>::iterator it;
//				for(it = periodicLog->periodicItemList.begin(); it != periodicLog->periodicItemList.end(); it++) {
//					if (find(_setIds.begin(), _setIds.end(), item->keyCode) != _setIds.end() ) {
						result = true;
						break;
//					}
				}
			}
		break;
	}
  return result; 
}

bool DLOG_Filter::add(DLOG_Record* record)
{
	bool result = false;
	vector<DLOG_Record::DLOG_RecordType>::iterator it;
	vector<string>::iterator sit;

   if(addPeriodicLog(record)) {
	   return true;
   }
	/* If any search filter vector is not empty, exclude all */
	if(this->includeRecordTypes.empty() && 
	   this->excludeRecordTypes.empty() &&
	   this->includeStrings.empty() &&
	   this->excludeStrings.empty() && 
	   this->periodicSetNames.empty()) {
		result = true;
	}
	/* if includeRecordTypes is not empty, check if this particular
	   record type needs to be included */
	if(result && !this->includeRecordTypes.empty())
	{
		it = find(this->includeRecordTypes.begin(), 
				this->includeRecordTypes.end(), record->headerType) ;
		/* Is it an expected type? */
		if(it != this->includeRecordTypes.end()){
			result = true;         
		}
	}
	/* If excludeRecordTypes is not empty, check if this particular
	   record type needs to be excluded */
	if(result && !this->excludeRecordTypes.empty()) {
		it = find(this->excludeRecordTypes.begin(), 
				this->excludeRecordTypes.end(), record->headerType) ;
		/* No explicit exclude */
		if(it == this->excludeRecordTypes.end()){
			result = true;         
		}
	}
	
	/* implies stream record (or print record?) */
	if( !this->includeStrings.empty() )
	{
		switch (record->headerType)
		{
		case DLOG_Record::DLOG_StreamOutputId:
			{
				DLOG_StreamOutput* sr = (DLOG_StreamOutput*)record;
				for(sit = this->includeStrings.begin(); sit != this->includeStrings.end(); sit++) {
					if(sr->streamContents.find(*sit) != sr->streamContents.npos) {
						result = true;
						break;
						}
				}
			}
			break;
		default:
			break;
		}
	}

	return result;
}   

DLOG_Reader::DLOG_Reader()
{
	_initialized = false;
}

DLOG_Reader::~DLOG_Reader() 
{
	//DLOG_Record* p;
	while(!_dlogRecords.empty()) {
		delete _dlogRecords.front();
		_dlogRecords.erase(_dlogRecords.begin());
	}
	_recordMap.erase(_recordMap.begin(), _recordMap.end());
}

int DLOG_Reader::initialize()
{
	FILE * fp = fopen(_dlogName, "rb");
	if ( !fp )
	{
		printf("Cannot open file: %s\n", _dlogName);
		return 1;
	}
	/* Read header Record */
	DLOG_Header* headerRecord = new DLOG_Header;

	char	ch;
	bool	done = false;
	while ( (ch = fgetc(fp)) != EOF )
	{
		if (ch >= 0x20) headerRecord->startMessage += ch;
		if ( ch == '\0' )
		{
			break;
		}
	}


	fgetc(fp); fgetc(fp); fgetc(fp); fgetc(fp);
	if ( ch == EOF )
	{
		printf(" unexpected end\n");
		done = true;
	}

	
	unsigned short headerType;
	fread(&headerType, sizeof(headerType), 1, fp);
	headerRecord->headerType = headerType;
	if ( headerType != 0x5500 )
	{
		printf("invalid header type %x\n", (int)headerType);
		done = true;
	}
	else
	{
		int i;

		fread(&headerRecord->sizeOfChar, sizeof(headerRecord->sizeOfChar), 1, fp);  
		fread(&headerRecord->sizeOfInt, sizeof(headerRecord->sizeOfInt), 1, fp);  
		fread(&headerRecord->sizeOfLong, sizeof(headerRecord->sizeOfLong), 1, fp);  
		fread(&headerRecord->sizeOfFloat, sizeof(headerRecord->sizeOfFloat), 1, fp);  
		fread(&headerRecord->sizeOfDouble, sizeof(headerRecord->sizeOfDouble), 1, fp);  
		fread(&headerRecord->sizeOfTaskId, sizeof(headerRecord->sizeOfTaskId), 1, fp);  
		fread(&headerRecord->taskId, sizeof(headerRecord->taskId), 1, fp);  
		fread(&headerRecord->sizeOfNodeId, sizeof(headerRecord->sizeOfNodeId), 1, fp);  
		fread(&headerRecord->version, sizeof(headerRecord->version), 1, fp);  
			/*printf(" version = %d.%d\n", (version&0xff00) >> 8, (version & 0xff));*/
		fread(&headerRecord->sizeOfPlatformName, sizeof(headerRecord->sizeOfPlatformName), 1, fp);  
		for ( i=0; i<headerRecord->sizeOfPlatformName; i++ )
		{
			ch = fgetc(fp);
			headerRecord->platformName += ch;
		}

		fread(&headerRecord->nodeId, sizeof(headerRecord->nodeId), 1, fp); 

		ch = fgetc(fp); headerRecord->day = ch; 
		ch = fgetc(fp); headerRecord->month = ch; 
		fread(&headerRecord->year, sizeof(headerRecord->year), 1, fp); 
		ch = fgetc(fp); headerRecord->hour = ch; 
		ch = fgetc(fp); headerRecord->minute = ch;
		ch = fgetc(fp); headerRecord->second = ch;  

		fread(&headerRecord->sizeOfNodeName, sizeof(headerRecord->sizeOfNodeName), 1, fp); 
		for ( i=0; i<headerRecord->sizeOfNodeName; i++ )
		{
			ch = fgetc(fp);
			headerRecord->nodeName += ch;
		}
	}

	add(headerRecord);
    if (headerRecord->version != 0x0301)
    {
        printf("log file version %d.%d not supported\n", (headerRecord->version&0xff00) >> 8, (headerRecord->version & 0xff));
        exit(1);
    }

	gzFile	zfp = NULL;
	if ( !done )
	{
		fpos_t  fpPos; 

		fgetpos(fp, &fpPos);
		fclose(fp);

		int	fd = open(_dlogName, O_RDONLY | O_BINARY);
		lseek(fd, (long)fpPos, SEEK_SET);

		zfp = gzdopen(fd, "rb");
	}
	DLOG_Record* logRecord = NULL;
 	while ( !done )
	{
		unsigned short recordType;
		unsigned short	tempShort;
		unsigned int	tempInt;
		char * format;
		int i;
		
		if ( egzread(zfp, &recordType, sizeof(recordType)) <= 0 ) break;

		switch ( recordType )
		{
		case 0x5501:
			{
				DLOG_LogLevel* logLevelRecord = new DLOG_LogLevel;
				logLevelRecord->headerType = DLOG_Record::DLOG_LogLevelId;
				egzread(zfp, &logLevelRecord->timeSeconds, sizeof(logLevelRecord->timeSeconds));  
				egzread(zfp, &logLevelRecord->timeNanoSeconds, sizeof(logLevelRecord->timeNanoSeconds)); 
				egzread(zfp, &logLevelRecord->levelId, sizeof(logLevelRecord->levelId));  
				egzread(zfp, &logLevelRecord->nodeId, sizeof(logLevelRecord->nodeId)); 
				egzread(zfp, &logLevelRecord->sizeOfLevelName, sizeof(logLevelRecord->sizeOfLevelName)); 
				for ( i=0; i<logLevelRecord->sizeOfLevelName; i++ )
				{
					ch = egzgetc(zfp);
					logLevelRecord->levelName += ch;
				}
				logRecord = logLevelRecord;
			}
			break;

		case 0x5502:
			{
				DLOG_PrintOutput* printOutputRecord = new DLOG_PrintOutput;
				printOutputRecord->headerType = DLOG_Record::DLOG_PrintOutputId;
				egzread(zfp, &printOutputRecord->timeSeconds, sizeof(printOutputRecord->timeSeconds)); 
				egzread(zfp, &printOutputRecord->timeNanoSeconds, sizeof(printOutputRecord->timeNanoSeconds)); 
				egzread(zfp, &printOutputRecord->levelId, sizeof(printOutputRecord->levelId)); 
				egzread(zfp, &printOutputRecord->taskId, sizeof(printOutputRecord->taskId)); 
				egzread(zfp, &printOutputRecord->nodeId, sizeof(printOutputRecord->nodeId));
				egzread(zfp, &printOutputRecord->lenFormat, sizeof(printOutputRecord->lenFormat));
				egzread(zfp, &printOutputRecord->lenFile, sizeof(printOutputRecord->lenFile));
				egzread(zfp, &printOutputRecord->lineNo, sizeof(printOutputRecord->lineNo));
	
				format = new char[printOutputRecord->lenFormat+1];
				egzread(zfp, format, printOutputRecord->lenFormat);
				format[printOutputRecord->lenFormat] = '\0';
				printOutputRecord->formatString = format;

				for ( i=0; i<printOutputRecord->lenFile; i++ )
				{
					ch = egzgetc(zfp);
					printOutputRecord->fileName += ch;
				}
				parsePrint(format, zfp, printOutputRecord);
				delete[] format;
				logRecord = printOutputRecord;
			}
			break;

		case 0x5503:
			{
				DLOG_StreamOutput* streamOutputRecord = new DLOG_StreamOutput;
				streamOutputRecord->headerType = DLOG_Record::DLOG_StreamOutputId;
				egzread(zfp, &streamOutputRecord->timeSeconds, sizeof(streamOutputRecord->timeSeconds));  
				egzread(zfp, &streamOutputRecord->timeNanoSeconds, sizeof(streamOutputRecord->timeNanoSeconds));
				egzread(zfp, &streamOutputRecord->levelId, sizeof(streamOutputRecord->levelId));
				egzread(zfp, &streamOutputRecord->taskId, sizeof(streamOutputRecord->taskId)); 
				egzread(zfp, &streamOutputRecord->nodeId, sizeof(streamOutputRecord->nodeId));
				egzread(zfp, &streamOutputRecord->lenFile, sizeof(streamOutputRecord->lenFile));
				egzread(zfp, &streamOutputRecord->lineNo, sizeof(streamOutputRecord->lineNo)); 
	
				for ( i=0; i<streamOutputRecord->lenFile; i++ )
				{
					ch = egzgetc(zfp);
					streamOutputRecord->fileName += ch;
				}
	
				egzread(zfp, &streamOutputRecord->flags, sizeof(streamOutputRecord->flags)); 
				streamOutputRecord->precision = egzgetc(zfp);
				
				egzread(zfp, &streamOutputRecord->lenStream, sizeof(streamOutputRecord->lenStream));
				int	readLen;
	
				readLen = 0;
				stringstream outStream;
				while ( readLen < streamOutputRecord->lenStream)
				{
					int type = egzgetc(zfp);
					readLen += 1;
	
					switch (type)
					{
					case 1:
					case 2:
						ch = egzgetc(zfp);
						readLen += 1;
						outStream << " " << ch ;
						break;
	
					case 3:
					case 5:
						egzread(zfp, &tempInt, sizeof(tempInt)); outStream << " " << tempInt;
						readLen += 4;
						break;
	
					case 4:
					case 6:
						egzread(zfp, &tempInt, sizeof(tempInt)); outStream << " " << (unsigned int)tempInt;
						readLen += 4;
						break;
	
					case 7:
						egzread(zfp, &tempShort, sizeof(tempShort));
						readLen += 2 + tempShort;
						for ( i=0; i<tempShort; i++ )
						{
							ch = egzgetc(zfp);
							outStream << ch;
						}
					   break;
	
					case 8:
						float tempFloat;
						egzread(zfp, &tempFloat, sizeof(tempFloat));
						readLen += 4;
	
						outStream << " " << (double)tempFloat;
						break;
	
					case 9:
						double tempDouble;
						egzread(zfp, &tempDouble, sizeof(tempDouble));
						readLen += 8;
	
						outStream << " " << tempDouble;
						break;
	
					case 10:
						ch = egzgetc(zfp);
						readLen += 1;
						if(ch) 
							outStream << " true";
						else
							outStream << " false";
						break;
	
					case 100:
						egzread(zfp, &streamOutputRecord->flags, sizeof(streamOutputRecord->flags));  
						readLen += 2;
						break;
	
					case 101:
						ch = egzgetc(zfp);
						readLen += 1;
						streamOutputRecord->precision = ch;
						break;
	
					default:
						outStream << " bad argument type ";
						break;
					}
				}
				streamOutputRecord->streamContents = outStream.str();
				logRecord = streamOutputRecord;
			}
			break;

		case 0x5504:
			{
				DLOG_PeriodicLog* periodicLogRecord = new DLOG_PeriodicLog;
				periodicLogRecord->headerType = DLOG_Record::DLOG_PeriodicLogId;
				egzread(zfp, &periodicLogRecord->timeSeconds, sizeof(periodicLogRecord->timeSeconds));
				egzread(zfp, &periodicLogRecord->timeNanoSeconds, sizeof(periodicLogRecord->timeNanoSeconds)); 
				egzread(zfp, &periodicLogRecord->setId, sizeof(periodicLogRecord->setId));
				egzread(zfp, &periodicLogRecord->nodeId, sizeof(periodicLogRecord->nodeId)); 
				egzread(zfp, &periodicLogRecord->itemCount, sizeof(periodicLogRecord->itemCount));  
				for (i=0; i<periodicLogRecord->itemCount; i++)
				{
					PeriodicLogItem periodicItem;
					int	j;
	
					egzread(zfp, &periodicItem.itemSize, sizeof(periodicItem.itemSize));
					egzread(zfp, &periodicItem.keyCode, sizeof(periodicItem.keyCode));
					//printf(" item size=%d, key=%d, data=", (int)itemSize, (int)keyCode);
					for (j=0; j<periodicItem.itemSize; j++)
					{
						ch = egzgetc(zfp);
						periodicItem.data += ch;
					}
					periodicLogRecord->periodicItemList.push_back(periodicItem);
				}
				logRecord = periodicLogRecord;
			}
			break;

		case 0x5505:
			{
				DLOG_PeriodicSet* periodicSetRecord = new DLOG_PeriodicSet;
				periodicSetRecord->headerType = DLOG_Record::DLOG_PeriodicSetId;
				egzread(zfp, &periodicSetRecord->timeSeconds, sizeof(periodicSetRecord->timeSeconds));  
				egzread(zfp, &periodicSetRecord->timeNanoSeconds, sizeof(periodicSetRecord->timeNanoSeconds));  
				egzread(zfp, &periodicSetRecord->setId, sizeof(periodicSetRecord->setId));  
				egzread(zfp, &periodicSetRecord->nodeId, sizeof(periodicSetRecord->nodeId)); 
				egzread(zfp, &periodicSetRecord->itemCount, sizeof(periodicSetRecord->itemCount)); 
				for ( i=0; i<periodicSetRecord->itemCount; i++ )
				{
					ch = egzgetc(zfp);
					periodicSetRecord->setName += ch;
				}
				logRecord = periodicSetRecord;
			}
			break;

		case 0x5506:
			{
			   DLOG_PeriodicSetItem* periodicSetItemRecord = new DLOG_PeriodicSetItem;
			   periodicSetItemRecord->headerType = DLOG_Record::DLOG_PeriodicSetItemId;
				egzread(zfp, &periodicSetItemRecord->timeSeconds, sizeof(periodicSetItemRecord->timeSeconds));  
				egzread(zfp, &periodicSetItemRecord->timeNanoSeconds, sizeof(periodicSetItemRecord->timeNanoSeconds)); 
				egzread(zfp, &periodicSetItemRecord->keyCode, sizeof(periodicSetItemRecord->keyCode)); 
				egzread(zfp, &periodicSetItemRecord->nodeId, sizeof(periodicSetItemRecord->nodeId)); 

				egzread(zfp, &periodicSetItemRecord->lenKeyName, sizeof(periodicSetItemRecord->lenKeyName)); 
				egzread(zfp, &periodicSetItemRecord->lenDescription, sizeof(periodicSetItemRecord->lenDescription)); 
				egzread(zfp, &periodicSetItemRecord->lenFormat, sizeof(periodicSetItemRecord->lenFormat)); 
	
				for ( i=0; i<periodicSetItemRecord->lenKeyName; i++ )
				{
					ch = egzgetc(zfp);
					periodicSetItemRecord->keyName += ch;
				}
	
				for ( i=0; i<periodicSetItemRecord->lenDescription; i++ )
				{
					ch = egzgetc(zfp);
					periodicSetItemRecord->description += ch;
				}
	
				for ( i=0; i<periodicSetItemRecord->lenFormat; i++ )
				{
					ch = egzgetc(zfp);
					periodicSetItemRecord->format += ch;
				}
				logRecord = periodicSetItemRecord;
			}
			break;

		case 0x5507:
			{
				DLOG_TaskCreated* taskCreatedRecord = new DLOG_TaskCreated;
				taskCreatedRecord->headerType = DLOG_Record::DLOG_TaskCreatedId;
				egzread(zfp, &taskCreatedRecord->timeSeconds, sizeof(taskCreatedRecord->timeSeconds));  
				egzread(zfp, &taskCreatedRecord->timeNanoSeconds, sizeof(taskCreatedRecord->timeNanoSeconds));
				egzread(zfp, &taskCreatedRecord->levelId, sizeof(taskCreatedRecord->levelId));
				egzread(zfp, &taskCreatedRecord->taskId, sizeof(taskCreatedRecord->taskId)); 
				egzread(zfp, &taskCreatedRecord->nodeId, sizeof(taskCreatedRecord->nodeId));

				egzread(zfp, &taskCreatedRecord->lenTaskName, sizeof(taskCreatedRecord->lenTaskName));
	
				format = new char[taskCreatedRecord->lenTaskName+1];
				egzread(zfp, format,taskCreatedRecord->lenTaskName );
				format[taskCreatedRecord->lenTaskName] = '\0';
				taskCreatedRecord->taskName = format;
				delete[] format;
				logRecord = taskCreatedRecord;
			}
			break;

		case 0x5508:
			{
				DLOG_TaskDeleted* taskDeletedRecord = new DLOG_TaskDeleted;
				taskDeletedRecord->headerType = DLOG_Record::DLOG_TaskDeletedId;

				egzread(zfp, &taskDeletedRecord->timeSeconds, sizeof(taskDeletedRecord->timeSeconds));  
				egzread(zfp, &taskDeletedRecord->timeNanoSeconds, sizeof(taskDeletedRecord->timeNanoSeconds));
				egzread(zfp, &taskDeletedRecord->levelId, sizeof(taskDeletedRecord->levelId));
				egzread(zfp, &taskDeletedRecord->taskId, sizeof(taskDeletedRecord->taskId)); 
				egzread(zfp, &taskDeletedRecord->nodeId, sizeof(taskDeletedRecord->nodeId));
				logRecord = taskDeletedRecord;
			}
			break;

		case 0x5509:
			{
				DLOG_NetworkLogHeader* networkLogHeader = new DLOG_NetworkLogHeader;
				networkLogHeader->headerType = DLOG_Header::DLOG_NetworkLogHeaderId;
				egzread(zfp, &networkLogHeader->nodeId, sizeof(networkLogHeader->nodeId)); 
				ch = egzgetc(zfp); networkLogHeader->day = ch; 
				ch = egzgetc(zfp); networkLogHeader->month = ch; 
				egzread(zfp, &networkLogHeader->year, sizeof(networkLogHeader->year));  
				ch = egzgetc(zfp); networkLogHeader->hour = ch; 
				ch = egzgetc(zfp); networkLogHeader->minute = ch; 
				ch = egzgetc(zfp); networkLogHeader->second = ch; 
	
				egzread(zfp, &networkLogHeader->lenNodeName, sizeof(networkLogHeader->lenNodeName)); 
				for ( i=0; i<networkLogHeader->lenNodeName; i++ )
				{
					ch = egzgetc(zfp);
					networkLogHeader->nodeName += ch;
				}
				logRecord = networkLogHeader;
			}
			break;

		case 0x55f0:
			{
				DLOG_Binary* binaryRecord = new DLOG_Binary;
				binaryRecord->headerType = DLOG_Record::DLOG_BinaryId;
				egzread(zfp, &binaryRecord->nItems, sizeof(binaryRecord->nItems));
				for ( i=0; i<(int)binaryRecord->nItems; i++ )
				{
					ch = egzgetc(zfp);
					binaryRecord->binaryData += ch;
				}
				logRecord = binaryRecord;
			}
			break;
		case 0x55fc:
			{
				DLOG_EndNetworkLog* endNetworkLog = new DLOG_EndNetworkLog;
				endNetworkLog->headerType = DLOG_Record::DLOG_EndNetworkLogId; 
				egzread(zfp, &endNetworkLog->timeSeconds, sizeof(endNetworkLog->timeSeconds));  
				egzread(zfp, &endNetworkLog->timeNanoSeconds, sizeof(endNetworkLog->timeNanoSeconds));
				egzread(zfp, &endNetworkLog->nodeId, sizeof(endNetworkLog->nodeId));
				logRecord = endNetworkLog;
			}
			break;
		case 0x55fd:
			{
				DLOG_EndLog* endLog = new DLOG_EndLog;
				endLog->headerType = DLOG_EndLog::DLOG_EndLogId; 
				egzread(zfp, &endLog->timeSeconds, sizeof(endLog->timeSeconds));  
				egzread(zfp, &endLog->timeNanoSeconds, sizeof(endLog->timeNanoSeconds));
				logRecord = endLog;
			}
			break;

		case 0x55fe:
			{
				DLOG_TimeStamp* endTimeStampLog = new DLOG_TimeStamp;
				endTimeStampLog->headerType = DLOG_Record::DLOG_TimeStampId; 
				//egzread(zfp, &endTimeStampLog->nodeId, sizeof(endTimeStampLog->nodeId));
				egzread(zfp, &endTimeStampLog->timeSeconds, sizeof(endTimeStampLog->timeSeconds));  
				egzread(zfp, &endTimeStampLog->timeNanoSeconds, sizeof(endTimeStampLog->timeNanoSeconds));
				logRecord = endTimeStampLog;
			}
			break;

		case 0x55ff:
			{
				DLOG_MissedData* missedData = new DLOG_MissedData;
				missedData->headerType = DLOG_Record::DLOG_MissedDataId; 
				logRecord = missedData;
			}
			break;
		default:
			printf("\nUnknown record type %04x\n", recordType);
//			done = true;
		} 
		if((logRecord != NULL) && !add(logRecord)) {
			delete logRecord;
			logRecord = NULL;
		}
		done = terminate(); 
	}
	   
	_initialized = true;
	return 0;
}

/* Uses the filter. If eligible, adds the 
   record. Discards it otherwise. */
bool DLOG_Reader::add(DLOG_Record* record)
{
	bool result = _dlogFilter.add(record);
	if(result){ 
		_dlogRecords.push_back(record);
		DLOGREC_MAP::iterator it;
		DLOG_Record::DLOG_RecordType dlogRecordType = static_cast<DLOG_Record::DLOG_RecordType>(record->headerType);
		it = _recordMap.find(dlogRecordType);
		if (it == _recordMap.end()){
			DLOGREC_VECTOR v; v.push_back(record);
			_recordMap.insert(pair<DLOG_Record::DLOG_RecordType, DLOGREC_VECTOR >(dlogRecordType, v));
		}
		else
		{
		   _recordMap[dlogRecordType].push_back(record); 
		}
	}
	return result;
}


bool DLOG_Reader::terminate()
{
	if ((_dlogFilter.recordCount != 0) && (_dlogFilter.recordCount == _dlogRecords.size()))
		return true;

	if (!_dlogFilter.recordCountMap.empty()) {
		map<DLOG_Record::DLOG_RecordType, unsigned int>::iterator it;
		for(it = _dlogFilter.recordCountMap.begin(); it != _dlogFilter.recordCountMap.end(); it++) {
		   DLOG_Record::DLOG_RecordType dlogRecordType = it->first;
		   if((_recordMap.find(dlogRecordType)) == _recordMap.end()){
			   return false;
		   }
		   else if ((_dlogFilter.recordCountMap[dlogRecordType] != _recordMap[dlogRecordType].size())) {
				   return false;
			   }
		   }
		return true; //if all record counts match.
	}
	return false;
}
