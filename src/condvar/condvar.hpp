#pragma once

#include <thread>
#include "mutex/mutex.hpp"

// Alas windows does not have futex nor ulock so it's going to be a stupid wrapper for winapi CONDITION_VARIABLE :< 
class CondVar {
	public:
	CondVar()
	{
		InitializeConditionVariable(&real_condvar);
	}

	~CondVar() {}

    CondVar(const CondVar&) = delete;
    CondVar& operator=(const CondVar&) = delete;

  	void Wait(Mutex& in_mutex) {
		// NOTE: This is fucking horrific :)
		EnterCriticalSection(&critical_section);
		
		in_mutex.Unlock();
		
		SleepConditionVariableCS(&real_condvar, &critical_section, INFINITE);
		LeaveCriticalSection(&critical_section);
		
		in_mutex.Lock();
  	}

  	void NotifyOne() {
		WakeConditionVariable(&real_condvar);
	}

  	void NotifyAll() {
		WakeAllConditionVariable(&real_condvar);
	}

	private:
	CONDITION_VARIABLE real_condvar;
	CRITICAL_SECTION critical_section;
};