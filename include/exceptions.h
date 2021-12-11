#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

class TrafficWardenInitializationException : public std::runtime_error {
 public:
  TrafficWardenInitializationException()
      : std::runtime_error("TrafficWarden initialization failed") {}
};

class StreamTransformerSetupException : public std::runtime_error {
 public:
  StreamTransformerSetupException()
      : std::runtime_error("Stream transformer setup failed") {}
};

class StreamTransformerExecutionException : public std::runtime_error {
 public:
  StreamTransformerExecutionException()
      : std::runtime_error("Stream transformer execution failed") {}
};

#endif  // EXCEPTIONS_H
