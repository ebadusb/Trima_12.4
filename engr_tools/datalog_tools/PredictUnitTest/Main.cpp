#include <string>
#include <cstdio>
#include <iostream>

#include "Assert.h"
#include "TestResult.h"
#include "DummyTest.h"
#include "TestCollection.h"
#include "PIRTest.h"
#include "SubstateTest.h"
#include "BaseResultGenerator.h"
#include "OldVersionCompareTest.h"
#include "WBCPurgeTimeTest.h"

using namespace UnitTest;
using namespace std;

int main(int argc, char* argv[])
{
	if(	BaseResultGenerator::Generate(argc, argv) )
		return 0;

    TestCollection coll;
	coll.Add( new WBCPurgeTimeTest());
    //coll.Add( new DummyTest());
	//coll.Add( new OldVersionCompareTest(1));
    coll.Add( new SubstateTest());
    coll.Add( new PIRTest(1));
    coll.Add( new PIRTest(2));
    coll.Add( new PIRTest(3));

	coll.Execute();
    coll.Log(argc, argv);

    return coll.GetFailures();
}
