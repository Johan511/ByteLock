#include <range_lock/util/spin_lock.hpp>

void SpinLock::lock() {
  static constexpr timespec ns = {0, 1};
  int i = 0;
  do {
    if (i == 8) {
      i = 0;
      nanosleep(&ns, NULL);
    }
  } while ((flag.load(std::memory_order_relaxed) ||
            flag.exchange(true, std::memory_order_acquire)));
}

void SpinLock::unlock() { flag.store(false); }

bool SpinLock::try_lock() {
  return !(flag.load(std::memory_order_relaxed) ||
           flag.exchange(true, std::memory_order_acquire));
}
