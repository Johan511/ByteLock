#include <algorithm>
#include <atomic>
#include <random>
#include <thread>
#include <vector>

namespace util {

// TYPE DEFS /////////////////////////////////////////////////////////
using IncrementsTy = std::vector<std::pair<std::size_t, std::size_t>>;
//////////////////////////////////////////////////////////////////////

// DEFAULT PARAMS ////////////////////////////////////////////////////
const auto LE5000 = [](std::size_t x, std::size_t max) {
  return std::min(x + 5000, max);
};

template <typename T> const auto IncrementBy1 = [](T &x) { x++; };
//////////////////////////////////////////////////////////////////////

std::mt19937_64 mtGen(std::random_device{}());

/*
  1) Derived from std::thread, Mthread is final
  2) maintains start and end time of thread
  3) join is removed by making it private
  4) derived_join to be used in place of join, returns execution time
*/
class MThread final : public std::thread {

  // make sure join is not callable, only derived_join is
  using std::thread::join;

public:
  using chrono_granularity = std::chrono::high_resolution_clock;
  using chrono_duration = std::chrono::duration<double>;

  template <typename F, typename... Args>
  MThread(F &&f, Args &&...args)
      : std::thread(
            [f = std::forward<F>(f)](Args &&...args) mutable {
              auto const begin = chrono_granularity::now();
              f(std::forward<Args>(args)...);
              auto const end = chrono_granularity::now();
              std::cout
                  << std::chrono::duration_cast<std::chrono::milliseconds>(
                         end - begin)
                         .count()
                  << std::endl;
            },
            std::forward<Args>(args)...) {}

  // overloading based on return type not allowed
  chrono_duration derived_join() {
    this->join();
    return chrono_granularity::now().time_since_epoch();
  };
};

/*
    Generates ranges (x, y) where x < y
    x is generated randomly
    y is generated using f(x)
*/
template <typename RangeEndGen = decltype(LE5000)>
IncrementsTy
get_increment_ranges(std::size_t const numRanges,
                     std::size_t const len = 1'000'000,
                     RangeEndGen &&f = std::forward<RangeEndGen>(LE5000)) {

  IncrementsTy ranges;
  ranges.resize(numRanges);

  for (std::size_t i = 0; i != numRanges; i++) {
    std::size_t x, y;

    x = mtGen() % len;
    y = f(x, len);

    std::size_t begin = std::min(x, y);
    std::size_t end = std::max(x, y);

    ranges[i] = std::make_pair(begin, end);
  }

  return ranges;
}

/*
    for each range in increments
        for each element in range
            modifier(element)
*/
template <typename T, typename Modifier = decltype(IncrementBy1<T>)>
std::vector<T> get_final_vector(
    std::vector<T> &&initialVector, IncrementsTy increments,
    Modifier &&modifier = std::forward<Modifier>(IncrementBy1<T>)) {

  const auto ForEachUnaryFunc = [&modifier, &initialVector](auto const &p) {
    std::size_t begin = p.first;
    std::size_t end = p.second;

    for (std::size_t i = begin; i < end; i++)
      modifier(initialVector[i]);
  };

  std::for_each(increments.begin(), increments.end(), ForEachUnaryFunc);

  return initialVector;
}

} // namespace util
