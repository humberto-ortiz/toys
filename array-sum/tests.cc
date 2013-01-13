#include <cstdlib>
#include <iostream>

#include "Summer.h"

using std::cout;
using std::endl;

void assert_all_legal_equal(const Summer* a, const Summer* b,
                            unsigned width, unsigned height) {
  for (unsigned i = 0; i < width; ++i) {
    for (unsigned j = 0; j < height; ++j) {
      for (unsigned w = 0; w <= width - i; ++w) {
        for (unsigned h = 0; h <= height - j; ++h) {
          assert(a->SumRect(i, j, w, h) == b->SumRect(i, j, w, h));
        }
      } 
    }
  }
}

void randomize_2d_vector(vector<vector<int> >& data) {
  for (unsigned i = 0; i < data.size(); ++i) {
    for (unsigned j = 0; j < data[i].size(); ++j) {
      data[i][j] = rand() % 2000 - 1000;
    }
  }
}

void check_given_dims(int width, int height) {
  vector<vector<int> > test(width, vector<int>(height));

  randomize_2d_vector(test);

  SummerNaiveImpl ref(test);
  SummerImpl fast(test);

  assert_all_legal_equal(&ref, &fast, width, height);
}

int main() {
  check_given_dims(0, 0);
  check_given_dims(0, 1);
  check_given_dims(1, 0);
  check_given_dims(1, 1);

  check_given_dims(1, 2);
  check_given_dims(2, 1);
  check_given_dims(2, 2);

  check_given_dims(7, 3);
  check_given_dims(7, 7);
  return 0;
}
