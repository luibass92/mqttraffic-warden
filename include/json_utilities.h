#ifndef JSON_UTILITIES_H
#define JSON_UTILITIES_H

#include "nlohmann/json.hpp"
//#include "spdlog/spdlog.h"

namespace utilities {
std::string dump_json(const nlohmann::json& json) {
  std::string result;
  try {
    result = json.dump();
  } catch (nlohmann::json::type_error& te) {
    // spdlog::error("{}", te.what());
  }
  return result;
}
}  // namespace utilities

#endif  // JSON_UTILITIES_H
