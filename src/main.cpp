#include "lock2.cpp"
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

#define MY_ASSERT(clause, log)                                                 \
  if (!(clause)) {                                                             \
    std::cout << "Assertion failed: " << #clause << std::endl;                 \
    std::cout << "File: " << __FILE__ << std::endl;                            \
    fflush(stdout);                                                            \
    log();                                                                     \
    exit(1);                                                                   \
  }

std::size_t numThreads = 6;
static constexpr std::size_t numCores = 40;
static constexpr std::size_t len = 1'000'000;
static constexpr std::size_t numIncrementsPerThread = 10'000;

std::ofstream debugOut("debug.log", std::ios::out | std::ios::trunc);
std::ofstream timeLog("time.csv", std::ios::out | std::ios::trunc);

std::vector<std::pair<std::size_t, std::size_t>>
get_increment_ranges(std::size_t size, std::size_t range = 0) {
  std::vector<std::pair<std::size_t, std::size_t>> ranges;
  for (std::size_t i = 0; i != size; i++) {
    std::size_t x, y;

    x = rand() % len;
    y = std::min(x + 5000, len);

    std::size_t begin = std::min(x, y);
    std::size_t end = std::max(x, y);

    ranges.push_back({begin, end});
  }
  return ranges;
}

std::vector<std::size_t> get_final_vector(
    std::vector<std::pair<std::size_t, std::size_t>> const &increments) {
  std::vector<std::size_t> v(len);
  for (auto const &p : increments) {
    std::size_t begin = p.first;
    std::size_t end = p.second;

    for (std::size_t i = begin; i < end; i++)
      v[i]++;
  }
  return v;
}

void whole_lock(std::vector<std::pair<std::size_t, std::size_t>> &&increments) {
  // ByteLock<SpinLockGuard, SpinLock> bl;
  std::mutex mtx;

  std::vector<std::thread> threads;
  std::vector<std::size_t> v(len);

  std::vector<int64_t> timeTaken(numThreads);

  for (std::size_t i = 0; i < numThreads; i++) {
    threads.emplace_back(std::thread([&v, &increments, &mtx, i, &timeTaken]() {
      std::chrono::time_point<std::chrono::system_clock> const start =
          std::chrono::system_clock::now();
      for (std::size_t _ = numIncrementsPerThread * i;
           _ < numIncrementsPerThread * (i + 1); _++) {
        std::pair<std::size_t, std::size_t> p = increments[_];

        const std::lock_guard<std::mutex> lock(mtx);
        // std::cout << "Lock Acquired: " << lockId << std::endl;
        for (std::size_t x = p.first; x < p.second; x++) {
          v[x]++;
        }
        // std::cout << "Lock Released: " << lockId << std::endl;
      }
      std::chrono::time_point<std::chrono::system_clock> const end =
          std::chrono::system_clock::now();

      int64_t tt =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
              .count();

      timeTaken[i] = tt;
    }));
  }
  for (std::thread &t : threads)
    t.join();

  auto const final_vector = get_final_vector(increments);

  MY_ASSERT(v == final_vector, [&]() {
    for (std::size_t i = 0; i != v.size(); i++) {
      if (v[i] != final_vector[i])
        debugOut << "--------------- ";
      debugOut << v[i] << " " << final_vector[i] << '\n';
    }
  });

  timeLog << std::accumulate(timeTaken.begin(), timeTaken.end(), 0) / numThreads
          << '\n';
}

void run(std::vector<std::pair<std::size_t, std::size_t>> &&increments) {
  // ByteLock<SpinLockGuard, SpinLock> bl;
  ByteLock<std::lock_guard, std::mutex> bl;

  std::vector<std::thread> threads;
  std::vector<std::size_t> v(len);

  std::vector<int64_t> timeTaken(numThreads);

  for (std::size_t i = 0; i < numThreads; i++) {
    threads.emplace_back(std::thread([&v, &bl, &increments, i, &timeTaken]() {
      std::chrono::time_point<std::chrono::system_clock> const start =
          std::chrono::system_clock::now();
      for (std::size_t _ = numIncrementsPerThread * i;
           _ < numIncrementsPerThread * (i + 1); _++) {
        std::pair<std::size_t, std::size_t> p = increments[_];
        std::size_t lockId = bl.acquire_lock(p.first, p.second);
        // std::cout << "Lock Acquired: " << lockId << std::endl;
        for (std::size_t x = p.first; x < p.second; x++) {
          v[x]++;
        }
        // std::cout << "Lock Released: " << lockId << std::endl;
        bl.release_lock(lockId);
      }
      std::chrono::time_point<std::chrono::system_clock> const end =
          std::chrono::system_clock::now();

      int64_t tt =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
              .count();

      timeTaken[i] = tt;
    }));
  }

  for (std::thread &t : threads)
    t.join();

  auto const final_vector = get_final_vector(increments);

  MY_ASSERT(v == final_vector, [&]() {
    for (std::size_t i = 0; i != v.size(); i++) {
      if (v[i] != final_vector[i])
        debugOut << "--------------- ";
      debugOut << v[i] << " " << final_vector[i] << '\n';
    }
  });

  timeLog << std::accumulate(timeTaken.begin(), timeTaken.end(), 0) / numThreads
          << '\n';
}

int main() {
  // while (numThreads <= numCores) {
  for (int i = 0; i < 200; i++) {
    timeLog << numThreads << ',';
    std::vector<std::pair<std::size_t, std::size_t>> increments =
        get_increment_ranges(numIncrementsPerThread * numThreads);

    run(std::move(increments));
  }
  numThreads += 2;
  // }
}
