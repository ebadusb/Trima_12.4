#include "vxworks.h"

#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "crcgen.h"

using namespace std;
#define OK 0
#define ERROR (-1) 
static OptionFileData * swData = NULL;

static int processSectionHeader(const char * fileName, int lineNumber, char * token, const char * sectionName, int * foundSection)
{
	int	result = OK;
	int tokenLen = strcspn(token, "]");

	if ( token[tokenLen] != ']' )
	{
		fprintf(stderr, "%s(%d): error parsing section header\n", fileName, lineNumber);
		result = ERROR;
	}
	else 
	{
		token[tokenLen] = '\0';
		*foundSection = (strcmp(token, sectionName) == 0) ? 1 : 0;
	}

	return result;
}

static int processKeyword(const char * fileName, int lineNumber, char * token, OptionFileData ** data, int crcRequired)
{
	int	result = OK;
	char * equals = strchr(token, '=');
	int	keywordLen = 0;
	int	settingLen = 0;

	if ( !equals )
	{
		fprintf(stderr, "%s(%d): missing \"=\"\n", fileName, lineNumber);
		result = ERROR;
	}
	else
	{
		keywordLen = equals-token;
		if ( crcRequired &&
			  equals[1] != 0 )
		{
			char * comma = strrchr(token, ',');
			unsigned long	checkValue = 0;
			unsigned long	verifyValue = 0;

			if ( !comma ||
				  sscanf(comma+1, "%lx", &verifyValue) != 1 )
			{
				fprintf(stderr, "%s(%d): missing CRC value\n", fileName, lineNumber);
				result = ERROR;
		   }

			settingLen = comma-equals-1; 
			crcgen32(&checkValue, (unsigned char *)token, keywordLen+settingLen+1);
         if ( checkValue != verifyValue )
			{
				fprintf(stderr, "%s(%d): incorrect CRC value\n", fileName, lineNumber);
				result = ERROR;
			}
		}
		else
		{
			settingLen = strlen(equals+1);
		}
   }

	if ( result == OK )
	{
		/*
		 *	Allocate one chunck of memory to store new data entry as well as the
		 * name and setting strings.
		 */
		char * newData = (char*) malloc(sizeof(OptionFileData) + (keywordLen+settingLen+2)*sizeof(char));

		OptionFileData * newEntry = (OptionFileData *)newData;
		newEntry->name = (char *)(&newEntry[1]);
		newEntry->setting = &(newEntry->name[keywordLen+1]);

		strncpy(newEntry->name, token, keywordLen);
		newEntry->name[keywordLen] = '\0';

		strncpy((char *)newEntry->setting, equals+1, settingLen);
		newEntry->setting[settingLen] = '\0';

		newEntry->next = *data;
		*data = newEntry;
	}

	return result;
}

int readOptionFile(const char * fileName, const char * sectionName, OptionFileData ** data, int crcRequired)
{
	enum 	{ LineSize = 256 };
	char	line[LineSize];
	int	result = OK;

	FILE * fp = fopen(fileName, "r");
	int	lineNumber = 0;
	int	foundSection = 0;
	if ( !fp )
	{
		perror(fileName);
		result = ERROR;
	}

	while ( result == OK &&
           fgets(line, LineSize, fp) )
	{
		char * token = line + strspn(line, " \t\r\n");
		token[strcspn(token, "\r\n")] = '\0';
		
		lineNumber += 1;
		switch ( token[0] )
		{
		case '#':	// comment line
			break;

		case '[':	// section header
			result = processSectionHeader(fileName, lineNumber, &token[1], sectionName, &foundSection);
			break;

		default:		// keyword
			if ( foundSection && token[0] )
			{
				if(!crcRequired) {
					char* c = strchr(token, ',');
					if (c != NULL) *c = '\0';
				}		
				result = processKeyword(fileName, lineNumber, token, data, crcRequired);
				
			}
			break;
		}
	}

	if ( fp )
	{
		fclose(fp);
	}

	return result;
}

int trimaSysLoadOptionFiles(void)
{
	int result = 0;
	if ( readOptionFile("sw.dat", "FEATURES", &swData, 0) != OK )
	{
		result = -1;
	}
	return result;
}

int getOption(const char * optionName, const char ** optionSetting, OptionFileData * data)
{
	OptionFileData * entry = data;

	*optionSetting = NULL;
	while ( entry && !*optionSetting )
	{
		if ( strcmp(entry->name, optionName) == 0 )
		{
			*optionSetting = entry->setting;
		}

		entry = entry->next;
	}

	return (*optionSetting) ? OK : ERROR;
}

int trimaSysGetSoftwareSetting(const char *optionName, unsigned long *value)
{
	int errorCode = -1;
	const char * setting;
	unsigned long	result = 0;

	if ((value != NULL) && (getOption(optionName, &setting, swData) == OK)) {
		unsigned int i = 0;

		errorCode = 0;

		for (; i < strlen(setting); ++i)
		{
			if ( setting[i] == '0' )
			{
				result *= 2;
			}
			else if ( setting[i] == '1' )
			{
				result = result*2 + 1;
			}
			else
			{
				result = 0;
				errorCode = -1;
				fprintf(stderr, "Illegal software setting %s=%s\n", optionName, setting);
				break;
			}
		}	

		*value = result;
	}

	return errorCode;
}

bool attrib(const char*, const char*)
{
    return true;
}


