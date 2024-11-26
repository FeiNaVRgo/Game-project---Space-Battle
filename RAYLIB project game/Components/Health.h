#pragma once
#include <cstdint>
#include <src/raylib-cpp.hpp>

struct Health {
	float maxHealth;//1
	float health;//2
	bool isDamaged;//3
	uint16_t frameImmunityTime;//4
	float healthToSubstract;//5
	bool toBeDamaged;//6

	void drawHealthBar(const raylib::Vector2& origin, const raylib::Vector2& offsetFromOrigin);

	void drawDamageIndicator();

	void clear();
};

