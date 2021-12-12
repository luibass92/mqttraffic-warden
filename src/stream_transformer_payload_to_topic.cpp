#include "stream_transformer_payload_to_topic.h"

#include <iostream>

#include "cast.h"
#include "exceptions.h"
#include "spdlog/spdlog.h"
#include "utilities.h"

namespace tw {

void StreamTransformerPayloadToTopic::setup(const nlohmann::json& p_json) {
  if (!is_valid_setup(p_json)) {
    spdlog::error("{} has an invalid setup", get_class_name());
    throw StreamTransformerSetupException();
  }

  // mandatory fields
  m_transformation.fromPayload = p_json["from"].get<std::string>();
  m_transformation.toTopic = p_json["to"].get<int>();

  // optional fields
  if (p_json.contains("keep")) {
    m_transformation.keep = p_json["keep"].get<bool>();
  } else {
    m_transformation.keep = false;
  }
}

void StreamTransformerPayloadToTopic::execute(
    const std::string&, const nlohmann::json& p_inputPayload,
    std::string& p_outputTopic, nlohmann::json& p_outputPayload) {
  std::vector<std::string> l_outputTopicSplitted =
      utilities::splitString(p_outputTopic, "/");
  if (l_outputTopicSplitted.size() <= m_transformation.toTopic) {
    spdlog::warn(
        "The output sub-topic at index '{}' does not exists. Payload "
        "will be appended at the end of the output topic '{}'",
        m_transformation.toTopic, p_outputTopic);
    p_outputTopic.append(
        "/" + std::string(p_inputPayload.at(m_transformation.fromPayload)));
  } else {
    l_outputTopicSplitted.insert(
        l_outputTopicSplitted.begin() + m_transformation.toTopic,
        std::string(p_inputPayload.at(m_transformation.fromPayload)));
    p_outputTopic = utilities::joinStrings(l_outputTopicSplitted, "/");
  }
  // If 'keep' is false, the original payload key-value pair must be deleted
  if (!m_transformation.keep) {
    p_outputPayload.erase(m_transformation.fromPayload);
  }
}

bool StreamTransformerPayloadToTopic::is_valid_setup(
    const nlohmann::json& p_json) {
  if (!p_json.is_object()) {
    spdlog::error("{} is not a JSON object", get_class_name());
    return false;
  }
  // mandatory fields
  else if (!p_json.contains("from")) {
    spdlog::error("{} must contain a '{}' key", get_class_name(), "from");
    return false;
  } else if (!p_json["from"].is_string()) {
    spdlog::error("{} '{}' value must be a string", get_class_name(), "from");
    return false;
  } else if (!p_json.contains("to")) {
    spdlog::error("{} must contain a '{}' key", get_class_name(), "to");
    return false;
  } else if (!p_json["to"].is_number_unsigned()) {
    spdlog::error("{} '{}' value must be a number", get_class_name(), "to");
    return false;
  }
  // optional fields
  else if (p_json.contains("keep") && !p_json["keep"].is_boolean()) {
    spdlog::error("{} '{}' value must be a boolean", get_class_name(), "keep");
    return false;
  }

  return true;
}

}  // namespace tw
