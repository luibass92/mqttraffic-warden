#include "json_utilities.h"

#include "spdlog/spdlog.h"

namespace utilities {
std::string dump_json(const nlohmann::json& json) {
  std::string result;
  try {
    result = json.dump();
  } catch (nlohmann::json::type_error& te) {
    spdlog::error("{}", te.what());
  }
  return result;
}

bool json_to_string(const nlohmann::json& p_json, std::string& p_string) {
  if (p_json.is_string())
    p_string = p_json.get<std::string>();
  else if (p_json.is_boolean())
    p_string = p_json.get<bool>();
  else if (p_json.is_number_unsigned())
    p_string = p_json.get<unsigned long>();
  else if (p_json.is_number_integer())
    p_string = p_json.get<long>();
  else if (p_json.is_number_float())
    p_string = p_json.get<double>();
  else {
    spdlog::error("JSON '{}' has unknown type");
    return false;
  }

  return true;
}

}  // namespace utilities
