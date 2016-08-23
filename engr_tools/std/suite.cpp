/* FILENAME: Suite.cpp
 * PURPOSE: std's Suite class implemenation
 * CHANGELOG:
 *   11/10/99 - dyes - initial version
 */


#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "platform.h"
#include "logger.hpp"
#include "clock.hpp"
#include "suite.hpp"
#include "keywordvalueparser.hpp"
#include "test.hpp"
#include "version.h"
//
// initialize statics
//
int Suite::_total_tests = 0;
int Suite::_total_run = 0;
int Suite::_total_pass = 0;
int Suite::_total_fail = 0;
int Suite::_total_skipped = 0;
char* Suite::_single_test_name = 0;
bool Suite::_exedir_cleanup = true;


// FUNCTION: Suite
// PURPOSE:  constructs a suite instance
Suite::Suite(const char* path, int levels)
: _levels(levels), _subsuite_list(0), _test_list(0),
  _source_dir(0), _suite_failed(false), _parsing_error_count(0),
  _datafile(0)
{
    ASSERT(path);
    ASSERT(levels > 0);

    // determine source_dir for this suite
    char* saved_dir = getcwd(0,0);
    if (chdir(path) != 0)
    {
        LOG_FATAL("suite construction, from %s chdir %s failed",
                   saved_dir, path);
    }

    _source_dir = getcwd(0,0);
    if(chdir(saved_dir) != 0)
    {
        LOG_FATAL("suite construction, chdir back to %s failed",
                   saved_dir);
    }
    free(saved_dir);

    // init _name until it is parsed from std.dat
    _name = new char[1];
    *_name = 0;

    Clock::getAsciiTime(_start_time);

    LOG_DEBUG("Suite constructed, source_dir=%s  levels=%d  starttime=%s",
              _source_dir, _levels, _start_time);
}


// FUNCTION: ~Suite
// PURPOSE:  destroys a suite instance
Suite::~Suite()
{
    LOG_DEBUG("destroying Suite %s  dir=%s", getName(), getSourceDir());
    
    // delete all subsuites and tests
    while(_subsuite_list)
        deleteSubsuite();
    while(_test_list)
        deleteTest();

    free(_source_dir); // malloced in constructor by getcwd()
    _source_dir = 0;

    delete [] _name;
    _name = 0;
}


// FUNCTION: summary
// PURPOSE: prints an execution summary to stdout
void Suite::summary()
{
    char end_time[40];
    Clock::getAsciiTime(end_time);

    // title
    LOG_VERBATIM("\n********** std version %s summary **********", Version);

    // suite name, datafile path
    LOG_VERBATIM("suite=%s     dir=%s", getName(), getSourceDir());

    // start time, end time
    LOG_VERBATIM("started  %s\nfinished %s", _start_time, end_time);

    // total tests 
    LOG_VERBATIM("total tests=%d", _total_tests);

    // total run and not run
    LOG_VERBATIM("        run=%d      skipped=%d", _total_run, _total_skipped);

    // total pass and fail
    LOG_VERBATIM("       pass=%d         fail=%d", _total_pass, _total_fail);
}


// FUNCTION: check
// PURPOSE: performs integrity check of this Suite instance including its member data
// returns true if all OK, else false
bool Suite::check()
{
    bool pass_flag = true; // only set true here, any failure will set to false

    LOG_DEBUG("check: name=%s  dir=%s", getName(), getSourceDir());

    // verify no parsing errors
    if(_parsing_error_count != 0)
    {
        LOG_ERROR("check failed, suite %s has %d parsing errors",
                  getSourceDir(), _parsing_error_count);
        pass_flag = false;
    }

    // check all tests
    testnode_t* pTestnode;
    for(pTestnode = _test_list; pTestnode; pTestnode = pTestnode->pNext)
    {
        if (pTestnode->pTest->check() == false)
            pass_flag = false;
    }

    // check all subsuites
    suitenode_t* pSuitenode;
    for(pSuitenode = _subsuite_list; pSuitenode; pSuitenode = pSuitenode->pNext)
    {
        if (pSuitenode->pSuite->check() == false)
            pass_flag = false;
    }
    
    // check that if running a single test, that it is defined only once in this suite
    if(_single_test_name)
    {
        bool single_test_found = false;
        for(pTestnode = _test_list; pTestnode; pTestnode = pTestnode->pNext)
        {
            if (strcmp(pTestnode->pTest->getName(), _single_test_name) == 0)
            {
                if(single_test_found)
                {
                    LOG_ERROR("test=%s defined multiple times in suite=%s", 
                              pTestnode->pTest->getName(), getName());
                    pass_flag = false;
                }
                single_test_found = true;
            }
        }
        if(single_test_found == false)
        {
            LOG_ERROR("test=%s not defined in suite=%s",
                      _single_test_name, getName());
            pass_flag = false;
        }
    }

    return pass_flag;

} // end check()


// FUNCTION: run
// PURPOSE: runs the test suite, returns true if suite passed
bool Suite::run()
{
    ASSERT(_source_dir);
    LOG_INFO("running suite=%s from %s", getName(), getSourceDir());


    // run subsuites first, but only if not doing a single test
    if(_single_test_name == 0)
    {
        for(suitenode_t* pSuiteNode=_subsuite_list; pSuiteNode; pSuiteNode=pSuiteNode->pNext)
        {
            if(pSuiteNode->pSuite->run() == false)
            {
                _suite_failed = true;
            }
        }
    }

    // run tests
    // if running a specific test, run that one only
    for(testnode_t* pTestNode=_test_list; pTestNode; pTestNode=pTestNode->pNext)
    {
        if(_single_test_name)
        {
            if(strcmp(_single_test_name, pTestNode->pTest->getName()) != 0)
            {
                continue;
            }
        }

        _total_tests++;

        // skip test only if it wasn't run with -test option
        if(pTestNode->pTest->getSkip() == true && _single_test_name == 0)
        {
            pTestNode->pTest->skip();
            _total_skipped++;
        }
        else
        {
            _total_run++;
            if(pTestNode->pTest->run() == true)
            {
                _total_pass++;
            }
            else
            {
                _total_fail++;
                _suite_failed = true;
            }
        }
    }


    if(_suite_failed)
        return false;
    else
        return true;
}


// FUNCTION: parse
// PURPOSE: parses suite's datafile and builds suite accordingly
void Suite::parseDataFile(const char* datafile)
{
    ASSERT(datafile);
    _datafile = datafile;
    char* saved_dir = getcwd(0,0);

    // cd to suite's directory
    if(chdir(_source_dir) == -1)
    {
        LOG_ERROR("parseDataFile: chdir %s failed", _source_dir);
        _parsing_error_count++;
        return;
    }
    LOG_DEBUG("parsing %s/%s", _source_dir, datafile);

    KeywordValueParser parser(datafile); // opens std.dat
    if(parser.check() == false)
    {
        parsingError("parser.check failed", parser);

        // cd back and return
        if(chdir(saved_dir) == -1)
        {
            LOG_FATAL("failed to chdir back to %s", saved_dir);
        }
        free(saved_dir);
        return;
    }

    const char* keyword;
    const char* value;
    struct stat statbuf;
    Suite* pNewsuite;
    Test* pNewtest = 0;
    int exit_code;
    char* cwd;
    while(parser.getNext())
    {
        keyword = parser.getKeyword();
        value = parser.getValue();
        if(strcmp("SUITE", keyword) == 0)
        {
            if(value == 0)
            {
                parsingError("SUITE name not defined", parser);
                continue;
            }
            if(strlen(getName()) > 0)
            {
                parsingError("SUITE name already defined", parser);
                continue;
            }
            delete [] _name;
            _name = new char[strlen(value)+1];
            strcpy(_name, value);
            LOG_DEBUG("parsed SUITE %s", getName());
        }
        else if(strcmp("SUBSUITE", keyword) == 0)
        {
            // skips subsuites if running just one test
            if(_single_test_name)
            {
                continue;
            }

            // check nesting constraint
            if(_levels <= 1)
            {
                parsingError("nesting limit exceeded", parser);
                continue;
            }
            // ensure value is valid directory
            if(stat(value, &statbuf) != 0)
            {
                cwd = getcwd(0,0);
                LOG_DEBUG("stat failed, cwd=%s path=\"%s\"", cwd, value);
                free(cwd);
                parsingError("stat failed on subsuite path", parser);
                continue;
            }
            if((statbuf.st_mode & _S_IFDIR) == 0)
            {
                parsingError("subsuite path not directory", parser);
                continue;
            }
            
            LOG_DEBUG("parsed SUBSUITE %s", value);
            pNewsuite = new Suite(value, _levels-1);
            pNewsuite->parseDataFile();
            addSubsuite(pNewsuite);
        }
        else if(strcmp("TEST", keyword) == 0)
        {
            if(value == 0)
            {
                parsingError("NULL value", parser);
                continue;
            }
            LOG_DEBUG("parsed TEST %s", value);
            pNewtest = new Test(value, this);
            addTest(pNewtest);
        }
        else if(strcmp("COPY", keyword) == 0)
        {
            if(value == 0)
            {
                parsingError("NULL value", parser);
                continue;
            }
            // add to current test
            // pNewtest still points to current test
            if(pNewtest == 0)
            {
                parsingError("no current test", parser);
                continue;                
            }
            LOG_DEBUG("parsed COPY %s", value);
            pNewtest->addFiles(value);
        }
        else if(strcmp("RUN", keyword) == 0)
        {
            if(value == 0)
            {
                parsingError("NULL value for RUN", parser);
                continue;
            }
            // RUN commands cannot be backgrounded
            if(isBackgrounded(value))
            {
                parsingError("backgrounded RUN command", parser);
                continue;
            }
            // add to current test
            // pNewtest still points to current test
            if(pNewtest == 0)
            {
                parsingError("no current test", parser);
                continue;                
            }
            if(pNewtest->getRunCommand())
            {
                parsingError("RUN command already defined", parser);
                continue;
            }
            LOG_DEBUG("parsed RUN %s", value);
            pNewtest->setRunCommand(value);
        }
        else if(strcmp("VERIFY", keyword) == 0)
        {
            if(value == 0)
            {
                parsingError("NULL value", parser);
                continue;
            }
            if(isBackgrounded(value))
            {
                parsingError("backgrounded VERIFY command", parser);
                continue;
            }
            // add to current test
            // pNewtest still points to current test
            if(pNewtest == 0)
            {
                parsingError("no current test", parser);
                continue;                
            }
            LOG_DEBUG("parsed VERIFY %s", value);
            pNewtest->addVerify(value);
        }
        else if(strcmp("SKIP", keyword) == 0)
        {
            if(value != 0)
            {
                parsingError("no value expected", parser);
                continue;
            }
            // add to current test
            // pNewtest still points to current test
            if(pNewtest == 0)
            {
                parsingError("no current test", parser);
                continue;                
            }
            LOG_DEBUG("parsed SKIP");
            pNewtest->setSkip();
        }
        else if(strcmp("EXIT", keyword) == 0)
        {
            if(value == 0)
            {
                parsingError("exit value expected", parser);
                continue;
            }
            // add to current test
            // pNewtest still points to current test
            if(pNewtest == 0)
            {
                parsingError("no current test", parser);
                continue;                
            }
            if(sscanf(value, "%d", &exit_code) != 1)
            {
                parsingError("could not parse EXIT value", parser);
                continue;
            }
            LOG_DEBUG("parsed EXIT %s", value);
            pNewtest->setExpectedExitCode(exit_code);
        }
        else if(strcmp("SETUP", keyword) == 0)
        {
            if(value == 0)
            {
                parsingError("NULL value", parser);
                continue;
            }
            // add to current test
            // pNewtest still points to current test
            if(pNewtest == 0)
            {
                parsingError("no current test", parser);
                continue;                
            }
            LOG_DEBUG("parsed SETUP %s", value);
            pNewtest->addSetup(value);
        }
        else if(strcmp("CLEANUP", keyword) == 0)
        {
            if(value == 0)
            {
                parsingError("NULL value", parser);
                continue;
            }
            // add to current test
            // pNewtest still points to current test
            if(pNewtest == 0)
            {
                parsingError("no current test", parser);
                continue;                
            }
            LOG_DEBUG("parsed CLEANUP %s", value);
            pNewtest->addCleanup(value);
        }
        else if(strcmp("PRECONDITION", keyword) == 0)
        {
            if(value == 0)
            {
                parsingError("NULL value", parser);
                continue;
            }
            // add to current test
            // pNewtest still points to current test
            if(pNewtest == 0)
            {
                parsingError("no current test", parser);
                continue;                
            }
            LOG_DEBUG("parsed PRECONDITION %s", value);
            pNewtest->addPrecondition(value);
        }
        else if(strcmp("SPEC", keyword) == 0)
        {
            if(value == 0)
            {
                parsingError("NULL value", parser);
                continue;
            }
            // add to current test
            // pNewtest still points to current test
            if(pNewtest == 0)
            {
                parsingError("no current test", parser);
                continue;                
            }
            LOG_DEBUG("parsed SPEC %s", value);
            pNewtest->addSpec(value);
        }
        else if(strcmp("EXPORT", keyword) == 0)
        {
            if(value == 0)
            {
                parsingError("NULL value", parser);
                continue;
            }
            // also verify that value is of form "a=b"
            if(strchr(value, '=') == 0           // no '='
               || strchr(value, '=') == value)  // 1st char is '='
            {
                parsingError("form \"a=b\" expected", parser);
                continue;
            }

            // add to current test
            // pNewtest still points to current test
            if(pNewtest == 0)
            {
                parsingError("no current test", parser);
                continue;                
            }
            LOG_DEBUG("parsed EXPORT %s", value);
            pNewtest->addExport(value);
        }
        else // invalid keyword
        {
            parsingError("invalid keyword", parser);
        }
    } // end while

    // verify that SUITE name has been given
    if(strlen(getName()) == 0)
    {
        parsingError("no SUITE name given", parser);
    }

    // cd back
    if(chdir(saved_dir) == -1)
        LOG_FATAL("failed to chdir back to %s", saved_dir);

    free(saved_dir);

} // end parseDataFile()


// FUNCTION: isBackgrounded(const char* command)
// PURPOSE: true if command is backgrounded, i.e. has a '&' at end
bool Suite::isBackgrounded(const char* command)
{
    ASSERT(command);
    if(command[strlen(command)-1] == '&')
        return true;

    return false;
}


// FUNCTION: parsingError
// PURPOSE: logs parsing errors and increments parsingErrorCount
void Suite::parsingError(char* msg, KeywordValueParser& parser)
{
    if(msg == 0)
        msg = "";

    LOG_ERROR("parsingError: \"%s\" file=%s/%s linenum=%d line=%s",
              msg, _source_dir, _datafile, parser.getLineNumber(), parser.getLine());
    _parsing_error_count++;
}


// FUNCTION: addSubsuite
// PURPOSE: adds subsuite to subsuite list
void Suite::addSubsuite(Suite* pSuite)
{
    ASSERT(pSuite);

    suitenode_t* newnode = new suitenode_t;
    newnode->pSuite = pSuite;
    newnode->pNext = 0;

    // walk to end of list and add node
    if(_subsuite_list == 0)
    {
        _subsuite_list = newnode;
    }
    else
    {
        suitenode_t* p;
        for(p = _subsuite_list; p->pNext ; p = p->pNext);
        p->pNext = newnode;
    }

    LOG_DEBUG("added suite=%s %s", pSuite->getName(), pSuite->getSourceDir());
}


// FUNCTION: deleteSubsuite
// PURPOSE: remove and delete 1st subsuite on subsuite list
// when last subsuite has been removed, _subsuite_list is NULL
void Suite::deleteSubsuite()
{
    ASSERT(_subsuite_list);

    suitenode_t* temp = _subsuite_list->pNext;
    delete _subsuite_list->pSuite;
    delete _subsuite_list;
    _subsuite_list = temp;
}



// FUNCTION: addTest
// PURPOSE: adds test to test list
void Suite::addTest(Test* pTest)
{
    ASSERT(pTest);

    testnode_t* newnode = new testnode_t;
    newnode->pTest = pTest;
    newnode->pNext = 0;

    // walk to end of list and add node
    if(_test_list == 0)
    {
        _test_list = newnode;
    }
    else
    {
        testnode_t* p;
        for(p = _test_list; p->pNext ; p = p->pNext);
        p->pNext = newnode;
    }
    LOG_DEBUG("added test=%s", pTest->getName());
}


// FUNCTION: deleteTest
// PURPOSE: remove and delete 1st Test on test list
// when last test has been removed, _test_list is NULL
void Suite::deleteTest()
{
    ASSERT(_test_list);

    testnode_t* temp = _test_list->pNext;
    delete _test_list->pTest;
    delete _test_list;
    _test_list = temp;
}


// FUNCTION: showSpecs
// PURPOSE: shows test specs
void Suite::showSpecs()
{
    // traverse subsuites first
    for(suitenode_t* pSuiteNode=_subsuite_list; pSuiteNode; pSuiteNode=pSuiteNode->pNext)
    {
        pSuiteNode->pSuite->showSpecs();
    }

    // then traverse tests in this suite
    for(testnode_t* pTestNode=_test_list; pTestNode; pTestNode=pTestNode->pNext)
    {
        pTestNode->pTest->showSpecs();
    }
}


// FUNCTION: showSkips
// PURPOSE: shows skipped tests
void Suite::showSkips()
{
    // traverse subsuites first
    for(suitenode_t* pSuiteNode=_subsuite_list; pSuiteNode; pSuiteNode=pSuiteNode->pNext)
    {
        pSuiteNode->pSuite->showSkips();
    }

    // then traverse tests in this suite
    for(testnode_t* pTestNode=_test_list; pTestNode; pTestNode=pTestNode->pNext)
    {
        _total_tests++;
        if(pTestNode->pTest->getSkip() == true)
        {
            pTestNode->pTest->skip();
            _total_skipped++;
        }    
    }
}


// FUNCTION: workspaceCleanup
// PURPOSE: forces removal of test workspace
void Suite::workspaceCleanup()
{
    ASSERT(_source_dir);

    // cleanup subsuites first, but only if not doing a single test
    if(_single_test_name == 0)
    {
        for(suitenode_t* pSuiteNode=_subsuite_list; pSuiteNode; pSuiteNode=pSuiteNode->pNext)
        {
            pSuiteNode->pSuite->workspaceCleanup();
        }
    }

    // cleanup test workspaces
    // if a specific test, cleanup that one only
    for(testnode_t* pTestNode=_test_list; pTestNode; pTestNode=pTestNode->pNext)
    {
        if(_single_test_name)
        {
            if(strcmp(_single_test_name, pTestNode->pTest->getName()) != 0)
            {
                continue;
            }
        }

        pTestNode->pTest->workspaceCleanup();
    }
}
