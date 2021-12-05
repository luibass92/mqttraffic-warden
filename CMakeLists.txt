
#
# cmake version
#
cmake_minimum_required(VERSION 3.7)

# flag for linux and not Apple
if(UNIX AND NOT APPLE)
   set(LINUX TRUE)
endif()
#
# The version number.
set (MQTTrafficWarden_VERSION_MAJOR 0)
set (MQTTrafficWarden_VERSION_MINOR 0)
set (MQTTrafficWarden_VERSION_PATCH 1)
#
# variable definitions
#
set(PROJECT_BASENAME "mqttraffic_warden")
set(PROJECT_BASEDIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PROJECT_INCLUDE_DIR "${PROJECT_BASEDIR}/include")
set(PROJECT_SRC_DIR "${PROJECT_BASEDIR}/src")
set(PROJECT_LIB_EXT "${PROJECT_BASEDIR}/third_party")
# paho
set(PAHO_MQTT_C_PATH "${PROJECT_LIB_EXT}/paho.mqtt.c")
set(PAHO_MQTT_CPP_PATH "${PROJECT_LIB_EXT}/paho.mqtt.cpp")
# tbb
set(TBB_PATH "${PROJECT_LIB_EXT}/tbb")
# json-schema-validator
set(JSON_SCHEMA_VALIDATOR_PATH "${PROJECT_LIB_EXT}/json-schema-validator")
# spdlog
set(SPDLOG_PATH "${PROJECT_LIB_EXT}/spdlog")
#
#
#


#
# project name
#
project(${PROJECT_BASENAME})
#
# project general options
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

#
# Adjust C(XX)FLAGS
#
if(UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
endif(UNIX)


#
# debug and release stuff (default is debug)
#
message("CMake Build Type: ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(SHORTNAME_BUILD_TYPE "debug")
    set(EXECUTABLE_FOLDER "Debug")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    set(SHORTNAME_BUILD_TYPE "release")
    set(EXECUTABLE_FOLDER "Release")
elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    set(SHORTNAME_BUILD_TYPE "debug")
    set(EXECUTABLE_FOLDER "RelWithDebInfo")
else()
    message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_INCLUDE_DIR}/version.h.in"
  "${PROJECT_BINARY_DIR}/version.h"
)
#
# dependencies
#

# threads
#
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)



if(WIN32)
    set(SHORTNAME_OS "win")
    set(LIB_OS_SUFFIX ".dll")
elseif(UNIX)
    set(SHORTNAME_OS "linux")
    set(LIB_OS_SUFFIX ".so")
else()
    message(FATAL_ERROR, "Unhandled platform")
endif()


#find_library(PAHO_MQTT_C_LIBRARY NAMES paho-mqtt3c HINTS ${PAHO_MQTT_C_PATH}/lib/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE})
#find_library(PAHO_MQTT_A_LIBRARY NAMES paho-mqtt3a HINTS ${PAHO_MQTT_C_PATH}/lib/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE})
#find_library(PAHO_MQTT_CS_LIBRARY NAMES paho-mqtt3cs HINTS ${PAHO_MQTT_C_PATH}/lib/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE})
#find_library(PAHO_MQTT_AS_LIBRARY NAMES paho-mqtt3as HINTS ${PAHO_MQTT_C_PATH}/lib/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE})
#find_library(PAHO_MQTT_CPP_LIBRARY NAMES paho-mqttpp3 HINTS ${PAHO_MQTT_CPP_PATH}/lib/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE})
#find_library(PAHO_MQTT_C_LIBRARY NAMES paho-mqtt3c HINTS ${PAHO_MQTT_C_PATH}/lib/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE})
#find_library(PAHO_MQTT_A_LIBRARY NAMES paho-mqtt3a HINTS ${PAHO_MQTT_C_PATH}/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/lib)
find_library(PAHO_MQTT_CS_LIBRARY NAMES paho-mqtt3cs HINTS ${PAHO_MQTT_C_PATH}/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/lib)
find_library(PAHO_MQTT_AS_LIBRARY NAMES paho-mqtt3as HINTS ${PAHO_MQTT_C_PATH}/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/lib)
find_library(PAHO_MQTT_CPP_LIBRARY NAMES paho-mqttpp3 HINTS ${PAHO_MQTT_CPP_PATH}/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/lib)
find_library(JSON_SCHEMA_VALIDATOR_LIBRARY nlohmann_json_schema_validator HINTS ${JSON_SCHEMA_VALIDATOR_PATH}/lib/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE})

#if(NOT TARGET eclipse-paho-mqtt-c)
#    set(eclipse-paho-mqtt-c_DIR "${PAHO_MQTT_C_PATH}/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/lib/cmake/eclipse-paho-mqtt-c")
#    find_package(eclipse-paho-mqtt-c REQUIRED)
#endif()
#if(NOT TARGET PahoMqttCpp)
#    set(PahoMqttC_DIR "${PAHO_MQTT_C_PATH}/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/lib/cmake/PahoMqttC")
#    set(PahoMqttCpp_DIR "${PAHO_MQTT_CPP_PATH}/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/lib/cmake/PahoMqttCpp")
#    find_package(PahoMqttCpp REQUIRED)
#endif()
if(NOT TARGET spdlog)
    set(spdlog_DIR "${SPDLOG_PATH}/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/lib/cmake/spdlog")
    find_package(spdlog REQUIRED)
    set_target_properties(spdlog::spdlog PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${SPDLOG_PATH}/include"
    )
endif()

if(MSVC)
    message("MSVC found")
    if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        message("Debug or RelWithDebInfo MSVC")
        ## tbb
        find_library(TBB_PROXY_LIBRARY tbb_debug HINTS ${TBB_PATH}/lib/win) 
    elseif(CMAKE_BUILD_TYPE MATCHES Release) # incase of release and relwithdebuginfo
        message("Release MSVC")
        # tbb
        find_library(TBB_PROXY_LIBRARY tbb HINTS ${TBB_PATH}/lib/win) 
    else()
        message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
    endif()
endif(MSVC)


if(LINUX)
    message("Linux found")
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        message("Debug Linux")
        # tbb
        find_library(TBB_PROXY_LIBRARY tbb_debug HINTS ${TBB_PATH}/lib/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE})
    else(CMAKE_BUILD_TYPE MATCHES Debug) # incase of release and relwithdebuginfo
        message("Release Linux")
        # tbb
        find_library(TBB_PROXY_LIBRARY tbb HINTS ${TBB_PATH}/lib/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE})
    endif()
endif(LINUX)

#
# sources
#
set(PROJECT_INCLUDE_FILES
    ${PROJECT_INCLUDE_DIR}/mqtt_client.h
    ${PROJECT_INCLUDE_DIR}/mqtt_client_connection_callback.h
    ${PROJECT_INCLUDE_DIR}/mqtt_action_listener.h
    ${PROJECT_INCLUDE_DIR}/mqtt_delivery_action_listener.h
)


set(PROJECT_SOURCE_FILES
    ${PROJECT_SRC_DIR}/main.cpp
    ${PROJECT_SRC_DIR}/mqtt_client.cpp
)


set(PROJECT_SOURCES
    ${PROJECT_SOURCE_FILES}
    ${PROJECT_INCLUDE_FILES}
)

#
# includes
#
include_directories(${PROJECT_INCLUDE_DIR})
include_directories(${INCLUDE_DIR})
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")
#
# external include libraries
#
include_directories(${PROJECT_LIB_EXT}/jsonrpcpp/include)
include_directories(${PROJECT_LIB_EXT}/stduuid/include)
include_directories(${PAHO_MQTT_C_PATH}/include)
include_directories(${PAHO_MQTT_CPP_PATH}/include)
# tbb 
include_directories(${TBB_PATH}/include)
# json-schema-validator
include_directories(${JSON_SCHEMA_VALIDATOR_PATH}/include)
# spdlog
include_directories(${SPDLOG_PATH}/include)

link_directories(${TBB_PATH}/lib/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE})
link_directories(${PAHO_MQTT_C_PATH}/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/lib)
link_directories(${PAHO_MQTT_CPP_PATH}/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/lib)

#
# main target
#
add_executable(
    ${PROJECT_BASENAME}
    ${PROJECT_INCLUDE_FILES}
    ${PROJECT_SOURCE_FILES}
)

#
# target link libraries
#
target_link_libraries(
    ${PROJECT_BASENAME}
    ## mqttraffic_warden-lib
    #mqttraffic_warden-lib
    # paho
    #${PAHO_MQTT_C_LIBRARY}
    #${PAHO_MQTT_A_LIBRARY}
    ${PAHO_MQTT_CS_LIBRARY}
    ${PAHO_MQTT_AS_LIBRARY}
    ${PAHO_MQTT_CPP_LIBRARY}
    # tbb
    ${TBB_PROXY_LIBRARY}
    # threads
    Threads::Threads
    # json-schema-validator
    ${JSON_SCHEMA_VALIDATOR_LIBRARY}
    # spdlog
    spdlog::spdlog
)

if(UNIX)
    target_link_libraries(${PROJECT_BASENAME} dl)
elseif(WIN32)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_INCLUDE_FILES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCE_FILES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SQLITE})
    target_link_libraries(${PROJECT_BASENAME} wsock32 ws2_32)
else()
    message(FATAL_ERROR, "Platform unhandled")
endif()

#find_file(CONFIG_JSON config.json HINTS ${PROJECT_BASEDIR})
#find_file(RABBITMQ_CLIENT_CERT rabbitmq_client_certificate.pem HINTS ${PROJECT_BASEDIR}/rabbitmq_certificates)
#find_file(RABBITMQ_CA_CERT rabbitmq_ca_certificate.pem HINTS ${PROJECT_BASEDIR}/rabbitmq_certificates)
#find_file(RABBITMQ_CLIENT_KEY rabbitmq_client_key.pem HINTS ${PROJECT_BASEDIR}/rabbitmq_certificates)

if(WIN32)
    if(MSVC_VERSION LESS_EQUAL 1900)
    set (BUILD_FOLDER /${OUTPUT_PATH_WIN32}/${EXECUTABLE_FOLDER})
    else()
    set (BUILD_FOLDER "")
    endif()
    set(PAHO_MQTT_CPP_DLL ${PAHO_MQTT_CPP_PATH}/bin/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/paho-mqttpp3.dll)
    set(PAHO_MQTT_C_3A_DLL ${PAHO_MQTT_C_PATH}/bin/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/paho-mqtt3a.dll)
    set(PAHO_MQTT_C_3AS_DLL ${PAHO_MQTT_C_PATH}/bin/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/paho-mqtt3as.dll)
    set(PAHO_MQTT_C_3C_DLL ${PAHO_MQTT_C_PATH}/bin/${SHORTNAME_OS}/${SHORTNAME_BUILD_TYPE}/paho-mqtt3cs.dll)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set(TBB_LIB "${TBB_PATH}/bin/tbb_debug.dll")
        set(TBB_LIB_MALLOC "${TBB_PATH}/bin/tbbmalloc_debug.dll")
        set(TBB_LIB_MALLOC_PROXY "${TBB_PATH}/bin/tbbmalloc_proxy_debug.dll")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(TBB_LIB "${TBB_PATH}/bin/tbb.dll")
        set(TBB_LIB_MALLOC "${TBB_PATH}/bin/tbbmalloc.dll")
        set(TBB_LIB_MALLOC_PROXY "${TBB_PATH}/bin/tbbmalloc_proxy.dll")
    else()
        message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
    endif()

    add_custom_command(TARGET ${PROJECT_BASENAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PAHO_MQTT_CPP_DLL} ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PAHO_MQTT_C_3A_DLL} ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PAHO_MQTT_C_3AS_DLL} ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PAHO_MQTT_C_3C_DLL} ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TBB_LIB} ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TBB_LIB_MALLOC} ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TBB_LIB_MALLOC_PROXY} ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
                       #COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CONFIG_JSON} ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
                       #COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RABBITMQ_CA_CERT} ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
                       #COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RABBITMQ_CLIENT_CERT} ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
                       #COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RABBITMQ_CLIENT_KEY} ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/json_schemas ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
    )
    #if(CMAKE_BUILD_TYPE MATCHES Debug)
    #    add_custom_command(
    #    TARGET ${PROJECT_BASENAME} POST_BUILD
    #    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPCUALIB_PATH}/bin/win/${SHORTNAME_BUILD_TYPE}/opcuad.pdb ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
    #    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPCUAMODEL_PATH}/bin/win/${SHORTNAME_BUILD_TYPE}/opcua_modeld.pdb ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
    #    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPEN62541_PATH}/bin/win/${SHORTNAME_BUILD_TYPE}/open62541d.pdb ${CMAKE_CURRENT_BINARY_DIR}${BUILD_FOLDER}
    #    )
    #endif()
endif(WIN32)

if(LINUX)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,.")
    set(CMAKE_INSTALL_RPATH ".")
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    add_custom_command(TARGET ${PROJECT_BASENAME} POST_BUILD
                        #COMMAND ${CMAKE_COMMAND} -E copy ${CONFIG_JSON} ${CMAKE_CURRENT_BINARY_DIR}
                        #COMMAND ${CMAKE_COMMAND} -E copy ${RABBITMQ_CA_CERT} ${CMAKE_CURRENT_BINARY_DIR}
                        #COMMAND ${CMAKE_COMMAND} -E copy ${RABBITMQ_CLIENT_CERT} ${CMAKE_CURRENT_BINARY_DIR}
                        #COMMAND ${CMAKE_COMMAND} -E copy ${RABBITMQ_CLIENT_KEY} ${CMAKE_CURRENT_BINARY_DIR}
                        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/json_schemas ${CMAKE_CURRENT_BINARY_DIR}
    )

    if (CMAKE_BUILD_TYPE MATCHES Release)
        set(OUTPUT_FOLDER_NAME "${PROJECT_BASENAME}_v${MQTTrafficWarden_VERSION_MAJOR}.${MQTTrafficWarden_VERSION_MINOR}.${MQTTrafficWarden_VERSION_PATCH}")
        add_custom_command(TARGET ${PROJECT_BASENAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FOLDER_NAME}"
                           COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_BASENAME}${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FOLDER_NAME}"
                           #COMMAND ${CMAKE_COMMAND} -E copy ${CONFIG_JSON} ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FOLDER_NAME}
                           #COMMAND ${CMAKE_COMMAND} -E copy ${RABBITMQ_CA_CERT} ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FOLDER_NAME}
                           #COMMAND ${CMAKE_COMMAND} -E copy ${RABBITMQ_CLIENT_CERT} ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FOLDER_NAME}
                           #COMMAND ${CMAKE_COMMAND} -E copy ${RABBITMQ_CLIENT_KEY} ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FOLDER_NAME}
                           COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/json_schemas ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FOLDER_NAME}
        )
    endif()
endif(LINUX)

##
## testing
##
## It is necessary to have a mqttraffic_warden-lib for unit-testing
#add_library(mqttraffic_warden-lib ${PROJECT_SOURCES})
#
#target_link_libraries(
#   mqttraffic_warden-lib PUBLIC
#   # paho
#   ${PAHO_MQTT_CS_LIBRARY}
#   ${PAHO_MQTT_AS_LIBRARY}
#   ${PAHO_MQTT_CPP_LIBRARY}
#   # tbb
#   ${TBB_PROXY_LIBRARY}
#   # threads
#   Threads::Threads
#   # json-schema-validator
#   ${JSON_SCHEMA_VALIDATOR_LIBRARY}
#)
#
#if(MSVC)
#    target_compile_definitions(mqttraffic_warden-lib PRIVATE WIN32_LEAN_AND_MEAN)
#    target_link_libraries(mqttraffic_warden-lib PUBLIC wsock32 ws2_32)
#endif(MSVC)
#
#if(LINUX)
#    target_link_libraries(mqttraffic_warden-lib PUBLIC dl)
#endif(LINUX)
##
## catch2
##
#set(UNIT_TEST "tests")
#enable_testing()
#set(PROJECT_TESTS_DIR "${PROJECT_BASEDIR}/tests")
#set(CATCH_INCLUDE_DIR "${PROJECT_LIB_EXT}/catch2")
#add_library(Catch2 INTERFACE)
#target_include_directories(Catch2 INTERFACE ${CATCH_INCLUDE_DIR})
#set(PROJECT_TEST_FILES
#    ${PROJECT_TESTS_DIR}/catch2_main.cpp
#    ${PROJECT_TESTS_DIR}/example.test.cpp
#)
#add_executable(${UNIT_TEST} ${PROJECT_TEST_FILES})
#target_link_libraries(${UNIT_TEST} Catch2 mqttraffic_warden-lib)
#add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})
#add_custom_command(
#     TARGET ${UNIT_TEST}
#     COMMENT "Run tests"
#     POST_BUILD
#     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#     COMMAND ${CMAKE_CTEST_COMMAND} -C -R "^${UNIT_TEST}$" --output-on-failures
#)


   
