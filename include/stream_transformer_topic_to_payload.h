#ifndef STREAM_TRANSFORMER_TOPIC_TO_PAYLOAD_H
#define STREAM_TRANSFORMER_TOPIC_TO_PAYLOAD_H

#include <list>

#include "stream_transformer.h"

namespace tw {

typedef struct {
  int fromTopic;
  std::string toPayload;
  std::string asType;
} TopicToPayloadTransformation_t;

class StreamTransformerTopicToPayload : public StreamTransformer {
 public:
  StreamTransformerTopicToPayload(){};
  void setup(const nlohmann::json& p_json) override;
  void execute(const std::string& p_inputTopic,
               const nlohmann::json& p_inputPayload, std::string& p_outputTopic,
               nlohmann::json& p_outputPayload) override;

 private:
  std::list<tw::TopicToPayloadTransformation_t> m_transformations;
};

}  // namespace tw
#endif  // STREAM_TRANSFORMER_TOPIC_TO_PAYLOAD_H
