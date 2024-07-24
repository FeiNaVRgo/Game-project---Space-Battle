#pragma once
#include <coroutine>

struct ReturnObject {
	struct promise_type {
		int val_;
		promise_type() = default;
		std::suspend_never initial_suspend() { return {}; }
		std::suspend_never final_suspend() noexcept { return {}; }
		ReturnObject get_return_object() { return ReturnObject{ std::coroutine_handle<promise_type>::from_promise(*this)}; }
		std::suspend_always yield_value(int val) {
			this->val_ = val;
			return std::suspend_always{};
		}
		void unhandled_exception(){}
		void return_void(){}
	};

	std::coroutine_handle<promise_type> h_;
	ReturnObject(std::coroutine_handle<promise_type> h) : h_{ h } {}
	operator std::coroutine_handle<promise_type>() const { return h_; }
	int get_value() const { return h_.promise().val_; }
};

static inline ReturnObject foo() {
	
	co_await std::suspend_always{};
}

static inline ReturnObject generator(int start, int end, int step = 1) {
	for (int val{ start }; val < end; val += step) {
		co_yield val;
	}
}