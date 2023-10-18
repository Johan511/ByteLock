#pragma once

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

template <class Lock, template <class> class Guard>
std::size_t ByteLock<Lock, Guard>::lock(std::size_t begin, std::size_t end) {
  std::size_t lockId = 0;
  std::atomic_flag flag(true);
  bool localFlag = 0;
  while (lockId == 0) {
    {
      const Guard<Lock> lock_guard(mtx);
      for (auto const &it : rangeMap) {
        std::size_t b = it.second.first;
        std::size_t e = it.second.second;

        if (!(e < begin) && !(end < b)) {
          waiting[it.first].push_back(flag);
          localFlag = true;
          break;
        }
      }
      if (localFlag == false) {
        rangeMap[++counter] = {begin, end};
        lockId = counter;
      }
    }
    if (localFlag != false) {
      flag.wait(0);
      localFlag = false;
    }
  }
  return lockId;
}

template <class Lock, template <class> class Guard>
void ByteLock<Lock, Guard>::unlock(std::size_t lockId) {
  const Guard lock_guard(mtx);

  rangeMap.erase(lockId);

  std::vector<std::reference_wrapper<std::atomic_flag>> waitingFlags =
      std::move(waiting[lockId]);
  waiting.erase(lockId);

  for (std::atomic_flag &f : waitingFlags) {
    f.test_and_set();
    f.notify_all();
  }
}

template <class Lock, template <class> class Guard>
std::size_t ByteLock<Lock, Guard>::try_lock(std::size_t begin, std::size_t end) {
  const Guard lock_guard(mtx);
  for (auto const &it : rangeMap) {
    std::size_t b = it.second.first;
    std::size_t e = it.second.second;

    if (!(e < begin) && !(end < b)) {
      return 0;
    }
  }
  rangeMap[++counter] = {begin, end};
  return counter;
}

