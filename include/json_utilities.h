#ifndef JSON_UTILITIES_H
#define JSON_UTILITIES_H

#include "nlohmann/json.hpp"

namespace utilities {
std::string dump_json(const nlohmann::json& json);
bool json_to_string(const nlohmann::json& p_json, std::string& p_string);
}  // namespace utilities

#endif  // JSON_UTILITIES_H
