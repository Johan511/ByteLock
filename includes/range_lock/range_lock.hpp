#pragma once

template <typename RangeLock> class range_lock {
  std::reference_wrapper<RangeLock> lockRef;
  std::size_t lockId = 0;

public:
  range_lock(RangeLock &lock, std::size_t begin, std::size_t end)
      : lockRef(lock) {
    lockId = lockRef.get().lock(begin, end);
  }
  ~range_lock() { lockRef.get().unlock(lockId); }
};
