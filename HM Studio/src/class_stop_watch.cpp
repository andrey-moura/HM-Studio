#include "class_stop_watch.hpp"

void StopWatch::Start()
{
	m_Start = std::chrono::high_resolution_clock::now();
}

void StopWatch::Stop()
{
	m_Stop = std::chrono::high_resolution_clock::now();
}

long long StopWatch::Elapsed()
{
	return duration_cast<microseconds>(m_Stop - m_Start).count();	
}
