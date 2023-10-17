#include <range_lock/lock2.hpp>
#include <range_lock_tests/util/test_lock.hpp>

static std::size_t NUM_THREADS;
static std::size_t NUM_INCREMENTS_PER_THREAD;
static std::size_t LEN;

int main() {
  auto const modifier = [](std::size_t &i) { i++; };

  auto const criticalSection =
      [modifier](ByteLock<> &bl, std::vector<std::size_t> &v,
                 std::size_t const begin, std::size_t const end) {
        std::size_t lockId = bl.lock(begin, end);

        for (std::size_t i = begin; i != end; i++)
          modifier(v[i]);

        bl.unlock(lockId);
      };

  std::vector<std::vector<util::MThread::chrono_duration>> executionTimes =
      test_lock_n_times(1'000, 100, 10, 10, util::LE5000, criticalSection);
  return 0;
};
