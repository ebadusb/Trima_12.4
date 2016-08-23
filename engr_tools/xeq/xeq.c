#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <memlib.h>
#include <symLib.h>
#include <sysSymTbl.h>
#include <ioLib.h>
#include <taskVarLib.h>
#include <loadLib.h>
#include <unldLib.h>
#include <errno.h>
#include <usrLib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

static char *WHITESPACE=" \t\v\r\n\f";  /* Documented as the "isspace()" set */
static char *DIRECTORY_DELIMITER="/";
static char *SUFFIX_DELIMITER=".";
static char *ENVIRONMENTALS="_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
static char *ENVIRONMENTAL_PREFIX="$";
static char *ENVIRONMENTAL_ESCAPE="\\";

typedef struct {
 char *privateDEBUG;    /* Debug output flag */
 char *privatePATH;       /* Path for p-forms */
 char *privateEXESUFFIX;  /* Default file suffix */
} PRIVATE_GLOBALS;

static PRIVATE_GLOBALS *pPrivateGlobals;
/* Macros for accessing private globals */
#define DEBUG (pPrivateGlobals->privateDEBUG)
#define PATH  (pPrivateGlobals->privatePATH)
#define EXESUFFIX  (pPrivateGlobals->privateEXESUFFIX)

/* ALlocate an initialize task-private global variables */
static PRIVATE_GLOBALS *SetupPrivateGlobals(int pFlag)
{
    char *cp;

    char *myDEBUG=NULL;   /* Debug output flag will be updated by DEBUG environmental variable, if it exists */
    myDEBUG = getenv("DEBUG");
    if( myDEBUG && ( (strcmp(myDEBUG,"0") == 0) || (strlen(myDEBUG) == 0 )) ) myDEBUG = NULL;

    /* Add the PRIVATE_GLOBALS pointer to task-private variables for this task */
    if ( taskVarAdd (0, (int *)&pPrivateGlobals) != OK)
        {
        int my_errno = errno;
        char ErrorText[_POSIX_SSIZE_MAX]="";
        strerror_r( my_errno, ErrorText );
        my_errno = errno;
        printErr("SetupPrivateGlobals can't taskVarAdd pPrivateGlobals, errno = %d (%s)\n", my_errno, ErrorText );
        return( NULL );
        }

    /* Allocate a task-local copy of the PRIVATE_GLOBALS structure */
    if ((pPrivateGlobals = (PRIVATE_GLOBALS *) memalign (sizeof(char *), sizeof (PRIVATE_GLOBALS))) == NULL)
        {
        int my_errno = errno;
        char ErrorText[_POSIX_SSIZE_MAX]="";
        strerror_r( my_errno, ErrorText );
        printErr("SetupPrivateGlobals can't allocate PRIVATE_GLOBALS structure, errno = %d (%s)\n", my_errno, ErrorText );
        return( NULL );
        }

    /* Set up the global DEBUG output flag */
    DEBUG = myDEBUG;   /* Copy local value of DEBUG flag to global */

    /* Path for p-forms will be setup by PATH environmental variable, if it exists */
    if( pFlag && (cp = getenv("PATH")) )
        PATH = cp;
    else
        PATH="";

    /* Default file suffix will be setup by EXESUFFIX environmental variable, if it exists */
    if( (cp = getenv("EXESUFFIX")) )
        EXESUFFIX = cp;
    else
        EXESUFFIX=".o";  

    /* Display the values, if DEBUG flag set */
    if( DEBUG )
    {
        fprintf( stdout,  "DEBUG = \"%s\"\n", DEBUG ); 
        fprintf( stdout,  "PATH = \"%s\"\n", PATH );
        fprintf( stdout,  "EXESUFFIX = \"%s\"\n", EXESUFFIX );
    }

    return( pPrivateGlobals );
}

/* Delete Private Globals */
static void RemovePrivateGlobals()
{
    if( pPrivateGlobals )
    {
        int retval;
        char *myDEBUG=DEBUG;
        retval = cfree( (char *)pPrivateGlobals );
        if( myDEBUG ) fprintf( stdout, "Return from cfree(pPrivateGlobals) = %d\n", retval );
        taskVarDelete (0, (int *)&pPrivateGlobals);
    }
    return;
}


/* In place, trim whitespace off the beginning & end of a string */
static char *trim( char **string )
{
    char *s = *string;
    int j;

    while( isspace( (int)(*s) ) ) { s++; }  /* Eliminate leading whitespace */

    for( j=(strlen(s)-1); j >= 0 && isspace( (int)(s[j]) ); j-- );  s[j+1] = '\0'; /* Eliminate trailing whitespace */

    *string = s;

    return s;
}

/* Copy in string to out string, substituting ENVIRONMENTAL VARIABLES into out string;
   in and out may point to the same buffer. */
static int subenv( char *out, char *in, size_t max )
{
    size_t paramlen;
    
    int retval = 0;
    int preceding = 1;
    size_t roomleft=max;
    char *ParameterPointer = in;
    char OutBuffer[max+1];

    OutBuffer[0] = '\0';

    if( DEBUG ) fprintf( stdout,  "subenv: input string = \"%s\"\n", in );

    /* Loop until either there is no room left in the caller's buffer,
       or the input string is exhausted. */
    while( roomleft > 0 && *ParameterPointer != '\0' )
    {
        /* Find the first/next ENVIRONMENTAL_PREFIX character in the input string */
        if( DEBUG ) fprintf( stdout,  "subenv: ENVIRONMENTAL_PREFIX = \"%s\"\n", ENVIRONMENTAL_PREFIX );
        if( DEBUG ) fprintf( stdout,  "subenv: ParameterPointer = 0x%X\n", (unsigned)ParameterPointer );
        if( DEBUG ) fprintf( stdout,  "subenv: *ParameterPointer = \"%s\"\n", ParameterPointer );
        if( ( paramlen = strcspn(ParameterPointer,ENVIRONMENTAL_PREFIX) ) > 0 )
        {
            size_t len;
            int index;
            /* Include all escaped ENVIRONMENTAL_PREFIX's (i.e. "\$") as part of the parameter;
               Get to the "real" ENVIRONMENTAL_PREFIX character, if any, or end-of-string. */
            while( strchr( ENVIRONMENTAL_ESCAPE, *( ParameterPointer + paramlen -1 ) ) &&
                   *( ParameterPointer + paramlen ) != '\0' )
            {
                if( DEBUG ) fprintf( stdout,  "subenv: including escaped %s's as part of parameter\n", ENVIRONMENTAL_PREFIX );
                paramlen++;
                paramlen += strcspn(ParameterPointer + paramlen,ENVIRONMENTAL_PREFIX);
            }
            if( preceding )  /* Is this token a case of the first "real" ENVIRONMENTAL_PREFIX character
                                 preceded by other characters (e.g. "abc$XYZ")? */
            {
                /* Yes, just copy the string contents (e.g. "abc") preceding the first ENVIRONMENTAL_PREFIX character */
                index = 0;
                preceding = 0;
            }
            else
            {
                char *envptr=NULL;
                /* Compute the length of the prospective ENVIRONMENTAL variable */
                if( ( len = strspn(ParameterPointer, ENVIRONMENTALS) ) > 0 )
                {
                    /* There is a prospective ENVIRONMENTAL variable, look it up, longest first */
                    char substring[len+1];
                    strncpy(substring,ParameterPointer,len) ;
                    for( envptr=NULL, index=(int)len; index>0; index-- )
                    {
                        substring[index] = '\0';
                        if( DEBUG ) fprintf( stdout,  "subenv: substring = \"%s\"\n", substring );
                        if( ( envptr = getenv( substring ) ) ) break;
                    }
                }
                if( envptr != NULL )
                {
                    /* Found an ENVIRONMENTAL variable, substitute it */
                    if( DEBUG ) fprintf( stdout,  "subenv: *envptr = \"%s\"\n", envptr );
                    len = strlen( envptr );
                    if( len > roomleft ) len = roomleft;
                    if( len )
                    {
                        strncat( OutBuffer, envptr, len );
                        roomleft -= len;
                    }
                    retval = (roomleft > 0);  /* Tell caller to rescan, unless the output is full */
                }
                else
                {   /* Either couldn't find the ENVIRONMENTAL variable,
                       or there was an ENVIRONMENTAL_PREFIX character followed by non-ENVIRONMENTALS */
                    index = -1;  /* Back up, so that the code below will copy the prefix character */
                }
            }
            /* Append (the rest of) the parameter */
            len=paramlen-index;
            if( len > roomleft ) len = roomleft;
            if( len ) strncat( OutBuffer, ParameterPointer+index, len  );
            roomleft -= len;
            ParameterPointer += paramlen;   /* Point to the next ENVIRONMENTAL_PREFIX
                                               or end-of-string character. */
        }
        else
        {   /* Handle the ENVIRONMENTAL_PREFIX character itself; check the next character
              in the string for being another ENVIRONMENTAL_PREFIX character */
            if( strchr(ENVIRONMENTAL_PREFIX,*(ParameterPointer+1)) )
            {
                if( roomleft >= 1 )
                {
                    strncat( OutBuffer, ParameterPointer, 1 );
                    roomleft--;
                }
            }
            else  /* Next character is not another ENVIRONMENTAL_PREFIX character;
                     clear the preceding character string flag.  If there were preceding
                     characters, they should have already been handled */
            {
                preceding = 0;
            }
            ParameterPointer++;  /* Point to the next character of whatever kind */
        }
    }
    strcpy( out, OutBuffer );   /* DELIVER OUTPUT STRING TO CALLER */
    if( DEBUG ) fprintf( stdout,  "subenv: output string = \"%s\", return = %d\n", out, retval );
    return( retval );   /* 1 -> rescan needed, 0 -> no rescan needed */
}


/* Allocate & populate the arg vector and arg strings arrays in contiguous heap storage */ 
static char** SetupArgv( char *BufferPointer,  size_t TotalArgsSize, int argc )
{
    char **argv=NULL;

    if( argc > 0 )
    {
        int j, my_errno;
        char *ArgsPointer;
        char *ParameterPointer;

        /* Allocate the argv and arg strings arrays in heap storage */
        if( DEBUG ) fprintf( stdout,  "Allocating %d heap bytes\n", (argc+1)*sizeof(char *) + TotalArgsSize ); 
        argv= (char **) memalign( sizeof(char *), (argc+1)*sizeof(char *) + TotalArgsSize );
        if( !argv )
        {
            char ErrorText[_POSIX_SSIZE_MAX]="";
            my_errno = errno;
            strerror_r( my_errno, ErrorText );
            printErr("Could not allocate parameter and argv array; count = %d; errno = %d (%s)\n",
                      argc, my_errno, ErrorText );
            return( NULL );
        }
        ArgsPointer = (char *)(argv + (argc + 1));
        if( DEBUG ) fprintf( stdout,  "argv array @ 0x%X, arg strings @ 0x%X\n", (unsigned)argv, (unsigned)ArgsPointer );

        /* Populate the argv and arg strings arrays in heap storage */
        for( j = 0, ParameterPointer = BufferPointer;
             j < argc;
             ParameterPointer += (strlen(ParameterPointer) +1) )
        {
          /* Skip to the next non-null parameter */
          while( *ParameterPointer == '\0' ) ParameterPointer++;
          strcpy( ArgsPointer, ParameterPointer );
          argv[ j++ ] = ArgsPointer;
          ArgsPointer += (strlen(ParameterPointer) +1);
        }
        argv[j] = NULL;
    }

    return( argv );
}

/* Deallocate the arg vector and arg strings arrays in contiguous heap storage */
void RemoveArgv( char** argv )
{
    if( argv )
    {
        int retval;
        retval = cfree( (char *)argv );
        if( DEBUG ) fprintf( stdout, "Return from cfree(argv) = %d\n", retval );
    }
}


static void Usage( FILE *fp )
{
    fprintf( fp, "\n" );
    fprintf( fp, "                        x      x     eeee        qqqq  qq\n" );
    fprintf( fp, "                         x    x     e    e      q    q q\n" );
    fprintf( fp, "                          x  x     e      e    q      qq\n" );
    fprintf( fp, "                           xx      eeeeeeee   q        q\n" );
    fprintf( fp, "                           xx      e          q        q\n" );
    fprintf( fp, "                          x  x     e      e    q      qq\n" );
    fprintf( fp, "                         x    x     e    e      q    q q\n" );
    fprintf( fp, "                        x      x     eeee        qqqq  q\n" );
    fprintf( fp, "                                                       q\n" );
    fprintf( fp, "                                                       q\n" );
    fprintf( fp, "                                                       q\n" );
    fprintf( fp, "                                                       qq\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "Usage: Load and execute a program needing an argc/argv, like a unix main().\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "Caveat:For reasons which are not evident, xeq exhibits non-reentrant behavior\n" );
    fprintf( fp, "       (GPFs) when several instances run under SIMNTgnu trying to execute the\n" );
    fprintf( fp, "       same target program.  This caveat may be lifted if and when the under-\n" );
    fprintf( fp, "       lying cause is discovered.\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       There are four forms of xeq, modeled after posix exec(), but without\n" );
    fprintf( fp, "       an \"e\" form, as follows:\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       xeql( char *arg0, char *arg1, ..., char *arg9 )\n" );
    fprintf( fp, "       xeqlp( char *arg0, char *arg1, ..., char *arg9 )\n" );
    fprintf( fp, "       xeqv( char *argstring )\n" );
    fprintf( fp, "       xeqvp( char *argstring )\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       where:\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       1. The \"l\" forms of xeq contain an argument list, one command\n" );
    fprintf( fp, "          parameter per each of the ten possible function arguments.\n" );
    fprintf( fp, "       2. The \"v\" forms of xeq contain a pointer to a string\n" );
    fprintf( fp, "          in which the command parameters (arg0 ...) are separated by\n" );
    fprintf( fp, "          whitespace.\n" );
    fprintf( fp, "       3. For the \"p\" forms of xeq, arg0 must contain the file name\n" );
    fprintf( fp, "          portion of a path to an executable module.  If no directory\n" );
    fprintf( fp, "          delimiter (/) is included in arg0, the contents of the\n" );
    fprintf( fp, "          \"PATH\" environment variable are prefixed to arg0.  If a\n" );
    fprintf( fp, "          directory delimiter (/) is in arg0, then it is treated\n" );
    fprintf( fp, "          as in 4, below.\n" );
    fprintf( fp, "       4. For the non-\"p\" forms of xeq, arg0 must contain the fully-\n" );
    fprintf( fp, "          qualified pathname of an executable module.\n" );
    fprintf( fp, "       5. For both the \"p\" and non-\"p\" forms of xeq, if no filename\n" );
    fprintf( fp, "          suffix delimiter (.) is included in arg0, the contents of the\n" );
    fprintf( fp, "          \"EXESUFFIX\" environment variable are suffixed to arg0.  If a\n" );
    fprintf( fp, "          filename suffix delimiter (.) is in arg0, then it is assumed\n" );
    fprintf( fp, "          that suffixing is unnecessary.\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       In addition to \"PATH\", above, all forms of xeq can use two other\n" );
    fprintf( fp, "       environment variables: \"EXESUFFIX\" and \"DEBUG\".  The former is\n" );
    fprintf( fp, "       used as an extension on the file name of the executable module, if\n" );
    fprintf( fp, "       none is contained within arg0, and must contain the suffix delimiter\n" );
    fprintf( fp, "       appropriate for the filesystem.  The latter controls the output of\n" );
    fprintf( fp, "       debug information from xeq.  DEBUG=0 implies don't output; 1, do\n" );
    fprintf( fp, "       output.  These environment variables may be defined, in either the\n" );
    fprintf( fp, "       Host or Target Shell, as follows:\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "          putenv( \"PATH=/tgtsvr\" )\n" );
    fprintf( fp, "          putenv( \"EXESUFFIX=.o\" )\n" );
    fprintf( fp, "          putenv( \"DEBUG=0\" )\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       The values shown above are the defaults that xeq assumes in the\n" );
    fprintf( fp, "       absence of a setting for the environment variable.\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       Any of the args can contain references to environment variables in the\n" );
    fprintf( fp, "       simplest form: $name.  The parenthized version of referencing environment\n" );
    fprintf( fp, "       variables (e.g. $(PATH) ) is not supported.  However, embedded variables\n" );
    fprintf( fp, "       are supported.  Embedded variables come about when one variable references\n" );
    fprintf( fp, "       another, e.g. putenv(\"EXECUTABLES=$PATH\").\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       There are two formats that the arg0 parameter can take.  The first is\n" );
    fprintf( fp, "       a simple program name, where the program name == file name == entry\n" );
    fprintf( fp, "       point name.  In the second format, program name == file name, but the\n" );
    fprintf( fp, "       entry point name is separately specified, following a greater-than (>)\n" );
    fprintf( fp, "       character.  For example, to load the file test.o and execute the entry\n" );
    fprintf( fp, "       point \"cat\" use the format \"test>cat\".\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       The following examples call the entry point \"test\" in program file test.o\n" );
    fprintf( fp, "       in the directory /tgtsvr with parameters \"a\" and \"$DEBUG\" (for which the\n" );
    fprintf( fp, "       value of the DEBUG environment variable is substituted).  They are all\n" );
    fprintf( fp, "       functionally equivalent, assuming the environment variable settings above:\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "           xeqlp \"test\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeqlp \"test>test\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeqlp \"test.o\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeqlp \"test.o>test\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeql \"/tgtsvr/test\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeql \"/tgtsvr/test>test\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeql \"/tgtsvr/test.o\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeql \"/tgtsvr/test.o>test\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeql \"$PATH/test\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeql \"$PATH/test>test\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeql \"$PATH/test.o\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeql \"$PATH/test.o>test\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeql \"$EXECUTABLES/test\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeql \"$EXECUTABLES/test>test\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeql \"$EXECUTABLES/test.o\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeql \"$EXECUTABLES/test.o>test\",\"a\",\"$DEBUG\"\n" );
    fprintf( fp, "           xeqvp \"test a $DEBUG\"\n" );
    fprintf( fp, "           xeqvp \"test>test a $DEBUG\"\n" );
    fprintf( fp, "           xeqvp \"test.o a $DEBUG\"\n" );
    fprintf( fp, "           xeqvp \"test.o>test a $DEBUG\"\n" );
    fprintf( fp, "           xeqv \"/tgtsvr/test a $DEBUG\"\n" );
    fprintf( fp, "           xeqv \"/tgtsvr/test>test a $DEBUG\"\n" );
    fprintf( fp, "           xeqv \"/tgtsvr/test.o a $DEBUG\"\n" );
    fprintf( fp, "           xeqv \"/tgtsvr/test.o>test a $DEBUG\"\n" );
    fprintf( fp, "           xeqv \"$PATH/test a $DEBUG\"\n" );
    fprintf( fp, "           xeqv \"$PATH/test>test a $DEBUG\"\n" );
    fprintf( fp, "           xeqv \"$PATH/test.o a $DEBUG\"\n" );
    fprintf( fp, "           xeqv \"$PATH/test.o>test a $DEBUG\"\n" );
    fprintf( fp, "           xeqv \"$EXECUTABLES/test a $DEBUG\"\n" );
    fprintf( fp, "           xeqv \"$EXECUTABLES/test>test a $DEBUG\"\n" );
    fprintf( fp, "           xeqv \"$EXECUTABLES/test.o a $DEBUG\"\n" );
    fprintf( fp, "           xeqv \"$EXECUTABLES/test.o>test a $DEBUG\"\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       These examples will produce output similar to the following.  What the\n" );
    fprintf( fp, "       program displays for argv[0] depends upon the \"arg0 format\" (see above)\n" );
    fprintf( fp, "       and the fullness of the specification for the program file.  Note that the\n" );
    fprintf( fp, "       environment variables are substituted into the args before they are given\n" );
    fprintf( fp, "       to the program:\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "           argv[0] = \"/tgtsvr/test.o\"\n" );
    fprintf( fp, "           argv[1] = \"a\"\n" );
    fprintf( fp, "           argv[2] = \"0\"\n" );
    fprintf( fp, "           value = 0 = 0x0\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       for non-\">\" formats, or\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "           argv[0] = \"/tgtsvr/test.o>test\"\n" );
    fprintf( fp, "           argv[1] = \"a\"\n" );
    fprintf( fp, "           argv[2] = \"0\"\n" );
    fprintf( fp, "           value = 0 = 0x0\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       for \">\" formats.\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       The following is the C++ program used to produce the output:\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "           #include <stdio.h>\n" );
    fprintf( fp, "           \n" );
    fprintf( fp, "           #ifdef __cplusplus \n" );
    fprintf( fp, "           extern \"C\" void test ( int argc, char** argv); \n" );
    fprintf( fp, "           #else \n" );
    fprintf( fp, "           void test ( int argc, char** argv); \n" );
    fprintf( fp, "           #endif\n" );
    fprintf( fp, "           \n" );
    fprintf( fp, "           void test( int argc, char** argv)\n" );
    fprintf( fp, "           {\n" );
    fprintf( fp, "               for( int j=0; j<argc; j++ )\n" );
    fprintf( fp, "               {\n" );
    fprintf( fp, "                   printf( \"argv[%%d] = \\\"%%s\\\"\\n\", j, argv[j] );\n" );
    fprintf( fp, "               }\n" );
    fprintf( fp, "           \n" );
    fprintf( fp, "           }\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "       This program also illustrates what must be done to suppress C++ \"name\n" );
    fprintf( fp, "       mangling\" under the Tornado GNU C compiler.  This is necessary so that\n" );
    fprintf( fp, "       xeq can locate the entry point name.  One could use the \">\" arg0 format\n" );
    fprintf( fp, "       to feed the \"mangled\" name to xeq, if using the extern in the program's\n" );
    fprintf( fp, "       source file is objectionable for any reason.\n" );


    return;
}


static int xeq( int argc, char **argv )
{
    int j;
    int FileDescriptor;
    int my_errno;
    char *cp;  /* General Purpose Character pointer */
    char FileSpecifier[256],EntryName[256];
    FUNCPTR EntryPoint;  /* where to put symbol value */
    SYM_TYPE  Type;    /* where to put symbol type */
    STATUS status=ERROR;
    MODULE_ID ModuleID;

    if( DEBUG ) { /* Display Command Args */
        for( j=0; j<argc; j++ ) fprintf( stdout,  "argv[%d] = \"%s\"\n", j, argv[j] );
    }

    /* IF NO COMMAND ARGS, JUST DISPLAY MAN PAGE */
    if( argc == 0 )
    {
        Usage(stderr);
        if( fileno(stdout) != fileno(stderr) ) Usage(stdout);
        return ( 0 );
    }

    /* PARSE THE MODULE FILE > ENTRY NAME PARAMETER FROM THE FIRST ARG */
    if( argv && argv[0][0] != '\0' )
    {
        char ParseBuffer[256];
        char *ParameterPointer;
        char *Next=NULL;

        ParameterPointer = trim(&argv[0]); /* Trim whitespace from argv[0] */
        strcpy( ParseBuffer, ParameterPointer );
        strcpy( EntryName, "_" );
        if( (ParameterPointer = strtok_r( ParseBuffer, ">", &Next )) == NULL )
        {
            printErr( "No module file to load on command line\n" );
            my_errno = EINVAL;
            return( my_errno );
        }
        else
        {
            char *startname, *endname;

            /* Set file prefix and suffix to environmental defaults */
            char *prefix=PATH;
            char *suffix=EXESUFFIX;

            /* Parse File Specifier */
            ParameterPointer = trim( &ParameterPointer );  /* Trim whitespace */
            if( strlen( ParameterPointer ) == 0 )
            {
                printErr( "Module file using \">\" syntax with no file name\n" );
                my_errno = EINVAL;
                return( my_errno );
            }
            startname = ParameterPointer;
            endname=(ParameterPointer+(strlen(ParameterPointer)-1));
            /* Test for a Suffix Delimiter in the name section of the FileSpecifier path */
            for(cp=endname; cp >= startname; cp-- )
            {
                /* Test for a directory delimiter */
                if (strchr(DIRECTORY_DELIMITER,(int)*cp) )
                {
                    prefix = NULL;  /* There is already a directory delimiter, null out prefix */
                    startname = cp+1;  /* Save pointer to start of File Name */
                    break; /* If a prefix delimiter is encountered before a suffix delimiter, there is no suffix already */
                }
                /* Test for a file suffix delimiter */
                if (strchr(SUFFIX_DELIMITER,(int)*cp) )
                {
                    suffix = NULL;  /* There is already a suffix delimiter, null out suffix */
                    endname = cp-1;  /* Save pointer to end of File Name */
                }
            }
            /* Now, form full File Specifier.
               1. Prefix with PATH, if necessary */
            if( prefix )
            {
                strcpy( FileSpecifier, prefix );
                if( !strchr(DIRECTORY_DELIMITER, (int)prefix[strlen(prefix)-1]) )
                    strncat( FileSpecifier, DIRECTORY_DELIMITER, 1 );
            }
            else
            {
                strcpy( FileSpecifier, "" );
            }
            /* 2. Put in middle part of file spec */
            strcat( FileSpecifier, ParameterPointer );
            /* 3. Suffix with EXESUFFIX, if necessary */
            if( suffix ) strcat( FileSpecifier, suffix );
            if( DEBUG ) fprintf( stdout, "Full File Specifier = \"%s\"\n", FileSpecifier );

            /* Parse the Entry Point Name */
            if( (ParameterPointer = strtok_r( NULL, ">", &Next )) == NULL )
            {
                /* No specified Entry Point Name, derive it from the file spec */
                strncat( EntryName, startname, endname-startname+1 );
                if( DEBUG ) fprintf( stdout, "Derived Entry Point Name = \"%s\"\n", EntryName );
            }
            else
            {
                /* Entry Point Name is Specified, process it */
                ParameterPointer = trim( &ParameterPointer );  /* Trim whitespace */
                if( strlen( ParameterPointer ) == 0 )
                {
                    printErr( "Module file using \">\" syntax with no entry point name\n" );
                    my_errno = EINVAL;
                    return( my_errno );
                }
                strcat( EntryName, ParameterPointer );
                if( DEBUG ) fprintf( stdout, "Specified Entry Point Name = \"%s\"\n", EntryName );
            }
        }
    }
    else
    {
        printErr( "No module file to load on command line\n" );
        my_errno = EINVAL;
        return( my_errno );
    }

    /* DOWNLOAD THE MODULE FILE */
    FileDescriptor = open ( FileSpecifier, O_RDONLY, 0644 );
    my_errno = errno;
    if( DEBUG ) fprintf( stdout, "open: File Descriptor = %d, errno = %d\n", FileDescriptor, my_errno );
    if( FileDescriptor >= 0 )
    {
        ModuleID = loadModule (FileDescriptor, LOAD_ALL_SYMBOLS );
        my_errno = errno;
        if( ModuleID != NULL )
        {
            if( DEBUG ) fprintf( stdout,  "Module ID = 0x%X\n", (unsigned)ModuleID ); 
        }
        else
        {
            char ErrorText[_POSIX_SSIZE_MAX]="";
            strerror_r( my_errno, ErrorText );
            printErr( "Unable to load \"%s\"\n", FileSpecifier );
            printErr("Could not read, not enough memory, or format is illegal; errno = %d (%s)\n", my_errno, ErrorText );
            close (FileDescriptor);
            return (my_errno);
        }
    }
    else
    {
        char ErrorText[_POSIX_SSIZE_MAX]="";
        strerror_r( my_errno, ErrorText );
        printErr( "Could not open \"%s\"; errno = %d (%s)\n", FileSpecifier, my_errno, ErrorText );
        return (my_errno);
    }
    close (FileDescriptor);

    /* LOOK UP ENTRY POINT */
    status = symFindByName (
                       sysSymTbl, /* ID of symbol table to look in */
                       EntryName,     /* symbol name to look for */
                       (char **)&EntryPoint,  /* where to put symbol value */
                       &Type     /* where to put symbol type */
                      );
    my_errno = errno;
    if( DEBUG )
       fprintf( stdout,  "Status = %s, Type = 0x%X, %s = 0x%X\n", status==OK ? "OK" : "ERROR", Type, EntryName, (int)EntryPoint );

    if( status != OK )
    {
        char ErrorText[_POSIX_SSIZE_MAX]="";
        strerror_r( my_errno, ErrorText );
        printErr("Unable to locate entry point \"%s\", errno = %d (%s)\n", EntryName, my_errno, ErrorText );
        return (my_errno);
    }


    /* RUN THE PROGRAM */
    (*EntryPoint)( argc, argv );
    my_errno = errno;
    if( DEBUG )
        fprintf( stdout, "Return from %s()\n", EntryName );


    /* UNLOAD THE MODULE, IF NECESSARY */
    if( ModuleID != NULL )
    {
        status = unldByModuleId( ModuleID, 0 );
        if( status != OK )
        {
            char ErrorText[_POSIX_SSIZE_MAX]="";
            my_errno = errno;
            strerror_r( my_errno, ErrorText );
            printErr("Unable to unload module 0x%X, errno = %d (%s)\n", ModuleID, my_errno, ErrorText );
            return (my_errno);
        }
    }

    return (my_errno);
}

/* Perform the xeql form */
static int do_xeql( int pform, char *a, va_list which )
{

    if( SetupPrivateGlobals( pform ) )
    {

        char ParseBuffer[4096];
        int retval;
        size_t len;
    
        int argc=0;
        char **argv=NULL;
        char *ParameterPointer=a;
        char *BufferPointer = ParseBuffer;
        size_t TotalArgsSize=0;
    
        /* Count parameters */
        while ( ParameterPointer != NULL  )
        {
            strcpy( BufferPointer, ParameterPointer );
            /* Substitute Environmental Variables */
            while( subenv( BufferPointer, BufferPointer, sizeof(ParseBuffer) -1 -(BufferPointer - ParseBuffer) ) );
            len = (strlen(BufferPointer) +1);
            argc++;
            TotalArgsSize += len;
            BufferPointer += len;
            ParameterPointer = va_arg( which, char * );
        }
    
        /* Allocate the arg vector array in heap storage */
        argv = SetupArgv( ParseBuffer, TotalArgsSize, argc );

        retval = ( xeq( argc, argv ) );

        RemoveArgv( argv );

        RemovePrivateGlobals();

        return( retval );
    }
    else
        return( -1);
}

/* Perform the xeqv form */
static int do_xeqv( int pform, char *argstring )
{
    if( SetupPrivateGlobals( pform ) )
    {
        char ParseBuffer[4096];
        size_t len;
        char *TokenPointer;
        int retval;

        char TokenBuffer[strlen(argstring) +1];
        
        int argc=0;
        char **argv=NULL;
        size_t TotalArgsSize=0;
        char *BufferPointer=ParseBuffer;
        char *NextToken=NULL;

        strcpy( TokenBuffer, argstring );
        len = strlen( TokenBuffer );

        /* Count parameters */
        if( len > 0 ) 
        {
            for( TokenPointer = strtok_r( TokenBuffer, WHITESPACE, &NextToken );
                 (TokenPointer != NULL);
                 TokenPointer = strtok_r( NULL, WHITESPACE, &NextToken ) )
            {
              /* Skip to the next non-null parameter */
              while( TokenPointer != NULL && *TokenPointer == '\0' ) TokenPointer = strtok_r( NULL, WHITESPACE, &NextToken );
              if( TokenPointer != NULL )
              {
                  strcpy( BufferPointer, TokenPointer );
                  /* Substitute Environmental Variables */
                  while( subenv( BufferPointer, BufferPointer, sizeof(ParseBuffer) -1 -(BufferPointer - ParseBuffer) ) );
                  len = (strlen(BufferPointer) +1);
                  argc++;
                  TotalArgsSize += len;
                  BufferPointer += len;
              }
            }
        }

        argv = SetupArgv( ParseBuffer, TotalArgsSize, argc );

        retval = ( xeq( argc, argv ) );

        RemoveArgv( argv );

        RemovePrivateGlobals();

        return( retval );
    }
    else
        return( -1);
}


/*************************/
/* Callable entry points */
/*************************/

int xeql( char *a, ... )
{
    int retval;
    va_list which;

    va_start( which, a );
    retval = do_xeql( 0, a, which );
    va_end( which );

    return( retval );
}

int xeqlp( char *a, ... )
{
    int retval;
    va_list which;

    va_start( which, a );
    retval = do_xeql( 1, a, which );
    va_end( which );

    return( retval );
}

int xeqv( char *argstring )
{
    int retval;

    retval = do_xeqv( 0, argstring );

    return( retval );
}

int xeqvp( char *argstring )
{
    int retval;

    retval = do_xeqv( 1, argstring );

    return( retval );
}
