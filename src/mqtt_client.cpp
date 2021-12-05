#include "mqtt_client.h"

namespace traffic_warden {

void MqttClient::connect() {
  try {
    std::cout << "Connecting to the MQTT server..." << std::flush;
    m_asyncClient.connect(m_connectionOptions, nullptr, m_connectionCallback);
  } catch (const mqtt::exception& exc) {
    std::cerr << "\nERROR: Unable to connect to MQTT server: " << exc
              << std::endl;
    // return 1;
  }
}

void MqttClient::disconnect() {
  if (m_asyncClient.is_connected()) {
    try {
      std::cout << "\nDisconnecting from the MQTT server..." << std::flush;
      m_asyncClient.disconnect()->wait();
      std::cout << "OK" << std::endl;
    } catch (const mqtt::exception& exc) {
      std::cerr << exc << std::endl;
      // return 1;
    }
  }
}

// void subscribe();

// void unsubscribe();

void MqttClient::publish() {
  m_asyncClient.publish(mqtt::make_message("myTopic", "myPayload", 3, true),
                        nullptr, m_deliveryActionListener);
}

// void ping();

}  // namespace traffic_warden
