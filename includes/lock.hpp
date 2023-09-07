#include <atomic>
#include <mutex>
#include <unordered_map>

template <template <class> class Guard = std::lock_guard,
          class Lock = std::mutex>
class ByteLock {
  Lock lock;
  std::size_t idx = 0;
  std::unordered_map<std::size_t, std::pair<std::size_t, std::size_t>> map;

public:
  std::size_t acquire_lock(std::size_t begin, std::size_t end);
  void release_lock(std::size_t lockNum);
};