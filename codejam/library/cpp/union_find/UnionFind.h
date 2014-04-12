// Friendly boost disjoint_sets wrapper
// Alex Roper
// alex@aroper.net
//
//
// This is a friendly wrapper around boost::disjoint_sets. The intent is to
// create a container class with as easy to use (and hard to mess up) an
// interface as possible, with performance as an acceptable cost. This is
// accomplished by providing a class that manages all the union find internals
// such as mapping from arbitrary keys to integers for disjoint_sets, resizing
// as necessary, etc.
//
// Boost functions with tricky semantics (eg, link vs union_set) are removed,
// exceptions used for illegal operations, etc.
//
// This comes at the cost of an indirection layer for all lookups, less
// general backing than Boost allows, and unnecessary lookups in union_sets.
//
// Intended for use in coding competitions, where the slight performance hit
// is well worth it for simpler semantics.
//
//

#ifndef UNIONFIND_0bb7b9ebeb3048f58090c09247502b31
#define UNIONFIND_0bb7b9ebeb3048f58090c09247502b31

#include <unordered_map>

#include <boost/pending/disjoint_sets.hpp>
#include <boost/iterator/counting_iterator.hpp>

namespace calmofthestorm {

template <typename T>
class UnionFind {
  public:
    UnionFind();
    UnionFind(std::initializer_list<typename std::vector<T>::value_type> l);
    UnionFind(const UnionFind& other);
    UnionFind(UnionFind&& other);
    UnionFind& operator=(const UnionFind& other);
    UnionFind& operator=(UnionFind&& other);

    // Get element, adding it as a singleton set if it does not exist.
    T& operator[] (const T& elem);
    T& operator[] (T&& elem);

    // Access element, throwing std::out_of_range if it does not exist.
    T& at (const T& elem);

    // Union the two sets. Throws std::out_of_range if either does not exist.
    T& union_sets(const T& elem1, const T& elem2);

    // Get the number of SETS.
    size_t count_sets();

    // Get the number of ELEMENTS.
    size_t count_elements() const;

    // Get a reference to the elements in the union find (Handy for iteration).
    inline const std::vector<T>& get_elements() const { return id_to_handle; }

  private:
    std::vector<size_t> rank, parent;
    std::vector<T> id_to_handle;
    std::unordered_map<T, size_t> handle_to_id;
    boost::disjoint_sets<size_t*, size_t*> uf;

    bool grow();
};

}  // namespace calmofthestorm

#include "UnionFind.tcc"

#endif  /* UNIONFIND_0bb7b9ebeb3048f58090c09247502b31 */
