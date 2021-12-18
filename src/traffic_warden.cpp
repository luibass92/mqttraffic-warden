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
  // get broker configuration from configuration JSON file
  BrokerConfiguration_t l_brokerConfiguration =
      retrieve_broker_infos(p_configurations);

  // use the retrieved configuration to set up the MQTT client
  configure_mqtt_client(l_brokerConfiguration);

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
  } else {
    spdlog::warn("No routes have been defined, exiting.");
  }
}

BrokerConfiguration_t TrafficWarden::retrieve_broker_infos(
    const nlohmann::json& p_configurations) {
  BrokerConfiguration_t l_brokerConfigurations;
  // mandatory fields
  if (!p_configurations.contains(k_brokerAddress)) {
    spdlog::error("The key '{}' is mandatory", k_brokerAddress);
    throw TrafficWardenInitializationException();
  } else if (!p_configurations.at(k_brokerAddress).is_string()) {
    spdlog::error("The key '{}' must be a string", k_brokerAddress);
    throw TrafficWardenInitializationException();
  } else if (!utilities::isValidIp(
                 p_configurations.at(k_brokerAddress).get<std::string>())) {
    spdlog::error("Broker IP address '{}' must be a valid IPv4 or IPv6",
                  p_configurations.at(k_brokerAddress).get<std::string>());
    throw TrafficWardenInitializationException();
  } else {
    l_brokerConfigurations.host =
        p_configurations.at(k_brokerAddress).get<std::string>();
  }

  if (!p_configurations.contains(k_brokerUser)) {
    spdlog::error("The key '{}' is mandatory", k_brokerUser);
    throw TrafficWardenInitializationException();
  } else if (!p_configurations.at(k_brokerUser).is_string()) {
    spdlog::error("The key '{}' must be a string", k_brokerUser);
    throw TrafficWardenInitializationException();
  } else {
    l_brokerConfigurations.user =
        p_configurations.at(k_brokerUser).get<std::string>();
  }

  if (!p_configurations.contains(k_brokerPassword)) {
    spdlog::error("The key '{}' is mandatory", k_brokerPassword);
    throw TrafficWardenInitializationException();
  } else if (!p_configurations.at(k_brokerPassword).is_string()) {
    spdlog::error("The key '{}' must be a string", k_brokerPassword);
    throw TrafficWardenInitializationException();
  } else {
    l_brokerConfigurations.password =
        p_configurations.at(k_brokerPassword).get<std::string>();
  }

  if (p_configurations.contains(k_clientId) &&
      !p_configurations.at(k_clientId).is_string()) {
    spdlog::error("The key '{}' must be a string", k_clientId);
    throw TrafficWardenInitializationException();
  } else if (p_configurations.contains(k_clientId)) {
    l_brokerConfigurations.clientId =
        p_configurations.at(k_clientId).get<std::string>();
  } else {
    l_brokerConfigurations.clientId = k_defaultClientId;
  }

  if (p_configurations.contains(k_keepAliveInterval) &&
      !p_configurations.at(k_keepAliveInterval).is_number_unsigned()) {
    spdlog::error("The key '{}' must be an unsigned number",
                  k_keepAliveInterval);
    throw TrafficWardenInitializationException();
  } else if (p_configurations.contains(k_keepAliveInterval)) {
    l_brokerConfigurations.keepAliveInterval =
        p_configurations.at(k_keepAliveInterval).get<int>();
  } else {
    l_brokerConfigurations.keepAliveInterval = k_defaultKeepAliveInterval;
  }

  bool l_ssl = false;
  if (!p_configurations.contains(k_ssl)) {
    spdlog::error("The key '{}' is mandatory", k_ssl);
    throw TrafficWardenInitializationException();
  } else if (!p_configurations.at(k_ssl).is_boolean()) {
    spdlog::error("The key '{}' must be a boolean", k_ssl);
    throw TrafficWardenInitializationException();
  } else {
    l_ssl = p_configurations.at(k_ssl).get<bool>();
  }

  // in case ssl is true, trustStore, keyStore and privateKey must be set
  // otherwise they are set to std::nullopt
  l_brokerConfigurations.trustStore = std::nullopt;
  l_brokerConfigurations.keyStore = std::nullopt;
  l_brokerConfigurations.privateKey = std::nullopt;
  if (l_ssl) {
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
      l_brokerConfigurations.trustStore =
          p_configurations.at(k_trustStore).get<std::string>();
      l_brokerConfigurations.keyStore =
          p_configurations.at(k_keyStore).get<std::string>();
      l_brokerConfigurations.privateKey =
          p_configurations.at(k_privateKey).get<std::string>();
    }
  }

  return l_brokerConfigurations;
}

void TrafficWarden::configure_mqtt_client(
    const BrokerConfiguration_t& p_brokerConfiguration) {
  if (p_brokerConfiguration.trustStore.has_value() &&
      p_brokerConfiguration.keyStore.has_value() &&
      p_brokerConfiguration.privateKey.has_value()) {
    // with ssl
    m_mqttClient = std::make_shared<MqttClient>(
        p_brokerConfiguration.host, p_brokerConfiguration.clientId,
        p_brokerConfiguration.user, p_brokerConfiguration.password,
        p_brokerConfiguration.keepAliveInterval,
        p_brokerConfiguration.trustStore.value(),
        p_brokerConfiguration.keyStore.value(),
        p_brokerConfiguration.privateKey.value());
  } else {
    // without ssl
    m_mqttClient = std::make_shared<MqttClient>(
        p_brokerConfiguration.host, p_brokerConfiguration.clientId,
        p_brokerConfiguration.user, p_brokerConfiguration.password,
        p_brokerConfiguration.keepAliveInterval);
  }
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
        if (!(*it_route).contains(k_streamTransformers)) {
          spdlog::info("No stream transformer configured for route {}", l_name);
        } else if (!(*it_route).at(k_streamTransformers).is_array()) {
          spdlog::error("streamTransformers field must be an array");
        } else {
          std::list<std::shared_ptr<StreamTransformer>> l_streamTransformerList;
          nlohmann::json l_streamTransformers =
              (*it_route).at(k_streamTransformers);
          for (auto it_streamTransformer = l_streamTransformers.begin();
               it_streamTransformer != l_streamTransformers.end();
               ++it_streamTransformer) {
            if (!it_streamTransformer->contains(k_transformerType)) {
              spdlog::error(
                  "Found a stream transformer without mandatory '{}' "
                  "attribute, ignoring it.",
                  k_transformerType);
            } else if (!(*it_streamTransformer)
                            .at(k_transformerType)
                            .is_string()) {
              spdlog::error(
                  "Stream transformer '{}' attribute must be a string");
            } else if (std::none_of(
                           k_streamTransformerTypes.begin(),
                           k_streamTransformerTypes.end(),
                           [&it_streamTransformer](std::string it_transformer) {
                             return it_transformer ==
                                    (*it_streamTransformer)
                                        .at(k_transformerType);
                           })) {
              spdlog::error("'{}' must have one of the following values: {}",
                            k_transformerType,
                            fmt::join(k_streamTransformerTypes, ", "));
            } else {
              std::shared_ptr<StreamTransformer> l_streamTransformer;
              if (k_topicToPayload ==
                  (*it_streamTransformer).at(k_transformerType)) {
                l_streamTransformer =
                    std::make_shared<StreamTransformerTopicToPayload>();
              }
              if (k_topicToTopic ==
                  (*it_streamTransformer).at(k_transformerType)) {
                l_streamTransformer =
                    std::make_shared<StreamTransformerTopicToTopic>();
              }
              if (k_payloadToTopic ==
                  (*it_streamTransformer).at(k_transformerType)) {
                l_streamTransformer =
                    std::make_shared<StreamTransformerPayloadToTopic>();
              }
              if (k_payloadToPayload ==
                  (*it_streamTransformer).at(k_transformerType)) {
                l_streamTransformer =
                    std::make_shared<StreamTransformerPayloadToPayload>();
              }
              l_streamTransformer->setup((*it_streamTransformer));
              l_streamTransformerList.push_back(l_streamTransformer);
            }
          }

          l_result.insert({l_name, std::make_tuple(l_inputTopic, l_outputTopic,
                                                   l_streamTransformerList)});
        }
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
        m_mqttClient->publish(l_message.first, l_message.second.dump(),
                              k_defaultQos, false);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

}  // namespace tw
