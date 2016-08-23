/*
 * Assert.h
 * 
 * This is the Assert class definition. 
 * This is a helper class that encapsulates
 * the methods of asserting various types.
 *
 *  Created on: May 19, 2012
 *      Author: Allan Gallano
 */

#ifndef ASSERT_H_
#define ASSERT_H_
#include "TestResult.h"
namespace UnitTest {

int const MAX_MESSAGE_LENGTH = 2000;

template <typename T>
class Assert {
public:
	Assert(){}
	virtual ~Assert(){}

	static AssertResult* AreEqual(const T& expected, const T& actual, string message );

	static AssertResult* AreNotEqual(const T& expected, const T& actual, string message );

	static AssertResult* IsNotNull(void* data, string message );
	static AssertResult* IsNull(void* data, string message );
	static AssertResult* IsTrue(bool data, string message );
	static AssertResult* IsFalse(bool data, string message );
};

template <>
class Assert<int>
{
public:
	static AssertResult* AreEqual(const int& expected, const int& actual, string message );
	static AssertResult* AreNotEqual(const int& expected, const int& actual, string message );
};

template <>
class Assert<const int>
{
public:
	static AssertResult* AreEqual(const int& expected, const int& actual, string message );
	static AssertResult* AreNotEqual(const int& expected, const int& actual, string message );
};

template <>
class Assert<double>
{
public:
	static AssertResult* AreEqual(const double& expected, const double& actual, string message );
	static AssertResult* AreNotEqual(const double& expected, const double& actual, string message );
};

template <>
class Assert<const double>
{
public:
	static AssertResult* AreEqual(const double& expected, const double& actual, string message );
	static AssertResult* AreNotEqual(const double& expected, const double& actual, string message );
};

template <>
class Assert<float>
{
public:
	static AssertResult* AreEqual(const float& expected, const float& actual, string message );
	static AssertResult* AreNotEqual(const float& expected, const float& actual, string message );
};

template <>
class Assert<const float>
{
public:
	static AssertResult* AreEqual(const float& expected, const float& actual, string message );
	static AssertResult* AreNotEqual(const float& expected, const float& actual, string message );
};

template <>
class Assert<string>
{
public:
	static AssertResult* AreEqual(const string& expected, const string& actual, string message );
	static AssertResult* AreNotEqual(const string& expected, const string& actual, string message );
};

template <typename T>
AssertResult* Assert<T>::AreEqual(const T& expected, const T& actual, string message )
{
	AssertResult* result = new AssertResult(expected == actual);
    if( !result->IsSuccess() )
        result->SetMessage(message);
	return result;
}

template <typename T>
AssertResult* Assert<T>::AreNotEqual(const T& expected, const T& actual, string message )
{
	AssertResult* result = new AssertResult(expected != actual);
    if( !result->IsSuccess() )
        result->SetMessage(message);
	return result;
}

template <typename T>
AssertResult* Assert<T>::IsNotNull(void* data, string message )
{
	AssertResult* result = new AssertResult(data != NULL);
    if( !result->IsSuccess() )
        result->SetMessage(message);
	return result;
}

template <typename T>
AssertResult* Assert<T>::IsNull(void* data, string message )
{
	AssertResult* result = new AssertResult(data == NULL);
    if( !result->IsSuccess() )
        result->SetMessage(message);
	return result;
}

template <typename T>
AssertResult* Assert<T>::IsTrue(bool data, string message )
{
	AssertResult* result = new AssertResult(data == true);
    if( !result->IsSuccess() )
        result->SetMessage(message);
	return result;
}

template <typename T>
AssertResult* Assert<T>::IsFalse(bool data, string message )
{
	return IsTrue( !data, message);
}

}

#endif /* ASSERT_H_ */
