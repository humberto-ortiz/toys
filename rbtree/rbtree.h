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

#define _DEBUG

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
    std::unique_ptr<Node_t>& getInRef(Node_t* root_in) {
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

    void rotateLeft(Node_t* root_in) {
      std::unique_ptr<Node_t> tmp;
      std::unique_ptr<Node_t>& root = getInRef(root_in);

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

    void rotateRight(Node_t* root_in) {
      std::unique_ptr<Node_t> tmp;
      std::unique_ptr<Node_t>& root = getInRef(root_in);

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

  public:
    RBTree()
      : mTree(nullptr), mSize(0) {
      #ifdef _DEBUG
        checkInvariant();
      #endif
    }

    // Returns true if the key exists as a key in the tree, otherwise false.
    bool has(const KeyT& key) const {
      #ifdef _DEBUG
        checkInvariant();
      #endif
    
      return locate(key).second != nullptr;
    
      #ifdef _DEBUG
        checkInvariant();
      #endif
    }

    // Returns the value for the given tree.
    const ValT& get(const KeyT& key) const {
      #ifdef _DEBUG
        checkInvariant();
      #endif
    
      Node_t* loc = locate(key).second;
      if (!loc) {
        throw KeyError();
      } else {
        return loc->value;
      }
    
      #ifdef _DEBUG
        checkInvariant();
      #endif
    }

    // Returns the value for the given tree.
    ValT& get(const KeyT& key) {
      #ifdef _DEBUG
        checkInvariant();
      #endif
    
      Node_t* loc = locate(key).second;
      if (!loc) {
        throw KeyError();
      } else {
        return loc->value;
      }
    
      #ifdef _DEBUG
        checkInvariant();
      #endif
    }

    // Sets the value of the given key to the given value. This will replace the
    // current value (if any).
    void insert(const KeyT& key, const ValT& value) {
      #ifdef _DEBUG
        checkInvariant();
      #endif

      Node_t* prev;
      Node_t* loc;
      std::tie(prev, loc) = locate(key);
      if (loc) {
        loc->value = value;
      } else {
        std::unique_ptr<Node_t> new_node(new Node_t(key, value, COLOR::RED, prev));
        ++mSize;
        Node_t* cur = new_node.get();

        if (!mTree) {
          assert(mSize == 1);
          new_node->color = COLOR::BLACK;
          std::swap(mTree, new_node);
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
            Node_t* cousin = cur->parent->parent->right.get();
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
              // TODO: what if that if is false?
            }
          } else if (cur->parent->parent && cur->parent == cur->parent->parent->right.get()) {
            Node_t* cousin = cur->parent->parent->left.get();
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
              // TODO: what if that if is false?
            }
          }
        }
        mTree->color = COLOR::BLACK;
      }
      #ifdef _DEBUG
        checkInvariant();
      #endif
    }

    // Removes the given key from the tree if it is present. If not, this is a
    // no-op.
    void remove(const KeyT& key) {
      #ifdef _DEBUG
        checkInvariant();
      #endif

      Node_t* prev;
      Node_t* loc; // z
      Node_t* tmp; // y
      Node_t* cur; // x
      std::tie(prev, loc) = locate(key);
      if (!loc) {
        return;
      }

      --mSize;

      if (loc == mTree.get()) {
        // Deleted node is root.
        if (!loc->left && !loc->right) {
          mTree.reset();
          return;
        } else if (loc->left && !loc->right) {
          mTree = loc->left;
          return;
        } else if (loc->right && !loc->left) {
          mTree = loc->left;
          return;
        } else {
          Node_t* succ = treeSuccessor(loc);
          assert(!succ->left);
          std::swap(loc->key, succ->key);
          std::swap(loc->value, succ->value);

          if (succ->parent->left.get() == succ) {
            succ->parent->left = std::move(succ->right);
          } else {
            succ->parent->right = std::move(succ->right);
          }
        }
      } else {
        // Deleted node is not root.
        std::unique_ptr<Node_t>& holder = (loc->parent->left ?
                                           loc->parent->left.get() == loc :
                                           loc->parent->right);
        if (!loc->left && !loc->right) {
          holder.reset();
        } else if (loc->left && !loc->right) {
          holder.reset(loc->left);
        } else if (loc->right && !loc->left) {
          holder.reset(loc->right);
        } else {
          Node_t* succ = treeSuccessor(loc);
          assert(!succ->left);
          std::swap(loc->key, succ->key);
          std::swap(loc->value, succ->value);

          if (succ->parent->left.get() == succ) {
            succ->parent->left = std::move(succ->right);
          } else {
            succ->parent->right = std::move(succ->right);
          }
        }
      }

      /*
      if (!loc->left || !loc->right) {
        tmp = loc;
      } else {
        tmp = treeSuccessor(loc);
      }

      if (tmp->left) {
        cur = tmp->left.get();
      } else {
        cur = tmp->right.get();
      }

      cur->parent = tmp->parent;

      if (!tmp->parent) {
        mTree = getInRef(cur);
      } else if (tmp->parent->left.get() == tmp) {
        tmp->parent->left = cur;
      } else {
        tmp->parent->right = cur;
      }

      if (tmp != loc) {
        std::swap(cur->key, tmp->key);
        std::swap(cur->value, tmp->value);
      }

      if (tmp->color == COLOR::BLACK) {
        
      }*/
    
      #ifdef _DEBUG
        checkInvariant();
      #endif
    }

  private:
    void checkInvariant() const {
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

    // Find a key or where it should go in the tree.
    // Returns:
    // * Tree is empty: (nullptr, nullptr)
    // * Key is at root: (nullptr, root)
    // * Key is in tree: (parent, key)
    // * Key is not in tree: (parent_where_should_go, nullptr)
    std::pair<Node_t*, Node_t*> locate(const KeyT& key) const {
      Node_t* cur = mTree.get();
      Node_t* prev = nullptr;
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
};

#endif
