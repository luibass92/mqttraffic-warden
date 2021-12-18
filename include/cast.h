#ifndef CAST_H
#define CAST_H

#include <sstream>

namespace utilities {

/* fallback */
template <class T>
T lexical_cast(const char* str) {
  static std::istringstream
      ss; /* reusing has severe (positive) impact on performance */
  T value;
  ss.str(str);
  ss >> value;
  ss.clear();
  return value;
}

/* trivial conversion */
template <>
std::string lexical_cast(const char* str);

/* conversions that exist in stl */
template <>
float lexical_cast(const char* str);
template <>
double lexical_cast(const char* str);
template <>
long double lexical_cast(const char* str);
template <>
long lexical_cast(const char* str);
template <>
long long lexical_cast(const char* str);
template <>
unsigned long lexical_cast(const char* str);
template <>
unsigned long long lexical_cast(const char* str);

/* conversions that need to be truncated */
template <>
short lexical_cast(const char* str);
template <>
int lexical_cast(const char* str);
template <>
unsigned short lexical_cast(const char* str);
template <>
unsigned int lexical_cast(const char* str);

}  // namespace utilities
#endif  // CAST_H
