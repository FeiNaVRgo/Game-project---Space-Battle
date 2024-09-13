#pragma once
#include <src/raylib-cpp.hpp>
#include "../ECS.h"


struct JustBorn {
	ECS::Entity parent;//1
	raylib::Vector2 bornPos;//2
	raylib::Vector2 bornVel;//3
	float bornAngle;//4
	double bornTime;//5
};

