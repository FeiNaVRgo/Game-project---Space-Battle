#pragma once
#include <src/raylib-cpp.hpp>
#include <magic_enum_all.hpp>

#include <unordered_map>
#include <memory>
#include <optional>

#include "../FunctionalBox.h"
#include "../ECS.h"
#include "utils-virtual-static-method.h"

struct Inventory;

enum class ID_WEAPON_TYPE {
	NORMAL = 1,
	MINI   = 2
};

enum class ID_WEAPON {
	ID_CANON       = 0,
	ID_MINIGUN     = 1,
	ID_LASERPISTOL = 2
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

struct WeaponInfo {
	FunctionalBox box_name;
	FunctionalBox box_stats;
	FunctionalBox box_description;
	FunctionalBox box_rarity;
	
	void updateInfoVariables(ECS::Entity weaponMini);
	void drawInfo(ECS::Entity weaponMini);
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
	
	WeaponInfo weaponInfo;
	
}; 

struct WeaponNormal {
	raylib::Texture2DUnmanaged bulletSprite;//1
	ID_WEAPON id;//2
};

struct WeaponType {
	ID_WEAPON_TYPE id;//1
};

struct Inventory;


//weapons derived from this must be named "Weapon_NAME" where NAME exactly matches name from ID_WEAPON::ID_NAME
template<typename T>
struct IWeapon {
	struct defaults {
		static void d_createMini() { assert(1 > 2 && "static virtual from IWeapon called"); }
		static void d_createNormal(Inventory& inv, WeaponMini& weaponMini) { assert(1 > 2 && "static virtual from IWeapon called"); }
		static void d_behaviourWeaponNormal(ECS::Entity weaponNormalEntity) { assert(1 > 2 && "static virtual from IWeapon called"); }
	};

	virtual_static_method(d_createMini, void(void), T, defaults);
	virtual_static_method(d_createNormal, void(Inventory&, WeaponMini&), T, defaults);
	virtual_static_method(d_behaviourWeaponNormal, void(ECS::Entity), T, defaults);

	static void createMini() {
		d_createMini();
	}
	static void createNormal(Inventory& inv, WeaponMini& weaponMini) {
		d_createNormal(inv, weaponMini);
	}
	static void behaviourNormal(ECS::Entity weaponNormalEntity) {
		d_behaviourWeaponNormal(weaponNormalEntity);
	}
	//lol
	static void entityAngleToPos(raylib::Vector2 const& entityPos, float& angle, raylib::Vector2 const& pos);

	static std::optional<raylib::Vector2> findEmptySlot(Inventory& inv, raylib::Texture2DUnmanaged const& sprite, ECS::Entity WeaponMini);
};

struct Weapon_CANON : IWeapon<Weapon_CANON> {
	static void d_createMini();
	static void d_createNormal(Inventory& inv, WeaponMini& weaponMini);
	static void d_behaviourWeaponNormal(ECS::Entity weaponNormalEntity);
};

struct Weapon_MINIGUN : IWeapon<Weapon_MINIGUN> {
	static void d_createMini();
	static void d_createNormal(Inventory& inv, WeaponMini& weaponMini);
	static void d_behaviourWeaponNormal(ECS::Entity weaponNormalEntity);
};

struct Weapon_LASERPISTOL : IWeapon<Weapon_LASERPISTOL> {
	static void d_createMini();
	static void d_createNormal(Inventory& inv, WeaponMini& weaponMini);
	static void d_behaviourWeaponNormal(ECS::Entity weaponNormalEntity);
};