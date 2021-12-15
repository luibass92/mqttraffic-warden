#include "stream_transformer_topic_to_topic.h"

#include "exceptions.h"
#include "spdlog/spdlog.h"
#include "utilities.h"

namespace tw {

void StreamTransformerTopicToTopic::setup(const nlohmann::json& p_json) {
  if (!is_valid_setup(p_json)) {
    spdlog::error("{} has an invalid setup", get_class_name());
    throw StreamTransformerSetupException();
  }

  // mandatory fields
  TopicToTopicTransformation_t l_transformation;
  l_transformation.fromTopic = p_json[k_from].get<int>();
  l_transformation.toTopic = p_json[k_to].get<int>();

  m_transformation = l_transformation;
}

void StreamTransformerTopicToTopic::execute(const std::string& p_inputTopic,
                                            const nlohmann::json&,
                                            std::string& p_outputTopic,
                                            nlohmann::json&) {
  std::vector<std::string> l_inputTopicSplitted =
      utilities::splitString(p_inputTopic, "/");
  std::vector<std::string> l_outputTopicSplitted =
      utilities::splitString(p_outputTopic, "/");

  if (l_inputTopicSplitted.size() <= m_transformation.fromTopic) {
    spdlog::error(
        "The input sub-topic at index '{}' does not exists. Input topic is "
        "'{}' and has max index = '{}'",
        m_transformation.fromTopic, p_inputTopic,
        l_inputTopicSplitted.size() - 1);
    throw StreamTransformerExecutionException();
  } else if (l_outputTopicSplitted.size() <= m_transformation.toTopic) {
    spdlog::warn(
        "The output sub-topic at index '{}' does not exists. Output sub-topic "
        "will be appended at the end of the output topic '{}'",
        m_transformation.toTopic, p_outputTopic);
    p_outputTopic.append("/" +
                         l_inputTopicSplitted.at(m_transformation.fromTopic));
  } else {
    l_outputTopicSplitted.insert(
        l_outputTopicSplitted.begin() + m_transformation.toTopic,
        l_inputTopicSplitted.at(m_transformation.fromTopic));
    p_outputTopic = utilities::joinStrings(l_outputTopicSplitted, "/");
  }
}

bool StreamTransformerTopicToTopic::is_valid_setup(
    const nlohmann::json& p_json) {
  if (!p_json.is_object()) {
    spdlog::error("{} is not a JSON object", get_class_name());
    return false;
  } else if (!p_json.contains(k_from)) {
    spdlog::error("{} must contain a '{}' key", get_class_name(), k_from);
    return false;
  } else if (!p_json[k_from].is_number_unsigned()) {
    spdlog::error("{} '{}' value must be an unsigned number", get_class_name(),
                  k_from);
    return false;
  } else if (!p_json.contains(k_to)) {
    spdlog::error("{} must contain a '{}' key", get_class_name(), k_to);
    return false;
  } else if (!p_json[k_to].is_number_unsigned()) {
    spdlog::error("{} '{}' value must be an unsigned number", get_class_name(),
                  k_to);
    return false;
  }

  return true;
}
}  // namespace tw
