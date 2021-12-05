#ifndef TRAFFIC_WARDEN_H
#define TRAFFIC_WARDEN_H

#include <optional>

#include "nlohmann/json.hpp"

namespace tw {
typedef std::tuple<std::string, std::string, std::string,
                   std::optional<std::string>, std::optional<int>,
                   std::optional<std::string>, std::optional<std::string>,
                   std::optional<std::string>>
    BrokerConfigurations_t;
class TrafficWarden {
 public:
  TrafficWarden();
  TrafficWarden(const nlohmann::json& p_configurations);
  ~TrafficWarden();

  void setup(const nlohmann::json& p_configurations);

 private:
  BrokerConfigurations_t retrieve_broker_infos(
      const nlohmann::json& p_configurations);
};
}  // namespace tw

#endif  // TRAFFIC_WARDEN_H
