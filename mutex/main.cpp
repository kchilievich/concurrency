#pragma once

#include <windows.h>
#include <atomic>
#include <synchapi.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "Synchronization.lib")

// POSIX clock_gettime for Windows port
LARGE_INTEGER
getFILETIMEoffset()
{
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}

int
clock_gettime(struct timeval *tv)
{
    LARGE_INTEGER           t;
    FILETIME            f;
    double                  microseconds;
    static LARGE_INTEGER    offset;
    static double           frequencyToMicroseconds;
    static int              initialized = 0;
    static BOOL             usePerformanceCounter = 0;

    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        } else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }
    if (usePerformanceCounter) QueryPerformanceCounter(&t);
    else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_usec = t.QuadPart % 1000000;
    return (0);
}
// END

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

 private:
  std::atomic<bool> lock_flag = false;
  bool undesired_value = true;
};

void CriticalFunction()
{
    for (int i = 0; i < 10; ++i)
    {
        std::cout << "Tick: " << i << std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main()
{
    Mutex mutex;

    // Warmup
    mutex.Lock();
    mutex.Unlock();

    std::thread sleeper([&] {
      mutex.Lock();
      std::this_thread::sleep_for(std::chrono::seconds(3));
      CriticalFunction();
      mutex.Unlock();
    });

    std::thread waiter([&] {
      std::this_thread::sleep_for(std::chrono::seconds(1));

      timeval start;
      clock_gettime(&start);

      mutex.Lock();
      CriticalFunction();
      mutex.Unlock();

      timeval now;
      clock_gettime(&now);

      std::cout << "Spent: " << (now.tv_usec - start.tv_usec) / 1000.f << "ms" << std::endl;
    });

    sleeper.join();
    waiter.join();

}
