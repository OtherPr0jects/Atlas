#include <vector>
#include <sstream>
#include <algorithm>
#include "StringUtility.h"

std::string StringUtility::ToLowerCase(std::string str) {
    std::string lowercaseString;

    std::for_each(str.begin(), str.end(), [&](char character) {
        lowercaseString.push_back(tolower(character));
    });

    return lowercaseString;
}

std::vector<std::string> StringUtility::Split(std::string str, char delimiter) {
    std::vector<std::string> split;

    std::stringstream stringStream(str);
    std::string splitString;
    while (std::getline(stringStream, splitString, delimiter)) {
        split.push_back(splitString);
    }

    return split;
}