// FILENAME:  bg.cpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/bg/rcs/bg.cpp 1.1 2002/11/19 22:11:04 td07711 Exp td07711 $
// PURPOSE:  spawns given function as a background task
//   BASIC USAGE - the user specifies the following keyword value command line options:
//     bg <function_name>,[options]
//
//
// CHANGELOG:
//   $Log: bg.cpp $
//   Revision 1.1  2002/11/19 22:11:04  td07711
//   Initial revision


// vxworks includes
#include <vxWorks.h>
#include <symLib.h>
#include <usrLib.h>
#include <errnoLib.h>

// standard includes
#include <stdio.h>

// common includes
#include "optionparser.h"


// libtools includes
#include "logger.hpp"
 

int bg( FUNCPTR funcEntry, const char* args )
{
    //
    // following args to taskSpawn are set using env variables.
    // if not set, args are defaulted as follows...
    //
    int taskPriority = 100;
    int taskOptions = 0;
    int taskStackSize = 100000;

    char* priorityString = getenv("BG_PRIORITY");
    if( priorityString )
    {
        if( sscanf( priorityString, "%d", &taskPriority ) != 1 )
        {
            printf("bg: failed to parse BG_PRIORITY=%s\n", 
                   priorityString );
            return 0;
        }
        if( taskPriority < 1 || taskPriority > 200 )
        {
            printf("bg: BG_PRIORITY=%s out of range 1-200\n",
                   priorityString );
            return 0;
        }
    }

    char* taskStackSizeString = getenv("BG_STACKSIZE");
    if( taskStackSizeString )
    {
        if( sscanf( taskStackSizeString, "%d", &taskStackSize ) != 1 )
        {
            printf("bg: failed to parse BG_STACKSIZE=%s\n", 
                   taskStackSizeString );
            return 0;
        }
        if( taskStackSize < 1 || taskPriority > 10000000 )
        {
            printf("bg: BG_STACKSIZE=%s out of range 1-10000000\n",
                   taskStackSizeString );
            return 0;
        }
    }
    

    int rval = taskSpawn( 0, taskPriority, taskOptions, taskStackSize, funcEntry, 
                         (int)args, 0, 0, 0, 0,
                          0, 0, 0, 0, 0 );

    if( rval == ERROR )
    {
        printf("bg: taskSpawn failed errno=%d\n", errno);
        printErrno( errnoGet() );
    }
    else
    {
        printf("bg: taskSpawn succeeded, taskID = 0x%x, priority=%d, stacksize=%d\n", 
               rval, taskPriority, taskStackSize );
    }

    return rval;
}
