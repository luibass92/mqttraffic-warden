#ifndef STREAM_TRANSFORMER_TOPIC_TO_PAYLOAD_H
#define STREAM_TRANSFORMER_TOPIC_TO_PAYLOAD_H

#include <list>

#include "reflection.h"
#include "stream_transformer.h"

namespace tw {

typedef struct {
  unsigned long fromTopic;
  std::string toPayload;
  JsonType asType;
} TopicToPayloadTransformation_t;

class StreamTransformerTopicToPayload : public StreamTransformer {
  ADD_CLASS_NAME(StreamTransformerTopicToPayload);

 public:
  StreamTransformerTopicToPayload(){};
  void setup(const nlohmann::json& p_json) override;
  void execute(const std::string& p_inputTopic,
               const nlohmann::json& p_inputPayload, std::string& p_outputTopic,
               nlohmann::json& p_outputPayload) override;

 private:
  void validate(const nlohmann::json& p_json);
  tw::TopicToPayloadTransformation_t m_transformation;
};

}  // namespace tw
#endif  // STREAM_TRANSFORMER_TOPIC_TO_PAYLOAD_H
