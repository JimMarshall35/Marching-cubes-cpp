#pragma once
#include <chrono>
#include <map>
#include <string>
#include <functional>
typedef std::function<void(double, std::string)> StopwatchCallback;  // double timer_result, string timer_name
typedef std::chrono::duration < double, std::milli>  duration;
typedef std::chrono::steady_clock::time_point time_point;

enum class STOPWATCH_TRIGGER_TYPE{
	NONE,
	CALLS,
	DURATION
};
struct IndividualWatch {
	time_point t1;
	duration time_accumulated;
	int calls;
	STOPWATCH_TRIGGER_TYPE trigger_type = STOPWATCH_TRIGGER_TYPE::NONE;
	StopwatchCallback call_back;
	union {
		duration duration_limit;
		int calls_limit;
	};
};
class Stopwatch
{
public:
	void RegisterTimer(StopwatchCallback callback, std::string timer_name, duration updateAfter);
	void RegisterTimer(StopwatchCallback callback, std::string timer_name, unsigned int updateAfterCalls);
	void StartTimer(std::string name);
	void StopTimer(std::string name);
private:
	std::map<std::string, IndividualWatch> _watches_map;
};

