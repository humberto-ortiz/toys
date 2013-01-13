// Summer is a simple class that holds the prefix sums of the array for
// efficient querying.
//

#ifndef ARRAY_SUM_SUMMER_H_
#define ARRAY_SUM_SUMMER_H_

#include <map>
#include <vector>
#include <cstring>
#include <cassert>
#include <utility>

using std::map;
using std::vector;
using std::pair;
using std::make_pair;

class Summer {
  public:
    // Return the sum of the given region. Requires that all rects
    // be in bounds.
    virtual int SumRect(unsigned x, unsigned y, unsigned width,
                        unsigned height) const = 0;

    virtual ~Summer();

  protected:
    unsigned width;
    unsigned height;
    vector<vector<int> > data;
};

class SummerImpl : public Summer {
  public:
    // Perform precomputation on specified array. Requires that all inner
    // vectors be same length.
    explicit SummerImpl(const vector<vector<int> >& data);

    int SumRect(unsigned x, unsigned y, unsigned width, unsigned height) const;
};

class SummerNaiveImpl : public Summer {
  public:
    explicit SummerNaiveImpl(const vector<vector<int> >& data);
    int SumRect(unsigned x, unsigned y, unsigned width, unsigned height) const;
};

#endif  // ARRAY_SUM_SUMMER_H_
