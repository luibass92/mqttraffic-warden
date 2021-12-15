#ifndef TRAFFIC_WARDEN_H
#define TRAFFIC_WARDEN_H

#include <list>
#include <optional>

#include "nlohmann/json.hpp"
#include "tbb/concurrent_queue.h"

namespace tw {

class MqttClient;
class StreamTransformer;

typedef std::tuple<std::string, std::string, std::string, std::string, int,
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
  inline static std::string const k_brokerAddress = "brokerAddress";
  inline static std::string const k_brokerPassword = "brokerPassword";
  inline static std::string const k_brokerUser = "brokerUser";
  inline static std::string const k_clientId = "clientId";
  inline static std::string const k_keepAliveInterval = "keepAliveInterval";
  inline static std::string const k_ssl = "ssl";
  inline static std::string const k_trustStore = "trustStore";
  inline static std::string const k_keyStore = "keyStore";
  inline static std::string const k_privateKey = "privateKey";

  inline static std::string const k_routes = "routes";
  inline static std::string const k_routeName = "name";
  inline static std::string const k_routeInputTopic = "inputTopic";
  inline static std::string const k_routeOutputTopic = "outputTopic";

  inline static std::string const k_streamTransformers = "streamTransformers";
  inline static std::string const k_transformerType = "type";
  inline static std::string const k_topicToTopic = "topicToTopic";
  inline static std::string const k_topicToPayload = "topicToPayload";
  inline static std::string const k_payloadToTopic = "payloadToTopic";
  inline static std::string const k_payloadToPayload = "payloadToPayload";
  inline static std::vector<std::string> const k_streamTransformerTypes = {
      k_topicToTopic, k_topicToPayload, k_payloadToTopic, k_payloadToPayload};

  inline static std::string const k_defaultClientId = "";
  inline static int const k_defaultKeepAliveInterval = 60;
  inline static int const k_defaultQos = 2;

  BrokerConfigurations_t retrieve_broker_infos(
      const nlohmann::json& p_configurations);

  RouteConfigurations_t retrieve_routes(const nlohmann::json& p_routes);

  void transform();
  void publish();

  RouteConfigurations_t m_routes;
  std::shared_ptr<MqttClient> m_mqttClient;

  std::unordered_map<std::string, std::string> m_topicToRoute;
  tbb::concurrent_queue<std::pair<std::string, nlohmann::json>>
      m_streamTransformerQueue;
  tbb::concurrent_queue<std::pair<std::string, nlohmann::json>>
      m_publisherQueue;
  std::thread m_streamTransformerDataProducer;
};

}  // namespace tw

#endif  // TRAFFIC_WARDEN_H
