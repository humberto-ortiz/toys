#include "rbtree.h"

#include <iostream>
#include <vector>
#include <ctime>
#include <map>
/*
class foo {
  public:
    foo(int value) {
      val = new int(value);
      std::cout << "con" << std::endl;
    }

    foo() {
      val = NULL;
      std::cout << "con" << std::endl;
    }

    ~foo() {
      delete val;
      std::cout << "des" << std::endl;
    }

    bool operator< (const foo& rhs) const {
      return myval() < rhs.myval();
    }

    bool operator> (const foo& rhs) const {
      return myval() > rhs.myval();
    }



  private:
    int* val;
    const static int default_val = 0;

    const int& myval() const {
      if (val) {
        return *val;
      } else {
        return default_val;
      }
    }
};

const int foo::default_val;
*/
int main() {
  RBTree<int,int> tree;
  std::map<int,int> tester;

  /*
  tree.insert(43, 1);
  tree.insert(-26, 1);
  tree.insert(-44, 1);
  tree.insert(-24, 1);
  tree.insert(-103, 1);
  tree.insert(-21, 1);
  tree.insert(99, 1);
  tree.insert(111, 1);
  tree.insert(211, 1);
  tree.insert(213, 1);
  tree.insert(-213, 1);

  tree.remove(-44);
  tree.remove(-26);
  tree.remove(111);
  tree.remove(211);
  tree.remove(213);
  tree.remove(-213);
  */

  for (int i = 0; i < 14000000; ++i) {
    int key(rand() % 100000 - 50000);
    int val = rand() % 1000000 - 500000;
//    tree.insert(key, val);
    tester[key] = val;
  }

  for (int i = -50005; i < 50005; ++i) {
//    tree.remove(i);
    auto aa = tester.find(i);
    if (aa != tester.end()) {
      tester.erase(aa);
    }
  }

}
  /*
  for (int i = -5000; i < 5000; i++) {
    assert(!tree.has(i) == (tester.find(i) == tester.end()));
    if (tree.has(i)) {
      assert(tree.get(i) == tester.find(i)->second);
    }
  }
  */

//}
