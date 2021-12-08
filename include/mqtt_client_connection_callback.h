#ifndef MQTT_CLIENT_CALLBACK_H
#define MQTT_CLIENT_CALLBACK_H

#include "mqtt/async_client.h"
#include "mqtt_subscribe_action_listener.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "tbb/concurrent_queue.h"

namespace tw {

class MqttClientConnectionCallback : public virtual mqtt::callback,
                                     public virtual mqtt::iaction_listener {
 public:
  MqttClientConnectionCallback(mqtt::async_client& p_asyncClient,
                               mqtt::connect_options& p_connectionOptions)
      : m_asyncClient(p_asyncClient),
        m_connectionOptions(p_connectionOptions) {}

  void setTopicToSubscribe(const std::list<std::string>& p_topics) {
    m_topicsToSubscribe = p_topics;
  }

  void setMessageQueue(
      tbb::concurrent_queue<std::pair<std::string, nlohmann::json>>*
          p_messageQueue) {
    m_messageQueue = p_messageQueue;
  }

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
      spdlog::error("Error: {}", exc.what());
    }
  }

  // Re-connection failure
  void on_failure(const mqtt::token& tok) override {
    spdlog::error("Connection attempt failed");
    // if (++nretry_ > N_RETRY_ATTEMPTS) exit(1);
    reconnect();
  }

  // (Re)connection success
  // Either this or connected() can be used for callbacks.
  void on_success(const mqtt::token& tok) override {}

  // (Re)connection success
  void connected(const std::string& cause) override {
    spdlog::info("Connection success");
    // std::cout << "\nSubscribing to topic '" << TOPIC << "'\n"
    //         << "\tfor client " << CLIENT_ID
    //         << " using QoS" << QOS << "\n"
    //         << "\nPress Q<Enter> to quit\n" << std::endl;

    for (auto it_topic : m_topicsToSubscribe)
      m_asyncClient.subscribe(it_topic, 2, nullptr, m_subscribeActionListener);
  }

  // Callback for when the connection is lost.
  // This will initiate the attempt to manually reconnect.
  void connection_lost(const std::string& cause) override {
    spdlog::error("Connection lost");
    if (!cause.empty()) spdlog::error("cause: {}", cause);

    spdlog::info("Reconnecting...");
    // nretry_ = 0;
    reconnect();
  }

  // Callback for when a message arrives.
  void message_arrived(mqtt::const_message_ptr msg) override {
    spdlog::trace("Message arrived\n\ttopic: '{}'\n\tpayload: '{}'",
                  msg->get_topic(), msg->to_string());
    m_messageQueue->push(std::make_pair(
        msg->get_topic(), nlohmann::json::parse(msg->to_string())));
  }

  void delivery_complete(mqtt::delivery_token_ptr token) override {
    spdlog::trace(
        "Delivery complete for token: '{}'\n\ttopic: {}\n\tpayload: '{}'",
        token ? token->get_message_id() : -1, token->get_message()->get_topic(),
        token->get_message()->to_string());
  }

  tw::MqttSubscribeActionListener m_subscribeActionListener;
  mqtt::async_client& m_asyncClient;
  mqtt::connect_options& m_connectionOptions;
  std::list<std::string> m_topicsToSubscribe;
  tbb::concurrent_queue<std::pair<std::string, nlohmann::json>>* m_messageQueue;
};

}  // namespace tw

#endif  // MQTT_CLIENT_CALLBACK_H
