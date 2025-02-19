#pragma once

#include <mutex>
#include <condition_variable>
#include <deque>
#include <cassert>

#include <optional>

namespace tp {

// Unbounded blocking multi-producers/multi-consumers (MPMC) queue
template <typename T>
class UnboundedBlockingQueue {
public:
	// Thread role: producer
	bool Put(T new_element) {
		std::lock_guard<std::mutex> buffer_guard_lock(buffer_lock_);
		buffer_.push_back(new_element);
		buffer_not_empty_.notify_one();
		
		return true;
	}

	// Thread role: consumer
	std::optional<T> Take() {
		std::unique_lock unique_lock(buffer_lock_);
		while (buffer_.empty() && !closed_)
		{
			buffer_not_empty_.wait(unique_lock);
		}

		if (closed_)
		{
			return std::nullopt;
		}

		return TakeLocked();
	}

	void Close() {
		closed_ = true;
		buffer_not_empty_.notify_all();
	}

private:
	T TakeLocked() {
		assert(!buffer_.empty());

		T front{std::move(buffer_.front())};
		buffer_.pop_front();
		return front;
	}
private:	
	std::deque<T> buffer_; // protected by buffer_lock_
	std::mutex buffer_lock_{};

	bool closed_ = false;

	std::condition_variable buffer_not_empty_;
};

}  // namespace tp

