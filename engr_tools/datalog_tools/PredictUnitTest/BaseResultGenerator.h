#ifndef _BASE_RESULT_GENERATOR_H_
#define _BASE_RESULT_GENERATOR_H_

#include <string>

namespace UnitTest
{
	class BaseResultGenerator
	{
	public:
		static bool Generate(int argc, char* argv[]);

	private:
		std::string donor;
		std::string config;
		std::string cobeconfig;
		std::string features;
	};
}

#endif