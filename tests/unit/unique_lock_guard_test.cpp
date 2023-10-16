#include "../../includes/lock2.hpp"
#include "../../includes/range_guard.hpp"
#include "../util/test_lock.hpp"

int main() {
  auto const modifier = [](std::size_t &i) { i++; };
  auto const criticalSection =
      [modifier](ByteLock<> &bl, std::vector<std::size_t> &v,
                 std::size_t const begin, std::size_t const end) {
        range_unique_lock<ByteLock<>> rul(bl, begin, end);

        for (std::size_t i = begin; i != end; i++)
          modifier(v[i]);
      };

  std::vector<std::vector<util::MThread::chrono_duration>> executionTimes =
      test_lock_n_times(1'000, 100, 10, 10, util::LE5000, criticalSection);
};
