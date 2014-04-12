#ifndef HASHES_195420703a4a4162a82d91b98df64049
#define HASHES_195420703a4a4162a82d91b98df64049

#include <boost/functional/hash.hpp>

namespace std {

template<typename T1, typename T2>
struct hash<std::pair<T1, T2>> {
  size_t operator () (const std::pair<T1, T2>& key) const {
    size_t seed = 0;
    boost::hash_combine(seed, key.first);
    boost::hash_combine(seed, key.second);
    return seed;
  }
};

template<typename T1, typename T2>
struct hash<std::map<T1, T2>> {
  size_t operator () (const std::map<T1, T2>& key) const {
    size_t seed = 0;
    for (const auto& it : key) {
      boost::hash_combine(seed, it);
    }
    return seed;
  }
};

template<typename T>
struct hash<std::set<T>> {
  size_t operator () (const std::set<T>& key) const {
    size_t seed = 0;
    for (const auto& it : key) {
      boost::hash_combine(seed, it);
    }
    return seed;
  }
};

template<typename T>
struct hash<std::deque<T>> {
  size_t operator () (const std::deque<T>& key) const {
    size_t seed = 0;
    for (const auto& it : key) {
      boost::hash_combine(seed, it);
    }
    return seed;
  }
};

template<typename T, size_t n>
struct hash<std::array<T, n>> {
  size_t operator () (const std::array<T, n>& key) const {
    size_t seed = 0;
    for (const auto& it : key) {
      boost::hash_combine(seed, it);
    }
    return seed;
  }
};

template<typename T>
struct hash<std::vector<T>> {
  size_t operator () (const std::vector<T>& key) const {
    size_t seed = 0;
    for (const auto& it : key) {
      boost::hash_combine(seed, it);
    }
    return seed;
  }
};

}  // namespace std

#include <unordered_map>

#endif  /* HASHES_195420703a4a4162a82d91b98df64049 */
