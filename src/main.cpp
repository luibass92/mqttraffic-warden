#include <atomic>
#include <cassert>
#include <csignal>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "mqtt_client.h"
#include "nlohmann/json-schema.hpp"
#include "spdlog/spdlog.h"

// versioning
#include "version.h"

static const std::string k_programName("MQTTraffic-Warden");

std::atomic_bool globalCntrlExit = {false};

void stopHandler(int sig) {
  if (!globalCntrlExit.load()) {
    // write only the first exit request. Future ctrl+c will be ignored
    // utilities::log_info(constants::info::sys_id, "Exit request received:
    // program stopping");
  }
  // set global flag, threads will (gently) terminate
  globalCntrlExit = true;
  // listening for future SIGINT signals (for angry users!)
  signal(SIGINT, stopHandler);
}

void printVersion() {
  std::cout << R"(
___________              _____  _____.__
\__    ___/___________ _/ ____\/ ____\__| ____    .&%%/   *&%&,
  |    |  \_  __ \__  \\   __\\   __\|  |/ ___\   *@,*%,  %(*&*
  |    |   |  | \// __ \|  |   |  |  |  \  \___   .@/,%* .%/*&,
  |____|   |__|  (____  /__|   |__|  |__|\___  >   %%,%* .%((#.
                      \/                     \/   (&*#@@&@&*## .
                               .%%,             .#&,          ,@%
                             .%%.%@@&*         ,%/   /\   /\    (&,
                           .#%,. /@@@@@@(.     (#.      .        %#.
                          /@(    ,@@@@@@##%/.  (#.     ___       %#
                            ,#&/. %@@&, /%#*(&(,##.            ,&#.
                               ,(&&&*     ./&#,(%#%&(,......*(&(.
                                              /%#*(&%%***/*(&#############(((/,.
                                                 /&%.        *%&&%&&&&&&%%%%%%@@@@@@@@@@&.
                                                 /@,          /@,             #@@@@@@/ (&.
                                                /&,            ,@*            #@@@%.   (&
                                               ,%/              *&,           #&/      (&
                                               *&*               %(           ..,........
                                               ,%*       .       %(
                                               .##.             (&.
                                                .(&*          ,&#.  __      __                  .___
                                                   ,&#*((//(*/&,   /  \    /  \_____ _______  __| _/____   ____
                                                    &##(   /%(%,   \   \/\/   /\__  \\_  __ \/ __ |/ __ \ /    \
                                                    %%%/   *%#%,    \        /  / __ \|  | \/ /_/ \  ___/|   |  \
                                                    (&%(   /&%#.     \__/\  /  (____  /__|  \____ |\___  >___|  /
                                                  ,#&@@&, .%@@@%,         \/        \/           \/    \/     \/
                                                 ,#%&&&%/ *%&&&%%.                      )";

  // put version in log as well
  std::cout << fmt::format(
                   "{}", std::to_string(MQTTrafficWarden_VERSION_MAJOR) + "." +
                             std::to_string(MQTTrafficWarden_VERSION_MINOR) +
                             "." +
                             std::to_string(MQTTrafficWarden_VERSION_PATCH))
            << std::endl;
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

int main() {
  printVersion();

  traffic_warden::MqttClient myClient("10.0.2.15", "", "guest", "guest", true,
                                      true);

  myClient.connect();

  // Just block till user tells us to quit.

  while (std::tolower(std::cin.get()) != 'q')
    ;

  myClient.disconnect();

  return 0;
}
