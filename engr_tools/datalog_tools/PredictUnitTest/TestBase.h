/*
 * TestBase.h
 *
 *  Created on: May 19, 2012
 *      Author: allan
 */

#ifndef TESTBASE_H_
#define TESTBASE_H_
#include "TestResult.h"
#include "Assert.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <typeinfo>
#include <ctime>
#include <map>
#include <typeinfo>

namespace UnitTest {
using namespace std;

/*
 * This is the abstract base class of the every test. This is necessary
 * in order for the TestCollection class to iterate to a collections of
 * typed tests defined by the template TestBase.
 */
class ITest
{
public:
	ITest(){}
	virtual ~ITest(){}
	virtual const TestResult& Execute() = 0;
	virtual void Initialize() = 0;
	virtual void CleanUp() = 0;
};

/*
 * This is the template class for every test that this framework will recognize.
 */
template <typename T>
class TestBase : public ITest {
public:
    class MethodProperty;

	typedef void (T::*MethodPtr)();
    typedef typename vector<MethodProperty*>::const_iterator methodIter;
protected:
	string m_name;
    vector<MethodProperty*> m_methods;
	MethodProperty* m_currentlyExecuting;
    TestResult m_result;
public:
	TestBase(string testName);
	virtual ~TestBase();

	virtual void Initialize();
	virtual void CleanUp();
	virtual const TestResult& Execute();

    const TestResult& GetResult() const{ return m_result; }
	void AddTestMethod( MethodPtr ptr, string methodName );
    void AddTestMethod( MethodPtr ptr, string methodName, void* parameter );

    void* GetParameter()const;
    /* Assert Methods */
    AssertResult* AssertIsTrue( bool data, string message);
	AssertResult* AssertIsFalse( bool data, string message );
   
	AssertResult* AssertIsNotNull(void* data, string message);
	AssertResult* AssertIsNull(void* data, string message);

	template <typename P>
	AssertResult* AssertAreEqual( P& expected, P& actual, string message);
	AssertResult* AssertAreEqual( T& expected, T& actual, string message);

	template <typename P>
	AssertResult* AssertAreNotEqual(P& expected, P& actual, string message);

    ///this encapsulates the representation
    ///of method to test
	class MethodProperty
	{
	private:
		MethodPtr m_ptr;
		MethodResult m_result;
		string m_name;
        void* m_parameter;

	public:
		MethodProperty(MethodPtr ptr, string name) : m_ptr(ptr), m_result(name), m_name(name), m_parameter(NULL){}
		MethodProperty(MethodPtr ptr, string name, void* parameter) : m_ptr(ptr), m_result(name), m_name(name), m_parameter(parameter){}
		virtual ~MethodProperty();

		const string& GetMethodName() const { return m_name; }
		const MethodPtr& GetMethodPtr(){ return m_ptr;}
        MethodResult& GetResult(){ return m_result; }
        void  AddAssertResult(AssertResult& result){ m_result.AddAssertResult(result);}
        void  SetResultMessage( string message ){ m_result.SetMessage(message);}
        void* GetParameter() const { return m_parameter; }
	};

};

template <typename T>
TestBase<T>::TestBase(string testName) :
	m_name(testName),
    m_result(testName)
{
}

template <typename T>
TestBase<T>::~TestBase() {
	methodIter i = m_methods.begin();
	for( ; i != m_methods.end() ; i++)
		delete (*i);
}

template <typename T>
void TestBase<T>::Initialize()
{
   m_result.Clear();
}

template <typename T>
void TestBase<T>::CleanUp()
{
	m_currentlyExecuting = NULL;
}

template <typename T>
const TestResult& TestBase<T>::Execute()
{
	T* athis = (T*)this;
	methodIter i = m_methods.begin();
	for( ; i != m_methods.end(); i++)
	{
		int start = clock();
		MethodProperty* method = m_currentlyExecuting = (*i);
		const MethodPtr& m = method->GetMethodPtr();
		try{
			(athis->*m)();
		}
		catch(exception e){
			string message("Exception thrown: "); message.append(e.what());
			method->SetResultMessage(message);
		}

        int end = clock();
        double diff = (double)(end - start)/CLOCKS_PER_SEC;
        method->GetResult().SetTime(diff);
        m_result.AddMethodResult( method->GetResult() );
	}

    return m_result;
}

template <typename T>
void TestBase<T>::AddTestMethod(const MethodPtr ptr, string testMethodName)
{
	MethodProperty* method = new MethodProperty(ptr, testMethodName);
    m_methods.push_back(method);
}

template <typename T>
void TestBase<T>::AddTestMethod(const MethodPtr ptr, string testMethodName, void* parameter)
{
	MethodProperty* method = new MethodProperty(ptr, testMethodName, parameter);
    m_methods.push_back(method);
}

template <typename T>
void* TestBase<T>::GetParameter() const
{
    if( m_currentlyExecuting != NULL )
        return m_currentlyExecuting->GetParameter();
	return NULL;
}

template <typename T>
template <typename P>
AssertResult*  TestBase<T>::AssertAreEqual(P& expected, P& actual, string message)
{
	AssertResult* result = Assert<P>::AreEqual(expected, actual, message);
	if( m_currentlyExecuting != NULL )
	{
		m_currentlyExecuting->AddAssertResult(*result);
	}
	return result;
}

template <typename T>
AssertResult*  TestBase<T>::AssertAreEqual(T& expected, T& actual, string message)
{
	AssertResult* result = Assert<const T>::AreEqual(expected, actual, message);
	if( m_currentlyExecuting != NULL )
	{
		m_currentlyExecuting->AddAssertResult(*result);
	}
	return result;
}

template <typename T>
template <typename P>
AssertResult*  TestBase<T>::AssertAreNotEqual(P& expected, P& actual, string message)
{
	AssertResult* result = Assert<P>::AreNotEqual(expected, actual, message);
	if( m_currentlyExecuting != NULL )
	{
		m_currentlyExecuting->AddAssertResult(*result);
	}
	return result;
}

template <typename T>
AssertResult*  TestBase<T>::AssertIsTrue( bool data, string message)
{
	AssertResult* result = Assert<T>::IsTrue(data, message);
	if( m_currentlyExecuting != NULL )
	{
        m_currentlyExecuting->AddAssertResult(*result);
	}
	return result;
}

template <typename T>
AssertResult*  TestBase<T>::AssertIsFalse( bool data, string message)
{
	AssertResult* result = Assert<bool>::IsFalse(data, message);
	if( m_currentlyExecuting != NULL )
	{
		m_currentlyExecuting->AddAssertResult(*result);
	}
	return result;
}

template <typename T>
AssertResult*  TestBase<T>::AssertIsNotNull( void* data, string message)
{
    AssertResult* result = Assert<T*>::IsNotNull(data, message);
	if( m_currentlyExecuting != NULL )
	{
		m_currentlyExecuting->AddAssertResult(*result);
	}
	return result;
}

template <typename T>
AssertResult*  TestBase<T>::AssertIsNull( void* data, string message)
{
    AssertResult* result = Assert<void*>::IsNull(data, message);
	if( m_currentlyExecuting != NULL )
	{
		m_currentlyExecuting->AddAssertResult(*result);
	}
	return result;
}

template <typename T>
TestBase<T>::MethodProperty::~MethodProperty()
{
    if( m_parameter != NULL )
        delete m_parameter;
    m_parameter = NULL;
}
}//namespace


#endif