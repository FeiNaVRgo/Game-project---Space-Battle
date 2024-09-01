#pragma once
#include "../globals.h"
#include "../Components.h"
#include "../ECS.h"
#include <unordered_map>

enum class ID_WEAPON {
	ID_CANON
};

enum class ID_WEAPON_RARITY {
	COMMON           = 0,
	UNCOMMON         = 1,
	RARE             = 2,
	EPIC             = 3,
	INSANE           = 4,
	LEGENDARY        = 5,
	MYTHICAL         = 6,
	TRANSCENDENT     = 7
};

struct WeaponMini {
	bool isHeld;//1
	bool isEquipped;//2

	ID_WEAPON_RARITY rarity;//3

	std::string name;//4
	std::string description;//5

	bool isSelected = false;
	bool afterSelecting = false;

	raylib::Vector2 posToStay{};
};

struct WeaponNormal {
	raylib::Texture2DUnmanaged bulletSprite;//1
};