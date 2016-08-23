/*****************************************************************************\
**
** ManagedPredictBuildProcess.h
**
** Provides the implementation for the automatic code generation.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <fstream>
#include <sstream>
#include <iostream>

#include "ManagedPredictBuildProcess.h"

/* Constants *****************************************************************/

static const char* TAG_FUNCTION = "<function>";
static const char* TAG_NAME = "<name>";
static const char* TAG_TYPE = "<type>";
static const char* TAG_GENERATE = "#pragma region Generated Funstions";

/* Functions *****************************************************************/

void ReplaceTags(std::string &text, const std::string &tag, const std::string &value)
{
	std::string::size_type iter = text.find(tag);
	std::string::size_type length = tag.length();

	while (iter != std::string::npos) {
		text.replace(iter, length, value);
		iter = text.find(tag);
	}
}

std::string GetMemberName(const std::string &line)
{
	std::string::size_type end = line.find_last_of(';');
	std::string::size_type begin = line.find_last_of(' ', end) + 1;
	
	return line.substr(begin, end - begin);
}

std::string GetMemberType(const std::string &line)
{
	std::string::size_type end = line.find_last_of('>');
	std::string::size_type begin = line.find_last_of(" ,<", end) + 1;
	
	return line.substr(begin, end - begin);
}

std::string GetFunctionName(const std::string &name, const char *prefix)
{
	return prefix + name;
}

std::string GetFunctionDeclaration(const std::string &function, const std::string &member,
		const std::string &type, std::string format)
{
	ReplaceTags(format, TAG_FUNCTION, function);
	ReplaceTags(format, TAG_TYPE, type);
	ReplaceTags(format, TAG_NAME, member);

	return format;
}

int main(int argc, const char* argv[])
{
	if (argc != 2) {
		return 1;
	}
	
	std::string folder = std::string(argv[1]) + "\\";
	std::ifstream predictSourceStream((folder + FILE_PREDICT_SOURCE).c_str());
	std::ifstream sourceTemplateStream((folder + FILE_SOURCE_TEMPLATE).c_str());
	std::ofstream sourceGeneratedStream((folder + FILE_SOURCE_PRODUCT).c_str());

	std::string line;
	std::string member;
	std::string type;
	std::string function;
	std::string declaration;

	while(line.find(TAG_GENERATE) == std::string::npos && std::getline(sourceTemplateStream, line)) {
		sourceGeneratedStream << line << std::endl;
	}

	while(std::getline(predictSourceStream, line)) {
		if (line.find(PREDICT_FUNCTIONS_BEGIN) != std::string::npos) {
			while(std::getline(predictSourceStream, line) && line.find(PREDICT_FUNCTIONS_END) == std::string::npos) {
				if (line.find_last_of(';') != std::string::npos) {
					member = GetMemberName(line);
					type = GetMemberType(line);
					function = GetFunctionName(member, FUNCTION_PREDICT_GET_PREFIX);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_PROCEDURE_GET_TEMPLATE);
					sourceGeneratedStream << declaration << std::endl;
				}
			}
		}

		if (line.find(PROCEDURE_FUNCTIONS_BEGIN) != std::string::npos) {
			while(std::getline(predictSourceStream, line) && line.find(PROCEDURE_FUNCTIONS_END) == std::string::npos) {
				if (line.find_last_of(';') != std::string::npos) {
					member = GetMemberName(line);
					type = GetMemberType(line);
					function = GetFunctionName(member, FUNCTION_PROCEDURE_GET_PREFIX);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_PROCEDURE_GET_TEMPLATE);
					sourceGeneratedStream << declaration << std::endl;
				}
			}
		}

		if (line.find(CONFIG_FUNCTIONS_BEGIN) != std::string::npos) {
			while(std::getline(predictSourceStream, line) && line.find(CONFIG_FUNCTIONS_END) == std::string::npos) {
				if (line.find_last_of(';') != std::string::npos) {
					member = GetMemberName(line);
					type = GetMemberType(line);
					function = GetFunctionName(member, FUNCTION_CONFIG_GET_PREFIX);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_CONFIG_GET_TEMPLATE);
					sourceGeneratedStream << declaration << std::endl;
					function = GetFunctionName(member, FUNCTION_CONFIG_SET_PREFIX);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_CONFIG_SET_TEMPLATE);
					sourceGeneratedStream << declaration << std::endl;
				}
			}
		}

		if (line.find(CONFIGARRAY_FUNCTIONS_BEGIN) != std::string::npos) {
			while(std::getline(predictSourceStream, line) && line.find(CONFIGARRAY_FUNCTIONS_END) == std::string::npos) {
				if (line.find_last_of(';') != std::string::npos) {
					member = GetMemberName(line);
					type = GetMemberType(line);
					function = GetFunctionName(member, FUNCTION_CONFIG_GET_PREFIX);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_CONFIGARRAY_GET_TEMPLATE);
					sourceGeneratedStream << declaration << std::endl;
					function = GetFunctionName(member, FUNCTION_CONFIG_SET_PREFIX);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_CONFIGARRAY_SET_TEMPLATE);
					sourceGeneratedStream << declaration << std::endl;
				}
			}
		}

		if (line.find(SUBSTATE_FUNCTIONS_BEGIN) != std::string::npos) {
			while(std::getline(predictSourceStream, line) && line.find(SUBSTATE_FUNCTIONS_END) == std::string::npos) {
				if (line.find_last_of(';') != std::string::npos) {
					member = GetMemberName(line);
					type = GetMemberType(line);
					function = GetFunctionName(member, FUNCTION_SUBSTATE_GET_PREFIX);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_SUBSTATE_GET_TEMPLATE);
					sourceGeneratedStream << declaration << std::endl;
				}
			}
		}

		if (line.find(CONDITION_FUNCTIONS_BEGIN) != std::string::npos) {
			while(std::getline(predictSourceStream, line) && line.find(CONDITION_FUNCTIONS_END) == std::string::npos) {
				if (line.find_last_of(';') != std::string::npos) {
					member = GetMemberName(line);
					type = GetMemberType(line);
					function = GetFunctionName(member, FUNCTION_CONDITION_GET_PREFIX);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_CONDITION_GET_TEMPLATE);
					sourceGeneratedStream << declaration << std::endl;
					function = GetFunctionName(member, FUNCTION_CONDITION_SET_PREFIX);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_CONDITION_SET_TEMPLATE);
					sourceGeneratedStream << declaration << std::endl;
				}
			}
		}
	}

	while(std::getline(sourceTemplateStream, line)) {
		sourceGeneratedStream << line << std::endl;
	}

	predictSourceStream.close();
	sourceTemplateStream.close();
	sourceGeneratedStream.close();

	return 0;
}


