#ifndef MQTT_DELIVERY_ACTION_LISTENER_H
#define MQTT_DELIVERY_ACTION_LISTENER_H

#include <atomic>

#include "mqtt_action_listener.h"

namespace traffic_warden {
class MqttDeliveryActionListener : public MqttActionListener {
 public:
  MqttDeliveryActionListener() : m_done(false) {}
  bool is_done() const { return m_done; }

 private:
  std::atomic<bool> m_done;

  void on_failure(const mqtt::token& tok) override {
    MqttActionListener::on_failure(tok);
    m_done = true;
  }

  void on_success(const mqtt::token& tok) override {
    MqttActionListener::on_success(tok);
    m_done = true;
  }
};
}  // namespace traffic_warden

#endif  // MQTT_DELIVERY_ACTION_LISTENER_H
