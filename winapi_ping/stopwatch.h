#pragma once
#include <chrono>
#include <ctime>
#include <cstdint>

class Stopwatch
{
public:
	void start()
	{
		_time_start = std::chrono::high_resolution_clock::now();
	}

	std::chrono::steady_clock::duration duration()
	{
		//auto end_time = std::chrono::high_resolution_clock::now();
		return (std::chrono::high_resolution_clock::now() - _time_start);
		//return std::chrono::duration_cast<std::chrono::microseconds>(end_time - _time_start).count();
	}

	//uint32_t duration_ms()
	//{
	//	return duration_mcs() / 1000;
	//}

private:
	std::chrono::steady_clock::time_point _time_start;
};
