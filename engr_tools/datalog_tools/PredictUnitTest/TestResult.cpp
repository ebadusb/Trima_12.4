/*
 * TestResult.cpp
 *
 *  Created on: May 19, 2012
 *      Author: allan
 */

#include "TestResult.h"

namespace UnitTest {


void Result::Log( ostream& out )
{
}

void AssertResult::Log( ostream& out )
{
    if( IsSuccess() )
        return;
    out << "<Assert>" << endl;
	out << "<Message>" << this->m_message << "</Message>" << endl;
	if( diffPercent != 0 )
	{
		out << "<Diff>";
        char* abuff = new char[100];
        sprintf(abuff,"%0.2f", diffPercent);
		out << abuff;
		out << "</Diff>" << endl;
		delete abuff;
	}
	out << "</Assert>" << endl;
}

MethodResult::~MethodResult()
{
    AssertConstIterator iter = m_assertResults.begin();
    for( ; iter != m_assertResults.end(); iter++)
        delete(*iter);
    m_assertResults.clear();
}

void MethodResult::AddAssertResult( AssertResult& result )
{
    m_success &= result.IsSuccess();
    m_assertResults.push_back(&result);
}

void MethodResult::Log( ostream& out )
{
    out << "<Method>" << endl;
    out << "<Name>" << this->m_methodName << "</Name>" << endl;
    string strResult = IsSuccess() ? "Success" : "Failed";
    out << "<Result>" << strResult << "</Result>" << endl;
    out << "<Time>" << this->m_time << "</Time>" << endl;
	AssertConstIterator iter = m_assertResults.cbegin();
	if( iter  != m_assertResults.cend() )
	{
	    out << "<Asserts>" << endl;
		for( ; iter != m_assertResults.cend() ; iter++)
			(*iter)->Log(out);
	    out << "</Asserts>" << endl;
	}
    out << "</Method>" << endl;
}

TestResult::~TestResult() 
{
	m_methodResults.clear();
}

void TestResult::DeleteResults()
{
	MethodConstIterator iter = m_methodResults.begin();
	for( ; iter != m_methodResults.end(); iter++)
		delete (*iter);
	m_methodResults.clear();
}

const string& TestResult::GetTestName() const
{
	return m_testName;
}

void TestResult::AddMethodResult(MethodResult& result)
{
    m_success &= result.IsSuccess();
	m_methodResults.push_back(&result);
}

void TestResult::Clear()
{
    m_success = true;
    m_methodResults.clear();
}

void TestResult::Log( ostream& out )
{
    out << "<Class>" << endl;
    out << "<Name>" << this->m_testName << "</Name>" << endl;
    MethodConstIterator iter = m_methodResults.cbegin();
    for( ; iter != m_methodResults.cend() ; iter++)
        (*iter)->Log(out);
    out << "</Class>" << endl;
}

}
