#pragma once
#include <set>
#include <queue>
#include <functional>
#include <unordered_map>
#include <memory>
#include "ECS.h"


namespace events {
	using Listener   = ECS::Entity;
	using Target     = ECS::Entity;
	using Args       = std::list<float>;
	using ActionFunc = std::function<void(Target, Args)>;
	using Condition  = std::function<bool(Listener)>;  

	struct Actions {
		static inline void heal(Target target, Args const& args) {
			//bruh
		}
		static void splitBullet(Target target, Args const& args);
	
	};

	struct Action {
		ActionFunc func;
	};

	struct Event {
		Target target;
		Action action;
		Args args;
	};
	

	class EventManager { 
	private:
		std::queue<Event> mEvents{};

		void dispatchEvents(std::queue<Event>& events) {
			while (!events.empty()) {
				doEvent(events.front());
				events.pop();
			}
		}
	public:
		EventManager() = default;

		void doEvent(Event const& event) const {
			event.action.func(event.target, event.args);
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
