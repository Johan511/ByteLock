#pragma once

#include <atomic>

class SpinLock {
  std::atomic<bool> flag{false};

public:
  void lock();
  bool try_lock();
  void unlock();
};