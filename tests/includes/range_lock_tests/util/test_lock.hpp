#include <iterator>
#include <range_lock/range_mutex.hpp>
#include <range_lock_tests/util/assertions.hpp>
#include <range_lock_tests/util/util.hpp>
#include <thread>

/*
  thread execution times of each run
*/
template <typename Lock, typename RangeEndGen, typename CriticalSection>
std::vector<std::vector<util::MThread::chrono_duration>> test_lock_n_times(
    std::size_t const n, std::size_t const len, std::size_t const numThreads,
    std::size_t const numIncrementsPerThreads, RangeEndGen &&rangeEndGen,
    CriticalSection &&criticalSection) {

  std::vector<std::vector<util::MThread::chrono_duration>> execTimeLog;
  execTimeLog.reserve(n);

  for (std::size_t i = 0; i != n; i++) {

    std::vector<util::MThread::chrono_duration> threadExecTimes =
        test_lock_once<Lock>(len, numThreads, numIncrementsPerThreads,
                             std::forward<RangeEndGen>(rangeEndGen),
                             std::forward<CriticalSection>(criticalSection));

    execTimeLog.emplace_back(std::move(threadExecTimes));
  }

  return execTimeLog;
}

/*
  1) Generates incremenet numThreads * numIncrementsPerThreads valid ranges
  2) Spawns numThreads threads and assigns all equal amount of work
  3) Checks if result is valid

  NOTE : Critical Section should take care of obtaining the lock
          passed as first parameter
*/
template <typename Lock, typename RangeEndGen, typename CriticalSection>
std::vector<util::MThread::chrono_duration>
test_lock_once(std::size_t const len, std::size_t const numThreads,
               std::size_t const numIncrementsPerThreads,
               RangeEndGen &&rangeEndGen, CriticalSection &&criticalSection) {
  std::vector<util::MThread> threads;
  std::vector<std::size_t> v(len);

  util::IncrementsTy increments =
      util::get_increment_ranges(numIncrementsPerThreads * numThreads, len,
                                 std::forward<RangeEndGen>(rangeEndGen));

  Lock bl;

  for (std::size_t i = 0; i != numThreads; i++) {

    util::IncrementsTy::iterator incrBeginIter =
        increments.begin() + (i * numIncrementsPerThreads);

    util::IncrementsTy::iterator incrEndIter =
        increments.begin() + ((i + 1) * numIncrementsPerThreads);

    threads.emplace_back(
        [&bl, &v, &criticalSection, incrBeginIter, incrEndIter, i]() {
          util::IncrementsTy::iterator mutableIncBeginIter = incrBeginIter;
          for (; mutableIncBeginIter != incrEndIter; mutableIncBeginIter++) {
            std::size_t begin = mutableIncBeginIter->first;
            std::size_t end = mutableIncBeginIter->second;

            criticalSection(bl, v, begin, end);
          }
        });
  }

  std::vector<util::MThread::chrono_duration> threadExecTimes(numThreads);

  for (std::size_t i = 0; auto &t : threads) {
    threadExecTimes[i++] = t.derived_join();
  }

  std::vector<std::size_t> expectedVector = util::get_final_vector(
      std::vector<std::size_t>(len), std::move(increments));

  if (!util::assertions::assert_eq(expectedVector, v)) {
    exit(1);
  }
  return threadExecTimes;
}
