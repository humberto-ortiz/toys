#ifndef OSTREAMS_7246b508c27543109c2794f3beac9787
#define OSTREAMS_7246b508c27543109c2794f3beac9787

#include <deque>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace calmofthestorm {
namespace insertion_overloads {

/////////////////////////////////////
// Stream output (debugging focused).

// TODO: See if there's a better way

#define OUTPUT_SEQUENCE(v) \
  os << "[\n"; \
  for (const auto& it : v) { \
    os << "\t" << it << std::endl; \
  } \
  os  << "]" << std::endl;

#define OUTPUT_MAPPING(v) \
  os << "{\n"; \
  for (const auto& it : v) { \
    os << "\t" << it.first << ": " << it.second << std::endl; \
  } \
  os  << "}" << std::endl;

// Deque
template <typename T>
std::ostream& operator<< (std::ostream& os, const std::deque<T>& v) {
  OUTPUT_SEQUENCE(v);
  return os;
}

// Array
template <typename T, size_t n>
std::ostream& operator<< (std::ostream& os, const std::array<T, n>& v) {
  OUTPUT_SEQUENCE(v);
  return os;
}

// Vector
template <typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& v) {
  OUTPUT_SEQUENCE(v);
  return os;
}

// Set
template <typename T>
std::ostream& operator<< (std::ostream& os, const std::set<T>& v) {
  OUTPUT_SEQUENCE(v);
  return os;
}

// Multiset
template <typename T>
std::ostream& operator<< (std::ostream& os, const std::multiset<T>& v) {
  OUTPUT_SEQUENCE(v);
  return os;
}

// Unordered set
template <typename T>
std::ostream& operator<< (std::ostream& os, const std::unordered_set<T>& v) {
  OUTPUT_SEQUENCE(v);
  return os;
}

// Unordered multiset
template <typename T>
std::ostream& operator<< (std::ostream& os, const std::unordered_multiset<T>& v) {
  OUTPUT_SEQUENCE(v);
  return os;
}

// Pair
template <typename T1, typename T2>
std::ostream& operator<< (std::ostream& os, const std::pair<T1, T2>& v) {
  os << "(" << v.first << ", " << v.second << ")";
  return os;
}

// Map
template <typename T1, typename T2>
std::ostream& operator<< (std::ostream& os, const std::map<T1, T2>& v) {
  OUTPUT_MAPPING(v);
  return os;
}

// Unordered map
template <typename T1, typename T2>
std::ostream& operator<< (std::ostream& os, const std::unordered_map<T1, T2>& v) {
  OUTPUT_MAPPING(v);
  return os;
}

// Multimap
template <typename T1, typename T2>
std::ostream& operator<< (std::ostream& os, const std::multimap<T1, T2>& v) {
  OUTPUT_MAPPING(v);
  return os;
}

// Unordered multimap
template <typename T1, typename T2>
std::ostream& operator<< (std::ostream& os, const std::unordered_multimap<T1, T2>& v) {
  OUTPUT_MAPPING(v);
  return os;
}

}  // namespace insertion_overloads
}  // namespace calmofthestorm

#endif  /* OSTREAMS_7246b508c27543109c2794f3beac9787 */
