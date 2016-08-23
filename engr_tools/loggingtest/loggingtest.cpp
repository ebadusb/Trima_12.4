/*****************************************************
 * FILENAME: loggingtest.cpp
 * PURPOSE: utility to verify Trima logging bandwitdt to tracelog
 * USAGE: loggingtest <num msgs> [num_secs]
 *   default is to log all msgs at once.  If num_secs is supplied then
 *   log num_msgs/num_secs, sleep 1 second, ... until done
 *   Uses VA_INFO from routerf.lib to do the logging
 * CHANGELOG:
 *   $Header: E:/BCT_Development/Trima5.R/Trima/engr_tools/loggingtest/rcs/loggingtest.cpp 6.3 2002/05/30 20:19:31Z js70375 Exp dslausb $
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

#include "platform.h"


const char* ProgramName = "loggingtest";

static const char* Usage = "USAGE: loggingtest <num log msgs> [time window]\n"
						   "<num log msgs> - number of log msgs to generate\n"
						   "[time window] - time in seconds within which msgs will be logged, default=1\n";
vector<char *> parse(char*);
void usage(void);
int loggingtest_main(char *);

int loggingtest_main(char *args)
{
    int num_msgs = 0;  //argument 1 number of messages to log
    int num_secs = 1;  //argument 2 time window in which to log the messages
    int sec 	 = 0;
    int msg_num	 = 0;

	vector<char *> arguments = parse(args);

	int arg_size = arguments.size();

    num_msgs = atoi(arguments[0]);
	num_secs = atoi(arguments[1]);


    // verify correct usage
    if (arg_size < 2 || arg_size > 3 ) {
        fprintf(stderr, "ERROR %s: invalid arg count\n", ProgramName);
        usage();
    }
 
    if (arg_size > 2 && num_secs != 1) {
        fprintf(stderr, "ERROR %s: scanf failed to parse [time window]=%s\n",
                ProgramName, arguments[1]);
        usage();
    }

    if (num_msgs < 0) {
        fprintf(stderr, "ERROR %s: num log msgs = %d, must be > 0\n",
                ProgramName, num_msgs);
        usage();
    }
    if (num_secs < 0) {
        fprintf(stderr, "ERROR %s: time window = %d, must be > 0\n",
                ProgramName, num_secs);
        usage();
    }

    // calculate how many msgs to log at once, must be > 0.
    int n = num_msgs/num_secs;

    if (n <= 0) {
        fprintf(stderr, "ERROR %s: msgs per second must be > 0, try again\n",
                ProgramName);
        exit(1);
    }

    // log the msgs
    for (msg_num = 1, sec = 0; msg_num <= num_msgs; sec++) {
		for (int i = 0; i < n ; i++) {
			 fprintf(stderr, "VA_INFO (msg_num=%d seconds=%d msgs_left=%d",
                msg_num++, sec, num_msgs - msg_num);
            //VA_INFO("msg_num=%d seconds=%d msgs_left=%d",
             //       msg_num++, sec, num_msgs - msg_num);

            if (msg_num > num_msgs) // done
                return 0;
        }
        sleep(1);
    }

    return 0;

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
		if (token !=NULL)
			arguments.push_back(token);

    } 
	return arguments;
}

void usage(void)
{
    fprintf(stderr, "%s", Usage);
    exit(1);
}

