#include "Stopwatch.h"

void Stopwatch::RegisterTimer(StopwatchCallback callback, std::string timer_name, duration updateAfter)
{
	_watches_map[timer_name].trigger_type = STOPWATCH_TRIGGER_TYPE::DURATION;
	_watches_map[timer_name].call_back = callback;
	_watches_map[timer_name].duration_limit = updateAfter;
}

void Stopwatch::RegisterTimer(StopwatchCallback callback, std::string timer_name, unsigned int updateAfterCalls)
{
	_watches_map[timer_name].trigger_type = STOPWATCH_TRIGGER_TYPE::CALLS;
	_watches_map[timer_name].call_back = callback;
	_watches_map[timer_name].calls_limit = updateAfterCalls;
}

void Stopwatch::StartTimer(std::string name)
{
	_watches_map[name].t1 = std::chrono::high_resolution_clock::now();
}

void Stopwatch::StopTimer(std::string name)
{
	auto _t2 = std::chrono::high_resolution_clock::now();
	IndividualWatch& watch = _watches_map[name];
	std::chrono::duration<double, std::milli> ms_double = _t2 - watch.t1;
	watch.time_accumulated += ms_double;
	watch.calls++;
	
	switch (watch.trigger_type)
	{
	case STOPWATCH_TRIGGER_TYPE::CALLS:
		
		if (watch.calls >= watch.calls_limit) {
			auto average = watch.time_accumulated / watch.calls;
			watch.call_back(average.count(), name);
			watch.calls = 0;
			watch.time_accumulated = std::chrono::duration < double, std::milli>(0);
		}
		break;
	case STOPWATCH_TRIGGER_TYPE::DURATION:
		if (watch.time_accumulated >= watch.duration_limit) {
			auto average = watch.time_accumulated / watch.calls;
			watch.call_back(average.count(), name);
			watch.calls = 0;
			watch.time_accumulated = std::chrono::duration < double, std::milli>(0);
		}
		break;
	default:
		break;
	}
}
