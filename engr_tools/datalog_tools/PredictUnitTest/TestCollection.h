/*
 * TestCollection.h
 *
 *  Created on: May 19, 2012
 *      Author: allan
 */

#ifndef TESTCOLLECTION_H_
#define TESTCOLLECTION_H_
#include <vector>
#include <map>
#include <typeinfo>
#include <fstream>
#include "TestBase.h"
#include "TestResult.h"

namespace UnitTest {

using namespace std;

class TestCollection {
protected:
	vector<ITest*> m_tests;
    TestResults m_results;

    int m_methods;
    int m_failures;
    int m_successes;

public:
	TestCollection();
	virtual ~TestCollection();

	const TestResults& Execute();
	void Add( ITest* test);
    void Log(ostream& out);
    void Log( int argc, char* argv[]);

    int GetFailures() const { return m_failures; }
private:
    void Summarize();
};

}

#endif /* TESTCOLLECTION_H_ */
