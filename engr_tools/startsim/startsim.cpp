// startsim.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <direct.h>
#include <process.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

static char *pathname = strdup("");
static char *fspath = strdup("c:\\trima_vxsim\\");

#define ITDOESNOTEXIST 0
#define ITISAFILE      1
#define ITISADIRECTORY 2

int fileType( const char* path )
{
    struct stat statdata;
    int statret;
    statret = stat(path, &statdata );
    if( statret == -1 ) return( ITDOESNOTEXIST );
    if( statdata.st_mode & _S_IFDIR )  return( ITISADIRECTORY );
    else if( statdata.st_mode & _S_IFREG ) return( ITISAFILE );
    else return( ITDOESNOTEXIST );
}

void fini( int exitvalue )
{
    free( pathname );
    free( fspath );
    exit( exitvalue );
}


void usage(void)
{
    fprintf(stderr, "Usage: startsim [-p processor_number] [-m memory_size]\n                [-v simulator_pathname] [-f|-R tgtsvr_dir]\n                [-b tgtsvr_buffer_size]\n");
    fprintf(stderr, " processor_number must be in the range 0-9\n");
    fprintf(stderr, " memory_size (in megabytes) must be in the range 2-128\n");
    fprintf(stderr, " simulator_pathname must be a fully-qualified path to the\n  directory where vxworks.exe resides (default=.).\n");
    fprintf(stderr, " tgtsvr_dir must be a fully-qualified path to the\n  directory where /tgtsvr is to be mapped.\n");
    fprintf(stderr, " tgtsvr_buffer size (for the -m option of tgtsvr command line, in bytes)\n");
    fprintf(stderr, "\n");
    fini( 1 );
}

bool checkForProc(int procNum)
{
    bool    result = false;
    char    cmd[80];
    sprintf(cmd, "ping -n 1 -w 250 90.0.0.%d > .pingtest.tmp", procNum+1);
    system(cmd);

    FILE * fp = fopen(".pingtest.tmp", "r");
    if ( fp != NULL )
    {
        char        buffer[80];
        char *  receivePtr = NULL; 

        while ( !receivePtr &&
                  fgets(buffer, 80, fp) != NULL )
        {
            receivePtr = strstr(buffer, "ECEIVE");
            if ( !receivePtr )  receivePtr = strstr(buffer, "eceive");

            while ( receivePtr &&
                      *receivePtr &&
                      (*receivePtr < '0' || *receivePtr > '9') )
            {
                receivePtr += 1;
            }

            if ( receivePtr && !*receivePtr )
            {
                receivePtr = NULL;
            }
        }

        fclose(fp);
        unlink(".pingtest.tmp");

        if ( !receivePtr )
        {
            fprintf(stderr, "Can't find received count in ping output");
            fini( 1 );
        }

        int receiveCount = atoi(receivePtr);
        if ( receiveCount > 0 )
        {
            result = true;
        }
    }
    else
    {
        fprintf(stderr, "No output from ping command\n");
        fini( 1 );
    }

    return result;
}

int main(int argc, char* argv[])
{
    long tgtbufSize=4194304;
    int procNum = 0;
    int memSize = 12;
    int argIdx = 1;

    if ( !getenv("WIND_UID") )
    {
        putenv("WIND_UID=1");
    }

    while ( argIdx < argc )
    {
        if ( strncmp(argv[argIdx], "-p", 2) == 0 )
        {
            if ( strlen(argv[argIdx]) > 2 )
            {
                procNum = atoi(argv[argIdx]+2);
            }
            else if ( argc > argIdx+1 )
            {
                argIdx += 1;
                procNum = atoi(argv[argIdx]);
            }
            else
            {
                fprintf(stderr,"Error in -p option\n");
                usage();
            }

            if ( procNum < 0 || procNum > 9 )
            {
                usage();
            }
        }
        else if ( strncmp(argv[argIdx], "-m", 2) == 0 )
        {
            if ( strlen(argv[argIdx]) > 2 )
            {
                memSize = atol(argv[argIdx]+2);
            }
            else if ( argc > argIdx+1 )
            {
                argIdx += 1;
                memSize = atol(argv[argIdx]);
            }
            else
            {
                fprintf(stderr,"Error in -m option\n");
                usage();
            }

            if ( memSize < 2 || memSize > 128 )
            {
                usage();
            }
        }
        else if ( strncmp(argv[argIdx], "-v", 2) == 0 )
        {
            char *cp;
            if ( strlen(argv[argIdx]) > 2 )
            {
                cp = argv[argIdx]+2;
            }
            else if ( argc > argIdx+1 )
            {
                argIdx += 1;
                cp = argv[argIdx];
            }

            if ( cp == NULL )
            {
                fprintf(stderr,"Error in -v option\n");
                usage();
            }
            else
            {
                char* fullname= (char *)malloc( strlen(cp)+strlen("\\vxWorks.exe") + 1 ); 
                strcpy( fullname, cp );
                for( cp = fullname; *cp; cp++ )
                {
                    if( *cp == '/' ) *cp = '\\';
                }
                if( --cp > fullname )
                {
                    if( *cp != '\\' ) strcat( cp, "\\" );
                }
                else
                {
                    free( fullname );
                    fprintf(stderr,"Error parsing -v filename\n");
                    usage();
                }
                free( pathname );
                pathname = strdup( fullname );
                strcat( fullname, "vxWorks" );
                if( fileType( fullname ) != ITISAFILE )
                {
                    fprintf( stderr, "\"%s\" not found\n", fullname );
                    usage();
                }
                else
                {
                    strcat( fullname, ".exe" );
                    if( fileType( fullname ) != ITISAFILE ) {
                        fprintf( stderr, "\"%s\" not found\n", fullname );
                        usage();
                    }
                }
            }
        }
        else if ( strncmp(argv[argIdx], "-f", 2) == 0 || strncmp(argv[argIdx], "-R", 2) == 0 )
        {
            char *cp;
            if ( strlen(argv[argIdx]) > 2 )
            {
                cp = argv[argIdx]+2;
            }
            else if ( argc > argIdx+1 )
            {
                argIdx += 1;
                cp = argv[argIdx];
            }

            if ( cp == NULL )
            {
                fprintf(stderr,"Error in -f option\n");
                usage();
            }
            else
            {
                char *dup = strdup(cp);
                for( cp = dup; *cp; cp++ )
                {
                    if( *cp == '/' ) *cp = '\\';
                }
                //
                //
                //   Remove trailing slash if needed
                //
                cp--; if (*cp == '\\') *cp = '\0';
                if( fileType( dup ) != ITISADIRECTORY )
                {
                    fprintf(stderr,"Error: -f does not specify a directory\n");
                    usage();
                }
                free( fspath );
                fspath = dup;
            }
        }
        else if ( strncmp(argv[argIdx], "-b", 2) == 0 )
        {
            long temp;
            if ( strlen(argv[argIdx]) > 2 )
            {
                temp = atol( argv[argIdx]+2 );
            }
            else if ( argc > argIdx+1 )
            {
                argIdx += 1;
                temp = atol( argv[argIdx] );
            }
            if( temp <= 0L) {
                fprintf(stderr,"Error in -b option\n");
                usage();
            } else
                tgtbufSize = temp;

        }
        else
        {
            fprintf(stderr,"Error: invalid argument %s\n",argv[argIdx]);
            usage();  
        }

        argIdx += 1;
    }

    //
    // First ping doesn't always work, so retry twice
    //
    if ( !checkForProc(procNum) &&
          !checkForProc(procNum) )
    {
        static char cmd[256];
        static char cmdPath[256];
        static char path[256];
        static char arg[12][256];

        sprintf(cmd, "%svxWorks.exe", pathname);
        sprintf(arg[0], "/p%d", procNum);
        sprintf(arg[1], "/r%ld", (long)memSize*1024*1024);
        fprintf(stderr, "Launching simulator: %s %s %s\n", cmd, arg[0], arg[1] );
        spawnl(P_NOWAIT, cmd, cmd, arg[0], arg[1], NULL);

        time_t  start = time(NULL);
        while ( time(NULL)-start < 3 )
            ;
        while( !checkForProc(procNum) )
            ;
        strcpy(cmd, "tgtsvr.exe");
        sprintf(arg[0], "90.0.0.%d", procNum+1);
        sprintf(arg[1], "-V");
        sprintf(arg[2], "-m");
        sprintf(arg[3], "%ld", tgtbufSize );
        sprintf(arg[4], "-B");
        sprintf(arg[5], "wdbpipe");
        sprintf(arg[6], "-R");
        if( *fspath )
        {
            strcpy(arg[7], fspath);
        }
        else
        {
            getcwd(path, 256);
            strcpy(arg[7], path);
        }
        sprintf(arg[8], "-RW");
        sprintf(arg[9], "-c");
        if( *pathname )
        {
            sprintf(arg[10], "%svxWorks", pathname);
        }
        else
        {
            getcwd(path, 256);
            sprintf(arg[10], "%s\\vxWorks", path);
        }

        sprintf(cmdPath, "%s\\host\\x86-win32\\bin\\%s", getenv("WIND_BASE"), cmd);
        fprintf(stderr, "Launching target server: %s", cmdPath);
        {
            int j;
            for(j = 0; j < 11; j++ ) fprintf(stderr, " %s", arg[j] );
        }
        spawnl(P_NOWAIT, cmdPath, cmdPath, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9], arg[10], NULL);
    }
    else
    {
        fprintf(stderr, "Simulator for processor number %d is already running\n", procNum);
        fini( 1 );
    }

    return 0;
}

