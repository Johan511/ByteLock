#pragma once

template <typename RangeLock> class range_guard {
  std::reference_wrapper<RangeLock> lockRef;
  std::size_t lockId = 0;

public:
  range_guard(RangeLock &lock, std::size_t begin, std::size_t end)
      : lockRef(lock) {
    lockId = lockRef.get().lock(begin, end);
  }
  ~range_guard() { lockRef.get().unlock(lockId); }
};