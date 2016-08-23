/* FILENAME: Test.hpp
 * $Header: I:/Trima_Project/Source/tools/STD/rcs/TEST.HPP 1.5 2000/01/13 21:23:24 TD07711 Exp $
 * PURPOSE: header file for std Test class
 * CHANGELOG:
 *   $Log: TEST.HPP $
 *   Revision 1.5  2000/01/13 21:23:24  TD07711
 *   01/05/2000 - dyes
 *      1. changed CLEANUP behavior to ignore exit code
 *      2. support PRECONDITION commands
 *      3. support -cleanup option
 *      4. support -skips option
 *   11/12/99 - dyes - initial version
 */

#ifndef __Test_HPP__ //  prevents multiple inclusion
#define __Test_HPP__

#include <list>

#ifdef BUILD_FOR_NT
using namespace std; // required by vc++ to use STL components
#endif

#define COMMAND_BUF_SIZE 1024

class Suite;


// CLASSNAME: Test
// RESPONSIBILITIES:
//   1. encapsulates all info needed to run a single test
//   2. provides run function to run the test
//   3. creates a test execution directory, and copies all files needed there.
//   4. executes the command line needed to run the test
//   5. directs stdout from the test command line to test.out
//   6. directs stderr from the test command line to test.err
//   7. runs any verification commands specified or the default verifier if none
//      are supplied.
//   8. directs verification output to verify.out and verify.err
// COLLABORATIONS:
//   1. Tests are created and managed by Suite instance
class Test
{
    public:

        Test(const char* name, Suite* parent);
        virtual ~Test();

        bool check(); // integrity check of this Test, true if OK, else false
		bool run(); // executes the test, returns true if passed
		void skip(); // logs info about test being skipped

        bool didItPass() { return _pass_flag; }; 

        void addFiles(const char* files);
        void setRunCommand(const char* command);
        const char* getRunCommand() { return _run_command; };
        void addVerify(const char* command);
        void addSetup(const char* command);
        void addCleanup(const char* command);
        void addPrecondition(const char* command);
        void addSpec(const char* spec);
        void addExport(const char* spec);
        void setSkip() { _skip_flag = true; };
        bool getSkip() { return _skip_flag; };
        void setExpectedExitCode(int code) { _expected_exit_code = code; };
        void workspaceCleanup(); // force cleanup of test's workspace
        void showSpecs(); // display specs associated with this test

        const char* getName() { return _name; };
        const char* getSourceDir();
        const char* getExeDir() { return _exe_dir; };
        const char* getQuotedExeDir() { return _quoted_exe_dir; };
 
    protected:

        void cleanupExedir(); // cleanup execution space
        void deleteFiles();
        void deleteVerify();
        void deleteSetup();
        void deleteCleanup();
        void deletePrecondition();
        void deleteSpec();
        bool isOutputRedirected(const char* command);
        void waitLock();
        bool getExeDirCleanup();

        Suite* _parent;
        bool _nocleanup;

        char* _name; // name of this test
        char _start_time[40]; // ascii start time of this test

        char* _exe_dir; // absolute path of test's exe space
        char* _quoted_exe_dir; //quoted for use in shell commands
        char* _lockfile; // prevents simultaneous use of exe dir
        bool _skip_flag; // true if this test should be skipped
        bool _pass_flag; // true if test ran and passed
        int _expected_exit_code;

        list<const char*> _files_list;
        list<const char*> _verify_list;
        list<const char*> _setup_list;
        list<const char*> _cleanup_list;
        list<const char*> _precondition_list;
        list<const char*> _spec_list;
        list<const char*> _export_list;

        char* _run_command;
        static char _command_buf[COMMAND_BUF_SIZE];

    private:

        Test(); // catch unauthorized use
        Test(const Test&); // catch unauthorized use
        Test& operator=(const Test& rhs); // catch unauthorized use
};

#endif // __Test_HPP__

