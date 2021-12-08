#include "traffic_warden.h"

#include "mqtt_client.h"
#include "spdlog/spdlog.h"
#include "stream_transformer_topic_to_payload.h"

namespace tw {

TrafficWarden::TrafficWarden() {}
TrafficWarden::~TrafficWarden(){};

void TrafficWarden::init(const nlohmann::json& p_configurations) {
  auto [l_host, l_user, l_password, l_clientId, l_keepAliveInterval,
        l_trustStore, l_keyStore, l_privateKey] =
      retrieve_broker_infos(p_configurations);
  if (l_trustStore.has_value() && l_keyStore.has_value() &&
      l_privateKey.has_value()) {
    // with ssl
    m_mqttClient = std::make_shared<MqttClient>(
        l_host, l_clientId.has_value() ? l_clientId.value() : "", l_user,
        l_password,
        l_keepAliveInterval.has_value() ? l_keepAliveInterval.value() : 60,
        l_trustStore.value(), l_keyStore.value(), l_privateKey.value());
  } else {
    // without ssl
    m_mqttClient = std::make_shared<MqttClient>(
        l_host, l_clientId.has_value() ? l_clientId.value() : "", l_user,
        l_password,
        l_keepAliveInterval.has_value() ? l_keepAliveInterval.value() : 60);
  }

  m_routes = retrieve_routes(p_configurations.at("routes"));

  std::list<std::string> l_inputTopics;
  for (auto it_route : m_routes) {
    l_inputTopics.push_back(std::get<0>(it_route.second));
    m_topicToRoute.insert({std::get<0>(it_route.second), it_route.first});
  }
  std::thread t1(&TrafficWarden::transform, this);
  std::thread t2(&TrafficWarden::publish, this);

  m_mqttClient->initCallback(l_inputTopics, &m_streamTransformerQueue);
  m_mqttClient->connect();

  if (t1.joinable()) t1.join();
  if (t2.joinable()) t2.join();
}

BrokerConfigurations_t TrafficWarden::retrieve_broker_infos(
    const nlohmann::json& p_configurations) {
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
  std::optional<std::string> l_clientId = std::nullopt;
  if (p_configurations.contains("clientId"))
    l_clientId = p_configurations.at("clientId").get<std::string>();

  std::optional<int> l_keepAliveInterval = std::nullopt;
  if (p_configurations.contains("keepAliveInterval"))
    l_keepAliveInterval = p_configurations.at("keepAliveInterval").get<int>();

  std::optional<std::string> l_trustStore = std::nullopt;
  std::optional<std::string> l_keyStore = std::nullopt;
  std::optional<std::string> l_privateKey = std::nullopt;
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

RouteConfigurations_t TrafficWarden::retrieve_routes(
    const nlohmann::json& p_routes) {
  RouteConfigurations_t l_result;

  if (!p_routes.is_array()) {
    /* complain */
  } else if (p_routes.empty()) {
    /* complain */
  } else {
    for (auto it = p_routes.begin(); it != p_routes.end(); ++it) {
      if (l_result.find((*it).at("name")) != l_result.end()) {
        spdlog::warn("Route with name {} already present, ignoring it.",
                     (*it).at("name"));
      } else {
        std::string l_name = (*it).at("name");
        std::string l_inputTopic = (*it).at("inputTopic");
        std::string l_outputTopic = (*it).at("outputTopic");
        std::list<std::shared_ptr<StreamTransformer>> l_streamTransformers;
        if ((*it).contains("topicToPayload")) {
          std::shared_ptr<StreamTransformerTopicToPayload> l_streamTransformer =
              std::make_shared<StreamTransformerTopicToPayload>();
          l_streamTransformer->setup((*it).at("topicToPayload"));
          l_streamTransformers.push_back(l_streamTransformer);
        }
        if ((*it).contains("topicToTopic")) {
          /* do something */
        }
        if ((*it).contains("payloadToPayload")) {
          /* do something */
        }
        if ((*it).contains("payloadToTopic")) {
          /* do something */
        }

        l_result.insert({l_name, std::make_tuple(l_inputTopic, l_outputTopic,
                                                 l_streamTransformers)});
      }
    }
  }

  return l_result;
}

void TrafficWarden::transform() {
  while (true) {
    while (!m_streamTransformerQueue.empty()) {
      std::pair<std::string, nlohmann::json> l_message;
      if (m_streamTransformerQueue.try_pop(l_message)) {
        std::cout
            << fmt::format(
                   "Transform message dequeued --> topic: {} | payload: {}",
                   l_message.first, l_message.second.dump())
            << std::endl;
        if (m_topicToRoute.find(l_message.first) != m_topicToRoute.end()) {
          std::string l_route = m_topicToRoute.at(l_message.first);
          auto [l_inputTopic, l_outputTopic, l_streamTransformers] =
              m_routes.at(l_route);
          for (auto l_streamTransformer : l_streamTransformers) {
            nlohmann::json l_outputPayload;
            l_streamTransformer->execute(l_inputTopic, l_message.second,
                                         l_outputTopic, l_outputPayload);
            std::cout << l_outputPayload << std::endl;
            m_publisherQueue.push(
                std::make_pair(l_outputTopic, l_outputPayload));
          }
        }
      }
    }
    spdlog::trace("Transform thread sleeping for 1 sec");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

void TrafficWarden::publish() {
  while (true) {
    while (!m_publisherQueue.empty()) {
      std::pair<std::string, nlohmann::json> l_message;
      if (m_publisherQueue.try_pop(l_message)) {
        std::cout << fmt::format(
                         "Publish message dequeued --> topic: {} | payload: {}",
                         l_message.first, l_message.second.dump())
                  << std::endl;
        m_mqttClient->publish(l_message.first, l_message.second.dump(), 2,
                              false);
      }
    }
    spdlog::trace("Publish thread sleeping for 1 sec");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

}  // namespace tw
