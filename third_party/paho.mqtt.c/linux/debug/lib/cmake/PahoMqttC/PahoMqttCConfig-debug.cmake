#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "PahoMqttC::paho-mqtt3c" for configuration "Debug"
set_property(TARGET PahoMqttC::paho-mqtt3c APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PahoMqttC::paho-mqtt3c PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libpaho-mqtt3c.so.1.3.9"
  IMPORTED_SONAME_DEBUG "libpaho-mqtt3c.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS PahoMqttC::paho-mqtt3c )
list(APPEND _IMPORT_CHECK_FILES_FOR_PahoMqttC::paho-mqtt3c "${_IMPORT_PREFIX}/lib/libpaho-mqtt3c.so.1.3.9" )

# Import target "PahoMqttC::paho-mqtt3a" for configuration "Debug"
set_property(TARGET PahoMqttC::paho-mqtt3a APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PahoMqttC::paho-mqtt3a PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libpaho-mqtt3a.so.1.3.9"
  IMPORTED_SONAME_DEBUG "libpaho-mqtt3a.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS PahoMqttC::paho-mqtt3a )
list(APPEND _IMPORT_CHECK_FILES_FOR_PahoMqttC::paho-mqtt3a "${_IMPORT_PREFIX}/lib/libpaho-mqtt3a.so.1.3.9" )

# Import target "PahoMqttC::paho-mqtt3cs" for configuration "Debug"
set_property(TARGET PahoMqttC::paho-mqtt3cs APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PahoMqttC::paho-mqtt3cs PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libpaho-mqtt3cs.so.1.3.9"
  IMPORTED_SONAME_DEBUG "libpaho-mqtt3cs.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS PahoMqttC::paho-mqtt3cs )
list(APPEND _IMPORT_CHECK_FILES_FOR_PahoMqttC::paho-mqtt3cs "${_IMPORT_PREFIX}/lib/libpaho-mqtt3cs.so.1.3.9" )

# Import target "PahoMqttC::paho-mqtt3as" for configuration "Debug"
set_property(TARGET PahoMqttC::paho-mqtt3as APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PahoMqttC::paho-mqtt3as PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libpaho-mqtt3as.so.1.3.9"
  IMPORTED_SONAME_DEBUG "libpaho-mqtt3as.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS PahoMqttC::paho-mqtt3as )
list(APPEND _IMPORT_CHECK_FILES_FOR_PahoMqttC::paho-mqtt3as "${_IMPORT_PREFIX}/lib/libpaho-mqtt3as.so.1.3.9" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
