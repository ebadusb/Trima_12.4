/*
 * TestCollection.cpp
 *
 *  Created on: May 19, 2012
 *      Author: allan
 */

#include "TestCollection.h"
namespace UnitTest {

TestCollection::TestCollection() {
}

TestCollection::~TestCollection() {
	vector<ITest*>::const_iterator iter = m_tests.begin();
	for( ; iter != m_tests.end(); iter++)
	{
		delete *iter;
	}
	m_tests.clear();
}

void TestCollection::Add(ITest* test)
{
	m_tests.push_back(test);
}

const TestResults& TestCollection::Execute()
{
    m_results.clear();
    m_successes = 0;
    m_failures = 0;
    m_methods = 0;

	vector<ITest*>::const_iterator iter = m_tests.begin();
	for( ; iter != m_tests.end(); iter++)
	{
		(*iter)->Initialize();
		TestResult& result = (TestResult&)(*iter)->Execute();
        m_results.push_back(&result);
		(*iter)->CleanUp();
	}

    Summarize();

    return m_results;
}

void TestCollection::Summarize()
{
    TestConstIterator iterCount = m_results.cbegin();
    for( ; iterCount != m_results.cend() ; iterCount++)
    {
        TestResult::MethodConstIterator mIter = (*iterCount)->GetResults().cbegin();
        for( ; mIter != (*iterCount)->GetResults().cend() ; mIter++)
        {
            if( (*mIter)->IsSuccess() )
                m_successes++;
            else
                m_failures++;
            m_methods++;
        }
    }
}

void TestCollection::Log(ostream& out)
{
    out << "<?xml version='1.0' encoding='utf-8' ?>" << endl;
    out << "<UnitTest>" << endl;

    out << "<Summary>" << endl;
    out << "<Classes>" << m_tests.size() << "</Classes>" << endl;
    out << "<Methods>" << m_methods << "</Methods>" << endl;
    out << "<Success>" << m_successes << "</Success>" << endl;
    out << "<Failure>" << m_failures << "</Failure>" << endl;
    out << "</Summary>" << endl;
    out << "<Tests>" << endl;
    TestConstIterator iter = m_results.cbegin();
    for( ; iter != m_results.cend() ; iter++)
        (*iter)->Log(out);
    out << "</Tests>" << endl;
    out << "</UnitTest>" << endl;
}

void TestCollection::Log( int argc, char* argv[])
{
    if( argc == 2 )
    {
        std::fstream output;
        output.open(argv[1],fstream::out);
        Log(output);
        output.flush();
        output.close();
    }
    else
    {
        Log(cout);
        cout.flush();
    }
}

}
