/* FILENAME: Test.cpp
 * $Header: J:/BCT_Development/Trima/engr_tools/std/rcs/test.cpp 1.6 2000/03/09 21:53:12 TD07711 Exp hr10414 $
 * PURPOSE: std's Test class implemenation
 * CHANGELOG:
 *   $Log: test.cpp $
 *   Revision 1.6  2000/03/09 21:53:12  TD07711
 *   IT4580
 *   01/18/2000 - dyes - replace lists with STL list<>
 *   Revision 1.6  2000/01/13 21:23:21  TD07711
 *   01/05/2000 - dyes - remove -p copy option to avoid permission problems
 *      run all CLEANUP commands, even if 1st one fails.
 *      remove Warning message on CLEANUP failure.
 *   12/08/99 - dyes - added lockfile for concurrent use
 *   11/10/99 - dyes - initial version
 */


#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>


#include "platform.h"
#include "logger.hpp"
#include "clock.hpp"
#include "test.hpp"
#include "suite.hpp"


typedef list<const char*>::iterator iter_t;

// 
// initialize statics
//
char Test::_command_buf[COMMAND_BUF_SIZE];


// FUNCTION: Test
// PURPOSE:  constructor
Test::Test(const char* name, Suite* parent)
: _run_command(0), _parent(parent),
  _skip_flag(false), _pass_flag(false), _expected_exit_code(0), _lockfile(0),
  _exe_dir(0), _quoted_exe_dir(0)
{
    ASSERT(name);
    ASSERT(parent);

    _name = new char[strlen(name)+1];
    strcpy(_name, name);

    Clock::getAsciiTime(_start_time);

    // determine exedir and lockfile
    int namelength = strlen(_parent->getSourceDir()) + strlen(getName()) + 10;
    _exe_dir = new char[namelength];
    _quoted_exe_dir = new char[namelength+10];
    _lockfile = new char[namelength+20];
#ifdef BUILD_FOR_NT
    sprintf(_exe_dir, "%s\\%s.dir", _parent->getSourceDir(), getName());
    sprintf(_lockfile, "%s\\std.lock", _exe_dir);
#else
    sprintf(_exe_dir, "%s/%s.dir", _parent->getSourceDir(), getName());
    sprintf(_lockfile, "%s/std.lock", _exe_dir);
#endif
    sprintf(_quoted_exe_dir, "\"%s\"", _exe_dir);
	LOG_DEBUG("Test=%s constructed, source_dir=%s exedir=%s",
              getName(), getSourceDir(), getExeDir());
}


// FUNCTION: ~Test
// PURPOSE:  destructor
Test::~Test()
{
    LOG_DEBUG("destroying Test=%s  dir=%s", getName(), getSourceDir());
    
    // free storage for list data
    iter_t i;
    for(i = _export_list.begin(); i != _export_list.end(); i++)
        free((void*)*i);
    for(i = _files_list.begin(); i != _files_list.end(); i++)
        free((void*)*i);
    for(i = _verify_list.begin(); i != _verify_list.end(); i++)
        free((void*)*i);
    for(i = _setup_list.begin(); i != _setup_list.end(); i++)
        free((void*)*i);
    for(i = _cleanup_list.begin(); i != _cleanup_list.end(); i++)
        free((void*)*i);
    for(i = _precondition_list.begin(); i != _precondition_list.end(); i++)
        free((void*)*i);

    delete [] _name;
    _name = 0;

    delete [] _run_command;
    _run_command = 0;

    delete [] _exe_dir;
    _exe_dir = 0;

    delete [] _quoted_exe_dir;
    _quoted_exe_dir = 0;
}


// FUNCTION: check
// PURPOSE: performs integrity check of this Test instance including its member data
// returns true if all OK, else false
bool Test::check()
{
    LOG_DEBUG("check: Test name=%s   dir=%s", getName(), getSourceDir());

    if(_run_command == 0)
    {
        LOG_INFO("suite=%s test=%s has no RUN command",
                  getSourceDir(), getName());
        return false;
    }
    
    return true;

} // end check()


// FUNCTION: skip
// PURPOSE: log message indicating this test is being skipped
void Test::skip()
{
    LOG_INFO(" SKIP   test=%s from=%s", getName(), getSourceDir());
}


// FUNCTION: run
// PURPOSE: runs the test, returns true is test ran and passed
bool Test::run()
{
    ASSERT(_exe_dir);
    ASSERT(_run_command);
    LOG_INFO("running test=%s in %s", getName(), getExeDir());


    char* savedir = getcwd(0,0);
    if(chdir(getSourceDir()) == -1)
    {
        LOG_INFO(" ABORT  test=%s failed to chdir to %s", 
                 getName(), getSourceDir());
        if(chdir(savedir) == -1)
        {
            LOG_FATAL("test=%s failed to chdir back to %s", getName(), savedir);
        }
        free(savedir);
        return false; // indicate test failed
    }

    // abort test if workdir is locked
    bool test_aborted = false; // set true if any part of setup fails
    struct stat statbuf;
    if( stat(_lockfile, &statbuf) == 0 )
    {
        test_aborted = true;
        LOG_INFO(" ABORT  test=%s workdir locked", getName());
        free(savedir);
        return false;
    }


    // first removed test's execution directory if present
    if(stat(getExeDir(), &statbuf) == 0)
    {
        if((statbuf.st_mode & _S_IFDIR) == false)
        {
            LOG_INFO(" ABORT  test=%s could not use %s for execution space",
                     getName(), getExeDir());
            test_aborted = true;
        }
        else
        {
            cleanupExedir();
        }
    }

    // create new execution directory and lockfile
    if(test_aborted == false)
    {
        LOG_DEBUG("test=%s, creating execution dir %s", getName(), getExeDir());
        if(mkdir(getExeDir(), 0777) == -1)
        {
            LOG_DEBUG("mkdir failed on %s", getExeDir());
            LOG_INFO(" ABORT  test=%s mkdir failed on %s", 
                     getName(), getExeDir());
            test_aborted = true;
        }
        else // create lock file to avoid simultaneous access problems 
        {
            FILE* fp;
            if ( (fp = fopen(_lockfile, "w")) == 0)
            {
                LOG_INFO(" ABORT  test=%s failed to create lockfile",
                        getName());
                test_aborted = true;
            }
            if (fclose(fp) != 0)
            {
                LOG_ERROR("test=%s failed to close lockfile=%s",
                    getName(), _lockfile);
            }
        }
    }
        
    //
    // copy files into execution space
    //
    iter_t iter;
    if(test_aborted == false)
    {
        char* cwd;
        for(iter=_files_list.begin(); iter != _files_list.end(); iter++)
        {
            sprintf(_command_buf, "cp %s %s", *iter, getQuotedExeDir());
            LOG_DEBUG("test=%s, copy command: %s", getName(), _command_buf);
            if(system(_command_buf) != 0)
            {
                cwd = getcwd(0,0);
                LOG_INFO(" ABORT  test=%s from=%s COPY failed: %s",
                         getName(), cwd, _command_buf);
                free(cwd);
                test_aborted = true;
                break;
            }
        }
    }

    //
    // cd to execution space
    //
    if(test_aborted == false)
    {
        if(chdir(getExeDir()) == -1)
        {
            LOG_INFO(" ABORT  test=%s chdir failed: %s", getName(), getExeDir());
            test_aborted = true;
        }
    }
    
    //
    // verify any preconditions before proceeding
    //
    if(test_aborted == false)
    {
        for(iter=_precondition_list.begin(); iter != _precondition_list.end(); iter++)
        {
            LOG_DEBUG("test=%s, PRECONDITION %s", getName(), *iter);
            if(system(*iter) != 0)
            {
                LOG_INFO(" ABORT  test=%s PRECONDITION failed: %s", getName(), *iter);
                test_aborted = true;
                break;
            }
        }
    }

    // if we're still OK, enable cleanups
    bool do_cleanups = !test_aborted;

    //
    // set any environment variables
    //
    if(test_aborted == false)
    {
        char* buf; // need writable local copy of export data
        char* env_name; // environment variable name
        char* env_value; // environment variable value 
        for(iter=_export_list.begin(); iter != _export_list.end(); iter++)
        {
            // *iter points to a string of form "a=b"
            // "a" is the environment variable name and "b" is the value assigned
            // std.dat parsing has already confirmed the "a=b" format
            // we now replace the '=' with a NULL, and assign value to next char
            // NOTE: value can be NULL which will delete the name env variable
            buf = strdup(*iter);
            env_value = strchr(buf, '=');
            ASSERT(env_value);
            env_name = buf;
            *env_value++ = 0; // null terminate name
            if(*env_value == 0)
            {
                env_value = 0; // required to remove the env variable
            }
            LOG_DEBUG("test=%s, EXPORT %s", getName(), *iter);
            if(setenv(env_name, env_value, 1) != 0)
            {
                LOG_INFO(" ABORT  test=%s EXPORT failed: %s",
                         getName(), *iter);
                test_aborted = true;
                free((void*)buf);
                break;
            }
            free((void*)buf);
        }
    }

    //
    // run any setup commands
    //
    if(test_aborted == false)
    {
        for(iter=_setup_list.begin(); iter != _setup_list.end(); iter++)
        {
            LOG_DEBUG("test=%s, SETUP %s", getName(), *iter);
            if(system(*iter) != 0)
            {
                LOG_INFO(" ABORT  test=%s SETUP failed: %s", getName(), *iter);
                test_aborted = true;
                break;
            }
        }
    }
    
    
    //
    // execute RUN command
    //
    if(test_aborted == false) 
    {
        sprintf(_command_buf, "%s >test.out 2>test.err", _run_command);
        LOG_DEBUG("test=%s, run command: %s", getName(), _command_buf);
        int run_status = system(_command_buf);
        if(run_status == -1)
        {
            LOG_INFO(" ABORT  test=%s, RUN command could not run: %s",
                     getName(), _run_command);
            test_aborted = true;
        }
        else
        {
#ifdef BUILD_FOR_UNIX
            run_status = WEXITSTATUS(run_status);
#endif
            if(run_status == _expected_exit_code)
            {
                _pass_flag = true;
            }
            else
            {
                LOG_INFO(" FAIL   test=%s exitcode=%d expected=%d RUN: %s", 
                         getName(), run_status, _expected_exit_code, _run_command);
            }
        }
    }


    //
    // execute other verifiers if test has passed
    //
    if(_pass_flag == true)
    {
        // check verifiers which on failure will set _pass_flag to false
        for(iter=_verify_list.begin(); iter != _verify_list.end(); iter++)
        {
            if(isOutputRedirected(*iter))
            {
                sprintf(_command_buf, "%s", *iter);
            }
            else
            {
                sprintf(_command_buf, "%s > verify.out", *iter);
            }
            LOG_DEBUG("test=%s, VERIFY %s", getName(), _command_buf);
            
            if(system(_command_buf) != 0)
            {
                LOG_INFO(" FAIL   test=%s VERIFY failed: %s", getName(), _command_buf);
                _pass_flag = false;
                break;
            }
        }
    }


    //
    // run any cleanup commands
    //
    if(do_cleanups)
    {
        for(iter=_cleanup_list.begin(); iter != _cleanup_list.end(); iter++)
        {
            LOG_DEBUG("test=%s, CLEANUP %s", getName(), *iter);
            system(*iter);
            // NOTE: always run all CLEANUP commands, no warnings on failure
        }
    }

    if (remove(_lockfile) != 0)
    {
        LOG_ERROR("test=%s failed to remove lockfile=%s",
            getName(), _lockfile);
    }

    // cd back to source dir and cleanup exedir
    if(chdir(getSourceDir()) == -1)
    {
        LOG_FATAL("test=%s failed to chdir back to %s", getName(), getSourceDir());
    }
    if(getExeDirCleanup()==true && didItPass()==true)
    {
        cleanupExedir();
    }


    // cd back to saved dir
    if(chdir(savedir) == -1)
    {
        LOG_FATAL("test=%s failed to chdir back to %s", getName(), savedir);
    }
    free(savedir);

    return(didItPass());

} // end run()


// FUNCTION: isOutputRedirected
// PURPOSE: true if stdout is redirected, else false
bool Test::isOutputRedirected(const char* command)
{
    const char* p;
    bool rval = false;
    for(p = command; *p; p++)
    {
        if(*p == '>')
        {
            rval = true;
        }
        else if(*p == '|')
        {
            rval = false;
        }
    }
    return rval;
}


// FUNCTION: cleanupExedir
// PURPOSE: removes exedir and all contents, returns true if successful
void Test::cleanupExedir()
{
    ASSERT(strlen(getExeDir()) > 6); // avoid dinking with /, /bin, etc.

    // remove exedir and all contents
#ifdef BUILD_FOR_NT
    sprintf(_command_buf, "rm -rf %s 2>nul", getQuotedExeDir());
#else
    sprintf(_command_buf, "rm -rf %s 2>/dev/null", getQuotedExeDir());
#endif
    LOG_DEBUG("test=%s, cleanup command: %s", getName(), _command_buf);
    if(system(_command_buf) != 0)
    {
        LOG_WARNING("test=%s command failed: %s", 
                  getName(), _command_buf);
    }
}


// FUNCTION: getSourceDir()
// PURPOSE: returns path of test's source directory
const char* Test::getSourceDir()
{
    return(_parent->getSourceDir());
}


// FUNCTION: getExeDirCleanup()
// PURPOSE: true if exedir should be cleaned up
bool Test::getExeDirCleanup()
{
    return(_parent->getExeDirCleanup());
}


// FUNCTION: addFiles
// PURPOSE: adds files to copy to files list
void Test::addFiles(const char* files)
{
    _files_list.push_back(strdup(files));
    LOG_DEBUG("test=%s added FILES %s", getName(), _files_list.back());
}


// FUNCTION: addVerify
// PURPOSE: adds verify command to verify list
void Test::addVerify(const char* command)
{
    _verify_list.push_back(strdup(command));
    LOG_DEBUG("test=%s added VERIFY %s", getName(), _verify_list.back());
}


// FUNCTION: addSetup
// PURPOSE: adds setup command to setup list
void Test::addSetup(const char* command)
{
    _setup_list.push_back(strdup(command));
    LOG_DEBUG("test=%s added SETUP %s", getName(), _setup_list.back());
}


// FUNCTION: addCleanup
// PURPOSE: adds cleanup command to cleanup list
void Test::addCleanup(const char* command)
{
    _cleanup_list.push_back(strdup(command));
    LOG_DEBUG("test=%s added CLEANUP %s", getName(), _cleanup_list.back());
}


// FUNCTION: setRunCommand
// PURPOSE: registers the command that will run the test
void Test::setRunCommand(const char* command)
{
    _run_command = new char[strlen(command)+1];
    strcpy(_run_command, command);
}


// FUNCTION: addPrecondition
// PURPOSE: adds precondition command to precondition list
void Test::addPrecondition(const char* command)
{
    _precondition_list.push_back(strdup(command));
    LOG_DEBUG("test=%s added PRECONDITION %s", getName(), _precondition_list.back());
}


// FUNCTION: addSpec
// PURPOSE: adds spec to spec list
void Test::addSpec(const char* spec)
{
    _spec_list.push_back(strdup(spec));
    LOG_DEBUG("test=%s added SPEC %s", getName(), _spec_list.back());
}


// FUNCTION: addExport
// PURPOSE: add export entry to export list
void Test::addExport(const char* export)
{
    _export_list.push_back(strdup(export));
    LOG_DEBUG("test=%s added EXPORT %s", getName(), _export_list.back());
}


// FUNCTION: workspaceCleanup()
// PURPOSE: force cleanup of test's workspace
void Test::workspaceCleanup()
{
    // first removed test's execution directory if present
    struct stat statbuf;
    if(stat(getExeDir(), &statbuf) == 0)
    {
        if((statbuf.st_mode & _S_IFDIR) == false)
        {
            LOG_ERROR("test=%s workspace=%s is not a directory, remove by hand",
                     getName(), getExeDir());
        }
        else
        {
            LOG_INFO("removing workspace=%s", getExeDir());
            cleanupExedir();
        }
    }
}


// FUNCTION: showSpecs()
// PURPOSE: display specs associated with this test
void Test::showSpecs()
{
    printf("\nTEST %s  %s\n", getName(), getSourceDir());
    for(iter_t iter=_spec_list.begin(); iter != _spec_list.end(); iter++)
    {
        printf("SPEC %s\n", *iter);
    }
}
