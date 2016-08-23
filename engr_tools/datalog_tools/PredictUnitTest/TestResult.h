/*
 * TestResult.h
 *
 *  Created on: May 19, 2012
 *      Author: allan
 */

#ifndef TESTRESULT_H_
#define TESTRESULT_H_
#include <string>
#include <vector>
#include <iostream>

namespace UnitTest {

using namespace std;

class Result
{
protected:
	string  m_message;
	bool    m_success;
public:
    Result( bool success ):m_success(success){}
    virtual ~Result(){}

    void SetMessage( string message ){ m_message = message;}
    const string& GetMessage() const{ return m_message;}
    bool IsSuccess() const { return m_success; }

    virtual void Log( ostream& out);
};

class AssertResult : public Result
{
public:
    AssertResult(bool success): Result(success){ diffPercent = 0;}
    virtual ~AssertResult(){}

    virtual void Log( ostream& out);

	void SetDiffPercent(float percent){ diffPercent = percent; if( diffPercent < 0 ) diffPercent = -(diffPercent);}
	float GetDiffPercent(){ return diffPercent;}
protected:
	float diffPercent;
};

class MethodResult : public Result
{
public:
    typedef vector<AssertResult*> AssertResults;
    typedef vector<AssertResult*>::iterator AssertIterator;
    typedef vector<AssertResult*>::const_iterator AssertConstIterator;

private:
    AssertResults m_assertResults;
    string        m_methodName;
    double        m_time;
public:
    MethodResult(string methodName):
       Result(true),
       m_methodName(methodName) {}
    virtual ~MethodResult();

    void AddAssertResult(AssertResult& result);
    const AssertResults& GetResults() const { return m_assertResults;}
    void SetTime( double time){ m_time = time; }
    virtual void Log( ostream& out);
};

class TestResult : public Result
{
public:
    typedef vector<MethodResult*> MethodResults;
    typedef vector<MethodResult*>::iterator MethodIterator;
    typedef vector<MethodResult*>::const_iterator MethodConstIterator;

private:
	string m_testName;
	MethodResults m_methodResults;
    void DeleteResults();
public:
	TestResult(string testName) :
            Result(true),
            m_testName(testName){}

	virtual ~TestResult();

	const string& GetTestName() const;

	void AddMethodResult(MethodResult& result);

    const MethodResults& GetResults() const { return m_methodResults;}
    void Clear();
    virtual void Log( ostream& out);
};

typedef vector<TestResult*> TestResults;
typedef vector<TestResult*>::iterator TestIterator;
typedef vector<TestResult*>::const_iterator TestConstIterator;

}

#endif /* TESTRESULT_H_ */
