#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <stack>
#include <utility>

namespace {
  template <typename KeyT, typename ValT>
  using Node_unique_ptr = std::unique_ptr<Node<KeyT, ValT>>;
}

template <typename KeyT, typename ValT>
Node_unique_ptr<KeyT, ValT>& RBTree<KeyT, ValT>::getInRef(Node_t* root_in) {
  if (root_in->parent) {
    if (root_in->parent->left.get() == root_in) {
      return root_in->parent->left;
    } else {
      return root_in->parent->right;
    }
  } else {
    return mTree;
  }
}

template <typename KeyT, typename ValT>
void RBTree<KeyT, ValT>::rotateLeft(Node<KeyT, ValT>* root_in) {
  Node_unique_ptr<KeyT, ValT> tmp;
  Node_unique_ptr<KeyT, ValT>& root = getInRef(root_in);

  tmp = std::move(root);
  root = std::move(root_in->right);
  tmp->right = std::move(root->left);
  root->left = std::move(tmp);

  root->parent = root->left->parent;
  root->left->parent = root.get();
  if (root->left->right) {
    root->left->right->parent = root->left.get();
  }
}

template <typename KeyT, typename ValT>
void RBTree<KeyT, ValT>::rotateRight(Node<KeyT, ValT>* root_in) {
  Node_unique_ptr<KeyT, ValT> tmp;
  Node_unique_ptr<KeyT, ValT>& root = getInRef(root_in);

  tmp = std::move(root);
  root = std::move(root_in->left);
  tmp->left = std::move(root->right);
  root->right = std::move(tmp);

  root->parent = root->right->parent;
  root->right->parent = root.get();
  if (root->right->left) {
    root->right->left->parent = root->right.get();
  }
}

template <typename KeyT, typename ValT>
RBTree<KeyT, ValT>::RBTree()
  : mTree(nullptr), mSize(0) {
  #ifdef _DEBUG
    checkInvariant();
  #endif
}

template <typename KeyT, typename ValT>
bool RBTree<KeyT, ValT>::has(const KeyT& key) const {
  #ifdef _DEBUG
    checkInvariant();
  #endif

  return locate(key).second != nullptr;
}

template <typename KeyT, typename ValT>
ValT& RBTree<KeyT, ValT>::get(const KeyT& key, ValT& default_val) {
  #ifdef _DEBUG
    checkInvariant();
  #endif

  Node_t* loc = locate(key).second;
  if (!loc) {
    return default_val;
  } else {
    return loc->value;
  }
}
template <typename KeyT, typename ValT>
ValT& RBTree<KeyT, ValT>::get(const KeyT& key) throw (KeyError) {
  #ifdef _DEBUG
    checkInvariant();
  #endif

  Node_t* loc = locate(key).second;
  if (!loc) {
    throw KeyError();
  } else {
    return loc->value;
  }
}

template <typename KeyT, typename ValT>
const ValT& RBTree<KeyT, ValT>::get(const KeyT& key,
                                    const ValT& default_value) const {
  #ifdef _DEBUG
    checkInvariant();
  #endif

  Node_t* loc = locate(key).second;
  if (!loc) {
    return default_value;
  } else {
    return loc->value;
  }
}

template <typename KeyT, typename ValT>
const ValT& RBTree<KeyT, ValT>::get(const KeyT& key) const throw (KeyError) {
  #ifdef _DEBUG
    checkInvariant();
  #endif

  Node_t* loc = locate(key).second;
  if (!loc) {
    throw KeyError();
  } else {
    return loc->value;
  }
}

template <typename KeyT, typename ValT>
void RBTree<KeyT, ValT>::insert(const KeyT& key, const ValT& value) {
  insert(key, ValT(value));
}

template <typename KeyT, typename ValT>
void RBTree<KeyT, ValT>::insert(const KeyT& key, ValT&& value) {
  #ifdef _DEBUG
    checkInvariant();
  #endif

  Node<KeyT, ValT>* prev;
  Node<KeyT, ValT>* loc;
  std::tie(prev, loc) = locate(key);
  if (loc) {
    loc->value = std::move(value);
  } else {
    Node_unique_ptr<KeyT, ValT> new_node(new Node_t(key, value, COLOR::RED, prev));
    ++mSize;
    Node<KeyT, ValT>* cur = new_node.get();

    if (!mTree) {
      assert(mSize == 1);
      new_node->color = COLOR::BLACK;
      std::swap(mTree, new_node);
      #ifdef _DEBUG
        checkInvariant();
      #endif
      return;
    }
    assert(mSize > 1);

    if (key < prev->key) {
      std::swap(prev->left, new_node);
    } else {
      std::swap(prev->right, new_node);
    }
    assert(!new_node);

    while (cur && cur->parent && cur->parent->color == COLOR::RED) {
      if (cur->parent->parent && cur->parent == cur->parent->parent->left.get()) {
        Node<KeyT, ValT>* cousin = cur->parent->parent->right.get();
        if (cousin && cousin->color == COLOR::RED) {
          cur->parent->color = COLOR::BLACK;
          cousin->color = COLOR::BLACK;
          cur->parent->parent->color = COLOR::RED;
          cur = cur->parent->parent;
        } else {
          if (cur->parent->right.get() == cur) {
            cur = cur->parent;
            rotateLeft(cur);
          }
          if (cur->parent) {
            cur->parent->color = COLOR::BLACK;
            if (cur->parent->parent) {
              cur->parent->parent->color = COLOR::RED;
              rotateRight(cur->parent->parent);
            }
          }
        }
      } else if (cur->parent->parent && cur->parent == cur->parent->parent->right.get()) {
        Node<KeyT, ValT>* cousin = cur->parent->parent->left.get();
        if (cousin && cousin->color == COLOR::RED) {
          cur->parent->color = COLOR::BLACK;
          cousin->color = COLOR::BLACK;
          cur->parent->parent->color = COLOR::RED;
          cur = cur->parent->parent;
        } else {
          if (cur->parent->left.get() == cur) {
            cur = cur->parent;
            rotateRight(cur);
          }
          if (cur->parent) {
            cur->parent->color = COLOR::BLACK;
            if (cur->parent->parent) {
              cur->parent->parent->color = COLOR::RED;
              rotateLeft(cur->parent->parent);
            }
          }
        }
      }
    }
    mTree->color = COLOR::BLACK;
  }
  #ifdef _DEBUG
    checkInvariant();
  #endif
}

template <typename KeyT, typename ValT>
void RBTree<KeyT, ValT>::remove(const KeyT& key, bool discard) throw (KeyError) {
  #ifdef _DEBUG
    checkInvariant();
  #endif

  Node<KeyT, ValT>* prev;
  Node<KeyT, ValT>* loc;
  Node<KeyT, ValT>* tmp;
  Node<KeyT, ValT>* cur;
  std::tie(prev, loc) = locate(key);
  if (!loc) {
    if (!discard) {
      throw KeyError();
    } else {
      return;
    }
  }

  --mSize;

  std::unique_ptr<Node<KeyT, ValT>>& holder = getInRef(loc);
  std::unique_ptr<Node<KeyT, ValT>> removed_node = std::move(holder);

  Node<KeyT, ValT>* removed_parent = loc->parent;

  if (!loc->left && !loc->right) {
    cur = nullptr;
  } else if (loc->left && !loc->right) {
    cur = loc->left.get();
    loc->left->parent = loc->parent;
    holder = std::move(loc->left);
  } else if (loc->right && !loc->left) {
    cur = loc->right.get();
    loc->right->parent = loc->parent;
    holder = std::move(loc->right);
  } else {
    // A successor is guaranteed to exist since node has two children, and
    // it must have a null left since otherwise that left would be the succ.
    holder = std::move(removed_node);
    Node<KeyT, ValT>* succ = treeSuccessor(loc);
    cur = succ->right.get();
    removed_parent = succ->parent;
    assert(!succ->left);
    std::swap(loc->key, succ->key);
    std::swap(loc->value, succ->value);

    if (succ->right) {
      succ->right->parent = succ;
    }

    if (succ->parent->left.get() == succ) {
      removed_node = std::move(succ->parent->left);
      succ->parent->left = std::move(succ->right);
    } else {
      removed_node = std::move(succ->parent->right);
      succ->parent->right = std::move(succ->right);
    }
  }

  // If necessary, rebalance the tree.
  if (removed_node->color == COLOR::BLACK) {
    while (cur != mTree.get() && (!cur || cur->color == COLOR::BLACK)) {
      Node<KeyT, ValT>* sibling;
      if (removed_parent->left.get() == cur) {
        sibling = removed_parent->right.get();
        if (sibling && sibling->color == COLOR::RED) {
          sibling->color = COLOR::BLACK;
          removed_parent->color = COLOR::RED;
          rotateLeft(removed_parent);
          sibling = removed_parent->right.get();
        }
        if (sibling && 
            (!sibling->left || sibling->left->color == COLOR::BLACK) &&
            (!sibling->right || sibling->right->color == COLOR::BLACK)) {
          sibling->color = COLOR::RED;
          cur = removed_parent;
          removed_parent = cur->parent;
        } else {
          if (!sibling->right || sibling->right->color == COLOR::BLACK) {
            sibling->left->color = COLOR::BLACK;
            sibling->color = COLOR::RED;
            rotateRight(sibling);
            sibling = removed_parent->right.get();
          }
          sibling->color = removed_parent->color;
          removed_parent->color = COLOR::BLACK;
          sibling->right->color = COLOR::BLACK;
          rotateLeft(removed_parent);
          cur = mTree.get();
          removed_parent = cur->parent;
        }
      } else {
        sibling = removed_parent->left.get();
        if (sibling && sibling->color == COLOR::RED) {
          sibling->color = COLOR::BLACK;
          removed_parent->color = COLOR::RED;
          rotateRight(removed_parent);
          sibling = removed_parent->left.get();
        }
        if (sibling && 
            (!sibling->right || sibling->right->color == COLOR::BLACK) &&
            (!sibling->left || sibling->left->color == COLOR::BLACK)) {
          sibling->color = COLOR::RED;
          cur = removed_parent;
          removed_parent = cur->parent;
        } else {
          if (!sibling->left || sibling->left->color == COLOR::BLACK) {
            sibling->right->color = COLOR::BLACK;
            sibling->color = COLOR::RED;
            rotateLeft(sibling);
            sibling = removed_parent->left.get();
          }
          sibling->color = removed_parent->color;
          removed_parent->color = COLOR::BLACK;
          sibling->left->color = COLOR::BLACK;
          rotateRight(removed_parent);
          cur = mTree.get();
          removed_parent = cur->parent;
        }
      }
    }
    if (cur) {
      cur->color = COLOR::BLACK;
    }
  }
  #ifdef _DEBUG
    checkInvariant();
  #endif
}

template <typename KeyT, typename ValT>
Node<KeyT, ValT>* RBTree<KeyT, ValT>::treeSuccessor(const Node_t* node_in) {
  assert(node_in);
  Node_t* node = node_in->right.get();
  assert(node);

  while (node->left) {
    node = node->left.get();
  }
  return node;
}
#ifdef _DEBUG
template <typename KeyT, typename ValT>
void RBTree<KeyT, ValT>::checkInvariant() const {
  // Need to check:
  // 1) All leaves are black.
  // 2) All paths have same number of black nodes
  // 3) Both children of every red node are black.

  // Empty trees always valid.
  if (mSize == 0) {
    assert(!mTree);
    return;
  } else {
    assert(mTree);
  }

  size_t counted = 0;

  // Root must be black.
  assert(mTree->color == COLOR::BLACK);

  // Count the number of black nodes along the path as we go through the
  // tree. It must be the same for all paths. As we traverse the tree,
  // also verify property 3.
  int pathBlackNodeCount = -1;
  int max_path_length;
  int min_path_length;
  std::stack<std::tuple<int, int, Node_t*>> todo;
  todo.push(std::make_tuple(0, 0, mTree.get()));
  while (!todo.empty()) {
    int numBlack;
    int pathLen;
    Node_t* cur;
    std::tie(numBlack, pathLen, cur) = todo.top();
    todo.pop();

    if (!cur) {
      if (pathBlackNodeCount == -1) {
        pathBlackNodeCount = numBlack;
        min_path_length = max_path_length = pathLen;
      } else {
        assert(pathBlackNodeCount == numBlack);
        min_path_length = std::min(min_path_length, pathLen);
        max_path_length = std::max(max_path_length, pathLen);
      }
    } else {
      ++counted;
      if (cur->color == COLOR::BLACK) {
        ++numBlack;
      } else {
        assert(!cur->left || cur->left->color == COLOR::BLACK);
        assert(!cur->right || cur->right->color == COLOR::BLACK);
      }
      ++pathLen;
      assert(!cur->left || cur->left->parent == cur);
      assert(!cur->right || cur->right->parent == cur);
      todo.push(std::make_tuple(numBlack, pathLen, cur->left.get()));
      todo.push(std::make_tuple(numBlack, pathLen, cur->right.get()));
    }
  }

  assert(min_path_length * 2 >= max_path_length);
  assert(counted == mSize);
}
#endif

// Find a key or where it should go in the tree.
// Returns:
// * Tree is empty: (nullptr, nullptr)
// * Key is at root: (nullptr, root)
// * Key is in tree: (parent, key)
// * Key is not in tree: (parent_where_should_go, nullptr)
template <typename KeyT, typename ValT>
std::pair<Node<KeyT, ValT>*,
          Node<KeyT, ValT>*> RBTree<KeyT, ValT>::locate(const KeyT& key) const {
  Node<KeyT, ValT>* cur = mTree.get();
  Node<KeyT, ValT>* prev = nullptr;
  while (cur) {
    if (cur->key == key) {
      break;
    } else if (cur->key > key) {
      prev = cur;
      cur = cur->left.get();
    } else {
      prev = cur;
      cur = cur->right.get();
    }
  }
  return std::make_pair(prev, cur);
}
