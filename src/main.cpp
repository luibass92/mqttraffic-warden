#include <atomic>
#include <cassert>
#include <csignal>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "mqtt_client.h"
#include "nlohmann/json-schema.hpp"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "traffic_warden.h"

// versioning
#include "version.h"

static const std::string k_programName("MQTTraffic-Warden");

std::atomic_bool globalCntrlExit = {false};

void stopHandler(int sig) {
  if (!globalCntrlExit.load()) {
    spdlog::info("Exit request received: program stopping");
  }
  // set global flag, threads will gently terminate
  globalCntrlExit = true;
  // listening for future SIGINT signal
  signal(SIGINT, stopHandler);
}

std::string versionStr() {
  return std::to_string(MQTTrafficWarden_VERSION_MAJOR) + "." +
         std::to_string(MQTTrafficWarden_VERSION_MINOR) + "." +
         std::to_string(MQTTrafficWarden_VERSION_PATCH);
}

void printLogo() {
  std::cout << R"(
 _______         __  __ _   __          __           _
 |__   __|       / _|/ _(_)  \ \        / /          | |
    | |_ __ __ _| |_| |_ _  __\ \  /\  / /_ _ _ __ __| | ___ _ __
    | | '__/ _` |  _|  _| |/ __\ \/  \/ / _` | '__/ _` |/ _ \ '_ \
    | | | | (_| | | | | | | (__ \  /\  / (_| | | | (_| |  __/ | | |
    |_|_|  \__,_|_| |_| |_|\___| \/  \/ \__,_|_|  \__,_|\___|_| |_|)";
  std::cout << versionStr();
  std::cout << R"(
                             ..:::.
                        :+#@@%#####%@%*=.
                     :*@#=:.   -*.   .-+%%+.
                   -%%=      ..-+:..     :+@*.
            .     -@+  -+#@@@%%###%%@@%*=. .@@
         -%%#%%*  :@**@#=:            .-+@%+@%
        +@=   .@#  .+%@%.               =@@#-
        *@:    %%    +@%@%%%%%%%%%%%%%%%@%@:
        *@:    %%    #@  ............... +@-
        *@:    %%    +@:                 #@:
        *@:    %%     %%.               =@*
        *@:    %%     .%@-            .+@+
        *@:    +@+++++++%@@*-.    .:=#@@*++++++++++++++===-.
        *@:     .-=======#@**@@@@@@%*%@===================*@%.
        *@:              -@++@%  :@@:#%                    .@*
        =@+......        .@@@=%%+@**@@*                   .*@=
         :*#####%@*       @%. -@@@  =@-      .#@@@@@@@@@@@%*:
                 =@=          #@+@-          #@
                 =@=         :@+ @#          %%
                 =@=         #@: +@.         %%
                 =@=        .@%   @*         %%
                 =@=        -@#. :%%         %%
                 =@=         .*@%@+          %%
                 -@+           .=.           %%
                  =@#***********************@%:
                    .:::::::::::::::::::::::

                                                                )";
  std::cout << std::endl;
}

static void loader(const nlohmann::json_uri& uri, nlohmann::json& schema) {
  const std::string filename = "./" + uri.path();
  std::ifstream lf(filename);

  if (!lf.good()) {
    throw std::invalid_argument("could not open " + uri.url() + " tried with " +
                                filename);
  }

  try {
    lf >> schema;
  } catch (const std::exception& e) {
    throw e;
  }
}

void initLogger(const std::string& p_logLevel) {
  auto l_consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto l_rotatingSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      "logs/traffic_warden.log", 1024 * 1024, 5, false);

  spdlog::sinks_init_list sink_list = {l_rotatingSink, l_consoleSink};

  std::shared_ptr<spdlog::logger> l_logger = std::make_shared<spdlog::logger>(
      "", std::begin(sink_list), std::end(sink_list));
  // l_logger->set_pattern("[%Y-%m-%d %T.%e] [%^%-4!l%$] [%6!t] %v");
  auto l_logLevel = spdlog::level::from_str(p_logLevel);
  if (l_logLevel == spdlog::level::off) {
    spdlog::warn("TrafficWarden log disabled!");
  }
  l_logger->set_level(l_logLevel);

  spdlog::set_default_logger(l_logger);
}

int main() {
  printLogo();
  initLogger("trace");
  spdlog::trace("trace");
  spdlog::debug("debug");
  spdlog::info("info");
  spdlog::warn("warn");
  spdlog::error("error");
  spdlog::critical("critical");

  // tw::TrafficWarden l_trafficWarden(
  //     nlohmann::json::parse(std::ifstream("config.json")));
  tw::TrafficWarden l_trafficWarden;
  auto l_config = std::ifstream("config.json");
  l_trafficWarden.init(nlohmann::json::parse(l_config));

  while (std::tolower(std::cin.get()) != 'q')
    ;
  return 0;
}
