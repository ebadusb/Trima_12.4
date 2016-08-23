/*
Purpose: Provide some logging functionality needed by our script logging mechanism,
         like timestamps.
*/
#ifndef TestLogger_HPP // prevents multiple inclusion
#define TestLogger_HPP
// libtools includes
#include "logger.hpp"

#define TEST_LOG TestLogger::Log_Test_Info
#define VERIFY TestLogger::Log_Verify

class TestLogger : Logger
{
    public:
        TestLogger();
        static void Init(const char* programName){ Logger::Init(programName); };
        static void LogToStdout() { Logger::LogToStdout(); }; 
        static void Log_Test_Info(char* format,...);  // Prefixes with timestamp and wraps in <test_data> tag
        static void Log_Verify(char* format,...); // Wraps in <verify> tag
};
#endif // TestLogger_HPP
