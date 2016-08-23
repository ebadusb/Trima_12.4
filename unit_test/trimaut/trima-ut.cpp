#include <trima-ut.hpp>

/**
 * TrimaUTCase::AssertionFailed implementations
 * */
TrimaUTCase::AssertionFailed::AssertionFailed(const char * context, const char * message):_ctx(context), _msg(message){}
TrimaUTCase::AssertionFailed::AssertionFailed(const TrimaUTCase::AssertionFailed &af):_ctx(af.getContext()), _msg(af.getMessage()){}
TrimaUTCase::AssertionFailed::~AssertionFailed(){}

const char * TrimaUTCase::AssertionFailed::toString() const
{
    string assertString("Assertion failed (");
    assertString += string(this->getContext()) + string("): ") + string(this->getMessage());
    
    return assertString.c_str();
}

/**
 * TrimaUTCase implementations
 * */
TrimaUTCase::TrimaUTCase(const char * name, TrimaUTFunctPtr testFunct):
    ITrimaUTRunnable(name), _test(testFunct), _status(NOTRUN){}

TrimaUTCase::~TrimaUTCase(){}

int TrimaUTCase::run() throw(TrimaUTCase::AssertionFailed)
{
    //make sure we have a test to run
    if(!_test) return 0;

    _test(this, _data);

    _status = PASSED;

    return 1;
}

/**
 * TrimaUTSuite implementations
 * */
TrimaUTSuite::TrimaUTSuite(const char * name):ITrimaUTRunnable(name), _passed(0), _frontEnd(0){ }

TrimaUTSuite::~TrimaUTSuite()
{
    if(!_suite.empty())
        _suite.clear();

    _frontEnd = 0;
}

int TrimaUTSuite::run()
{
    if(_suite.empty() || (!_frontEnd)) return 0;

    int tests_run = 0;

    for(list< TrimaUTCase >::iterator itTC = _suite.begin(); itTC != _suite.end(); itTC++){
        _frontEnd->postBanner(itTC->getName());

        try{
            itTC->pre();
        }
        catch(...)
        {
            //failed test setup may have left test data in an
            //inconsistent state, so exit now
            break;
        }

        try{
            itTC->setData(this->_data);

            if(itTC->run()) tests_run++; _passed++;
        }
        catch(const TrimaUTCase::AssertionFailed &afe)
        {
            _frontEnd->postFailure(afe);

            tests_run++;
        }
        catch(...)
        {
            string exString(itTC->getName());
            exString += " aborted with unknown error";

            _frontEnd->postMessage(exString);
        }

        _frontEnd->postStatus(itTC->getName(), TEST_STATUS_TEXT(itTC->getStatus()));

        try{
            itTC->post();
        }
        catch(...)
        {
            //failed test teardown may have left test data in an
            //inconsistent state, so exit now
            break;
        }
    }

    return tests_run;
}

/**
 * TrimaUTRunner::DefaultFrontEnd implementations
 * */
inline void TrimaUTRunner::DefaultFrontEnd::postBanner(const char * name) const
{ 
    cout << "Running " << name << "..." << endl; 
}

inline void TrimaUTRunner::DefaultFrontEnd::postStatus(const char * name, const char * status) const
{ 
    cout << name << " " << status << endl; 
}

inline void TrimaUTRunner::DefaultFrontEnd::postMessage(const char * msg) const
{ 
    cout << msg << endl; 
}

inline void TrimaUTRunner::DefaultFrontEnd::postMessage(const string &msg) const
{ 
    cout << msg << endl; 
}

inline void TrimaUTRunner::DefaultFrontEnd::postFailure(const TrimaUTCase::AssertionFailed &afe) const
{ 
    cout << afe.toString() << endl; 
}

inline void TrimaUTRunner::DefaultFrontEnd::postRollup(const char * name, int passed, int run) const
{
    cout << name << " COMPLETE (" << passed << "/" << run << " Passed)" << endl;
}

/**
 * TrimaUTRunner implementations
 * */
void TrimaUTRunner::execute(TrimaUTCase tc)
{
    _frontEnd->postBanner(tc.getName());

    try{
        tc.pre();
    }
    catch(...)
    {
        //failed test setup may have left test data in an
        //inconsistent state, so exit now
        _frontEnd->postStatus(tc.getName(), TEST_STATUS_TEXT(tc.getStatus()));

        _frontEnd->postRollup(tc.getName(), 0, 0);

        return;
    }

    try{
        tc.run();
    }
    catch(const TrimaUTCase::AssertionFailed &afe)
    {
        _frontEnd->postFailure(afe);
    }
    catch(...)
    {
        string exString(tc.getName());
        exString += " aborted with unknown error";

        _frontEnd->postMessage(exString);
    }

    try{
        tc.post();
    }
    catch(...){/* don't care */}

    _frontEnd->postStatus(tc.getName(), TEST_STATUS_TEXT(tc.getStatus()));

    _frontEnd->postRollup(
        tc.getName(), 
        ((tc.getStatus() == PASSED)?1:0), 
        ((tc.getStatus() == NOTRUN)?0:1)
    );
}

void TrimaUTRunner::execute(TrimaUTSuite ts)
{
    ts.setConsole(_frontEnd);

    _frontEnd->postBanner(ts.getName());

    try{
        ts.pre();
    }
    catch(...)
    {
        //failed test setup may have left test data in an
        //inconsistent state, so exit now
        _frontEnd->postRollup(ts.getName(), 0, 0);

        return;
    }

    int runs = ts.run();

    try{
        ts.post();
    }
    catch(...){/* don't care */}

    _frontEnd->postRollup(ts.getName(), ts.getPassed(), runs);
}
