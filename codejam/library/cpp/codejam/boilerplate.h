// Alex Roper
// alex@aroper.net
//

#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <cmath>
#include <deque>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifndef NOMT
  #define MULTITHREADED
#endif

#ifdef LP
  #define PARALLELISM_LEVEL LP
#endif

namespace calmofthestorm {
namespace boilerplate {

using namespace std;

class ISolution {
  public:
    // Read the problem from the specified stream.
    virtual void read(std::istream& is) = 0;

    // Solve the problem. Read must have been previously called.
    virtual void solve() = 0;

    // Write the solution to the specified stream. Solve must have
    // been previously called.
    virtual void write(std::ostream& os) const = 0;

    virtual ~ISolution() { }
};

class ProblemWrapper {
  public:
    explicit ProblemWrapper(std::unique_ptr<ISolution> solution);

    // Read problem in from file.
    void read(std::istream& is = std::cin);

    // Do the computation asynchronously. Will return as soon as the task
    // is scheduled.
    void solve();

    // Wait for and print the result.
    void write(std::ostream& os = std::cout) const;

  private:
    std::unique_ptr<ISolution> solution;

    #ifdef MULTITHREADED
    void solve_wrapper();

    static size_t active_count;
    static std::mutex parallel_lock;
    static std::condition_variable parallel_cv;
    std::future<void> done;
    #endif
};

int boilerplate_main(
    int argc,
    char** argv,
    std::unique_ptr<ISolution> (*create_solution) ()
  );

}  // namespace boilerplate
}  // namespace calmofthestorm
