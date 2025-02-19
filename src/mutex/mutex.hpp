#pragma once

#include <Windows.h>
#include <atomic>
#include <synchapi.h>

class Mutex {
 public:
  void Lock() {
    while(lock_flag.exchange(true))
    {
        WaitOnAddress(&lock_flag, &undesired_value, sizeof(bool), INFINITE);
    }
  }

  void Unlock() {
    lock_flag.store(false);
    WakeByAddressSingle(&lock_flag);
  }

  void lock() {
    Lock();
  }

  void unlock() {
    Unlock();
  }

 private:
  std::atomic<bool> lock_flag = false;
  bool undesired_value = true;
};