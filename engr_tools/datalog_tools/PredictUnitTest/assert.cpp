#include "Assert.h"

#include "stdio.h"

namespace UnitTest {


AssertResult* Assert<int>::AreEqual( const int& expected, const int& actual, string message )
{
	AssertResult* result = new AssertResult(expected == actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + 100];
        sprintf(abuff,"%s expected: %d, actual: %d", message.c_str(), expected, actual);
        result->SetMessage(string(abuff));
		if( expected == 0 )
			result->SetDiffPercent( 0x8FFFFFFF );
		else
			result->SetDiffPercent( (actual - expected)*100/expected );
        delete abuff;
    }
	return result;
}

AssertResult* Assert<int>::AreNotEqual( const int& expected, const int& actual, string message )
{
	AssertResult* result = new AssertResult(expected != actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + 100];
        sprintf(abuff,"%s expected: %d, actual: %d", message.c_str(), expected, actual);
        result->SetMessage(string(abuff));
		if( expected == 0 )
			result->SetDiffPercent( 0x8FFFFFFF );
		else
			result->SetDiffPercent( (actual - expected)*100/expected );
        delete abuff;
    }
	return result;
}

AssertResult* Assert<const int>::AreEqual( const int& expected, const int& actual, string message )
{
	AssertResult* result = new AssertResult(expected == actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + 100];
        sprintf(abuff,"%s expected: %d, actual: %d", message.c_str(), expected, actual);
        result->SetMessage(string(abuff));
		if( expected == 0 )
			result->SetDiffPercent( 0x8FFFFFFF );
		else
			result->SetDiffPercent( (actual - expected)*100/expected );
        delete abuff;
    }
	return result;
}

AssertResult* Assert<const int>::AreNotEqual( const int& expected, const int& actual, string message )
{
	AssertResult* result = new AssertResult(expected != actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + 100];
        sprintf(abuff,"%s expected: %d, actual: %d", message.c_str(), expected, actual);
        result->SetMessage(string(abuff));
        delete abuff;
    }
	return result;
}

AssertResult* Assert<const float>::AreEqual( const float& expected, const float& actual, string message )
{
    int intExpected = (int)(expected * 100 + 0.5);
    int intActual = (int)(actual * 100 + 0.5);
	AssertResult* result = new AssertResult(intExpected == intActual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + 100];
        sprintf(abuff,"%s expected: %5.2f, actual: %5.2f", message.c_str(), expected, actual);
        result->SetMessage(string(abuff));
        		if( expected == 0 )
			result->SetDiffPercent( 0x8FFFFFFF );
		else
			result->SetDiffPercent( (actual - expected)*100/expected );
delete abuff;
    }
	return result;
}

AssertResult* Assert<const float>::AreNotEqual( const float& expected, const float& actual, string message )
{
	AssertResult* result = new AssertResult(expected != actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + 100];
        sprintf(abuff,"%s expected: %5.2f, actual: %5.2f", message.c_str(), expected, actual);
        result->SetMessage(string(abuff));
		if( expected == 0 )
			result->SetDiffPercent( 0x8FFFFFFF );
		else
			result->SetDiffPercent( (actual - expected)*100/expected );
        delete abuff;
    }
	return result;
}

AssertResult* Assert<float>::AreEqual( const float& expected, const float& actual, string message )
{
	AssertResult* result = new AssertResult(expected == actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + 100];
        sprintf(abuff,"%s expected: %5.2f, actual: %5.2f", message.c_str(), expected, actual);
        result->SetMessage(string(abuff));
		if( expected == 0 )
			result->SetDiffPercent( 0x8FFFFFFF );
		else
			result->SetDiffPercent( (actual - expected)*100/expected );
        delete abuff;
    }
	return result;
}

AssertResult* Assert<float>::AreNotEqual( const float& expected, const float& actual, string message )
{
	AssertResult* result = new AssertResult(expected != actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + 100];
        sprintf(abuff,"%s expected: %5.10f, actual: %5.10f", message.c_str(), expected, actual);
        result->SetMessage(string(abuff));
        delete abuff;
    }
	return result;
}

AssertResult* Assert<double>::AreEqual( const double& expected, const double& actual, string message )
{
	AssertResult* result = new AssertResult(expected == actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + 100];
        sprintf(abuff,"%s expected: %5.2f, actual: %5.2f", message.c_str(), expected, actual);
        result->SetMessage(string(abuff));
		if( expected == 0 )
			result->SetDiffPercent( 0x8FFFFFFF );
		else
			result->SetDiffPercent( (actual - expected)*100/expected );
        delete abuff;
    }
	return result;
}

AssertResult* Assert<double>::AreNotEqual( const double& expected, const double& actual, string message )
{
	AssertResult* result = new AssertResult(expected != actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + 100];
        sprintf(abuff,"%s expected: %5.2f, actual: %5.2f", message.c_str(), expected, actual);
        result->SetMessage(string(abuff));
        delete abuff;
    }
	return result;
}

AssertResult* Assert<const double>::AreEqual( const double& expected, const double& actual, string message )
{
	AssertResult* result = new AssertResult(expected == actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + 100];
        sprintf(abuff,"%s expected: %5.2f, actual: %5.2f", message.c_str(), expected, actual);
        result->SetMessage(string(abuff));
		if( expected == 0 )
			result->SetDiffPercent( 0x8FFFFFFF );
		else
			result->SetDiffPercent( (actual - expected)*100/expected );
        delete abuff;
    }
	return result;
}

AssertResult* Assert<const double>::AreNotEqual( const double& expected, const double& actual, string message )
{
	AssertResult* result = new AssertResult(expected != actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + 100];
        sprintf(abuff,"%s expected: %5.2f, actual: %5.ff", message.c_str(), expected, actual);
        result->SetMessage(string(abuff));
        delete abuff;
    }
	return result;
}

AssertResult* Assert<string>::AreEqual( const string& expected, const string& actual, string message )
{
	AssertResult* result = new AssertResult(expected == actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + expected.length() + actual.length() + 100];
        sprintf(abuff,"%s expected: %s, actual: %s", message.c_str(), expected.c_str(), actual.c_str());
        result->SetMessage(string(abuff));
        delete abuff;
    }
	return result;
}

AssertResult* Assert<string>::AreNotEqual( const string& expected, const string& actual, string message )
{
	AssertResult* result = new AssertResult(expected != actual);
    if( !result->IsSuccess() )
    {
        char* abuff = new char[message.length() + expected.length() + actual.length() + 100];
        sprintf(abuff,"%s expected: %s, actual: %s", message.c_str(), expected.c_str(), actual.c_str());
        result->SetMessage(string(abuff));
        delete abuff;
    }
	return result;
}
}