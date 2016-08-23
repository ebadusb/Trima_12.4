/*****************************************************
 * FILENAME: fatalerror.cpp
 * PURPOSE: utility that test the fatal error function from routerf.lib
 * USAGE: fatalerror confirm
 * CHANGELOG:
 *   $Header: E:/BCT_Development/vxWorks/Trima/engr_tools/fatalerror/rcs/fatalerror.cpp 6.1 2002/05/06 21:13:26Z js70375 Exp js70375 $
 *   $Log: fatalerror.cpp $
 *   Revision 6.1  2002/05/06 21:13:26Z  js70375
 *   strimbu - initial revision for vxworks compatibility 
 *   Revision 1.1  2002/05/06 21:12:34Z  js70375
 *   Initial revision
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>


//#include "error.h"
//#include "debug.h"

void usage(void);
vector<char *> parse(char*);
void fatalerror_main (char *);

void fatalerror_main(char* args)
{
	//debug_scan_argv(&argc, *args); // debug setup

	vector<char *> arguments = parse(args);
	
	int argc = arguments.size();

	// verify correct usage
    if (argc != 1 || strcmp(arguments[0], "confirm")) {
        usage();
    }

    //LOG_DEBUG("testing LOG_DEBUG");
	fprintf(stderr, "testing LOG_DEBUG");
    //VA_INFO("%s VA_INFO", "testing");
	fprintf(stderr, "%s VA_INFO", "testing");
    //VA_ERROR("%s %s %s", "just", "testing", "VA_ERROR");
	fprintf(stderr, "%s %s %s", "just", "testing", "VA_ERROR");
    //VA_FATAL("testing %s", "VA_FATAL");
	fprintf(stderr, "testing %s", "VA_FATAL");


    // abort should prevent us from getting here
    //VA_ERROR("abort failed, should not have gotten here");
	fprintf(stderr, "abort failed, shold not have gotten here");
    
}

vector<char *> parse(char* args)
{
	char* token = NULL;
	vector<char *> arguments;

	token = strtok(args, " ");
	arguments.push_back(token);
    while(token != NULL)
	{
		token = strtok(NULL, " ");
		if(token != NULL)
			arguments.push_back(token);

    } 
	return arguments;
}


void usage(void)
{
    fprintf(stderr, "USAGE: fatalerror confirm\n");
    exit(1);
}

