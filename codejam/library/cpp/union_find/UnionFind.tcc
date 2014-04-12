namespace calmofthestorm {

template <typename T>
UnionFind<T>::UnionFind ()
: rank{0},
  parent{0},
  id_to_handle(),
  handle_to_id(),
  uf(&rank[0], &parent[0]) { }

template <typename T>
UnionFind<T>::UnionFind(
    std::initializer_list<typename std::vector<T>::value_type> l
  )
: rank(l.size()),
  parent(l.size()),
  id_to_handle(l),
  handle_to_id(),
  uf(&rank[0], &parent[0])
{
  for (size_t i = 0; i < id_to_handle.size(); ++i) {
    handle_to_id[id_to_handle[i]] = i;
    uf.make_set(i);
  }
}

template <typename T>
UnionFind<T>::UnionFind(const UnionFind& other)
: rank(other.rank),
  parent(other.parent),
  id_to_handle(other.id_to_handle),
  handle_to_id(other.handle_to_id),
  uf(&rank[0], &parent[0]) { }

template <typename T>
UnionFind<T>::UnionFind(UnionFind&& other)
: rank(std::move(other.rank)),
  parent(std::move(other.parent)),
  id_to_handle(std::move(other.id_to_handle)),
  handle_to_id(std::move(other.handle_to_id)),
  uf(&rank[0], &parent[0]) { }

template <typename T>
UnionFind<T>& UnionFind<T>::operator=(const UnionFind& other) {
  rank = other.rank;
  parent = other.parent;
  id_to_handle = other.id_to_handle;
  handle_to_id = other.handle_to_id;
  uf = boost::disjoint_sets<size_t*, size_t*>(&rank[0], &parent[0]);
  return *this;
}

template <typename T>
UnionFind<T>& UnionFind<T>::operator=(UnionFind&& other) {
  std::swap(rank, other.rank);
  std::swap(parent, other.parent);
  std::swap(id_to_handle, other.id_to_handle);
  std::swap(handle_to_id, other.handle_to_id);
  uf = boost::disjoint_sets<size_t*, size_t*>(&rank[0], &parent[0]);
  return *this;
}

template <typename T>
T& UnionFind<T>::operator[] (T&& elem) {
  auto it = handle_to_id.find(elem);
  if (it != handle_to_id.end()) {
    return id_to_handle[uf.find_set(it->second)];
  } else {
    size_t id = id_to_handle.size();
    handle_to_id[elem] = id;
    id_to_handle.push_back(elem);
    if (id_to_handle.size() > rank.size() && grow()) {
      uf = boost::disjoint_sets<size_t*, size_t*>(&rank[0], &parent[0]);
    }
    uf.make_set(id);
    return id_to_handle.at(id);
  }
}

template <typename T>
T& UnionFind<T>::operator[] (const T& elem) {
  return this->operator[] (T(elem));
}

template <typename T>
T& UnionFind<T>::at(const T& elem) {
  return id_to_handle.at(uf.find_set(handle_to_id.at(elem)));
}

template <typename T>
T& UnionFind<T>::union_sets(const T& elem1, const T& elem2) {
  size_t id1 = handle_to_id.at(elem1);
  size_t id2 = handle_to_id.at(elem2);
  uf.union_set(id1, id2);
  return id_to_handle.at(uf.find_set(id1));
}

template <typename T>
bool UnionFind<T>::grow() {
  bool realloc_occurred = false;
  size_t* buf = &parent[0];
  parent.resize(parent.size() + 1);
  realloc_occurred |= (buf != &parent[0]);

  buf = &rank[0];
  rank.resize(rank.size() + 1);
  realloc_occurred |= (buf != &rank[0]);

  return realloc_occurred;
}

template <typename T>
size_t UnionFind<T>::count_sets() {
  if (id_to_handle.empty()) {
    return 0;
  } else {
    return uf.count_sets(
        boost::counting_iterator<size_t>(0),
        boost::counting_iterator<size_t>(rank.size())
      );
  }
}

template <typename T>
size_t UnionFind<T>::count_elements() const {
  return id_to_handle.size();
}

}  // namespace calmofthestorm
