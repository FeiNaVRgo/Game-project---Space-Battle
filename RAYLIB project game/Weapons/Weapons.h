#pragma once
#include <src/raylib-cpp.hpp>
#include <magic_enum_all.hpp>

#include <unordered_map>
#include <memory>
#include <optional>

#include "../FunctionalBox.h"
#include "../ECS.h"
#include "utils-virtual-static-method.h"
#include "WeaponEnumeraions.h"

struct Inventory;

class WeaponInfo {
private:
	FunctionalBox box_name;
	FunctionalBox box_stats;
	FunctionalBox box_description;
	FunctionalBox box_rarity;
public:
	WeaponInfo(ECS::Entity weaponMini);

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
	
	//WeaponInfo weaponInfo{};
	
}; 

struct WeaponNormal {
	raylib::Texture2DUnmanaged bulletSprite;//1
	ID_WEAPON id;//2
};

struct WeaponType {
	ID_WEAPON_TYPE id;//1
};