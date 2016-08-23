/*****************************************************
 * FILENAME: makebigfile.cpp
 * PURPOSE: utility that creates a big file
 * USAGE: makebigfile <filename> <size in MB>
 * CHANGELOG:
 *   $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/makebigfile/rcs/makebigfile.cpp 6.10 2004/01/19 21:03:11Z td07711 Exp td07711 $
 *   $Log: makebigfile.cpp $
 *   Revision 6.10  2004/01/19 21:03:11Z  td07711
 *   IT6599 - use open() instead of creat()
 *   Revision 6.9  2004/01/13 22:12:57Z  td07711
 *   IT6599 - bugfix - create filesizes as specified by -size
 *   Revision 6.8  2003/12/01 21:04:29Z  td07711
 *   IT6599 - bugfix, replace sizeof(buf) with bufsize.
 *   Revision 6.7  2003/11/24 18:25:36Z  td07711
 *   IT6599 - add -priority cmd line option
 *   Revision 6.6  2003/11/24 18:07:59Z  td07711
 *   IT6599 - added cmd line options
 *   Revision 6.5  2003/11/24 17:24:30Z  td07711
 *   IT6599 - fix to avoid vxworks filesystem bug
 *   Revision 6.4  2003/11/21 22:38:16Z  td07711
 *   IT6599 - port makebigfile for vxworks
 *   Revision 6.3  2002/05/30 20:20:50Z  js70375
 *   changed entry point to makebigfile_main
 *   Revision 6.2  2002/05/06 21:41:46Z  js70375
 *   strimbu - change to parse function
 *   Revision 1.1  2002/05/01 21:20:58Z  js70375
 *   Initial revision
 *   Revision 1.1  1998/10/21 19:02:31Z  TD07711
 *   Initial revision
 *   10/21/98 - dyes - initial version
 *****************************************************/

// vxworks includes
#include <vxWorks.h>
#include <ioLib.h>

// common includes
#include "optionparser.h"
#include "datalog_levels.h"

// libtool includes
#include "logger.hpp"


#ifdef __cplusplus
extern "C" int makebigfile_main( const char* args );
#endif


int makebigfile_main( const char* args )
{
    Logger::Init( "makebigfile_main" );
    Logger::LogToStderr(); // to get log msgs to datalog


    // parse command line
    OptionParser commandline( "makebigfile_main", "creates bogus rundata files" );
    commandline.init( args );

    // storage for parsed command line options
    const char* Name;
    const char* Directory;
    int Size;
    int NumFiles;
    int Seqnum;
    int Priority;

    commandline.parse( "-name", "filename prefix, ex 1T00030_19990131", &Name, "bogusfile" );
    commandline.parse( "-dir", "directory to create files in", &Directory, "/machine/log" );
    commandline.parse( "-size", "file size in Mbytes", &Size, 100, 1, 2000 );
    commandline.parse( "-n", "number of files to create", &NumFiles, 1, 1, 10000 );
    commandline.parse( "-seqnum", "initial sequence number", &Seqnum, 1, 1, 999899 );
    commandline.parse( "-priority", "set task priority", &Priority, 100, 1, 255 );

    commandline.done();

    if( commandline.anyErrors() )
    {
	return 1;
    }

    // set process priority
    if( taskPrioritySet( taskIdSelf(), Priority ) != 0 ) 
    {
        LOG_ERROR( "taskPrioritySet failed to set priority to %d", Priority );
        return 1;
    }

    //
    // create the files
    //
    char filename[ 100 ];
    int fd;
    int n;
    int numWrites = Size*100; // 100 10000byte writes per MB
    bool error = false;
    
    const int bufsize = 10000;
    char* buf = new char[ bufsize ]; 
    memset( buf, (int)'Z', bufsize );

    for( int i = 1; i <= NumFiles; i++, Seqnum++ )
    {
	sprintf( filename, "%s/%s_%03d_%06d.dlog", 
		 Directory, Name, i, Seqnum );

	fd = open( filename, O_CREAT | O_WRONLY, 0666 );
	if( fd == ERROR )
	{
	    LOG_ERROR( "failed to create %s", filename );
	    error = true;
	    break;
	}
	ftruncate( fd, 0 ); // IT6599 - workaround for vxworks filesystem problem

	//
	// NOTE: vxworks filesystem has bug where partition becomes unusable if you lseek
	// past end of file.  So we have to do actual writes to build the file instead.
	//
	for( int j = 1; j <= numWrites; j++ )
	{
	    n = write( fd, buf, bufsize );
	    if( n == ERROR )
	    {
		LOG_ERROR( "failed to write %s", filename );
		error = true;
		break;
	    }
	    if( n != bufsize )
	    {
		LOG_ERROR( "partial write of %d bytes to %s, expected %d", 
			   n, filename, bufsize );
		error = true;
		break;;
	    }
	}
	if( error )
	{
	    close( fd );
	    break;
	}

	LOG_INFO( "created %s size=%d Mbytes", filename, Size );
	close( fd );
    }

    //
    // cleanup and return
    //
    delete [] buf;
    if( error )
    {
	return 1;
    }
    else
    {
	return 0;
    }
}
