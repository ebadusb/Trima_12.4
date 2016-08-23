// log_convert.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "zlib.h"
#include <io.h>
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
	char	type = '\0';

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

static void parsePrint(const char * format, gzFile zfp)
{
	unsigned int	starModifierCount;
	size_t			currentIndex = 0;
	PrintArg_Type	argType;
	bool				argsDone = false;

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
					printf(" %d\n", fieldLen);
				}

				switch ( argType )
				{
				case PrintArg_Char:
					{
						char	chArg;
						egzread(zfp, &chArg, sizeof(chArg));
						printf(" %c\n", chArg);
					}
					break;

				case PrintArg_Int:
					{
						int	intArg;
						egzread(zfp, &intArg, sizeof(intArg));
						printf(" %d\n", intArg);
					}
					break;

				case PrintArg_Long:
					{
						long	longArg;
						egzread(zfp, &longArg, sizeof(longArg));
						printf(" %ld\n", longArg);

					}
					break;

				case PrintArg_Float:
					{
						float	floatArg;
						egzread(zfp, &floatArg, sizeof(floatArg));
						printf(" %lg\n", (double)floatArg);
					}
					break;

				case PrintArg_Double:
					{
						double	doubleArg;
						egzread(zfp, &doubleArg, sizeof(doubleArg));
						printf(" %lg\n", doubleArg);
					}
					break;

				case PrintArg_String:
					{
						unsigned short len;
						egzread(zfp, &len, sizeof(len));
						printf(" ");

						for (int i=0; i<len; i++)
						{
							int ch = egzgetc(zfp);
							putchar(ch);
						}
						printf("\n");
					}
					break;

				default:
					printf(" ** unknown arg type **\n");
					break;
			   }
			}
		}
}

int main(int argc, char* argv[])
{
	if ( argc != 2 )
	{
		fprintf(stderr, "Usage: log_convert <file_name>\n");
		exit(1);
	}

	FILE * fp = fopen(argv[1], "rb");
	if ( !fp )
	{
		perror(argv[1]);
		exit(1);
	}

	int	ch;
	bool	done = false;
	while ( (ch = fgetc(fp)) != EOF )
	{
		if (ch >= 0x20) putchar(ch);
		if ( ch == '\0' )
		{
			break;
		}
	}

	printf("\n");

	fgetc(fp); fgetc(fp); fgetc(fp); fgetc(fp);
	if ( ch == EOF )
	{
		printf(" unexpected end\n");
		done = true;
	}

    unsigned short  version;
	
	unsigned short headerType;
	fread(&headerType, sizeof(headerType), 1, fp);
	if ( headerType != 0x5500 )
	{
		printf("invalid header type %x\n", (int)headerType);
		done = true;
	}
	else
	{
		unsigned short	tempShort;
		unsigned int	tempInt;
		int i;

		printf("\nLog file header:\n");
		fread(&tempShort, sizeof(tempShort), 1, fp);  printf(" sizeof(char) = %d\n", (int)tempShort);
		fread(&tempShort, sizeof(tempShort), 1, fp);  printf(" sizeof(int) = %d\n", (int)tempShort);
		fread(&tempShort, sizeof(tempShort), 1, fp);  printf(" sizeof(long) = %d\n", (int)tempShort);
		fread(&tempShort, sizeof(tempShort), 1, fp);  printf(" sizeof(float) = %d\n", (int)tempShort);
		fread(&tempShort, sizeof(tempShort), 1, fp);  printf(" sizeof(double) = %d\n", (int)tempShort);
		fread(&tempShort, sizeof(tempShort), 1, fp);  printf(" sizeof(task ID) = %d\n", (int)tempShort);
		fread(&tempInt, sizeof(tempInt), 1, fp);  printf(" DATALOG_CURRENT_TASK = %d\n", tempInt);
		fread(&tempShort, sizeof(tempShort), 1, fp);  printf(" sizeof(node ID) = %d\n", (int)tempShort);
 		fread(&version, sizeof(version), 1, fp);  printf(" version = %d.%d\n", (version&0xff00) >> 8, (version & 0xff));
		fread(&tempShort, sizeof(tempShort), 1, fp);  printf(" platform name = ");
		for ( i=0; i<tempShort; i++ )
		{
			ch = fgetc(fp);
			putchar(ch);
		}
		printf("\n");

		fread(&tempInt, sizeof(tempInt), 1, fp); printf(" node ID = %08x\n", (unsigned int)tempInt);

		ch = fgetc(fp); printf("\n day = %d\n", (int)ch);
		ch = fgetc(fp); printf(" month = %d\n", (int)ch);
		fread(&tempShort, sizeof(tempShort), 1, fp);  printf(" year = %d\n", (int)tempShort);
		ch = fgetc(fp); printf(" hour = %d\n", (int)ch);
		ch = fgetc(fp); printf(" minute = %d\n", (int)ch);
		ch = fgetc(fp); printf(" second = %d\n", (int)ch);

		fread(&tempShort, sizeof(tempShort), 1, fp);  printf(" node name = ");
		for ( i=0; i<tempShort; i++ )
		{
			ch = fgetc(fp);
			putchar(ch);
		}
	}

    if (version != 0x0301)
    {
        printf("log file version %d.%d unsupported\n", (version&0xff00) >> 8, (version & 0xff));
        exit(1);
    }

	gzFile	zfp = NULL;
	if ( !done )
	{
		fpos_t  fpPos; 

		fgetpos(fp, &fpPos);
		fclose(fp);

		int	fd = open(argv[1], O_RDONLY | O_BINARY);
		lseek(fd, (long)fpPos, SEEK_SET);

		zfp = gzdopen(fd, "rb");
	}

 	while ( !done )
	{
		unsigned short recordType;
		unsigned short	tempShort, tempShort2, tempShort3;
		unsigned int	tempInt, tempInt2;
		unsigned short formatLen, fileLen;
		char * format;
		int i;

		if ( egzread(zfp, &recordType, sizeof(recordType)) <= 0 ) break;

		switch ( recordType )
		{
		case 0x5501:
			printf("\nLog level record:\n");
			egzread(zfp, &tempInt, sizeof(tempInt));  egzread(zfp, &tempInt2, sizeof(tempInt2));  printf(" time = %d.%09d\n", tempInt, tempInt2);
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" level ID = %d\n", (int)tempShort);
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" node ID = %08x\n", (unsigned int)tempInt);
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" level name = ");
			for ( i=0; i<tempShort; i++ )
			{
				ch = egzgetc(zfp);
				putchar(ch);
			}
			printf("\n");
			break;

		case 0x5502:
			printf("\nPrint record:\n");
			egzread(zfp, &tempInt, sizeof(tempInt));  egzread(zfp, &tempInt2, sizeof(tempInt2));  printf(" time = %d.%09d\n", tempInt, tempInt2);
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" level ID = %d\n", (int)tempShort);
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" task ID = %08x\n", (unsigned int)tempInt);
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" node ID = %08x\n", (unsigned int)tempInt);
			egzread(zfp, &formatLen, sizeof(formatLen));
			egzread(zfp, &fileLen, sizeof(fileLen));
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" line = %d\n", (int)tempShort);

			format = new char[formatLen+1];
			egzread(zfp, format, formatLen);
			format[formatLen] = '\0';

			printf(" file = ");
			for ( i=0; i<fileLen; i++ )
			{
				ch = egzgetc(zfp);
				putchar(ch);
			}
			printf("\n");

			printf(" format = %s\n", format);
			parsePrint(format, zfp);
			delete[] format;
			break;

		case 0x5503:
			printf("\nStream record:\n");
			egzread(zfp, &tempInt, sizeof(tempInt));  egzread(zfp, &tempInt2, sizeof(tempInt2));  printf(" time = %d.%09d\n", tempInt, tempInt2);
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" level ID = %d\n", (int)tempShort);
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" task ID = %08x\n", (unsigned int)tempInt);
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" node ID = %08x\n", (unsigned int)tempInt);
			egzread(zfp, &fileLen, sizeof(fileLen));
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" line = %d\n", (int)tempShort);

			printf(" file = ");
			for ( i=0; i<fileLen; i++ )
			{
				ch = egzgetc(zfp);
				putchar(ch);
			}
			printf("\n");

			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" flags = %x\n", (int)tempShort);
			ch = egzgetc(zfp);
			printf(" precision = %d\n", (int)ch);

			egzread(zfp, &formatLen, sizeof(formatLen));
			int	readLen;

			readLen = 0;
			while ( readLen < formatLen )
			{
				int type = egzgetc(zfp);
				readLen += 1;

				switch (type)
				{
				case 1:
				case 2:
					ch = egzgetc(zfp);
					readLen += 1;
					printf(" type %d %c\n", type, ch);
					break;

				case 3:
				case 5:
					egzread(zfp, &tempInt, sizeof(tempInt)); printf(" type %d %d (%x)\n", type, tempInt, (unsigned int)tempInt);
					readLen += 4;
					break;

				case 4:
				case 6:
					egzread(zfp, &tempInt, sizeof(tempInt)); printf(" type %d %u (%x)\n", type, (unsigned int)tempInt, (unsigned int)tempInt);
					readLen += 4;
					break;

				case 7:
					egzread(zfp, &tempShort, sizeof(tempShort));
					readLen += 2 + tempShort;
					printf(" type %d \"", type);
					for ( i=0; i<tempShort; i++ )
					{
						ch = egzgetc(zfp);
						putchar(ch);
				   }
				   printf("\"\n");
				   break;

				case 8:
					float tempFloat;
					egzread(zfp, &tempFloat, sizeof(tempFloat));
					readLen += 4;

					printf(" type %d %lg\n", type, (double)tempFloat);
					break;

				case 9:
					double tempDouble;
					egzread(zfp, &tempDouble, sizeof(tempDouble));
					readLen += 8;

					printf(" type %d %lg\n", type, tempDouble);
					break;

				case 10:
					ch = egzgetc(zfp);
					readLen += 1;

					printf(" type %d %s\n", type, (ch) ? "true" : "false");
					break;

				case 100:
					egzread(zfp, &tempShort, sizeof(tempShort));  printf(" flags = %x\n", (int)tempShort);
					readLen += 2;
					break;

				case 101:
					ch = egzgetc(zfp);
					readLen += 1;
					printf(" precision = %d\n", (int)ch);
					break;

				default:
					printf(" bad argument type\n");
					break;
				}
			}
			break;

		case 0x5504:
			printf("\nPeriodic log output record:\n");
			egzread(zfp, &tempInt, sizeof(tempInt));  egzread(zfp, &tempInt2, sizeof(tempInt2));  printf(" time = %d.%09d\n", tempInt, tempInt2);
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" set ID = %d\n", (int)tempShort);
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" node ID = %08x\n", (unsigned int)tempInt);
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" item count = %d\n", (int)tempShort);
			for (i=0; i<tempShort; i++)
			{
				unsigned short itemSize, keyCode;
				int	j;

				egzread(zfp, &itemSize, sizeof(itemSize));
				egzread(zfp, &keyCode, sizeof(keyCode));
				printf(" item size=%d, key=%d, data=", (int)itemSize, (int)keyCode);
				for (j=0; j<itemSize; j++)
				{
					ch = egzgetc(zfp);
					printf("%02x ", (unsigned int)ch);
				}
				printf("\n");
			}
			break;

		case 0x5505:
			printf("\nPeriodic set record:\n");
			egzread(zfp, &tempInt, sizeof(tempInt));  egzread(zfp, &tempInt2, sizeof(tempInt2));  printf(" time = %d.%09d\n", tempInt, tempInt2);
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" set ID = %d\n", (int)tempShort);
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" node ID = %08x\n", (unsigned int)tempInt);
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" set name = ");
			for ( i=0; i<tempShort; i++ )
			{
				ch = egzgetc(zfp);
				putchar(ch);
			}
			printf("\n");
			break;

		case 0x5506:
			printf("\nPeriodic set item record:\n");
			egzread(zfp, &tempInt, sizeof(tempInt));  egzread(zfp, &tempInt2, sizeof(tempInt2));  printf(" time = %d.%09d\n", tempInt, tempInt2);
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" key code = %d\n", (int)tempShort);
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" node ID = %08x\n", (unsigned int)tempInt);
			egzread(zfp, &tempShort, sizeof(tempShort));
			egzread(zfp, &tempShort2, sizeof(tempShort2));
			egzread(zfp, &tempShort3, sizeof(tempShort3));

 			printf(" key name = ");
			for ( i=0; i<tempShort; i++ )
			{
				ch = egzgetc(zfp);
				putchar(ch);
			}
			printf("\n");

			printf(" description = ");
			for ( i=0; i<tempShort2; i++ )
			{
				ch = egzgetc(zfp);
				putchar(ch);
			}
			printf("\n");

			printf(" format = ");
			for ( i=0; i<tempShort3; i++ )
			{
				ch = egzgetc(zfp);
				putchar(ch);
			}
			printf("\n");

			break;

		case 0x5507:
			printf("\nTask create record:\n");
			egzread(zfp, &tempInt, sizeof(tempInt));  egzread(zfp, &tempInt2, sizeof(tempInt2));  printf(" time = %d.%09d\n", tempInt, tempInt2);
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" level ID = %d\n", (int)tempShort);
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" task ID = %08x\n", (unsigned int)tempInt);
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" node ID = %08x\n", (unsigned int)tempInt);
			egzread(zfp, &formatLen, sizeof(formatLen));

			format = new char[formatLen+1];
			egzread(zfp, format, formatLen);
			format[formatLen] = '\0';

			printf(" task name = %s\n", format);
			delete[] format;
			break;

		case 0x5508:
			printf("\nTask delete record:\n");
			egzread(zfp, &tempInt, sizeof(tempInt));  egzread(zfp, &tempInt2, sizeof(tempInt2));  printf(" time = %d.%09d\n", tempInt, tempInt2);
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" level ID = %d\n", (int)tempShort);
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" task ID = %08x\n", (unsigned int)tempInt);
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" node ID = %08x\n", (unsigned int)tempInt);
			break;

		case 0x5509:
			printf("\nNetwork log header:\n");
			egzread(zfp, &tempInt, sizeof(tempInt)); printf(" node ID = %08x\n", (unsigned int)tempInt);
			ch = egzgetc(zfp); printf(" day = %d\n", (int)ch);
			ch = egzgetc(zfp); printf(" month = %d\n", (int)ch);
			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" year = %d\n", (int)tempShort);
			ch = egzgetc(zfp); printf(" hour = %d\n", (int)ch);
			ch = egzgetc(zfp); printf(" minute = %d\n", (int)ch);
			ch = egzgetc(zfp); printf(" second = %d\n", (int)ch);

			egzread(zfp, &tempShort, sizeof(tempShort));  printf(" node name = ");
			for ( i=0; i<tempShort; i++ )
			{
				ch = egzgetc(zfp);
				putchar(ch);
			}
			printf("\n");

			break;

		case 0x55f0:
			printf("\nBinary record:");
			egzread(zfp, &tempInt, sizeof(tempInt));
			for ( i=0; i<(int)tempInt; i++ )
			{
				if ( i%16 == 0 )
				{
					printf("\n  ");
				}

				ch = egzgetc(zfp);
				printf("%02x ", ch);
			}

			printf("\n");
			break;

		case 0x55fd:
			printf("\nEnd of log file record:\n");
			egzread(zfp, &tempInt, sizeof(tempInt));  egzread(zfp, &tempInt2, sizeof(tempInt2));  printf(" time = %d.%09d\n", tempInt, tempInt2);
			break;

		case 0x55fe:
			printf("\nTime stamp record:\n");
			egzread(zfp, &tempInt, sizeof(tempInt));  egzread(zfp, &tempInt2, sizeof(tempInt2));  printf(" time = %d.%09d\n", tempInt, tempInt2);
			break;

		case 0x55ff:
			printf("\nData missed record\n");
			break;

		default:
			printf("\nUnknown record type %04x\n", recordType);
//			done = true;
		}
	}

	return 0;
}

