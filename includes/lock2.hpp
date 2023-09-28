#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

template <template <class> class Guard = std::lock_guard,
          class Lock = std::mutex>
class ByteLock {
  std::mutex mtx;
  std::size_t counter = 0;
  std::unordered_map<std::size_t, std::pair<std::size_t, std::size_t>> rangeMap;
  std::unordered_map<std::size_t, std::vector<std::atomic_flag *>> waiting;

public:
  std::size_t acquire_lock(std::size_t begin, std::size_t end);
  void release_lock(std::size_t lockId);
};