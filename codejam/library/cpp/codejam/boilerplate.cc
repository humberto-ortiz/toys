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
#include "boilerplate.h"

#include <iostream>
#include <vector>

namespace calmofthestorm {
namespace boilerplate {

#ifdef MULTITHREADED
size_t ProblemWrapper::active_count = 0;
std::condition_variable ProblemWrapper::parallel_cv;
std::mutex ProblemWrapper::parallel_lock;
#endif

std::istream& operator>> (std::istream& os, ProblemWrapper& p) {
  p.read(os);
  return os;
}

std::ostream& operator<< (std::ostream& os, const ProblemWrapper& p) {
  p.write(os);
  return os;
}

ProblemWrapper::ProblemWrapper(std::unique_ptr<ISolution> s)
: solution(std::move(s)) { }

void ProblemWrapper::read(std::istream& is) {
  solution->read(is);
}

#ifdef MULTITHREADED
void ProblemWrapper::solve_wrapper() {
  solution->solve();
  std::unique_lock<std::mutex> lock(parallel_lock);
  --this->active_count;
  this->parallel_cv.notify_one();
}
#endif

void ProblemWrapper::solve() {
  #ifdef MULTITHREADED
  {
    #ifdef PARALLELISM_LEVEL
      const size_t parallelism_level = PARALLELISM_LEVEL;
    #else
      const size_t parallelism_level = std::thread::hardware_concurrency();
    #endif
    std::unique_lock<std::mutex> lock(parallel_lock);
    while (active_count >= parallelism_level) {
      parallel_cv.wait(lock);
    }
    ++active_count;
  }
  std::future<void> tmp(
      std::async(std::launch::async, [this](){ this->solve_wrapper(); })
    );
  std::swap(tmp, done);
  #else
  solution->solve();
  #endif
}

void ProblemWrapper::write(std::ostream& os) const {
  #ifdef MULTITHREADED
  done.wait();
  #endif
  solution->write(os);
}

int boilerplate_main(int, char**, std::unique_ptr<ISolution> (*create_solution) ()) {
  size_t n;
  std::cin >> n;

  // Read the problems in and solve them asynchronously.
  std::vector<ProblemWrapper> problems;
  problems.reserve(n);
  for (size_t i = 0; i < n; ++i) {
    problems.push_back(ProblemWrapper(create_solution()));
    std::cin >> problems.back();
    problems.back().solve();
  }

  // Print the results.
  for (size_t i = 1; i <= n; ++i) {
    std::cout << "Case #" << i << ": " << problems[i - 1] << std::endl;
  }

  return 0;
}

}  // namespace boilerplate
}  // namespace calmofthestorm
