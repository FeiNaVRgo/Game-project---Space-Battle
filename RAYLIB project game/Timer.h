#pragma once
#include <chrono>
#include <map>
#include <functional>
#include <iostream>

enum TIMER_ID {
	WAITTIMER_ID
};

class Timer {
public:
	using TimePoint = std::chrono::steady_clock::time_point;
	using HRC = std::chrono::high_resolution_clock;

	TimePoint timePoint;
	long long timeToWait;
	uint8_t timerId;
	bool isValid = true;
	
	Timer(long long timeToWait, uint8_t timerId);
	~Timer();

	TimePoint GetTimePoint() const;
	long long GetTimeToWait() const;
	long long GetElapsedTime();
	void SetTimePoint(TimePoint tP);
	bool checkTimer();
	void reset();
};


class TimerContainer {
private:
	std::map<uint32_t, Timer> mapTimer{};
public:
	bool checkTimer(uint32_t id);
	void resetTimer(uint32_t id);
	void insertTimer(uint32_t id, Timer t);
	long long getElapsedTime(uint32_t id);
	void removeTimer(uint32_t id);
};


