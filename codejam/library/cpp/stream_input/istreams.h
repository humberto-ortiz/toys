// Coding contest library (C++)
// Alex Roper
// alex@aroper.net
//
// Note: Relies on Boost (http://www.boost.org)
//
// All code available under the terms of the GPLv3, and in particular you are
// welcome to use this library in your own contest coding. See
// https://github.com/calmofthestorm/toys/codejam/library for the latest version
// or to submit issues/pull requests.
//
#ifndef ISTREAMS_9e126372879b480a9b7b68d637254ab3
#define ISTREAMS_9e126372879b480a9b7b68d637254ab3

#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace calmofthestorm {

namespace extraction_overloads {
/////////////////////////////////////////////
// Raw stream input (based on structure size)

template <typename T, size_t N>
std::istream& operator>> (std::istream& is, std::array<T, N>& v) {
  for (auto& elem : v) {
    is >> elem;
  }
  return is;
}

template <typename T>
std::istream& operator>> (std::istream& is, std::vector<T>& v) {
  for (auto& elem : v) {
    is >> elem;
  }
  return is;
}

template <typename T>
std::istream& operator>> (std::istream& is, std::deque<T>& v) {
  for (auto& elem : v) {
    is >> elem;
  }
  return is;
}

template <typename T1, typename T2>
std::istream& operator>> (std::istream& is, std::pair<T1, T2>& p) {
  is >> p.first >> p.second;
  return is;
}

}  // namespace extraction_overloads

namespace io {

namespace {
  using extraction_overloads::operator>>;
}  // anonymous namespace

/////////////////////
// Sized stream input

// TODO: There has to be a better way to write these.

// Helper for reading several STL containers by size and items.
template <typename T>
class sized_container {
  public:
    explicit sized_container(T& v)
      : container(v) { }
    T& container;
};

// Deque
template <typename T>
sized_container<std::deque<T>> sized(std::deque<T>& t) {
  return sized_container<std::deque<T>>(t);
}

template <typename T>
std::istream& operator>> (std::istream& is, sized_container<std::deque<T>>&& nv) {
  int n;
  is >> n;
  nv.container.resize(n);
  for (auto& it : nv.container) {
    is >> it;
  }
  return is;
}

// Vector
template <typename T>
sized_container<std::vector<T>> sized(std::vector<T>& t) {
  return sized_container<std::vector<T>>(t);
}

template <typename T>
std::istream& operator>> (std::istream& is, sized_container<std::vector<T>>&& nv) {
  int n;
  is >> n;
  nv.container.resize(n);
  for (auto& it : nv.container) {
    is >> it;
  }
  return is;
}

// Set
template <typename T>
sized_container<std::set<T>> sized(std::set<T>& t) {
  return sized_container<std::set<T>>(t);
}

template <typename T>
std::istream& operator>> (std::istream& is, sized_container<std::set<T>>&& nv) {
  int n;
  is >> n;
  nv.container.resize(n);
  for (auto& it : nv.container) {
    is >> it;
  }
  return is;
}

// Unordered Set
template <typename T>
sized_container<std::unordered_set<T>> sized(std::unordered_set<T>& t) {
  return sized_container<std::unordered_set<T>>(t);
}

template <typename T>
std::istream& operator>> (std::istream& is, sized_container<std::unordered_set<T>>&& nv) {
  int n;
  is >> n;
  nv.container.resize(n);
  for (auto& it : nv.container) {
    is >> it;
  }
  return is;
}

// Multiset
template <typename T>
sized_container<std::multiset<T>> sized(std::multiset<T>& t) {
  return sized_container<std::multiset<T>>(t);
}

template <typename T>
std::istream& operator>> (std::istream& is, sized_container<std::multiset<T>>&& nv) {
  int n;
  is >> n;
  nv.container.resize(n);
  for (auto& it : nv.container) {
    is >> it;
  }
  return is;
}

// Unordered multiset
template <typename T>
sized_container<std::unordered_multiset<T>> sized(std::unordered_multiset<T>& t) {
  return sized_container<std::unordered_multiset<T>>(t);
}

template <typename T>
std::istream& operator>> (std::istream& is, sized_container<std::unordered_multiset<T>>&& nv) {
  int n;
  is >> n;
  nv.container.resize(n);
  for (auto& it : nv.container) {
    is >> it;
  }
  return is;
}

// Map
template <typename T1, typename T2>
sized_container<std::map<T1, T2>> sized(std::map<T1, T2>& t) {
  return sized_container<std::map<T1, T2>>(t);
}

template <typename T1, typename T2>
std::istream& operator>> (std::istream& is, sized_container<std::map<T1, T2>>&& nv) {
  int n;
  is >> n;
  for (size_t i = 0; i < n; ++i) {
    T1 k;
    T2 v;
    is >> k >> v;
    nv.container[k] = v;
  }
  return is;
}

// Multimap
template <typename T1, typename T2>
sized_container<std::multimap<T1, T2>> sized(std::multimap<T1, T2>& t) {
  return sized_container<std::multimap<T1, T2>>(t);
}

template <typename T1, typename T2>
std::istream& operator>> (std::istream& is, sized_container<std::multimap<T1, T2>>&& nv) {
  int n;
  is >> n;
  for (size_t i = 0; i < n; ++i) {
    T1 k;
    T2 v;
    is >> k >> v;
    nv.container[k] = v;
  }
  return is;
}

// Unordered map
template <typename T1, typename T2>
sized_container<std::unordered_map<T1, T2>> sized(std::unordered_map<T1, T2>& t) {
  return sized_container<std::unordered_map<T1, T2>>(t);
}

template <typename T1, typename T2>
std::istream& operator>> (std::istream& is, sized_container<std::unordered_map<T1, T2>>&& nv) {
  int n;
  is >> n;
  for (size_t i = 0; i < n; ++i) {
    T1 k;
    T2 v;
    is >> k >> v;
    nv.container[k] = v;
  }
  return is;
}

// Unordered multimap
template <typename T1, typename T2>
sized_container<std::unordered_multimap<T1, T2>> sized(std::unordered_multimap<T1, T2>& t) {
  return sized_container<std::unordered_multimap<T1, T2>>(t);
}

template <typename T1, typename T2>
std::istream& operator>> (std::istream& is, sized_container<std::unordered_multimap<T1, T2>>&& nv) {
  int n;
  is >> n;
  for (size_t i = 0; i < n; ++i) {
    T1 k;
    T2 v;
    is >> k >> v;
    nv.container[k] = v;
  }
  return is;
}

// Read a rectangle's size followed by elements.
template <typename T>
class rect_container {
  public:
    explicit rect_container(T& v)
      : container(v) { }
    T& container;
};

template <typename T>
std::istream& operator>> (std::istream& is, rect_container<std::vector<std::vector<T>>>&& nv) {
  int r, c;
  is >> r >> c;
  nv.container.resize(0);
  nv.container.resize(r, std::vector<T>(c));
  is >> nv.container;
  return is;
}

template <typename T>
rect_container<std::vector<std::vector<T>>> rect(std::vector<std::vector<T>>& t) {
  return rect_container<std::vector<std::vector<T>>>(t);
}

// Read a square's size followed by elements.
template <typename T>
class square_container {
  public:
    explicit square_container(T& v)
      : container(v) { }
    T& container;
};

template <typename T>
std::istream& operator>> (std::istream& is, square_container<std::vector<std::vector<T>>>&& nv) {
  int n;
  is >> n;
  nv.container.resize(0);
  nv.container.resize(n, std::vector<T>(n));
  is >> nv.container;
  return is;
}

template <typename T>
square_container<std::vector<std::vector<T>>> square(std::vector<std::vector<T>>& t) {
  return square_container<std::vector<std::vector<T>>>(t);
}

}  // namespace io
}  // namespace calmofthestorm

#endif  /* ISTREAMS_9e126372879b480a9b7b68d637254ab3 */
