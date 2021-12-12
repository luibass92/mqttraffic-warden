#include "stream_transformer_payload_to_payload.h"

#include <iostream>

#include "cast.h"
#include "exceptions.h"
#include "json_utilities.h"
#include "spdlog/spdlog.h"
#include "utilities.h"

namespace tw {

void StreamTransformerPayloadToPayload::setup(const nlohmann::json& p_json) {
  if (!is_valid_setup(p_json)) {
    spdlog::error("{} has an invalid setup", get_class_name());
    throw StreamTransformerSetupException();
  }

  // mandatory fields
  m_transformation.fromPayload = p_json["from"].get<std::string>();
  m_transformation.toPayload = p_json["to"].get<std::string>();

  // optional fields
  if (p_json.contains("as")) {
    if ("number" == p_json["as"].get<std::string>()) {
      m_transformation.asType = JsonType::Number;
    } else if ("string" == p_json["as"].get<std::string>()) {
      m_transformation.asType = JsonType::String;
    } else if ("boolean" == p_json["as"].get<std::string>()) {
      m_transformation.asType = JsonType::Boolean;
    }
  } else {
    m_transformation.asType = JsonType::JsonTypeUnknown;
  }
  if (p_json.contains("keep")) {
    m_transformation.keep = p_json["keep"].get<bool>();
  } else {
    m_transformation.keep = false;
  }
}

void StreamTransformerPayloadToPayload::execute(
    const std::string&, const nlohmann::json& p_inputPayload, std::string&,
    nlohmann::json& p_outputPayload) {
  if (m_transformation.asType == JsonType::JsonTypeUnknown) {
    // No type conversion
    p_outputPayload[m_transformation.toPayload] =
        p_inputPayload.at(m_transformation.fromPayload);
  } else {
    // Type conversion
    std::string l_inputPayloadStr = "";
    if (p_inputPayload.at(m_transformation.fromPayload).is_string()) {
      l_inputPayloadStr = p_inputPayload.at(m_transformation.fromPayload);
    } else {
      l_inputPayloadStr =
          utilities::dump_json(p_inputPayload.at(m_transformation.fromPayload));
    }
    switch (m_transformation.asType) {
      case JsonType::Number:
        if (utilities::isInt(l_inputPayloadStr)) {
          p_outputPayload[m_transformation.toPayload] =
              utilities::lexical_cast<long>(l_inputPayloadStr.c_str());
        } else if (utilities::isFloat(l_inputPayloadStr)) {
          p_outputPayload[m_transformation.toPayload] =
              utilities::lexical_cast<float>(l_inputPayloadStr.c_str());
        } else if (utilities::isDouble(l_inputPayloadStr)) {
          p_outputPayload[m_transformation.toPayload] =
              utilities::lexical_cast<double>(l_inputPayloadStr.c_str());
        } else {
          spdlog::error(
              "The input payload is expected to be a number (integer or "
              "float), instead it is: '{}'",
              l_inputPayloadStr);
          throw StreamTransformerExecutionException();
        }
        break;
      case JsonType::String:
        p_outputPayload[m_transformation.toPayload] = l_inputPayloadStr;
        break;
      case JsonType::Boolean:
        if (utilities::isBool(l_inputPayloadStr)) {
          p_outputPayload[m_transformation.toPayload] =
              utilities::lexical_cast<bool>(l_inputPayloadStr.c_str());
        } else {
          spdlog::error(
              "The input payload is expected to be a boolean, instead it is: "
              "'{}'",
              l_inputPayloadStr);
          throw StreamTransformerExecutionException();
        }
        break;
      default:
        spdlog::error(
            "{} '{}' value must be one of the following: '{}', '{}', '{}'",
            get_class_name(), "as", "number", "string", "boolean");
        throw StreamTransformerExecutionException();
        break;
    }
  }
  if (!m_transformation.keep) {
    // If 'keep' is false, the original payload key-value pair must be deleted
    p_outputPayload.erase(m_transformation.fromPayload);
  }
}

bool StreamTransformerPayloadToPayload::is_valid_setup(
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
  } else if (!p_json["to"].is_string()) {
    spdlog::error("{} '{}' value must be a string", get_class_name(), "to");
    return false;
  }
  // optional fields
  else if (p_json.contains("as")) {
    if (!p_json["as"].is_string()) {
      spdlog::error("{} '{}' value must be a string", get_class_name(), "as");
      return false;
    } else if ("number" != p_json["as"].get<std::string>() &&
               "string" != p_json["as"].get<std::string>() &&
               "boolean" != p_json["as"].get<std::string>()) {
      spdlog::error(
          "{} '{}' key must have one of the "
          "following values: '{}', '{}', '{}'",
          get_class_name(), "as", "number", "string", "boolean");
      return false;
    }
  } else if (p_json.contains("keep") && !p_json["keep"].is_boolean()) {
    spdlog::error("{} '{}' value must be a boolean", get_class_name(), "keep");
    return false;
  }

  return true;
}

}  // namespace tw
