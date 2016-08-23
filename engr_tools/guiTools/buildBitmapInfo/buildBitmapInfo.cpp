/*
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/engr_tools/guiTools/buildBitmapInfo/rcs/buildBitmapInfo.cpp 1.8 2009/03/20 16:22:14Z dslausb Exp dslausb $
 * $Log: buildBitmapInfo.cpp $
 * Revision 1.8  2009/03/20 16:22:14Z  dslausb
 * Fix transparent bitmap generation stuff
 * Revision 1.7  2009/02/12 22:37:48Z  dslausb
 * Update!!!
 * Revision 1.6  2009/02/10 16:38:27Z  dslausb
 * Get rid of extra dependencies in headers for Win32 tools.
 * Revision 1.5  2009/02/06 00:25:46Z  dslausb
 * Fix compression buffer size
 * Revision 1.3  2009/02/04 21:05:12Z  dslausb
 * IT 8277 - Port transparent bitmaps over from cgui
 * Revision 1.2  2003/11/14 15:19:24Z  jl11312
 * - added support for service mode palette
 * Revision 1.1  2003/08/21 15:54:03Z  jl11312
 * Initial revision
 *
 * Utility to pre-process all Trima bitmap data
 */

#include "stdafx.h"
#include <list>
#include <string>
#include <string.h>
#include <windows.h>
#include "zlib.h"

using namespace std;

struct BitmapDataEntry
{
	string	id;
	string	absolute;
	string	fileName;
	string	maskFileName;
	int		width;
	int		height;


	BitmapDataEntry(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct BitmapAliasEntry
{
	string 	id;
	string	aliasId;
};

list<BitmapDataEntry *>		bitmapTable;
list<BitmapAliasEntry *>	aliasTable;

struct OSPaletteEntry
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

OSPaletteEntry normalPalette[256] =
{
  {   0,    0,   0 },
  {  247, 247, 247 },
  {  239, 239, 239 },
  {  231, 231, 231 },
  {  222, 222, 222 },
  {  181, 181, 181 },
  {  173, 173, 173 },
  {  165, 165, 165 },
  {  156, 156, 156 },
  {  148, 148, 148 },
  {  140, 140, 140 },
  {  132, 132, 132 },
  {  123, 123, 123 },
  {  115, 115, 115 },
  {  107, 107, 107 },
  {   99,  99,  99 },
  {   90,  90,  90 },
  {   74,  74,  74 },
  {   57,  57,  57 },
  {   49,  49,  49 },
  {   16,  16,  16 },
  {  222, 214, 214 },
  {  206, 198, 198 },
  {  189, 181, 181 },
  {  173, 165, 165 },
  {  115, 107, 107 },
  {  181, 165, 165 },
  {  156,  90,  90 },
  {  247, 132, 132 },
  {  173,  49,  49 },
  {  156,  33,  33 },
  {  156,  24,  24 },
  {  189,  16,  16 },
  {  239,   8,   0 },
  {  222,   8,   0 },
  {  132,  16,   8 },
  {  173, 107,  99 },
  {  239,  33,   0 },
  {  115,  82,  74 },
  {  239,  74,  16 },
  {  165, 107,  82 },
  {  115,  82,  66 },
  {  206, 148, 115 },
  {  247, 107,  24 },
  {  247, 123,  33 },
  {  148,  99,  57 },
  {  156, 107,  57 },
  {  247, 140,  33 },
  {  140, 107,  66 },
  {  198, 148,  82 },
  {  156, 115,  57 },
  {  156, 123,  66 },
  {  123, 107,  74 },
  {  247, 181,  49 },
  {  255, 198,  49 },
  {  165, 148,  90 },
  {  255, 239, 181 },
  {  206, 173,  49 },
  {  239, 198,  41 },
  {  198, 173,  66 },
  {  181, 156,  49 },
  {  222, 189,  41 },
  {  247, 206,  41 },
  {  206, 189, 107 },
  {  206, 189,  99 },
  {  206, 181,  66 },
  {  222, 214, 165 },
  {  206, 189,  90 },
  {  247, 222,  66 },
  {  255, 222,  41 },
  {  173, 165, 107 },
  {  222, 206,  82 },
  {  198, 189, 107 },
  {  189, 181,  99 },
  {  239, 222,  74 },
  {  231, 214,  66 },
  {  247, 231,  66 },
  {  214, 198,  57 },
  {  255, 247, 132 },
  {  206, 198,  99 },
  {  181, 173,  57 },
  {  255, 239,  66 },
  {  140, 132,  33 },
  {  214, 206,  74 },
  {  255, 247,  57 },
  {  173, 173, 165 },
  {  165, 165, 156 },
  {  148, 148, 140 },
  {  132, 132, 123 },
  {  107, 107,  99 },
  {  132, 132, 115 },
  {  214, 214, 173 },
  {  255, 255, 189 },
  {  222, 222, 115 },
  {  255, 255, 123 },
  {  222, 222,  90 },
  {  132, 132,  49 },
  {  231, 231,  82 },
  {  222, 231, 140 },
  {  206, 214, 140 },
  {  140, 148,  99 },
  {  140, 156,  82 },
  {  123, 140,  57 },
  {  181, 198, 148 },
  {  115, 132,  90 },
  {  123, 140,  99 },
  {  181, 206, 148 },
  {  173, 198, 148 },
  {  115, 132,  99 },
  {   90,  99,  82 },
  {  173, 214, 165 },
  {  165, 173, 165 },
  {  165, 214, 165 },
  {  140, 198, 156 },
  {  140, 173, 156 },
  {  140, 189, 173 },
  {  123, 198, 181 },
  {  107, 189, 181 },
  {  206, 214, 214 },
  {  189, 198, 198 },
  {  140, 148, 148 },
  {  123, 132, 132 },
  {  165, 181, 181 },
  {  206, 239, 239 },
  {   49,  57,  57 },
  {  173, 206, 206 },
  {  107, 132, 132 },
  {  148, 198, 198 },
  {   82, 115, 115 },
  {  140, 198, 198 },
  {  115, 198, 198 },
  {  107, 189, 189 },
  {   90, 181, 181 },
  {   66, 132, 132 },
  {   82, 173, 173 },
  {   82, 181, 181 },
  {   66, 165, 165 },
  {   57, 156, 156 },
  {   41, 132, 132 },
  {   33, 132, 132 },
  {   33, 140, 140 },
  {   24, 132, 132 },
  {   16, 132, 132 },
  {    0, 115, 115 },
  {  107, 165, 173 },
  {   82, 140, 148 },
  {   41,  82,  90 },
  {    8,  49,  57 },
  {    8,  82,  99 },
  {    0,  74,  90 },
  {   66, 132, 148 },
  {   41, 107, 123 },
  {   90, 148, 165 },
  {   57, 115, 132 },
  {    0,  57,  74 },
  {  107, 156, 173 },
  {  198, 222, 231 },
  {   16,  82, 107 },
  {  123, 140, 148 },
  {  140, 173, 189 },
  {  173, 189, 198 },
  {   74,  82,  90 },
  {   41,  74, 107 },
  {    0,  33,  66 },
  {   33,  74, 132 },
  {  165, 173, 189 },
  {   41,  66, 115 },
  {   49,  66, 107 },
  {  148, 165, 214 },
  {  123, 140, 189 },
  {  148, 156, 181 },
  {  140, 148, 181 },
  {   49,  57,  90 },
  {   99, 107, 148 },
  {   41,  49,  90 },
  {   49,  66, 156 },
  {  115, 123, 173 },
  {  107, 115, 165 },
  {   90,  99, 156 },
  {   74,  82, 132 },
  {   66,  74, 123 },
  {   57,  66, 115 },
  {   57,  66, 123 },
  {  115, 123, 181 },
  {  107, 115, 173 },
  {   99, 107, 165 },
  {   82,  90, 148 },
  {   74,  82, 140 },
  {   66,  74, 132 },
  {   49,  57, 115 },
  {   57,  66, 140 },
  {   49,  57, 132 },
  {  181, 181, 189 },
  {  140, 140, 148 },
  {  123, 123, 132 },
  {  148, 148, 165 },
  {  140, 140, 165 },
  {  148, 148, 181 },
  {   82,  82, 140 },
  {   74,  74, 132 },
  {   49,  49, 115 },
  {   49,  41, 123 },
  {  165, 156, 198 },
  {   82,  66, 132 },
  {   66,  41, 140 },
  {   57,  24, 132 },
  {   74,  41, 140 },
  {  107,  82, 156 },
  {   49,   8, 123 },
  {   74,  33, 140 },
  {   57,   8, 132 },
  {  189, 181, 198 },
  {  156, 140, 173 },
  {   99,  74, 115 },
  {   57,  24,  74 },
  {   66,  33,  82 },
  {   49,   8,  66 },
  {  115,  82, 123 },
  {   99,  66, 107 },
  {   82,  49,  90 },
  {   57,  16,  66 },
  {   74,  33,  82 },
  {   49,   0,  57 },
  {  198, 181, 198 },
  {  165, 148, 165 },
  {   74,  24,  66 },
  {  123,  82, 115 },
  {  107,  57,  82 },
  {  107,  16,  57 },
  {  181, 156, 165 },
  {  165,  90, 115 },
  {  140,  57,  82 },
  {  222, 189, 198 },
  {  156, 123, 132 },
  {  231, 173, 189 },
  {  148,  57,  82 },
  {  206, 173, 181 },
  {  173, 140, 148 },
  {  115,  82,  90 },
  {  107,  74,  82 },
  {  189, 123, 140 },
  {  181, 115, 132 },
  {  173, 107, 123 },
  {  148,  82,  99 },
  {  132,  66,  82 },
  {  107,  41,  57 },
  {  214, 140, 156 },
  {  206, 132, 148 },
  {  198, 123, 140 },
  {  156,  82,  99 },
  {   82,  41,  49 },
  {  156,  57,  74 },
  {  140,  33,  49 },
  {  148,  33,  41 },
  {  165,   8,  16 },
  {    0,   0,   0 }
};

OSPaletteEntry servicePalette[256] =
{
  {   0,   0,   0 },
  {   5,   0,   0 },
  {   0,   5,   0 },
  {   0,   0,   5 },
  {   5,   5,   5 },
  {   9,   0,   0 },
  {  13,   0,   0 },
  {   0,   9,   0 },
  {   0,  13,   0 },
  {   0,   0,   9 },
  {   0,   0,  13 },
  {   9,   9,   9 },
  {  13,  13,  13 },
  {  17,   0,   0 },
  {  21,   0,   0 },
  {  25,   0,   0 },
  {  29,   0,   0 },
  {   0,  17,   0 },
  {   0,  21,   0 },
  {   0,  25,   0 },
  {   0,  29,   0 },
  {   0,   0,  17 },
  {   0,   0,  21 },
  {   0,   0,  25 },
  {   0,   0,  29 },
  {  17,  17,  17 },
  {  21,  21,  21 },
  {  25,  25,  25 },
  {  29,  29,  29 },
  {  33,   0,   0 },
  {  37,   0,   0 },
  {  41,   0,   0 },
  {  45,   0,   0 },
  {  49,   0,   0 },
  {  53,   0,   0 },
  {  57,   0,   0 },
  {  61,   0,   0 },
  {   0,  33,   0 },
  {   0,  37,   0 },
  {   0,  41,   0 },
  {   0,  45,   0 },
  {   0,  49,   0 },
  {   0,  53,   0 },
  {   0,  57,   0 },
  {   0,  61,   0 },
  {   0,   0,  33 },
  {   0,   0,  37 },
  {   0,   0,  41 },
  {   0,   0,  45 },
  {   0,   0,  49 },
  {   0,   0,  53 },
  {   0,   0,  57 },
  {   0,   0,  61 },
  {  33,  33,  33 },
  {  37,  37,  37 },
  {  41,  41,  41 },
  {  45,  45,  45 },
  {  49,  49,  49 },
  {  53,  53,  53 },
  {  57,  57,  57 },
  {  61,  61,  61 },
  {  65,   0,   0 },
  {  69,   0,   0 },
  {  73,   0,   0 },
  {  77,   0,   0 },
  {  81,   0,   0 },
  {  85,   0,   0 },
  {  89,   0,   0 },
  {  93,   0,   0 },
  {  97,   0,   0 },
  { 101,   0,   0 },
  { 105,   0,   0 },
  { 109,   0,   0 },
  { 113,   0,   0 },
  { 117,   0,   0 },
  { 121,   0,   0 },
  { 125,   0,   0 },
  {   0,  65,   0 },
  {   0,  69,   0 },
  {   0,  73,   0 },
  {   0,  77,   0 },
  {   0,  81,   0 },
  {   0,  85,   0 },
  {   0,  89,   0 },
  {   0,  93,   0 },
  {   0,  97,   0 },
  {   0, 101,   0 },
  {   0, 105,   0 },
  {   0, 109,   0 },
  {   0, 113,   0 },
  {   0, 117,   0 },
  {   0, 121,   0 },
  {   0, 125,   0 },
  {   0,   0,  65 },
  {   0,   0,  69 },
  {   0,   0,  73 },
  {   0,   0,  77 },
  {   0,   0,  81 },
  {   0,   0,  85 },
  {   0,   0,  89 },
  {   0,   0,  93 },
  {   0,   0,  97 },
  {   0,   0, 101 },
  {   0,   0, 105 },
  {   0,   0, 109 },
  {   0,   0, 113 },
  {   0,   0, 117 },
  {   0,   0, 121 },
  {   0,   0, 125 },
  {  65,  65,  65 },
  {  69,  69,  69 },
  {  73,  73,  73 },
  {  77,  77,  77 },
  {  81,  81,  81 },
  {  85,  85,  85 },
  {  89,  89,  89 },
  {  93,  93,  93 },
  {  97,  97,  97 },
  { 101, 101, 101 },
  { 105, 105, 105 },
  { 109, 109, 109 },
  { 113, 113, 113 },
  { 117, 117, 117 },
  { 121, 121, 121 },
  { 125, 125, 125 },
  { 129,   0,   0 },
  { 133,   0,   0 },
  { 137,   0,   0 },
  { 141,   0,   0 },
  { 145,   0,   0 },
  { 149,   0,   0 },
  { 153,   0,   0 },
  { 157,   0,   0 },
  { 161,   0,   0 },
  { 165,   0,   0 },
  { 169,   0,   0 },
  { 173,   0,   0 },
  { 177,   0,   0 },
  { 181,   0,   0 },
  { 185,   0,   0 },
  { 189,   0,   0 },
  { 193,   0,   0 },
  { 197,   0,   0 },
  { 201,   0,   0 },
  { 205,   0,   0 },
  { 209,   0,   0 },
  { 213,   0,   0 },
  { 217,   0,   0 },
  { 221,   0,   0 },
  { 225,   0,   0 },
  { 229,   0,   0 },
  { 233,   0,   0 },
  { 237,   0,   0 },
  { 241,   0,   0 },
  { 245,   0,   0 },
  { 249,   0,   0 },
  { 253,   0,   0 },
  {   0, 129,   0 },
  {   0, 133,   0 },
  {   0, 137,   0 },
  {   0, 141,   0 },
  {   0, 145,   0 },
  {   0, 149,   0 },
  {   0, 153,   0 },
  {   0, 157,   0 },
  {   0, 161,   0 },
  {   0, 165,   0 },
  {   0, 169,   0 },
  {   0, 173,   0 },
  {   0, 177,   0 },
  {   0, 181,   0 },
  {   0, 185,   0 },
  {   0, 189,   0 },
  {   0, 193,   0 },
  {   0, 197,   0 },
  {   0, 201,   0 },
  {   0, 205,   0 },
  {   0, 209,   0 },
  {   0, 213,   0 },
  {   0, 217,   0 },
  {   0, 221,   0 },
  {   0, 225,   0 },
  {   0, 229,   0 },
  {   0, 233,   0 },
  {   0, 237,   0 },
  {   0, 241,   0 },
  {   0, 245,   0 },
  {   0, 249,   0 },
  {   0, 253,   0 },
  {   0,   0, 129 },
  {   0,   0, 133 },
  {   0,   0, 137 },
  {   0,   0, 141 },
  {   0,   0, 145 },
  {   0,   0, 149 },
  {   0,   0, 153 },
  {   0,   0, 157 },
  {   0,   0, 161 },
  {   0,   0, 165 },
  {   0,   0, 169 },
  {   0,   0, 173 },
  {   0,   0, 177 },
  {   0,   0, 181 },
  {   0,   0, 185 },
  {   0,   0, 189 },
  {   0,   0, 193 },
  {   0,   0, 197 },
  {   0,   0, 201 },
  {   0,   0, 205 },
  {   0,   0, 209 },
  {   0,   0, 213 },
  {   0,   0, 217 },
  {   0,   0, 221 },
  {   0,   0, 225 },
  {   0,   0, 229 },
  {   0,   0, 233 },
  {   0,   0, 237 },
  {   0,   0, 241 },
  {   0,   0, 245 },
  {   0,   0, 249 },
  {   0,   0, 253 },
  {   0, 255, 255 },
  { 128, 128, 128 },
  { 133, 133, 133 },
  { 137, 137, 137 },
  { 141, 141, 141 },
  { 145, 145, 145 },
  { 149, 149, 149 },
  { 153, 153, 153 },
  { 157, 157, 157 },
  { 161, 161, 161 },
  { 165, 165, 165 },
  { 169, 169, 169 },
  { 173, 173, 173 },
  { 177, 177, 177 },
  { 181, 181, 181 },
  { 185, 185, 185 },
  { 189, 189, 189 },
  { 193, 193, 193 },
  { 197, 197, 197 },
  { 201, 201, 201 },
  { 205, 205, 205 },
  { 209, 209, 209 },
  { 213, 213, 213 },
  { 217, 217, 217 },
  { 221, 221, 221 },
  { 225, 225, 225 },
  { 229, 229, 229 },
  { 233, 233, 233 },
  { 237, 237, 237 },
  { 241, 241, 241 },
  { 245, 245, 245 },
  { 248, 248, 248 },
  { 250, 250, 250 },
  { 252, 252, 252 },
  { 254, 254, 0   }
};

OSPaletteEntry * palette = normalPalette;

void read_bitmap_info(FILE * bitmapInfo)
{
	char	lineBuffer[256];
	bool	readingFileTable = false;
	bool	readingAliasTable = false;
	int	line = 0;

	while ( fgets(lineBuffer, 256, bitmapInfo) != NULL )
	{
		line += 1;
		char * firstToken = strtok(lineBuffer, " \t\n");
		if ( !firstToken || firstToken[0] == '#' )
		{
			continue;
		}

		if ( stricmp(firstToken, "BITMAP_FILE_TABLE_START") == 0 )
		{
			if ( readingFileTable || readingAliasTable )
			{
				fprintf(stderr, "line %d: unexpected BITMAP_FILE_TABLE_START\n", line);
				exit(1);
			}

			readingFileTable = true;
			continue;
		}

		if ( stricmp(firstToken, "BITMAP_FILE_TABLE_END") == 0 )
		{
			if ( !readingFileTable )
			{
				fprintf(stderr, "line %d: unexpected BITMAP_FILE_TABLE_END\n", line);
				exit(1);
			}

			readingFileTable = false;
			continue;
		}

		if ( stricmp(firstToken, "BITMAP_ALIAS_TABLE_START") == 0 )
		{
			if ( readingFileTable || readingAliasTable )
			{
				fprintf(stderr, "line %d: unexpected BITMAP_ALIAS_TABLE_START\n", line);
				exit(1);
			}

			readingAliasTable = true;
			continue;
		}

		if ( stricmp(firstToken, "BITMAP_ALIAS_TABLE_END") == 0 )
		{
			if ( !readingAliasTable )
			{
				fprintf(stderr, "line %d: unexpected BITMAP_ALIAS_TABLE_END\n", line);
				exit(1);
			}

			readingAliasTable = false;
			continue;
		}

		if ( readingFileTable )
		{
			BitmapDataEntry	* entry = new BitmapDataEntry;

			entry->id = firstToken;
			entry->absolute = strtok(NULL, " \t\n");
			entry->fileName = strtok(NULL, " \t\n");
			char *pMaybe = strtok(NULL, " \t\n");
			if (pMaybe != NULL)
			{
				entry->maskFileName = pMaybe;
			}

			if ( entry->id.empty() || entry->absolute.empty() || entry->fileName.empty() )
			{
				fprintf(stderr, "line %d: bad entry in bitmap file table\n", line);
				exit(1);
			}

			if ( stricmp(entry->absolute.c_str(), "true") == 0 )
			{
				// absolute path - just add it to the table
				bitmapTable.push_back(entry);
				continue;
			}

			//
			// Not an absolute path - check if an entry for this file name has already been defined.
			// If it has, just add this entry to the alias table instead.
			//
			bool aliasEntryMade = false;
			list<BitmapDataEntry *>::iterator 	entryIter;
			for ( entryIter = bitmapTable.begin(); entryIter != bitmapTable.end(); ++entryIter )
			{
				if ( stricmp(entry->fileName.c_str(), (*entryIter)->fileName.c_str()) == 0 )
				{
					BitmapAliasEntry * aliasEntry = new BitmapAliasEntry;
					aliasEntry->id = entry->id;
					aliasEntry->aliasId = (*entryIter)->id;
					aliasTable.push_back(aliasEntry);

					aliasEntryMade = true;

					delete entry;
					break;
				}
			}

			if (!aliasEntryMade)
			{
				bitmapTable.push_back(entry);
			}

			continue;
		}

		if ( readingAliasTable )
		{
			BitmapAliasEntry	* entry = new BitmapAliasEntry;

			entry->id = firstToken;
			entry->aliasId = strtok(NULL, " \t\n");

			if ( entry->id.empty() || entry->aliasId.empty() )
			{
				fprintf(stderr, "line %d: bad entry in bitmap alias table\n", line);
				exit(1);
			}

			aliasTable.push_back(entry);
		}
	}

	if ( readingFileTable )
	{
		fprintf(stderr, "line %d: unterminated bitmap file table\n", line);
		exit(1);
	}

	if ( readingAliasTable )
	{
		fprintf(stderr, "line %d: unterminated bitmap alias table\n", line);
		exit(1);
	}
}

void generate_id_enum(FILE * bitmapOutId)
{
	fprintf(bitmapOutId, "// This file is auto-generated by the build_bitmap_info utility.  Do\n");
	fprintf(bitmapOutId, "// edit this file directly.\n");
	fprintf(bitmapOutId, "//\n\n");
	fprintf(bitmapOutId, "#ifndef __BITMAP_INFO_ID_H__\n#define __BITMAP_INFO_ID_H__\n\n");

	fprintf(bitmapOutId, "#ifndef WIN32 // Don't create vxWorks dependencies for win32 tools.\n");
	fprintf(bitmapOutId, "#include <vxWorks.h> // Needed for #if (CPU==SIMNT) directive.\n");
	fprintf(bitmapOutId, "#endif // #ifndef WIN32\n\n");

	fprintf(bitmapOutId, "enum BITMAP_ID\n{\n  BITMAP_NULL");

	list<BitmapDataEntry *>::iterator	dataIter;
	for ( dataIter = bitmapTable.begin(); dataIter != bitmapTable.end(); ++dataIter )
	{
		fprintf(bitmapOutId, ",\n  %s", (*dataIter)->id.c_str());
	}

	fprintf(bitmapOutId, ",\n\tBITMAP_ID_COUNT");

	list<BitmapAliasEntry *>::iterator	aliasIter;
	for ( aliasIter = aliasTable.begin(); aliasIter != aliasTable.end(); ++aliasIter )
	{
		fprintf(bitmapOutId, ",\n  %s = %s", (*aliasIter)->id.c_str(), (*aliasIter)->aliasId.c_str());
	}

	fprintf(bitmapOutId, "\n};\n\n#ifndef WIN32 // Don't create dependency on datalog for Win32 tools.\n");
	fprintf(bitmapOutId, "#if (CPU==SIMNT) // Only do special output in simulator.\n");
	fprintf(bitmapOutId, "#include \"trima_datalog.h\"\n");
	fprintf(bitmapOutId, "DataLog_Stream & operator << (DataLog_Stream &os, BITMAP_ID id);\n");
	fprintf(bitmapOutId, "#endif // #if (CPU==SIMNT)\n");
	fprintf(bitmapOutId, "#endif // #ifndef WIN32\n\n");

	fprintf(bitmapOutId, "#endif // #ifndef __BITMAP_INFO_ID_H__\n\n");
}

void generate_bitmap_data(FILE * bitmapOutData, FILE * bitmapOutDepend, FILE * bitmapLogging, const char * path)
{
	fprintf(bitmapOutData, "// This file is auto-generated by the build_bitmap_info utility.\n");
	fprintf(bitmapOutData, "//   DO NOT EDIT this file directly!\n");
	fprintf(bitmapOutData, "//\n\n");

	fprintf(bitmapOutData, "#ifndef WIN32 // Don't create vxWorks dependencies for win32 tools.\n");
	fprintf(bitmapOutData, "#include <vxWorks.h> // Needed for #if (CPU==SIMNT) directive.\n");
	fprintf(bitmapOutData, "#endif // #ifndef WIN32\n\n");

	fprintf(bitmapOutData, "#include \"filenames.h\"\n");
	fprintf(bitmapOutData, "#include \"bitmap_info.h\"\n\n");

	fprintf(bitmapLogging, "// This file is auto-generated by the build_bitmap_info utility.\n");
	fprintf(bitmapLogging, "// Do not edit this file directly.\n");
	fprintf(bitmapLogging, "//\n\n");

	fprintf(bitmapLogging, "#ifndef WIN32 // Don't create vxWorks dependencies for win32 tools.\n");
	fprintf(bitmapLogging, "#include <vxWorks.h> // Needed for #if (CPU==SIMNT) directive.\n");
	fprintf(bitmapLogging, "#include \"trima_datalog.h\"\n");
	fprintf(bitmapLogging, "#endif // #ifndef WIN32\n\n");

	fprintf(bitmapLogging, "#include \"bitmap_info_id.h\"\n\n");
    
	fprintf(bitmapLogging, "DataLog_Stream & operator << (DataLog_Stream &os, BITMAP_ID id)\n{\n\tswitch(id)\n\t{");

	fprintf(bitmapOutDepend, "bitmap_info.cpp bitmap_info_id.h bitmap_logging.cpp : bitmap.info");

	list<BitmapDataEntry *>::iterator	dataIter;

	for ( dataIter = bitmapTable.begin(); dataIter != bitmapTable.end(); ++dataIter )
	{
		if ( stricmp((*dataIter)->absolute.c_str(), "true") == 0 )
		{
			(*dataIter)->width = 0;
			(*dataIter)->height = 0;
			continue;
		}

		int loop = (*dataIter)->maskFileName.empty()? 1 : 2;

		string	fileName = path;
		fileName += (*dataIter)->fileName;

		for ( int m=0; m < loop; m++ )
		{
			FILE * bmapFile = fopen(fileName.c_str(), "rb");
			if ( !bmapFile )
			{
				perror(fileName.c_str());
				exit(1);
			}

			BITMAPFILEHEADER bmpFileHeader;
			BITMAPINFOHEADER bmpInfoHeader;

			fread((void *)&bmpFileHeader, sizeof(bmpFileHeader), 1, bmapFile);
			fread((void *)&bmpInfoHeader, sizeof(bmpInfoHeader), 1, bmapFile);

			BITMAPCOREHEADER bmpCoreHeader;
			
			if ( bmpInfoHeader.biSize == 0x0c )
			{
				memcpy(&bmpCoreHeader, &bmpInfoHeader, sizeof(bmpCoreHeader));
			}
			else
			{
				bmpCoreHeader.bcWidth = bmpInfoHeader.biWidth;
				bmpCoreHeader.bcHeight = bmpInfoHeader.biHeight;
				bmpCoreHeader.bcBitCount = bmpInfoHeader.biBitCount;
			}

			(*dataIter)->width = bmpCoreHeader.bcWidth;
			(*dataIter)->height = bmpCoreHeader.bcHeight;

			if ( bmpCoreHeader.bcWidth <= 0 || bmpCoreHeader.bcHeight <= 0 )
			{
				exit(1);
			}
			
			OSPaletteEntry	bmpPalette[256];
			for ( int color=0; color<256; color++ )
			{
				bmpPalette[color].blue = fgetc(bmapFile);
				bmpPalette[color].green = fgetc(bmapFile);
				bmpPalette[color].red = fgetc(bmapFile);
				fgetc(bmapFile);
			}
	
			unsigned char colorXlat[256];
			for ( int color=0; color<256; color++ )
			{
				int 	match = 0;
				int		matchScore = 999999;
	
				for ( int i=0; i<256 && matchScore!=0; i++ )
				{
					int	redScore = bmpPalette[color].red - palette[i].red;
					int	greenScore = bmpPalette[color].green - palette[i].green;
					int	blueScore = bmpPalette[color].blue - palette[i].blue;
					int	score = redScore * redScore + greenScore * greenScore + blueScore * blueScore;
	
					if ( i==0 || score < matchScore )
					{
						match = i;
						matchScore = score;
					}
				}
	
				colorXlat[color] = match;
			}
	
			// Don't worry, we reset stride before reading the transparent bitmap.
			unsigned int stride = (bmpCoreHeader.bcWidth + 3) / 4 * 4;
			uLongf	compressedBmpSize;
			unsigned char * compressedBmpData = NULL;

			size_t bmpSize = 0;

			/* read regular bitmap file and convert format */
			if ( m == 0 )
			{
				bmpSize = bmpCoreHeader.bcWidth * bmpCoreHeader.bcHeight;	
				unsigned char *	bmpData = new unsigned char[bmpSize];
		
				unsigned char *   lineData = bmpData + bmpCoreHeader.bcWidth*(bmpCoreHeader.bcHeight-1);
				fseek(bmapFile, bmpFileHeader.bfOffBits, SEEK_SET);
		
				for (unsigned int y = 1; y <= bmpCoreHeader.bcHeight; y++)
				{
					fread((void *)lineData, bmpCoreHeader.bcWidth, 1, bmapFile);
					if ( stride > bmpCoreHeader.bcWidth )
					{
						fseek(bmapFile, stride-bmpCoreHeader.bcWidth, SEEK_CUR);
					}
		
					lineData -= bmpCoreHeader.bcWidth;
				}
		
				for ( int i=0; i<bmpSize; i++ )
				{
					bmpData[i] = colorXlat[bmpData[i]];
				}
		
				compressedBmpSize = bmpSize*16/15 + 256;
				compressedBmpData = new unsigned char[compressedBmpSize];
		
				int g = compress2(compressedBmpData, &compressedBmpSize, (const Bytef *)bmpData, bmpSize, 9);
				if (g < Z_OK)
				{
					fprintf(stderr, "Unable to compress data for %s (retVal=%d)", fileName.c_str(), g);
					exit(1);
				}

				fprintf(bitmapOutData, "// %s\n", fileName.c_str());
				fprintf(bitmapOutData, "static const unsigned char %s_BMP[] = {\n", (*dataIter)->id.c_str());
				
				fprintf(bitmapLogging, "\n\t\tcase %s: os << \"%s(\" << ((int)id) << \")\"; break;", (*dataIter)->id.c_str(), (*dataIter)->id.c_str());

				delete[] bmpData;
			}
			
			stride = (3*bmpCoreHeader.bcWidth + 3) / 4 * 4;
	
			if ( m == 1 )
			{
				bmpSize = bmpCoreHeader.bcHeight * ((bmpCoreHeader.bcWidth+7)/8);
				unsigned char *	bmpData = new unsigned char[bmpSize];
 
				unsigned char *	lineData = bmpData + ((bmpCoreHeader.bcWidth+7)/8)*(bmpCoreHeader.bcHeight-1);
				fseek(bmapFile, bmpFileHeader.bfOffBits, SEEK_SET);
		
				for (unsigned int y = 1; y <= bmpCoreHeader.bcHeight; y++)
				{
					unsigned char monochromeData[3];

					/* zero out lineData */
					memset( lineData, 0, (bmpCoreHeader.bcWidth+7)/8);
					
					for ( unsigned int x = 0; x < bmpCoreHeader.bcWidth; x++)
					{
						/* read data and convert to monochrome format */
						fread((void *)monochromeData, 3, 1, bmapFile);

						/* make non-zero if white */
						if (monochromeData[0] == 0 || monochromeData[1] == 0 || monochromeData[2] == 0 )
						{
							lineData[x/8] |= 0x80 >> (x % 8);
						}
					}	/* for x */
					
					if ( stride > 3*bmpCoreHeader.bcWidth )
					{
						fseek(bmapFile, stride-3*bmpCoreHeader.bcWidth, SEEK_CUR);
					}

					lineData -= ( bmpCoreHeader.bcWidth + 7 ) / 8;

				}	/* for y */

				// Compression Buffer must be at least 110% the size of the uncompressed buffer.
				compressedBmpSize = (int)(((float)bmpSize) * 1.2);
				compressedBmpData = new unsigned char[compressedBmpSize];
				int g = compress2(compressedBmpData, &compressedBmpSize, (const Bytef *)bmpData, bmpSize, 9);

				if (g < Z_OK)
				{
					fprintf(stderr, "Unable to compress data for %s (retVal=%d)", fileName.c_str(), g);
					exit(1);
				}

				fprintf(bitmapOutData, "// %s\n", fileName.c_str());
				fprintf(bitmapOutData, "static const unsigned char %s_BMP_MASK[] = {\n", (*dataIter)->id.c_str());

				delete[] bmpData;
			}
	
			fclose(bmapFile);

			for ( int i=0; i<compressedBmpSize; i++ )
			{
				if (i != 0) // Don't print the first comma.
				{
					// Add a linebreak every 29th entry
					if ( (i % 29) == 0)
					{
						fprintf(bitmapOutData, ",\n  ");
					}
					// Add a comma after every entry
					else
					{
						fprintf(bitmapOutData, ", ");
					}
				}

                // Print a value from the compressed data.
				fprintf(bitmapOutData, "%d", (int)compressedBmpData[i]);
		   }
	
			fprintf(bitmapOutData, "\n};\n\n");
			delete[] compressedBmpData;

			fprintf(bitmapOutDepend, " \\\n  %s", fileName.c_str());
		
			if ( m == 0) 
			{
				fileName = path;
				fileName += (*dataIter)->maskFileName;
			}
		}
	}

	fprintf(bitmapOutDepend, "\n");

	fprintf(bitmapLogging, "\n\t\tdefault: os << \"UNKNOWN_BMP_ID(\" << ((int)id) << \")\"; break;\n\t}\n\n\treturn os;\n}\n\n");
}


void generate_bitmap_table(FILE * bitmapOutData)
{
	fprintf(bitmapOutData, "BITMAP_DATA_ENTRY bitmap_data_table[] = \n{\n  { /* BITMAP_NULL */ false, 0, 0, NULL, 0, NULL, 0 }");

	list<BitmapDataEntry *>::iterator	dataIter;

	for ( dataIter = bitmapTable.begin(); dataIter != bitmapTable.end(); ++dataIter )
	{
		if ( (*dataIter)->height > 0 )
		{
			if ( (*dataIter)->maskFileName.empty() )
			{
				fprintf(bitmapOutData, ",\n  { /* %s */ %s, %d, %d, %s_BMP, sizeof(%s_BMP), NULL, 0 }", (*dataIter)->id.c_str(), (*dataIter)->absolute.c_str(), (*dataIter)->width, (*dataIter)->height, (*dataIter)->id.c_str(), (*dataIter)->id.c_str());
			}
			else
			{
				fprintf(bitmapOutData, ",\n  { /* %s */ %s, %d, %d, %s_BMP, sizeof(%s_BMP), %s_BMP_MASK, sizeof(%s_BMP_MASK) }", (*dataIter)->id.c_str(), (*dataIter)->absolute.c_str(), (*dataIter)->width, (*dataIter)->height, (*dataIter)->id.c_str(), (*dataIter)->id.c_str(), (*dataIter)->id.c_str(), (*dataIter)->id.c_str());
			}
		}
		else
		{
			fprintf(bitmapOutData, ",\n  { /* %s */ %s, %d, %d, (const unsigned char *)%s, 0, NULL, 0 }", (*dataIter)->id.c_str(), (*dataIter)->absolute.c_str(), (*dataIter)->width, (*dataIter)->height, (*dataIter)->fileName.c_str());
		}
	}

	fprintf(bitmapOutData, "\n};\n");
}

int main(int argc, char * argv[])
{
	FILE  * bitmapInfo = NULL;
	FILE  * bitmapOutData = NULL;
	FILE  * bitmapOutId = NULL;
	FILE  * bitmapOutDepend = NULL;
    FILE  * bitmapLogging = NULL;
    
	if ( argc > 3 )
	{
		if ( (argc == 4) && strcmp(argv[3], "-service") == 0 )
		{
			palette = servicePalette;
		}
		else
		{
			fprintf(stderr, "Too many arguments. Usage: build_bitmap_info <source dir> <bitmap dir>\n");
			exit(1);
		}
	}
	else if (argc < 3)
	{
		fprintf(stderr, "Too few arguments. Usage: build_bitmap_info <source dir> <bitmap dir>\n");
		exit(1);
	}

	string fileName = argv[1];
	fileName += "bitmap.info";
	bitmapInfo = fopen(fileName.c_str(), "r");

	fileName = argv[1];
	fileName += "bitmap_info.cpp";
	bitmapOutData = fopen(fileName.c_str(), "w");

	fileName = argv[1];
	fileName += "bitmap_info_id.h";
	bitmapOutId = fopen(fileName.c_str(), "w");

	fileName = argv[1];
	fileName += ".dfile/bitmap_info_cpp.d";
	bitmapOutDepend = fopen(fileName.c_str(), "w");

	fileName = argv[1];
	fileName += "bitmap_logging.cpp";
	bitmapLogging = fopen(fileName.c_str(), "w");

	if ( !bitmapInfo || !bitmapOutData || !bitmapOutId || !bitmapOutDepend || !bitmapLogging)
	{
		fprintf(stderr, "One or more files was unable to be opened or accessed as required.\n");
		exit(1);
	}

	read_bitmap_info(bitmapInfo);
	generate_id_enum(bitmapOutId);
	generate_bitmap_data(bitmapOutData, bitmapOutDepend, bitmapLogging, argv[2]);
	generate_bitmap_table(bitmapOutData);

	fclose(bitmapInfo);
	fclose(bitmapOutData);
	fclose(bitmapOutId);
	fclose(bitmapOutDepend);
	fclose(bitmapLogging);

	return 0;
}

