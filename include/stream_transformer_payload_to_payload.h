#ifndef STREAM_TRANSFORMER_PAYLOAD_TO_PAYLOAD_H
#define STREAM_TRANSFORMER_PAYLOAD_TO_PAYLOAD_H

#include "reflection.h"
#include "stream_transformer.h"

namespace tw {

typedef struct {
  std::string fromPayload;
  std::string toPayload;
  JsonType asType;
  bool keep;
} PayloadToPayloadTransformation_t;

class StreamTransformerPayloadToPayload : public StreamTransformer {
  ADD_CLASS_NAME(StreamTransformerPayloadToPayload);

 public:
  StreamTransformerPayloadToPayload(){};
  void setup(const nlohmann::json& p_json) override;
  void execute(const std::string& p_inputTopic,
               const nlohmann::json& p_inputPayload, std::string& p_outputTopic,
               nlohmann::json& p_outputPayload) override;

 private:
  bool is_valid_setup(const nlohmann::json& p_json);
  PayloadToPayloadTransformation_t m_transformation;
};

}  // namespace tw

#endif  // STREAM_TRANSFORMER_PAYLOAD_TO_PAYLOAD_H
