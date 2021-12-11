#ifndef UTILITIES_H
#define UTILITIES_H

#include <algorithm>
#include <sstream>
#include <vector>

namespace utilities {

std::vector<std::string> splitString(const std::string& inputStr,
                                     const std::string& token);

std::string joinStrings(const std::vector<std::string>& p_strings,
                        const std::string& p_token);

bool isBool(std::string myString);

bool isInt(std::string myString);

bool isFloat(std::string myString);

bool isDouble(std::string myString);

bool isValidIp(const std::string& p_string);

}  // namespace utilities

#endif  // UTILITIES_H
