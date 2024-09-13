#pragma once
#include <src/raylib-cpp.hpp>
#include <unordered_map>
#include <memory>
#include <optional>
#include "../ECS.h"

struct Inventory;

enum class ID_WEAPON_TYPE {
	NORMAL = 1,
	MINI   = 2
};

enum class ID_WEAPON {
	ID_CANON   = 0,
	ID_MINIGUN = 1,
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

	ID_WEAPON id;//6
	std::shared_ptr<ECS::Entity> ptrWeaponNormal = nullptr;

	bool isSelected = false;
	bool afterSelecting = false;
	bool isNormalInWorld = false;

	raylib::Vector2 posToStay;
};

struct WeaponNormal {
	raylib::Texture2DUnmanaged bulletSprite;//1
	ID_WEAPON id;//2
};

struct WeaponType {
	ID_WEAPON_TYPE id;//1
};

struct Inventory;

namespace WEAPON_DEFINITIONS {
	namespace WEAPON_COMMONS {
		extern void entityAngleToPos(raylib::Vector2 const& entityPos, float& angle, raylib::Vector2 const& pos);
		extern std::optional<raylib::Vector2> findEmptySlot(Inventory& inv, raylib::Texture2DUnmanaged const& sprite, ECS::Entity WeaponMini);
	}

	namespace CANON {
		extern void createWeaponMini();
		extern void createWeaponNormal(Inventory& inv, WeaponMini& weaponMini);
		extern void behaviourWeaponNormal(ECS::Entity weaponNormalEntity);
	}

	namespace MINIGUN {
		extern void createWeaponMini();
		extern void createWeaponNormal(Inventory& inv, WeaponMini& weaponMini);
		extern void behaviourWeaponNormal(ECS::Entity weaponNormalEntity);
	}
}