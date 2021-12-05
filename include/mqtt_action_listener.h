#ifndef MQTT_ACTION_LISTENER_H
#define MQTT_ACTION_LISTENER_H

#include <iostream>

#include "mqtt/async_client.h"

namespace traffic_warden {
class MqttActionListener : public virtual mqtt::iaction_listener {
 protected:
  void on_failure(const mqtt::token& tok) override {
    std::cout << " failure";
    if (tok.get_message_id() != 0)
      std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
    std::cout << std::endl;
  }

  void on_success(const mqtt::token& tok) override {
    std::cout << " success";
    if (tok.get_message_id() != 0)
      std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
    auto top = tok.get_topics();
    if (top && !top->empty())
      std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
    std::cout << std::endl;
  }
};
}  // namespace traffic_warden

#endif  // MQTT_ACTION_LISTENER_H
