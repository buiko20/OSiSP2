#include <iostream>
#include <future>
#include"ThreadPool.h"

/* g++ main.cpp ThreadPool.cpp -std=c++11 -pthread -o 1.exe
исключения
https://msdn.microsoft.com/ru-ru/library/dd293602.aspx
*/

#ifdef _WIN32
	#include<Windows.h>
#else
	#include<unistd.h>
	#define Sleep(x) sleep(x / 1000)
#endif // _WIN32

int ThreadFunc1()
{
	for (int i = 0; i < 5; ++i)
	{
		printf("ThreadFunc1\n");
		fflush(stdout);
		Sleep(1000);
	}
	return 0;
}

int ThreadFunc2(const int intValue)
{
	for (int i = 0; i < 5; ++i)
	{
		printf("ThreadFunc2, parameters: %d\n", intValue);
		fflush(stdout);
		Sleep(1000);
	}
	return 0;
}

int ThreadFunc3(const int intValue, const std::string stringValue)
{
	for (int i = 0; i < 5; ++i)
	{
		printf("ThreadFunc3, parameters: %d %s\n", intValue, stringValue.c_str());
		fflush(stdout);
		Sleep(1000);
	}
	return 0;
}

int ThreadFunc4(const int intValue)
{
	for (int i = 0; i < 5; ++i)
	{
		printf("ThreadFunc4, parameters: %d\n", intValue);
		fflush(stdout);
		Sleep(1000);
		if (i == 3)
			throw std::runtime_error("ThreadFunc4 exception");
	}
	return 0;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	printf("Hardware concurrency: %u\n", std::thread::hardware_concurrency());

	thrplv2::ThreadPool threadPool;

	auto res1 = threadPool.addTask(&ThreadFunc1);
	auto res2 = threadPool.addTask(&ThreadFunc2, 42);
	auto res3 = threadPool.addTask(&ThreadFunc3, 22, std::string("Hello thread3"));
	auto res4 = threadPool.addTask(&ThreadFunc4, 22);

	printf("ThreadFunc1 result = %d\n", res1.get());
	printf("ThreadFunc2 result = %d\n", res2.get());
	printf("ThreadFunc3 result = %d\n", res3.get());
	try
	{
		printf("ThreadFunc4 result = %d\n", res4.get());
	}
	catch (std::exception ex)
	{
		printf("%s\n", ex.what());
	}
	
	getchar();
	return 0;
}
