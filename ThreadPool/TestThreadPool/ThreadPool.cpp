#define _CRT_SECURE_NO_WARNINGS

#include <cstddef>
#include <fstream>
#include <ctime>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <string>
#include <vector>
#include <queue>
#include "ThreadPool.h"

namespace thrplv2
{
	ThreadPool::ThreadPool(const ptrdiff_t threadCount) :
		threadCount_(threadCount), stop_(false)
	{
		if (threadCount_ <= 0) 
			throw std::runtime_error("threadCount can not be less than or equal to zero.");
			
		for (size_t i = 0; i < threadCount_; ++i)
		{
			threads_.emplace_back([this]()
			{
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(mutex_);
						cvNewTask_.wait(lock, [this]() { return !tasks_.empty() || stop_; });
						if (stop_ && tasks_.empty()) return;
						task = std::move(tasks_.front());
						tasks_.pop();
					}
					task();
				}
			});
		}
	}



	ThreadPool::~ThreadPool()
	{
		stop_ = true;
		cvNewTask_.notify_all();
		for (size_t i = 0; i < threads_.size(); ++i)
		{
			if (threads_[i].joinable())
				threads_[i].join();
		}
	}



	int ThreadPool::getThreadCount() const
	{
		return threadCount_;
	}



	std::string ThreadPool::getCurrentTimeData_()
	{
		time_t seconds = time(NULL);
		tm* timeInfo = localtime(&seconds);
		std::string result = asctime(timeInfo);
		return result;
	}

}
