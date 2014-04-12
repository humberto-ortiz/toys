#include "UnionFind.h"
#include <string>

namespace {
  using calmofthestorm::UnionFind;

  std::string hello("hello");
  std::string world("world");
  std::string today("today");
  std::string bees("bees");
}  // anonymous namespace

void test_basic() {
  UnionFind<std::string> uf;

  // Invalid at, size 0
  try {
    uf.at(std::string("hello"));
    assert(0);
  } catch (std::out_of_range) { }

  // Invalid union, size 0
  try {
    uf.union_sets(std::string("hello"), std::string("world"));
    assert(0);
  } catch (std::out_of_range) { }

  // Size is 0 elements, 0 sets.
  assert(uf.count_elements() == 0);
  assert(uf.count_sets() == 0);
}

void test_one_elem() {
  UnionFind<std::string> uf;

  // Size 1 set (add element by rvalue reference)
  assert(uf[std::string("hello")] == "hello");

  // Valid access by rvalue reference
  assert(uf[std::string("hello")] == "hello");

  // Union an element with itself.
  assert(uf.union_sets(hello, hello) == hello);
  assert(uf.at(hello) == hello);

  // Size is 0 elements, 0 sets.
  assert(uf.count_elements() == 1);
  assert(uf.count_sets() == 1);

  // Invalid access
  try {
    uf.at(std::string("invalid"));
    assert(0);
  } catch (std::out_of_range) { }

  // Invalid access
  try {
    uf.union_sets(hello, std::string("invalid"));
    assert(0);
  } catch (std::out_of_range) { }
}

void test_union_simple() {
  UnionFind<std::string> uf;
  uf[hello];

  // Add an element by rvalue reference (move).
  assert(uf[std::string("world")] == world);
  assert(uf.count_elements() == 2);
  assert(uf.count_sets() == 2);

  // Add two elements by const reference (copy).
  assert(uf[today] == today);

  assert(uf.count_elements() == 3);
  assert(uf.count_sets() == 3);

  assert(uf[bees] == bees);

  assert(uf.count_elements() == 4);
  assert(uf.count_sets() == 4);

  // Do some unioning!
  std::string world_today = uf.union_sets(world, today);
  assert(world_today == world || world_today == today);

  assert(uf.count_elements() == 4);
  assert(uf.count_sets() == 3);

  std::string hello_bees = uf.union_sets(hello, bees);
  assert(hello_bees == hello || hello_bees == bees);

  assert(uf.count_elements() == 4);
  assert(uf.count_sets() == 2);

  // Test access, comprehensive.
  assert(uf[world] == world_today);
  assert(uf[today] == world_today);
  assert(uf[hello] == hello_bees);
  assert(uf[bees] == hello_bees);

  assert(uf[std::string("world")] == world_today);
  assert(uf[std::string("today")] == world_today);
  assert(uf[std::string("hello")] == hello_bees);
  assert(uf[std::string("bees")] == hello_bees);

  assert(uf.at(world) == world_today);
  assert(uf.at(today) == world_today);
  assert(uf.at(hello) == hello_bees);
  assert(uf.at(bees) == hello_bees);
}

void test_agg_initialization() {
  UnionFind<std::string> uf{hello, world, today};

  assert(uf.at(hello) != uf.at(world));
  assert(uf.at(hello) != uf.at(today));
  assert(uf.at(world) != uf.at(today));
}

void test_union_representative() {
  UnionFind<std::string> uf{hello, world, today, bees};

  std::string world_today = uf.union_sets(world, today);
  std::string hello_bees = uf.union_sets(hello, bees);
  uf.union_sets(world_today, hello_bees);

  for (const auto& sr : {world, today, bees}) {
    assert(uf.at(sr) == uf.at(hello));
  }
}

void test_union_non_representative() {
  UnionFind<std::string> uf{hello, world, today, bees};

  std::string world_today = uf.union_sets(world, today);
  std::string hello_bees = uf.union_sets(hello, bees);

  uf.union_sets(
      world_today == world ? today : world,
      hello_bees == hello ? bees : hello
    );

  for (const auto& sr : {world, today, bees}) {
    assert(uf.at(sr) == uf.at(hello));
  }
}

void test_element_access() {
  UnionFind<std::string> uf{hello, world, today, bees};
  auto elem = uf.get_elements();
  std::sort(elem.begin(), elem.end());
  assert(elem == (std::vector<std::string>{bees, hello, today, world}));
}

void test_scalar() {
  UnionFind<int> uf;

  uf[27];
  uf[-5];

  assert(uf.at(27) == 27);
  assert(uf[-5] == -5);

  assert(uf.count_sets() == 2);
  assert(uf.count_elements() == 2);

  int res = uf.union_sets(27, -5);
  assert(res == -5 || res == 27);

  assert(uf.count_sets() == 1);
  assert(uf.count_elements() == 2);

  auto elem = uf.get_elements();
  std::sort(elem.begin(), elem.end());
  assert(elem == (std::vector<int>{-5, 27}));
}

void copy_move_test_helper(UnionFind<int>& uf, UnionFind<int>& uf2) {
  auto sorted1 = uf.get_elements();
  auto sorted2 = uf2.get_elements();
  std::sort(sorted1.begin(), sorted1.end());
  std::sort(sorted2.begin(), sorted2.end());

  auto ans = std::vector<int>{-3, 5, 8, 9, 17, 275};

  assert(sorted1 == ans);
  assert(sorted2 == ans);

  assert(uf[5] == uf[17]);
  assert(uf2[5] != uf2[17]);
}

void test_copy() {
  UnionFind<int> uf{5, 17, -3, 9, 275, 8};
  UnionFind<int> uf2 = uf;
  uf.union_sets(5, 17);
  copy_move_test_helper(uf, uf2);
  UnionFind<int> uf3(uf2);
  uf2.union_sets(5, 17);
  copy_move_test_helper(uf2, uf3);
}

void test_move() {
  UnionFind<int> uf{5, 17, -3, 9, 275, 8};
  UnionFind<int> uf_victim1(uf);
  UnionFind<int> uf_victim2(uf);

  UnionFind<int> uf1 = std::move(uf_victim1);

  UnionFind<int> uf2;
  std::swap(uf2, uf_victim2);

  uf.union_sets(5, 17);

  copy_move_test_helper(uf, uf1);
  copy_move_test_helper(uf, uf2);
}

void test_realloc() {
  UnionFind<short> bees;
  bees[0];
  for (short i = 1; i < 1000; i++) {
    bees[i];
    bees.union_sets(0, i);
  }
}

int main(int argc, char** argv) {
  test_basic();
  test_one_elem();
  test_agg_initialization();
  test_union_simple();
  test_union_representative();
  test_union_non_representative();
  test_element_access();
  test_scalar();
  test_copy();
  test_move();
  test_realloc();

  return 0;
}
