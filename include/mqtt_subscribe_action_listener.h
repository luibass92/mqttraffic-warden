#ifndef MQTT_ACTION_LISTENER_H
#define MQTT_ACTION_LISTENER_H

#include <iostream>

#include "mqtt/async_client.h"
#include "spdlog/spdlog.h"

namespace tw {
class MqttSubscribeActionListener : public virtual mqtt::iaction_listener {
 protected:
  void on_failure(const mqtt::token& tok) override {
    spdlog::error("Subscription failure");
    if (tok.get_message_id() != 0)
      spdlog::error(" for token: [{}]", tok.get_message_id());
  }

  void on_success(const mqtt::token& tok) override {
    spdlog::info("Successfully subscribed");
    if (tok.get_message_id() != 0)
      spdlog::info(" for token: [{}]", tok.get_message_id());
    auto top = tok.get_topics();
    if (top && !top->empty())
      spdlog::info("\ttoken topic: '{}', ...", (*top)[0]);
  }
};
}  // namespace tw

#endif  // MQTT_ACTION_LISTENER_H
