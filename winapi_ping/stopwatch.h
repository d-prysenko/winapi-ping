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

	uint32_t duration_ns()
	{
		auto end_time = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(end_time - _time_start).count();
	}

	uint32_t duration_ms()
	{
		return duration_ns() / 1000;
	}

private:
	std::chrono::steady_clock::time_point _time_start;
};
