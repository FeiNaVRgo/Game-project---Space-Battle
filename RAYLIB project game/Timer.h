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
	std::chrono::steady_clock::time_point timePoint;
	long long timeToWait;
	uint8_t timerId;
	bool isValid = true;

	std::chrono::steady_clock::time_point GetTimePoint() {
		return timePoint;
	}

	void SetTimePoint(std::chrono::steady_clock::time_point tP) {
		timePoint = tP;
	}

	long long GetTimeToWait() {
		return timeToWait;
	}

	long long GetElapsedTime() {
		auto tp = std::chrono::high_resolution_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(tp - GetTimePoint()).count();
		if(timerId == TIMER_ID::WAITTIMER_ID) {
			if (elapsedTime < timeToWait) {
				return elapsedTime;
			}
			return timeToWait;
		}
		return elapsedTime;
	}

	Timer(long long timeToWait, uint8_t timerId) : timeToWait(timeToWait), timerId(timerId) { timePoint = std::chrono::high_resolution_clock::now(); }

	~Timer() = default;

	bool checkTimer() {
		auto tp = std::chrono::high_resolution_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(tp - GetTimePoint()).count();

		if (timerId == TIMER_ID::WAITTIMER_ID) {
			if (elapsedTime >= GetTimeToWait()) {
				return true;
			}
			return false;
		}

		return false;
	}

	void reset() {
		SetTimePoint(std::chrono::high_resolution_clock::now());
	}
};


class TimerContainer {
private:
	std::map<uint32_t, Timer> mapTimer{};
public:
	bool checkTimer(uint32_t id) {
		return mapTimer.at(id).checkTimer();
	}

	void resetTimer(uint32_t id) {
		return mapTimer.at(id).reset();
	}

	void insertTimer(uint32_t id, Timer t) {
		mapTimer.try_emplace(id, t);
	}

	long long getElapsedTime(uint32_t id) {
		return mapTimer.at(id).GetElapsedTime();
	}

	void removeTimer(uint32_t id) {
		mapTimer.erase(id);
	}
};


