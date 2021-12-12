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
}  // namespace utilities
