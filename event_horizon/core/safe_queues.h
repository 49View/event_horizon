#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class ProducerConsumerQueue {
public:
	ProducerConsumerQueue() : queue(), mutex(), sync() {
	}

	~ProducerConsumerQueue() {
	}

	void enqueue( T t ) {
		std::lock_guard<std::mutex> lock( mutex );
		queue.push( t );
		sync.notify_one();
	}

	T dequeue() {
		std::unique_lock<std::mutex> lock( mutex );
		while ( queue.empty() ) {
			sync.wait( lock );
		}
		T val = queue.front();
		queue.pop();
		return val;
	}

	bool empty() {
		return queue.empty();
	}

private:
	std::queue<T> queue;
	mutable std::mutex mutex;
	std::condition_variable sync;
};

template <class T>
class ConcurrentQueue {
public:
	ConcurrentQueue() : queue(), mutex() {
	}

	~ConcurrentQueue() {
	}

	void enqueue( std::shared_ptr<T> t ) {
		std::lock_guard<std::mutex> lock( mutex );
		queue.push( t );
	}

	std::shared_ptr<T> dequeue() {
		std::lock_guard<std::mutex> lock( mutex );

		if ( !queue.empty() ) {
			auto val = queue.front();
			queue.pop();
			return val;
		}

		return nullptr;
	}

	bool empty() {
		std::lock_guard<std::mutex> lock( mutex );
		return queue.empty();
	}

	size_t size() {
		std::lock_guard<std::mutex> lock( mutex );
		return queue.size();
	}

private:
	std::queue<std::shared_ptr<T>> queue;
	mutable std::mutex mutex;
};
