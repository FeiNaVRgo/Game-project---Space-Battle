#pragma once
#include "ICreation.h"


template<size_t N>
struct StringLiteral {
	constexpr StringLiteral(const char(&str)[N]) {
		std::copy_n(str, N, value);
	}

	char value[N];
};

template
<
	class T,
	StringLiteral name,
	StringLiteral desc,
	ID_WEAPON id_weapon,
	ID_WEAPON_RARITY id_rarity,
	raylib::Texture2DUnmanaged const& texture_mini,
	raylib::Texture2DUnmanaged const& texture_normal,
	raylib::Texture2DUnmanaged const& bullet_sprite,
	Damage damage
>
class BasicCreation : public ICreation {//less go
public:
	static inline ID_WEAPON_RARITY getIdWeaponRarity() {
		return id_rarity;
	}

	inline void createMini() override {

		auto const& mini_weapon_entity = G::gCoordinator.CreateEntity();
		auto& inventory = G::gCoordinator.GetComponent<Inventory>(G::player);

		G::gCoordinator.AddComponent<WeaponType>(mini_weapon_entity, WeaponType{
			.id = ID_WEAPON_TYPE::MINI
			});
		G::gCoordinator.AddComponent<WeaponMini>(mini_weapon_entity, WeaponMini{
			.isHeld = false,
			.isEquipped = false,
			.rarity = id_rarity,
			.name = name.value,
			.description = desc.value,
			.id = id_weapon
			});
		G::gCoordinator.AddComponent<Sprite>(mini_weapon_entity, Sprite{
			.sprite = texture_mini,
			.angle = 0.0f,
			.tint {255, 255, 255, 255},
			.origin = raylib::Vector2(texture_mini.GetSize()) * 0.5f,
			.isDependent = true
			});
		G::gCoordinator.AddComponent<EntitySpecific>(mini_weapon_entity, EntitySpecific{
			.id = ID_ENTITY::WEAPON_ID
			});

		auto p_pos = findEmptySlot(inventory, texture_mini, mini_weapon_entity);
		if (!p_pos.has_value()) {
			G::gEntitySetToBeDestroyed.insert(mini_weapon_entity);
			return;
		}

		G::gCoordinator.AddComponent<Transforms>(mini_weapon_entity, Transforms{
			.position = p_pos.value(),
			.rotation = {0.0f, 0.0f},
			.scale = {1.0f, 1.0f}
			});
		auto& wp = G::gCoordinator.GetComponent<WeaponMini>(mini_weapon_entity);
		wp.posToStay = p_pos.value();
	}

	inline void createNormal(Inventory& inv, WeaponMini& weaponMini) override {
		auto const& canonNormal = G::gCoordinator.CreateEntity();

		G::gCoordinator.AddComponent<WeaponType>(canonNormal, WeaponType{
			.id = ID_WEAPON_TYPE::NORMAL
			});
		G::gCoordinator.AddComponent<Sprite>(canonNormal, Sprite{
			.sprite = texture_normal,
			.angle = 0.0f,
			.tint {255, 255, 255, 255},
			.origin = raylib::Vector2(texture_normal.GetSize()) * 0.5f,
			.isDependent = false
			});
		G::gCoordinator.AddComponent<Transforms>(canonNormal, Transforms{
			.position = G::gPlayerPos,
			.rotation {0.0f, 0.0f},
			.scale {1.0f, 1.0f}
			});
		G::gCoordinator.AddComponent<Damage>(canonNormal, Damage{
			damage
			});
		G::gCoordinator.AddComponent<WeaponNormal>(canonNormal, WeaponNormal{
			.bulletSprite = bullet_sprite,
			.id = id_weapon
			});
		G::gCoordinator.AddComponent<EntitySpecific>(canonNormal, EntitySpecific{
			.id = ID_ENTITY::WEAPON_ID
			});
		G::gCoordinator.AddComponent<TimerComponent>(canonNormal, TimerComponent{});

		weaponMini.ptrWeaponNormal = std::make_shared<ECS::Entity>(canonNormal);
	}
};