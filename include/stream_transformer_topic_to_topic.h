#ifndef STREAM_TRANSFORMER_TOPIC_TO_TOPIC_H
#define STREAM_TRANSFORMER_TOPIC_TO_TOPIC_H

#include "reflection.h"
#include "stream_transformer.h"

namespace tw {

typedef struct {
  unsigned long fromTopic;
  unsigned long toTopic;
} TopicToTopicTransformation_t;

class StreamTransformerTopicToTopic : public StreamTransformer {
  ADD_CLASS_NAME(StreamTransformerTopicToTopic);

 public:
  StreamTransformerTopicToTopic(){};
  void setup(const nlohmann::json& p_json) override;
  void execute(const std::string& p_inputTopic,
               const nlohmann::json& p_inputPayload, std::string& p_outputTopic,
               nlohmann::json& p_outputPayload) override;

 private:
  bool is_valid_setup(const nlohmann::json& p_json);
  TopicToTopicTransformation_t m_transformation;
};

}  // namespace tw

#endif  // STREAM_TRANSFORMER_TOPIC_TO_TOPIC_H
