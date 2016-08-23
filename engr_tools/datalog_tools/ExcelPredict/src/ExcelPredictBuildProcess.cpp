/*****************************************************************************\
**
** ExcelPredictBuildProcess.h
**
** Provides the implementation for the automatic code generation.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <fstream>
#include <sstream>
#include <iostream>

#include "ExcelPredictBuildProcess.h"

/* Constants *****************************************************************/

static const char* TAG_FUNCTION = "<function>";
static const char* TAG_NAME = "<name>";
static const char* TAG_TYPE = "<type>";
static const char* TAG_ALIAS = "<alias>";
static const char* TAG_ARRAY = "kFunctionList";

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

std::string ConvertExcelType(std::string type)
{
	if (type == "float") {
		type = "B";
	}
	else if (type == "bool") {
		type = "A";
	}
	else if (type == "int") {
		type = "J";
	}
	else if (type == "time_t") {
		type = "J";
	}

	return type;
}

std::string ConvertVBAType(std::string type)
{
	if (type == "float") {
		type = "Single";
	}
	else if (type == "bool") {
		type = "Boolean";
	}
	else if (type == "int") {
		type = "Long";
	}
	else if (type == "time_t") {
		type = "Long";
	}

	return type;
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
	std::string::size_type begin = line.find_last_of(" ,", end) + 1;
	
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

std::string GetFunctionDescription(const std::string &function, const std::string &member,
		const std::string &type, const char* format[10])
{
	std::ostringstream stream;
	std::string temp;

	stream << "{ ";

	stream.setf(std::ios::left);

	temp = format[0];
	ReplaceTags(temp, TAG_FUNCTION, function);
	stream.width(50);
	stream << "L\"" + temp + "\", ";

	temp = format[1];
	ReplaceTags(temp, TAG_TYPE, ConvertExcelType(type));
	stream.width(8);
	stream << "L\"" + temp + "\", ";

	temp = format[2];
	ReplaceTags(temp, TAG_FUNCTION, function);
	stream.width(50);
	stream << "L\"" + temp + "\", ";

	temp = format[3];
	stream.width(20);
	stream << "L\"" + temp + "\", ";

	for (int i = 4; i < 10; ++i) {
		stream << "L\"" << format[i] << "\", ";
	}
	
	stream << "},";

	return stream.str();
}

std::string GetAliasDeclaration(const std::string &function, const std::string &alias,
		const std::string &type, std::string format)
{
	ReplaceTags(format, TAG_FUNCTION, function);
	ReplaceTags(format, TAG_ALIAS, alias);
	ReplaceTags(format, TAG_TYPE, ConvertVBAType(type));
	
	return format;
}

int main(int argc, const char* argv[])
{
	if (argc != 2) {
		return 1;
	}
	
	std::string folder = std::string(argv[1]) + "\\";
	std::ifstream predictSourceStream((folder + FILE_PREDICT_SOURDE).c_str());
	std::ifstream definitionTemplateStream((folder + FILE_DEFINITION_TEMPLATE).c_str());
	std::ofstream definitionGeneratedStream((folder + FILE_DEFINITION_GENERATED).c_str());
	std::ifstream sourceTemplateStream((folder + FILE_SOURCE_TEMPLATE).c_str());
	std::ofstream sourceGeneratedStream((folder + FILE_SOURCE_PRODUCT).c_str());
	std::ifstream descriptionTemplateStream((folder + FILE_DESCRIPTION_TEMPLATE).c_str());
	std::ofstream descriptionGeneratedStream((folder + FILE_DESCRIPTION_PRODUCT).c_str());
	std::ifstream moduleTemplateStream((folder + FILE_MODULE_TEMPLATE).c_str());
	std::ofstream moduleGeneratedStream((folder + FILE_MODULE_PRODUCT).c_str());

	std::string line;
	std::string member;
	std::string type;
	std::string function;
	std::string alias;
	std::string declaration;
	std::string description;
	std::string module;

	while(std::getline(definitionTemplateStream, line)) {
		definitionGeneratedStream << line << std::endl;
	}

	while(std::getline(sourceTemplateStream, line)) {
		sourceGeneratedStream << line << std::endl;
	}

	while(line.find(TAG_ARRAY) == std::string::npos && std::getline(descriptionTemplateStream, line)) {
		descriptionGeneratedStream << line << std::endl;
	}

	while(std::getline(moduleTemplateStream, line)) {
		moduleGeneratedStream << line << std::endl;
	}

	while(std::getline(predictSourceStream, line)) {
		if (line.find(PROCEDURE_FUNCTIONS_BEGIN) != std::string::npos) {
			while(std::getline(predictSourceStream, line) && line.find(PROCEDURE_FUNCTIONS_END) == std::string::npos) {
				if (line.find_last_of(';') != std::string::npos) {
					member = GetMemberName(line);
					type = GetMemberType(line);
					function = GetFunctionName(member, FUNCTION_PROCEDURE_GET_PREFIX);
					alias = GetFunctionName(member, FUNCTION_PROCEDURE_GET_ALIAS);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_PROCEDURE_GET_TEMPLATE);
					description = GetFunctionDescription(function, member, type, FUNCTION_PROCEDURE_GET_DECLARE);
					module = GetAliasDeclaration(function, alias, type, FUNCTION_PROCEDURE_GET_MODULE);
					definitionGeneratedStream << '\t' << function << std::endl;
					sourceGeneratedStream << declaration << std::endl;
					descriptionGeneratedStream << description << std::endl;
					moduleGeneratedStream << module << std::endl;
				}
			}
		}

		if (line.find(CONFIG_FUNCTIONS_BEGIN) != std::string::npos) {
			while(std::getline(predictSourceStream, line) && line.find(CONFIG_FUNCTIONS_END) == std::string::npos) {
				if (line.find_last_of(';') != std::string::npos) {
					member = GetMemberName(line);
					type = GetMemberType(line);
					function = GetFunctionName(member, FUNCTION_CONFIG_GET_PREFIX);
					alias = GetFunctionName(member, FUNCTION_CONFIG_GET_ALIAS);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_CONFIG_GET_TEMPLATE);
					description = GetFunctionDescription(function, member, type, FUNCTION_CONFIG_GET_DECLARE);
					module = GetAliasDeclaration(function, alias, type, FUNCTION_CONFIG_GET_MODULE);
					definitionGeneratedStream << '\t' << function << std::endl;
					sourceGeneratedStream << declaration << std::endl;
					descriptionGeneratedStream << description << std::endl;
					moduleGeneratedStream << module << std::endl;
					function = GetFunctionName(member, FUNCTION_CONFIG_SET_PREFIX);
					alias = GetFunctionName(member, FUNCTION_CONFIG_SET_ALIAS);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_CONFIG_SET_TEMPLATE);
					description = GetFunctionDescription(function, member, type, FUNCTION_CONFIG_SET_DECLARE);
					module = GetAliasDeclaration(function, alias, type, FUNCTION_CONFIG_SET_MODULE);
					definitionGeneratedStream << '\t' << function << std::endl;
					sourceGeneratedStream << declaration << std::endl;
					descriptionGeneratedStream << description << std::endl;
					moduleGeneratedStream << module << std::endl;
				}
			}
		}

		if (line.find(CONFIGARRAY_FUNCTIONS_BEGIN) != std::string::npos) {
			while(std::getline(predictSourceStream, line) && line.find(CONFIGARRAY_FUNCTIONS_END) == std::string::npos) {
				if (line.find_last_of(';') != std::string::npos) {
					member = GetMemberName(line);
					type = GetMemberType(line);
					function = GetFunctionName(member, FUNCTION_CONFIG_GET_PREFIX);
					alias = GetFunctionName(member, FUNCTION_CONFIG_GET_ALIAS);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_CONFIGARRAY_GET_TEMPLATE);
					description = GetFunctionDescription(function, member, type, FUNCTION_CONFIGARRAY_GET_DECLARE);
					module = GetAliasDeclaration(function, alias, type, FUNCTION_CONFIGARRAY_GET_MODULE);
					definitionGeneratedStream << '\t' << function << std::endl;
					sourceGeneratedStream << declaration << std::endl;
					descriptionGeneratedStream << description << std::endl;
					moduleGeneratedStream << module << std::endl;
					function = GetFunctionName(member, FUNCTION_CONFIG_SET_PREFIX);
					alias = GetFunctionName(member, FUNCTION_CONFIG_SET_ALIAS);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_CONFIGARRAY_SET_TEMPLATE);
					description = GetFunctionDescription(function, member, type, FUNCTION_CONFIGARRAY_SET_DECLARE);
					module = GetAliasDeclaration(function, alias, type, FUNCTION_CONFIGARRAY_SET_MODULE);
					definitionGeneratedStream << '\t' << function << std::endl;
					sourceGeneratedStream << declaration << std::endl;
					descriptionGeneratedStream << description << std::endl;
					moduleGeneratedStream << module << std::endl;
				}
			}
		}

		if (line.find(SUBSTATE_FUNCTIONS_BEGIN) != std::string::npos) {
			while(std::getline(predictSourceStream, line) && line.find(SUBSTATE_FUNCTIONS_END) == std::string::npos) {
				if (line.find_last_of(';') != std::string::npos) {
					member = GetMemberName(line);
					type = GetMemberType(line);
					function = GetFunctionName(member, FUNCTION_SUBSTATE_GET_PREFIX);
					alias = GetFunctionName(member, FUNCTION_SUBSTATE_GET_ALIAS);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_SUBSTATE_GET_TEMPLATE);
					description = GetFunctionDescription(function, member, type, FUNCTION_SUBSTATE_GET_DECLARE);
					module = GetAliasDeclaration(function, alias, type, FUNCTION_SUBSTATE_GET_MODULE);
					definitionGeneratedStream << '\t' << function << std::endl;
					sourceGeneratedStream << declaration << std::endl;
					descriptionGeneratedStream << description << std::endl;
					moduleGeneratedStream << module << std::endl;
				}
			}
		}

		if (line.find(CONDITION_FUNCTIONS_BEGIN) != std::string::npos) {
			while(std::getline(predictSourceStream, line) && line.find(CONDITION_FUNCTIONS_END) == std::string::npos) {
				if (line.find_last_of(';') != std::string::npos) {
					member = GetMemberName(line);
					type = GetMemberType(line);
					function = GetFunctionName(member, FUNCTION_CONDITION_GET_PREFIX);
					alias = GetFunctionName(member, FUNCTION_CONDITION_GET_ALIAS);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_CONDITION_GET_TEMPLATE);
					description = GetFunctionDescription(function, member, type, FUNCTION_CONDITION_GET_DECLARE);
					module = GetAliasDeclaration(function, alias, type, FUNCTION_CONDITION_GET_MODULE);
					definitionGeneratedStream << '\t' << function << std::endl;
					sourceGeneratedStream << declaration << std::endl;
					descriptionGeneratedStream << description << std::endl;
					moduleGeneratedStream << module << std::endl;
					function = GetFunctionName(member, FUNCTION_CONDITION_SET_PREFIX);
					alias = GetFunctionName(member, FUNCTION_CONDITION_SET_ALIAS);
					declaration = GetFunctionDeclaration(function, member, type, FUNCTION_CONDITION_SET_TEMPLATE);
					description = GetFunctionDescription(function, member, type, FUNCTION_CONDITION_SET_DECLARE);
					module = GetAliasDeclaration(function, alias, type, FUNCTION_CONDITION_SET_MODULE);
					definitionGeneratedStream << '\t' << function << std::endl;
					sourceGeneratedStream << declaration << std::endl;
					descriptionGeneratedStream << description << std::endl;
					moduleGeneratedStream << module << std::endl;
				}
			}
		}
	}

	while(std::getline(descriptionTemplateStream, line)) {
		descriptionGeneratedStream << line << std::endl;
	}

	predictSourceStream.close();
	definitionTemplateStream.close();
	definitionGeneratedStream.close();
	sourceTemplateStream.close();
	sourceGeneratedStream.close();
	descriptionTemplateStream.close();
	descriptionGeneratedStream.close();

	return 0;
}


