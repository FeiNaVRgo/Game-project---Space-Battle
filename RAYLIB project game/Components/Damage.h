#pragma once

struct Damage {
	enum class DAMAGE_TYPE {
		PHYSICAL = 0,
		EXPLOSIVE = 1,
		PENETRATION = 2,
		FIRE = 3,
		LIGHTNING = 4,
		POISON = 5,
		RADIANT = 6,
		COLD = 7,
		ENERGY = 8
	};

	float damage;//1
	float damageOnContact;//2
	DAMAGE_TYPE damageType{};//3
};


