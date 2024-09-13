#pragma once
#include "../globals.h"

class ComponentCommons {
public:
	template<typename Component, typename... Types>
	static inline void addComponent(ECS::Entity entity, Types... args) {
		G::gCoordinator.AddComponent<Component>(entity, Component(args...));
	}
};

