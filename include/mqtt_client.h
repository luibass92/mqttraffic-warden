#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "mqtt/async_client.h"
#include "mqtt_client_connection_callback.h"
#include "mqtt_delivery_action_listener.h"

namespace traffic_warden {

class MqttClient {
 public:
  MqttClient(const std::string& p_address, const std::string& p_clientId,
             const std::string& p_user, const std::string& p_password,
             const bool p_cleanSession, const bool p_reconnect,
             const std::string& p_caCertificate,
             const std::string& p_clientCertificate,
             const std::string& p_clientKey)
      : m_asyncClient(p_address, p_clientId),
        m_connectionOptions(mqtt::connect_options_builder()
                                .user_name(p_user)
                                .password(p_password)
                                .automatic_reconnect(p_reconnect)
                                .clean_session(p_cleanSession)
                                .ssl(mqtt::ssl_options_builder()
                                         .trust_store(p_caCertificate)
                                         .key_store(p_clientCertificate)
                                         .private_key(p_clientKey)
                                         .finalize())
                                .finalize()),
        m_connectionCallback(m_asyncClient, m_connectionOptions) {
    m_asyncClient.set_callback(m_connectionCallback);
  }

  MqttClient(const std::string& p_address, const std::string& p_clientId,
             const std::string& p_user, const std::string& p_password,
             const bool p_cleanSession, const bool p_reconnect)
      : m_asyncClient(p_address, p_clientId),
        m_connectionOptions(mqtt::connect_options_builder()
                                .user_name(p_user)
                                .password(p_password)
                                .automatic_reconnect(p_reconnect)
                                .clean_session(p_cleanSession)
                                .finalize()),
        m_connectionCallback(m_asyncClient, m_connectionOptions) {
    m_asyncClient.set_callback(m_connectionCallback);
  }

  MqttClient(MqttClient const&) = delete;
  void operator=(MqttClient const&) = delete;

  void connect();
  void disconnect();
  void subscribe();
  void unsubscribe();
  void publish();
  void ping();

 private:
  mqtt::async_client m_asyncClient;

  mqtt::connect_options m_connectionOptions;

  traffic_warden::MqttClientConnectionCallback m_connectionCallback;

  traffic_warden::MqttDeliveryActionListener m_deliveryActionListener;
};

}  // namespace traffic_warden

#endif  // MQTT_CLIENT_H
