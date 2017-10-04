#ifndef __THREAD__POOL__28082017
#define __THREAD__POOL__28082017

#include <cstddef>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <string>
#include <vector>
#include <queue>

namespace thrplv2
{
	class ThreadPool
	{
	public:
		ThreadPool(const ptrdiff_t threadCount_ = std::thread::hardware_concurrency());
		virtual ~ThreadPool();

		int getThreadCount() const;

		template<class Func, class... Args>
		auto addTask(Func&& func, Args&&...args) ->
			std::future<typename std::result_of<Func(Args...)>::type>;

	private: // Private fields.
		size_t threadCount_{ 4 };
		std::mutex mutex_, mutexLog_;
		std::condition_variable cvNewTask_;
		std::vector<std::thread> threads_;
		std::queue < std::function<void()> > tasks_;
		std::atomic<bool> stop_{ false };

	private: // Private methods.
		std::string getCurrentTimeData_();
	};

	template<class Func, class... Args>
	auto ThreadPool::addTask(Func&& func, Args&&... args) ->
		std::future<typename std::result_of<Func(Args...)>::type>
	{
		using returnType = typename std::result_of<Func(Args...)>::type;
		auto task = std::make_shared< std::packaged_task<returnType()> >(
			[func, args...]() { return func(args...); });
		auto result = task->get_future();
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (stop_)
				throw std::runtime_error("Enqueue on stopped thread");

			tasks_.emplace([task]() { (*task)(); });
		}
		cvNewTask_.notify_one();
		return result;
	}

}

#endif // __THREAD__POOL__28082017
