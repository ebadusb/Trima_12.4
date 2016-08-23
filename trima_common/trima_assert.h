/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      trima_assert - used to test an expression and shut system down.
 *
 * ABSTRACT:   This is intended to replace assert() and ASSERT() in trima code.
 *
 */

#ifndef TRIMA_ASSERT_H
#define TRIMA_ASSERT_H

/*
 * COMPILE_TIME_ASSERT() creates a compile-time error if X is false.
 * MSG must be a single alphanumeric word (no quotes/punctuation).
 * It may be placed anywhere inside or outside of a code block,
 * or even in a header file. Example:
 *
 * COMPILE_TIME_ASSERT(1+1==3, BadMath); --> "size of array `StaticAssertion_BadMath' is negative"
 *
 */
#define COMPILE_TIME_ASSERT(X, MSG)      typedef char StaticAssertion_##MSG[(!!(X)) * 2 - 1]

// Token pasting helpers; not intended for use in code
#define STATIC_ASSERT_HELPER1b(X, L)     COMPILE_TIME_ASSERT(X, AtLine##L)
#define STATIC_ASSERT_HELPER1a(X, L)     STATIC_ASSERT_HELPER1b(X, L)
#define STATIC_ASSERT_HELPER2b(X, M, L)  COMPILE_TIME_ASSERT(X, AtLine##L##_##M)
#define STATIC_ASSERT_HELPER2a(X, M, L)  STATIC_ASSERT_HELPER2b(X, M, L)

// Compile-time assertion with line number info provided as the message
#define TRIMA_STATIC_ASSERT(X)           STATIC_ASSERT_HELPER1a(X, __LINE__)

// Compile-time assertion with a message prepended after the line number info
#define TRIMA_STATIC_ASSERT2(X, MSG)     STATIC_ASSERT_HELPER2a(X, MSG, __LINE__)

/*
 * Make sure assert functions are usable from both C and C++
 */
#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

extern void trima_assert_failed (const char*, const char*, int);

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#define trima_assert(expr)  ((expr) ? (void)0 : trima_assert_failed(#expr, __FILE__, __LINE__))
#define TRIMA_ASSERT(expr)  ((expr) ? (void)0 : trima_assert_failed(#expr, __FILE__, __LINE__))

#endif /*TRIMA_ASSERT_H*/

/* FORMAT HASH f5530f22f9f51d7f2841fe119995dc30 */
