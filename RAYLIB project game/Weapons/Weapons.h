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
};

struct WeaponNormal {
	ECS::Entity miniImpl;
	raylib::Texture2DUnmanaged bulletSprite;
};

struct WeaponSystem {
	using WeaponmMini = ECS::Entity;
	
	static inline void createWeaponNormalCanon(WeaponMini mini) {
		
	}

	static inline void createWeaponMiniCanon() {
		auto& canonMini = G::gCoordinator.CreateEntity();
		auto& inventory = G::gCoordinator.GetComponent<Inventory>(G::player);

		G::gCoordinator.AddComponent<WeaponMini>(canonMini, WeaponMini{
			.isHeld = false,
			.isEquipped = false,
			.rarity = ID_WEAPON_RARITY::COMMON,
			.name = "OMNIPOTENT CANON",
			.description = "This canon transcendents all of universe"
			});
		G::gCoordinator.AddComponent<Sprite>(canonMini, Sprite{
			.sprite = G::weapon_mini_canon,
			.angle = 0.0f,
			.tint {255, 255, 255, 255},
			.origin = raylib::Vector2(G::weapon_mini_canon.width * 0.5f, G::weapon_mini_canon.height * 0.5f)
			});
		//TODO : position will be determinet form inventory slot
		//G::gCoordinator.AddComponent<Transforms>(canonMini, Transforms{
			//.position = {}
			//});

	}
};