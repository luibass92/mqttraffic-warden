#include "traffic_warden.h"

namespace tw {

TrafficWarden::TrafficWarden() {}
TrafficWarden::TrafficWarden(const nlohmann::json& p_configurations) {
  setup(p_configurations);
}
TrafficWarden::~TrafficWarden(){};

void TrafficWarden::setup(const nlohmann::json& p_configurations) {
  auto [l_host, l_user, l_password, l_clientId, l_keepAliveInterval,
        l_trustStore, l_keyStore, l_privateKey] =
      retrieve_broker_infos(p_configurations);
}

BrokerConfigurations_t TrafficWarden::retrieve_broker_infos(
    const nlohmann::json& p_configurations) {
  BrokerConfigurations_t l_result;

  // mandatory fields
  std::string l_brokerAddress = "";
  if (p_configurations.contains("brokerAddress"))
    l_brokerAddress = p_configurations.at("brokerAddress").get<std::string>();
  else
    throw;

  std::string l_brokerUser = "";
  if (p_configurations.contains("brokerUser"))
    l_brokerUser = p_configurations.at("brokerUser").get<std::string>();
  else
    throw;

  std::string l_brokerPassword = "";
  if (p_configurations.contains("brokerPassword"))
    l_brokerPassword = p_configurations.at("brokerPassword").get<std::string>();
  else
    throw;

  // optional fields
  std::optional<std::string> l_clientId;
  if (p_configurations.contains("clientId"))
    l_clientId = p_configurations.at("clientId").get<std::string>();

  std::optional<int> l_keepAliveInterval;
  if (p_configurations.contains("keepAliveInterval"))
    l_keepAliveInterval = p_configurations.at("keepAliveInterval").get<int>();

  std::optional<std::string> l_trustStore;
  std::optional<std::string> l_keyStore;
  std::optional<std::string> l_privateKey;
  if (p_configurations.contains("ssl") &&
      true == p_configurations.at("ssl").get<bool>()) {
    if (p_configurations.contains("trustStore"))
      l_trustStore = p_configurations.at("trustStore").get<std::string>();
    if (p_configurations.contains("keyStore"))
      l_keyStore = p_configurations.at("keyStore").get<std::string>();
    if (p_configurations.contains("privateKey"))
      l_privateKey = p_configurations.at("privateKey").get<std::string>();
  }

  return std::tie(l_brokerAddress, l_brokerUser, l_brokerPassword, l_clientId,
                  l_keepAliveInterval, l_trustStore, l_keyStore, l_privateKey);
}
}  // namespace tw
