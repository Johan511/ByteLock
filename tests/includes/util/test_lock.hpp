#include "./util.hpp"
#include <range_lock/lock2.hpp>
#include <thread>

/*
  thread execution times of each run
*/
template <typename RangeEndGen, typename CriticalSection>
std::vector<std::vector<util::MThread::chrono_duration>> test_lock_n_times(
    std::size_t const n, std::size_t const len, std::size_t const numThreads,
    std::size_t const numIncrementsPerThreads, RangeEndGen &&rangeEndGen,
    CriticalSection &&criticalSection) {

  std::vector<std::vector<util::MThread::chrono_duration>> execTimeLog;
  execTimeLog.reserve(n);

  for (std::size_t i = 0; i != n; i++) {

    std::vector<util::MThread::chrono_duration> threadExecTimes =
        test_lock_once(len, numThreads, numIncrementsPerThreads,
                       std::forward<RangeEndGen>(rangeEndGen),
                       std::forward<CriticalSection>(criticalSection));

    execTimeLog.emplace_back(std::move(threadExecTimes));
  }
}

/*
  1) Generates incremenet numThreads * numIncrementsPerThreads valid ranges
  2) Spawns numThreads threads and assigns all equal amount of work
  3) Checks if result is valid

  NOTE : Critical Section should take care of obtaining the lock
          passed as first parameter
*/
template <typename RangeEndGen, typename CriticalSection>
util::MThread::chrono_duration
test_lock_once(std::size_t const len, std::size_t const numThreads,
               std::size_t const numIncrementsPerThreads,
               RangeEndGen &&rangeEndGen, CriticalSection &&criticalSection) {
  std::vector<util::MThread> threads;
  std::vector<std::size_t> v(len);

  util::IncrementsTy increments =
      util::get_increment_ranges(numIncrementsPerThreads * numThreads, len,
                                 std::forward<RangeEndGen>(rangeEndGen));

  ByteLock<> bl;

  for (std::size_t i = 0; i != numThreads; i++) {

    auto incrBeginIter = increments.begin() + (i * numIncrementsPerThreads);

    auto incrEndIter = increments.begin() + ((i + 1) * numIncrementsPerThreads);

    threads.emplace_back(
        [&bl, &v, &criticalSection, incrBeginIter, incrEndIter, i]() {
          for (; incrBeginIter != incrEndIter; incrBeginIter++) {
            std::size_t begin = incrBeginIter->first;
            std::size_t end = incrBeginIter->second;

            criticalSection(bl, v, begin, end);
          }
        });
  }

  std::vector<util::MThread::chrono_duration> threadExecTimes(numThreads);

  for (std::size_t i = 0; auto &t : threads) {
    threadExecTimes[i++] = t.derived_join();
  }

  std::vector<std::size_t> finalVector = util::get_final_vector(
      std::vector<std::size_t> v(len), std::move(increments));

  if (!util::assertions::assert_eq(v, finalVector)) {
    exit(1);
  }
  return threadExecTimes;
}