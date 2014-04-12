// Coding contest library (C++)
// Alex Roper
// alex@aroper.net
//
// Note: Relies on Boost (http://www.boost.org)
//
// All code available under the terms of the GPLv3, and in particular you are
// welcome to use this library in your own contest coding. See
// https://github.com/calmofthestorm/toys/codejam/library for the latest version
// or to submit issues/pull requests.
//
#include "template.h"

namespace {

class Solution : public ISolution {
  public:
    virtual void read(std::istream& is) override final;
    virtual void solve() override final;
    virtual void write(std::ostream& os) const override final;

  private:
};

void Solution::read(std::istream& is) {

}

void Solution::write(std::ostream& os) const {

}

void Solution::solve() {

}

std::unique_ptr<ISolution> create_solution() {
  return std::unique_ptr<ISolution>(new Solution);
}

}  // anonymous namespace

// Solve the typical Codejam problem (N test cases, ...) in parallel.
int main(int argc, char** argv) {
  boilerplate_main(argc, argv, create_solution);
  return 0;
}
