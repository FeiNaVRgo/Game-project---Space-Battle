#pragma once
#include <vector>

struct upgDebuff {
	enum class DEBUFF_TYPES {
		BLEED = 0,
		BURN = 1,
		CHILL = 2,
		FROSTBITE = 3,
		GLITTER = 4,
		JUDGMENT = 5,
		NECROSIS = 6,
		NETTLES = 7,
		POSION = 8,
		RUPTURE = 9,
		SCORCH = 10,
		SHOCK = 11,
		TOXIC = 12,
		ZAP = 13
	};

	bool isDamageOverTime;
	uint32_t damageOverTime;
};

struct Debuffs : upgDebuff {
	struct debuffProck {
		DEBUFF_TYPES type;
		uint32_t prock;
	};

	std::vector<debuffProck> debuffVec;

	void checkDebuffs() {
		//TODO
	}
};

