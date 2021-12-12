#ifndef JSON_UTILITIES_H
#define JSON_UTILITIES_H

#include "nlohmann/json.hpp"

namespace utilities {
std::string dump_json(const nlohmann::json& json);
}  // namespace utilities

#endif  // JSON_UTILITIES_H
