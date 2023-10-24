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
  MapTy<std::size_t, std::shared_ptr<std::atomic_flag>> waiting;

public:
  std::size_t lock(std::size_t begin, std::size_t end);
  std::size_t try_lock(std::size_t begin, std::size_t end);
  void unlock(std::size_t lockId);
};

template <class Lock, template <class> class Guard>
std::size_t ByteLock<Lock, Guard>::lock(std::size_t begin, std::size_t end) {
  std::size_t lockId = 0;
  bool localFlag = false;
  std::size_t blockIdx;

  std::shared_ptr<std::atomic_flag> waitingFlag;

  while (lockId == 0) {
    {
      const Guard<Lock> lock_guard(mtx);
      for (auto const &it : rangeMap) {
        std::size_t b = it.second.first;
        std::size_t e = it.second.second;

        if (!(e < begin) & !(end < b)) {
          blockIdx = it.first;
          localFlag = true;
          waitingFlag = waiting[blockIdx];
          break;
        }
      }
      if (localFlag == false) {
        ++counter;
        rangeMap[counter] = {begin, end};
        waiting[counter] =
            std::shared_ptr<std::atomic_flag>(new std::atomic_flag(false));
        return counter;
      }
      localFlag = false;
    }
    waitingFlag->wait(false);
  }
  return lockId;
}

template <class Lock, template <class> class Guard>
void ByteLock<Lock, Guard>::unlock(std::size_t lockId) {
  const Guard lock_guard(mtx);

  rangeMap.erase(lockId);

  waiting[lockId]->test_and_set();
  waiting[lockId]->notify_all();

  waiting.erase(lockId);
  return;
}

template <class Lock, template <class> class Guard>
std::size_t ByteLock<Lock, Guard>::try_lock(std::size_t begin,
                                            std::size_t end) {
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
