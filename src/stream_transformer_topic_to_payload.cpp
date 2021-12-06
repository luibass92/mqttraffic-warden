#include "stream_transformer_topic_to_payload.h"

namespace tw {

void StreamTransformerTopicToPayload::setup(const nlohmann::json& p_json) {
  if (!p_json.is_array()) throw;
  if (p_json.empty()) return;
  for (auto it = p_json.begin(); it != p_json.end(); ++it) {
    if (!(*it).contains("from") || !(*it).contains("to") ||
        !(*it)["from"].is_number_integer() || !(*it)["from"].is_string())
      throw;

    // mandatory fields
    tw::TopicToPayloadTransformation_t l_transformation;
    l_transformation.fromTopic = (*it)["from"].get<int>();
    l_transformation.toPayload = (*it)["to"].get<std::string>();

    // optional fields
    if ((*it).contains("as"))
      l_transformation.asType = (*it)["as"].get<std::string>();

    m_transformations.push_back(l_transformation);
  }
}

void StreamTransformerTopicToPayload::execute(
    const std::vector<std::string>& p_inputTopic,
    const nlohmann::json& p_inputPayload, std::string&,
    nlohmann::json& p_outputPayload) {
  for (auto it_transformation : m_transformations) {
    p_outputPayload = p_inputPayload;
    if (p_inputTopic.size() <= it_transformation.fromTopic) {
      /* complain */
    } else if (it_transformation.asType == "") {
      p_outputPayload[it_transformation.toPayload] =
          p_inputTopic.at(it_transformation.fromTopic);
    } else {
      /*switch(it_transformation.asType){
          case Number:
          break;

      }*/
    }
  }
}

}  // namespace tw

/*
"topicToPayload": [{
                              "from": 1,
                              "to": "producer",
                              "as": "number"
                      }]
          */
