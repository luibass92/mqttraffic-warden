#ifndef STREAM_TRANSFORMER_H
#define STREAM_TRANSFORMER_H

#include <unordered_set>

#include "nlohmann/json.hpp"

namespace tw {

enum class JsonType {
  Number = 0,
  String,
  Boolean,
  Object,
  Array,
  Null,
  JsonTypeUnknown
};

class StreamTransformer {
 public:
  StreamTransformer() {}
  virtual ~StreamTransformer() {}
  virtual void setup(const nlohmann::json& p_json) = 0;
  virtual void execute(const std::string& p_inputTopic,
                       const nlohmann::json& p_inputPayload,
                       std::string& p_outputTopic,
                       nlohmann::json& p_outputPayload) = 0;

 protected:
  inline static std::string const k_from = "from";
  inline static std::string const k_to = "to";
  inline static std::string const k_as = "as";
  inline static std::string const k_keep = "keep";
  inline static std::string const k_typeNumber = "number";
  inline static std::string const k_typeString = "string";
  inline static std::string const k_typeBoolean = "boolean";
  inline static std::unordered_set<std::string> const k_types = {
      k_typeNumber, k_typeString, k_typeBoolean};
};
}  // namespace tw
#endif  // STREAM_TRANSFORMER_H
