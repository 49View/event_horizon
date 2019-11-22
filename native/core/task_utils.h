#pragma once

#include <vector>
#include <future>
#include <string>

template<class T>
//wait for all tasks
void collect( std::vector<std::shared_ptr< std::future<T>>> tasks ) {
	for ( auto& task : tasks )
		task->wait();
}

template<class T>
//wait for all tasks
const std::vector<T> collect( std::string stage, std::vector<std::shared_ptr< std::future<T>>> tasks, std::function<void( std::string, size_t, size_t )> onProgress ) {
	size_t total = tasks.size();
	size_t count = 0;
	//auto sleeptime = std::chrono::microseconds( 10 );

	std::vector<T> results;
	results.reserve( total );

	for ( auto& task : tasks ) {
		task->wait();
		results.emplace_back( task->get() );

		count++;
		onProgress( stage, count, total );
	}

	return results;
}

//template <typename Fut, typename Work>
//auto then( Fut f, Work w )->std::shared_ptr < std::future<decltype( w( f.get() ) )>> {
//	return std::make_shared < std::future<auto >>( std::async( [=] { w( f->get() ); } ) );
//}

template <typename Fut, typename Work>
auto then( Fut f, Work w )->std::shared_ptr < std::future<void>> {
	return std::make_shared<std::future<void>>( std::async( [=] { w(); } ) );
}

class CancellationToken {
public:
	CancellationToken() :cancelled{ false } {};

	CancellationToken( CancellationToken  const & ) = delete;
	CancellationToken( CancellationToken  const && ) = delete;
	void operator=( CancellationToken const &x ) = delete;

	bool isCancellationRequested() const { return cancelled; }

private:
	bool cancelled;
	friend class CancellationTokenSource;
};

class CancellationTokenSource {
public:
	CancellationTokenSource() { token = std::make_shared<CancellationToken>(); }
	void cancel() { token->cancelled = true; }
	std::shared_ptr<CancellationToken> getToken() const { return token; }
private:
	std::shared_ptr<CancellationToken> token;
};