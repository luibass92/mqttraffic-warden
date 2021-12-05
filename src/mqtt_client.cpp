#include "mqtt_client.h"

#include "spdlog/spdlog.h"

namespace tw {

void MqttClient::connect() {
  try {
    spdlog::info("Connecting to the MQTT server...");
    m_asyncClient.connect(m_connectionOptions, nullptr, m_connectionCallback);
  } catch (const mqtt::exception& exc) {
    spdlog::error("ERROR: Unable to connect to MQTT server: {}", exc.what());
  }
}

void MqttClient::disconnect() {
  if (m_asyncClient.is_connected()) {
    try {
      spdlog::info("Disconnecting from the MQTT server...");
      m_asyncClient.disconnect()->wait();
      spdlog::info("Disconnected successfully");
    } catch (const mqtt::exception& exc) {
      spdlog::error("{}", exc.what());
    }
  }
}

void MqttClient::unsubscribe(const std::string& p_topic) {
  if (m_asyncClient.is_connected()) {
    try {
      m_asyncClient.unsubscribe(p_topic);
    } catch (const mqtt::exception& exc) {
      spdlog::error("{}", exc.what());
    }
  }
}

void MqttClient::publish(const std::string& p_topic,
                         const std::string& p_payload, const int p_qos,
                         const bool p_retained) {
  m_asyncClient.publish(
      mqtt::make_message(p_topic, p_payload, p_qos, p_retained));
}

// void ping();

}  // namespace tw
