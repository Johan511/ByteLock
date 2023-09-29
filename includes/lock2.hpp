#include <atomic>
#include <boost/container/flat_map.hpp>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

template <template <class> class Guard = std::lock_guard,
          class Lock = std::mutex>
class ByteLock {

  template <typename Key, typename Value>
  using MapTy = boost::container::flat_map<Key, Value>;

  std::mutex mtx;
  std::size_t counter = 0;
  MapTy<std::size_t, std::pair<std::size_t, std::size_t>> rangeMap;
  MapTy<std::size_t, std::vector<std::atomic_flag *>> waiting;

public:
  std::size_t acquire_lock(std::size_t begin, std::size_t end);
  void release_lock(std::size_t lockId);
};