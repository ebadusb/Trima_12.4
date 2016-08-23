#ifndef __TRIMAUT__
#define __TRIMAUT__

#include <list>
#include <iterator>
#include <iostream>
#include <string>
#include <exception>

using namespace std;

enum ENUMERATED_TEST_STATUS{ NOTRUN, PASSED, FAILED};

#define TEST_STATUS_TEXT(s) \
( \
    (s == NOTRUN) ? "NOT RUN" : \
    (s == PASSED) ? "PASSED" : \
    (s == FAILED) ? "FAILED" : "UNKNOWN" \
)

/**
 * Test data storage class
 * */
typedef void * TrimaUTDataPtr;

/**
 * Abstract base class providing behavior common for all runnable testing modules - 
 * both test cases and suites.  Type TestDataPtr is a placeholder for storage classes 
 * encapsulating data for testing.
 * */
class ITrimaUTRunnable{
public:
    void setData(TrimaUTDataPtr td){ this->_data = td; }

    virtual int run() = 0;

    const char * getName() const { return _name;}
    
    //test lifecycle events
    virtual void pre(){/* Subclass can implement setup routine */}
    virtual void post(){/* Subclass can implement teardown routine */}

protected:
    ITrimaUTRunnable(const char * name):_name(name){}

    const char * _name;
    TrimaUTDataPtr _data;
};

/**
 * Base class for test cases.  Test case is nothing more than a
 * function container for the unit under test (UTT), allowing us to couple UTT 
 * with lifecycle behavior, test data, and status.  Users need only extend
 * this class for the purpose of defining pre- and post-test operations - i.e.,
 * setup and teardown.  Otherwise, the default behavior of TrimaUTCase simply runs
 * the provided function.  Send to TestRunner directly, or add to a suite.
 **/
class TrimaUTCase: public ITrimaUTRunnable{

    //Test function prototype
    typedef void(*TrimaUTFunctPtr)(TrimaUTCase * const, TrimaUTDataPtr);

public:
    /**
     * AssertionFailed is the exception class thrown indirectly by the test function
     * as a consequence of the condition passed to TrimaUTCase::assertCondition evaluating
     * false.
     * */
    class AssertionFailed{
    public:
        AssertionFailed(const char * context, const char * message);
        //copy constructor for rethrow
        AssertionFailed(const TrimaUTCase::AssertionFailed &af);
        ~AssertionFailed();

        const char * getContext() const { return _ctx; }
        const char * getMessage() const { return _msg; }

        const char * toString() const;

    private:
        //InternalContext is a Momento of the test function location at the point of
        //failure - i.e., the test case object for UTT.
        const char *_ctx;
        const char * _msg;
    };

    TrimaUTCase(const char * name, TrimaUTFunctPtr testFunct);
    virtual ~TrimaUTCase();

    //Returns 1 if UTT completed run, or 0 otherwise
    int run() throw(TrimaUTCase::AssertionFailed);

    //should be called only by the test function
    void assertCondition(bool condition, const char * message) throw(TrimaUTCase::AssertionFailed){
        _status = FAILED;

        if(!condition) 
            throw TrimaUTCase::AssertionFailed(this->getName(), message);
    }

    ENUMERATED_TEST_STATUS getStatus() const { return _status; }

protected:
    TrimaUTFunctPtr _test;
    ENUMERATED_TEST_STATUS _status;
};

/**
 * Abstract interface for any controller class responsible for posting test
 * output.  By decoupling this interface from the executor class 
 * (TrimaUTRunner), we can quickly adapt this unit testing framework
 * for different environments and output/display requirements.
 * */
class ITrimaUTConsole{
public:
    //For posting test metrics and progress
    virtual void postBanner(const char * name) const = 0;
    virtual void postStatus(const char * name, const char * status) const = 0;
    virtual void postMessage(const char * msg) const = 0;
    virtual void postMessage(const string &msg) const = 0;
    virtual void postFailure(const TrimaUTCase::AssertionFailed &afe) const = 0;
    virtual void postRollup(const char * name, int passed, int run) const = 0;
};

/**
 * Aggregation for running multiple test cases in succession.  Users need only extend
 * this class for the purpose of defining pre- and post-test operations - i.e.,
 * setup and teardown before/after the entire suite.
 * */
class TrimaUTSuite: public ITrimaUTRunnable{
public:
    TrimaUTSuite(const char * name);
    ~TrimaUTSuite();

    void addTestCase(TrimaUTCase tc){ _suite.push_back(tc); }
    void setConsole(ITrimaUTConsole * console) { _frontEnd = console; }

    //Returns the number of test cases run.  Use the getPassed method
    //to actually get the number of test cases passing
    int run();

    //metrics (not including actual run size reported from TestSuite::run())
    int getCount(){ return _suite.size(); }
    int getPassed() const { return _passed; }

private:
    list<TrimaUTCase> _suite;
    int _passed;
    ITrimaUTConsole *_frontEnd;
};

/**
 * Delegate for VxWorks command-line facing functions to run test cases.  Note
 * that a front-end adapter needs to be bound to the runner in order to display
 * output appropriately.
 * */
class TrimaUTRunner{
public:
    /**
     * Default console implementation, which simply posts output to standard out.
     * */
    class DefaultFrontEnd: public ITrimaUTConsole {
    public:
        DefaultFrontEnd(){/* default to standard output */}
        
        void postBanner(const char * name) const;
        void postStatus(const char * name, const char * status) const;
        void postMessage(const char * msg) const;
        void postMessage(const string &msg) const;
        void postFailure(const TrimaUTCase::AssertionFailed &afe) const;
        void postRollup(const char * name, int passed, int run) const;
    };

    TrimaUTRunner(ITrimaUTConsole * console){ _frontEnd = console;}

    //Overloaded executors for test cases run separately, or in a suite
    void execute(TrimaUTCase tc);
    void execute(TrimaUTSuite ts);

private:
    ITrimaUTConsole *_frontEnd;
};

#endif  //__TRIMAUT__
