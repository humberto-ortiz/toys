#include "Summer.h"

SummerImpl::SummerImpl(const vector<vector<int> >& data_in) {
  // Infer dimensions of array.
  data = data_in;
  width = data_in.size();
  if (width == 0) {
    height = 0;
  } else {
    height = data_in[0].size();
  }

  // Base cases
  if (width > 0 && height > 0) {
    int rsum, csum;
    rsum = csum = data[0][0] = data_in[0][0];
    for (unsigned i = 1; i < width; ++i) {
      data[i][0] = rsum += data[i][0];
    }

    for (unsigned j = 1; j < height; ++j) {
      data[0][j] = csum += data[0][j];
    }
  }

  // Inductive step
  for (unsigned j = 1; j < height; ++j) {
    for (unsigned i = 1; i < width; ++i) {
      data[i][j] += data[i][j - 1] + data[i - 1][j] - data[i - 1][j - 1];
    }
  }
}

int SummerImpl::SumRect(unsigned x, unsigned y, unsigned w, unsigned h) const {
  if (width == 0 || height == 0 || w == 0 || h == 0) {
    return 0;
  }

  int sum = data[x + w - 1][y + h - 1];


  if (x > 0) {
    sum -= data[x - 1][y + h - 1];
  }

  if (y > 0) {
    sum -= data[x + w - 1][y - 1];
  }

  if (x > 0 && y > 0) {
    sum += data[x - 1][y - 1];
  }

  return sum;
}

SummerNaiveImpl::SummerNaiveImpl(const vector<vector<int> >& data_in) {
  data = data_in;
  width = data.size();

  if (width == 0) {
    height = 0;
  } else {
    height = data[0].size();
  }
}

int SummerNaiveImpl::SumRect(unsigned x, unsigned y, unsigned w,
                             unsigned h) const {
  int sum = 0;
  for (int i = x; i - x < w; ++i) {
    for (int j = y; j - y < h; ++j) {
      sum += data[i][j];
    }
  }
  return sum;
}
