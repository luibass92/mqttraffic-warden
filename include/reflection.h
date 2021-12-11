#ifndef REFLECTION_H
#define REFLECTION_H

#define ADD_CLASS_NAME(expression) \
 public:                           \
  static std::string get_class_name() { return std::string(#expression); }

#endif  // REFLECTION_H
