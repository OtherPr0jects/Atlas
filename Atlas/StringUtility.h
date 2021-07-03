#pragma once
#include <vector>

class StringUtility {
	public:
		static std::string ToLowerCase(std::string str);
		
		static std::vector<std::string> Split(std::string str, char delimiter);
};