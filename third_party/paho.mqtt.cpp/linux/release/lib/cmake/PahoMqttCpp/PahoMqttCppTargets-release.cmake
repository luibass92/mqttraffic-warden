#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "PahoMqttCpp::paho-mqttpp3" for configuration "Release"
set_property(TARGET PahoMqttCpp::paho-mqttpp3 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PahoMqttCpp::paho-mqttpp3 PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libpaho-mqttpp3.so.1.2.0"
  IMPORTED_SONAME_RELEASE "libpaho-mqttpp3.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS PahoMqttCpp::paho-mqttpp3 )
list(APPEND _IMPORT_CHECK_FILES_FOR_PahoMqttCpp::paho-mqttpp3 "${_IMPORT_PREFIX}/lib/libpaho-mqttpp3.so.1.2.0" )

# Import target "PahoMqttCpp::paho-mqttpp3-static" for configuration "Release"
set_property(TARGET PahoMqttCpp::paho-mqttpp3-static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PahoMqttCpp::paho-mqttpp3-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libpaho-mqttpp3.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS PahoMqttCpp::paho-mqttpp3-static )
list(APPEND _IMPORT_CHECK_FILES_FOR_PahoMqttCpp::paho-mqttpp3-static "${_IMPORT_PREFIX}/lib/libpaho-mqttpp3.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
