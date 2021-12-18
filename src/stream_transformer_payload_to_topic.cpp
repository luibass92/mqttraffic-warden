#include "stream_transformer_payload_to_topic.h"

#include <iostream>

#include "cast.h"
#include "exceptions.h"
#include "json_utilities.h"
#include "spdlog/spdlog.h"
#include "utilities.h"

namespace tw {

void StreamTransformerPayloadToTopic::setup(const nlohmann::json& p_json) {
  if (!is_valid_setup(p_json)) {
    spdlog::error("{} has an invalid setup", get_class_name());
    throw StreamTransformerSetupException();
  }

  // mandatory fields
  m_transformation.fromPayload = p_json[k_from].get<std::string>();
  m_transformation.toTopic = p_json[k_to].get<int>();

  // optional fields
  if (p_json.contains(k_keep)) {
    m_transformation.keep = p_json[k_keep].get<bool>();
  } else {
    m_transformation.keep = false;
  }
}

void StreamTransformerPayloadToTopic::execute(
    const std::string&, const nlohmann::json& p_inputPayload,
    std::string& p_outputTopic, nlohmann::json& p_outputPayload) {
  std::vector<std::string> l_outputTopicSplitted =
      utilities::splitString(p_outputTopic, "/");

  std::string l_inputPayloadStr = "";
  if (!p_inputPayload.contains(m_transformation.fromPayload)) {
    spdlog::error("Key '{}' not found in input payload '{}'",
                  m_transformation.fromPayload,
                  utilities::dump_json(p_inputPayload));
    throw StreamTransformerExecutionException();
  } else if (!utilities::json_to_string(
                 p_inputPayload.at(m_transformation.fromPayload),
                 l_inputPayloadStr)) {
    spdlog::error("JSON to string conversion failed");
    throw StreamTransformerExecutionException();
  } else if (l_outputTopicSplitted.size() <= m_transformation.toTopic) {
    spdlog::warn(
        "The output topic '{}' does not have index '{}'. Payload '{}'"
        "will be appended at the end.",
        p_outputTopic, l_inputPayloadStr, m_transformation.toTopic);
    p_outputTopic.append("/" + l_inputPayloadStr);

  } else {
    l_outputTopicSplitted.insert(
        l_outputTopicSplitted.begin() + m_transformation.toTopic,
        l_inputPayloadStr);
    p_outputTopic = utilities::joinStrings(l_outputTopicSplitted, "/");
  }
  // If 'keep' is false, the original payload key-value pair must be deleted
  if (!m_transformation.keep &&
      p_outputPayload.contains(m_transformation.fromPayload)) {
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
  else if (!p_json.contains(k_from)) {
    spdlog::error("{} must contain a '{}' key", get_class_name(), k_from);
    return false;
  } else if (!p_json[k_from].is_string()) {
    spdlog::error("{} '{}' value must be a string", get_class_name(), k_from);
    return false;
  } else if (!p_json.contains(k_to)) {
    spdlog::error("{} must contain a '{}' key", get_class_name(), k_to);
    return false;
  } else if (!p_json[k_to].is_number_unsigned()) {
    spdlog::error("{} '{}' value must be a number", get_class_name(), k_to);
    return false;
  }
  // optional fields
  else if (p_json.contains(k_keep) && !p_json[k_keep].is_boolean()) {
    spdlog::error("{} '{}' value must be a boolean", get_class_name(), k_keep);
    return false;
  }

  return true;
}

}  // namespace tw
