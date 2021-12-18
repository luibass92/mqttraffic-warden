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
      l_inputTopics.push_back(it_route.first);
    }
    std::thread t1(&TrafficWarden::ingest, this);
    std::thread t2(&TrafficWarden::transform, this);
    std::thread t3(&TrafficWarden::publish, this);

    m_mqttClient->initCallback(l_inputTopics, &m_incomingQueue);
    m_mqttClient->connect();

    if (t1.joinable()) t1.join();
    if (t2.joinable()) t2.join();
    if (t3.joinable()) t3.join();
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

bool TrafficWarden::is_valid_route(const nlohmann::json& p_route) {
  if (!p_route.contains(k_routeName)) {
    spdlog::error("Key '{}' is mandatory", k_routeName);
    return false;
  } else if (!p_route.at(k_routeName).is_string()) {
    spdlog::error("Key '{}' must be a string", k_routeName);
    return false;
  } else if (!p_route.contains(k_routeInputTopic)) {
    spdlog::error("Key '{}' is mandatory", k_routeInputTopic);
    return false;
  } else if (!p_route.at(k_routeInputTopic).is_string()) {
    spdlog::error("Key '{}' must be a string", k_routeInputTopic);
    return false;
  } else if (!p_route.contains(k_routeOutputTopic)) {
    spdlog::error("Key '{}' is mandatory", k_routeOutputTopic);
    return false;
  } else if (!p_route.at(k_routeOutputTopic).is_string()) {
    spdlog::error("Key '{}' must be a string", k_routeOutputTopic);
    return false;
  }

  return true;
}

bool TrafficWarden::is_valid_stream_transformer(
    const nlohmann::json& p_streamTransformer) {
  if (!p_streamTransformer.contains(k_transformerType)) {
    spdlog::error(
        "Found a stream transformer without mandatory '{}' "
        "attribute.",
        k_transformerType);
    return false;
  } else if (!p_streamTransformer.at(k_transformerType).is_string()) {
    spdlog::error("Stream transformer '{}' attribute must be a string");
    return false;
  }

  return true;
}

RouteConfigurations_t TrafficWarden::retrieve_routes(
    const nlohmann::json& p_routes) {
  RouteConfigurations_t l_routeConfigurations;

  if (!p_routes.is_array()) {
    spdlog::error("Routes must be a JSON array");
    throw TrafficWardenInitializationException();
  } else if (p_routes.empty()) {
    spdlog::warn("Routes is an empty array, nothing to do");
  } else {
    for (auto it_route = p_routes.begin(); it_route != p_routes.end();
         ++it_route) {
      // validate route
      if (!is_valid_route(*it_route)) {
        spdlog::error("Route validation failed");
        throw TrafficWardenInitializationException();
      }

      RouteProperties_t l_routeProperties;
      l_routeProperties.name = it_route->at(k_routeName);
      l_routeProperties.outputTopic = it_route->at(k_routeOutputTopic);

      std::string l_inputTopic = it_route->at(k_routeInputTopic);

      // retrieve stream transformers if present
      if (!it_route->contains(k_streamTransformers)) {
        spdlog::info("No stream transformer configured for route {}",
                     l_routeProperties.name);
      } else if (!it_route->at(k_streamTransformers).is_array()) {
        spdlog::error("Key '{}' must be an array", k_streamTransformers);
        throw TrafficWardenInitializationException();
      } else {
        // stream transformers are present
        nlohmann::json l_streamTransformers =
            it_route->at(k_streamTransformers);
        for (auto it_streamTransformer = l_streamTransformers.begin();
             it_streamTransformer != l_streamTransformers.end();
             ++it_streamTransformer) {
          // validate stream transformer
          if (!is_valid_stream_transformer(*it_streamTransformer)) {
            spdlog::error("Stream Transformer validation failed");
            throw TrafficWardenInitializationException();
          }

          // retrieve stream transformer
          std::shared_ptr<StreamTransformer> l_streamTransformer;
          if (k_topicToPayload == it_streamTransformer->at(k_transformerType)) {
            l_streamTransformer =
                std::make_shared<StreamTransformerTopicToPayload>();
          } else if (k_topicToTopic ==
                     it_streamTransformer->at(k_transformerType)) {
            l_streamTransformer =
                std::make_shared<StreamTransformerTopicToTopic>();
          } else if (k_payloadToTopic ==
                     it_streamTransformer->at(k_transformerType)) {
            l_streamTransformer =
                std::make_shared<StreamTransformerPayloadToTopic>();
          } else if (k_payloadToPayload ==
                     it_streamTransformer->at(k_transformerType)) {
            l_streamTransformer =
                std::make_shared<StreamTransformerPayloadToPayload>();
          } else {
            spdlog::error("'{}' must have one of the following values: {}",
                          k_transformerType,
                          fmt::join(k_streamTransformerTypes, ", "));
            throw TrafficWardenInitializationException();
          }
          l_streamTransformer->setup((*it_streamTransformer));
          l_routeProperties.streamTransformers.push_back(l_streamTransformer);
        }
      }

      if (l_routeConfigurations.find(l_inputTopic) !=
          l_routeConfigurations.end()) {
        l_routeConfigurations.at(l_inputTopic).push_back(l_routeProperties);
      } else {
        l_routeConfigurations.insert(
            {l_inputTopic, std::list<RouteProperties_t>{l_routeProperties}});
      }
    }
  }

  return l_routeConfigurations;
}

void TrafficWarden::ingest() {
  while (true) {
    while (!m_incomingQueue.empty()) {
      std::pair<std::string, nlohmann::json> l_message;
      if (m_incomingQueue.try_pop(l_message)) {
        const std::string l_inputTopic = l_message.first;
        const nlohmann::json l_inputPayload = l_message.second;
        nlohmann::json l_outputPayload = l_inputPayload;
        auto l_route = m_routes.find(l_inputTopic);
        if (l_route == m_routes.end()) {
          spdlog::warn(
              "Incoming message for topic '{}' does not have any associated "
              "routing, ignoring it",
              l_inputTopic);
          continue;
        }

        const std::list<RouteProperties_t> l_routeProperties = l_route->second;
        if (l_routeProperties.empty()) {
          spdlog::warn(
              "Incoming message for topic '{}' has associated an empty list "
              "of routing configurations, ignoring it",
              l_inputTopic);
          continue;
        }

        for (auto it_routeProperty : l_routeProperties) {
          std::string l_outputTopic = it_routeProperty.outputTopic;
          std::list<std::shared_ptr<StreamTransformer>> l_streamTransformers =
              it_routeProperty.streamTransformers;
          if (!l_streamTransformers.empty()) {
            // pass stream transformers data to transformer thread
            m_transformQueue.push(std::make_tuple(
                l_streamTransformers, l_inputTopic, l_inputPayload,
                l_outputTopic, l_outputPayload));
          } else {
            // pass output topic and payload to publisher thread
            m_outgoingQueue.push(
                std::make_pair(l_outputTopic, l_outputPayload));
          }
        }
      }
    }
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void TrafficWarden::transform() {
  while (true) {
    while (!m_transformQueue.empty()) {
      std::pair<std::string, nlohmann::json> l_message;
      std::tuple<std::list<std::shared_ptr<StreamTransformer>>, std::string,
                 nlohmann::json, std::string, nlohmann::json>
          l_dataToTransform;
      if (m_transformQueue.try_pop(l_dataToTransform)) {
        // execute stream transformers if present
        std::list<std::shared_ptr<StreamTransformer>> l_streamTransformers =
            std::get<static_cast<int>(
                TransformQueueEnum::StreamTransformerList)>(l_dataToTransform);
        if (l_streamTransformers.empty()) {
          continue;
        }

        std::string l_inputTopic =
            std::get<static_cast<int>(TransformQueueEnum::InputTopic)>(
                l_dataToTransform);
        nlohmann::json l_inputPayload =
            std::get<static_cast<int>(TransformQueueEnum::InputPayload)>(
                l_dataToTransform);
        std::string l_outputTopic =
            std::get<static_cast<int>(TransformQueueEnum::OutputTopic)>(
                l_dataToTransform);
        nlohmann::json l_outputPayload =
            std::get<static_cast<int>(TransformQueueEnum::OutputPayload)>(
                l_dataToTransform);
        for (auto it_streamTransformer : l_streamTransformers) {
          it_streamTransformer->execute(l_inputTopic, l_inputPayload,
                                        l_outputTopic, l_outputPayload);
        }
        m_outgoingQueue.push(std::make_pair(l_outputTopic, l_outputPayload));
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

void TrafficWarden::publish() {
  while (true) {
    while (!m_outgoingQueue.empty()) {
      std::pair<std::string, nlohmann::json> l_message;
      if (m_outgoingQueue.try_pop(l_message)) {
        m_mqttClient->publish(l_message.first, l_message.second.dump(),
                              k_defaultQos, false);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

}  // namespace tw
