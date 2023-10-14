#include "../../includes/lock2.hpp"
#include "../util/util.hpp"
#include <thread>

template <typename Modifier, typename RangeEndGen>
bool test_lock(std::size_t const numRanges, std::size_t const numThreads,
               std::size_t const len,
               std::size_t const numIncrementsPerThreadRange,
               Modifier &&modifier, RangeEndGen &&rangeEndGen) {

  std::vector<std::thread> threads;
  std::vector<std::size_t> v(len);

  util::IncrementsTy increments = util::get_increment_ranges(
      numRanges, len, std::forward<RangeEndGen>(rangeEndGen));

  ByteLock<> bl;

  // fix
  for (std::size_t i = 0; i != numThreads; i++) {
    threads.emplace_back([&bl, &v, &increments, &modifier, i]() {
      for (auto const &p : increments) {
        std::size_t begin = p.first;
        std::size_t end = p.second;

        std::size_t lockId = bl.lock(begin, end);

        for (std::size_t i = begin; i < end; i++)
          modifier(v[i]);

        bl.unlock(lockId);
      }
    });
  }

  for (auto &t : threads)
    t.join();

  std::vector<std::size_t> finalVector = util::get_final_vector(
      std::vector<std::size_t> v(len), std::move(increments));

  return util::assertions::assert_eq(v, finalVector);
}

int main() {
  test_lock(
      100, 4, 100, 100, [](auto &x) { x++; }, util::LE5000);
}
