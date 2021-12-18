#ifndef TRAFFIC_WARDEN_H
#define TRAFFIC_WARDEN_H

#include <list>
#include <optional>

#include "nlohmann/json.hpp"
#include "tbb/concurrent_queue.h"

namespace tw {

class MqttClient;
class StreamTransformer;

typedef struct {
  std::string host;
  std::string user;
  std::string password;
  std::string clientId;
  int keepAliveInterval;
  std::optional<std::string> trustStore;
  std::optional<std::string> keyStore;
  std::optional<std::string> privateKey;
} BrokerConfiguration_t;

/* < name, <inputTopic, outputTopic, <listOfStransformers>>> */
// typedef std::unordered_map<
//     std::string, std::tuple<std::string, std::string,
//                             std::list<std::shared_ptr<StreamTransformer>>>>
//     RouteConfigurations_t;

typedef struct {
  std::string name;
  std::string outputTopic;
  std::list<std::shared_ptr<StreamTransformer>> streamTransformers;
} RouteProperties_t;
/*
 *   < inputTopic1, [
 *                  (name1, outputTopic1, [ST11, ST12, ...]),
 *                  (name, 2outputTopic2, [ST21, ST22, ...])
 *                 ],
 *     inputTopic2, [
 *                  (name3, outputTopic3, [ ]),
 *                  (name4, outputTopic4, [ST41])
 *                 ]
 *    >
 */
typedef std::unordered_map<std::string, std::list<RouteProperties_t>>
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

  BrokerConfiguration_t retrieve_broker_infos(
      const nlohmann::json& p_configurations);
  void configure_mqtt_client(
      const BrokerConfiguration_t& p_brokerConfiguration);

  RouteConfigurations_t retrieve_routes(const nlohmann::json& p_routes);
  bool is_valid_route(const nlohmann::json& p_route);
  bool is_valid_stream_transformer(const nlohmann::json& p_streamTransformer);

  void ingest();
  void transform();
  void publish();

  RouteConfigurations_t m_routes;
  std::shared_ptr<MqttClient> m_mqttClient;

  tbb::concurrent_queue<std::pair<std::string, nlohmann::json>> m_incomingQueue;
  tbb::concurrent_queue<std::pair<std::string, nlohmann::json>> m_outgoingQueue;
  tbb::concurrent_queue<
      std::tuple<std::list<std::shared_ptr<StreamTransformer>>, std::string,
                 nlohmann::json, std::string, nlohmann::json>>
      m_transformQueue;

  enum class TransformQueueEnum {
    StreamTransformerList = 0,
    InputTopic = 1,
    InputPayload = 2,
    OutputTopic = 3,
    OutputPayload = 4
  };
};

}  // namespace tw

#endif  // TRAFFIC_WARDEN_H
