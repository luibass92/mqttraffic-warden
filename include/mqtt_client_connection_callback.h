#ifndef MQTT_CLIENT_CALLBACK_H
#define MQTT_CLIENT_CALLBACK_H

#include "mqtt/async_client.h"
#include "mqtt_action_listener.h"

namespace traffic_warden {

class MqttClientConnectionCallback : public virtual mqtt::callback,
                                     public virtual mqtt::iaction_listener {
 public:
  MqttClientConnectionCallback(mqtt::async_client& p_asyncClient,
                               mqtt::connect_options& p_connectionOptions)
      : m_asyncClient(p_asyncClient),
        m_connectionOptions(p_connectionOptions) {}

 private:
  // This deomonstrates manually reconnecting to the broker by calling
  // connect() again. This is a possibility for an application that keeps
  // a copy of it's original connect_options, or if the app wants to
  // reconnect with different options.
  // Another way this can be done manually, if using the same options, is
  // to just call the async_client::reconnect() method.
  void reconnect() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    try {
      m_asyncClient.connect(m_connectionOptions, nullptr, *this);
    } catch (const mqtt::exception& exc) {
      std::cerr << "Error: " << exc.what() << std::endl;
      exit(1);
    }
  }

  // Re-connection failure
  void on_failure(const mqtt::token& tok) override {
    std::cout << "Connection attempt failed" << std::endl;
    // if (++nretry_ > N_RETRY_ATTEMPTS) exit(1);
    reconnect();
  }

  // (Re)connection success
  // Either this or connected() can be used for callbacks.
  void on_success(const mqtt::token& tok) override {}

  // (Re)connection success
  void connected(const std::string& cause) override {
    std::cout << "\nConnection success" << std::endl;
    // std::cout << "\nSubscribing to topic '" << TOPIC << "'\n"
    //         << "\tfor client " << CLIENT_ID
    //         << " using QoS" << QOS << "\n"
    //         << "\nPress Q<Enter> to quit\n" << std::endl;

    m_asyncClient.subscribe("TOPIC", 2, nullptr, m_actionListener);
  }

  // Callback for when the connection is lost.
  // This will initiate the attempt to manually reconnect.
  void connection_lost(const std::string& cause) override {
    std::cout << "\nConnection lost" << std::endl;
    if (!cause.empty()) std::cout << "\tcause: " << cause << std::endl;

    std::cout << "Reconnecting..." << std::endl;
    // nretry_ = 0;
    reconnect();
  }

  // Callback for when a message arrives.
  void message_arrived(mqtt::const_message_ptr msg) override {
    std::cout << "Message arrived" << std::endl;
    std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
    std::cout << "\tpayload: '" << msg->to_string() << "'\n" << std::endl;
  }

  void delivery_complete(mqtt::delivery_token_ptr token) override {
    std::cout << "\tDelivery complete for token: "
              << (token ? token->get_message_id() : -1) << std::endl;
  }

  traffic_warden::MqttActionListener m_actionListener;
  mqtt::async_client& m_asyncClient;
  mqtt::connect_options& m_connectionOptions;
};

}  // namespace traffic_warden

#endif  // MQTT_CLIENT_CALLBACK_H
