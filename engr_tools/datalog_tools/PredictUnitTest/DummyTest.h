#ifndef DUMMY_TEST_H
#define DUMMY_TEST_H

#include "TestBase.h"

using namespace UnitTest;

class DummyTest :
	public TestBase<DummyTest>
{
protected:
	void setName();
public:
	DummyTest(void);
	virtual ~DummyTest(void);

	virtual void Initialize()
	{
		this->AddTestMethod( &DummyTest::AddIntTest, "AddIntTest");
		this->AddTestMethod( &DummyTest::AddDoubleTest, "AddDoubleTest");
	}

	/*
	* test methods
	*/
	void AddIntTest();
	void AddDoubleTest();
};


template< typename T>
class Calculator
{
public:
    static T Add( T a, T b);
};

template<typename T>
T Calculator<T>::Add( T a, T b)
{
    return a + b;
}

#endif
