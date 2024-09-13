#pragma once
#include <src/raylib-cpp.hpp>
#include "../ECS.h"

class Sprite {
public:
	raylib::Texture2DUnmanaged sprite;//1
	float angle;//2
	raylib::Color tint;//3
	raylib::Vector2 origin;//4
	bool isDependent = false;//5

	void draw(ECS::Entity entity);

	~Sprite();
};

