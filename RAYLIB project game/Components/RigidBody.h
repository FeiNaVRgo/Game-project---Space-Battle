#pragma once
#include <src/raylib-cpp.hpp>

class Hitbox {
public:
	raylib::Rectangle hitboxRect;
	raylib::Color hitboxColor;

	raylib::Vector2 getHitBoxCenter();
};

template<typename T>
struct Side {
	T isLeft;//1
	T isRight;//2
	T isTop;//3
	T isBottom;//4
};

struct RigidBody {
	raylib::Vector2 velocity;//1
	raylib::Vector2 acceleration;//2
	Hitbox hitbox;//3
	bool isColliding;//4
	Side<bool> onWhatSideIsColliding;//5
};

