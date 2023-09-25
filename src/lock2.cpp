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
  std::size_t acquire_lock(std::size_t begin, std::size_t end,
                           std::atomic_flag *flag) {
    const Guard lock_guard(mtx);

    for (auto const &it : rangeMap) {
      std::size_t b = it.second.first;
      std::size_t e = it.second.second;

      if (!(e < begin) && !(end < b)) {
        flag->clear();
        waiting[it.first].push_back(flag);
        return 0;
      }
    }
    flag->test_and_set();
    rangeMap[++counter] = {begin, end};
    return counter;
  }

  void release_lock(std::size_t lockId) {
    const Guard lock_guard(mtx);

    rangeMap.erase(lockId);

    std::vector<std::atomic_flag *> waitingFlags = std::move(waiting[lockId]);
    waiting.erase(lockId);

    for (std::atomic_flag *f : waitingFlags) {
      f->test_and_set();
      f->notify_all();
    }
  }
};