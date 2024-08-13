#pragma once
#include <set>
#include <queue>
#include <functional>
#include <unordered_map>
#include <memory>
#include "ECS.h"
#include "globals.h"
#include "EntityIds.h"

namespace events {
	using Listener = ECS::Entity;
	using Target = ECS::Entity;
	using Args = std::list<float>;
	using ActionFunc = std::function<void(Target, Args)>;
	using Condition = std::function<bool(Listener)>;

	struct Actions {
		static inline void heal(Target target, Args args) {
			//bruh
		}
	};

	struct Action {
		ActionFunc func;
	};

	struct Event {
		std::set<Target> targets;
		Action action;
		Args args;
	};
	

	class EventManager { 
	private:
		std::queue<Event> mEvents{};

		void dispatchEvents(std::queue<Event>& events) {
			if (events.empty()) {
				return;
			}
			doEvent(events.front());
			events.pop();
			dispatchEvents(events);
		}
	public:
		EventManager() = default;

		void doEvent(Event event) {
			for (const auto& target : event.targets) {
				event.action.func(target, event.args);
			}
		}

		void dispatchEvents() {
			dispatchEvents(mEvents);
		}

		void addEvent(const Event &event) { mEvents.push(event); }
	};
	
	struct IListener {
		Listener listener;
		std::vector<std::pair<Event, Condition>> events;

		void checkEvent(EventManager& eventManager) {
			for (auto const& [event, condition] : events) {
				if (condition(listener)) {
					eventManager.addEvent(event);
				}
			}
		}
	};
}
