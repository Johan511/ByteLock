#include "../includes/lock.hpp"

class SpinLock {
public:
  std::atomic<bool> flag = {0};
};

template <typename Lock> class SpinLockGuard {
  Lock *l;

public:
  SpinLockGuard(Lock &l_) {
    l = &l_;
    while (l->flag.exchange(true, std::memory_order_acquire)) {
    };
  }
  ~SpinLockGuard() { l->flag.store(false, std::memory_order_release); }
};

template <template <class> class Guard, class Lock>
std::size_t ByteLock<Guard, Lock>::acquire_lock(std::size_t begin,
                                                std::size_t end) {
  const Guard lock_guard(lock);

  for (auto const &it : map) {
    std::size_t b = it.second.first;
    std::size_t e = it.second.second;

    if ((b >= begin && b < end) || (e > begin && e <= end)) {
      return 0;
    }
  }

  map[++idx] = {begin, end};
  return idx;
}

template <template <class> class Guard, class Lock>
void ByteLock<Guard, Lock>::release_lock(std::size_t lockNum) {
  const Guard lock_guard(lock);
  map.erase(lockNum);
}