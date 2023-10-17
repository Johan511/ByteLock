#include <atomic>
#include <boost/container/flat_map.hpp>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

template <typename Lock = std::mutex,
          template <typename> typename Guard = std::lock_guard>
class ByteLock {

  template <typename Key, typename Value>
  using MapTy = boost::container::flat_map<Key, Value>;

  Lock mtx;
  std::size_t counter = 0;
  MapTy<std::size_t, std::pair<std::size_t, std::size_t>> rangeMap;
  MapTy<std::size_t, std::vector<std::reference_wrapper<std::atomic_flag>>>
      waiting;

public:
  std::size_t lock(std::size_t begin, std::size_t end);
  std::size_t try_lock(std::size_t begin, std::size_t end);
  void unlock(std::size_t lockId);
};
