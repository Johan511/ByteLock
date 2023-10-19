#include <iostream>
#include <mutex>
#include <range_lock_tests/util/test_lock.hpp>
#include <vector>

static std::size_t NUM_CORES = 40;
static std::size_t NUM_ITERATIONS = 200;
static std::size_t NUM_THREADS = 6;
static std::size_t NUM_INCREMENTS_PER_THREAD = 1'000;
static std::size_t LEN = 1'000'000;

int main() {
  auto const modifier = [](std::size_t &i) { i++; };

  auto const criticalSection =
      [modifier](std::mutex &bl, std::vector<std::size_t> &v,
                 std::size_t const begin, std::size_t const end) {
        bl.lock();

        for (std::size_t i = begin; i != end; i++)
          modifier(v[i]);

        bl.unlock();
      };
  // for (; NUM_THREADS <= NUM_CORES; NUM_THREADS += 2)
  std::vector<std::vector<util::MThread::chrono_duration>> executionTimes =
      test_lock_n_times<std::mutex>(NUM_ITERATIONS, LEN, NUM_THREADS,
                                    NUM_INCREMENTS_PER_THREAD, util::LE5000,
                                    criticalSection);

  return 0;
};
