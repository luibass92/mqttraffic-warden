#include "utilities.h"

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
