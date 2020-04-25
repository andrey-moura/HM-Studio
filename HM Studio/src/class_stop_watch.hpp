#pragma once

#include <chrono>

using namespace std::chrono;

class StopWatch
{
public:
	StopWatch() = default;;
	~StopWatch() = default;
private:
	time_point<steady_clock> m_Start;
	time_point<steady_clock> m_Stop;
public:
	void Start();
	void Stop();

	long long Elapsed();
};

