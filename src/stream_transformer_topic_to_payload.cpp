#include "stream_transformer_topic_to_payload.h"

#include <iostream>

#include "cast.h"
#include "exceptions.h"
#include "spdlog/spdlog.h"
#include "utilities.h"

namespace tw {

void StreamTransformerTopicToPayload::setup(const nlohmann::json& p_json) {
  try {
    if (!is_valid_setup(p_json)) {
      spdlog::error("{} has an invalid setup", get_class_name());
      throw StreamTransformerSetupException();
    };

    // mandatory fields
    tw::TopicToPayloadTransformation_t l_transformation;
    l_transformation.fromTopic = p_json[k_from].get<int>();
    l_transformation.toPayload = p_json[k_to].get<std::string>();

    // optional fields
    if (p_json.contains(k_as)) {
      std::string l_stringJsonType = p_json[k_as].get<std::string>();
      if (k_typeNumber == l_stringJsonType) {
        l_transformation.asType = JsonType::Number;
      } else if (k_typeString == l_stringJsonType) {
        l_transformation.asType = JsonType::String;
      } else if (k_typeBoolean == l_stringJsonType) {
        l_transformation.asType = JsonType::Boolean;
      } else {
        spdlog::error(
            "{} '{}' key must have one of the "
            "following values: {}",
            get_class_name(), k_as, fmt::join(k_types, ", "));
        throw StreamTransformerSetupException();
      }
    } else {
      l_transformation.asType = JsonType::JsonTypeUnknown;
    }

    m_transformation = l_transformation;

  } catch (std::exception) {
    throw StreamTransformerSetupException();
  }
}

void StreamTransformerTopicToPayload::execute(const std::string& p_inputTopic,
                                              const nlohmann::json&,
                                              std::string&,
                                              nlohmann::json& p_outputPayload) {
  std::vector<std::string> l_inputTopicSplitted =
      utilities::splitString(p_inputTopic, "/");

  if (l_inputTopicSplitted.size() <= m_transformation.fromTopic) {
    spdlog::error(
        "The input sub-topic at index '{}' does not exists. Input topic is "
        "'{}' and has max index = '{}'",
        m_transformation.fromTopic, p_inputTopic,
        l_inputTopicSplitted.size() - 1);
    throw StreamTransformerExecutionException();
  } else if (m_transformation.asType == JsonType::JsonTypeUnknown) {
    // No type conversion
    p_outputPayload[m_transformation.toPayload] =
        l_inputTopicSplitted.at(m_transformation.fromTopic);
  } else {
    // Type conversion
    std::string l_inputTopicStr =
        l_inputTopicSplitted.at(m_transformation.fromTopic);
    switch (m_transformation.asType) {
      case JsonType::Number:
        if (utilities::isInt(l_inputTopicStr)) {
          p_outputPayload[m_transformation.toPayload] =
              utilities::lexical_cast<long>(l_inputTopicStr.c_str());
        } else if (utilities::isFloat(l_inputTopicStr)) {
          p_outputPayload[m_transformation.toPayload] =
              utilities::lexical_cast<float>(l_inputTopicStr.c_str());
        } else if (utilities::isDouble(l_inputTopicStr)) {
          p_outputPayload[m_transformation.toPayload] =
              utilities::lexical_cast<double>(l_inputTopicStr.c_str());
        } else {
          spdlog::error(
              "The input sub-topic is expected to be a number (integer or "
              "float), instead it is: '{}'",
              l_inputTopicStr);
          throw StreamTransformerExecutionException();
        }
        break;
      case JsonType::String:
        p_outputPayload[m_transformation.toPayload] = l_inputTopicStr;
        break;
      case JsonType::Boolean:
        if (utilities::isBool(l_inputTopicStr)) {
          p_outputPayload[m_transformation.toPayload] =
              utilities::lexical_cast<bool>(l_inputTopicStr.c_str());
        }
        break;
      default:
        spdlog::error("{} '{}' value must be one of the following: {}",
                      get_class_name(), k_as, fmt::join(k_types, ", "));
        throw StreamTransformerExecutionException();
        break;
    }
  }
}

bool StreamTransformerTopicToPayload::is_valid_setup(
    const nlohmann::json& p_json) {
  if (!p_json.is_object()) {
    spdlog::error("{} is not a JSON object", get_class_name());
    return false;
  } else if (!p_json.contains(k_from)) {
    spdlog::error("{} must contain a '{}' key", get_class_name(), k_from);
    return false;
  } else if (!p_json[k_from].is_number_unsigned()) {
    spdlog::error("{} '{}' value must be a number", get_class_name(), k_from);
    return false;
  } else if (!p_json.contains(k_to)) {
    spdlog::error("{} must contain a '{}' key", get_class_name(), k_to);
    return false;
  } else if (!p_json[k_to].is_string()) {
    spdlog::error("{} '{}' value must be a string", get_class_name(), k_to);
    return false;
  } else if (p_json.contains(k_as) && !p_json[k_as].is_string()) {
    spdlog::error("{} '{}' value must be a string", get_class_name(), k_as);
    return false;
  }

  return true;
}

}  // namespace tw
