#include <random>
#include <vector>

namespace util {

// TYPE DEFS /////////////////////////////////////////////////////////
using IncrementsTy = std::vector<std::pair<std::size_t, std::size_t>>;
//////////////////////////////////////////////////////////////////////

// DEFAULT PARAMS ////////////////////////////////////////////////////
static const auto LE5000 = [](std::size_t x, std::size_t max) {
  return std::min(x + 5000, max);
};

template <typename T> static const auto IncrementBy1 = [](T &x) { x++; };
//////////////////////////////////////////////////////////////////////

std::mt19937_64 mtGen(std::random_device{}());

/*
    Generates ranges (x, y) where x < y
    x is generated randomly
    y is generated using f(x)
*/
template <typename F>
IncrementsTy get_increment_ranges(std::size_t const size,
                                  std::size_t const len = 1'000'000,
                                  F &&f = LE5000) {

  IncrementsTy ranges;
  for (std::size_t i = 0; i != size; i++) {
    std::size_t x, y;

    x = mtGen % len;
    y = f(x, len);

    std::size_t begin = std::min(x, y);
    std::size_t end = std::max(x, y);

    ranges.emplace_back({begin, end});
  }
  return ranges;
}

/*
    for each range in increments
        for each element in range
            modifier(element)
*/
template <typename T, typename Modifier>
std::vector<T> get_final_vector(std::vector<T> &&initialVector,
                                IncrementsTy increments,
                                Modifier &&modifier = IncrementBy1<T>) {

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
