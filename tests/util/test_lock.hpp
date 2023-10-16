#include "../../includes/lock2.hpp"
#include "./util.hpp"
#include <thread>

template <typename... Args>
void test_lock_n_times(std::size_t const n, Args &&...args) {
  for (std::size_t i = 0; i != n; i++) {
    test_lock_once(std::forward<Args>(args)...);
  }
}

/*
  1) Generates incremenet numThreads * numIncrementsPerThreads valid ranges
  2) Spawns numThreads threads and assigns all equal amount of work
  3) Checks if result is valid

  NOTE : Critical Section should take care of obtaining the lock
          passed as first parameter
*/
template <typename CriticalSection, typename RangeEndGen>
void test_lock_once(std::size_t const len, std::size_t const numThreads,
                    std::size_t const numIncrementsPerThreads,
                    RangeEndGen &&rangeEndGen,
                    CriticalSection &&criticalSection) {

  std::vector<std::thread> threads;
  std::vector<std::size_t> v(len);

  util::IncrementsTy increments =
      util::get_increment_ranges(numIncrementsPerThreads * numThreads, len,
                                 std::forward<RangeEndGen>(rangeEndGen));

  ByteLock<> bl;

  for (std::size_t i = 0; i != numThreads; i++) {

    util::IncrementsTy::iterator incrBeginIter =
        increments.begin() + (i * numIncrementsPerThreads);

    util::IncrementsTy::iterator incrEndIter =
        increments.begin() + increments.begin() +
        ((i + 1) * numIncrementsPerThreads);

    threads.emplace_back([&bl, &v, &modifier, incrBeginIter, incrEndIter, i]() {
      for (; incrBeginIter != incrEndIter; incrBeginIter++) {
        std::size_t begin = p.first;
        std::size_t end = p.second;

        criticalSection(bl, v, begin, end);
      }
    });
  }

  for (auto &t : threads)
    t.join();

  std::vector<std::size_t> finalVector = util::get_final_vector(
      std::vector<std::size_t> v(len), std::move(increments));

  if (!util::assertions::assert_eq(v, finalVector)) {
    exit(1);
  }
  return;
}
