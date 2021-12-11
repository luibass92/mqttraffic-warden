#include "utilities.h"

#include <regex>

namespace utilities {

std::vector<std::string> splitString(const std::string& inputStr,
                                     const std::string& token) {
  std::string str = inputStr;
  std::vector<std::string> result;
  while (str.size()) {
    std::size_t index = str.find(token);
    if (index != std::string::npos) {
      result.push_back(str.substr(0, index));
      str = str.substr(index + token.size());
      if (str.size() == 0) result.push_back(str);
    } else {
      result.push_back(str);
      str.clear();
    }
  }
  return result;
}

bool isBool(std::string myString) {
  std::transform(myString.begin(), myString.end(), myString.begin(), ::tolower);
  std::istringstream iss(myString);
  bool b;
  iss >> std::boolalpha >> b;
  return iss.eof() && !iss.fail();
}

bool isInt(std::string myString) {
  std::istringstream iss(myString);
  int n;
  iss >> std::noskipws >> n;  // noskipws considers leading whitespace invalid
  // Check the entire string was consumed and if either failbit or badbit is set
  return iss.eof() && !iss.fail();
}

bool isFloat(std::string myString) {
  std::istringstream iss(myString);
  float n;
  iss >> std::noskipws >> n;  // noskipws considers leading whitespace invalid
  // Check the entire string was consumed and if either failbit or badbit is set
  return iss.eof() && !iss.fail();
}

bool isDouble(std::string myString) {
  std::istringstream iss(myString);
  double n;
  iss >> std::noskipws >> n;  // noskipws considers leading whitespace invalid
  // Check the entire string was consumed and if either failbit or badbit is set
  return iss.eof() && !iss.fail();
}

bool isValidIp(const std::string& p_string) {
  // Regex expression for validating IPv4
  std::regex ipv4(
      "(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9]["
      "0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");

  // Regex expression for validating IPv6
  std::regex ipv6("((([0-9a-fA-F]){1,4})\\:){7}([0-9a-fA-F]){1,4}");

  // Checking if it is a valid IPv4 addresses
  if (regex_match(p_string, ipv4)) return true;

  // Checking if it is a valid IPv6 addresses
  else if (regex_match(p_string, ipv6))
    return true;

  // Return Invalid
  return false;
}
}  // namespace utilities
