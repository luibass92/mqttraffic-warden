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

/* < name, <inputTopic, outputTopic, <listOfStransformers>>> */
typedef std::unordered_map<
    std::string, std::tuple<std::string, std::string,
                            std::list<std::shared_ptr<StreamTransformer>>>>
    RouteConfigurations_t;

class TrafficWarden {
 public:
  TrafficWarden();
  ~TrafficWarden();

  void init(const nlohmann::json& p_configurations);

 private:
  BrokerConfigurations_t retrieve_broker_infos(
      const nlohmann::json& p_configurations);

  RouteConfigurations_t retrieve_routes(const nlohmann::json& p_routes);

  std::shared_ptr<MqttClient> m_mqttClient;
};

}  // namespace tw

#endif  // TRAFFIC_WARDEN_H
