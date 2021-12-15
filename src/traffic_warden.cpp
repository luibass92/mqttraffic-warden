#include "traffic_warden.h"

#include "exceptions.h"
#include "mqtt_client.h"
#include "spdlog/spdlog.h"
#include "stream_transformer_payload_to_payload.h"
#include "stream_transformer_payload_to_topic.h"
#include "stream_transformer_topic_to_payload.h"
#include "stream_transformer_topic_to_topic.h"
#include "utilities.h"

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
        l_host, l_clientId, l_user, l_password, l_keepAliveInterval,
        l_trustStore.value(), l_keyStore.value(), l_privateKey.value());
  } else {
    // without ssl
    m_mqttClient = std::make_shared<MqttClient>(
        l_host, l_clientId, l_user, l_password, l_keepAliveInterval);
  }

  m_routes = retrieve_routes(p_configurations.at(k_routes));

  if (!m_routes.empty()) {
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
}

BrokerConfigurations_t TrafficWarden::retrieve_broker_infos(
    const nlohmann::json& p_configurations) {
  // mandatory fields
  std::string l_brokerAddress = "";
  if (!p_configurations.contains(k_brokerAddress)) {
    spdlog::error("The key '{}' is mandatory", k_brokerAddress);
    throw TrafficWardenInitializationException();
  } else if (!p_configurations.at(k_brokerAddress).is_string()) {
    spdlog::error("The key '{}' must be a string", k_brokerAddress);
    throw TrafficWardenInitializationException();
  } else {
    l_brokerAddress = p_configurations.at(k_brokerAddress).get<std::string>();
    if (!utilities::isValidIp(l_brokerAddress)) {
      spdlog::error("Broker IP address '{}' must be a valid IPv4 or IPv6",
                    l_brokerAddress);
      throw TrafficWardenInitializationException();
    }
  }

  std::string l_brokerUser = "";
  if (!p_configurations.contains(k_brokerUser)) {
    spdlog::error("The key '{}' is mandatory", k_brokerUser);
    throw TrafficWardenInitializationException();
  } else if (!p_configurations.at(k_brokerUser).is_string()) {
    spdlog::error("The key '{}' must be a string", k_brokerUser);
    throw TrafficWardenInitializationException();
  } else {
    l_brokerUser = p_configurations.at(k_brokerUser).get<std::string>();
  }

  std::string l_brokerPassword = "";
  if (!p_configurations.contains(k_brokerPassword)) {
    spdlog::error("The key '{}' is mandatory", k_brokerPassword);
    throw TrafficWardenInitializationException();
  } else if (!p_configurations.at(k_brokerPassword).is_string()) {
    spdlog::error("The key '{}' must be a string", k_brokerPassword);
    throw TrafficWardenInitializationException();
  } else {
    l_brokerPassword = p_configurations.at(k_brokerPassword).get<std::string>();
  }

  std::string l_clientId = k_defaultClientId;
  if (p_configurations.contains(k_clientId))
    l_clientId = p_configurations.at(k_clientId).get<std::string>();

  int l_keepAliveInterval = k_defaultKeepAliveInterval;
  if (p_configurations.contains(k_keepAliveInterval))
    l_keepAliveInterval = p_configurations.at(k_keepAliveInterval).get<int>();

  // optional fields
  std::optional<std::string> l_trustStore = std::nullopt;
  std::optional<std::string> l_keyStore = std::nullopt;
  std::optional<std::string> l_privateKey = std::nullopt;
  if (!p_configurations.contains(k_ssl)) {
    spdlog::error("The key '{}' is mandatory", k_ssl);
    throw TrafficWardenInitializationException();
  } else if (!p_configurations.at(k_ssl).is_boolean()) {
    spdlog::error("The key '{}' must be a boolean", k_ssl);
    throw TrafficWardenInitializationException();
  } else if (true == p_configurations.at(k_ssl).get<bool>()) {
    if (!p_configurations.contains(k_trustStore)) {
      spdlog::error("Since '{}' is set to '{}', the field '{}' is mandatory",
                    k_ssl, p_configurations.at(k_ssl), k_trustStore);
      throw TrafficWardenInitializationException();
    } else if (!p_configurations.at(k_trustStore).is_string()) {
      spdlog::error("The key '{}' must be a string", k_trustStore);
      throw TrafficWardenInitializationException();
    } else if (!p_configurations.contains(k_keyStore)) {
      spdlog::error("Since '{}' is set to '{}', the field '{}' is mandatory",
                    k_ssl, p_configurations.at(k_ssl), k_keyStore);
      throw TrafficWardenInitializationException();
    } else if (!p_configurations.at(k_keyStore).is_string()) {
      spdlog::error("The key '{}' must be a string", k_keyStore);
      throw TrafficWardenInitializationException();
    } else if (!p_configurations.contains(k_privateKey)) {
      spdlog::error("Since '{}' is set to '{}', the field '{}' is mandatory",
                    k_ssl, p_configurations.at(k_ssl), k_privateKey);
      throw TrafficWardenInitializationException();
    } else if (!p_configurations.at(k_privateKey).is_string()) {
      spdlog::error("The key '{}' must be a string", k_privateKey);
      throw TrafficWardenInitializationException();
    } else {
      l_trustStore = p_configurations.at(k_trustStore).get<std::string>();
      l_keyStore = p_configurations.at(k_keyStore).get<std::string>();
      l_privateKey = p_configurations.at(k_privateKey).get<std::string>();
    }
  }

  return std::tie(l_brokerAddress, l_brokerUser, l_brokerPassword, l_clientId,
                  l_keepAliveInterval, l_trustStore, l_keyStore, l_privateKey);
}

RouteConfigurations_t TrafficWarden::retrieve_routes(
    const nlohmann::json& p_routes) {
  RouteConfigurations_t l_result;

  if (!p_routes.is_array()) {
    spdlog::error("Routes must be a JSON array");
    throw TrafficWardenInitializationException();
  } else if (p_routes.empty()) {
    spdlog::warn("Routes is an empty array, nothing to do");
  } else {
    for (auto it_route = p_routes.begin(); it_route != p_routes.end();
         ++it_route) {
      if (l_result.find((*it_route).at(k_routeName)) != l_result.end()) {
        spdlog::warn("Route with name {} already present, ignoring it.",
                     (*it_route).at(k_routeName));
      } else {
        std::string l_name = (*it_route).at(k_routeName);
        std::string l_inputTopic = (*it_route).at(k_routeInputTopic);
        std::string l_outputTopic = (*it_route).at(k_routeOutputTopic);
        std::list<std::shared_ptr<StreamTransformer>> l_streamTransformers;
        if ((*it_route).contains(k_routeTopicToPayload)) {
          nlohmann::json l_topicToPayloads =
              (*it_route).at(k_routeTopicToPayload);

          for (auto jt = l_topicToPayloads.begin();
               jt != l_topicToPayloads.end(); ++jt) {
            std::shared_ptr<StreamTransformer> l_streamTransformer =
                std::make_shared<StreamTransformerTopicToPayload>();
            l_streamTransformer->setup((*jt));
            l_streamTransformers.push_back(l_streamTransformer);
          }
        }
        if ((*it_route).contains(k_routeTopicToTopic)) {
          nlohmann::json l_topicToTopic = (*it_route).at(k_routeTopicToTopic);

          for (auto jt = l_topicToTopic.begin(); jt != l_topicToTopic.end();
               ++jt) {
            std::shared_ptr<StreamTransformer> l_streamTransformer =
                std::make_shared<StreamTransformerTopicToTopic>();
            l_streamTransformer->setup((*jt));
            l_streamTransformers.push_back(l_streamTransformer);
          }
        }
        if ((*it_route).contains(k_routePayloadToPayload)) {
          nlohmann::json l_topicToTopic =
              (*it_route).at(k_routePayloadToPayload);

          for (auto jt = l_topicToTopic.begin(); jt != l_topicToTopic.end();
               ++jt) {
            std::shared_ptr<StreamTransformer> l_streamTransformer =
                std::make_shared<StreamTransformerPayloadToPayload>();
            l_streamTransformer->setup((*jt));
            l_streamTransformers.push_back(l_streamTransformer);
          }
        }
        if ((*it_route).contains(k_routePayloadToTopic)) {
          nlohmann::json l_topicToTopic = (*it_route).at(k_routePayloadToTopic);

          for (auto jt = l_topicToTopic.begin(); jt != l_topicToTopic.end();
               ++jt) {
            std::shared_ptr<StreamTransformer> l_streamTransformer =
                std::make_shared<StreamTransformerPayloadToTopic>();
            l_streamTransformer->setup((*jt));
            l_streamTransformers.push_back(l_streamTransformer);
          }
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
        nlohmann::json l_inputPayload = l_message.second;
        if (m_topicToRoute.find(l_message.first) != m_topicToRoute.end()) {
          std::string l_route = m_topicToRoute.at(l_message.first);
          auto [l_inputTopic, l_outputTopic, l_streamTransformers] =
              m_routes.at(l_route);
          // output payload should be equal to input payload at beginning
          nlohmann::json l_outputPayload = l_inputPayload;
          for (auto l_streamTransformer : l_streamTransformers) {
            l_streamTransformer->execute(l_inputTopic, l_inputPayload,
                                         l_outputTopic, l_outputPayload);
          }
          m_publisherQueue.push(std::make_pair(l_outputTopic, l_outputPayload));
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

void TrafficWarden::publish() {
  while (true) {
    while (!m_publisherQueue.empty()) {
      std::pair<std::string, nlohmann::json> l_message;
      if (m_publisherQueue.try_pop(l_message)) {
        m_mqttClient->publish(l_message.first, l_message.second.dump(), 2,
                              false);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

}  // namespace tw
