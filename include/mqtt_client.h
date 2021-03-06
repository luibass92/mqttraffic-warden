#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "mqtt/async_client.h"
#include "mqtt_client_connection_callback.h"
#include "nlohmann/json.hpp"
#include "tbb/concurrent_queue.h"

namespace tw {

class MqttClient {
 public:
  MqttClient(const std::string& p_address, const std::string& p_clientId,
             const std::string& p_user, const std::string& p_password,
             const int p_keepAliveInterval)
      : m_asyncClient(p_address, p_clientId),
        m_connectionOptions(mqtt::connect_options_builder()
                                .user_name(p_user)
                                .password(p_password)
                                .keep_alive_interval(std::chrono::duration<int>(
                                    p_keepAliveInterval))
                                .finalize()),
        m_connectionCallback(m_asyncClient, m_connectionOptions) {}

  MqttClient(const std::string& p_address, const std::string& p_clientId,
             const std::string& p_user, const std::string& p_password,
             const int p_keepAliveInterval, const std::string& p_caCertificate,
             const std::string& p_clientCertificate,
             const std::string& p_clientKey)
      : m_asyncClient(p_address, p_clientId),
        m_connectionOptions(mqtt::connect_options_builder()
                                .user_name(p_user)
                                .password(p_password)
                                .keep_alive_interval(std::chrono::duration<int>(
                                    p_keepAliveInterval))
                                .ssl(mqtt::ssl_options_builder()
                                         .trust_store(p_caCertificate)
                                         .key_store(p_clientCertificate)
                                         .private_key(p_clientKey)
                                         .finalize())
                                .finalize()),
        m_connectionCallback(m_asyncClient, m_connectionOptions) {}

  MqttClient(MqttClient const&) = delete;
  void operator=(MqttClient const&) = delete;

  void initCallback(
      const std::list<std::string>& p_topics,
      tbb::concurrent_queue<std::pair<std::string, nlohmann::json>>*
          p_messageQueue);
  void connect();
  void disconnect();
  void unsubscribe(const std::string& p_topic);
  void publish(const std::string& p_topic, const std::string& p_payload,
               const int p_qos, const bool p_retained);

 private:
  mqtt::async_client m_asyncClient;

  mqtt::connect_options m_connectionOptions;

  tw::MqttClientConnectionCallback m_connectionCallback;
};

}  // namespace tw

#endif  // MQTT_CLIENT_H
