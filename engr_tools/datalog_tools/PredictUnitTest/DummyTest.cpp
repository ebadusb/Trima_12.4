
#include "DummyTest.h"

DummyTest::DummyTest(void) : TestBase<DummyTest>("DummyTest")
{
}


DummyTest::~DummyTest(void)
{
}


void DummyTest::AddIntTest()
{
    int actual = Calculator<int>::Add( 10, 12);
    int expected = 22;
    AssertAreEqual<int>(expected, actual, "Result not equal.");
}

void DummyTest::AddDoubleTest()
{
    double actual = Calculator<double>::Add( 10.5f, 12.5f);
    double expected = 23.5f;
    AssertAreEqual<double>(expected, actual, "Result not equal.");
}
