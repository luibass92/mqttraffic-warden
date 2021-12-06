#ifndef TRAFFIC_WARDEN_H
#define TRAFFIC_WARDEN_H

#include <list>
#include <optional>

#include "nlohmann/json.hpp"

namespace tw {

class MqttClient;
class StreamTransformer;

typedef std::tuple<std::string, std::string, std::string,
                   std::optional<std::string>, std::optional<int>,
                   std::optional<std::string>, std::optional<std::string>,
                   std::optional<std::string>>
    BrokerConfigurations_t;
class TrafficWarden {
 public:
  TrafficWarden();
  ~TrafficWarden();

  void init(const nlohmann::json& p_configurations);

 private:
  BrokerConfigurations_t retrieve_broker_infos(
      const nlohmann::json& p_configurations);

  std::list<std::shared_ptr<StreamTransformer>> retrieve_routes(
      const nlohmann::json& p_configurations);

  std::shared_ptr<MqttClient> m_mqttClient;
};

}  // namespace tw

#endif  // TRAFFIC_WARDEN_H
