/* FILENAME: Suite.hpp
 * PURPOSE: header file for std Suite class
 * CHANGELOG:
 *   11/10/99 - dyes - initial version
 */

#ifndef __SUITE_HPP__ //  prevents multiple inclusion
#define __SUITE_HPP__


// forward declarations
class KeywordValueParser;
class Suite;
class Test;


typedef struct suitenode
{
    Suite* pSuite;
    struct suitenode* pNext;
} suitenode_t;

typedef struct testnode
{
    Test* pTest;
    struct testnode* pNext;
} testnode_t;


// CLASSNAME: Suite
// RESPONSIBILITIES:
//   1. provides single run summary, using statics so that multiple Suites can be created and run.
//   2. verifies integrity of datafile and all subsuite datafiles prior to running tests.
//   3. creates and runs subsuites as called out in the datafile
//   4. creates and runs tests as called out in the datafile
// COLLABORATIONS:
//   1. creates Suite instances for each subsuite called out in the datafile
//   2. creats Test instances for each test called out in the datafile
class Suite
{
    public:

        Suite(const char* path, int levels);
        virtual ~Suite();

        void parseDataFile(const char* datafile="std.dat");
        bool check(); // integrity check of this Suite, true if OK, else false
		bool run(); // executes the suite, returns true if all passed
		bool didItPass() { return(!_suite_failed); };
        void summary(); // execution summary to std.sum
        void showSkips(); // show skipped tests to user
        void showSpecs(); // show test specs to user
        void workspaceCleanup(); // cleans up workspace

        static void setSingleTest(char* name) { _single_test_name = name; };
        static void setNoCleanup() { _exedir_cleanup = false; };

        const char* getName() { return _name; };
        const char* getSourceDir() { return _source_dir; };
        bool getExeDirCleanup() { return _exedir_cleanup; };

    protected:

        void parsingError(char* msg, KeywordValueParser& parser);
        void addSubsuite(Suite* suite);
        void deleteSubsuite();
        void addTest(Test* test);
        void deleteTest();
        bool isBackgrounded(const char* command);

        suitenode_t* _subsuite_list;
        testnode_t* _test_list;
        int _levels;

        char* _name; // name of this suite
        char _start_time[40]; // ascii start time of this suite

        char* _source_dir; // absolute path of this suite
        const char* _datafile;

        static char* _single_test_name; // 0 or name of single test to run
        int _parsing_error_count;

        bool _suite_failed;

        static bool _exedir_cleanup; // false to disable work dir cleanup


        // following used to generate summary info
        static int _total_tests;
        static int _total_run;
        static int _total_pass;
        static int _total_fail;
        static int _total_skipped;

    private:

        Suite(); // catch unauthorized use
        Suite(const Suite&); // catch unauthorized use
        Suite& operator=(const Suite& rhs); // catch unauthorized use
};

#endif // __SUITE_HPP__

