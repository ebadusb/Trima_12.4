#include "BaseResultGenerator.h"

using namespace UnitTest;

bool BaseResultGenerator::Generate(int argc, char* argv[])
{
	//donor, config, cobe_config, features
	if(argc != 5 )
		return false;

	BaseResultGenerator generator;
	generator.donor = std::string(argv[1]);

	return true;
}
