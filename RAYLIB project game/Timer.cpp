#include "Timer.h"

Timer::TimePoint Timer::GetTimePoint() const {
	return timePoint;
}


long long Timer::GetTimeToWait() const {
	return timeToWait;
}

long long Timer::GetElapsedTime() {
	auto tp = HRC::now();
	auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(tp - GetTimePoint()).count();
	if (timerId == TIMER_ID::WAITTIMER_ID) {
		if (elapsedTime < timeToWait) {
			return elapsedTime;
		}
		return timeToWait;
	}
	return elapsedTime;
}

void Timer::SetTimePoint(TimePoint tP) {
	timePoint = tP;
}

Timer::Timer(long long timeToWait, uint8_t timerId) : timeToWait(timeToWait), timerId(timerId) { timePoint = HRC::now(); }

Timer::~Timer() = default;

bool Timer::checkTimer() {
	auto tp = HRC::now();
	auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(tp - GetTimePoint()).count();

	if (timerId == TIMER_ID::WAITTIMER_ID) {
		if (elapsedTime >= GetTimeToWait()) {
			return true;
		}
		return false;
	}

	return false;
}

void Timer::reset() {
	SetTimePoint(HRC::now());
}

bool TimerContainer::checkTimer(uint32_t id) {
	return mapTimer.at(id).checkTimer();
}

void TimerContainer::resetTimer(uint32_t id) {
	return mapTimer.at(id).reset();
}

void TimerContainer::insertTimer(uint32_t id, Timer t) {
	mapTimer.try_emplace(id, t);
}

long long TimerContainer::getElapsedTime(uint32_t id) {
	return mapTimer.at(id).GetElapsedTime();
}

void TimerContainer::removeTimer(uint32_t id) {
	mapTimer.erase(id);
}