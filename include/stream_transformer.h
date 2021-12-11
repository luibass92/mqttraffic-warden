#ifndef STREAM_TRANSFORMER_H
#define STREAM_TRANSFORMER_H

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
};
}  // namespace tw
#endif  // STREAM_TRANSFORMER_H
