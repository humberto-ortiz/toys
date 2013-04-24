#ifndef net_ugcs_alexr_fun_rbtree
#define net_ugcs_alexr_fun_rbtree

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <stack>
#include <utility>

//#define _DEBUG

struct KeyError {};

enum class COLOR {BLACK, RED};

template <typename KeyT, typename ValT>
struct Node {
  KeyT key;
  ValT value;
  COLOR color;
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
  Node* parent;

  Node(const KeyT& key, const ValT& value, COLOR color)
    : key(key), value(value), color(color), left(nullptr), right(nullptr),
      parent(nullptr) {}

  Node(const KeyT& key, const ValT& value, COLOR color, Node* parenti)
    : key(key), value(value), color(color), left(nullptr), right(nullptr),
      parent(parenti) {}

  Node* grandparent() const {
    if (parent) {
      return parent->parent;
    } else {
      return nullptr;
    }
  }

  Node* uncle() const {
    Node* gp = grandparent();
    if (!gp) {
      return nullptr;
    } else {
      if (parent == gp->left.get()) {
        return gp->right.get();
      } else {
        return gp->left.get();
      }
    }
  }
};

template <typename KeyT, typename ValT>
class RBTree {
  private:
    typedef Node<KeyT, ValT> Node_t;

    std::unique_ptr<Node_t> mTree;

    // Number of items stored in the tree.
    size_t mSize;

    // Helper for rotations. Returns a reference to the unique_ptr holding the
    // given node.
    std::unique_ptr<Node_t>& getInRef(Node_t* root_in);

    void rotateLeft(Node_t* root_in);

    void rotateRight(Node_t* root_in);

  public:
    RBTree();

    // Returns true if the key exists as a key in the tree, otherwise false.
    bool has(const KeyT& key) const;

    // Returns the value for the given tree.
    const ValT& get(const KeyT& key) const throw (KeyError);
    ValT& get(const KeyT& key) throw (KeyError);
    const ValT& get(const KeyT& key, const ValT& default_value) const;
    ValT& get(const KeyT& key, ValT& default_value);


    // Sets the value of the given key to the given value. This will replace the
    // current value (if any).
    void insert(const KeyT& key, const ValT& value);
    void insert(const KeyT& key, ValT&& value);

    // Removes the given key from the tree if it is present. If not present,
    // will throw KeyError if discard is false or do nothing if it is true.
    void remove(const KeyT& key, bool discard=false) throw (KeyError);

  private:
    Node_t* treeSuccessor(const Node_t* node_in);

    #ifdef _DEBUG
    void checkInvariant() const;
    #endif

    // Find a key or where it should go in the tree.
    // Returns:
    // * Tree is empty: (nullptr, nullptr)
    // * Key is at root: (nullptr, root)
    // * Key is in tree: (parent, key)
    // * Key is not in tree: (parent_where_should_go, nullptr)
    std::pair<Node_t*, Node_t*> locate(const KeyT& key) const;
};

#include "rbtree.tcc"

#endif
